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

#include "../../gui.h"

namespace reone {

namespace gui {

class Button;

}

namespace game {

class MessagesMenu : public GameGUI {
public:
    MessagesMenu(Game &game, ServicesView &services) :
        GameGUI(game, services) {
        _resRef = guiResRef("messages");
    }

    void onGUILoaded() override;

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnExit;

        // KotOR only
        std::shared_ptr<gui::Button> btnShow;
        // END KotOR only
    } _binding;

    void bindControls();
};

} // namespace game

} // namespace reone
