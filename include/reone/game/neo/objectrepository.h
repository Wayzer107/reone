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

#include "object.h"

namespace reone {

namespace game {

namespace neo {

class IEventCollector;

class Area;
class Camera;
class Creature;
class Door;
class Encounter;
class Item;
class Module;
class Placeable;
class Sound;
class Store;
class Trigger;
class Waypoint;

class IObjectRepository {
public:
    virtual ~IObjectRepository() = default;

    virtual Area &newArea(ObjectTag tag) = 0;
    virtual Camera &newCamera(ObjectTag tag) = 0;
    virtual Creature &newCreature(ObjectTag tag) = 0;
    virtual Door &newDoor(ObjectTag tag) = 0;
    virtual Encounter &newEncounter(ObjectTag tag) = 0;
    virtual Item &newItem(ObjectTag tag) = 0;
    virtual Module &newModule(ObjectTag tag) = 0;
    virtual Placeable &newPlaceable(ObjectTag tag) = 0;
    virtual Sound &newSound(ObjectTag tag) = 0;
    virtual Store &newStore(ObjectTag tag) = 0;
    virtual Trigger &newTrigger(ObjectTag tag) = 0;
    virtual Waypoint &newWaypoint(ObjectTag tag) = 0;

    virtual Object &get(ObjectId objectId) = 0;
    virtual std::optional<std::reference_wrapper<Object>> find(ObjectId objectId) = 0;
};

class ObjectRepository : public IObjectRepository, boost::noncopyable {
public:
    ObjectRepository(IEventCollector &eventCollector) :
        _eventCollector(eventCollector) {
    }

    Area &newArea(ObjectTag tag) override;
    Camera &newCamera(ObjectTag tag) override;
    Creature &newCreature(ObjectTag tag) override;
    Door &newDoor(ObjectTag tag) override;
    Encounter &newEncounter(ObjectTag tag) override;
    Item &newItem(ObjectTag tag) override;
    Module &newModule(ObjectTag tag) override;
    Placeable &newPlaceable(ObjectTag tag) override;
    Sound &newSound(ObjectTag tag) override;
    Store &newStore(ObjectTag tag) override;
    Trigger &newTrigger(ObjectTag tag) override;
    Waypoint &newWaypoint(ObjectTag tag) override;

    Object &get(ObjectId objectId) override {
        return find(objectId).value();
    }

    std::optional<std::reference_wrapper<Object>> find(ObjectId objectId) override {
        auto it = _idToObject.find(objectId);
        if (it == _idToObject.end()) {
            return std::nullopt;
        }
        return it->second;
    }

private:
    IEventCollector &_eventCollector;

    ObjectId _nextObjectId {2};
    std::list<std::unique_ptr<Object>> _objects;
    std::map<ObjectId, std::reference_wrapper<Object>> _idToObject;

    template <class O, class... Args>
    inline O &newObject(ObjectTag tag, Args &&...args) {
        auto object = std::make_unique<O>(
            _nextObjectId++,
            std::move(tag),
            std::forward<Args>(args)...);
        _objects.push_back(std::move(object));
        auto &inserted = *_objects.back();
        _idToObject.insert({inserted.id(), inserted});
        return static_cast<O &>(inserted);
    }
};

} // namespace neo

} // namespace game

} // namespace reone
