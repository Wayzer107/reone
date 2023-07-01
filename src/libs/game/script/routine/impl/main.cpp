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

#include "reone/game/event.h"
#include "reone/game/game.h"
#include "reone/game/script/routine/argutil.h"
#include "reone/game/script/routine/context.h"
#include "reone/game/script/routine/objectutil.h"
#include "reone/game/script/routines.h"
#include "reone/script/executioncontext.h"
#include "reone/script/routine/exception/argument.h"
#include "reone/script/routine/exception/notimplemented.h"
#include "reone/script/variable.h"
#include "reone/system/logutil.h"
#include "reone/system/randomutil.h"

#define R_VOID script::VariableType::Void
#define R_INT script::VariableType::Int
#define R_FLOAT script::VariableType::Float
#define R_OBJECT script::VariableType::Object
#define R_STRING script::VariableType::String
#define R_EFFECT script::VariableType::Effect
#define R_EVENT script::VariableType::Event
#define R_LOCATION script::VariableType::Location
#define R_TALENT script::VariableType::Talent
#define R_VECTOR script::VariableType::Vector
#define R_ACTION script::VariableType::Action

#ifdef _DEBUG
static constexpr bool kShipBuild = false;
#else
static constexpr bool kShipBuild = true;
#endif

using namespace reone::script;
using namespace reone::scene;

namespace reone {

namespace game {

namespace routine {

static Variable Random(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nMaxInteger = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(random(0, nMaxInteger - 1));
}

static Variable PrintString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);

    // Transform

    // Execute
    info(sString);
    return Variable::ofNull();
}

static Variable PrintFloat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fFloat = getFloat(args, 0);
    auto nWidth = getIntOrElse(args, 1, 18);
    auto nDecimals = getIntOrElse(args, 2, 9);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PrintFloat");
}

static Variable FloatToString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fFloat = getFloat(args, 0);
    auto nWidth = getIntOrElse(args, 1, 18);
    auto nDecimals = getIntOrElse(args, 2, 9);

    // Transform

    // Execute
    return Variable::ofString(std::to_string(fFloat));
}

static Variable PrintInteger(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInteger = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PrintInteger");
}

static Variable PrintObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PrintObject");
}

static Variable AssignCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oActionSubject = getObject(args, 0, ctx);
    auto aActionToAssign = getAction(args, 1);

    // Transform

    // Execute
    auto commandAction = ctx.game.actionFactory().newDoCommand(std::move(aActionToAssign));
    oActionSubject->addAction(std::move(commandAction));
    return Variable::ofNull();
}

static Variable DelayCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fSeconds = getFloat(args, 0);
    auto aActionToDelay = getAction(args, 1);

    // Transform

    // Execute
    auto commandAction = ctx.game.actionFactory().newDoCommand(std::move(aActionToDelay));
    getCaller(ctx)->delayAction(std::move(commandAction), fSeconds);
    return Variable::ofNull();
}

static Variable ExecuteScript(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sScript = getString(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nScriptVar = getIntOrElse(args, 2, -1);

    // Transform

    // Execute
    ctx.game.scriptRunner().run(sScript, oTarget->id(), kObjectInvalid, kObjectInvalid, nScriptVar);
    return Variable::ofNull();
}

static Variable ClearAllActions(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    getCaller(ctx)->clearAllActions();
    return Variable::ofNull();
}

static Variable SetFacing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fDirection = getFloat(args, 0);

    // Transform

    // Execute
    auto caller = getCaller(ctx);
    caller->setFacing(glm::radians(fDirection));
    return Variable::ofNull();
}

static Variable SwitchPlayerCharacter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SwitchPlayerCharacter");
}

static Variable SetTime(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nHour = getInt(args, 0);
    auto nMinute = getInt(args, 1);
    auto nSecond = getInt(args, 2);
    auto nMillisecond = getInt(args, 3);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetTime");
}

static Variable SetPartyLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    ctx.game.party().setPartyLeader(nNPC);
    return Variable::ofNull();
}

static Variable SetAreaUnescapable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bUnescapable = getInt(args, 0);

    // Transform
    auto unescapable = static_cast<bool>(bUnescapable);

    // Execute
    ctx.game.module()->area()->setUnescapable(unescapable);
    return Variable::ofNull();
}

static Variable GetAreaUnescapable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    bool unescapable = ctx.game.module()->area()->isUnescapable();
    return Variable::ofInt(static_cast<int>(unescapable));
}

static Variable GetTimeHour(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTimeHour");
}

static Variable GetTimeMinute(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTimeMinute");
}

static Variable GetTimeSecond(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTimeSecond");
}

static Variable GetTimeMillisecond(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTimeMillisecond");
}

static Variable GetArea(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto area = ctx.game.module()->area();
    return Variable::ofObject(getObjectIdOrInvalid(area));
}

static Variable GetEnteringObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

static Variable GetExitingObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

static Variable GetPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofVector(oTarget->position());
}

static Variable GetFacing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    float facing = glm::degrees(oTarget->getFacing());
    return Variable::ofFloat(facing);
}

static Variable GetItemPossessor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetItemPossessor");
}

static Variable GetItemPossessedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto sItemTag = getString(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);
    auto itemTag = boost::to_lower_copy(sItemTag);

    // Execute
    if (itemTag.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }
    auto item = creature->getItemByTag(itemTag);
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable CreateItemOnObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sItemTemplate = getString(args, 0);
    auto oTarget = getObjectOrCaller(args, 1, ctx);
    auto nStackSize = getIntOrElse(args, 2, 1);
    auto nHideMessage = getIntOrElse(args, 3, 0);

    // Transform
    auto itemTemplate = boost::to_lower_copy(sItemTemplate);

    // Execute
    if (itemTemplate.empty()) {
        return Variable::ofObject(kObjectInvalid);
    }
    auto item = oTarget->addItem(itemTemplate, nStackSize, true);
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable GetLastAttacker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttackee = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttacker");
}

static Variable GetNearestCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFirstCriteriaType = getInt(args, 0);
    auto nFirstCriteriaValue = getInt(args, 1);
    auto oTarget = getObjectOrCaller(args, 2, ctx);
    auto nNth = getIntOrElse(args, 3, 1);
    auto nSecondCriteriaType = getIntOrElse(args, 4, -1);
    auto nSecondCriteriaValue = getIntOrElse(args, 5, -1);
    auto nThirdCriteriaType = getIntOrElse(args, 6, -1);
    auto nThirdCriteriaValue = getIntOrElse(args, 7, -1);

    // Transform
    auto firstCriteriaType = static_cast<CreatureType>(nFirstCriteriaType);
    auto secondCriteriaType = static_cast<CreatureType>(nSecondCriteriaType);
    auto thirdCriteriaType = static_cast<CreatureType>(nThirdCriteriaType);

    // Execute
    Area::SearchCriteriaList criterias;
    criterias.push_back(std::make_pair(firstCriteriaType, nFirstCriteriaValue));
    if (secondCriteriaType != CreatureType::Invalid) {
        criterias.push_back(std::make_pair(secondCriteriaType, nSecondCriteriaValue));
    }
    if (thirdCriteriaType != CreatureType::Invalid) {
        criterias.push_back(std::make_pair(thirdCriteriaType, nThirdCriteriaValue));
    }
    auto creature = ctx.game.module()->area()->getNearestCreature(oTarget, criterias, nNth - 1);
    return Variable::ofObject(getObjectIdOrInvalid(creature));
}

static Variable GetDistanceToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto caller = getCaller(ctx);
    return Variable::ofFloat(caller->getDistanceTo(*oObject));
}

static Variable GetIsObjectValid(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool valid;
    try {
        auto oObject = getObject(args, 0, ctx);
        valid = static_cast<bool>(oObject);
    } catch (const RoutineArgumentException &ignored) {
        valid = false;
    }
    return Variable::ofInt(static_cast<bool>(valid));
}

static Variable SetCameraFacing(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fDirection = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCameraFacing");
}

static Variable PlaySound(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sSoundName = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlaySound");
}

static Variable GetSpellTargetObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellTargetObject");
}

static Variable GetCurrentHitPoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    int hitPoints = oObject->currentHitPoints();
    return Variable::ofInt(hitPoints);
}

static Variable GetMaxHitPoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    int hitPoints = oObject->maxHitPoints();
    return Variable::ofInt(hitPoints);
}

static Variable GetLastItemEquipped(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastItemEquipped");
}

static Variable GetSubScreenID(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSubScreenID");
}

static Variable CancelCombat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("CancelCombat");
}

static Variable GetCurrentForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCurrentForcePoints");
}

static Variable GetMaxForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetMaxForcePoints");
}

static Variable PauseGame(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bPause = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PauseGame");
}

static Variable SetPlayerRestrictMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bRestrict = getInt(args, 0);

    // Transform
    auto restrict = static_cast<bool>(bRestrict);

    // Execute
    ctx.game.module()->player().setRestrictMode(restrict);
    return Variable::ofNull();
}

static Variable GetStringLength(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(sString.length()));
}

static Variable GetStringUpperCase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetStringUpperCase");
}

static Variable GetStringLowerCase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetStringLowerCase");
}

static Variable GetStringRight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto nCount = getInt(args, 1);

    // Transform

    // Execute
    std::string right;
    if (sString.size() >= nCount) {
        right = sString.substr(sString.length() - nCount, nCount);
    }
    return Variable::ofString(std::move(right));
}

static Variable GetStringLeft(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto nCount = getInt(args, 1);

    // Transform

    // Execute
    std::string left;
    if (sString.size() >= nCount) {
        left = sString.substr(0, nCount);
    }
    return Variable::ofString(std::move(left));
}

static Variable InsertString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sDestination = getString(args, 0);
    auto sString = getString(args, 1);
    auto nPosition = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("InsertString");
}

static Variable GetSubString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto nStart = getInt(args, 1);
    auto nCount = getInt(args, 2);

    // Transform

    // Execute
    return Variable::ofString(sString.substr(nStart, nStart));
}

static Variable FindSubString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto sSubString = getString(args, 1);

    // Transform

    // Execute
    size_t pos = sString.find(sSubString);
    return Variable::ofInt(pos != std::string::npos ? static_cast<int>(pos) : -1);
}

static Variable fabs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("fabs");
}

static Variable cos(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("cos");
}

static Variable sin(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("sin");
}

static Variable tan(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("tan");
}

static Variable acos(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("acos");
}

static Variable asin(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("asin");
}

static Variable atan(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("atan");
}

static Variable log(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("log");
}

static Variable pow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);
    auto fExponent = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("pow");
}

static Variable sqrt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fValue = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("sqrt");
}

static Variable abs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nValue = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(std::abs(nValue));
}

static Variable GetPlayerRestrictMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    bool restrict = ctx.game.module()->player().isRestrictMode();
    return Variable::ofInt(static_cast<int>(restrict));
}

static Variable GetCasterLevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCasterLevel");
}

static Variable GetFirstEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofEffect(creature->getFirstEffect());
}

static Variable GetNextEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofEffect(creature->getNextEffect());
}

static Variable RemoveEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto eEffect = getEffect(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveEffect");
}

static Variable GetIsEffectValid(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool valid;
    try {
        auto eEffect = getEffect(args, 0);
        valid = static_cast<bool>(eEffect);
    } catch (const RoutineArgumentException &ignored) {
        valid = false;
    }
    return Variable::ofInt(static_cast<int>(valid));
}

static Variable GetEffectDurationType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEffectDurationType");
}

static Variable GetEffectSubType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEffectSubType");
}

static Variable GetEffectCreator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEffectCreator");
}

static Variable IntToString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInteger = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofString(std::to_string(nInteger));
}

static Variable GetFirstObjectInArea(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObjectOrNull(args, 0, ctx);
    auto nObjectFilter = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstObjectInArea");
}

static Variable GetNextObjectInArea(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObjectOrNull(args, 0, ctx);
    auto nObjectFilter = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextObjectInArea");
}

static Variable d2(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::random(1, 2);
    }
    return Variable::ofInt(total);
}

static Variable d3(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::random(1, 3);
    }
    return Variable::ofInt(total);
}

static Variable d4(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::random(1, 4);
    }
    return Variable::ofInt(total);
}

static Variable d6(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::random(1, 6);
    }
    return Variable::ofInt(total);
}

static Variable d8(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::random(1, 8);
    }
    return Variable::ofInt(total);
}

static Variable d10(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::random(1, 10);
    }
    return Variable::ofInt(total);
}

static Variable d12(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::random(1, 12);
    }
    return Variable::ofInt(total);
}

static Variable d20(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::random(1, 20);
    }
    return Variable::ofInt(total);
}

static Variable d100(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumDice = getIntOrElse(args, 0, 1);

    // Transform
    auto numDice = std::max(1, nNumDice);

    // Execute
    int total = 0;
    for (int i = 0; i < numDice; ++i) {
        total += reone::random(1, 100);
    }
    return Variable::ofInt(total);
}

static Variable VectorMagnitude(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vVector = getVector(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VectorMagnitude");
}

static Variable GetMetaMagicFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetMetaMagicFeat");
}

static Variable GetObjectType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(oTarget->type()));
}

static Variable GetRacialType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->racialType()));
}

static Variable FortitudeSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nDC = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);
    auto oSaveVersus = getObjectOrCaller(args, 3, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("FortitudeSave");
}

static Variable ReflexSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nDC = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);
    auto oSaveVersus = getObjectOrCaller(args, 3, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ReflexSave");
}

static Variable WillSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nDC = getInt(args, 1);
    auto nSaveType = getIntOrElse(args, 2, 0);
    auto oSaveVersus = getObjectOrCaller(args, 3, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("WillSave");
}

static Variable GetSpellSaveDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellSaveDC");
}

static Variable MagicalEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MagicalEffect");
}

static Variable SupernaturalEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SupernaturalEffect");
}

static Variable ExtraordinaryEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ExtraordinaryEffect");
}

static Variable GetAC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nForFutureUse = getIntOrElse(args, 1, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAC");
}

static Variable RoundsToSeconds(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nRounds = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofFloat(nRounds / 6.0f);
}

static Variable HoursToSeconds(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nHours = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(nHours * 3600);
}

static Variable TurnsToSeconds(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nTurns = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TurnsToSeconds");
}

static Variable SoundObjectSetFixedVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto fFixedVariance = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectSetFixedVariance");
}

static Variable GetGoodEvilValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetGoodEvilValue");
}

static Variable GetPartyMemberCount(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofInt(ctx.game.party().getSize());
}

static Variable GetAlignmentGoodEvil(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAlignmentGoodEvil");
}

static Variable GetFirstObjectInShape(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nShape = getInt(args, 0);
    auto fSize = getFloat(args, 1);
    auto lTarget = getLocationArgument(args, 2);
    auto bLineOfSight = getIntOrElse(args, 3, 0);
    auto nObjectFilter = getIntOrElse(args, 4, 1);
    auto vOrigin = getVectorOrElse(args, 5, glm::vec3(0.0f, 0.0f, 0.0f));

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstObjectInShape");
}

static Variable GetNextObjectInShape(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nShape = getInt(args, 0);
    auto fSize = getFloat(args, 1);
    auto lTarget = getLocationArgument(args, 2);
    auto bLineOfSight = getIntOrElse(args, 3, 0);
    auto nObjectFilter = getIntOrElse(args, 4, 1);
    auto vOrigin = getVectorOrElse(args, 5, glm::vec3(0.0f, 0.0f, 0.0f));

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextObjectInShape");
}

static Variable SignalEvent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto evToRun = getEvent(args, 1);

    // Transform

    // Execute
    debug(boost::format("Event signalled: %s %s") % oObject->tag() % evToRun->number(), LogChannel::Script);
    ctx.game.scriptRunner().run(oObject->getOnUserDefined(), oObject->id(), kObjectInvalid, evToRun->number());
    return Variable::ofNull();
}

static Variable EventUserDefined(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nUserDefinedEventNumber = getInt(args, 0);

    // Transform

    // Execute
    auto event = std::make_shared<Event>(nUserDefinedEventNumber);
    return Variable::ofEvent(std::move(event));
}

static Variable VectorNormalize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vVector = getVector(args, 0);

    // Transform

    // Execute
    return Variable::ofVector(glm::normalize(vVector));
}

static Variable GetItemStackSize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    return Variable::ofInt(item->stackSize());
}

static Variable GetAbilityScore(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nAbilityType = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);
    auto ability = static_cast<Ability>(nAbilityType);

    // Execute
    return Variable::ofInt(creature->attributes().getAbilityScore(ability));
}

static Variable GetIsDead(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->isDead()));
}

static Variable PrintVector(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vVector = getVector(args, 0);
    auto bPrepend = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PrintVector");
}

static Variable Vector(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto x = getFloatOrElse(args, 0, 0.0f);
    auto y = getFloatOrElse(args, 1, 0.0f);
    auto z = getFloatOrElse(args, 2, 0.0f);

    // Transform

    // Execute
    return Variable::ofVector(glm::vec3(x, y, z));
}

static Variable SetFacingPoint(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vTarget = getVector(args, 0);

    // Transform

    // Execute
    auto caller = getCaller(ctx);
    caller->face(vTarget);
    return Variable::ofNull();
}

static Variable AngleToVector(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fAngle = getFloat(args, 0);

    // Transform

    // Execute
    auto vector = glm::vec3(glm::cos(fAngle), glm::sin(fAngle), 0.0f);
    return Variable::ofVector(std::move(vector));
}

static Variable VectorToAngle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vVector = getVector(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VectorToAngle");
}

static Variable TouchAttackMelee(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto bDisplayFeedback = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TouchAttackMelee");
}

static Variable TouchAttackRanged(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto bDisplayFeedback = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TouchAttackRanged");
}

static Variable SetItemStackSize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto nStackSize = getInt(args, 1);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    item->setStackSize(nStackSize);
    return Variable::ofNull();
}

static Variable GetDistanceBetween(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObjectA = getObject(args, 0, ctx);
    auto oObjectB = getObject(args, 1, ctx);

    // Transform

    // Execute
    return Variable::ofFloat(oObjectA->getDistanceTo(*oObjectB));
}

static Variable SetReturnStrref(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bShow = getInt(args, 0);
    auto srStringRef = getIntOrElse(args, 1, 0);
    auto srReturnQueryStrRef = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetReturnStrref");
}

static Variable GetItemInSlot(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInventorySlot = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    auto item = creature->getEquippedItem(nInventorySlot);
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable SetGlobalString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto sValue = getString(args, 1);

    // Transform

    // Execute
    ctx.game.setGlobalString(sIdentifier, sValue);
    return Variable::ofNull();
}

static Variable SetCommandable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bCommandable = getInt(args, 0);
    auto oTarget = getObjectOrCaller(args, 1, ctx);

    // Transform
    bool commandable = static_cast<bool>(bCommandable);

    // Execute
    oTarget->setCommandable(commandable);
    return Variable::ofNull();
}

static Variable GetCommandable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(oTarget->isCommandable()));
}

static Variable GetHitDice(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(creature->attributes().getAggregateLevel());
}

static Variable GetTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofString(oObject->tag());
}

static Variable ResistForce(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSource = getObject(args, 0, ctx);
    auto oTarget = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ResistForce");
}

static Variable GetEffectType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(eEffect->type()));
}

static Variable GetFactionEqual(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFirstObject = getObject(args, 0, ctx);
    auto oSecondObject = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto firstObject = checkCreature(oFirstObject);
    auto secondObject = checkCreature(oSecondObject);

    // Execute
    return Variable::ofInt(static_cast<int>(firstObject->faction() == secondObject->faction()));
}

static Variable ChangeFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObjectToChangeFaction = getObject(args, 0, ctx);
    auto oMemberOfFactionToJoin = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeFaction");
}

static Variable GetIsListening(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsListening");
}

static Variable SetListening(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto bValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetListening");
}

static Variable SetListenPattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto sPattern = getString(args, 1);
    auto nNumber = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetListenPattern");
}

static Variable TestStringAgainstPattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sPattern = getString(args, 0);
    auto sStringToTest = getString(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TestStringAgainstPattern");
}

static Variable GetMatchedSubstring(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nString = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetMatchedSubstring");
}

static Variable GetMatchedSubstringsCount(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetMatchedSubstringsCount");
}

static Variable GetFactionWeakestMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionWeakestMember");
}

static Variable GetFactionStrongestMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionStrongestMember");
}

static Variable GetFactionMostDamagedMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionMostDamagedMember");
}

static Variable GetFactionLeastDamagedMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionLeastDamagedMember");
}

static Variable GetFactionGold(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionGold");
}

static Variable GetFactionAverageReputation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSourceFactionMember = getObject(args, 0, ctx);
    auto oTarget = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionAverageReputation");
}

static Variable GetFactionAverageGoodEvilAlignment(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionAverageGoodEvilAlignment");
}

static Variable SoundObjectGetFixedVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectGetFixedVariance");
}

static Variable GetFactionAverageLevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionAverageLevel");
}

static Variable GetFactionAverageXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionAverageXP");
}

static Variable GetFactionMostFrequentClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionMostFrequentClass");
}

static Variable GetFactionWorstAC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionWorstAC");
}

static Variable GetFactionBestAC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFactionMember = getObjectOrCaller(args, 0, ctx);
    auto bMustBeVisible = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionBestAC");
}

static Variable GetGlobalString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);

    // Transform

    // Execute
    return Variable::ofString(ctx.game.getGlobalString(sIdentifier));
}

static Variable GetListenPatternNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetListenPatternNumber");
}

static Variable GetWaypointByTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sWaypointTag = getString(args, 0);

    // Transform
    auto waypointTag = boost::to_lower_copy(sWaypointTag);

    // Execute
    std::shared_ptr<Object> waypoint;
    for (auto &object : ctx.game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        if (object->tag() == waypointTag) {
            waypoint = object;
            break;
        }
    }
    return Variable::ofObject(getObjectIdOrInvalid(waypoint));
}

static Variable GetTransitionTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTransition = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTransitionTarget");
}

static Variable GetObjectByTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sTag = getString(args, 0);
    auto nNth = getIntOrElse(args, 1, 0);

    // Transform
    auto tag = boost::to_lower_copy(sTag);

    // Execute
    std::shared_ptr<Object> object;
    if (!tag.empty()) {
        object = ctx.game.module()->area()->getObjectByTag(tag, nNth);
    } else {
        object = ctx.game.party().player();
    }
    return Variable::ofObject(getObjectIdOrInvalid(object));
}

static Variable AdjustAlignment(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSubject = getObject(args, 0, ctx);
    auto nAlignment = getInt(args, 1);
    auto nShift = getInt(args, 2);
    auto bDontModifyNPCs = getIntOrElse(args, 3, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AdjustAlignment");
}

static Variable SetAreaTransitionBMP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPredefinedAreaTransition = getInt(args, 0);
    auto sCustomAreaTransitionBMP = getStringOrElse(args, 1, "");

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetAreaTransitionBMP");
}

static Variable GetReputation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSource = getObject(args, 0, ctx);
    auto oTarget = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetReputation");
}

static Variable AdjustReputation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSourceFactionMember = getObject(args, 1, ctx);
    auto nAdjustment = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AdjustReputation");
}

static Variable GetModuleFileName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleFileName");
}

static Variable GetGoingToBeAttackedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetGoingToBeAttackedBy");
}

static Variable GetLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto location = std::make_shared<game::Location>(oObject->position(), oObject->getFacing());
    return Variable::ofLocation(std::move(location));
}

static Variable Location(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vPosition = getVector(args, 0);
    auto fOrientation = getFloat(args, 1);

    // Transform
    auto orientation = glm::radians(fOrientation);

    // Execute
    auto location = std::make_shared<game::Location>(std::move(vPosition), orientation);
    return Variable::ofLocation(location);
}

static Variable ApplyEffectAtLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDurationType = getInt(args, 0);
    auto eEffect = getEffect(args, 1);
    auto lLocation = getLocationArgument(args, 2);
    auto fDuration = getFloatOrElse(args, 3, 0.0f);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ApplyEffectAtLocation");
}

static Variable GetIsPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    bool pc = creature == ctx.game.party().player();
    return Variable::ofInt(static_cast<int>(pc));
}

static Variable FeetToMeters(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fFeet = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("FeetToMeters");
}

static Variable YardsToMeters(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fYards = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("YardsToMeters");
}

static Variable ApplyEffectToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDurationType = getInt(args, 0);
    auto eEffect = getEffect(args, 1);
    auto oTarget = getObject(args, 2, ctx);
    auto fDuration = getFloatOrElse(args, 3, 0.0f);

    // Transform
    auto durationType = static_cast<DurationType>(nDurationType);

    // Execute
    oTarget->applyEffect(eEffect, durationType, fDuration);
    return Variable::ofNull();
}

static Variable SpeakString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sStringToSpeak = getString(args, 0);
    auto nTalkVolume = getIntOrElse(args, 1, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpeakString");
}

static Variable GetSpellTargetLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellTargetLocation");
}

static Variable GetPositionFromLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lLocation = getLocationArgument(args, 0);

    // Transform

    // Execute
    return Variable::ofVector(lLocation->position());
}

static Variable GetFacingFromLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lLocation = getLocationArgument(args, 0);

    // Transform

    // Execute
    return Variable::ofFloat(glm::degrees(lLocation->facing()));
}

static Variable GetNearestCreatureToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFirstCriteriaType = getInt(args, 0);
    auto nFirstCriteriaValue = getInt(args, 1);
    auto lLocation = getLocationArgument(args, 2);
    auto nNth = getIntOrElse(args, 3, 1);
    auto nSecondCriteriaType = getIntOrElse(args, 4, -1);
    auto nSecondCriteriaValue = getIntOrElse(args, 5, -1);
    auto nThirdCriteriaType = getIntOrElse(args, 6, -1);
    auto nThirdCriteriaValue = getIntOrElse(args, 7, -1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNearestCreatureToLocation");
}

static Variable GetNearestObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nObjectType = getIntOrElse(args, 0, 32767);
    auto oTarget = getObjectOrCaller(args, 1, ctx);
    auto nNth = getIntOrElse(args, 2, 1);

    // Transform
    auto objectType = static_cast<ObjectType>(nObjectType);

    // Execute
    auto object = ctx.game.module()->area()->getNearestObject(oTarget->position(), nNth - 1, [&objectType](auto &object) {
        return object->type() == objectType;
    });
    return Variable::ofObject(getObjectIdOrInvalid(object));
}

static Variable GetNearestObjectToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nObjectType = getInt(args, 0);
    auto lLocation = getLocationArgument(args, 1);
    auto nNth = getIntOrElse(args, 2, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNearestObjectToLocation");
}

static Variable GetNearestObjectByTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sTag = getString(args, 0);
    auto oTarget = getObjectOrCaller(args, 1, ctx);
    auto nNth = getIntOrElse(args, 2, 1);

    // Transform
    auto tag = boost::to_lower_copy(sTag);

    // Execute
    auto object = ctx.game.module()->area()->getNearestObject(oTarget->position(), nNth - 1, [&tag](auto &object) {
        return object->tag() == tag;
    });
    return Variable::ofObject(getObjectIdOrInvalid(object));
}

static Variable IntToFloat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInteger = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofFloat(static_cast<float>(nInteger));
}

static Variable FloatToInt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fFloat = getFloat(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(fFloat));
}

static Variable StringToInt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sNumber = getString(args, 0);

    // Transform

    // Execute
    int intValue = 0;
    if (!sNumber.empty()) {
        intValue = stoi(sNumber);
    }
    return Variable::ofInt(intValue);
}

static Variable StringToFloat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sNumber = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("StringToFloat");
}

static Variable GetIsEnemy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto target = checkCreature(oTarget);
    auto source = checkCreature(oSource);

    // Execute
    bool enemy = ctx.services.game.reputes.getIsEnemy(*target, *source);
    return Variable::ofInt(static_cast<int>(enemy));
}

static Variable GetIsFriend(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto target = checkCreature(oTarget);
    auto source = checkCreature(oSource);

    // Execute
    bool isFriend = ctx.services.game.reputes.getIsFriend(*target, *source);
    return Variable::ofInt(static_cast<int>(isFriend));
}

static Variable GetIsNeutral(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto target = checkCreature(oTarget);
    auto source = checkCreature(oSource);

    // Execute
    bool neutral = ctx.services.game.reputes.getIsNeutral(*target, *source);
    return Variable::ofInt(static_cast<int>(neutral));
}

static Variable GetPCSpeaker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto player = ctx.game.party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

static Variable GetStringByStrRef(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStrRef = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofString(ctx.services.resource.strings.get(nStrRef));
}

static Variable DestroyObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oDestroy = getObject(args, 0, ctx);
    auto fDelay = getFloatOrElse(args, 1, 0.0f);
    auto bNoFade = getIntOrElse(args, 2, 0);
    auto fDelayUntilFade = getFloatOrElse(args, 3, 0.0f);
    auto nHideFeedback = getIntOrElse(args, 4, 0);

    // Transform

    // Execute
    ctx.game.module()->area()->destroyObject(*oDestroy);
    return Variable::ofNull();
}

