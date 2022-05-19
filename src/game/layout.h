/*
 * Copyright (c) 2020-2022 The reone project contributors
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

namespace reone {

namespace game {

struct Layout {
    struct Room {
        std::string name;
        glm::vec3 position {0.0f};
    };

    std::vector<Room> rooms;

    const Room *findByName(const std::string &name) const {
        for (auto &room : rooms) {
            if (room.name == name) {
                return &room;
            }
        }
        return nullptr;
    }
};

} // namespace game

} // namespace reone
