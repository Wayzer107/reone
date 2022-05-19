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

#include "surfaces.h"

#include "../../resource/2da.h"
#include "../../resource/2das.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace game {

void Surfaces::init() {
    shared_ptr<TwoDa> surfacemat(_twoDas.get("surfacemat"));
    if (!surfacemat) {
        return;
    }
    for (int row = 0; row < surfacemat->getRowCount(); ++row) {
        Surface surface;
        surface.label = surfacemat->getString(row, "label");
        surface.walk = surfacemat->getBool(row, "walk");
        surface.walkcheck = surfacemat->getBool(row, "walkcheck");
        surface.lineOfSight = surfacemat->getBool(row, "lineofsight");
        surface.grass = surfacemat->getBool(row, "grass");
        surface.sound = surfacemat->getString(row, "sound");
        _surfaces.push_back(move(surface));
    }
}

bool Surfaces::isWalkable(int index) const {
    if (index < 0 || index >= static_cast<int>(_surfaces.size())) {
        throw out_of_range("index is out of range");
    }
    return _surfaces[index].walk;
}

const Surface &Surfaces::getSurface(int index) const {
    if (index < 0 || index >= static_cast<int>(_surfaces.size())) {
        throw out_of_range("index is out of range: " + to_string(index));
    }
    return _surfaces[index];
}

set<uint32_t> Surfaces::getSurfaceIndices(const function<bool(const Surface &)> &pred) const {
    set<uint32_t> result;
    for (size_t i = 0; i < _surfaces.size(); ++i) {
        if (pred(_surfaces[i])) {
            result.insert(static_cast<uint32_t>(i));
        }
    }
    return move(result);
}

set<uint32_t> Surfaces::getGrassSurfaces() const {
    return getSurfaceIndices([](auto &surface) { return surface.grass; });
}

set<uint32_t> Surfaces::getWalkableSurfaces() const {
    return getSurfaceIndices([](auto &surface) { return surface.walk; });
}

set<uint32_t> Surfaces::getWalkcheckSurfaces() const {
    return getSurfaceIndices([](auto &surface) { return surface.walkcheck; });
}

set<uint32_t> Surfaces::getLineOfSightSurfaces() const {
    return getSurfaceIndices([](auto &surface) { return surface.lineOfSight; });
}

} // namespace game

} // namespace reone