static Variable GetModule(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofObject(getObjectIdOrInvalid(ctx.game.module()));
}

static Variable CreateObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nObjectType = getInt(args, 0);
    auto sTemplate = getString(args, 1);
    auto lLocation = getLocationArgument(args, 2);
    auto bUseAppearAnimation = getIntOrElse(args, 3, 0);

    // Transform
    auto objectType = static_cast<ObjectType>(nObjectType);
    auto tmplt = boost::to_lower_copy(sTemplate);

    // Execute
    auto object = ctx.game.module()->area()->createObject(objectType, tmplt, lLocation);
    return Variable::ofObject(getObjectIdOrInvalid(object));
}

static Variable EventSpellCastAt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCaster = getObject(args, 0, ctx);
    auto nSpell = getInt(args, 1);
    auto bHarmful = getIntOrElse(args, 2, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EventSpellCastAt");
}

static Variable GetLastSpellCaster(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastSpellCaster");
}

static Variable GetLastSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastSpell");
}

static Variable GetUserDefinedEventNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofInt(ctx.execution.userDefinedEventNumber);
}

static Variable GetSpellId(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellId");
}

static Variable RandomName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("RandomName");
}

static Variable GetLoadFromSaveGame(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLoadFromSaveGame");
}

static Variable GetName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofString(oObject->name());
}

static Variable GetLastSpeaker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastSpeaker");
}

static Variable BeginConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sResRef = getStringOrElse(args, 0, "");
    auto oObjectToDialog = getObjectOrNull(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("BeginConversation");
}

static Variable GetLastPerceived(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    auto perceived = caller->perception().lastPerceived;
    return Variable::ofObject(getObjectIdOrInvalid(perceived));
}

static Variable GetLastPerceptionHeard(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    bool heard = caller->perception().lastPerception == PerceptionType::Heard;
    return Variable::ofInt(static_cast<int>(heard));
}

static Variable GetLastPerceptionInaudible(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    bool inaudible = caller->perception().lastPerception == PerceptionType::NotHeard;
    return Variable::ofInt(static_cast<int>(inaudible));
}

static Variable GetLastPerceptionSeen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    bool seen = caller->perception().lastPerception == PerceptionType::Seen;
    return Variable::ofInt(static_cast<int>(seen));
}

static Variable GetLastClosedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

static Variable GetLastPerceptionVanished(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    bool vanished = caller->perception().lastPerception == PerceptionType::NotSeen;
    return Variable::ofInt(static_cast<int>(vanished));
}

static Variable GetFirstInPersistentObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPersistentObject = getObjectOrCaller(args, 0, ctx);
    auto nResidentObjectType = getIntOrElse(args, 1, 1);
    auto nPersistentZone = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstInPersistentObject");
}

static Variable GetNextInPersistentObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPersistentObject = getObjectOrCaller(args, 0, ctx);
    auto nResidentObjectType = getIntOrElse(args, 1, 1);
    auto nPersistentZone = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextInPersistentObject");
}

static Variable GetAreaOfEffectCreator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAreaOfEffectObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAreaOfEffectCreator");
}

static Variable ShowLevelUpGUI(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("ShowLevelUpGUI");
}

static Variable SetItemNonEquippable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto bNonEquippable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetItemNonEquippable");
}

static Variable GetButtonMashCheck(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetButtonMashCheck");
}

static Variable SetButtonMashCheck(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCheck = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetButtonMashCheck");
}

static Variable GiveItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto oGiveTo = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GiveItem");
}

static Variable ObjectToString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofString(str(boost::format("%x") % oObject->id()));
}

static Variable GetIsImmune(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nImmunityType = getInt(args, 1);
    auto oVersus = getObjectOrNull(args, 2, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsImmune");
}

static Variable GetEncounterActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oEncounter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEncounterActive");
}

static Variable SetEncounterActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNewValue = getInt(args, 0);
    auto oEncounter = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEncounterActive");
}

static Variable GetEncounterSpawnsMax(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oEncounter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEncounterSpawnsMax");
}

static Variable SetEncounterSpawnsMax(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNewValue = getInt(args, 0);
    auto oEncounter = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEncounterSpawnsMax");
}

static Variable GetEncounterSpawnsCurrent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oEncounter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEncounterSpawnsCurrent");
}

static Variable SetEncounterSpawnsCurrent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNewValue = getInt(args, 0);
    auto oEncounter = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEncounterSpawnsCurrent");
}

static Variable GetModuleItemAcquired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleItemAcquired");
}

static Variable GetModuleItemAcquiredFrom(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleItemAcquiredFrom");
}

static Variable SetCustomToken(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCustomTokenNumber = getInt(args, 0);
    auto sTokenValue = getString(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCustomToken");
}

static Variable GetHasFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto feat = static_cast<FeatType>(nFeat);
    auto creature = checkCreature(oCreature);

    // Execute
    bool hasFeat = creature->attributes().hasFeat(feat);
    return Variable::ofInt(static_cast<int>(hasFeat));
}

static Variable GetHasSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto skill = static_cast<SkillType>(nSkill);
    auto creature = checkCreature(oCreature);

    // Execute
    bool hasSkill = creature->attributes().hasSkill(skill);
    return Variable::ofInt(static_cast<int>(hasSkill));
}

static Variable GetObjectSeen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto source = checkCreature(oSource);

    // Execute
    bool seen = source->perception().seen.count(oTarget) > 0;
    return Variable::ofInt(static_cast<int>(seen));
}

static Variable GetObjectHeard(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto oSource = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetObjectHeard");
}

static Variable GetLastPlayerDied(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastPlayerDied");
}

static Variable GetModuleItemLost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleItemLost");
}

static Variable GetModuleItemLostBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleItemLostBy");
}

static Variable EventConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("EventConversation");
}

static Variable SetEncounterDifficulty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nEncounterDifficulty = getInt(args, 0);
    auto oEncounter = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEncounterDifficulty");
}

static Variable GetEncounterDifficulty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oEncounter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEncounterDifficulty");
}

static Variable GetDistanceBetweenLocations(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lLocationA = getLocationArgument(args, 0);
    auto lLocationB = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetDistanceBetweenLocations");
}

static Variable GetReflexAdjustedDamage(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamage = getInt(args, 0);
    auto oTarget = getObject(args, 1, ctx);
    auto nDC = getInt(args, 2);
    auto nSaveType = getIntOrElse(args, 3, 0);
    auto oSaveVersus = getObjectOrCaller(args, 4, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetReflexAdjustedDamage");
}

static Variable PlayAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAnimation = getInt(args, 0);
    auto fSpeed = getFloatOrElse(args, 1, 1.0f);
    auto fSeconds = getFloatOrElse(args, 2, 0.0f);

    // Transform
    auto animation = static_cast<AnimationType>(nAnimation);

    // Execute
    AnimationProperties properties;
    properties.speed = fSpeed;
    auto caller = getCaller(ctx);
    caller->playAnimation(animation, std::move(properties));
    return Variable::ofNull();
}

static Variable TalentSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);

    // Transform

    // Execute
    auto talent = std::make_shared<Talent>(TalentType::Spell, nSpell);
    return Variable::ofTalent(std::move(talent));
}

static Variable TalentFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);

    // Transform

    // Execute
    auto talent = std::make_shared<Talent>(TalentType::Feat, nFeat);
    return Variable::ofTalent(std::move(talent));
}

static Variable TalentSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("TalentSkill");
}

static Variable GetHasSpellEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oObject = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHasSpellEffect");
}

static Variable GetEffectSpellId(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eSpellEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetEffectSpellId");
}

static Variable GetCreatureHasTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tTalent = getTalent(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureHasTalent");
}

static Variable GetCreatureTalentRandom(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCategory = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);
    auto nInclusion = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureTalentRandom");
}

static Variable GetCreatureTalentBest(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCategory = getInt(args, 0);
    auto nCRMax = getInt(args, 1);
    auto oCreature = getObjectOrCaller(args, 2, ctx);
    auto nInclusion = getIntOrElse(args, 3, 0);
    auto nExcludeType = getIntOrElse(args, 4, -1);
    auto nExcludeId = getIntOrElse(args, 5, -1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureTalentBest");
}

static Variable GetGoldPieceValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetGoldPieceValue");
}

static Variable GetIsPlayableRacialType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPlayableRacialType");
}

static Variable JumpToLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lDestination = getLocationArgument(args, 0);

    // Transform

    // Execute
    auto action = ctx.game.actionFactory().newJumpToLocation(std::move(lDestination));
    getCaller(ctx)->addActionOnTop(std::move(action));
    return Variable::ofNull();
}

static Variable GetSkillRank(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);
    auto oTarget = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto skill = static_cast<SkillType>(nSkill);
    auto target = checkCreature(oTarget);

    // Execute
    return Variable::ofInt(target->attributes().getSkillRank(skill));
}

static Variable GetAttackTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    auto target = creature->getAttackTarget();
    return Variable::ofObject(getObjectIdOrInvalid(target));
}

static Variable GetLastAttackType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttackType");
}

static Variable GetLastAttackMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttackMode");
}

static Variable GetDistanceBetween2D(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObjectA = getObject(args, 0, ctx);
    auto oObjectB = getObject(args, 1, ctx);

    // Transform

    // Execute
    float distance = oObjectA->getDistanceTo(glm::vec2(oObjectB->position()));
    return Variable::ofFloat(distance);
}

static Variable GetIsInCombat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);
    auto bOnlyCountReal = getIntOrElse(args, 1, 0);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->isInCombat()));
}

static Variable GetLastAssociateCommand(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAssociate = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAssociateCommand");
}

static Variable GiveGoldToCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nGP = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    creature->giveGold(nGP);
    return Variable::ofNull();
}

static Variable SetIsDestroyable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bDestroyable = getInt(args, 0);
    auto bRaiseable = getIntOrElse(args, 1, 1);
    auto bSelectableWhenDead = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetIsDestroyable");
}

static Variable SetLocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto bLocked = getInt(args, 1);

    // Transform
    auto target = checkDoor(oTarget);
    bool locked = static_cast<bool>(bLocked);

    // Execute
    target->setLocked(locked);
    return Variable::ofNull();
}

static Variable GetLocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform
    auto target = checkDoor(oTarget);

    // Execute
    return Variable::ofInt(static_cast<int>(target->isLocked()));
}

static Variable GetClickingObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetClickingObject");
}

static Variable SetAssociateListenPatterns(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetAssociateListenPatterns");
}

static Variable GetLastWeaponUsed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastWeaponUsed");
}

static Variable GetLastUsedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastUsedBy");
}

static Variable GetAbilityModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAbility = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAbilityModifier");
}

static Variable GetIdentified(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIdentified");
}

static Variable SetIdentified(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto bIdentified = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetIdentified");
}

static Variable GetDistanceBetweenLocations2D(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto lLocationA = getLocationArgument(args, 0);
    auto lLocationB = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetDistanceBetweenLocations2D");
}

static Variable GetDistanceToObject2D(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    auto caller = getCaller(ctx);
    float result = caller->getDistanceTo(glm::vec2(oObject->position()));
    return Variable::ofFloat(result);
}

static Variable GetBlockingDoor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetBlockingDoor");
}

static Variable GetIsDoorActionPossible(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTargetDoor = getObject(args, 0, ctx);
    auto nDoorAction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsDoorActionPossible");
}

static Variable DoDoorAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTargetDoor = getObject(args, 0, ctx);
    auto nDoorAction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DoDoorAction");
}

static Variable GetFirstItemInInventory(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    auto item = oTarget->getFirstItem();
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable GetNextItemInInventory(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    auto item = oTarget->getNextItem();
    return Variable::ofObject(getObjectIdOrInvalid(item));
}

static Variable GetClassByPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClassPosition = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    auto clazz = creature->attributes().getClassByPosition(nClassPosition);
    return Variable::ofInt(static_cast<int>(clazz));
}

static Variable GetLevelByPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClassPosition = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    int level = creature->attributes().getLevelByPosition(nClassPosition);
    return Variable::ofInt(level);
}

static Variable GetLevelByClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClassType = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);
    auto classType = static_cast<ClassType>(nClassType);

    // Execute
    int level = creature->attributes().getClassLevel(classType);
    return Variable::ofInt(level);
}

static Variable GetDamageDealtByType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDamageType = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetDamageDealtByType");
}

static Variable GetTotalDamageDealt(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetTotalDamageDealt");
}

static Variable GetLastDamager(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastDamager");
}

static Variable GetLastDisarmed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastDisarmed");
}

static Variable GetLastDisturbed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastDisturbed");
}

static Variable GetLastLocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastLocked");
}

static Variable GetLastUnlocked(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastUnlocked");
}

static Variable GetInventoryDisturbType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetInventoryDisturbType");
}

static Variable GetInventoryDisturbItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetInventoryDisturbItem");
}

static Variable ShowUpgradeScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObjectOrNull(args, 0, ctx);
    auto oCharacter = getObjectOrNull(args, 1, ctx);
    auto nDisableItemCreation = getIntOrElse(args, 2, 0);
    auto nDisableUpgrade = getIntOrElse(args, 3, 0);
    auto sOverride2DA = getStringOrElse(args, 4, "");

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowUpgradeScreen");
}

static Variable VersusAlignmentEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);
    auto nLawChaos = getIntOrElse(args, 1, 0);
    auto nGoodEvil = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VersusAlignmentEffect");
}

static Variable VersusRacialTypeEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);
    auto nRacialType = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VersusRacialTypeEffect");
}

static Variable VersusTrapEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("VersusTrapEffect");
}

static Variable GetGender(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->gender()));
}

static Variable GetIsTalentValid(const std::vector<Variable> &args, const RoutineContext &ctx) {
    bool valid;
    try {
        auto tTalent = getTalent(args, 0);
        valid = static_cast<bool>(tTalent);
    } catch (const RoutineArgumentException &ignored) {
        valid = false;
    }
    return Variable::ofInt(static_cast<int>(valid));
}

static Variable GetAttemptedAttackTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    auto target = caller->getAttemptedAttackTarget();
    return Variable::ofObject(getObjectIdOrInvalid(target));
}

static Variable GetTypeFromTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tTalent = getTalent(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(tTalent->type()));
}

static Variable GetIdFromTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tTalent = getTalent(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIdFromTalent");
}

static Variable PlayPazaak(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nOpponentPazaakDeck = getInt(args, 0);
    auto sEndScript = getString(args, 1);
    auto nMaxWager = getInt(args, 2);
    auto bShowTutorial = getIntOrElse(args, 3, 0);
    auto oOpponent = getObjectOrNull(args, 4, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayPazaak");
}

static Variable GetLastPazaakResult(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastPazaakResult");
}

static Variable DisplayFeedBackText(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nTextConstant = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisplayFeedBackText");
}

static Variable AddJournalQuestEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);
    auto nState = getInt(args, 1);
    auto bAllowOverrideHigher = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddJournalQuestEntry");
}

static Variable RemoveJournalQuestEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveJournalQuestEntry");
}

static Variable GetJournalEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetJournalEntry");
}

static Variable PlayRumblePattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPattern = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayRumblePattern");
}

static Variable StopRumblePattern(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPattern = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("StopRumblePattern");
}

static Variable SendMessageToPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlayer = getObject(args, 0, ctx);
    auto szMessage = getString(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SendMessageToPC");
}

static Variable GetAttemptedSpellTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetAttemptedSpellTarget");
}

static Variable GetLastOpenedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto triggerrer = getTriggerrer(ctx);
    return Variable::ofObject(getObjectIdOrInvalid(triggerrer));
}

static Variable GetHasSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHasSpell");
}

static Variable OpenStore(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oStore = getObject(args, 0, ctx);
    auto oPC = getObject(args, 1, ctx);
    auto nBonusMarkUp = getIntOrElse(args, 2, 0);
    auto nBonusMarkDown = getIntOrElse(args, 3, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("OpenStore");
}

static Variable GetFirstFactionMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMemberOfFaction = getObject(args, 0, ctx);
    auto bPCOnly = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstFactionMember");
}

static Variable GetNextFactionMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMemberOfFaction = getObject(args, 0, ctx);
    auto bPCOnly = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextFactionMember");
}

static Variable GetJournalQuestExperience(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetJournalQuestExperience");
}

static Variable JumpToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oToJumpTo = getObject(args, 0, ctx);
    auto nWalkStraightLineToPoint = getIntOrElse(args, 1, 1);

    // Transform
    auto walkStraightLineToPoint = static_cast<bool>(nWalkStraightLineToPoint);

    // Execute
    auto action = ctx.game.actionFactory().newJumpToObject(std::move(oToJumpTo), walkStraightLineToPoint);
    getCaller(ctx)->addActionOnTop(std::move(action));
    return Variable::ofNull();
}

static Variable SetMapPinEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMapPin = getObject(args, 0, ctx);
    auto nEnabled = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetMapPinEnabled");
}

static Variable PopUpGUIPanel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPC = getObject(args, 0, ctx);
    auto nGUIPanel = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PopUpGUIPanel");
}

static Variable AddMultiClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClassType = getInt(args, 0);
    auto oSource = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddMultiClass");
}

static Variable GetIsLinkImmune(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto eEffect = getEffect(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsLinkImmune");
}

static Variable GiveXPToCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nXpAmount = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    creature->giveXP(nXpAmount);
    return Variable::ofNull();
}

static Variable SetXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nXpAmount = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    creature->setXP(nXpAmount);
    return Variable::ofNull();
}

static Variable GetXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(creature->xp());
}

static Variable IntToHexString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nInteger = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IntToHexString");
}

static Variable GetBaseItemType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform
    auto item = checkItem(oItem);

    // Execute
    return Variable::ofInt(item->baseItemType());
}

static Variable GetItemHasItemProperty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto nProperty = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetItemHasItemProperty");
}

static Variable GetItemACValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetItemACValue");
}

static Variable ExploreAreaForPlayer(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto oPlayer = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ExploreAreaForPlayer");
}

static Variable GetIsDay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsDay");
}

static Variable GetIsNight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsNight");
}

static Variable GetIsDawn(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsDawn");
}

static Variable GetIsDusk(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsDusk");
}

static Variable GetIsEncounterCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsEncounterCreature");
}

static Variable GetLastPlayerDying(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastPlayerDying");
}

static Variable GetStartingLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetStartingLocation");
}

static Variable ChangeToStandardFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreatureToChange = getObject(args, 0, ctx);
    auto nStandardFaction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeToStandardFaction");
}

static Variable SoundObjectPlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform
    auto sound = checkSound(oSound);

    // Execute
    sound->setActive(true);
    return Variable::ofNull();
}

static Variable SoundObjectStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform
    auto sound = checkSound(oSound);

    // Execute
    sound->setActive(false);
    return Variable::ofNull();
}

static Variable SoundObjectSetVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto nVolume = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectSetVolume");
}

static Variable SoundObjectSetPosition(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto vPosition = getVector(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectSetPosition");
}

static Variable SpeakOneLinerConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sDialogResRef = getStringOrElse(args, 0, "");
    auto oTokenTarget = getObjectOrNull(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpeakOneLinerConversation");
}

static Variable GetGold(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oTarget);

    // Execute
    return Variable::ofInt(creature->gold());
}

static Variable GetLastRespawnButtonPresser(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastRespawnButtonPresser");
}

static Variable SetLightsaberPowered(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto bOverride = getInt(args, 1);
    auto bPowered = getIntOrElse(args, 2, 1);
    auto bShowTransition = getIntOrElse(args, 3, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetLightsaberPowered");
}

static Variable GetIsWeaponEffective(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oVersus = getObjectOrNull(args, 0, ctx);
    auto bOffHand = getIntOrElse(args, 1, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsWeaponEffective");
}

static Variable GetLastSpellHarmful(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastSpellHarmful");
}

static Variable EventActivateItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);
    auto lTarget = getLocationArgument(args, 1);
    auto oTarget = getObjectOrNull(args, 2, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EventActivateItem");
}

static Variable MusicBackgroundPlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundPlay");
}

static Variable MusicBackgroundStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundStop");
}

static Variable MusicBackgroundSetDelay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nDelay = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundSetDelay");
}

static Variable MusicBackgroundChangeDay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);
    auto nStreamingMusic = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundChangeDay");
}

static Variable MusicBackgroundChangeNight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);
    auto nStreamingMusic = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundChangeNight");
}

static Variable MusicBattlePlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBattlePlay");
}

static Variable MusicBattleStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBattleStop");
}

static Variable MusicBattleChange(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBattleChange");
}

static Variable AmbientSoundPlay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundPlay");
}

static Variable AmbientSoundStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundStop");
}

static Variable AmbientSoundChangeDay(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundChangeDay");
}

static Variable AmbientSoundChangeNight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nTrack = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundChangeNight");
}

static Variable GetLastKiller(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastKiller");
}

static Variable GetSpellCastItem(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellCastItem");
}

static Variable GetItemActivated(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemActivated");
}

static Variable GetItemActivator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemActivator");
}

static Variable GetItemActivatedTargetLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemActivatedTargetLocation");
}

static Variable GetItemActivatedTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemActivatedTarget");
}

static Variable GetIsOpen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(oObject->isOpen()));
}

static Variable TakeGoldFromCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAmount = getInt(args, 0);
    auto oCreatureToTakeFrom = getObject(args, 1, ctx);
    auto bDestroy = getIntOrElse(args, 2, 0);

    // Transform
    auto creatureToTakeFrom = checkCreature(oCreatureToTakeFrom);
    bool destroy = static_cast<bool>(bDestroy);

    // Execute
    if (creatureToTakeFrom) {
        creatureToTakeFrom->takeGold(nAmount);
    }
    if (!destroy) {
        auto caller = checkCreature(getCaller(ctx));
        caller->giveGold(nAmount);
    }
    return Variable::ofNull();
}

static Variable GetIsInConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsInConversation");
}

static Variable GetPlotFlag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    bool plotFlag = oTarget->plotFlag();
    return Variable::ofInt(static_cast<int>(plotFlag));
}

static Variable SetPlotFlag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto nPlotFlag = getInt(args, 1);

    // Transform
    bool plotFlag = static_cast<bool>(nPlotFlag);

    // Execute
    oTarget->setPlotFlag(plotFlag);
    return Variable::ofNull();
}

static Variable SetDialogPlaceableCamera(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCameraId = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetDialogPlaceableCamera");
}

static Variable GetSoloMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    bool solo = ctx.game.party().isSoloMode();
    return Variable::ofInt(static_cast<int>(solo));
}

static Variable GetMaxStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetMaxStealthXP");
}

static Variable SetMaxStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nMax = getInt(args, 0);

    // Transform

    // Execute
    ctx.game.module()->area()->setMaxStealthXP(nMax);
    return Variable::ofNull();
}

static Variable GetCurrentStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    int xp = ctx.game.module()->area()->currentStealthXP();
    return Variable::ofInt(xp);
}

static Variable GetNumStackedItems(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNumStackedItems");
}

static Variable SurrenderToEnemies(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("SurrenderToEnemies");
}

static Variable SetCurrentStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCurrent = getInt(args, 0);

    // Transform

    // Execute
    ctx.game.module()->area()->setCurrentStealthXP(nCurrent);
    return Variable::ofNull();
}

static Variable GetCreatureSize(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureSize");
}

static Variable AwardStealthXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AwardStealthXP");
}

static Variable GetStealthXPEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetStealthXPEnabled");
}

static Variable SetStealthXPEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bEnabled = getInt(args, 0);

    // Transform
    bool enabled = static_cast<bool>(bEnabled);

    // Execute
    ctx.game.module()->area()->setStealthXPEnabled(enabled);
    return Variable::ofNull();
}

static Variable GetLastTrapDetected(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastTrapDetected");
}

static Variable GetNearestTrapToObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);
    auto nTrapDetected = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNearestTrapToObject");
}

static Variable GetAttemptedMovementTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetAttemptedMovementTarget");
}

static Variable GetBlockingCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetBlockingCreature");
}

static Variable GetFortitudeSavingThrow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFortitudeSavingThrow");
}

static Variable GetWillSavingThrow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetWillSavingThrow");
}

static Variable GetReflexSavingThrow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetReflexSavingThrow");
}

static Variable GetChallengeRating(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetChallengeRating");
}

static Variable GetFoundEnemyCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFoundEnemyCreature");
}

static Variable GetMovementRate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetMovementRate");
}

static Variable GetSubRace(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->subrace()));
}

static Variable GetStealthXPDecrement(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetStealthXPDecrement");
}

static Variable SetStealthXPDecrement(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nDecrement = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetStealthXPDecrement");
}

static Variable DuplicateHeadAppearance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidCreatureToChange = getObject(args, 0, ctx);
    auto oidCreatureToMatch = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DuplicateHeadAppearance");
}

static Variable CutsceneAttack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto nAnimation = getInt(args, 1);
    auto nAttackResult = getInt(args, 2);
    auto nDamage = getInt(args, 3);

    // Transform
    auto attackResult = static_cast<AttackResultType>(nAttackResult);

    // Execute
    auto caller = checkCreature(getCaller(ctx));
    ctx.game.combat().addAttack(caller, oTarget, nullptr, attackResult, nDamage);
    return Variable::ofNull();
}

static Variable SetCameraMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlayer = getObject(args, 0, ctx);
    auto nCameraMode = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCameraMode");
}

static Variable SetLockOrientationInDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetLockOrientationInDialog");
}

static Variable SetLockHeadFollowInDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetLockHeadFollowInDialog");
}

static Variable CutsceneMove(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto vPosition = getVector(args, 1);
    auto nRun = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("CutsceneMove");
}

static Variable EnableVideoEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nEffectType = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EnableVideoEffect");
}

static Variable StartNewModule(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sModuleName = getString(args, 0);
    auto sWayPoint = getStringOrElse(args, 1, "");
    auto sMovie1 = getStringOrElse(args, 2, "");
    auto sMovie2 = getStringOrElse(args, 3, "");
    auto sMovie3 = getStringOrElse(args, 4, "");
    auto sMovie4 = getStringOrElse(args, 5, "");
    auto sMovie5 = getStringOrElse(args, 6, "");
    auto sMovie6 = getStringOrElse(args, 7, "");

    // Transform
    auto moduleName = boost::to_lower_copy(sModuleName);
    auto waypoint = boost::to_lower_copy(sWayPoint);

    // Execute
    ctx.game.scheduleModuleTransition(moduleName, waypoint);
    return Variable::ofNull();
}

static Variable DisableVideoEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("DisableVideoEffect");
}

static Variable GetWeaponRanged(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oItem = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetWeaponRanged");
}

static Variable DoSinglePlayerAutoSave(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("DoSinglePlayerAutoSave");
}

static Variable GetGameDifficulty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetGameDifficulty");
}

static Variable GetUserActionsPending(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = checkCreature(getCaller(ctx));
    return Variable::ofInt(static_cast<int>(caller->hasUserActionsPending()));
}

static Variable RevealMap(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vPoint = getVectorOrElse(args, 0, glm::vec3(0.0f, 0.0f, 0.0f));
    auto nRadius = getIntOrElse(args, 1, -1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RevealMap");
}

static Variable SetTutorialWindowsEnabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bEnabled = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetTutorialWindowsEnabled");
}

static Variable ShowTutorialWindow(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nWindow = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowTutorialWindow");
}

static Variable StartCreditSequence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bTransparentBackground = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("StartCreditSequence");
}

static Variable IsCreditSequenceInProgress(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("IsCreditSequenceInProgress");
}

static Variable GetCurrentAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    auto action = oObject->getCurrentAction();
    return Variable::ofInt(static_cast<int>(action ? action->type() : ActionType::QueueEmpty));
}

static Variable GetDifficultyModifier(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetDifficultyModifier");
}

static Variable GetAppearanceType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetAppearanceType");
}

static Variable FloatingTextStrRefOnCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStrRefToDisplay = getInt(args, 0);
    auto oCreatureToFloatAbove = getObject(args, 1, ctx);
    auto bBroadcastToFaction = getIntOrElse(args, 2, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("FloatingTextStrRefOnCreature");
}

static Variable FloatingTextStringOnCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sStringToDisplay = getString(args, 0);
    auto oCreatureToFloatAbove = getObject(args, 1, ctx);
    auto bBroadcastToFaction = getIntOrElse(args, 2, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("FloatingTextStringOnCreature");
}

static Variable GetTrapDisarmable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDisarmable");
}

static Variable GetTrapDetectable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDetectable");
}

static Variable GetTrapDetectedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);
    auto oCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDetectedBy");
}

static Variable GetTrapFlagged(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapFlagged");
}

static Variable GetTrapBaseType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapBaseType");
}

static Variable GetTrapOneShot(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapOneShot");
}

static Variable GetTrapCreator(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapCreator");
}

static Variable GetTrapKeyTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapKeyTag");
}

static Variable GetTrapDisarmDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDisarmDC");
}

static Variable GetTrapDetectDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrapObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetTrapDetectDC");
}

