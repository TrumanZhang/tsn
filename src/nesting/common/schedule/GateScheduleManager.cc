//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "nesting/common/schedule/GateScheduleManager.h"
#include "nesting/common/schedule/ScheduleBuilder.h"

namespace nesting {

Define_Module(GateScheduleManager);

GateScheduleManager::GateScheduleManager() {}

const GateBitvector GateScheduleManager::initialAdminState()
{
    return GateBitvector(par("initialAdminGateStates").stringValue());
}

std::shared_ptr<const Schedule<GateBitvector>> GateScheduleManager::initialAdminSchedule()
{
    cXMLElement* xml = par("initialAdminSchedule");
    Schedule<GateBitvector>* scheduleRawPtr = ScheduleBuilder::createGateBitvectorScheduleV2(xml);
    std::shared_ptr<Schedule<GateBitvector>> schedule(scheduleRawPtr);
    return schedule;
}

void GateScheduleManager::setAdminSchedule(std::shared_ptr<const Schedule<GateBitvector>> adminSchedule)
{
    if (adminSchedule->getControlListLength() == 0) {
        EV_WARN << "Loading a schedule with controlListLenth of 0." << std::endl;
    }
    ScheduleManager<GateBitvector>::setAdminSchedule(adminSchedule);
}

simtime_t GateScheduleManager::timeUntilGateCloseEvent(uint64_t gateIndex, simtime_t maxLookahead)
{
    // Preconditions
    assert(gateIndex >= 0 && gateIndex < 8);
    assert(maxLookahead >= 0);

    const simtime_t currentTime = clock->updateAndGetLocalTime();
    const uint64_t operControlListLength = operSchedule->getControlListLength();

    // TODO what if controlListLength == 0

    // Case 1: If gating is disabled we only have to check the current
    // operState and neither the operSchedule or adminSchedule.
    if (!enabled) {
        return operState.test(gateIndex) ? maxLookahead : SimTime::ZERO;
    }
    // Case 2: Gating is enabled and no schedule swap will be executed within
    // the maxLookahead time interval.
    else if (enabled && (!configPending || configChangeTime > currentTime + maxLookahead)) {
        simtime_t timeUntilGateCloseEvent = SimTime::ZERO;
        uint64_t lookaheadListPointer = listPointer;
        GateBitvector lookaheadOperState = operState;
        while (timeUntilGateCloseEvent <= maxLookahead && lookaheadOperState.test(gateIndex)) {
            // Update time to gate close event.
            timeUntilGateCloseEvent += operSchedule->getTimeInterval(lookaheadListPointer);
            // Update lookahead- listPointer and operState.
            lookaheadListPointer = (lookaheadListPointer + 1) % operControlListLength;
            GateBitvector lookaheadOperState = operSchedule->getScheduledObject(lookaheadListPointer);
        }

        return timeUntilGateCloseEvent > maxLookahead ? maxLookahead : timeUntilGateCloseEvent;
    }
    // Case 3: Gating is
}

} // namespace nesting
