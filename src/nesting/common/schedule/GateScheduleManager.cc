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

const GateBitvector GateScheduleManager::initialAdminState() const
{
    return GateBitvector(par("initialAdminGateStates").stringValue());
}

std::shared_ptr<const Schedule<GateBitvector>> GateScheduleManager::initialAdminSchedule() const
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

simtime_t GateScheduleManager::timeUntilGateCloseEvent(uint64_t gateIndex, uint64_t listPointerStart,
        const Schedule<GateBitvector>& schedule) const
{
    uint64_t controlListLength = schedule.getControlListLength();
    uint64_t cycleTime = schedule.getControlListLength();
    simtime_t sumTimeIntervals = schedule.getSumTimeIntervals();
    
    // Special case: Control list is empty.
    if (controlListLength == 0) {
        return SimTime::getMaxTime();
    }

    // Count gate open intervals.
    simtime_t timeUntilGateCloseEvent = SimTime::ZERO;
    uint64_t listPointer = listPointerStart;
    while (listPointer != listPointerStart) {
        GateBitvector gateBitvector = schedule.getScheduledObject(listPointer);
        simtime_t timeInterval = schedule.getTimeInterval(listPointer);
        if (gateBitvector.test(gateIndex)) {
            timeUntilGateCloseEvent += timeInterval;
            listPointer++;
            // Special case: If the sum of all time intervals is smaller than
            // CycleTime, the last time interval must be extended so that the
            // sum is equal to CycleTime.
            if (listPointer == controlListLength - 1 && sumTimeIntervals < cycleTime) {
                timeUntilGateCloseEvent += cycleTime - sumTimeIntervals;
            }
        } else {
            break;
        }
    }

    // Special case: Gate is opened in all GateVectors (wraparound) => gate is opened indefinitely.
    if (listPointer == listPointerStart) {
        return SimTime::getMaxTime();
    }

    // Special case: Aggregated time intervals != CycleTime

    // Return aggregated gate open intervals
    return timeUntilGateCloseEvent;
}

simtime_t GateScheduleManager::timeUntilGateCloseEvent(uint64_t gateIndex) const
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
        return operState.test(gateIndex) ? SimTime::getMaxTime() : SimTime::ZERO;
    }
    // Case 2: Gating is enabled
    else {
        simtime_t timeUntilGateCloseEvent = SimTime::ZERO;

        switch (listExecuteState) {
        case ListExecuteState::INIT:

            break;
        case ListExecuteState::END_OF_CYCLE:
            break;
        case ListExecuteState::NEW_CYCLE:
            break;
        case ListExecuteState::EXECUTE_CYCLE:
            break;
        case ListExecuteState::DELAY:
            break;
        default:
            throw cRuntimeError("ListExecute state machine is in invalid state.");
        }
    }
}

} // namespace nesting