static Variable GetLockKeyRequired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockKeyRequired");
}

static Variable GetLockKeyTag(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockKeyTag");
}

static Variable GetLockLockable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockLockable");
}

static Variable GetLockUnlockDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockUnlockDC");
}

static Variable GetLockLockDC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLockLockDC");
}

static Variable GetPCLevellingUp(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetPCLevellingUp");
}

static Variable GetHasFeatEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);
    auto oObject = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHasFeatEffect");
}

static Variable SetPlaceableIllumination(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObjectOrCaller(args, 0, ctx);
    auto bIlluminate = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetPlaceableIllumination");
}

static Variable GetPlaceableIllumination(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetPlaceableIllumination");
}

static Variable GetIsPlaceableObjectActionPossible(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObject(args, 0, ctx);
    auto nPlaceableAction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPlaceableObjectActionPossible");
}

static Variable DoPlaceableObjectAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObject(args, 0, ctx);
    auto nPlaceableAction = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DoPlaceableObjectAction");
}

static Variable GetFirstPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto player = ctx.game.party().player();
    return Variable::ofObject(getObjectIdOrInvalid(player));
}

static Variable GetNextPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofObject(kObjectInvalid);
}

static Variable SetTrapDetectedBy(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrap = getObject(args, 0, ctx);
    auto oDetector = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetTrapDetectedBy");
}

static Variable GetIsTrapped(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsTrapped");
}

static Variable SetEffectIcon(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto eEffect = getEffect(args, 0);
    auto nIcon = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetEffectIcon");
}

static Variable FaceObjectAwayFromObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oFacer = getObject(args, 0, ctx);
    auto oObjectToFaceAwayFrom = getObject(args, 1, ctx);

    // Transform

    // Execute
    oFacer->faceAwayFrom(*oObjectToFaceAwayFrom);
    return Variable::ofNull();
}

static Variable PopUpDeathGUIPanel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPC = getObject(args, 0, ctx);
    auto bRespawnButtonEnabled = getIntOrElse(args, 1, 1);
    auto bWaitForHelpButtonEnabled = getIntOrElse(args, 2, 1);
    auto nHelpStringReference = getIntOrElse(args, 3, 0);
    auto sHelpString = getStringOrElse(args, 4, "");

    // Transform

    // Execute
    throw RoutineNotImplementedException("PopUpDeathGUIPanel");
}

static Variable SetTrapDisabled(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTrap = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetTrapDisabled");
}

static Variable GetLastHostileActor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oVictim = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastHostileActor");
}

static Variable ExportAllCharacters(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("ExportAllCharacters");
}

static Variable MusicBackgroundGetDayTrack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundGetDayTrack");
}

static Variable MusicBackgroundGetNightTrack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundGetNightTrack");
}

static Variable WriteTimestampedLogEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sLogEntry = getString(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("WriteTimestampedLogEntry");
}

static Variable GetModuleName(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetModuleName");
}

static Variable GetFactionLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMemberOfFaction = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFactionLeader");
}

static Variable EndGame(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nShowEndGameGui = getIntOrElse(args, 0, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EndGame");
}

static Variable GetRunScriptVar(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofInt(ctx.execution.scriptVar);
}

static Variable GetCreatureMovmentType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCreatureMovmentType");
}

static Variable AmbientSoundSetDayVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nVolume = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundSetDayVolume");
}

static Variable AmbientSoundSetNightVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto nVolume = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AmbientSoundSetNightVolume");
}

static Variable MusicBackgroundGetBattleTrack(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("MusicBackgroundGetBattleTrack");
}

static Variable GetHasInventory(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHasInventory");
}

static Variable GetStrRefSoundDuration(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStrRef = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetStrRefSoundDuration");
}

static Variable AddToParty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPC = getObject(args, 0, ctx);
    auto oPartyLeader = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddToParty");
}

static Variable RemoveFromParty(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPC = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveFromParty");
}

static Variable AddPartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto oCreature = getObject(args, 1, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    bool added = ctx.game.party().addAvailableMember(nNPC, creature->blueprintResRef());
    return Variable::ofInt(static_cast<int>(added));
}

static Variable RemovePartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    bool removed = false;
    if (ctx.game.party().isMember(nNPC)) {
        ctx.game.party().removeMember(nNPC);
        auto area = ctx.game.module()->area();
        area->unloadParty();
        area->reloadParty();
        removed = true;
    }
    return Variable::ofInt(static_cast<int>(removed));
}

static Variable IsObjectPartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    bool member = ctx.game.party().isMember(*creature);
    return Variable::ofInt(static_cast<int>(member));
}

static Variable GetPartyMemberByIndex(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nIndex = getInt(args, 0);

    // Transform

    // Execute
    auto member = ctx.game.party().getMember(nIndex);
    return Variable::ofObject(getObjectIdOrInvalid(member));
}

static Variable GetGlobalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);

    // Transform

    // Execute
    bool value = ctx.game.getGlobalBoolean(sIdentifier);
    return Variable::ofInt(static_cast<int>(value));
}

static Variable SetGlobalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto nValue = getInt(args, 1);

    // Transform

    // Execute
    ctx.game.setGlobalBoolean(sIdentifier, nValue);
    return Variable::ofNull();
}

static Variable GetGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);

    // Transform

    // Execute
    int value = ctx.game.getGlobalNumber(sIdentifier);
    return Variable::ofInt(value);
}

static Variable SetGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto nValue = getInt(args, 1);

    // Transform

    // Execute
    ctx.game.setGlobalNumber(sIdentifier, nValue);
    return Variable::ofNull();
}

static Variable AurPostString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sString = getString(args, 0);
    auto nX = getInt(args, 1);
    auto nY = getInt(args, 2);
    auto fLife = getFloat(args, 3);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AurPostString");
}

static Variable AddJournalWorldEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nIndex = getInt(args, 0);
    auto szEntry = getString(args, 1);
    auto szTitle = getStringOrElse(args, 2, "World Entry");

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddJournalWorldEntry");
}

static Variable AddJournalWorldEntryStrref(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto strref = getInt(args, 0);
    auto strrefTitle = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddJournalWorldEntryStrref");
}

static Variable BarkString(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto strRef = getInt(args, 1);
    auto nBarkX = getIntOrElse(args, 2, -1);
    auto nBarkY = getIntOrElse(args, 3, -1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("BarkString");
}

static Variable DeleteJournalWorldAllEntries(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("DeleteJournalWorldAllEntries");
}

static Variable DeleteJournalWorldEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nIndex = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DeleteJournalWorldEntry");
}

static Variable DeleteJournalWorldEntryStrref(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto strref = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DeleteJournalWorldEntryStrref");
}

static Variable PlayVisualAreaEffect(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nEffectID = getInt(args, 0);
    auto lTarget = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayVisualAreaEffect");
}

static Variable SetJournalQuestEntryPicture(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto szPlotID = getString(args, 0);
    auto oObject = getObject(args, 1, ctx);
    auto nPictureIndex = getInt(args, 2);
    auto bAllPartyMemebers = getIntOrElse(args, 3, 1);
    auto bAllPlayers = getIntOrElse(args, 4, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetJournalQuestEntryPicture");
}

static Variable GetLocalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nIndex = getInt(args, 1);

    // Transform

    // Execute
    bool value = oObject->getLocalBoolean(nIndex);
    return Variable::ofInt(static_cast<int>(value));
}

static Variable SetLocalBoolean(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nIndex = getInt(args, 1);
    auto nValue = getInt(args, 2);

    // Transform

    // Execute
    oObject->setLocalBoolean(nIndex, nValue);
    return Variable::ofNull();
}

static Variable GetLocalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nIndex = getInt(args, 1);

    // Transform

    // Execute
    return Variable::ofInt(oObject->getLocalNumber(nIndex));
}

static Variable SetLocalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nIndex = getInt(args, 1);
    auto nValue = getInt(args, 2);

    // Transform

    // Execute
    oObject->setLocalNumber(nIndex, nValue);
    return Variable::ofNull();
}

static Variable SoundObjectGetPitchVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectGetPitchVariance");
}

static Variable SoundObjectSetPitchVariance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto fVariance = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectSetPitchVariance");
}

static Variable SoundObjectGetVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectGetVolume");
}

static Variable GetGlobalLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);

    // Transform

    // Execute
    return Variable::ofLocation(ctx.game.getGlobalLocation(sIdentifier));
}

static Variable SetGlobalLocation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto lValue = getLocationArgument(args, 1);

    // Transform

    // Execute
    ctx.game.setGlobalLocation(sIdentifier, lValue);
    return Variable::ofNull();
}

static Variable AddAvailableNPCByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto oCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddAvailableNPCByObject");
}

static Variable RemoveAvailableNPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    bool removed = ctx.game.party().removeAvailableMember(nNPC);
    return Variable::ofInt(static_cast<int>(removed));
}

static Variable IsAvailableCreature(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    bool available = ctx.game.party().isMemberAvailable(nNPC);
    return Variable::ofInt(static_cast<int>(available));
}

static Variable AddAvailableNPCByTemplate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto sTemplate = getString(args, 1);

    // Transform
    auto tmplt = boost::to_lower_copy(sTemplate);

    // Execute
    bool added = ctx.game.party().addAvailableMember(nNPC, tmplt);
    return Variable::ofInt(static_cast<int>(added));
}

static Variable SpawnAvailableNPC(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto lPosition = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpawnAvailableNPC");
}

static Variable IsNPCPartyMember(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    bool member = ctx.game.party().isMember(nNPC);
    return Variable::ofInt(static_cast<int>(member));
}

static Variable GetIsConversationActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsConversationActive");
}

static Variable GetPartyAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetPartyAIStyle");
}

static Variable GetNPCAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->aiStyle()));
}

static Variable SetPartyAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStyle = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetPartyAIStyle");
}

static Variable SetNPCAIStyle(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nStyle = getInt(args, 1);

    // Transform
    auto creature = checkCreature(oCreature);
    auto style = static_cast<NPCAIStyle>(nStyle);

    // Execute
    creature->setAIStyle(style);
    return Variable::ofNull();
}

static Variable SetNPCSelectability(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto nSelectability = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetNPCSelectability");
}

static Variable GetNPCSelectability(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNPCSelectability");
}

static Variable ClearAllEffects(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    auto caller = getCaller(ctx);
    caller->clearAllEffects();
    return Variable::ofNull();
}

static Variable GetLastConversation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastConversation");
}

static Variable ShowPartySelectionGUI(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sExitScript = getStringOrElse(args, 0, "");
    auto nForceNPC1 = getIntOrElse(args, 1, -1);
    auto nForceNPC2 = getIntOrElse(args, 2, -1);
    auto nAllowCancel = getIntOrElse(args, 3, 0);

    // Transform
    auto exitScript = boost::to_lower_copy(sExitScript);
    auto forceNPC1 = static_cast<bool>(nForceNPC1);
    auto forceNPC2 = static_cast<bool>(nForceNPC2);

    // Execute
    PartySelectionContext partyCtx;
    partyCtx.exitScript = exitScript;
    partyCtx.forceNpc1 = forceNPC1;
    partyCtx.forceNpc2 = forceNPC2;
    ctx.game.openPartySelection(partyCtx);
    return Variable::ofNull();
}

static Variable GetStandardFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oObject);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->faction()));
}

static Variable GivePlotXP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sPlotName = getString(args, 0);
    auto nPercentage = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GivePlotXP");
}

static Variable GetMinOneHP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    return Variable::ofInt(static_cast<int>(oObject->isMinOneHP()));
}

static Variable SetMinOneHP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nMinOneHP = getInt(args, 1);

    // Transform
    auto minOneHP = static_cast<bool>(nMinOneHP);

    // Execute
    oObject->setMinOneHP(minOneHP);
    return Variable::ofNull();
}

static Variable SetGlobalFadeIn(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fWait = getFloatOrElse(args, 0, 0.0f);
    auto fLength = getFloatOrElse(args, 1, 0.0f);
    auto fR = getFloatOrElse(args, 2, 0.0f);
    auto fG = getFloatOrElse(args, 3, 0.0f);
    auto fB = getFloatOrElse(args, 4, 0.0f);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetGlobalFadeIn");
}

static Variable SetGlobalFadeOut(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fWait = getFloatOrElse(args, 0, 0.0f);
    auto fLength = getFloatOrElse(args, 1, 0.0f);
    auto fR = getFloatOrElse(args, 2, 0.0f);
    auto fG = getFloatOrElse(args, 3, 0.0f);
    auto fB = getFloatOrElse(args, 4, 0.0f);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetGlobalFadeOut");
}

static Variable GetLastHostileTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastHostileTarget");
}

static Variable GetLastAttackAction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttackAction");
}

static Variable GetLastForcePowerUsed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastForcePowerUsed");
}

static Variable GetLastCombatFeatUsed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastCombatFeatUsed");
}

static Variable GetLastAttackResult(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetLastAttackResult");
}

static Variable GetWasForcePowerSuccessful(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAttacker = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetWasForcePowerSuccessful");
}

static Variable GetFirstAttacker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFirstAttacker");
}

static Variable GetNextAttacker(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetNextAttacker");
}

static Variable SetFormation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oAnchor = getObject(args, 0, ctx);
    auto oCreature = getObject(args, 1, ctx);
    auto nFormationPattern = getInt(args, 2);
    auto nPosition = getInt(args, 3);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetFormation");
}

static Variable SetForcePowerUnsuccessful(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nResult = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetForcePowerUnsuccessful");
}

static Variable GetIsDebilitated(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform
    auto creature = checkCreature(oCreature);

    // Execute
    return Variable::ofInt(static_cast<int>(creature->isDebilitated()));
}

static Variable PlayMovie(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sMovie = getString(args, 0);
    auto nStreamingMusic = getIntOrElse(args, 1, 0);

    // Transform
    auto movie = boost::to_lower_copy(sMovie);

    // Execute
    ctx.game.playVideo(movie);
    return Variable::ofNull();
}

static Variable SaveNPCState(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SaveNPCState");
}

static Variable GetCategoryFromTalent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto tTalent = getTalent(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCategoryFromTalent");
}

static Variable SurrenderByFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFactionFrom = getInt(args, 0);
    auto nFactionTo = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SurrenderByFaction");
}

static Variable ChangeFactionByFaction(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFactionFrom = getInt(args, 0);
    auto nFactionTo = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeFactionByFaction");
}

static Variable PlayRoomAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sRoom = getString(args, 0);
    auto nAnimation = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayRoomAnimation");
}

static Variable ShowGalaxyMap(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowGalaxyMap");
}

static Variable SetPlanetSelectable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);
    auto bSelectable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetPlanetSelectable");
}

static Variable GetPlanetSelectable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetPlanetSelectable");
}

static Variable SetPlanetAvailable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);
    auto bAvailable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetPlanetAvailable");
}

static Variable GetPlanetAvailable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPlanet = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetPlanetAvailable");
}

static Variable GetSelectedPlanet(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSelectedPlanet");
}

static Variable SoundObjectFadeAndStop(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oSound = getObject(args, 0, ctx);
    auto fSeconds = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SoundObjectFadeAndStop");
}

static Variable SetAreaFogColor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oArea = getObject(args, 0, ctx);
    auto fRed = getFloat(args, 1);
    auto fGreen = getFloat(args, 2);
    auto fBlue = getFloat(args, 3);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetAreaFogColor");
}

static Variable ChangeItemCost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sItem = getString(args, 0);
    auto fCostMultiplier = getFloat(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeItemCost");
}

static Variable GetIsLiveContentAvailable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPkg = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsLiveContentAvailable");
}

static Variable ResetDialogState(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("ResetDialogState");
}

static Variable SetGoodEvilValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nAlignment = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetGoodEvilValue");
}

static Variable GetIsPoisoned(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPoisoned");
}

static Variable GetSpellTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSpellTarget");
}

static Variable SetSoloMode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto bActivate = getInt(args, 0);

    // Transform
    auto activate = static_cast<bool>(bActivate);

    // Execute
    ctx.game.party().setSoloMode(activate);
    return Variable::ofNull();
}

static Variable CancelPostDialogCharacterSwitch(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("CancelPostDialogCharacterSwitch");
}

static Variable SetMaxHitPoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nMaxHP = getInt(args, 1);

    // Transform

    // Execute
    oObject->setMaxHitPoints(nMaxHP);
    return Variable::ofNull();
}

static Variable NoClicksFor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fDuration = getFloat(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("NoClicksFor");
}

static Variable HoldWorldFadeInForDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("HoldWorldFadeInForDialog");
}

static Variable ShipBuild(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    return Variable::ofInt(static_cast<int>(kShipBuild));
}

static Variable SurrenderRetainBuffs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("SurrenderRetainBuffs");
}

static Variable SuppressStatusSummaryEntry(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNumEntries = getIntOrElse(args, 0, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SuppressStatusSummaryEntry");
}

static Variable GetCheatCode(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nCode = getInt(args, 0);

    // Transform

    // Execute
    return Variable::ofInt(0);
}

static Variable SetMusicVolume(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto fVolume = getFloatOrElse(args, 0, 1.0f);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetMusicVolume");
}

static Variable CreateItemOnFloor(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sTemplate = getString(args, 0);
    auto lLocation = getLocationArgument(args, 1);
    auto bUseAppearAnimation = getIntOrElse(args, 2, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("CreateItemOnFloor");
}

static Variable SetAvailableNPCId(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto oidNPC = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetAvailableNPCId");
}

static Variable GetScriptParameter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nIndex = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetScriptParameter");
}

static Variable SetFadeUntilScript(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("SetFadeUntilScript");
}

static Variable GetItemComponent(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemComponent");
}

static Variable GetItemComponentPieceValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetItemComponentPieceValue");
}

static Variable ShowChemicalUpgradeScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCharacter = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowChemicalUpgradeScreen");
}

static Variable GetChemicals(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetChemicals");
}

static Variable GetChemicalPieceValue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetChemicalPieceValue");
}

static Variable GetSpellForcePointCost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetSpellForcePointCost");
}

static Variable GetFeatAcquired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetFeatAcquired");
}

static Variable GetSpellAcquired(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oCreature = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSpellAcquired");
}

static Variable ShowSwoopUpgradeScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("ShowSwoopUpgradeScreen");
}

static Variable GrantFeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFeat = getInt(args, 0);
    auto oCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GrantFeat");
}

static Variable GrantSpell(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpell = getInt(args, 0);
    auto oCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GrantSpell");
}

static Variable SpawnMine(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nMineType = getInt(args, 0);
    auto lPoint = getLocationArgument(args, 1);
    auto nDetectDCBase = getInt(args, 2);
    auto nDisarmDCBase = getInt(args, 3);
    auto oCreator = getObject(args, 4, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpawnMine");
}

static Variable SetFakeCombatState(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nEnable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetFakeCombatState");
}

static Variable GetOwnerDemolitionsSkill(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetOwnerDemolitionsSkill");
}

static Variable SetOrientOnClick(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObjectOrCaller(args, 0, ctx);
    auto nState = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetOrientOnClick");
}

static Variable GetInfluence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetInfluence");
}

static Variable SetInfluence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto nInfluence = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetInfluence");
}

static Variable ModifyInfluence(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto nModifier = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ModifyInfluence");
}

static Variable GetRacialSubType(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetRacialSubType");
}

static Variable IncrementGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto nAmount = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IncrementGlobalNumber");
}

static Variable DecrementGlobalNumber(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sIdentifier = getString(args, 0);
    auto nAmount = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DecrementGlobalNumber");
}

static Variable SetBonusForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nBonusFP = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetBonusForcePoints");
}

static Variable AddBonusForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nBonusFP = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddBonusForcePoints");
}

static Variable GetBonusForcePoints(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetBonusForcePoints");
}

static Variable IsMoviePlaying(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("IsMoviePlaying");
}

static Variable QueueMovie(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sMovie = getString(args, 0);
    auto nSkippable = getIntOrElse(args, 1, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("QueueMovie");
}

static Variable PlayMovieQueue(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nAllowSkips = getIntOrElse(args, 0, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayMovieQueue");
}

static Variable YavinHackDoorClose(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("YavinHackDoorClose");
}

static Variable IsStealthed(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsStealthed");
}

static Variable IsMeditating(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsMeditating");
}

static Variable IsInTotalDefense(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsInTotalDefense");
}

static Variable SetHealTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidHealer = getObject(args, 0, ctx);
    auto oidTarget = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetHealTarget");
}

static Variable GetHealTarget(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidHealer = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetHealTarget");
}

static Variable GetRandomDestination(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto rangeLimit = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetRandomDestination");
}

static Variable IsFormActive(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nFormID = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsFormActive");
}

static Variable GetSpellFormMask(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpellID = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSpellFormMask");
}

static Variable GetSpellBaseForcePointCost(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSpellID = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSpellBaseForcePointCost");
}

static Variable SetKeepStealthInDialog(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStealthState = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetKeepStealthInDialog");
}

static Variable HasLineOfSight(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto vSource = getVector(args, 0);
    auto vTarget = getVector(args, 1);
    auto oSource = getObjectOrNull(args, 2, ctx);
    auto oTarget = getObjectOrNull(args, 3, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("HasLineOfSight");
}

static Variable ShowDemoScreen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto sTexture = getString(args, 0);
    auto nTimeout = getInt(args, 1);
    auto nDisplayString = getInt(args, 2);
    auto nDisplayX = getInt(args, 3);
    auto nDisplayY = getInt(args, 4);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ShowDemoScreen");
}

static Variable ForceHeartbeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ForceHeartbeat");
}

static Variable IsRunning(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("IsRunning");
}

static Variable SetForfeitConditions(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nForfeitFlags = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetForfeitConditions");
}

static Variable GetLastForfeitViolation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetLastForfeitViolation");
}

static Variable ModifyReflexSavingThrowBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto aObject = getObject(args, 0, ctx);
    auto aModValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ModifyReflexSavingThrowBase");
}

static Variable ModifyFortitudeSavingThrowBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto aObject = getObject(args, 0, ctx);
    auto aModValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ModifyFortitudeSavingThrowBase");
}

static Variable ModifyWillSavingThrowBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto aObject = getObject(args, 0, ctx);
    auto aModValue = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ModifyWillSavingThrowBase");
}

static Variable GetScriptStringParameter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetScriptStringParameter");
}

static Variable GetObjectPersonalSpace(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto aObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetObjectPersonalSpace");
}

static Variable AdjustCreatureAttributes(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nAttribute = getInt(args, 1);
    auto nAmount = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AdjustCreatureAttributes");
}

static Variable SetCreatureAILevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nPriority = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCreatureAILevel");
}

static Variable ResetCreatureAILevel(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ResetCreatureAILevel");
}

static Variable AddAvailablePUPByTemplate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto sTemplate = getString(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddAvailablePUPByTemplate");
}

static Variable AddAvailablePUPByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto oPuppet = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddAvailablePUPByObject");
}

static Variable AssignPUP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto nNPC = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AssignPUP");
}

static Variable SpawnAvailablePUP(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto lLocation = getLocationArgument(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SpawnAvailablePUP");
}

static Variable AddPartyPuppet(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto oidCreature = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AddPartyPuppet");
}

static Variable GetPUPOwner(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPUP = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetPUPOwner");
}

static Variable GetIsPuppet(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPUP = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPuppet");
}

static Variable GetIsPartyLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCharacter = getObjectOrCaller(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPartyLeader");
}

static Variable GetPartyLeader(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetPartyLeader");
}

static Variable RemoveNPCFromPartyToBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveNPCFromPartyToBase");
}

static Variable CreatureFlourishWeapon(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("CreatureFlourishWeapon");
}

static Variable ChangeObjectAppearance(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObjectToChange = getObject(args, 0, ctx);
    auto nAppearance = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("ChangeObjectAppearance");
}

static Variable GetIsXBox(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("GetIsXBox");
}

static Variable PlayOverlayAnimation(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oTarget = getObject(args, 0, ctx);
    auto nAnimation = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("PlayOverlayAnimation");
}

static Variable UnlockAllSongs(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("UnlockAllSongs");
}

