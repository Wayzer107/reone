/*
 * Copyright (c) 2020-2023 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "explorerframe.h"

#include <wx/dirdlg.h>
#include <wx/mstream.h>

#include "reone/audio/clip.h"
#include "reone/audio/format/mp3reader.h"
#include "reone/audio/format/wavreader.h"
#include "reone/game/script/routines.h"
#include "reone/graphics/format/lipreader.h"
#include "reone/graphics/lipanimation.h"
#include "reone/resource/format/2dareader.h"
#include "reone/resource/format/bifreader.h"
#include "reone/resource/format/erfreader.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/format/rimreader.h"
#include "reone/resource/format/ssfreader.h"
#include "reone/resource/format/tlkreader.h"
#include "reone/resource/talktable.h"
#include "reone/resource/typeutil.h"
#include "reone/system/fileutil.h"
#include "reone/system/hexutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stream/memoryoutput.h"
#include "reone/tools/legacy/2da.h"
#include "reone/tools/legacy/audio.h"
#include "reone/tools/legacy/erf.h"
#include "reone/tools/legacy/gff.h"
#include "reone/tools/legacy/keybif.h"
#include "reone/tools/legacy/lip.h"
#include "reone/tools/legacy/ncs.h"
#include "reone/tools/legacy/rim.h"
#include "reone/tools/legacy/ssf.h"
#include "reone/tools/legacy/tlk.h"
#include "reone/tools/legacy/tpc.h"

#include "../../viewmodel/resource/gff.h"
#include "../../viewmodel/resource/ncs.h"
#include "../../viewmodel/resource/nss.h"
#include "../../viewmodel/resource/table.h"
#include "../../viewmodel/resource/text.h"

#include "../composelipdialog.h"

#include "audiopanel.h"
#include "gffpanel.h"
#include "imagepanel.h"
#include "modelpanel.h"
#include "ncspanel.h"
#include "nsspanel.h"
#include "tablepanel.h"
#include "textpanel.h"

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

static constexpr char kIconName[] = "toolkit";

static const std::set<std::string> kFilesArchiveExtensions {".bif", ".erf", ".sav", ".rim", ".mod"};

static const std::set<PageType> kStaticPageTypes {
    PageType::Image,
    PageType::Model,
    PageType::Audio};

struct EventHandlerID {
    static constexpr int openGameDir = wxID_HIGHEST + 1;
    static constexpr int extractAllBifs = wxID_HIGHEST + 2;
    static constexpr int batchTpcToTga = wxID_HIGHEST + 3;
    static constexpr int composeLip = wxID_HIGHEST + 4;
    static constexpr int extractTool = wxID_HIGHEST + 5;
    static constexpr int unwrapTool = wxID_HIGHEST + 6;
    static constexpr int toRimTool = wxID_HIGHEST + 7;
    static constexpr int toErfTool = wxID_HIGHEST + 8;
    static constexpr int toModTool = wxID_HIGHEST + 9;
    static constexpr int toXmlTool = wxID_HIGHEST + 10;
    static constexpr int toTwoDaTool = wxID_HIGHEST + 11;
    static constexpr int toGffTool = wxID_HIGHEST + 12;
    static constexpr int toTlkTool = wxID_HIGHEST + 13;
    static constexpr int toLipTool = wxID_HIGHEST + 14;
    static constexpr int toSsfTool = wxID_HIGHEST + 15;
    static constexpr int toTgaTool = wxID_HIGHEST + 16;
    static constexpr int toPcodeTool = wxID_HIGHEST + 17;
    static constexpr int toNcsTool = wxID_HIGHEST + 18;
    static constexpr int toNssTool = wxID_HIGHEST + 19;
    static constexpr int saveFile = wxID_HIGHEST + 20;
};

struct CommandID {
    static constexpr int extract = 1;
    static constexpr int decompile = 2;
    static constexpr int decompileNoOptimize = 3;
    static constexpr int exportFile = 4;
};

struct TimerID {
    static constexpr int render = 1;
    static constexpr int audio = 2;
};

ResourceExplorerFrame::ResourceExplorerFrame() :
    wxFrame(nullptr, wxID_ANY, "reone toolkit", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE) {
    _viewModel = std::make_unique<ResourceExplorerViewModel>();

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif
    SetMinClientSize(wxSize(1024, 768));
    Maximize();

    auto fileMenu = new wxMenu();
    fileMenu->Append(EventHandlerID::openGameDir, "&Open game directory...");
    fileMenu->AppendSeparator();
    _saveFileMenuItem = fileMenu->Append(EventHandlerID::saveFile, "&Save");
    _saveFileMenuItem->Enable(false);
    auto toolsMenu = new wxMenu();
    toolsMenu->Append(EventHandlerID::extractAllBifs, "Extract all BIF archives...");
    toolsMenu->Append(EventHandlerID::batchTpcToTga, "Batch convert TPC to TGA/TXI...");
    toolsMenu->Append(EventHandlerID::composeLip, "Compose LIP...");
    toolsMenu->AppendSeparator();
    toolsMenu->Append(EventHandlerID::extractTool, "Extract BIF/RIM/ERF archive...");
    toolsMenu->Append(EventHandlerID::unwrapTool, "Unwrap WAV to WAV/MP3...");
    toolsMenu->Append(EventHandlerID::toRimTool, "Create RIM from directory...");
    toolsMenu->Append(EventHandlerID::toErfTool, "Create ERF from directory...");
    toolsMenu->Append(EventHandlerID::toModTool, "Create MOD from directory...");
    toolsMenu->Append(EventHandlerID::toXmlTool, "Convert 2DA/GFF/TLK/LIP/SSF to XML...");
    toolsMenu->Append(EventHandlerID::toTwoDaTool, "Convert XML to 2DA...");
    toolsMenu->Append(EventHandlerID::toGffTool, "Convert XML to GFF...");
    toolsMenu->Append(EventHandlerID::toTlkTool, "Convert XML to TLK...");
    toolsMenu->Append(EventHandlerID::toLipTool, "Convert XML to LIP...");
    toolsMenu->Append(EventHandlerID::toSsfTool, "Convert XML to SSF...");
    toolsMenu->Append(EventHandlerID::toTgaTool, "Convert TPC to TGA/TXI...");
    toolsMenu->Append(EventHandlerID::toPcodeTool, "Disassemble NCS to PCODE...");
    toolsMenu->Append(EventHandlerID::toNcsTool, "Assemble NCS from PCODE...");
    toolsMenu->Append(EventHandlerID::toNssTool, "Decompile NCS (experimental)...");
    auto menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(toolsMenu, "&Tools");
    SetMenuBar(menuBar);

    _splitter = new wxSplitterWindow(this, wxID_ANY);
    _splitter->SetMinimumPaneSize(300);

    auto filesPanel = new wxPanel(_splitter);
    _filesTreeCtrl = new wxDataViewTreeCtrl(filesPanel, wxID_ANY);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING, &ResourceExplorerFrame::OnFilesTreeCtrlItemExpanding, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &ResourceExplorerFrame::OnFilesTreeCtrlItemContextMenu, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &ResourceExplorerFrame::OnFilesTreeCtrlItemActivated, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &ResourceExplorerFrame::OnFilesTreeCtrlItemStartEditing, this);
    auto filesSizer = new wxStaticBoxSizer(wxVERTICAL, filesPanel, "Game Directory");
    filesSizer->Add(_filesTreeCtrl, 1, wxEXPAND);
    filesPanel->SetSizer(filesSizer);

    _notebook = new wxAuiNotebook(_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE & ~(wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE));
    _notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &ResourceExplorerFrame::OnNotebookPageClose, this);
    _notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &ResourceExplorerFrame::OnNotebookPageChanged, this);

    _imagePanel = new ImageResourcePanel(_viewModel->imageResViewModel(), _notebook);
    _modelPanel = new ModelResourcePanel(_notebook);
    _audioPanel = new AudioResourcePanel(_notebook);

    _splitter->SplitVertically(filesPanel, _notebook, 1);

    for (auto &page : kStaticPageTypes) {
        auto window = GetStaticPageWindow(page);
        window->Hide();
    }

    _viewModel->pages().addChangedHandler([this](const auto &args) {
        switch (args.type) {
        case CollectionChangeType::Add: {
            auto &page = args.addedItem->get();
            wxWindow *window;
            if (kStaticPageTypes.count(page->type) > 0) {
                window = GetStaticPageWindow(page->type);
            } else {
                window = NewPageWindow(*page);
            }
            window->Show();
            _notebook->AddPage(window, page->displayName, true);
        } break;
        case CollectionChangeType::Remove: {
            auto &page = args.removedItem->get();
            if (kStaticPageTypes.count(page->type) > 0) {
                auto window = GetStaticPageWindow(page->type);
                window->Hide();
                _notebook->RemovePage(*args.removedItemIdx);
            } else {
                _notebook->DeletePage(*args.removedItemIdx);
            }
        } break;
        default:
            throw std::logic_error("Unsupported collection state change type");
        }
    });
    _viewModel->selectedPage().addChangedHandler([this](const auto &page) {
        _notebook->SetSelection(page);
    });
    _viewModel->progress().addChangedHandler([this](const auto &progress) {
        if (progress.visible) {
            if (!_progressDialog) {
                _progressDialog = new wxProgressDialog("", "", 100, this);
            }
            _progressDialog->SetTitle(progress.title);
            _progressDialog->Update(progress.value, progress.message);
        } else {
            if (_progressDialog) {
                _progressDialog->Destroy();
                _progressDialog = nullptr;
            }
        }
    });
    _viewModel->engineLoadRequested().addChangedHandler([this](const auto &requested) {
        if (!requested) {
            return;
        }
        _modelPanel->SetViewModel(_viewModel->modelResViewModel());
        _modelPanel->OnEngineLoadRequested();
        _audioPanel->SetViewModel(_viewModel->audioResViewModel());
        _audioPanel->OnEngineLoadRequested();
    });
    _viewModel->renderEnabled().addChangedHandler([this](const auto &enabled) {
        if (enabled) {
            wxWakeUpIdle();
        }
    });
    _viewModel->onViewCreated();

    // CreateStatusBar();
}

void ResourceExplorerFrame::SaveFile() {
}

wxWindow *ResourceExplorerFrame::NewPageWindow(Page &page) {
    switch (page.type) {
    case PageType::Text:
        return new TextResourcePanel(*std::static_pointer_cast<TextResourceViewModel>(page.viewModel), _notebook);
    case PageType::Table:
        return new TableResourcePanel(*std::static_pointer_cast<TableResourceViewModel>(page.viewModel), _notebook);
    case PageType::GFF:
        return new GFFResourcePanel(*std::static_pointer_cast<GFFResourceViewModel>(page.viewModel), _notebook);
    case PageType::NCS:
        return new NCSResourcePanel(*std::static_pointer_cast<NCSResourceViewModel>(page.viewModel), _notebook);
    case PageType::NSS:
        return new NSSResourcePanel(*std::static_pointer_cast<NSSResourceViewModel>(page.viewModel), _notebook);
    default:
        throw std::invalid_argument("Invalid page type: " + std::to_string(static_cast<int>(page.type)));
    }
}

wxWindow *ResourceExplorerFrame::GetStaticPageWindow(PageType type) const {
    switch (type) {
    case PageType::Image:
        return _imagePanel;
    case PageType::Model:
        return _modelPanel;
    case PageType::Audio:
        return _audioPanel;
    default:
        return nullptr;
    }
}

void ResourceExplorerFrame::OnClose(wxCloseEvent &event) {
    Destroy();
    _viewModel->onViewDestroyed();
}

void ResourceExplorerFrame::OnIdle(wxIdleEvent &event) {
    bool renderEnabled = *_viewModel->renderEnabled();
    if (renderEnabled) {
        _viewModel->modelResViewModel().update3D();
        _modelPanel->RefreshGL();
    }
    bool hasAudio = _audioPanel->HasAudioSource();
    if (hasAudio) {
        _audioPanel->UpdateAudioSource();
    }
    if (renderEnabled || hasAudio) {
        event.RequestMore();
    }
}

void ResourceExplorerFrame::OnOpenGameDirectoryCommand(wxCommandEvent &event) {
    auto dialog = new wxDirDialog(nullptr, "Choose game directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dialog->ShowModal() != wxID_OK) {
        return;
    }
    auto gamePath = std::filesystem::path((std::string)dialog->GetPath());
    auto keyPath = findFileIgnoreCase(gamePath, "chitin.key");
    auto modulesPath = findFileIgnoreCase(gamePath, "modules");
    if (!keyPath || !modulesPath) {
        wxMessageBox("Not a valid game directory", "Error", wxICON_ERROR);
        return;
    }
    _viewModel->onGameDirectoryChanged(gamePath);

    auto key = FileInputStream(*keyPath);
    auto keyReader = KeyReader(key);
    keyReader.load();
    _keyKeys = keyReader.keys();
    _keyFiles = keyReader.files();

    _filesTreeCtrl->Freeze();
    _filesTreeCtrl->DeleteAllItems();
    int numGameDirItems = _viewModel->getGameDirItemCount();
    for (int i = 0; i < numGameDirItems; ++i) {
        auto &item = _viewModel->getGameDirItem(i);
        void *itemId;
        if (item.container) {
            auto treeItem = _filesTreeCtrl->AppendContainer(wxDataViewItem(), item.displayName);
            itemId = treeItem.GetID();
        } else {
            auto treeItem = _filesTreeCtrl->AppendItem(wxDataViewItem(), item.displayName);
            itemId = treeItem.GetID();
        }
        _viewModel->onGameDirectoryItemIdentified(i, itemId);
    }
    _filesTreeCtrl->Thaw();
}

void ResourceExplorerFrame::OnSaveFileCommand(wxCommandEvent &event) {
    SaveFile();
}

void ResourceExplorerFrame::OnFilesTreeCtrlItemExpanding(wxDataViewEvent &event) {
    auto expandingItemId = event.GetItem().GetID();
    auto &expandingItem = _viewModel->getGameDirItemById(expandingItemId);
    if (expandingItem.loaded) {
        return;
    }
    _viewModel->onGameDirectoryItemExpanding(expandingItemId);
    _filesTreeCtrl->Freeze();
    int numGameDirItems = _viewModel->getGameDirItemCount();
    for (int i = 0; i < numGameDirItems; ++i) {
        auto &item = _viewModel->getGameDirItem(i);
        if (item.id || item.parentId != expandingItemId) {
            continue;
        }
        void *itemId;
        if (item.container) {
            auto treeItem = _filesTreeCtrl->AppendContainer(wxDataViewItem(expandingItemId), item.displayName);
            itemId = treeItem.GetID();
        } else {
            auto treeItem = _filesTreeCtrl->AppendItem(wxDataViewItem(expandingItemId), item.displayName);
            itemId = treeItem.GetID();
        }
        _viewModel->onGameDirectoryItemIdentified(i, itemId);
    }
    _filesTreeCtrl->Thaw();
    expandingItem.loaded = true;
}

void ResourceExplorerFrame::OnFilesTreeCtrlItemActivated(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    _viewModel->onGameDirectoryItemActivated(itemId);
}

void ResourceExplorerFrame::OnFilesTreeCtrlItemContextMenu(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    auto &item = _viewModel->getGameDirItemById(itemId);
    if (item.resId) {
        auto menu = wxMenu();
        menu.Append(CommandID::exportFile, "Export...");
        if (item.resId->type == ResType::Ncs) {
            menu.Append(CommandID::decompile, "Decompile");
            menu.Append(CommandID::decompileNoOptimize, "Decompile without optimization");
        }
        menu.SetClientData(itemId);
        menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ResourceExplorerFrame::OnPopupCommandSelected), nullptr, this);
        PopupMenu(&menu, event.GetPosition());
    } else {
        if (item.archived || !std::filesystem::is_regular_file(item.path)) {
            return;
        }
        auto extension = item.path.extension().string();
        if (kFilesArchiveExtensions.count(extension) == 0) {
            return;
        }
        auto menu = wxMenu();
        menu.Append(CommandID::extract, "Extract...");
        menu.SetClientData(itemId);
        menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ResourceExplorerFrame::OnPopupCommandSelected), nullptr, this);
        PopupMenu(&menu, event.GetPosition());
    }
}

void ResourceExplorerFrame::OnFilesTreeCtrlItemStartEditing(wxDataViewEvent &event) {
    event.Veto();
}

void ResourceExplorerFrame::OnNotebookPageClose(wxAuiNotebookEvent &event) {
    int pageIdx = event.GetSelection();
    _viewModel->onNotebookPageClose(pageIdx);
    event.Veto();
}

void ResourceExplorerFrame::OnNotebookPageChanged(wxAuiNotebookEvent &event) {
    int pageIdx = event.GetSelection();
    if (pageIdx == -1) {
        return;
    }
    auto &page = _viewModel->getPage(pageIdx);
    if (page.dirty) {
        _saveFileMenuItem->Enable(true);
    } else {
        _saveFileMenuItem->Enable(false);
    }
    event.Skip();
}

void ResourceExplorerFrame::OnPopupCommandSelected(wxCommandEvent &event) {
    auto menu = static_cast<wxMenu *>(event.GetEventObject());

    if (event.GetId() == CommandID::extract) {
        auto itemId = menu->GetClientData();
        auto &item = _viewModel->getGameDirItemById(itemId);

        auto dialog = new wxDirDialog(nullptr, "Choose extraction directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dialog->ShowModal() != wxID_OK) {
            return;
        }
        auto destPath = std::filesystem::path(std::string(dialog->GetPath()));

        _viewModel->extractArchive(item.path, destPath);
        wxMessageBox("Operation completed successfully", "Success");

    } else if (event.GetId() == CommandID::decompile) {
        auto itemId = menu->GetClientData();
        _viewModel->decompile(itemId, true);

    } else if (event.GetId() == CommandID::decompileNoOptimize) {
        auto itemId = menu->GetClientData();
        _viewModel->decompile(itemId, false);

    } else if (event.GetId() == CommandID::exportFile) {
        auto itemId = menu->GetClientData();
        auto dialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dialog->ShowModal() != wxID_OK) {
            return;
        }
        auto destPath = std::filesystem::path(std::string(dialog->GetPath()));
        _viewModel->exportFile(itemId, destPath);
        wxMessageBox("Operation completed successfully", "Success");
    }
}

void ResourceExplorerFrame::OnExtractAllBifsCommand(wxCommandEvent &event) {
    if (_viewModel->gamePath().empty()) {
        wxMessageBox("Game directory must be open", "Error", wxICON_ERROR);
        return;
    }
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = std::filesystem::path((std::string)destDirDialog->GetPath());
    _viewModel->extractAllBifs(destPath);
    wxMessageBox("Operation completed successfully", "Success");
}

void ResourceExplorerFrame::OnBatchConvertTpcToTgaCommand(wxCommandEvent &event) {
    auto srcDirDialog = new wxDirDialog(nullptr, "Choose source directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (srcDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto srcPath = std::filesystem::path((std::string)srcDirDialog->GetPath());
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = std::filesystem::path((std::string)destDirDialog->GetPath());
    _viewModel->batchConvertTpcToTga(srcPath, destPath);
    wxMessageBox("Operation completed successfully", "Success");
}

void ResourceExplorerFrame::OnComposeLipCommand(wxCommandEvent &event) {
    auto dialog = ComposeLipDialog(this, -1, "LIP Composer");
    if (dialog.ShowModal() != wxID_OK) {
    }
}

void ResourceExplorerFrame::OnExtractToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::Extract);
}

void ResourceExplorerFrame::OnUnwrapToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::Unwrap);
}

void ResourceExplorerFrame::OnToRimToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToRIM);
}

void ResourceExplorerFrame::OnToErfToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToERF);
}

void ResourceExplorerFrame::OnToModToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToMOD);
}

void ResourceExplorerFrame::OnToXmlToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToXML);
}

void ResourceExplorerFrame::OnToTwoDaToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::To2DA);
}

void ResourceExplorerFrame::OnToGffToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToGFF);
}

void ResourceExplorerFrame::OnToTlkToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToTLK);
}

void ResourceExplorerFrame::OnToLipToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToLIP);
}

void ResourceExplorerFrame::OnToSsfToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToSSF);
}

void ResourceExplorerFrame::OnToTgaToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToTGA);
}

void ResourceExplorerFrame::OnToPcodeToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToPCODE);
}

void ResourceExplorerFrame::OnToNcsToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToNCS);
}

void ResourceExplorerFrame::OnToNssToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToNSS);
}

void ResourceExplorerFrame::InvokeTool(Operation operation) {
    std::filesystem::path srcPath;
    switch (operation) {
    case Operation::ToERF:
    case Operation::ToRIM:
    case Operation::ToMOD: {
        auto srcDirDialog = new wxDirDialog(
            nullptr,
            "Choose source directory",
            "",
            wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (srcDirDialog->ShowModal() != wxID_OK) {
            return;
        }
        srcPath = (std::string)srcDirDialog->GetPath();
    } break;
    default: {
        auto srcFileDialog = new wxFileDialog(
            nullptr,
            "Choose source file",
            "",
            "",
            wxString::FromAscii(wxFileSelectorDefaultWildcardStr),
            wxFD_DEFAULT_STYLE | wxFD_FILE_MUST_EXIST);
        if (srcFileDialog->ShowModal() != wxID_OK) {
            return;
        }
        srcPath = (std::string)srcFileDialog->GetPath();
    } break;
    }
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = std::filesystem::path((std::string)destDirDialog->GetPath());
    if (_viewModel->invokeTool(operation, srcPath, destPath)) {
        wxMessageBox("Operation completed successfully", "Success");
    } else {
        wxMessageBox("Tool not found", "Error", wxICON_ERROR);
    }
}

wxBEGIN_EVENT_TABLE(ResourceExplorerFrame, wxFrame)
    EVT_CLOSE(ResourceExplorerFrame::OnClose)                                                     //
    EVT_IDLE(ResourceExplorerFrame::OnIdle)                                                       //
    EVT_MENU(EventHandlerID::openGameDir, ResourceExplorerFrame::OnOpenGameDirectoryCommand)      //
    EVT_MENU(EventHandlerID::saveFile, ResourceExplorerFrame::OnSaveFileCommand)                  //
    EVT_MENU(EventHandlerID::extractAllBifs, ResourceExplorerFrame::OnExtractAllBifsCommand)      //
    EVT_MENU(EventHandlerID::batchTpcToTga, ResourceExplorerFrame::OnBatchConvertTpcToTgaCommand) //
    EVT_MENU(EventHandlerID::composeLip, ResourceExplorerFrame::OnComposeLipCommand)              //
    EVT_MENU(EventHandlerID::extractTool, ResourceExplorerFrame::OnExtractToolCommand)            //
    EVT_MENU(EventHandlerID::unwrapTool, ResourceExplorerFrame::OnUnwrapToolCommand)              //
    EVT_MENU(EventHandlerID::toRimTool, ResourceExplorerFrame::OnToRimToolCommand)                //
    EVT_MENU(EventHandlerID::toErfTool, ResourceExplorerFrame::OnToErfToolCommand)                //
    EVT_MENU(EventHandlerID::toModTool, ResourceExplorerFrame::OnToModToolCommand)                //
    EVT_MENU(EventHandlerID::toXmlTool, ResourceExplorerFrame::OnToXmlToolCommand)                //
    EVT_MENU(EventHandlerID::toTwoDaTool, ResourceExplorerFrame::OnToTwoDaToolCommand)            //
    EVT_MENU(EventHandlerID::toGffTool, ResourceExplorerFrame::OnToGffToolCommand)                //
    EVT_MENU(EventHandlerID::toTlkTool, ResourceExplorerFrame::OnToTlkToolCommand)                //
    EVT_MENU(EventHandlerID::toLipTool, ResourceExplorerFrame::OnToLipToolCommand)                //
    EVT_MENU(EventHandlerID::toSsfTool, ResourceExplorerFrame::OnToSsfToolCommand)                //
    EVT_MENU(EventHandlerID::toTgaTool, ResourceExplorerFrame::OnToTgaToolCommand)                //
    EVT_MENU(EventHandlerID::toPcodeTool, ResourceExplorerFrame::OnToPcodeToolCommand)            //
    EVT_MENU(EventHandlerID::toNcsTool, ResourceExplorerFrame::OnToNcsToolCommand)                //
    EVT_MENU(EventHandlerID::toNssTool, ResourceExplorerFrame::OnToNssToolCommand)                //
    wxEND_EVENT_TABLE()

} // namespace reone
