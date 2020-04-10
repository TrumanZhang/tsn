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

namespace nesting {

Define_Module(GateScheduleManager);

GateScheduleManager::GateScheduleManager() {}

const GateBitvector GateScheduleManager::defaultAdminState()
{
    return GateBitvector("11111111");
}

std::shared_ptr<const Schedule<GateBitvector>> GateScheduleManager::defaultAdminSchedule()
{
    std::shared_ptr<Schedule<GateBitvector>> defaultSchedule = std::make_shared<Schedule<GateBitvector>>();
    defaultSchedule->setBaseTime(SimTime::ZERO);
    defaultSchedule->setCycleTime(SimTime(100, SIMTIME_US));
    defaultSchedule->addControlListEntry(SimTime(100, SIMTIME_US), GateBitvector("11111111"));
    return defaultSchedule;
}

void GateScheduleManager::setAdminSchedule(std::shared_ptr<const Schedule<GateBitvector>> adminSchedule)
{
    if (adminSchedule->getCycleTime() <= SimTime::ZERO) {
        throw cRuntimeError("Can't load schedule with cycle time of zero.");
    }
    ScheduleManager<GateBitvector>::setAdminSchedule(adminSchedule);
}

} // namespace nesting