static Variable DisableMap(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFlag = getIntOrElse(args, 0, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisableMap");
}

static Variable DetonateMine(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oMine = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DetonateMine");
}

static Variable DisableHealthRegen(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFlag = getIntOrElse(args, 0, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisableHealthRegen");
}

static Variable SetCurrentForm(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nFormID = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetCurrentForm");
}

static Variable SetDisableTransit(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFlag = getIntOrElse(args, 0, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetDisableTransit");
}

static Variable SetInputClass(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nClass = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetInputClass");
}

static Variable SetForceAlwaysUpdate(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nFlag = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SetForceAlwaysUpdate");
}

static Variable EnableRain(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nFlag = getInt(args, 0);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EnableRain");
}

static Variable DisplayMessageBox(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nStrRef = getInt(args, 0);
    auto sIcon = getStringOrElse(args, 1, "");

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisplayMessageBox");
}

static Variable DisplayDatapad(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oDatapad = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("DisplayDatapad");
}

static Variable RemoveHeartbeat(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oPlaceable = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveHeartbeat");
}

static Variable RemoveEffectByID(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto nEffectID = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveEffectByID");
}

static Variable RemoveEffectByExactMatch(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);
    auto eEffect = getEffect(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("RemoveEffectByExactMatch");
}

static Variable AdjustCreatureSkills(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto nSkill = getInt(args, 1);
    auto nAmount = getInt(args, 2);

    // Transform

    // Execute
    throw RoutineNotImplementedException("AdjustCreatureSkills");
}

static Variable GetSkillRankBase(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nSkill = getInt(args, 0);
    auto oObject = getObjectOrCaller(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetSkillRankBase");
}

static Variable EnableRendering(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oObject = getObject(args, 0, ctx);
    auto bEnable = getInt(args, 1);

    // Transform

    // Execute
    throw RoutineNotImplementedException("EnableRendering");
}

static Variable GetCombatActionsPending(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oCreature = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetCombatActionsPending");
}

static Variable SaveNPCByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nNPC = getInt(args, 0);
    auto oidCharacter = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SaveNPCByObject");
}

static Variable SavePUPByObject(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto nPUP = getInt(args, 0);
    auto oidPuppet = getObject(args, 1, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("SavePUPByObject");
}

static Variable GetIsPlayerMadeCharacter(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Load
    auto oidCharacter = getObject(args, 0, ctx);

    // Transform

    // Execute
    throw RoutineNotImplementedException("GetIsPlayerMadeCharacter");
}

static Variable RebuildPartyTable(const std::vector<Variable> &args, const RoutineContext &ctx) {
    // Execute
    throw RoutineNotImplementedException("RebuildPartyTable");
}

} // namespace routine

void registerMainKotorRoutines(Routines &routines) {
    routines.insert(0, "Random", R_INT, {R_INT}, &routine::Random);
    routines.insert(1, "PrintString", R_VOID, {R_STRING}, &routine::PrintString);
    routines.insert(2, "PrintFloat", R_VOID, {R_FLOAT, R_INT, R_INT}, &routine::PrintFloat);
    routines.insert(3, "FloatToString", R_STRING, {R_FLOAT, R_INT, R_INT}, &routine::FloatToString);
    routines.insert(4, "PrintInteger", R_VOID, {R_INT}, &routine::PrintInteger);
    routines.insert(5, "PrintObject", R_VOID, {R_OBJECT}, &routine::PrintObject);
    routines.insert(6, "AssignCommand", R_VOID, {R_OBJECT, R_ACTION}, &routine::AssignCommand);
    routines.insert(7, "DelayCommand", R_VOID, {R_FLOAT, R_ACTION}, &routine::DelayCommand);
    routines.insert(8, "ExecuteScript", R_VOID, {R_STRING, R_OBJECT, R_INT}, &routine::ExecuteScript);
    routines.insert(9, "ClearAllActions", R_VOID, {}, &routine::ClearAllActions);
    routines.insert(10, "SetFacing", R_VOID, {R_FLOAT}, &routine::SetFacing);
    routines.insert(11, "SwitchPlayerCharacter", R_INT, {R_INT}, &routine::SwitchPlayerCharacter);
    routines.insert(12, "SetTime", R_VOID, {R_INT, R_INT, R_INT, R_INT}, &routine::SetTime);
    routines.insert(13, "SetPartyLeader", R_INT, {R_INT}, &routine::SetPartyLeader);
    routines.insert(14, "SetAreaUnescapable", R_VOID, {R_INT}, &routine::SetAreaUnescapable);
    routines.insert(15, "GetAreaUnescapable", R_INT, {}, &routine::GetAreaUnescapable);
    routines.insert(16, "GetTimeHour", R_INT, {}, &routine::GetTimeHour);
    routines.insert(17, "GetTimeMinute", R_INT, {}, &routine::GetTimeMinute);
    routines.insert(18, "GetTimeSecond", R_INT, {}, &routine::GetTimeSecond);
    routines.insert(19, "GetTimeMillisecond", R_INT, {}, &routine::GetTimeMillisecond);
    routines.insert(24, "GetArea", R_OBJECT, {R_OBJECT}, &routine::GetArea);
    routines.insert(25, "GetEnteringObject", R_OBJECT, {}, &routine::GetEnteringObject);
    routines.insert(26, "GetExitingObject", R_OBJECT, {}, &routine::GetExitingObject);
    routines.insert(27, "GetPosition", R_VECTOR, {R_OBJECT}, &routine::GetPosition);
    routines.insert(28, "GetFacing", R_FLOAT, {R_OBJECT}, &routine::GetFacing);
    routines.insert(29, "GetItemPossessor", R_OBJECT, {R_OBJECT}, &routine::GetItemPossessor);
    routines.insert(30, "GetItemPossessedBy", R_OBJECT, {R_OBJECT, R_STRING}, &routine::GetItemPossessedBy);
    routines.insert(31, "CreateItemOnObject", R_OBJECT, {R_STRING, R_OBJECT, R_INT}, &routine::CreateItemOnObject);
    routines.insert(36, "GetLastAttacker", R_OBJECT, {R_OBJECT}, &routine::GetLastAttacker);
    routines.insert(38, "GetNearestCreature", R_OBJECT, {R_INT, R_INT, R_OBJECT, R_INT, R_INT, R_INT, R_INT, R_INT}, &routine::GetNearestCreature);
    routines.insert(41, "GetDistanceToObject", R_FLOAT, {R_OBJECT}, &routine::GetDistanceToObject);
    routines.insert(42, "GetIsObjectValid", R_INT, {R_OBJECT}, &routine::GetIsObjectValid);
    routines.insert(45, "SetCameraFacing", R_VOID, {R_FLOAT}, &routine::SetCameraFacing);
    routines.insert(46, "PlaySound", R_VOID, {R_STRING}, &routine::PlaySound);
    routines.insert(47, "GetSpellTargetObject", R_OBJECT, {}, &routine::GetSpellTargetObject);
    routines.insert(49, "GetCurrentHitPoints", R_INT, {R_OBJECT}, &routine::GetCurrentHitPoints);
    routines.insert(50, "GetMaxHitPoints", R_INT, {R_OBJECT}, &routine::GetMaxHitPoints);
    routines.insert(52, "GetLastItemEquipped", R_OBJECT, {}, &routine::GetLastItemEquipped);
    routines.insert(53, "GetSubScreenID", R_INT, {}, &routine::GetSubScreenID);
    routines.insert(54, "CancelCombat", R_VOID, {R_OBJECT}, &routine::CancelCombat);
    routines.insert(55, "GetCurrentForcePoints", R_INT, {R_OBJECT}, &routine::GetCurrentForcePoints);
    routines.insert(56, "GetMaxForcePoints", R_INT, {R_OBJECT}, &routine::GetMaxForcePoints);
    routines.insert(57, "PauseGame", R_VOID, {R_INT}, &routine::PauseGame);
    routines.insert(58, "SetPlayerRestrictMode", R_VOID, {R_INT}, &routine::SetPlayerRestrictMode);
    routines.insert(59, "GetStringLength", R_INT, {R_STRING}, &routine::GetStringLength);
    routines.insert(60, "GetStringUpperCase", R_STRING, {R_STRING}, &routine::GetStringUpperCase);
    routines.insert(61, "GetStringLowerCase", R_STRING, {R_STRING}, &routine::GetStringLowerCase);
    routines.insert(62, "GetStringRight", R_STRING, {R_STRING, R_INT}, &routine::GetStringRight);
    routines.insert(63, "GetStringLeft", R_STRING, {R_STRING, R_INT}, &routine::GetStringLeft);
    routines.insert(64, "InsertString", R_STRING, {R_STRING, R_STRING, R_INT}, &routine::InsertString);
    routines.insert(65, "GetSubString", R_STRING, {R_STRING, R_INT, R_INT}, &routine::GetSubString);
    routines.insert(66, "FindSubString", R_INT, {R_STRING, R_STRING}, &routine::FindSubString);
    routines.insert(67, "fabs", R_FLOAT, {R_FLOAT}, &routine::fabs);
    routines.insert(68, "cos", R_FLOAT, {R_FLOAT}, &routine::cos);
    routines.insert(69, "sin", R_FLOAT, {R_FLOAT}, &routine::sin);
    routines.insert(70, "tan", R_FLOAT, {R_FLOAT}, &routine::tan);
    routines.insert(71, "acos", R_FLOAT, {R_FLOAT}, &routine::acos);
    routines.insert(72, "asin", R_FLOAT, {R_FLOAT}, &routine::asin);
    routines.insert(73, "atan", R_FLOAT, {R_FLOAT}, &routine::atan);
    routines.insert(74, "log", R_FLOAT, {R_FLOAT}, &routine::log);
    routines.insert(75, "pow", R_FLOAT, {R_FLOAT, R_FLOAT}, &routine::pow);
    routines.insert(76, "sqrt", R_FLOAT, {R_FLOAT}, &routine::sqrt);
    routines.insert(77, "abs", R_INT, {R_INT}, &routine::abs);
    routines.insert(83, "GetPlayerRestrictMode", R_INT, {R_OBJECT}, &routine::GetPlayerRestrictMode);
    routines.insert(84, "GetCasterLevel", R_INT, {R_OBJECT}, &routine::GetCasterLevel);
    routines.insert(85, "GetFirstEffect", R_EFFECT, {R_OBJECT}, &routine::GetFirstEffect);
    routines.insert(86, "GetNextEffect", R_EFFECT, {R_OBJECT}, &routine::GetNextEffect);
    routines.insert(87, "RemoveEffect", R_VOID, {R_OBJECT, R_EFFECT}, &routine::RemoveEffect);
    routines.insert(88, "GetIsEffectValid", R_INT, {R_EFFECT}, &routine::GetIsEffectValid);
    routines.insert(89, "GetEffectDurationType", R_INT, {R_EFFECT}, &routine::GetEffectDurationType);
    routines.insert(90, "GetEffectSubType", R_INT, {R_EFFECT}, &routine::GetEffectSubType);
    routines.insert(91, "GetEffectCreator", R_OBJECT, {R_EFFECT}, &routine::GetEffectCreator);
    routines.insert(92, "IntToString", R_STRING, {R_INT}, &routine::IntToString);
    routines.insert(93, "GetFirstObjectInArea", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFirstObjectInArea);
    routines.insert(94, "GetNextObjectInArea", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetNextObjectInArea);
    routines.insert(95, "d2", R_INT, {R_INT}, &routine::d2);
    routines.insert(96, "d3", R_INT, {R_INT}, &routine::d3);
    routines.insert(97, "d4", R_INT, {R_INT}, &routine::d4);
    routines.insert(98, "d6", R_INT, {R_INT}, &routine::d6);
    routines.insert(99, "d8", R_INT, {R_INT}, &routine::d8);
    routines.insert(100, "d10", R_INT, {R_INT}, &routine::d10);
    routines.insert(101, "d12", R_INT, {R_INT}, &routine::d12);
    routines.insert(102, "d20", R_INT, {R_INT}, &routine::d20);
    routines.insert(103, "d100", R_INT, {R_INT}, &routine::d100);
    routines.insert(104, "VectorMagnitude", R_FLOAT, {R_VECTOR}, &routine::VectorMagnitude);
    routines.insert(105, "GetMetaMagicFeat", R_INT, {}, &routine::GetMetaMagicFeat);
    routines.insert(106, "GetObjectType", R_INT, {R_OBJECT}, &routine::GetObjectType);
    routines.insert(107, "GetRacialType", R_INT, {R_OBJECT}, &routine::GetRacialType);
    routines.insert(108, "FortitudeSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &routine::FortitudeSave);
    routines.insert(109, "ReflexSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &routine::ReflexSave);
    routines.insert(110, "WillSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &routine::WillSave);
    routines.insert(111, "GetSpellSaveDC", R_INT, {}, &routine::GetSpellSaveDC);
    routines.insert(112, "MagicalEffect", R_EFFECT, {R_EFFECT}, &routine::MagicalEffect);
    routines.insert(113, "SupernaturalEffect", R_EFFECT, {R_EFFECT}, &routine::SupernaturalEffect);
    routines.insert(114, "ExtraordinaryEffect", R_EFFECT, {R_EFFECT}, &routine::ExtraordinaryEffect);
    routines.insert(116, "GetAC", R_INT, {R_OBJECT, R_INT}, &routine::GetAC);
    routines.insert(121, "RoundsToSeconds", R_FLOAT, {R_INT}, &routine::RoundsToSeconds);
    routines.insert(122, "HoursToSeconds", R_FLOAT, {R_INT}, &routine::HoursToSeconds);
    routines.insert(123, "TurnsToSeconds", R_FLOAT, {R_INT}, &routine::TurnsToSeconds);
    routines.insert(124, "SoundObjectSetFixedVariance", R_VOID, {R_OBJECT, R_FLOAT}, &routine::SoundObjectSetFixedVariance);
    routines.insert(125, "GetGoodEvilValue", R_INT, {R_OBJECT}, &routine::GetGoodEvilValue);
    routines.insert(126, "GetPartyMemberCount", R_INT, {}, &routine::GetPartyMemberCount);
    routines.insert(127, "GetAlignmentGoodEvil", R_INT, {R_OBJECT}, &routine::GetAlignmentGoodEvil);
    routines.insert(128, "GetFirstObjectInShape", R_OBJECT, {R_INT, R_FLOAT, R_LOCATION, R_INT, R_INT, R_VECTOR}, &routine::GetFirstObjectInShape);
    routines.insert(129, "GetNextObjectInShape", R_OBJECT, {R_INT, R_FLOAT, R_LOCATION, R_INT, R_INT, R_VECTOR}, &routine::GetNextObjectInShape);
    routines.insert(131, "SignalEvent", R_VOID, {R_OBJECT, R_EVENT}, &routine::SignalEvent);
    routines.insert(132, "EventUserDefined", R_EVENT, {R_INT}, &routine::EventUserDefined);
    routines.insert(137, "VectorNormalize", R_VECTOR, {R_VECTOR}, &routine::VectorNormalize);
    routines.insert(138, "GetItemStackSize", R_INT, {R_OBJECT}, &routine::GetItemStackSize);
    routines.insert(139, "GetAbilityScore", R_INT, {R_OBJECT, R_INT}, &routine::GetAbilityScore);
    routines.insert(140, "GetIsDead", R_INT, {R_OBJECT}, &routine::GetIsDead);
    routines.insert(141, "PrintVector", R_VOID, {R_VECTOR, R_INT}, &routine::PrintVector);
    routines.insert(142, "Vector", R_VECTOR, {R_FLOAT, R_FLOAT, R_FLOAT}, &routine::Vector);
    routines.insert(143, "SetFacingPoint", R_VOID, {R_VECTOR}, &routine::SetFacingPoint);
    routines.insert(144, "AngleToVector", R_VECTOR, {R_FLOAT}, &routine::AngleToVector);
    routines.insert(145, "VectorToAngle", R_FLOAT, {R_VECTOR}, &routine::VectorToAngle);
    routines.insert(146, "TouchAttackMelee", R_INT, {R_OBJECT, R_INT}, &routine::TouchAttackMelee);
    routines.insert(147, "TouchAttackRanged", R_INT, {R_OBJECT, R_INT}, &routine::TouchAttackRanged);
    routines.insert(150, "SetItemStackSize", R_VOID, {R_OBJECT, R_INT}, &routine::SetItemStackSize);
    routines.insert(151, "GetDistanceBetween", R_FLOAT, {R_OBJECT, R_OBJECT}, &routine::GetDistanceBetween);
    routines.insert(152, "SetReturnStrref", R_VOID, {R_INT, R_INT, R_INT}, &routine::SetReturnStrref);
    routines.insert(155, "GetItemInSlot", R_OBJECT, {R_INT, R_OBJECT}, &routine::GetItemInSlot);
    routines.insert(160, "SetGlobalString", R_VOID, {R_STRING, R_STRING}, &routine::SetGlobalString);
    routines.insert(162, "SetCommandable", R_VOID, {R_INT, R_OBJECT}, &routine::SetCommandable);
    routines.insert(163, "GetCommandable", R_INT, {R_OBJECT}, &routine::GetCommandable);
    routines.insert(166, "GetHitDice", R_INT, {R_OBJECT}, &routine::GetHitDice);
    routines.insert(168, "GetTag", R_STRING, {R_OBJECT}, &routine::GetTag);
    routines.insert(169, "ResistForce", R_INT, {R_OBJECT, R_OBJECT}, &routine::ResistForce);
    routines.insert(170, "GetEffectType", R_INT, {R_EFFECT}, &routine::GetEffectType);
    routines.insert(172, "GetFactionEqual", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetFactionEqual);
    routines.insert(173, "ChangeFaction", R_VOID, {R_OBJECT, R_OBJECT}, &routine::ChangeFaction);
    routines.insert(174, "GetIsListening", R_INT, {R_OBJECT}, &routine::GetIsListening);
    routines.insert(175, "SetListening", R_VOID, {R_OBJECT, R_INT}, &routine::SetListening);
    routines.insert(176, "SetListenPattern", R_VOID, {R_OBJECT, R_STRING, R_INT}, &routine::SetListenPattern);
    routines.insert(177, "TestStringAgainstPattern", R_INT, {R_STRING, R_STRING}, &routine::TestStringAgainstPattern);
    routines.insert(178, "GetMatchedSubstring", R_STRING, {R_INT}, &routine::GetMatchedSubstring);
    routines.insert(179, "GetMatchedSubstringsCount", R_INT, {}, &routine::GetMatchedSubstringsCount);
    routines.insert(181, "GetFactionWeakestMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionWeakestMember);
    routines.insert(182, "GetFactionStrongestMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionStrongestMember);
    routines.insert(183, "GetFactionMostDamagedMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionMostDamagedMember);
    routines.insert(184, "GetFactionLeastDamagedMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionLeastDamagedMember);
    routines.insert(185, "GetFactionGold", R_INT, {R_OBJECT}, &routine::GetFactionGold);
    routines.insert(186, "GetFactionAverageReputation", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetFactionAverageReputation);
    routines.insert(187, "GetFactionAverageGoodEvilAlignment", R_INT, {R_OBJECT}, &routine::GetFactionAverageGoodEvilAlignment);
    routines.insert(188, "SoundObjectGetFixedVariance", R_FLOAT, {R_OBJECT}, &routine::SoundObjectGetFixedVariance);
    routines.insert(189, "GetFactionAverageLevel", R_INT, {R_OBJECT}, &routine::GetFactionAverageLevel);
    routines.insert(190, "GetFactionAverageXP", R_INT, {R_OBJECT}, &routine::GetFactionAverageXP);
    routines.insert(191, "GetFactionMostFrequentClass", R_INT, {R_OBJECT}, &routine::GetFactionMostFrequentClass);
    routines.insert(192, "GetFactionWorstAC", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionWorstAC);
    routines.insert(193, "GetFactionBestAC", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionBestAC);
    routines.insert(194, "GetGlobalString", R_STRING, {R_STRING}, &routine::GetGlobalString);
    routines.insert(195, "GetListenPatternNumber", R_INT, {}, &routine::GetListenPatternNumber);
    routines.insert(197, "GetWaypointByTag", R_OBJECT, {R_STRING}, &routine::GetWaypointByTag);
    routines.insert(198, "GetTransitionTarget", R_OBJECT, {R_OBJECT}, &routine::GetTransitionTarget);
    routines.insert(200, "GetObjectByTag", R_OBJECT, {R_STRING, R_INT}, &routine::GetObjectByTag);
    routines.insert(201, "AdjustAlignment", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::AdjustAlignment);
    routines.insert(203, "SetAreaTransitionBMP", R_VOID, {R_INT, R_STRING}, &routine::SetAreaTransitionBMP);
    routines.insert(208, "GetReputation", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetReputation);
    routines.insert(209, "AdjustReputation", R_VOID, {R_OBJECT, R_OBJECT, R_INT}, &routine::AdjustReputation);
    routines.insert(210, "GetModuleFileName", R_STRING, {}, &routine::GetModuleFileName);
    routines.insert(211, "GetGoingToBeAttackedBy", R_OBJECT, {R_OBJECT}, &routine::GetGoingToBeAttackedBy);
    routines.insert(213, "GetLocation", R_LOCATION, {R_OBJECT}, &routine::GetLocation);
    routines.insert(215, "Location", R_LOCATION, {R_VECTOR, R_FLOAT}, &routine::Location);
    routines.insert(216, "ApplyEffectAtLocation", R_VOID, {R_INT, R_EFFECT, R_LOCATION, R_FLOAT}, &routine::ApplyEffectAtLocation);
    routines.insert(217, "GetIsPC", R_INT, {R_OBJECT}, &routine::GetIsPC);
    routines.insert(218, "FeetToMeters", R_FLOAT, {R_FLOAT}, &routine::FeetToMeters);
    routines.insert(219, "YardsToMeters", R_FLOAT, {R_FLOAT}, &routine::YardsToMeters);
    routines.insert(220, "ApplyEffectToObject", R_VOID, {R_INT, R_EFFECT, R_OBJECT, R_FLOAT}, &routine::ApplyEffectToObject);
    routines.insert(221, "SpeakString", R_VOID, {R_STRING, R_INT}, &routine::SpeakString);
    routines.insert(222, "GetSpellTargetLocation", R_LOCATION, {}, &routine::GetSpellTargetLocation);
    routines.insert(223, "GetPositionFromLocation", R_VECTOR, {R_LOCATION}, &routine::GetPositionFromLocation);
    routines.insert(225, "GetFacingFromLocation", R_FLOAT, {R_LOCATION}, &routine::GetFacingFromLocation);
    routines.insert(226, "GetNearestCreatureToLocation", R_OBJECT, {R_INT, R_INT, R_LOCATION, R_INT, R_INT, R_INT, R_INT, R_INT}, &routine::GetNearestCreatureToLocation);
    routines.insert(227, "GetNearestObject", R_OBJECT, {R_INT, R_OBJECT, R_INT}, &routine::GetNearestObject);
    routines.insert(228, "GetNearestObjectToLocation", R_OBJECT, {R_INT, R_LOCATION, R_INT}, &routine::GetNearestObjectToLocation);
    routines.insert(229, "GetNearestObjectByTag", R_OBJECT, {R_STRING, R_OBJECT, R_INT}, &routine::GetNearestObjectByTag);
    routines.insert(230, "IntToFloat", R_FLOAT, {R_INT}, &routine::IntToFloat);
    routines.insert(231, "FloatToInt", R_INT, {R_FLOAT}, &routine::FloatToInt);
    routines.insert(232, "StringToInt", R_INT, {R_STRING}, &routine::StringToInt);
    routines.insert(233, "StringToFloat", R_FLOAT, {R_STRING}, &routine::StringToFloat);
    routines.insert(235, "GetIsEnemy", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetIsEnemy);
    routines.insert(236, "GetIsFriend", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetIsFriend);
    routines.insert(237, "GetIsNeutral", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetIsNeutral);
    routines.insert(238, "GetPCSpeaker", R_OBJECT, {}, &routine::GetPCSpeaker);
    routines.insert(239, "GetStringByStrRef", R_STRING, {R_INT}, &routine::GetStringByStrRef);
    routines.insert(241, "DestroyObject", R_VOID, {R_OBJECT, R_FLOAT, R_INT, R_FLOAT}, &routine::DestroyObject);
    routines.insert(242, "GetModule", R_OBJECT, {}, &routine::GetModule);
    routines.insert(243, "CreateObject", R_OBJECT, {R_INT, R_STRING, R_LOCATION, R_INT}, &routine::CreateObject);
    routines.insert(244, "EventSpellCastAt", R_EVENT, {R_OBJECT, R_INT, R_INT}, &routine::EventSpellCastAt);
    routines.insert(245, "GetLastSpellCaster", R_OBJECT, {}, &routine::GetLastSpellCaster);
    routines.insert(246, "GetLastSpell", R_INT, {}, &routine::GetLastSpell);
    routines.insert(247, "GetUserDefinedEventNumber", R_INT, {}, &routine::GetUserDefinedEventNumber);
    routines.insert(248, "GetSpellId", R_INT, {}, &routine::GetSpellId);
    routines.insert(249, "RandomName", R_STRING, {}, &routine::RandomName);
    routines.insert(251, "GetLoadFromSaveGame", R_INT, {}, &routine::GetLoadFromSaveGame);
    routines.insert(253, "GetName", R_STRING, {R_OBJECT}, &routine::GetName);
    routines.insert(254, "GetLastSpeaker", R_OBJECT, {}, &routine::GetLastSpeaker);
    routines.insert(255, "BeginConversation", R_INT, {R_STRING, R_OBJECT}, &routine::BeginConversation);
    routines.insert(256, "GetLastPerceived", R_OBJECT, {}, &routine::GetLastPerceived);
    routines.insert(257, "GetLastPerceptionHeard", R_INT, {}, &routine::GetLastPerceptionHeard);
    routines.insert(258, "GetLastPerceptionInaudible", R_INT, {}, &routine::GetLastPerceptionInaudible);
    routines.insert(259, "GetLastPerceptionSeen", R_INT, {}, &routine::GetLastPerceptionSeen);
    routines.insert(260, "GetLastClosedBy", R_OBJECT, {}, &routine::GetLastClosedBy);
    routines.insert(261, "GetLastPerceptionVanished", R_INT, {}, &routine::GetLastPerceptionVanished);
    routines.insert(262, "GetFirstInPersistentObject", R_OBJECT, {R_OBJECT, R_INT, R_INT}, &routine::GetFirstInPersistentObject);
    routines.insert(263, "GetNextInPersistentObject", R_OBJECT, {R_OBJECT, R_INT, R_INT}, &routine::GetNextInPersistentObject);
    routines.insert(264, "GetAreaOfEffectCreator", R_OBJECT, {R_OBJECT}, &routine::GetAreaOfEffectCreator);
    routines.insert(265, "ShowLevelUpGUI", R_INT, {}, &routine::ShowLevelUpGUI);
    routines.insert(266, "SetItemNonEquippable", R_VOID, {R_OBJECT, R_INT}, &routine::SetItemNonEquippable);
    routines.insert(267, "GetButtonMashCheck", R_INT, {}, &routine::GetButtonMashCheck);
    routines.insert(268, "SetButtonMashCheck", R_VOID, {R_INT}, &routine::SetButtonMashCheck);
    routines.insert(271, "GiveItem", R_VOID, {R_OBJECT, R_OBJECT}, &routine::GiveItem);
    routines.insert(272, "ObjectToString", R_STRING, {R_OBJECT}, &routine::ObjectToString);
    routines.insert(274, "GetIsImmune", R_INT, {R_OBJECT, R_INT, R_OBJECT}, &routine::GetIsImmune);
    routines.insert(276, "GetEncounterActive", R_INT, {R_OBJECT}, &routine::GetEncounterActive);
    routines.insert(277, "SetEncounterActive", R_VOID, {R_INT, R_OBJECT}, &routine::SetEncounterActive);
    routines.insert(278, "GetEncounterSpawnsMax", R_INT, {R_OBJECT}, &routine::GetEncounterSpawnsMax);
    routines.insert(279, "SetEncounterSpawnsMax", R_VOID, {R_INT, R_OBJECT}, &routine::SetEncounterSpawnsMax);
    routines.insert(280, "GetEncounterSpawnsCurrent", R_INT, {R_OBJECT}, &routine::GetEncounterSpawnsCurrent);
    routines.insert(281, "SetEncounterSpawnsCurrent", R_VOID, {R_INT, R_OBJECT}, &routine::SetEncounterSpawnsCurrent);
    routines.insert(282, "GetModuleItemAcquired", R_OBJECT, {}, &routine::GetModuleItemAcquired);
    routines.insert(283, "GetModuleItemAcquiredFrom", R_OBJECT, {}, &routine::GetModuleItemAcquiredFrom);
    routines.insert(284, "SetCustomToken", R_VOID, {R_INT, R_STRING}, &routine::SetCustomToken);
    routines.insert(285, "GetHasFeat", R_INT, {R_INT, R_OBJECT}, &routine::GetHasFeat);
    routines.insert(286, "GetHasSkill", R_INT, {R_INT, R_OBJECT}, &routine::GetHasSkill);
    routines.insert(289, "GetObjectSeen", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetObjectSeen);
    routines.insert(290, "GetObjectHeard", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetObjectHeard);
    routines.insert(291, "GetLastPlayerDied", R_OBJECT, {}, &routine::GetLastPlayerDied);
    routines.insert(292, "GetModuleItemLost", R_OBJECT, {}, &routine::GetModuleItemLost);
    routines.insert(293, "GetModuleItemLostBy", R_OBJECT, {}, &routine::GetModuleItemLostBy);
    routines.insert(295, "EventConversation", R_EVENT, {}, &routine::EventConversation);
    routines.insert(296, "SetEncounterDifficulty", R_VOID, {R_INT, R_OBJECT}, &routine::SetEncounterDifficulty);
    routines.insert(297, "GetEncounterDifficulty", R_INT, {R_OBJECT}, &routine::GetEncounterDifficulty);
    routines.insert(298, "GetDistanceBetweenLocations", R_FLOAT, {R_LOCATION, R_LOCATION}, &routine::GetDistanceBetweenLocations);
    routines.insert(299, "GetReflexAdjustedDamage", R_INT, {R_INT, R_OBJECT, R_INT, R_INT, R_OBJECT}, &routine::GetReflexAdjustedDamage);
    routines.insert(300, "PlayAnimation", R_VOID, {R_INT, R_FLOAT, R_FLOAT}, &routine::PlayAnimation);
    routines.insert(301, "TalentSpell", R_TALENT, {R_INT}, &routine::TalentSpell);
    routines.insert(302, "TalentFeat", R_TALENT, {R_INT}, &routine::TalentFeat);
    routines.insert(303, "TalentSkill", R_TALENT, {R_INT}, &routine::TalentSkill);
    routines.insert(304, "GetHasSpellEffect", R_INT, {R_INT, R_OBJECT}, &routine::GetHasSpellEffect);
    routines.insert(305, "GetEffectSpellId", R_INT, {R_EFFECT}, &routine::GetEffectSpellId);
    routines.insert(306, "GetCreatureHasTalent", R_INT, {R_TALENT, R_OBJECT}, &routine::GetCreatureHasTalent);
    routines.insert(307, "GetCreatureTalentRandom", R_TALENT, {R_INT, R_OBJECT, R_INT}, &routine::GetCreatureTalentRandom);
    routines.insert(308, "GetCreatureTalentBest", R_TALENT, {R_INT, R_INT, R_OBJECT, R_INT, R_INT, R_INT}, &routine::GetCreatureTalentBest);
    routines.insert(311, "GetGoldPieceValue", R_INT, {R_OBJECT}, &routine::GetGoldPieceValue);
    routines.insert(312, "GetIsPlayableRacialType", R_INT, {R_OBJECT}, &routine::GetIsPlayableRacialType);
    routines.insert(313, "JumpToLocation", R_VOID, {R_LOCATION}, &routine::JumpToLocation);
    routines.insert(315, "GetSkillRank", R_INT, {R_INT, R_OBJECT}, &routine::GetSkillRank);
    routines.insert(316, "GetAttackTarget", R_OBJECT, {R_OBJECT}, &routine::GetAttackTarget);
    routines.insert(317, "GetLastAttackType", R_INT, {R_OBJECT}, &routine::GetLastAttackType);
    routines.insert(318, "GetLastAttackMode", R_INT, {R_OBJECT}, &routine::GetLastAttackMode);
    routines.insert(319, "GetDistanceBetween2D", R_FLOAT, {R_OBJECT, R_OBJECT}, &routine::GetDistanceBetween2D);
    routines.insert(320, "GetIsInCombat", R_INT, {R_OBJECT}, &routine::GetIsInCombat);
    routines.insert(321, "GetLastAssociateCommand", R_INT, {R_OBJECT}, &routine::GetLastAssociateCommand);
    routines.insert(322, "GiveGoldToCreature", R_VOID, {R_OBJECT, R_INT}, &routine::GiveGoldToCreature);
    routines.insert(323, "SetIsDestroyable", R_VOID, {R_INT, R_INT, R_INT}, &routine::SetIsDestroyable);
    routines.insert(324, "SetLocked", R_VOID, {R_OBJECT, R_INT}, &routine::SetLocked);
    routines.insert(325, "GetLocked", R_INT, {R_OBJECT}, &routine::GetLocked);
    routines.insert(326, "GetClickingObject", R_OBJECT, {}, &routine::GetClickingObject);
    routines.insert(327, "SetAssociateListenPatterns", R_VOID, {R_OBJECT}, &routine::SetAssociateListenPatterns);
    routines.insert(328, "GetLastWeaponUsed", R_OBJECT, {R_OBJECT}, &routine::GetLastWeaponUsed);
    routines.insert(330, "GetLastUsedBy", R_OBJECT, {}, &routine::GetLastUsedBy);
    routines.insert(331, "GetAbilityModifier", R_INT, {R_INT, R_OBJECT}, &routine::GetAbilityModifier);
    routines.insert(332, "GetIdentified", R_INT, {R_OBJECT}, &routine::GetIdentified);
    routines.insert(333, "SetIdentified", R_VOID, {R_OBJECT, R_INT}, &routine::SetIdentified);
    routines.insert(334, "GetDistanceBetweenLocations2D", R_FLOAT, {R_LOCATION, R_LOCATION}, &routine::GetDistanceBetweenLocations2D);
    routines.insert(335, "GetDistanceToObject2D", R_FLOAT, {R_OBJECT}, &routine::GetDistanceToObject2D);
    routines.insert(336, "GetBlockingDoor", R_OBJECT, {}, &routine::GetBlockingDoor);
    routines.insert(337, "GetIsDoorActionPossible", R_INT, {R_OBJECT, R_INT}, &routine::GetIsDoorActionPossible);
    routines.insert(338, "DoDoorAction", R_VOID, {R_OBJECT, R_INT}, &routine::DoDoorAction);
    routines.insert(339, "GetFirstItemInInventory", R_OBJECT, {R_OBJECT}, &routine::GetFirstItemInInventory);
    routines.insert(340, "GetNextItemInInventory", R_OBJECT, {R_OBJECT}, &routine::GetNextItemInInventory);
    routines.insert(341, "GetClassByPosition", R_INT, {R_INT, R_OBJECT}, &routine::GetClassByPosition);
    routines.insert(342, "GetLevelByPosition", R_INT, {R_INT, R_OBJECT}, &routine::GetLevelByPosition);
    routines.insert(343, "GetLevelByClass", R_INT, {R_INT, R_OBJECT}, &routine::GetLevelByClass);
    routines.insert(344, "GetDamageDealtByType", R_INT, {R_INT}, &routine::GetDamageDealtByType);
    routines.insert(345, "GetTotalDamageDealt", R_INT, {}, &routine::GetTotalDamageDealt);
    routines.insert(346, "GetLastDamager", R_OBJECT, {}, &routine::GetLastDamager);
    routines.insert(347, "GetLastDisarmed", R_OBJECT, {}, &routine::GetLastDisarmed);
    routines.insert(348, "GetLastDisturbed", R_OBJECT, {}, &routine::GetLastDisturbed);
    routines.insert(349, "GetLastLocked", R_OBJECT, {}, &routine::GetLastLocked);
    routines.insert(350, "GetLastUnlocked", R_OBJECT, {}, &routine::GetLastUnlocked);
    routines.insert(352, "GetInventoryDisturbType", R_INT, {}, &routine::GetInventoryDisturbType);
    routines.insert(353, "GetInventoryDisturbItem", R_OBJECT, {}, &routine::GetInventoryDisturbItem);
    routines.insert(354, "ShowUpgradeScreen", R_VOID, {R_OBJECT}, &routine::ShowUpgradeScreen);
    routines.insert(355, "VersusAlignmentEffect", R_EFFECT, {R_EFFECT, R_INT, R_INT}, &routine::VersusAlignmentEffect);
    routines.insert(356, "VersusRacialTypeEffect", R_EFFECT, {R_EFFECT, R_INT}, &routine::VersusRacialTypeEffect);
    routines.insert(357, "VersusTrapEffect", R_EFFECT, {R_EFFECT}, &routine::VersusTrapEffect);
    routines.insert(358, "GetGender", R_INT, {R_OBJECT}, &routine::GetGender);
    routines.insert(359, "GetIsTalentValid", R_INT, {R_TALENT}, &routine::GetIsTalentValid);
    routines.insert(361, "GetAttemptedAttackTarget", R_OBJECT, {}, &routine::GetAttemptedAttackTarget);
    routines.insert(362, "GetTypeFromTalent", R_INT, {R_TALENT}, &routine::GetTypeFromTalent);
    routines.insert(363, "GetIdFromTalent", R_INT, {R_TALENT}, &routine::GetIdFromTalent);
    routines.insert(364, "PlayPazaak", R_VOID, {R_INT, R_STRING, R_INT, R_INT, R_OBJECT}, &routine::PlayPazaak);
    routines.insert(365, "GetLastPazaakResult", R_INT, {}, &routine::GetLastPazaakResult);
    routines.insert(366, "DisplayFeedBackText", R_VOID, {R_OBJECT, R_INT}, &routine::DisplayFeedBackText);
    routines.insert(367, "AddJournalQuestEntry", R_VOID, {R_STRING, R_INT, R_INT}, &routine::AddJournalQuestEntry);
    routines.insert(368, "RemoveJournalQuestEntry", R_VOID, {R_STRING}, &routine::RemoveJournalQuestEntry);
    routines.insert(369, "GetJournalEntry", R_INT, {R_STRING}, &routine::GetJournalEntry);
    routines.insert(370, "PlayRumblePattern", R_INT, {R_INT}, &routine::PlayRumblePattern);
    routines.insert(371, "StopRumblePattern", R_INT, {R_INT}, &routine::StopRumblePattern);
    routines.insert(374, "SendMessageToPC", R_VOID, {R_OBJECT, R_STRING}, &routine::SendMessageToPC);
    routines.insert(375, "GetAttemptedSpellTarget", R_OBJECT, {}, &routine::GetAttemptedSpellTarget);
    routines.insert(376, "GetLastOpenedBy", R_OBJECT, {}, &routine::GetLastOpenedBy);
    routines.insert(377, "GetHasSpell", R_INT, {R_INT, R_OBJECT}, &routine::GetHasSpell);
    routines.insert(378, "OpenStore", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT}, &routine::OpenStore);
    routines.insert(380, "GetFirstFactionMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFirstFactionMember);
    routines.insert(381, "GetNextFactionMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetNextFactionMember);
    routines.insert(384, "GetJournalQuestExperience", R_INT, {R_STRING}, &routine::GetJournalQuestExperience);
    routines.insert(385, "JumpToObject", R_VOID, {R_OBJECT, R_INT}, &routine::JumpToObject);
    routines.insert(386, "SetMapPinEnabled", R_VOID, {R_OBJECT, R_INT}, &routine::SetMapPinEnabled);
    routines.insert(388, "PopUpGUIPanel", R_VOID, {R_OBJECT, R_INT}, &routine::PopUpGUIPanel);
    routines.insert(389, "AddMultiClass", R_VOID, {R_INT, R_OBJECT}, &routine::AddMultiClass);
    routines.insert(390, "GetIsLinkImmune", R_INT, {R_OBJECT, R_EFFECT}, &routine::GetIsLinkImmune);
    routines.insert(393, "GiveXPToCreature", R_VOID, {R_OBJECT, R_INT}, &routine::GiveXPToCreature);
    routines.insert(394, "SetXP", R_VOID, {R_OBJECT, R_INT}, &routine::SetXP);
    routines.insert(395, "GetXP", R_INT, {R_OBJECT}, &routine::GetXP);
    routines.insert(396, "IntToHexString", R_STRING, {R_INT}, &routine::IntToHexString);
    routines.insert(397, "GetBaseItemType", R_INT, {R_OBJECT}, &routine::GetBaseItemType);
    routines.insert(398, "GetItemHasItemProperty", R_INT, {R_OBJECT, R_INT}, &routine::GetItemHasItemProperty);
    routines.insert(401, "GetItemACValue", R_INT, {R_OBJECT}, &routine::GetItemACValue);
    routines.insert(403, "ExploreAreaForPlayer", R_VOID, {R_OBJECT, R_OBJECT}, &routine::ExploreAreaForPlayer);
    routines.insert(405, "GetIsDay", R_INT, {}, &routine::GetIsDay);
    routines.insert(406, "GetIsNight", R_INT, {}, &routine::GetIsNight);
    routines.insert(407, "GetIsDawn", R_INT, {}, &routine::GetIsDawn);
    routines.insert(408, "GetIsDusk", R_INT, {}, &routine::GetIsDusk);
    routines.insert(409, "GetIsEncounterCreature", R_INT, {R_OBJECT}, &routine::GetIsEncounterCreature);
    routines.insert(410, "GetLastPlayerDying", R_OBJECT, {}, &routine::GetLastPlayerDying);
    routines.insert(411, "GetStartingLocation", R_LOCATION, {}, &routine::GetStartingLocation);
    routines.insert(412, "ChangeToStandardFaction", R_VOID, {R_OBJECT, R_INT}, &routine::ChangeToStandardFaction);
    routines.insert(413, "SoundObjectPlay", R_VOID, {R_OBJECT}, &routine::SoundObjectPlay);
    routines.insert(414, "SoundObjectStop", R_VOID, {R_OBJECT}, &routine::SoundObjectStop);
    routines.insert(415, "SoundObjectSetVolume", R_VOID, {R_OBJECT, R_INT}, &routine::SoundObjectSetVolume);
    routines.insert(416, "SoundObjectSetPosition", R_VOID, {R_OBJECT, R_VECTOR}, &routine::SoundObjectSetPosition);
    routines.insert(417, "SpeakOneLinerConversation", R_VOID, {R_STRING, R_OBJECT}, &routine::SpeakOneLinerConversation);
    routines.insert(418, "GetGold", R_INT, {R_OBJECT}, &routine::GetGold);
    routines.insert(419, "GetLastRespawnButtonPresser", R_OBJECT, {}, &routine::GetLastRespawnButtonPresser);
    routines.insert(421, "SetLightsaberPowered", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &routine::SetLightsaberPowered);
    routines.insert(422, "GetIsWeaponEffective", R_INT, {R_OBJECT, R_INT}, &routine::GetIsWeaponEffective);
    routines.insert(423, "GetLastSpellHarmful", R_INT, {}, &routine::GetLastSpellHarmful);
    routines.insert(424, "EventActivateItem", R_EVENT, {R_OBJECT, R_LOCATION, R_OBJECT}, &routine::EventActivateItem);
    routines.insert(425, "MusicBackgroundPlay", R_VOID, {R_OBJECT}, &routine::MusicBackgroundPlay);
    routines.insert(426, "MusicBackgroundStop", R_VOID, {R_OBJECT}, &routine::MusicBackgroundStop);
    routines.insert(427, "MusicBackgroundSetDelay", R_VOID, {R_OBJECT, R_INT}, &routine::MusicBackgroundSetDelay);
    routines.insert(428, "MusicBackgroundChangeDay", R_VOID, {R_OBJECT, R_INT}, &routine::MusicBackgroundChangeDay);
    routines.insert(429, "MusicBackgroundChangeNight", R_VOID, {R_OBJECT, R_INT}, &routine::MusicBackgroundChangeNight);
    routines.insert(430, "MusicBattlePlay", R_VOID, {R_OBJECT}, &routine::MusicBattlePlay);
    routines.insert(431, "MusicBattleStop", R_VOID, {R_OBJECT}, &routine::MusicBattleStop);
    routines.insert(432, "MusicBattleChange", R_VOID, {R_OBJECT, R_INT}, &routine::MusicBattleChange);
    routines.insert(433, "AmbientSoundPlay", R_VOID, {R_OBJECT}, &routine::AmbientSoundPlay);
    routines.insert(434, "AmbientSoundStop", R_VOID, {R_OBJECT}, &routine::AmbientSoundStop);
    routines.insert(435, "AmbientSoundChangeDay", R_VOID, {R_OBJECT, R_INT}, &routine::AmbientSoundChangeDay);
    routines.insert(436, "AmbientSoundChangeNight", R_VOID, {R_OBJECT, R_INT}, &routine::AmbientSoundChangeNight);
    routines.insert(437, "GetLastKiller", R_OBJECT, {}, &routine::GetLastKiller);
    routines.insert(438, "GetSpellCastItem", R_OBJECT, {}, &routine::GetSpellCastItem);
    routines.insert(439, "GetItemActivated", R_OBJECT, {}, &routine::GetItemActivated);
    routines.insert(440, "GetItemActivator", R_OBJECT, {}, &routine::GetItemActivator);
    routines.insert(441, "GetItemActivatedTargetLocation", R_LOCATION, {}, &routine::GetItemActivatedTargetLocation);
    routines.insert(442, "GetItemActivatedTarget", R_OBJECT, {}, &routine::GetItemActivatedTarget);
    routines.insert(443, "GetIsOpen", R_INT, {R_OBJECT}, &routine::GetIsOpen);
    routines.insert(444, "TakeGoldFromCreature", R_VOID, {R_INT, R_OBJECT, R_INT}, &routine::TakeGoldFromCreature);
    routines.insert(445, "GetIsInConversation", R_INT, {R_OBJECT}, &routine::GetIsInConversation);
    routines.insert(455, "GetPlotFlag", R_INT, {R_OBJECT}, &routine::GetPlotFlag);
    routines.insert(456, "SetPlotFlag", R_VOID, {R_OBJECT, R_INT}, &routine::SetPlotFlag);
    routines.insert(461, "SetDialogPlaceableCamera", R_VOID, {R_INT}, &routine::SetDialogPlaceableCamera);
    routines.insert(462, "GetSoloMode", R_INT, {}, &routine::GetSoloMode);
    routines.insert(464, "GetMaxStealthXP", R_INT, {}, &routine::GetMaxStealthXP);
    routines.insert(468, "SetMaxStealthXP", R_VOID, {R_INT}, &routine::SetMaxStealthXP);
    routines.insert(474, "GetCurrentStealthXP", R_INT, {}, &routine::GetCurrentStealthXP);
    routines.insert(475, "GetNumStackedItems", R_INT, {R_OBJECT}, &routine::GetNumStackedItems);
    routines.insert(476, "SurrenderToEnemies", R_VOID, {}, &routine::SurrenderToEnemies);
    routines.insert(478, "SetCurrentStealthXP", R_VOID, {R_INT}, &routine::SetCurrentStealthXP);
    routines.insert(479, "GetCreatureSize", R_INT, {R_OBJECT}, &routine::GetCreatureSize);
    routines.insert(480, "AwardStealthXP", R_VOID, {R_OBJECT}, &routine::AwardStealthXP);
    routines.insert(481, "GetStealthXPEnabled", R_INT, {}, &routine::GetStealthXPEnabled);
    routines.insert(482, "SetStealthXPEnabled", R_VOID, {R_INT}, &routine::SetStealthXPEnabled);
    routines.insert(486, "GetLastTrapDetected", R_OBJECT, {R_OBJECT}, &routine::GetLastTrapDetected);
    routines.insert(488, "GetNearestTrapToObject", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetNearestTrapToObject);
    routines.insert(489, "GetAttemptedMovementTarget", R_OBJECT, {}, &routine::GetAttemptedMovementTarget);
    routines.insert(490, "GetBlockingCreature", R_OBJECT, {R_OBJECT}, &routine::GetBlockingCreature);
    routines.insert(491, "GetFortitudeSavingThrow", R_INT, {R_OBJECT}, &routine::GetFortitudeSavingThrow);
    routines.insert(492, "GetWillSavingThrow", R_INT, {R_OBJECT}, &routine::GetWillSavingThrow);
    routines.insert(493, "GetReflexSavingThrow", R_INT, {R_OBJECT}, &routine::GetReflexSavingThrow);
    routines.insert(494, "GetChallengeRating", R_FLOAT, {R_OBJECT}, &routine::GetChallengeRating);
    routines.insert(495, "GetFoundEnemyCreature", R_OBJECT, {R_OBJECT}, &routine::GetFoundEnemyCreature);
    routines.insert(496, "GetMovementRate", R_INT, {R_OBJECT}, &routine::GetMovementRate);
    routines.insert(497, "GetSubRace", R_INT, {R_OBJECT}, &routine::GetSubRace);
    routines.insert(498, "GetStealthXPDecrement", R_INT, {}, &routine::GetStealthXPDecrement);
    routines.insert(499, "SetStealthXPDecrement", R_VOID, {R_INT}, &routine::SetStealthXPDecrement);
    routines.insert(500, "DuplicateHeadAppearance", R_VOID, {R_OBJECT, R_OBJECT}, &routine::DuplicateHeadAppearance);
    routines.insert(503, "CutsceneAttack", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &routine::CutsceneAttack);
    routines.insert(504, "SetCameraMode", R_VOID, {R_OBJECT, R_INT}, &routine::SetCameraMode);
    routines.insert(505, "SetLockOrientationInDialog", R_VOID, {R_OBJECT, R_INT}, &routine::SetLockOrientationInDialog);
    routines.insert(506, "SetLockHeadFollowInDialog", R_VOID, {R_OBJECT, R_INT}, &routine::SetLockHeadFollowInDialog);
    routines.insert(507, "CutsceneMove", R_VOID, {R_OBJECT, R_VECTOR, R_INT}, &routine::CutsceneMove);
    routines.insert(508, "EnableVideoEffect", R_VOID, {R_INT}, &routine::EnableVideoEffect);
    routines.insert(509, "StartNewModule", R_VOID, {R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING}, &routine::StartNewModule);
    routines.insert(510, "DisableVideoEffect", R_VOID, {}, &routine::DisableVideoEffect);
    routines.insert(511, "GetWeaponRanged", R_INT, {R_OBJECT}, &routine::GetWeaponRanged);
    routines.insert(512, "DoSinglePlayerAutoSave", R_VOID, {}, &routine::DoSinglePlayerAutoSave);
    routines.insert(513, "GetGameDifficulty", R_INT, {}, &routine::GetGameDifficulty);
    routines.insert(514, "GetUserActionsPending", R_INT, {}, &routine::GetUserActionsPending);
    routines.insert(515, "RevealMap", R_VOID, {R_VECTOR, R_INT}, &routine::RevealMap);
    routines.insert(516, "SetTutorialWindowsEnabled", R_VOID, {R_INT}, &routine::SetTutorialWindowsEnabled);
    routines.insert(517, "ShowTutorialWindow", R_VOID, {R_INT}, &routine::ShowTutorialWindow);
    routines.insert(518, "StartCreditSequence", R_VOID, {R_INT}, &routine::StartCreditSequence);
    routines.insert(519, "IsCreditSequenceInProgress", R_INT, {}, &routine::IsCreditSequenceInProgress);
    routines.insert(522, "GetCurrentAction", R_INT, {R_OBJECT}, &routine::GetCurrentAction);
    routines.insert(523, "GetDifficultyModifier", R_FLOAT, {}, &routine::GetDifficultyModifier);
    routines.insert(524, "GetAppearanceType", R_INT, {R_OBJECT}, &routine::GetAppearanceType);
    routines.insert(525, "FloatingTextStrRefOnCreature", R_VOID, {R_INT, R_OBJECT, R_INT}, &routine::FloatingTextStrRefOnCreature);
    routines.insert(526, "FloatingTextStringOnCreature", R_VOID, {R_STRING, R_OBJECT, R_INT}, &routine::FloatingTextStringOnCreature);
    routines.insert(527, "GetTrapDisarmable", R_INT, {R_OBJECT}, &routine::GetTrapDisarmable);
    routines.insert(528, "GetTrapDetectable", R_INT, {R_OBJECT}, &routine::GetTrapDetectable);
    routines.insert(529, "GetTrapDetectedBy", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetTrapDetectedBy);
    routines.insert(530, "GetTrapFlagged", R_INT, {R_OBJECT}, &routine::GetTrapFlagged);
    routines.insert(531, "GetTrapBaseType", R_INT, {R_OBJECT}, &routine::GetTrapBaseType);
    routines.insert(532, "GetTrapOneShot", R_INT, {R_OBJECT}, &routine::GetTrapOneShot);
    routines.insert(533, "GetTrapCreator", R_OBJECT, {R_OBJECT}, &routine::GetTrapCreator);
    routines.insert(534, "GetTrapKeyTag", R_STRING, {R_OBJECT}, &routine::GetTrapKeyTag);
    routines.insert(535, "GetTrapDisarmDC", R_INT, {R_OBJECT}, &routine::GetTrapDisarmDC);
    routines.insert(536, "GetTrapDetectDC", R_INT, {R_OBJECT}, &routine::GetTrapDetectDC);
    routines.insert(537, "GetLockKeyRequired", R_INT, {R_OBJECT}, &routine::GetLockKeyRequired);
    routines.insert(538, "GetLockKeyTag", R_INT, {R_OBJECT}, &routine::GetLockKeyTag);
    routines.insert(539, "GetLockLockable", R_INT, {R_OBJECT}, &routine::GetLockLockable);
    routines.insert(540, "GetLockUnlockDC", R_INT, {R_OBJECT}, &routine::GetLockUnlockDC);
    routines.insert(541, "GetLockLockDC", R_INT, {R_OBJECT}, &routine::GetLockLockDC);
    routines.insert(542, "GetPCLevellingUp", R_OBJECT, {}, &routine::GetPCLevellingUp);
    routines.insert(543, "GetHasFeatEffect", R_INT, {R_INT, R_OBJECT}, &routine::GetHasFeatEffect);
    routines.insert(544, "SetPlaceableIllumination", R_VOID, {R_OBJECT, R_INT}, &routine::SetPlaceableIllumination);
    routines.insert(545, "GetPlaceableIllumination", R_INT, {R_OBJECT}, &routine::GetPlaceableIllumination);
    routines.insert(546, "GetIsPlaceableObjectActionPossible", R_INT, {R_OBJECT, R_INT}, &routine::GetIsPlaceableObjectActionPossible);
    routines.insert(547, "DoPlaceableObjectAction", R_VOID, {R_OBJECT, R_INT}, &routine::DoPlaceableObjectAction);
    routines.insert(548, "GetFirstPC", R_OBJECT, {}, &routine::GetFirstPC);
    routines.insert(549, "GetNextPC", R_OBJECT, {}, &routine::GetNextPC);
    routines.insert(550, "SetTrapDetectedBy", R_INT, {R_OBJECT, R_OBJECT}, &routine::SetTrapDetectedBy);
    routines.insert(551, "GetIsTrapped", R_INT, {R_OBJECT}, &routine::GetIsTrapped);
    routines.insert(552, "SetEffectIcon", R_EFFECT, {R_EFFECT, R_INT}, &routine::SetEffectIcon);
    routines.insert(553, "FaceObjectAwayFromObject", R_VOID, {R_OBJECT, R_OBJECT}, &routine::FaceObjectAwayFromObject);
    routines.insert(554, "PopUpDeathGUIPanel", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT, R_STRING}, &routine::PopUpDeathGUIPanel);
    routines.insert(555, "SetTrapDisabled", R_VOID, {R_OBJECT}, &routine::SetTrapDisabled);
    routines.insert(556, "GetLastHostileActor", R_OBJECT, {R_OBJECT}, &routine::GetLastHostileActor);
    routines.insert(557, "ExportAllCharacters", R_VOID, {}, &routine::ExportAllCharacters);
    routines.insert(558, "MusicBackgroundGetDayTrack", R_INT, {R_OBJECT}, &routine::MusicBackgroundGetDayTrack);
    routines.insert(559, "MusicBackgroundGetNightTrack", R_INT, {R_OBJECT}, &routine::MusicBackgroundGetNightTrack);
    routines.insert(560, "WriteTimestampedLogEntry", R_VOID, {R_STRING}, &routine::WriteTimestampedLogEntry);
    routines.insert(561, "GetModuleName", R_STRING, {}, &routine::GetModuleName);
    routines.insert(562, "GetFactionLeader", R_OBJECT, {R_OBJECT}, &routine::GetFactionLeader);
    routines.insert(564, "EndGame", R_VOID, {R_INT}, &routine::EndGame);
    routines.insert(565, "GetRunScriptVar", R_INT, {}, &routine::GetRunScriptVar);
    routines.insert(566, "GetCreatureMovmentType", R_INT, {R_OBJECT}, &routine::GetCreatureMovmentType);
    routines.insert(567, "AmbientSoundSetDayVolume", R_VOID, {R_OBJECT, R_INT}, &routine::AmbientSoundSetDayVolume);
    routines.insert(568, "AmbientSoundSetNightVolume", R_VOID, {R_OBJECT, R_INT}, &routine::AmbientSoundSetNightVolume);
    routines.insert(569, "MusicBackgroundGetBattleTrack", R_INT, {R_OBJECT}, &routine::MusicBackgroundGetBattleTrack);
    routines.insert(570, "GetHasInventory", R_INT, {R_OBJECT}, &routine::GetHasInventory);
    routines.insert(571, "GetStrRefSoundDuration", R_FLOAT, {R_INT}, &routine::GetStrRefSoundDuration);
    routines.insert(572, "AddToParty", R_VOID, {R_OBJECT, R_OBJECT}, &routine::AddToParty);
    routines.insert(573, "RemoveFromParty", R_VOID, {R_OBJECT}, &routine::RemoveFromParty);
    routines.insert(574, "AddPartyMember", R_INT, {R_INT, R_OBJECT}, &routine::AddPartyMember);
    routines.insert(575, "RemovePartyMember", R_INT, {R_INT}, &routine::RemovePartyMember);
    routines.insert(576, "IsObjectPartyMember", R_INT, {R_OBJECT}, &routine::IsObjectPartyMember);
    routines.insert(577, "GetPartyMemberByIndex", R_OBJECT, {R_INT}, &routine::GetPartyMemberByIndex);
    routines.insert(578, "GetGlobalBoolean", R_INT, {R_STRING}, &routine::GetGlobalBoolean);
    routines.insert(579, "SetGlobalBoolean", R_VOID, {R_STRING, R_INT}, &routine::SetGlobalBoolean);
    routines.insert(580, "GetGlobalNumber", R_INT, {R_STRING}, &routine::GetGlobalNumber);
    routines.insert(581, "SetGlobalNumber", R_VOID, {R_STRING, R_INT}, &routine::SetGlobalNumber);
    routines.insert(582, "AurPostString", R_VOID, {R_STRING, R_INT, R_INT, R_FLOAT}, &routine::AurPostString);
    routines.insert(669, "AddJournalWorldEntry", R_VOID, {R_INT, R_STRING, R_STRING}, &routine::AddJournalWorldEntry);
    routines.insert(670, "AddJournalWorldEntryStrref", R_VOID, {R_INT, R_INT}, &routine::AddJournalWorldEntryStrref);
    routines.insert(671, "BarkString", R_VOID, {R_OBJECT, R_INT}, &routine::BarkString);
    routines.insert(672, "DeleteJournalWorldAllEntries", R_VOID, {}, &routine::DeleteJournalWorldAllEntries);
    routines.insert(673, "DeleteJournalWorldEntry", R_VOID, {R_INT}, &routine::DeleteJournalWorldEntry);
    routines.insert(674, "DeleteJournalWorldEntryStrref", R_VOID, {R_INT}, &routine::DeleteJournalWorldEntryStrref);
    routines.insert(677, "PlayVisualAreaEffect", R_VOID, {R_INT, R_LOCATION}, &routine::PlayVisualAreaEffect);
    routines.insert(678, "SetJournalQuestEntryPicture", R_VOID, {R_STRING, R_OBJECT, R_INT, R_INT, R_INT}, &routine::SetJournalQuestEntryPicture);
    routines.insert(679, "GetLocalBoolean", R_INT, {R_OBJECT, R_INT}, &routine::GetLocalBoolean);
    routines.insert(680, "SetLocalBoolean", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::SetLocalBoolean);
    routines.insert(681, "GetLocalNumber", R_INT, {R_OBJECT, R_INT}, &routine::GetLocalNumber);
    routines.insert(682, "SetLocalNumber", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::SetLocalNumber);
    routines.insert(689, "SoundObjectGetPitchVariance", R_FLOAT, {R_OBJECT}, &routine::SoundObjectGetPitchVariance);
    routines.insert(690, "SoundObjectSetPitchVariance", R_VOID, {R_OBJECT, R_FLOAT}, &routine::SoundObjectSetPitchVariance);
    routines.insert(691, "SoundObjectGetVolume", R_INT, {R_OBJECT}, &routine::SoundObjectGetVolume);
    routines.insert(692, "GetGlobalLocation", R_LOCATION, {R_STRING}, &routine::GetGlobalLocation);
    routines.insert(693, "SetGlobalLocation", R_VOID, {R_STRING, R_LOCATION}, &routine::SetGlobalLocation);
    routines.insert(694, "AddAvailableNPCByObject", R_INT, {R_INT, R_OBJECT}, &routine::AddAvailableNPCByObject);
    routines.insert(695, "RemoveAvailableNPC", R_INT, {R_INT}, &routine::RemoveAvailableNPC);
    routines.insert(696, "IsAvailableCreature", R_INT, {R_INT}, &routine::IsAvailableCreature);
    routines.insert(697, "AddAvailableNPCByTemplate", R_INT, {R_INT, R_STRING}, &routine::AddAvailableNPCByTemplate);
    routines.insert(698, "SpawnAvailableNPC", R_OBJECT, {R_INT, R_LOCATION}, &routine::SpawnAvailableNPC);
    routines.insert(699, "IsNPCPartyMember", R_INT, {R_INT}, &routine::IsNPCPartyMember);
    routines.insert(701, "GetIsConversationActive", R_INT, {}, &routine::GetIsConversationActive);
    routines.insert(704, "GetPartyAIStyle", R_INT, {}, &routine::GetPartyAIStyle);
    routines.insert(705, "GetNPCAIStyle", R_INT, {R_OBJECT}, &routine::GetNPCAIStyle);
    routines.insert(706, "SetPartyAIStyle", R_VOID, {R_INT}, &routine::SetPartyAIStyle);
    routines.insert(707, "SetNPCAIStyle", R_VOID, {R_OBJECT, R_INT}, &routine::SetNPCAIStyle);
    routines.insert(708, "SetNPCSelectability", R_VOID, {R_INT, R_INT}, &routine::SetNPCSelectability);
    routines.insert(709, "GetNPCSelectability", R_INT, {R_INT}, &routine::GetNPCSelectability);
    routines.insert(710, "ClearAllEffects", R_VOID, {}, &routine::ClearAllEffects);
    routines.insert(711, "GetLastConversation", R_STRING, {}, &routine::GetLastConversation);
    routines.insert(712, "ShowPartySelectionGUI", R_VOID, {R_STRING, R_INT, R_INT}, &routine::ShowPartySelectionGUI);
    routines.insert(713, "GetStandardFaction", R_INT, {R_OBJECT}, &routine::GetStandardFaction);
    routines.insert(714, "GivePlotXP", R_VOID, {R_STRING, R_INT}, &routine::GivePlotXP);
    routines.insert(715, "GetMinOneHP", R_INT, {R_OBJECT}, &routine::GetMinOneHP);
    routines.insert(716, "SetMinOneHP", R_VOID, {R_OBJECT, R_INT}, &routine::SetMinOneHP);
    routines.insert(719, "SetGlobalFadeIn", R_VOID, {R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT}, &routine::SetGlobalFadeIn);
    routines.insert(720, "SetGlobalFadeOut", R_VOID, {R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT}, &routine::SetGlobalFadeOut);
    routines.insert(721, "GetLastHostileTarget", R_OBJECT, {R_OBJECT}, &routine::GetLastHostileTarget);
    routines.insert(722, "GetLastAttackAction", R_INT, {R_OBJECT}, &routine::GetLastAttackAction);
    routines.insert(723, "GetLastForcePowerUsed", R_INT, {R_OBJECT}, &routine::GetLastForcePowerUsed);
    routines.insert(724, "GetLastCombatFeatUsed", R_INT, {R_OBJECT}, &routine::GetLastCombatFeatUsed);
    routines.insert(725, "GetLastAttackResult", R_INT, {R_OBJECT}, &routine::GetLastAttackResult);
    routines.insert(726, "GetWasForcePowerSuccessful", R_INT, {R_OBJECT}, &routine::GetWasForcePowerSuccessful);
    routines.insert(727, "GetFirstAttacker", R_OBJECT, {R_OBJECT}, &routine::GetFirstAttacker);
    routines.insert(728, "GetNextAttacker", R_OBJECT, {R_OBJECT}, &routine::GetNextAttacker);
    routines.insert(729, "SetFormation", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT}, &routine::SetFormation);
    routines.insert(731, "SetForcePowerUnsuccessful", R_VOID, {R_INT, R_OBJECT}, &routine::SetForcePowerUnsuccessful);
    routines.insert(732, "GetIsDebilitated", R_INT, {R_OBJECT}, &routine::GetIsDebilitated);
    routines.insert(733, "PlayMovie", R_VOID, {R_STRING}, &routine::PlayMovie);
    routines.insert(734, "SaveNPCState", R_VOID, {R_INT}, &routine::SaveNPCState);
    routines.insert(735, "GetCategoryFromTalent", R_INT, {R_TALENT}, &routine::GetCategoryFromTalent);
    routines.insert(736, "SurrenderByFaction", R_VOID, {R_INT, R_INT}, &routine::SurrenderByFaction);
    routines.insert(737, "ChangeFactionByFaction", R_VOID, {R_INT, R_INT}, &routine::ChangeFactionByFaction);
    routines.insert(738, "PlayRoomAnimation", R_VOID, {R_STRING, R_INT}, &routine::PlayRoomAnimation);
    routines.insert(739, "ShowGalaxyMap", R_VOID, {R_INT}, &routine::ShowGalaxyMap);
    routines.insert(740, "SetPlanetSelectable", R_VOID, {R_INT, R_INT}, &routine::SetPlanetSelectable);
    routines.insert(741, "GetPlanetSelectable", R_INT, {R_INT}, &routine::GetPlanetSelectable);
    routines.insert(742, "SetPlanetAvailable", R_VOID, {R_INT, R_INT}, &routine::SetPlanetAvailable);
    routines.insert(743, "GetPlanetAvailable", R_INT, {R_INT}, &routine::GetPlanetAvailable);
    routines.insert(744, "GetSelectedPlanet", R_INT, {}, &routine::GetSelectedPlanet);
    routines.insert(745, "SoundObjectFadeAndStop", R_VOID, {R_OBJECT, R_FLOAT}, &routine::SoundObjectFadeAndStop);
    routines.insert(746, "SetAreaFogColor", R_VOID, {R_OBJECT, R_FLOAT, R_FLOAT, R_FLOAT}, &routine::SetAreaFogColor);
    routines.insert(747, "ChangeItemCost", R_VOID, {R_STRING, R_FLOAT}, &routine::ChangeItemCost);
    routines.insert(748, "GetIsLiveContentAvailable", R_INT, {R_INT}, &routine::GetIsLiveContentAvailable);
    routines.insert(749, "ResetDialogState", R_VOID, {}, &routine::ResetDialogState);
    routines.insert(750, "SetGoodEvilValue", R_VOID, {R_OBJECT, R_INT}, &routine::SetGoodEvilValue);
    routines.insert(751, "GetIsPoisoned", R_INT, {R_OBJECT}, &routine::GetIsPoisoned);
    routines.insert(752, "GetSpellTarget", R_OBJECT, {R_OBJECT}, &routine::GetSpellTarget);
    routines.insert(753, "SetSoloMode", R_VOID, {R_INT}, &routine::SetSoloMode);
    routines.insert(757, "CancelPostDialogCharacterSwitch", R_VOID, {}, &routine::CancelPostDialogCharacterSwitch);
    routines.insert(758, "SetMaxHitPoints", R_VOID, {R_OBJECT, R_INT}, &routine::SetMaxHitPoints);
    routines.insert(759, "NoClicksFor", R_VOID, {R_FLOAT}, &routine::NoClicksFor);
    routines.insert(760, "HoldWorldFadeInForDialog", R_VOID, {}, &routine::HoldWorldFadeInForDialog);
    routines.insert(761, "ShipBuild", R_INT, {}, &routine::ShipBuild);
    routines.insert(762, "SurrenderRetainBuffs", R_VOID, {}, &routine::SurrenderRetainBuffs);
    routines.insert(763, "SuppressStatusSummaryEntry", R_VOID, {R_INT}, &routine::SuppressStatusSummaryEntry);
    routines.insert(764, "GetCheatCode", R_INT, {R_INT}, &routine::GetCheatCode);
    routines.insert(765, "SetMusicVolume", R_VOID, {R_FLOAT}, &routine::SetMusicVolume);
    routines.insert(766, "CreateItemOnFloor", R_OBJECT, {R_STRING, R_LOCATION, R_INT}, &routine::CreateItemOnFloor);
    routines.insert(767, "SetAvailableNPCId", R_VOID, {R_INT, R_OBJECT}, &routine::SetAvailableNPCId);
    routines.insert(768, "IsMoviePlaying", R_INT, {}, &routine::IsMoviePlaying);
    routines.insert(769, "QueueMovie", R_VOID, {R_STRING, R_INT}, &routine::QueueMovie);
    routines.insert(770, "PlayMovieQueue", R_VOID, {R_INT}, &routine::PlayMovieQueue);
    routines.insert(771, "YavinHackCloseDoor", R_VOID, {R_OBJECT}, &routine::YavinHackDoorClose);
}

void registerMainTslRoutines(Routines &routines) {
    routines.insert(0, "Random", R_INT, {R_INT}, &routine::Random);
    routines.insert(1, "PrintString", R_VOID, {R_STRING}, &routine::PrintString);
    routines.insert(2, "PrintFloat", R_VOID, {R_FLOAT, R_INT, R_INT}, &routine::PrintFloat);
    routines.insert(3, "FloatToString", R_STRING, {R_FLOAT, R_INT, R_INT}, &routine::FloatToString);
    routines.insert(4, "PrintInteger", R_VOID, {R_INT}, &routine::PrintInteger);
    routines.insert(5, "PrintObject", R_VOID, {R_OBJECT}, &routine::PrintObject);
    routines.insert(6, "AssignCommand", R_VOID, {R_OBJECT, R_ACTION}, &routine::AssignCommand);
    routines.insert(7, "DelayCommand", R_VOID, {R_FLOAT, R_ACTION}, &routine::DelayCommand);
    routines.insert(8, "ExecuteScript", R_VOID, {R_STRING, R_OBJECT, R_INT}, &routine::ExecuteScript);
    routines.insert(9, "ClearAllActions", R_VOID, {}, &routine::ClearAllActions);
    routines.insert(10, "SetFacing", R_VOID, {R_FLOAT}, &routine::SetFacing);
    routines.insert(11, "SwitchPlayerCharacter", R_INT, {R_INT}, &routine::SwitchPlayerCharacter);
    routines.insert(12, "SetTime", R_VOID, {R_INT, R_INT, R_INT, R_INT}, &routine::SetTime);
    routines.insert(13, "SetPartyLeader", R_INT, {R_INT}, &routine::SetPartyLeader);
    routines.insert(14, "SetAreaUnescapable", R_VOID, {R_INT}, &routine::SetAreaUnescapable);
    routines.insert(15, "GetAreaUnescapable", R_INT, {}, &routine::GetAreaUnescapable);
    routines.insert(16, "GetTimeHour", R_INT, {}, &routine::GetTimeHour);
    routines.insert(17, "GetTimeMinute", R_INT, {}, &routine::GetTimeMinute);
    routines.insert(18, "GetTimeSecond", R_INT, {}, &routine::GetTimeSecond);
    routines.insert(19, "GetTimeMillisecond", R_INT, {}, &routine::GetTimeMillisecond);
    routines.insert(24, "GetArea", R_OBJECT, {R_OBJECT}, &routine::GetArea);
    routines.insert(25, "GetEnteringObject", R_OBJECT, {}, &routine::GetEnteringObject);
    routines.insert(26, "GetExitingObject", R_OBJECT, {}, &routine::GetExitingObject);
    routines.insert(27, "GetPosition", R_VECTOR, {R_OBJECT}, &routine::GetPosition);
    routines.insert(28, "GetFacing", R_FLOAT, {R_OBJECT}, &routine::GetFacing);
    routines.insert(29, "GetItemPossessor", R_OBJECT, {R_OBJECT}, &routine::GetItemPossessor);
    routines.insert(30, "GetItemPossessedBy", R_OBJECT, {R_OBJECT, R_STRING}, &routine::GetItemPossessedBy);
    routines.insert(31, "CreateItemOnObject", R_OBJECT, {R_STRING, R_OBJECT, R_INT, R_INT}, &routine::CreateItemOnObject);
    routines.insert(36, "GetLastAttacker", R_OBJECT, {R_OBJECT}, &routine::GetLastAttacker);
    routines.insert(38, "GetNearestCreature", R_OBJECT, {R_INT, R_INT, R_OBJECT, R_INT, R_INT, R_INT, R_INT, R_INT}, &routine::GetNearestCreature);
    routines.insert(41, "GetDistanceToObject", R_FLOAT, {R_OBJECT}, &routine::GetDistanceToObject);
    routines.insert(42, "GetIsObjectValid", R_INT, {R_OBJECT}, &routine::GetIsObjectValid);
    routines.insert(45, "SetCameraFacing", R_VOID, {R_FLOAT}, &routine::SetCameraFacing);
    routines.insert(46, "PlaySound", R_VOID, {R_STRING}, &routine::PlaySound);
    routines.insert(47, "GetSpellTargetObject", R_OBJECT, {}, &routine::GetSpellTargetObject);
    routines.insert(49, "GetCurrentHitPoints", R_INT, {R_OBJECT}, &routine::GetCurrentHitPoints);
    routines.insert(50, "GetMaxHitPoints", R_INT, {R_OBJECT}, &routine::GetMaxHitPoints);
    routines.insert(52, "GetLastItemEquipped", R_OBJECT, {}, &routine::GetLastItemEquipped);
    routines.insert(53, "GetSubScreenID", R_INT, {}, &routine::GetSubScreenID);
    routines.insert(54, "CancelCombat", R_VOID, {R_OBJECT}, &routine::CancelCombat);
    routines.insert(55, "GetCurrentForcePoints", R_INT, {R_OBJECT}, &routine::GetCurrentForcePoints);
    routines.insert(56, "GetMaxForcePoints", R_INT, {R_OBJECT}, &routine::GetMaxForcePoints);
    routines.insert(57, "PauseGame", R_VOID, {R_INT}, &routine::PauseGame);
    routines.insert(58, "SetPlayerRestrictMode", R_VOID, {R_INT}, &routine::SetPlayerRestrictMode);
    routines.insert(59, "GetStringLength", R_INT, {R_STRING}, &routine::GetStringLength);
    routines.insert(60, "GetStringUpperCase", R_STRING, {R_STRING}, &routine::GetStringUpperCase);
    routines.insert(61, "GetStringLowerCase", R_STRING, {R_STRING}, &routine::GetStringLowerCase);
    routines.insert(62, "GetStringRight", R_STRING, {R_STRING, R_INT}, &routine::GetStringRight);
    routines.insert(63, "GetStringLeft", R_STRING, {R_STRING, R_INT}, &routine::GetStringLeft);
    routines.insert(64, "InsertString", R_STRING, {R_STRING, R_STRING, R_INT}, &routine::InsertString);
    routines.insert(65, "GetSubString", R_STRING, {R_STRING, R_INT, R_INT}, &routine::GetSubString);
    routines.insert(66, "FindSubString", R_INT, {R_STRING, R_STRING}, &routine::FindSubString);
    routines.insert(67, "fabs", R_FLOAT, {R_FLOAT}, &routine::fabs);
    routines.insert(68, "cos", R_FLOAT, {R_FLOAT}, &routine::cos);
    routines.insert(69, "sin", R_FLOAT, {R_FLOAT}, &routine::sin);
    routines.insert(70, "tan", R_FLOAT, {R_FLOAT}, &routine::tan);
    routines.insert(71, "acos", R_FLOAT, {R_FLOAT}, &routine::acos);
    routines.insert(72, "asin", R_FLOAT, {R_FLOAT}, &routine::asin);
    routines.insert(73, "atan", R_FLOAT, {R_FLOAT}, &routine::atan);
    routines.insert(74, "log", R_FLOAT, {R_FLOAT}, &routine::log);
    routines.insert(75, "pow", R_FLOAT, {R_FLOAT, R_FLOAT}, &routine::pow);
    routines.insert(76, "sqrt", R_FLOAT, {R_FLOAT}, &routine::sqrt);
    routines.insert(77, "abs", R_INT, {R_INT}, &routine::abs);
    routines.insert(83, "GetPlayerRestrictMode", R_INT, {R_OBJECT}, &routine::GetPlayerRestrictMode);
    routines.insert(84, "GetCasterLevel", R_INT, {R_OBJECT}, &routine::GetCasterLevel);
    routines.insert(85, "GetFirstEffect", R_EFFECT, {R_OBJECT}, &routine::GetFirstEffect);
    routines.insert(86, "GetNextEffect", R_EFFECT, {R_OBJECT}, &routine::GetNextEffect);
    routines.insert(87, "RemoveEffect", R_VOID, {R_OBJECT, R_EFFECT}, &routine::RemoveEffect);
    routines.insert(88, "GetIsEffectValid", R_INT, {R_EFFECT}, &routine::GetIsEffectValid);
    routines.insert(89, "GetEffectDurationType", R_INT, {R_EFFECT}, &routine::GetEffectDurationType);
    routines.insert(90, "GetEffectSubType", R_INT, {R_EFFECT}, &routine::GetEffectSubType);
    routines.insert(91, "GetEffectCreator", R_OBJECT, {R_EFFECT}, &routine::GetEffectCreator);
    routines.insert(92, "IntToString", R_STRING, {R_INT}, &routine::IntToString);
    routines.insert(93, "GetFirstObjectInArea", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFirstObjectInArea);
    routines.insert(94, "GetNextObjectInArea", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetNextObjectInArea);
    routines.insert(95, "d2", R_INT, {R_INT}, &routine::d2);
    routines.insert(96, "d3", R_INT, {R_INT}, &routine::d3);
    routines.insert(97, "d4", R_INT, {R_INT}, &routine::d4);
    routines.insert(98, "d6", R_INT, {R_INT}, &routine::d6);
    routines.insert(99, "d8", R_INT, {R_INT}, &routine::d8);
    routines.insert(100, "d10", R_INT, {R_INT}, &routine::d10);
    routines.insert(101, "d12", R_INT, {R_INT}, &routine::d12);
    routines.insert(102, "d20", R_INT, {R_INT}, &routine::d20);
    routines.insert(103, "d100", R_INT, {R_INT}, &routine::d100);
    routines.insert(104, "VectorMagnitude", R_FLOAT, {R_VECTOR}, &routine::VectorMagnitude);
    routines.insert(105, "GetMetaMagicFeat", R_INT, {}, &routine::GetMetaMagicFeat);
    routines.insert(106, "GetObjectType", R_INT, {R_OBJECT}, &routine::GetObjectType);
    routines.insert(107, "GetRacialType", R_INT, {R_OBJECT}, &routine::GetRacialType);
    routines.insert(108, "FortitudeSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &routine::FortitudeSave);
    routines.insert(109, "ReflexSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &routine::ReflexSave);
    routines.insert(110, "WillSave", R_INT, {R_OBJECT, R_INT, R_INT, R_OBJECT}, &routine::WillSave);
    routines.insert(111, "GetSpellSaveDC", R_INT, {}, &routine::GetSpellSaveDC);
    routines.insert(112, "MagicalEffect", R_EFFECT, {R_EFFECT}, &routine::MagicalEffect);
    routines.insert(113, "SupernaturalEffect", R_EFFECT, {R_EFFECT}, &routine::SupernaturalEffect);
    routines.insert(114, "ExtraordinaryEffect", R_EFFECT, {R_EFFECT}, &routine::ExtraordinaryEffect);
    routines.insert(116, "GetAC", R_INT, {R_OBJECT, R_INT}, &routine::GetAC);
    routines.insert(121, "RoundsToSeconds", R_FLOAT, {R_INT}, &routine::RoundsToSeconds);
    routines.insert(122, "HoursToSeconds", R_FLOAT, {R_INT}, &routine::HoursToSeconds);
    routines.insert(123, "TurnsToSeconds", R_FLOAT, {R_INT}, &routine::TurnsToSeconds);
    routines.insert(124, "SoundObjectSetFixedVariance", R_VOID, {R_OBJECT, R_FLOAT}, &routine::SoundObjectSetFixedVariance);
    routines.insert(125, "GetGoodEvilValue", R_INT, {R_OBJECT}, &routine::GetGoodEvilValue);
    routines.insert(126, "GetPartyMemberCount", R_INT, {}, &routine::GetPartyMemberCount);
    routines.insert(127, "GetAlignmentGoodEvil", R_INT, {R_OBJECT}, &routine::GetAlignmentGoodEvil);
    routines.insert(128, "GetFirstObjectInShape", R_OBJECT, {R_INT, R_FLOAT, R_LOCATION, R_INT, R_INT, R_VECTOR}, &routine::GetFirstObjectInShape);
    routines.insert(129, "GetNextObjectInShape", R_OBJECT, {R_INT, R_FLOAT, R_LOCATION, R_INT, R_INT, R_VECTOR}, &routine::GetNextObjectInShape);
    routines.insert(131, "SignalEvent", R_VOID, {R_OBJECT, R_EVENT}, &routine::SignalEvent);
    routines.insert(132, "EventUserDefined", R_EVENT, {R_INT}, &routine::EventUserDefined);
    routines.insert(137, "VectorNormalize", R_VECTOR, {R_VECTOR}, &routine::VectorNormalize);
    routines.insert(138, "GetItemStackSize", R_INT, {R_OBJECT}, &routine::GetItemStackSize);
    routines.insert(139, "GetAbilityScore", R_INT, {R_OBJECT, R_INT}, &routine::GetAbilityScore);
    routines.insert(140, "GetIsDead", R_INT, {R_OBJECT}, &routine::GetIsDead);
    routines.insert(141, "PrintVector", R_VOID, {R_VECTOR, R_INT}, &routine::PrintVector);
    routines.insert(142, "Vector", R_VECTOR, {R_FLOAT, R_FLOAT, R_FLOAT}, &routine::Vector);
    routines.insert(143, "SetFacingPoint", R_VOID, {R_VECTOR}, &routine::SetFacingPoint);
    routines.insert(144, "AngleToVector", R_VECTOR, {R_FLOAT}, &routine::AngleToVector);
    routines.insert(145, "VectorToAngle", R_FLOAT, {R_VECTOR}, &routine::VectorToAngle);
    routines.insert(146, "TouchAttackMelee", R_INT, {R_OBJECT, R_INT}, &routine::TouchAttackMelee);
    routines.insert(147, "TouchAttackRanged", R_INT, {R_OBJECT, R_INT}, &routine::TouchAttackRanged);
    routines.insert(150, "SetItemStackSize", R_VOID, {R_OBJECT, R_INT}, &routine::SetItemStackSize);
    routines.insert(151, "GetDistanceBetween", R_FLOAT, {R_OBJECT, R_OBJECT}, &routine::GetDistanceBetween);
    routines.insert(152, "SetReturnStrref", R_VOID, {R_INT, R_INT, R_INT}, &routine::SetReturnStrref);
    routines.insert(155, "GetItemInSlot", R_OBJECT, {R_INT, R_OBJECT}, &routine::GetItemInSlot);
    routines.insert(160, "SetGlobalString", R_VOID, {R_STRING, R_STRING}, &routine::SetGlobalString);
    routines.insert(162, "SetCommandable", R_VOID, {R_INT, R_OBJECT}, &routine::SetCommandable);
    routines.insert(163, "GetCommandable", R_INT, {R_OBJECT}, &routine::GetCommandable);
    routines.insert(166, "GetHitDice", R_INT, {R_OBJECT}, &routine::GetHitDice);
    routines.insert(168, "GetTag", R_STRING, {R_OBJECT}, &routine::GetTag);
    routines.insert(169, "ResistForce", R_INT, {R_OBJECT, R_OBJECT}, &routine::ResistForce);
    routines.insert(170, "GetEffectType", R_INT, {R_EFFECT}, &routine::GetEffectType);
    routines.insert(172, "GetFactionEqual", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetFactionEqual);
    routines.insert(173, "ChangeFaction", R_VOID, {R_OBJECT, R_OBJECT}, &routine::ChangeFaction);
    routines.insert(174, "GetIsListening", R_INT, {R_OBJECT}, &routine::GetIsListening);
    routines.insert(175, "SetListening", R_VOID, {R_OBJECT, R_INT}, &routine::SetListening);
    routines.insert(176, "SetListenPattern", R_VOID, {R_OBJECT, R_STRING, R_INT}, &routine::SetListenPattern);
    routines.insert(177, "TestStringAgainstPattern", R_INT, {R_STRING, R_STRING}, &routine::TestStringAgainstPattern);
    routines.insert(178, "GetMatchedSubstring", R_STRING, {R_INT}, &routine::GetMatchedSubstring);
    routines.insert(179, "GetMatchedSubstringsCount", R_INT, {}, &routine::GetMatchedSubstringsCount);
    routines.insert(181, "GetFactionWeakestMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionWeakestMember);
    routines.insert(182, "GetFactionStrongestMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionStrongestMember);
    routines.insert(183, "GetFactionMostDamagedMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionMostDamagedMember);
    routines.insert(184, "GetFactionLeastDamagedMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionLeastDamagedMember);
    routines.insert(185, "GetFactionGold", R_INT, {R_OBJECT}, &routine::GetFactionGold);
    routines.insert(186, "GetFactionAverageReputation", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetFactionAverageReputation);
    routines.insert(187, "GetFactionAverageGoodEvilAlignment", R_INT, {R_OBJECT}, &routine::GetFactionAverageGoodEvilAlignment);
    routines.insert(188, "SoundObjectGetFixedVariance", R_FLOAT, {R_OBJECT}, &routine::SoundObjectGetFixedVariance);
    routines.insert(189, "GetFactionAverageLevel", R_INT, {R_OBJECT}, &routine::GetFactionAverageLevel);
    routines.insert(190, "GetFactionAverageXP", R_INT, {R_OBJECT}, &routine::GetFactionAverageXP);
    routines.insert(191, "GetFactionMostFrequentClass", R_INT, {R_OBJECT}, &routine::GetFactionMostFrequentClass);
    routines.insert(192, "GetFactionWorstAC", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionWorstAC);
    routines.insert(193, "GetFactionBestAC", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFactionBestAC);
    routines.insert(194, "GetGlobalString", R_STRING, {R_STRING}, &routine::GetGlobalString);
    routines.insert(195, "GetListenPatternNumber", R_INT, {}, &routine::GetListenPatternNumber);
    routines.insert(197, "GetWaypointByTag", R_OBJECT, {R_STRING}, &routine::GetWaypointByTag);
    routines.insert(198, "GetTransitionTarget", R_OBJECT, {R_OBJECT}, &routine::GetTransitionTarget);
    routines.insert(200, "GetObjectByTag", R_OBJECT, {R_STRING, R_INT}, &routine::GetObjectByTag);
    routines.insert(201, "AdjustAlignment", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &routine::AdjustAlignment);
    routines.insert(203, "SetAreaTransitionBMP", R_VOID, {R_INT, R_STRING}, &routine::SetAreaTransitionBMP);
    routines.insert(208, "GetReputation", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetReputation);
    routines.insert(209, "AdjustReputation", R_VOID, {R_OBJECT, R_OBJECT, R_INT}, &routine::AdjustReputation);
    routines.insert(210, "GetModuleFileName", R_STRING, {}, &routine::GetModuleFileName);
    routines.insert(211, "GetGoingToBeAttackedBy", R_OBJECT, {R_OBJECT}, &routine::GetGoingToBeAttackedBy);
    routines.insert(213, "GetLocation", R_LOCATION, {R_OBJECT}, &routine::GetLocation);
    routines.insert(215, "Location", R_LOCATION, {R_VECTOR, R_FLOAT}, &routine::Location);
    routines.insert(216, "ApplyEffectAtLocation", R_VOID, {R_INT, R_EFFECT, R_LOCATION, R_FLOAT}, &routine::ApplyEffectAtLocation);
    routines.insert(217, "GetIsPC", R_INT, {R_OBJECT}, &routine::GetIsPC);
    routines.insert(218, "FeetToMeters", R_FLOAT, {R_FLOAT}, &routine::FeetToMeters);
    routines.insert(219, "YardsToMeters", R_FLOAT, {R_FLOAT}, &routine::YardsToMeters);
    routines.insert(220, "ApplyEffectToObject", R_VOID, {R_INT, R_EFFECT, R_OBJECT, R_FLOAT}, &routine::ApplyEffectToObject);
    routines.insert(221, "SpeakString", R_VOID, {R_STRING, R_INT}, &routine::SpeakString);
    routines.insert(222, "GetSpellTargetLocation", R_LOCATION, {}, &routine::GetSpellTargetLocation);
    routines.insert(223, "GetPositionFromLocation", R_VECTOR, {R_LOCATION}, &routine::GetPositionFromLocation);
    routines.insert(225, "GetFacingFromLocation", R_FLOAT, {R_LOCATION}, &routine::GetFacingFromLocation);
    routines.insert(226, "GetNearestCreatureToLocation", R_OBJECT, {R_INT, R_INT, R_LOCATION, R_INT, R_INT, R_INT, R_INT, R_INT}, &routine::GetNearestCreatureToLocation);
    routines.insert(227, "GetNearestObject", R_OBJECT, {R_INT, R_OBJECT, R_INT}, &routine::GetNearestObject);
    routines.insert(228, "GetNearestObjectToLocation", R_OBJECT, {R_INT, R_LOCATION, R_INT}, &routine::GetNearestObjectToLocation);
    routines.insert(229, "GetNearestObjectByTag", R_OBJECT, {R_STRING, R_OBJECT, R_INT}, &routine::GetNearestObjectByTag);
    routines.insert(230, "IntToFloat", R_FLOAT, {R_INT}, &routine::IntToFloat);
    routines.insert(231, "FloatToInt", R_INT, {R_FLOAT}, &routine::FloatToInt);
    routines.insert(232, "StringToInt", R_INT, {R_STRING}, &routine::StringToInt);
    routines.insert(233, "StringToFloat", R_FLOAT, {R_STRING}, &routine::StringToFloat);
    routines.insert(235, "GetIsEnemy", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetIsEnemy);
    routines.insert(236, "GetIsFriend", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetIsFriend);
    routines.insert(237, "GetIsNeutral", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetIsNeutral);
    routines.insert(238, "GetPCSpeaker", R_OBJECT, {}, &routine::GetPCSpeaker);
    routines.insert(239, "GetStringByStrRef", R_STRING, {R_INT}, &routine::GetStringByStrRef);
    routines.insert(241, "DestroyObject", R_VOID, {R_OBJECT, R_FLOAT, R_INT, R_FLOAT, R_INT}, &routine::DestroyObject);
    routines.insert(242, "GetModule", R_OBJECT, {}, &routine::GetModule);
    routines.insert(243, "CreateObject", R_OBJECT, {R_INT, R_STRING, R_LOCATION, R_INT}, &routine::CreateObject);
    routines.insert(244, "EventSpellCastAt", R_EVENT, {R_OBJECT, R_INT, R_INT}, &routine::EventSpellCastAt);
    routines.insert(245, "GetLastSpellCaster", R_OBJECT, {}, &routine::GetLastSpellCaster);
    routines.insert(246, "GetLastSpell", R_INT, {}, &routine::GetLastSpell);
    routines.insert(247, "GetUserDefinedEventNumber", R_INT, {}, &routine::GetUserDefinedEventNumber);
    routines.insert(248, "GetSpellId", R_INT, {}, &routine::GetSpellId);
    routines.insert(249, "RandomName", R_STRING, {}, &routine::RandomName);
    routines.insert(251, "GetLoadFromSaveGame", R_INT, {}, &routine::GetLoadFromSaveGame);
    routines.insert(253, "GetName", R_STRING, {R_OBJECT}, &routine::GetName);
    routines.insert(254, "GetLastSpeaker", R_OBJECT, {}, &routine::GetLastSpeaker);
    routines.insert(255, "BeginConversation", R_INT, {R_STRING, R_OBJECT}, &routine::BeginConversation);
    routines.insert(256, "GetLastPerceived", R_OBJECT, {}, &routine::GetLastPerceived);
    routines.insert(257, "GetLastPerceptionHeard", R_INT, {}, &routine::GetLastPerceptionHeard);
    routines.insert(258, "GetLastPerceptionInaudible", R_INT, {}, &routine::GetLastPerceptionInaudible);
    routines.insert(259, "GetLastPerceptionSeen", R_INT, {}, &routine::GetLastPerceptionSeen);
    routines.insert(260, "GetLastClosedBy", R_OBJECT, {}, &routine::GetLastClosedBy);
    routines.insert(261, "GetLastPerceptionVanished", R_INT, {}, &routine::GetLastPerceptionVanished);
    routines.insert(262, "GetFirstInPersistentObject", R_OBJECT, {R_OBJECT, R_INT, R_INT}, &routine::GetFirstInPersistentObject);
    routines.insert(263, "GetNextInPersistentObject", R_OBJECT, {R_OBJECT, R_INT, R_INT}, &routine::GetNextInPersistentObject);
    routines.insert(264, "GetAreaOfEffectCreator", R_OBJECT, {R_OBJECT}, &routine::GetAreaOfEffectCreator);
    routines.insert(265, "ShowLevelUpGUI", R_INT, {}, &routine::ShowLevelUpGUI);
    routines.insert(266, "SetItemNonEquippable", R_VOID, {R_OBJECT, R_INT}, &routine::SetItemNonEquippable);
    routines.insert(267, "GetButtonMashCheck", R_INT, {}, &routine::GetButtonMashCheck);
    routines.insert(268, "SetButtonMashCheck", R_VOID, {R_INT}, &routine::SetButtonMashCheck);
    routines.insert(271, "GiveItem", R_VOID, {R_OBJECT, R_OBJECT}, &routine::GiveItem);
    routines.insert(272, "ObjectToString", R_STRING, {R_OBJECT}, &routine::ObjectToString);
    routines.insert(274, "GetIsImmune", R_INT, {R_OBJECT, R_INT, R_OBJECT}, &routine::GetIsImmune);
    routines.insert(276, "GetEncounterActive", R_INT, {R_OBJECT}, &routine::GetEncounterActive);
    routines.insert(277, "SetEncounterActive", R_VOID, {R_INT, R_OBJECT}, &routine::SetEncounterActive);
    routines.insert(278, "GetEncounterSpawnsMax", R_INT, {R_OBJECT}, &routine::GetEncounterSpawnsMax);
    routines.insert(279, "SetEncounterSpawnsMax", R_VOID, {R_INT, R_OBJECT}, &routine::SetEncounterSpawnsMax);
    routines.insert(280, "GetEncounterSpawnsCurrent", R_INT, {R_OBJECT}, &routine::GetEncounterSpawnsCurrent);
    routines.insert(281, "SetEncounterSpawnsCurrent", R_VOID, {R_INT, R_OBJECT}, &routine::SetEncounterSpawnsCurrent);
    routines.insert(282, "GetModuleItemAcquired", R_OBJECT, {}, &routine::GetModuleItemAcquired);
    routines.insert(283, "GetModuleItemAcquiredFrom", R_OBJECT, {}, &routine::GetModuleItemAcquiredFrom);
    routines.insert(284, "SetCustomToken", R_VOID, {R_INT, R_STRING}, &routine::SetCustomToken);
    routines.insert(285, "GetHasFeat", R_INT, {R_INT, R_OBJECT}, &routine::GetHasFeat);
    routines.insert(286, "GetHasSkill", R_INT, {R_INT, R_OBJECT}, &routine::GetHasSkill);
    routines.insert(289, "GetObjectSeen", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetObjectSeen);
    routines.insert(290, "GetObjectHeard", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetObjectHeard);
    routines.insert(291, "GetLastPlayerDied", R_OBJECT, {}, &routine::GetLastPlayerDied);
    routines.insert(292, "GetModuleItemLost", R_OBJECT, {}, &routine::GetModuleItemLost);
    routines.insert(293, "GetModuleItemLostBy", R_OBJECT, {}, &routine::GetModuleItemLostBy);
    routines.insert(295, "EventConversation", R_EVENT, {}, &routine::EventConversation);
    routines.insert(296, "SetEncounterDifficulty", R_VOID, {R_INT, R_OBJECT}, &routine::SetEncounterDifficulty);
    routines.insert(297, "GetEncounterDifficulty", R_INT, {R_OBJECT}, &routine::GetEncounterDifficulty);
    routines.insert(298, "GetDistanceBetweenLocations", R_FLOAT, {R_LOCATION, R_LOCATION}, &routine::GetDistanceBetweenLocations);
    routines.insert(299, "GetReflexAdjustedDamage", R_INT, {R_INT, R_OBJECT, R_INT, R_INT, R_OBJECT}, &routine::GetReflexAdjustedDamage);
    routines.insert(300, "PlayAnimation", R_VOID, {R_INT, R_FLOAT, R_FLOAT}, &routine::PlayAnimation);
    routines.insert(301, "TalentSpell", R_TALENT, {R_INT}, &routine::TalentSpell);
    routines.insert(302, "TalentFeat", R_TALENT, {R_INT}, &routine::TalentFeat);
    routines.insert(303, "TalentSkill", R_TALENT, {R_INT}, &routine::TalentSkill);
    routines.insert(304, "GetHasSpellEffect", R_INT, {R_INT, R_OBJECT}, &routine::GetHasSpellEffect);
    routines.insert(305, "GetEffectSpellId", R_INT, {R_EFFECT}, &routine::GetEffectSpellId);
    routines.insert(306, "GetCreatureHasTalent", R_INT, {R_TALENT, R_OBJECT}, &routine::GetCreatureHasTalent);
    routines.insert(307, "GetCreatureTalentRandom", R_TALENT, {R_INT, R_OBJECT, R_INT}, &routine::GetCreatureTalentRandom);
    routines.insert(308, "GetCreatureTalentBest", R_TALENT, {R_INT, R_INT, R_OBJECT, R_INT, R_INT, R_INT}, &routine::GetCreatureTalentBest);
    routines.insert(311, "GetGoldPieceValue", R_INT, {R_OBJECT}, &routine::GetGoldPieceValue);
    routines.insert(312, "GetIsPlayableRacialType", R_INT, {R_OBJECT}, &routine::GetIsPlayableRacialType);
    routines.insert(313, "JumpToLocation", R_VOID, {R_LOCATION}, &routine::JumpToLocation);
    routines.insert(315, "GetSkillRank", R_INT, {R_INT, R_OBJECT}, &routine::GetSkillRank);
    routines.insert(316, "GetAttackTarget", R_OBJECT, {R_OBJECT}, &routine::GetAttackTarget);
    routines.insert(317, "GetLastAttackType", R_INT, {R_OBJECT}, &routine::GetLastAttackType);
    routines.insert(318, "GetLastAttackMode", R_INT, {R_OBJECT}, &routine::GetLastAttackMode);
    routines.insert(319, "GetDistanceBetween2D", R_FLOAT, {R_OBJECT, R_OBJECT}, &routine::GetDistanceBetween2D);
    routines.insert(320, "GetIsInCombat", R_INT, {R_OBJECT, R_INT}, &routine::GetIsInCombat);
    routines.insert(321, "GetLastAssociateCommand", R_INT, {R_OBJECT}, &routine::GetLastAssociateCommand);
    routines.insert(322, "GiveGoldToCreature", R_VOID, {R_OBJECT, R_INT}, &routine::GiveGoldToCreature);
    routines.insert(323, "SetIsDestroyable", R_VOID, {R_INT, R_INT, R_INT}, &routine::SetIsDestroyable);
    routines.insert(324, "SetLocked", R_VOID, {R_OBJECT, R_INT}, &routine::SetLocked);
    routines.insert(325, "GetLocked", R_INT, {R_OBJECT}, &routine::GetLocked);
    routines.insert(326, "GetClickingObject", R_OBJECT, {}, &routine::GetClickingObject);
    routines.insert(327, "SetAssociateListenPatterns", R_VOID, {R_OBJECT}, &routine::SetAssociateListenPatterns);
    routines.insert(328, "GetLastWeaponUsed", R_OBJECT, {R_OBJECT}, &routine::GetLastWeaponUsed);
    routines.insert(330, "GetLastUsedBy", R_OBJECT, {}, &routine::GetLastUsedBy);
    routines.insert(331, "GetAbilityModifier", R_INT, {R_INT, R_OBJECT}, &routine::GetAbilityModifier);
    routines.insert(332, "GetIdentified", R_INT, {R_OBJECT}, &routine::GetIdentified);
    routines.insert(333, "SetIdentified", R_VOID, {R_OBJECT, R_INT}, &routine::SetIdentified);
    routines.insert(334, "GetDistanceBetweenLocations2D", R_FLOAT, {R_LOCATION, R_LOCATION}, &routine::GetDistanceBetweenLocations2D);
    routines.insert(335, "GetDistanceToObject2D", R_FLOAT, {R_OBJECT}, &routine::GetDistanceToObject2D);
    routines.insert(336, "GetBlockingDoor", R_OBJECT, {}, &routine::GetBlockingDoor);
    routines.insert(337, "GetIsDoorActionPossible", R_INT, {R_OBJECT, R_INT}, &routine::GetIsDoorActionPossible);
    routines.insert(338, "DoDoorAction", R_VOID, {R_OBJECT, R_INT}, &routine::DoDoorAction);
    routines.insert(339, "GetFirstItemInInventory", R_OBJECT, {R_OBJECT}, &routine::GetFirstItemInInventory);
    routines.insert(340, "GetNextItemInInventory", R_OBJECT, {R_OBJECT}, &routine::GetNextItemInInventory);
    routines.insert(341, "GetClassByPosition", R_INT, {R_INT, R_OBJECT}, &routine::GetClassByPosition);
    routines.insert(342, "GetLevelByPosition", R_INT, {R_INT, R_OBJECT}, &routine::GetLevelByPosition);
    routines.insert(343, "GetLevelByClass", R_INT, {R_INT, R_OBJECT}, &routine::GetLevelByClass);
    routines.insert(344, "GetDamageDealtByType", R_INT, {R_INT}, &routine::GetDamageDealtByType);
    routines.insert(345, "GetTotalDamageDealt", R_INT, {}, &routine::GetTotalDamageDealt);
    routines.insert(346, "GetLastDamager", R_OBJECT, {}, &routine::GetLastDamager);
    routines.insert(347, "GetLastDisarmed", R_OBJECT, {}, &routine::GetLastDisarmed);
    routines.insert(348, "GetLastDisturbed", R_OBJECT, {}, &routine::GetLastDisturbed);
    routines.insert(349, "GetLastLocked", R_OBJECT, {}, &routine::GetLastLocked);
    routines.insert(350, "GetLastUnlocked", R_OBJECT, {}, &routine::GetLastUnlocked);
    routines.insert(352, "GetInventoryDisturbType", R_INT, {}, &routine::GetInventoryDisturbType);
    routines.insert(353, "GetInventoryDisturbItem", R_OBJECT, {}, &routine::GetInventoryDisturbItem);
    routines.insert(354, "ShowUpgradeScreen", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT, R_STRING}, &routine::ShowUpgradeScreen);
    routines.insert(355, "VersusAlignmentEffect", R_EFFECT, {R_EFFECT, R_INT, R_INT}, &routine::VersusAlignmentEffect);
    routines.insert(356, "VersusRacialTypeEffect", R_EFFECT, {R_EFFECT, R_INT}, &routine::VersusRacialTypeEffect);
    routines.insert(357, "VersusTrapEffect", R_EFFECT, {R_EFFECT}, &routine::VersusTrapEffect);
    routines.insert(358, "GetGender", R_INT, {R_OBJECT}, &routine::GetGender);
    routines.insert(359, "GetIsTalentValid", R_INT, {R_TALENT}, &routine::GetIsTalentValid);
    routines.insert(361, "GetAttemptedAttackTarget", R_OBJECT, {}, &routine::GetAttemptedAttackTarget);
    routines.insert(362, "GetTypeFromTalent", R_INT, {R_TALENT}, &routine::GetTypeFromTalent);
    routines.insert(363, "GetIdFromTalent", R_INT, {R_TALENT}, &routine::GetIdFromTalent);
    routines.insert(364, "PlayPazaak", R_VOID, {R_INT, R_STRING, R_INT, R_INT, R_OBJECT}, &routine::PlayPazaak);
    routines.insert(365, "GetLastPazaakResult", R_INT, {}, &routine::GetLastPazaakResult);
    routines.insert(366, "DisplayFeedBackText", R_VOID, {R_OBJECT, R_INT}, &routine::DisplayFeedBackText);
    routines.insert(367, "AddJournalQuestEntry", R_VOID, {R_STRING, R_INT, R_INT}, &routine::AddJournalQuestEntry);
    routines.insert(368, "RemoveJournalQuestEntry", R_VOID, {R_STRING}, &routine::RemoveJournalQuestEntry);
    routines.insert(369, "GetJournalEntry", R_INT, {R_STRING}, &routine::GetJournalEntry);
    routines.insert(370, "PlayRumblePattern", R_INT, {R_INT}, &routine::PlayRumblePattern);
    routines.insert(371, "StopRumblePattern", R_INT, {R_INT}, &routine::StopRumblePattern);
    routines.insert(374, "SendMessageToPC", R_VOID, {R_OBJECT, R_STRING}, &routine::SendMessageToPC);
    routines.insert(375, "GetAttemptedSpellTarget", R_OBJECT, {}, &routine::GetAttemptedSpellTarget);
    routines.insert(376, "GetLastOpenedBy", R_OBJECT, {}, &routine::GetLastOpenedBy);
    routines.insert(377, "GetHasSpell", R_INT, {R_INT, R_OBJECT}, &routine::GetHasSpell);
    routines.insert(378, "OpenStore", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT}, &routine::OpenStore);
    routines.insert(380, "GetFirstFactionMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetFirstFactionMember);
    routines.insert(381, "GetNextFactionMember", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetNextFactionMember);
    routines.insert(384, "GetJournalQuestExperience", R_INT, {R_STRING}, &routine::GetJournalQuestExperience);
    routines.insert(385, "JumpToObject", R_VOID, {R_OBJECT, R_INT}, &routine::JumpToObject);
    routines.insert(386, "SetMapPinEnabled", R_VOID, {R_OBJECT, R_INT}, &routine::SetMapPinEnabled);
    routines.insert(388, "PopUpGUIPanel", R_VOID, {R_OBJECT, R_INT}, &routine::PopUpGUIPanel);
    routines.insert(389, "AddMultiClass", R_VOID, {R_INT, R_OBJECT}, &routine::AddMultiClass);
    routines.insert(390, "GetIsLinkImmune", R_INT, {R_OBJECT, R_EFFECT}, &routine::GetIsLinkImmune);
    routines.insert(393, "GiveXPToCreature", R_VOID, {R_OBJECT, R_INT}, &routine::GiveXPToCreature);
    routines.insert(394, "SetXP", R_VOID, {R_OBJECT, R_INT}, &routine::SetXP);
    routines.insert(395, "GetXP", R_INT, {R_OBJECT}, &routine::GetXP);
    routines.insert(396, "IntToHexString", R_STRING, {R_INT}, &routine::IntToHexString);
    routines.insert(397, "GetBaseItemType", R_INT, {R_OBJECT}, &routine::GetBaseItemType);
    routines.insert(398, "GetItemHasItemProperty", R_INT, {R_OBJECT, R_INT}, &routine::GetItemHasItemProperty);
    routines.insert(401, "GetItemACValue", R_INT, {R_OBJECT}, &routine::GetItemACValue);
    routines.insert(403, "ExploreAreaForPlayer", R_VOID, {R_OBJECT, R_OBJECT}, &routine::ExploreAreaForPlayer);
    routines.insert(405, "GetIsDay", R_INT, {}, &routine::GetIsDay);
    routines.insert(406, "GetIsNight", R_INT, {}, &routine::GetIsNight);
    routines.insert(407, "GetIsDawn", R_INT, {}, &routine::GetIsDawn);
    routines.insert(408, "GetIsDusk", R_INT, {}, &routine::GetIsDusk);
    routines.insert(409, "GetIsEncounterCreature", R_INT, {R_OBJECT}, &routine::GetIsEncounterCreature);
    routines.insert(410, "GetLastPlayerDying", R_OBJECT, {}, &routine::GetLastPlayerDying);
    routines.insert(411, "GetStartingLocation", R_LOCATION, {}, &routine::GetStartingLocation);
    routines.insert(412, "ChangeToStandardFaction", R_VOID, {R_OBJECT, R_INT}, &routine::ChangeToStandardFaction);
    routines.insert(413, "SoundObjectPlay", R_VOID, {R_OBJECT}, &routine::SoundObjectPlay);
    routines.insert(414, "SoundObjectStop", R_VOID, {R_OBJECT}, &routine::SoundObjectStop);
    routines.insert(415, "SoundObjectSetVolume", R_VOID, {R_OBJECT, R_INT}, &routine::SoundObjectSetVolume);
    routines.insert(416, "SoundObjectSetPosition", R_VOID, {R_OBJECT, R_VECTOR}, &routine::SoundObjectSetPosition);
    routines.insert(417, "SpeakOneLinerConversation", R_VOID, {R_STRING, R_OBJECT}, &routine::SpeakOneLinerConversation);
    routines.insert(418, "GetGold", R_INT, {R_OBJECT}, &routine::GetGold);
    routines.insert(419, "GetLastRespawnButtonPresser", R_OBJECT, {}, &routine::GetLastRespawnButtonPresser);
    routines.insert(421, "SetLightsaberPowered", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &routine::SetLightsaberPowered);
    routines.insert(422, "GetIsWeaponEffective", R_INT, {R_OBJECT, R_INT}, &routine::GetIsWeaponEffective);
    routines.insert(423, "GetLastSpellHarmful", R_INT, {}, &routine::GetLastSpellHarmful);
    routines.insert(424, "EventActivateItem", R_EVENT, {R_OBJECT, R_LOCATION, R_OBJECT}, &routine::EventActivateItem);
    routines.insert(425, "MusicBackgroundPlay", R_VOID, {R_OBJECT}, &routine::MusicBackgroundPlay);
    routines.insert(426, "MusicBackgroundStop", R_VOID, {R_OBJECT}, &routine::MusicBackgroundStop);
    routines.insert(427, "MusicBackgroundSetDelay", R_VOID, {R_OBJECT, R_INT}, &routine::MusicBackgroundSetDelay);
    routines.insert(428, "MusicBackgroundChangeDay", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::MusicBackgroundChangeDay);
    routines.insert(429, "MusicBackgroundChangeNight", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::MusicBackgroundChangeNight);
    routines.insert(430, "MusicBattlePlay", R_VOID, {R_OBJECT}, &routine::MusicBattlePlay);
    routines.insert(431, "MusicBattleStop", R_VOID, {R_OBJECT}, &routine::MusicBattleStop);
    routines.insert(432, "MusicBattleChange", R_VOID, {R_OBJECT, R_INT}, &routine::MusicBattleChange);
    routines.insert(433, "AmbientSoundPlay", R_VOID, {R_OBJECT}, &routine::AmbientSoundPlay);
    routines.insert(434, "AmbientSoundStop", R_VOID, {R_OBJECT}, &routine::AmbientSoundStop);
    routines.insert(435, "AmbientSoundChangeDay", R_VOID, {R_OBJECT, R_INT}, &routine::AmbientSoundChangeDay);
    routines.insert(436, "AmbientSoundChangeNight", R_VOID, {R_OBJECT, R_INT}, &routine::AmbientSoundChangeNight);
    routines.insert(437, "GetLastKiller", R_OBJECT, {}, &routine::GetLastKiller);
    routines.insert(438, "GetSpellCastItem", R_OBJECT, {}, &routine::GetSpellCastItem);
    routines.insert(439, "GetItemActivated", R_OBJECT, {}, &routine::GetItemActivated);
    routines.insert(440, "GetItemActivator", R_OBJECT, {}, &routine::GetItemActivator);
    routines.insert(441, "GetItemActivatedTargetLocation", R_LOCATION, {}, &routine::GetItemActivatedTargetLocation);
    routines.insert(442, "GetItemActivatedTarget", R_OBJECT, {}, &routine::GetItemActivatedTarget);
    routines.insert(443, "GetIsOpen", R_INT, {R_OBJECT}, &routine::GetIsOpen);
    routines.insert(444, "TakeGoldFromCreature", R_VOID, {R_INT, R_OBJECT, R_INT}, &routine::TakeGoldFromCreature);
    routines.insert(445, "GetIsInConversation", R_INT, {R_OBJECT}, &routine::GetIsInConversation);
    routines.insert(455, "GetPlotFlag", R_INT, {R_OBJECT}, &routine::GetPlotFlag);
    routines.insert(456, "SetPlotFlag", R_VOID, {R_OBJECT, R_INT}, &routine::SetPlotFlag);
    routines.insert(461, "SetDialogPlaceableCamera", R_VOID, {R_INT}, &routine::SetDialogPlaceableCamera);
    routines.insert(462, "GetSoloMode", R_INT, {}, &routine::GetSoloMode);
    routines.insert(464, "GetMaxStealthXP", R_INT, {}, &routine::GetMaxStealthXP);
    routines.insert(468, "SetMaxStealthXP", R_VOID, {R_INT}, &routine::SetMaxStealthXP);
    routines.insert(474, "GetCurrentStealthXP", R_INT, {}, &routine::GetCurrentStealthXP);
    routines.insert(475, "GetNumStackedItems", R_INT, {R_OBJECT}, &routine::GetNumStackedItems);
    routines.insert(476, "SurrenderToEnemies", R_VOID, {}, &routine::SurrenderToEnemies);
    routines.insert(478, "SetCurrentStealthXP", R_VOID, {R_INT}, &routine::SetCurrentStealthXP);
    routines.insert(479, "GetCreatureSize", R_INT, {R_OBJECT}, &routine::GetCreatureSize);
    routines.insert(480, "AwardStealthXP", R_VOID, {R_OBJECT}, &routine::AwardStealthXP);
    routines.insert(481, "GetStealthXPEnabled", R_INT, {}, &routine::GetStealthXPEnabled);
    routines.insert(482, "SetStealthXPEnabled", R_VOID, {R_INT}, &routine::SetStealthXPEnabled);
    routines.insert(486, "GetLastTrapDetected", R_OBJECT, {R_OBJECT}, &routine::GetLastTrapDetected);
    routines.insert(488, "GetNearestTrapToObject", R_OBJECT, {R_OBJECT, R_INT}, &routine::GetNearestTrapToObject);
    routines.insert(489, "GetAttemptedMovementTarget", R_OBJECT, {}, &routine::GetAttemptedMovementTarget);
    routines.insert(490, "GetBlockingCreature", R_OBJECT, {R_OBJECT}, &routine::GetBlockingCreature);
    routines.insert(491, "GetFortitudeSavingThrow", R_INT, {R_OBJECT}, &routine::GetFortitudeSavingThrow);
    routines.insert(492, "GetWillSavingThrow", R_INT, {R_OBJECT}, &routine::GetWillSavingThrow);
    routines.insert(493, "GetReflexSavingThrow", R_INT, {R_OBJECT}, &routine::GetReflexSavingThrow);
    routines.insert(494, "GetChallengeRating", R_FLOAT, {R_OBJECT}, &routine::GetChallengeRating);
    routines.insert(495, "GetFoundEnemyCreature", R_OBJECT, {R_OBJECT}, &routine::GetFoundEnemyCreature);
    routines.insert(496, "GetMovementRate", R_INT, {R_OBJECT}, &routine::GetMovementRate);
    routines.insert(497, "GetSubRace", R_INT, {R_OBJECT}, &routine::GetSubRace);
    routines.insert(498, "GetStealthXPDecrement", R_INT, {}, &routine::GetStealthXPDecrement);
    routines.insert(499, "SetStealthXPDecrement", R_VOID, {R_INT}, &routine::SetStealthXPDecrement);
    routines.insert(500, "DuplicateHeadAppearance", R_VOID, {R_OBJECT, R_OBJECT}, &routine::DuplicateHeadAppearance);
    routines.insert(503, "CutsceneAttack", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &routine::CutsceneAttack);
    routines.insert(504, "SetCameraMode", R_VOID, {R_OBJECT, R_INT}, &routine::SetCameraMode);
    routines.insert(505, "SetLockOrientationInDialog", R_VOID, {R_OBJECT, R_INT}, &routine::SetLockOrientationInDialog);
    routines.insert(506, "SetLockHeadFollowInDialog", R_VOID, {R_OBJECT, R_INT}, &routine::SetLockHeadFollowInDialog);
    routines.insert(507, "CutsceneMove", R_VOID, {R_OBJECT, R_VECTOR, R_INT}, &routine::CutsceneMove);
    routines.insert(508, "EnableVideoEffect", R_VOID, {R_INT}, &routine::EnableVideoEffect);
    routines.insert(509, "StartNewModule", R_VOID, {R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING, R_STRING}, &routine::StartNewModule);
    routines.insert(510, "DisableVideoEffect", R_VOID, {}, &routine::DisableVideoEffect);
    routines.insert(511, "GetWeaponRanged", R_INT, {R_OBJECT}, &routine::GetWeaponRanged);
    routines.insert(512, "DoSinglePlayerAutoSave", R_VOID, {}, &routine::DoSinglePlayerAutoSave);
    routines.insert(513, "GetGameDifficulty", R_INT, {}, &routine::GetGameDifficulty);
    routines.insert(514, "GetUserActionsPending", R_INT, {}, &routine::GetUserActionsPending);
    routines.insert(515, "RevealMap", R_VOID, {R_VECTOR, R_INT}, &routine::RevealMap);
    routines.insert(516, "SetTutorialWindowsEnabled", R_VOID, {R_INT}, &routine::SetTutorialWindowsEnabled);
    routines.insert(517, "ShowTutorialWindow", R_VOID, {R_INT}, &routine::ShowTutorialWindow);
    routines.insert(518, "StartCreditSequence", R_VOID, {R_INT}, &routine::StartCreditSequence);
    routines.insert(519, "IsCreditSequenceInProgress", R_INT, {}, &routine::IsCreditSequenceInProgress);
    routines.insert(522, "GetCurrentAction", R_INT, {R_OBJECT}, &routine::GetCurrentAction);
    routines.insert(523, "GetDifficultyModifier", R_FLOAT, {}, &routine::GetDifficultyModifier);
    routines.insert(524, "GetAppearanceType", R_INT, {R_OBJECT}, &routine::GetAppearanceType);
    routines.insert(525, "FloatingTextStrRefOnCreature", R_VOID, {R_INT, R_OBJECT, R_INT}, &routine::FloatingTextStrRefOnCreature);
    routines.insert(526, "FloatingTextStringOnCreature", R_VOID, {R_STRING, R_OBJECT, R_INT}, &routine::FloatingTextStringOnCreature);
    routines.insert(527, "GetTrapDisarmable", R_INT, {R_OBJECT}, &routine::GetTrapDisarmable);
    routines.insert(528, "GetTrapDetectable", R_INT, {R_OBJECT}, &routine::GetTrapDetectable);
    routines.insert(529, "GetTrapDetectedBy", R_INT, {R_OBJECT, R_OBJECT}, &routine::GetTrapDetectedBy);
    routines.insert(530, "GetTrapFlagged", R_INT, {R_OBJECT}, &routine::GetTrapFlagged);
    routines.insert(531, "GetTrapBaseType", R_INT, {R_OBJECT}, &routine::GetTrapBaseType);
    routines.insert(532, "GetTrapOneShot", R_INT, {R_OBJECT}, &routine::GetTrapOneShot);
    routines.insert(533, "GetTrapCreator", R_OBJECT, {R_OBJECT}, &routine::GetTrapCreator);
    routines.insert(534, "GetTrapKeyTag", R_STRING, {R_OBJECT}, &routine::GetTrapKeyTag);
    routines.insert(535, "GetTrapDisarmDC", R_INT, {R_OBJECT}, &routine::GetTrapDisarmDC);
    routines.insert(536, "GetTrapDetectDC", R_INT, {R_OBJECT}, &routine::GetTrapDetectDC);
    routines.insert(537, "GetLockKeyRequired", R_INT, {R_OBJECT}, &routine::GetLockKeyRequired);
    routines.insert(538, "GetLockKeyTag", R_INT, {R_OBJECT}, &routine::GetLockKeyTag);
    routines.insert(539, "GetLockLockable", R_INT, {R_OBJECT}, &routine::GetLockLockable);
    routines.insert(540, "GetLockUnlockDC", R_INT, {R_OBJECT}, &routine::GetLockUnlockDC);
    routines.insert(541, "GetLockLockDC", R_INT, {R_OBJECT}, &routine::GetLockLockDC);
    routines.insert(542, "GetPCLevellingUp", R_OBJECT, {}, &routine::GetPCLevellingUp);
    routines.insert(543, "GetHasFeatEffect", R_INT, {R_INT, R_OBJECT}, &routine::GetHasFeatEffect);
    routines.insert(544, "SetPlaceableIllumination", R_VOID, {R_OBJECT, R_INT}, &routine::SetPlaceableIllumination);
    routines.insert(545, "GetPlaceableIllumination", R_INT, {R_OBJECT}, &routine::GetPlaceableIllumination);
    routines.insert(546, "GetIsPlaceableObjectActionPossible", R_INT, {R_OBJECT, R_INT}, &routine::GetIsPlaceableObjectActionPossible);
    routines.insert(547, "DoPlaceableObjectAction", R_VOID, {R_OBJECT, R_INT}, &routine::DoPlaceableObjectAction);
    routines.insert(548, "GetFirstPC", R_OBJECT, {}, &routine::GetFirstPC);
    routines.insert(549, "GetNextPC", R_OBJECT, {}, &routine::GetNextPC);
    routines.insert(550, "SetTrapDetectedBy", R_INT, {R_OBJECT, R_OBJECT}, &routine::SetTrapDetectedBy);
    routines.insert(551, "GetIsTrapped", R_INT, {R_OBJECT}, &routine::GetIsTrapped);
    routines.insert(552, "SetEffectIcon", R_EFFECT, {R_EFFECT, R_INT}, &routine::SetEffectIcon);
    routines.insert(553, "FaceObjectAwayFromObject", R_VOID, {R_OBJECT, R_OBJECT}, &routine::FaceObjectAwayFromObject);
    routines.insert(554, "PopUpDeathGUIPanel", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT, R_STRING}, &routine::PopUpDeathGUIPanel);
    routines.insert(555, "SetTrapDisabled", R_VOID, {R_OBJECT}, &routine::SetTrapDisabled);
    routines.insert(556, "GetLastHostileActor", R_OBJECT, {R_OBJECT}, &routine::GetLastHostileActor);
    routines.insert(557, "ExportAllCharacters", R_VOID, {}, &routine::ExportAllCharacters);
    routines.insert(558, "MusicBackgroundGetDayTrack", R_INT, {R_OBJECT}, &routine::MusicBackgroundGetDayTrack);
    routines.insert(559, "MusicBackgroundGetNightTrack", R_INT, {R_OBJECT}, &routine::MusicBackgroundGetNightTrack);
    routines.insert(560, "WriteTimestampedLogEntry", R_VOID, {R_STRING}, &routine::WriteTimestampedLogEntry);
    routines.insert(561, "GetModuleName", R_STRING, {}, &routine::GetModuleName);
    routines.insert(562, "GetFactionLeader", R_OBJECT, {R_OBJECT}, &routine::GetFactionLeader);
    routines.insert(564, "EndGame", R_VOID, {R_INT}, &routine::EndGame);
    routines.insert(565, "GetRunScriptVar", R_INT, {}, &routine::GetRunScriptVar);
    routines.insert(566, "GetCreatureMovmentType", R_INT, {R_OBJECT}, &routine::GetCreatureMovmentType);
    routines.insert(567, "AmbientSoundSetDayVolume", R_VOID, {R_OBJECT, R_INT}, &routine::AmbientSoundSetDayVolume);
    routines.insert(568, "AmbientSoundSetNightVolume", R_VOID, {R_OBJECT, R_INT}, &routine::AmbientSoundSetNightVolume);
    routines.insert(569, "MusicBackgroundGetBattleTrack", R_INT, {R_OBJECT}, &routine::MusicBackgroundGetBattleTrack);
    routines.insert(570, "GetHasInventory", R_INT, {R_OBJECT}, &routine::GetHasInventory);
    routines.insert(571, "GetStrRefSoundDuration", R_FLOAT, {R_INT}, &routine::GetStrRefSoundDuration);
    routines.insert(572, "AddToParty", R_VOID, {R_OBJECT, R_OBJECT}, &routine::AddToParty);
    routines.insert(573, "RemoveFromParty", R_VOID, {R_OBJECT}, &routine::RemoveFromParty);
    routines.insert(574, "AddPartyMember", R_INT, {R_INT, R_OBJECT}, &routine::AddPartyMember);
    routines.insert(575, "RemovePartyMember", R_INT, {R_INT}, &routine::RemovePartyMember);
    routines.insert(576, "IsObjectPartyMember", R_INT, {R_OBJECT}, &routine::IsObjectPartyMember);
    routines.insert(577, "GetPartyMemberByIndex", R_OBJECT, {R_INT}, &routine::GetPartyMemberByIndex);
    routines.insert(578, "GetGlobalBoolean", R_INT, {R_STRING}, &routine::GetGlobalBoolean);
    routines.insert(579, "SetGlobalBoolean", R_VOID, {R_STRING, R_INT}, &routine::SetGlobalBoolean);
    routines.insert(580, "GetGlobalNumber", R_INT, {R_STRING}, &routine::GetGlobalNumber);
    routines.insert(581, "SetGlobalNumber", R_VOID, {R_STRING, R_INT}, &routine::SetGlobalNumber);
    routines.insert(582, "AurPostString", R_VOID, {R_STRING, R_INT, R_INT, R_FLOAT}, &routine::AurPostString);
    routines.insert(669, "AddJournalWorldEntry", R_VOID, {R_INT, R_STRING, R_STRING}, &routine::AddJournalWorldEntry);
    routines.insert(670, "AddJournalWorldEntryStrref", R_VOID, {R_INT, R_INT}, &routine::AddJournalWorldEntryStrref);
    routines.insert(671, "BarkString", R_VOID, {R_OBJECT, R_INT, R_INT, R_INT}, &routine::BarkString);
    routines.insert(672, "DeleteJournalWorldAllEntries", R_VOID, {}, &routine::DeleteJournalWorldAllEntries);
    routines.insert(673, "DeleteJournalWorldEntry", R_VOID, {R_INT}, &routine::DeleteJournalWorldEntry);
    routines.insert(674, "DeleteJournalWorldEntryStrref", R_VOID, {R_INT}, &routine::DeleteJournalWorldEntryStrref);
    routines.insert(677, "PlayVisualAreaEffect", R_VOID, {R_INT, R_LOCATION}, &routine::PlayVisualAreaEffect);
    routines.insert(678, "SetJournalQuestEntryPicture", R_VOID, {R_STRING, R_OBJECT, R_INT, R_INT, R_INT}, &routine::SetJournalQuestEntryPicture);
    routines.insert(679, "GetLocalBoolean", R_INT, {R_OBJECT, R_INT}, &routine::GetLocalBoolean);
    routines.insert(680, "SetLocalBoolean", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::SetLocalBoolean);
    routines.insert(681, "GetLocalNumber", R_INT, {R_OBJECT, R_INT}, &routine::GetLocalNumber);
    routines.insert(682, "SetLocalNumber", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::SetLocalNumber);
    routines.insert(689, "SoundObjectGetPitchVariance", R_FLOAT, {R_OBJECT}, &routine::SoundObjectGetPitchVariance);
    routines.insert(690, "SoundObjectSetPitchVariance", R_VOID, {R_OBJECT, R_FLOAT}, &routine::SoundObjectSetPitchVariance);
    routines.insert(691, "SoundObjectGetVolume", R_INT, {R_OBJECT}, &routine::SoundObjectGetVolume);
    routines.insert(692, "GetGlobalLocation", R_LOCATION, {R_STRING}, &routine::GetGlobalLocation);
    routines.insert(693, "SetGlobalLocation", R_VOID, {R_STRING, R_LOCATION}, &routine::SetGlobalLocation);
    routines.insert(694, "AddAvailableNPCByObject", R_INT, {R_INT, R_OBJECT}, &routine::AddAvailableNPCByObject);
    routines.insert(695, "RemoveAvailableNPC", R_INT, {R_INT}, &routine::RemoveAvailableNPC);
    routines.insert(696, "IsAvailableCreature", R_INT, {R_INT}, &routine::IsAvailableCreature);
    routines.insert(697, "AddAvailableNPCByTemplate", R_INT, {R_INT, R_STRING}, &routine::AddAvailableNPCByTemplate);
    routines.insert(698, "SpawnAvailableNPC", R_OBJECT, {R_INT, R_LOCATION}, &routine::SpawnAvailableNPC);
    routines.insert(699, "IsNPCPartyMember", R_INT, {R_INT}, &routine::IsNPCPartyMember);
    routines.insert(701, "GetIsConversationActive", R_INT, {}, &routine::GetIsConversationActive);
    routines.insert(704, "GetPartyAIStyle", R_INT, {}, &routine::GetPartyAIStyle);
    routines.insert(705, "GetNPCAIStyle", R_INT, {R_OBJECT}, &routine::GetNPCAIStyle);
    routines.insert(706, "SetPartyAIStyle", R_VOID, {R_INT}, &routine::SetPartyAIStyle);
    routines.insert(707, "SetNPCAIStyle", R_VOID, {R_OBJECT, R_INT}, &routine::SetNPCAIStyle);
    routines.insert(708, "SetNPCSelectability", R_VOID, {R_INT, R_INT}, &routine::SetNPCSelectability);
    routines.insert(709, "GetNPCSelectability", R_INT, {R_INT}, &routine::GetNPCSelectability);
    routines.insert(710, "ClearAllEffects", R_VOID, {}, &routine::ClearAllEffects);
    routines.insert(711, "GetLastConversation", R_STRING, {}, &routine::GetLastConversation);
    routines.insert(712, "ShowPartySelectionGUI", R_VOID, {R_STRING, R_INT, R_INT, R_INT}, &routine::ShowPartySelectionGUI);
    routines.insert(713, "GetStandardFaction", R_INT, {R_OBJECT}, &routine::GetStandardFaction);
    routines.insert(714, "GivePlotXP", R_VOID, {R_STRING, R_INT}, &routine::GivePlotXP);
    routines.insert(715, "GetMinOneHP", R_INT, {R_OBJECT}, &routine::GetMinOneHP);
    routines.insert(716, "SetMinOneHP", R_VOID, {R_OBJECT, R_INT}, &routine::SetMinOneHP);
    routines.insert(719, "SetGlobalFadeIn", R_VOID, {R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT}, &routine::SetGlobalFadeIn);
    routines.insert(720, "SetGlobalFadeOut", R_VOID, {R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT, R_FLOAT}, &routine::SetGlobalFadeOut);
    routines.insert(721, "GetLastHostileTarget", R_OBJECT, {R_OBJECT}, &routine::GetLastHostileTarget);
    routines.insert(722, "GetLastAttackAction", R_INT, {R_OBJECT}, &routine::GetLastAttackAction);
    routines.insert(723, "GetLastForcePowerUsed", R_INT, {R_OBJECT}, &routine::GetLastForcePowerUsed);
    routines.insert(724, "GetLastCombatFeatUsed", R_INT, {R_OBJECT}, &routine::GetLastCombatFeatUsed);
    routines.insert(725, "GetLastAttackResult", R_INT, {R_OBJECT}, &routine::GetLastAttackResult);
    routines.insert(726, "GetWasForcePowerSuccessful", R_INT, {R_OBJECT}, &routine::GetWasForcePowerSuccessful);
    routines.insert(727, "GetFirstAttacker", R_OBJECT, {R_OBJECT}, &routine::GetFirstAttacker);
    routines.insert(728, "GetNextAttacker", R_OBJECT, {R_OBJECT}, &routine::GetNextAttacker);
    routines.insert(729, "SetFormation", R_VOID, {R_OBJECT, R_OBJECT, R_INT, R_INT}, &routine::SetFormation);
    routines.insert(731, "SetForcePowerUnsuccessful", R_VOID, {R_INT, R_OBJECT}, &routine::SetForcePowerUnsuccessful);
    routines.insert(732, "GetIsDebilitated", R_INT, {R_OBJECT}, &routine::GetIsDebilitated);
    routines.insert(733, "PlayMovie", R_VOID, {R_STRING, R_INT}, &routine::PlayMovie);
    routines.insert(734, "SaveNPCState", R_VOID, {R_INT}, &routine::SaveNPCState);
    routines.insert(735, "GetCategoryFromTalent", R_INT, {R_TALENT}, &routine::GetCategoryFromTalent);
    routines.insert(736, "SurrenderByFaction", R_VOID, {R_INT, R_INT}, &routine::SurrenderByFaction);
    routines.insert(737, "ChangeFactionByFaction", R_VOID, {R_INT, R_INT}, &routine::ChangeFactionByFaction);
    routines.insert(738, "PlayRoomAnimation", R_VOID, {R_STRING, R_INT}, &routine::PlayRoomAnimation);
    routines.insert(739, "ShowGalaxyMap", R_VOID, {R_INT}, &routine::ShowGalaxyMap);
    routines.insert(740, "SetPlanetSelectable", R_VOID, {R_INT, R_INT}, &routine::SetPlanetSelectable);
    routines.insert(741, "GetPlanetSelectable", R_INT, {R_INT}, &routine::GetPlanetSelectable);
    routines.insert(742, "SetPlanetAvailable", R_VOID, {R_INT, R_INT}, &routine::SetPlanetAvailable);
    routines.insert(743, "GetPlanetAvailable", R_INT, {R_INT}, &routine::GetPlanetAvailable);
    routines.insert(744, "GetSelectedPlanet", R_INT, {}, &routine::GetSelectedPlanet);
    routines.insert(745, "SoundObjectFadeAndStop", R_VOID, {R_OBJECT, R_FLOAT}, &routine::SoundObjectFadeAndStop);
    routines.insert(746, "SetAreaFogColor", R_VOID, {R_OBJECT, R_FLOAT, R_FLOAT, R_FLOAT}, &routine::SetAreaFogColor);
    routines.insert(747, "ChangeItemCost", R_VOID, {R_STRING, R_FLOAT}, &routine::ChangeItemCost);
    routines.insert(748, "GetIsLiveContentAvailable", R_INT, {R_INT}, &routine::GetIsLiveContentAvailable);
    routines.insert(749, "ResetDialogState", R_VOID, {}, &routine::ResetDialogState);
    routines.insert(750, "SetGoodEvilValue", R_VOID, {R_OBJECT, R_INT}, &routine::SetGoodEvilValue);
    routines.insert(751, "GetIsPoisoned", R_INT, {R_OBJECT}, &routine::GetIsPoisoned);
    routines.insert(752, "GetSpellTarget", R_OBJECT, {R_OBJECT}, &routine::GetSpellTarget);
    routines.insert(753, "SetSoloMode", R_VOID, {R_INT}, &routine::SetSoloMode);
    routines.insert(757, "CancelPostDialogCharacterSwitch", R_VOID, {}, &routine::CancelPostDialogCharacterSwitch);
    routines.insert(758, "SetMaxHitPoints", R_VOID, {R_OBJECT, R_INT}, &routine::SetMaxHitPoints);
    routines.insert(759, "NoClicksFor", R_VOID, {R_FLOAT}, &routine::NoClicksFor);
    routines.insert(760, "HoldWorldFadeInForDialog", R_VOID, {}, &routine::HoldWorldFadeInForDialog);
    routines.insert(761, "ShipBuild", R_INT, {}, &routine::ShipBuild);
    routines.insert(762, "SurrenderRetainBuffs", R_VOID, {}, &routine::SurrenderRetainBuffs);
    routines.insert(763, "SuppressStatusSummaryEntry", R_VOID, {R_INT}, &routine::SuppressStatusSummaryEntry);
    routines.insert(764, "GetCheatCode", R_INT, {R_INT}, &routine::GetCheatCode);
    routines.insert(765, "SetMusicVolume", R_VOID, {R_FLOAT}, &routine::SetMusicVolume);
    routines.insert(766, "CreateItemOnFloor", R_OBJECT, {R_STRING, R_LOCATION, R_INT}, &routine::CreateItemOnFloor);
    routines.insert(767, "SetAvailableNPCId", R_VOID, {R_INT, R_OBJECT}, &routine::SetAvailableNPCId);
    routines.insert(768, "GetScriptParameter", R_INT, {R_INT}, &routine::GetScriptParameter);
    routines.insert(769, "SetFadeUntilScript", R_VOID, {}, &routine::SetFadeUntilScript);
    routines.insert(771, "GetItemComponent", R_INT, {}, &routine::GetItemComponent);
    routines.insert(772, "GetItemComponentPieceValue", R_INT, {}, &routine::GetItemComponentPieceValue);
    routines.insert(773, "ShowChemicalUpgradeScreen", R_VOID, {R_OBJECT}, &routine::ShowChemicalUpgradeScreen);
    routines.insert(774, "GetChemicals", R_INT, {}, &routine::GetChemicals);
    routines.insert(775, "GetChemicalPieceValue", R_INT, {}, &routine::GetChemicalPieceValue);
    routines.insert(776, "GetSpellForcePointCost", R_INT, {}, &routine::GetSpellForcePointCost);
    routines.insert(783, "GetFeatAcquired", R_INT, {R_INT, R_OBJECT}, &routine::GetFeatAcquired);
    routines.insert(784, "GetSpellAcquired", R_INT, {R_INT, R_OBJECT}, &routine::GetSpellAcquired);
    routines.insert(785, "ShowSwoopUpgradeScreen", R_VOID, {}, &routine::ShowSwoopUpgradeScreen);
    routines.insert(786, "GrantFeat", R_VOID, {R_INT, R_OBJECT}, &routine::GrantFeat);
    routines.insert(787, "GrantSpell", R_VOID, {R_INT, R_OBJECT}, &routine::GrantSpell);
    routines.insert(788, "SpawnMine", R_VOID, {R_INT, R_LOCATION, R_INT, R_INT, R_OBJECT}, &routine::SpawnMine);
    routines.insert(791, "SetFakeCombatState", R_VOID, {R_OBJECT, R_INT}, &routine::SetFakeCombatState);
    routines.insert(793, "GetOwnerDemolitionsSkill", R_INT, {R_OBJECT}, &routine::GetOwnerDemolitionsSkill);
    routines.insert(794, "SetOrientOnClick", R_VOID, {R_OBJECT, R_INT}, &routine::SetOrientOnClick);
    routines.insert(795, "GetInfluence", R_INT, {R_INT}, &routine::GetInfluence);
    routines.insert(796, "SetInfluence", R_VOID, {R_INT, R_INT}, &routine::SetInfluence);
    routines.insert(797, "ModifyInfluence", R_VOID, {R_INT, R_INT}, &routine::ModifyInfluence);
    routines.insert(798, "GetRacialSubType", R_INT, {R_OBJECT}, &routine::GetRacialSubType);
    routines.insert(799, "IncrementGlobalNumber", R_VOID, {R_STRING, R_INT}, &routine::IncrementGlobalNumber);
    routines.insert(800, "DecrementGlobalNumber", R_VOID, {R_STRING, R_INT}, &routine::DecrementGlobalNumber);
    routines.insert(801, "SetBonusForcePoints", R_VOID, {R_OBJECT, R_INT}, &routine::SetBonusForcePoints);
    routines.insert(802, "AddBonusForcePoints", R_VOID, {R_OBJECT, R_INT}, &routine::AddBonusForcePoints);
    routines.insert(803, "GetBonusForcePoints", R_VOID, {R_OBJECT}, &routine::GetBonusForcePoints);
    routines.insert(805, "IsMoviePlaying", R_INT, {}, &routine::IsMoviePlaying);
    routines.insert(806, "QueueMovie", R_VOID, {R_STRING, R_INT}, &routine::QueueMovie);
    routines.insert(807, "PlayMovieQueue", R_VOID, {R_INT}, &routine::PlayMovieQueue);
    routines.insert(808, "YavinHackDoorClose", R_VOID, {R_OBJECT}, &routine::YavinHackDoorClose);
    routines.insert(810, "IsStealthed", R_INT, {R_OBJECT}, &routine::IsStealthed);
    routines.insert(811, "IsMeditating", R_INT, {R_OBJECT}, &routine::IsMeditating);
    routines.insert(812, "IsInTotalDefense", R_INT, {R_OBJECT}, &routine::IsInTotalDefense);
    routines.insert(813, "SetHealTarget", R_VOID, {R_OBJECT, R_OBJECT}, &routine::SetHealTarget);
    routines.insert(814, "GetHealTarget", R_OBJECT, {R_OBJECT}, &routine::GetHealTarget);
    routines.insert(815, "GetRandomDestination", R_VECTOR, {R_OBJECT, R_INT}, &routine::GetRandomDestination);
    routines.insert(816, "IsFormActive", R_INT, {R_OBJECT, R_INT}, &routine::IsFormActive);
    routines.insert(817, "GetSpellFormMask", R_INT, {R_INT}, &routine::GetSpellFormMask);
    routines.insert(818, "GetSpellBaseForcePointCost", R_INT, {R_INT}, &routine::GetSpellBaseForcePointCost);
    routines.insert(819, "SetKeepStealthInDialog", R_VOID, {R_INT}, &routine::SetKeepStealthInDialog);
    routines.insert(820, "HasLineOfSight", R_INT, {R_VECTOR, R_VECTOR, R_OBJECT, R_OBJECT}, &routine::HasLineOfSight);
    routines.insert(821, "ShowDemoScreen", R_INT, {R_STRING, R_INT, R_INT, R_INT, R_INT}, &routine::ShowDemoScreen);
    routines.insert(822, "ForceHeartbeat", R_VOID, {R_OBJECT}, &routine::ForceHeartbeat);
    routines.insert(824, "IsRunning", R_INT, {R_OBJECT}, &routine::IsRunning);
    routines.insert(826, "SetForfeitConditions", R_VOID, {R_INT}, &routine::SetForfeitConditions);
    routines.insert(827, "GetLastForfeitViolation", R_INT, {}, &routine::GetLastForfeitViolation);
    routines.insert(828, "ModifyReflexSavingThrowBase", R_VOID, {R_OBJECT, R_INT}, &routine::ModifyReflexSavingThrowBase);
    routines.insert(829, "ModifyFortitudeSavingThrowBase", R_VOID, {R_OBJECT, R_INT}, &routine::ModifyFortitudeSavingThrowBase);
    routines.insert(830, "ModifyWillSavingThrowBase", R_VOID, {R_OBJECT, R_INT}, &routine::ModifyWillSavingThrowBase);
    routines.insert(831, "GetScriptStringParameter", R_STRING, {}, &routine::GetScriptStringParameter);
    routines.insert(832, "GetObjectPersonalSpace", R_FLOAT, {R_OBJECT}, &routine::GetObjectPersonalSpace);
    routines.insert(833, "AdjustCreatureAttributes", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::AdjustCreatureAttributes);
    routines.insert(834, "SetCreatureAILevel", R_VOID, {R_OBJECT, R_INT}, &routine::SetCreatureAILevel);
    routines.insert(835, "ResetCreatureAILevel", R_VOID, {R_OBJECT}, &routine::ResetCreatureAILevel);
    routines.insert(836, "AddAvailablePUPByTemplate", R_INT, {R_INT, R_STRING}, &routine::AddAvailablePUPByTemplate);
    routines.insert(837, "AddAvailablePUPByObject", R_INT, {R_INT, R_OBJECT}, &routine::AddAvailablePUPByObject);
    routines.insert(838, "AssignPUP", R_INT, {R_INT, R_INT}, &routine::AssignPUP);
    routines.insert(839, "SpawnAvailablePUP", R_OBJECT, {R_INT, R_LOCATION}, &routine::SpawnAvailablePUP);
    routines.insert(840, "AddPartyPuppet", R_INT, {R_INT, R_OBJECT}, &routine::AddPartyPuppet);
    routines.insert(841, "GetPUPOwner", R_OBJECT, {R_OBJECT}, &routine::GetPUPOwner);
    routines.insert(842, "GetIsPuppet", R_INT, {R_OBJECT}, &routine::GetIsPuppet);
    routines.insert(844, "GetIsPartyLeader", R_INT, {R_OBJECT}, &routine::GetIsPartyLeader);
    routines.insert(845, "GetPartyLeader", R_OBJECT, {}, &routine::GetPartyLeader);
    routines.insert(846, "RemoveNPCFromPartyToBase", R_INT, {R_INT}, &routine::RemoveNPCFromPartyToBase);
    routines.insert(847, "CreatureFlourishWeapon", R_VOID, {R_OBJECT}, &routine::CreatureFlourishWeapon);
    routines.insert(850, "ChangeObjectAppearance", R_VOID, {R_OBJECT, R_INT}, &routine::ChangeObjectAppearance);
    routines.insert(851, "GetIsXBox", R_INT, {}, &routine::GetIsXBox);
    routines.insert(854, "PlayOverlayAnimation", R_VOID, {R_OBJECT, R_INT}, &routine::PlayOverlayAnimation);
    routines.insert(855, "UnlockAllSongs", R_VOID, {}, &routine::UnlockAllSongs);
    routines.insert(856, "DisableMap", R_VOID, {R_INT}, &routine::DisableMap);
    routines.insert(857, "DetonateMine", R_VOID, {R_OBJECT}, &routine::DetonateMine);
    routines.insert(858, "DisableHealthRegen", R_VOID, {R_INT}, &routine::DisableHealthRegen);
    routines.insert(859, "SetCurrentForm", R_VOID, {R_OBJECT, R_INT}, &routine::SetCurrentForm);
    routines.insert(860, "SetDisableTransit", R_VOID, {R_INT}, &routine::SetDisableTransit);
    routines.insert(861, "SetInputClass", R_VOID, {R_INT}, &routine::SetInputClass);
    routines.insert(862, "SetForceAlwaysUpdate", R_VOID, {R_OBJECT, R_INT}, &routine::SetForceAlwaysUpdate);
    routines.insert(863, "EnableRain", R_VOID, {R_INT}, &routine::EnableRain);
    routines.insert(864, "DisplayMessageBox", R_VOID, {R_INT, R_STRING}, &routine::DisplayMessageBox);
    routines.insert(865, "DisplayDatapad", R_VOID, {R_OBJECT}, &routine::DisplayDatapad);
    routines.insert(866, "RemoveHeartbeat", R_VOID, {R_OBJECT}, &routine::RemoveHeartbeat);
    routines.insert(867, "RemoveEffectByID", R_VOID, {R_OBJECT, R_INT}, &routine::RemoveEffectByID);
    routines.insert(868, "RemoveEffectByExactMatch", R_VOID, {R_OBJECT, R_EFFECT}, &routine::RemoveEffectByExactMatch);
    routines.insert(869, "AdjustCreatureSkills", R_VOID, {R_OBJECT, R_INT, R_INT}, &routine::AdjustCreatureSkills);
    routines.insert(870, "GetSkillRankBase", R_INT, {R_INT, R_OBJECT}, &routine::GetSkillRankBase);
    routines.insert(871, "EnableRendering", R_VOID, {R_OBJECT, R_INT}, &routine::EnableRendering);
    routines.insert(872, "GetCombatActionsPending", R_INT, {R_OBJECT}, &routine::GetCombatActionsPending);
    routines.insert(873, "SaveNPCByObject", R_VOID, {R_INT, R_OBJECT}, &routine::SaveNPCByObject);
    routines.insert(874, "SavePUPByObject", R_VOID, {R_INT, R_OBJECT}, &routine::SavePUPByObject);
    routines.insert(875, "GetIsPlayerMadeCharacter", R_INT, {R_OBJECT}, &routine::GetIsPlayerMadeCharacter);
    routines.insert(876, "RebuildPartyTable", R_VOID, {}, &routine::RebuildPartyTable);
}

} // namespace game

} // namespace reone
