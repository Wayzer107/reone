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

#pragma once

#include "reone/scene/graph.h"
#include "reone/scene/node/model.h"

#include "../character.h"
#include "../gui.h"

#include "chargen/abilities.h"
#include "chargen/classselect.h"
#include "chargen/custom.h"
#include "chargen/feats.h"
#include "chargen/levelup.h"
#include "chargen/nameentry.h"
#include "chargen/portraitselect.h"
#include "chargen/quick.h"
#include "chargen/quickorcustom.h"
#include "chargen/skills.h"

namespace reone {

namespace gui {

class Label;

}

namespace game {

enum class CharGenScreen {
    ClassSelection,
    QuickOrCustom,
    Quick,
    PortraitSelection,
    Name,
    Custom,
    Abilities,
    Skills,
    Feats,
    LevelUp
};

class CharacterGeneration : public GameGUI {
public:
    enum class Type {
        Quick,
        Custom,
        LevelUp
    };

    CharacterGeneration(Game &game, ServicesView &services);

    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void draw() override;

    void startQuick();
    void startCustom();
    void startLevelUp();

    void finish();
    void cancel();

    void openClassSelection();
    void openNameEntry();
    void openAbilities();
    void openSkills();
    void openFeats();
    void openPortraitSelection();
    void openQuick();
    void openQuickOrCustom();
    void openCustom();
    void openSteps();
    void openLevelUp();

    void goToNextStep();

    const Character &character() const { return _character; }

    void setCharacter(Character character);

    const std::string &musicResRef() const {
        return _musicResRef;
    }

    const std::string &loadScreenResRef() const {
        return _loadScreenResRef;
    }

private:
    struct Binding {
        std::shared_ptr<gui::Label> defArrowLbl;
        std::shared_ptr<gui::Label> fortArrowLbl;
        std::shared_ptr<gui::Label> lblClass;
        std::shared_ptr<gui::Label> lblDef;
        std::shared_ptr<gui::Label> lblLevel;
        std::shared_ptr<gui::Label> lblLevelVal;
        std::shared_ptr<gui::Label> lblName;
        std::shared_ptr<gui::Label> lblVit;
        std::shared_ptr<gui::Label> modelLbl;
        std::shared_ptr<gui::Label> newLbl;
        std::shared_ptr<gui::Label> oldLbl;
        std::shared_ptr<gui::Label> portraitLbl;
        std::shared_ptr<gui::Label> reflArrowLbl;
        std::shared_ptr<gui::Label> vitArrowLbl;
        std::shared_ptr<gui::Label> willArrowLbl;

        std::shared_ptr<gui::Label> strAbLbl;
        std::shared_ptr<gui::Label> dexAbLbl;
        std::shared_ptr<gui::Label> conAbLbl;
        std::shared_ptr<gui::Label> intAbLbl;
        std::shared_ptr<gui::Label> wisAbLbl;
        std::shared_ptr<gui::Label> chaAbLbl;

        // KotOR only

        std::shared_ptr<gui::Label> oldFortLbl;
        std::shared_ptr<gui::Label> oldReflLbl;
        std::shared_ptr<gui::Label> oldWillLbl;

        // END KotOR only

        // TSL only

        std::shared_ptr<gui::Label> newFortLbl;
        std::shared_ptr<gui::Label> newReflLbl;
        std::shared_ptr<gui::Label> newWillLbl;

        // END TSL only
    } _binding;

    std::string _musicResRef;
    std::string _loadScreenResRef;

    CharGenScreen _screen {CharGenScreen::ClassSelection};
    Type _type {Type::Quick};
    Character _character;

    // Sub GUI

    std::unique_ptr<ClassSelection> _classSelection;
    std::unique_ptr<QuickOrCustom> _quickOrCustom;
    std::unique_ptr<QuickCharacterGeneration> _quick;
    std::unique_ptr<CustomCharacterGeneration> _custom;
    std::unique_ptr<PortraitSelection> _portraitSelection;
    std::unique_ptr<CharGenAbilities> _abilities;
    std::unique_ptr<CharGenSkills> _charGenSkills;
    std::unique_ptr<CharGenFeats> _charGenFeats;
    std::unique_ptr<NameEntry> _nameEntry;
    std::unique_ptr<LevelUpMenu> _levelUp;

    // END Sub GUI

    void onGUILoaded() override;

    void bindControls();
    void reloadCharacterModel();
    void updateAttributes();
    void changeScreen(CharGenScreen screen);

    GameGUI *getSubGUI() const;
    std::shared_ptr<scene::ModelSceneNode> getCharacterModel(scene::ISceneGraph &sceneGraph);

    void setAttributesVisible(bool visible);

    // Loading

    void loadClassSelection();
    void loadQuickOrCustom();
    void loadQuick();
    void loadPortraitSelection();
    void loadNameEntry();
    void loadCustom();
    void loadAbilities();
    void loadSkills();
    void loadFeats();
    void loadLevelUp();

    // END Loading
};

} // namespace game

} // namespace reone
