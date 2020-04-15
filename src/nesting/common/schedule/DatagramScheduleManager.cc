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

#include "nesting/common/schedule/DatagramScheduleManager.h"
#include "nesting/common/schedule/ScheduleFactory.h"

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/InitStages.h"

namespace nesting {

Define_Module(DatagramScheduleManager);

const SendDatagramEvent DatagramScheduleManager::initialAdminState() const
{
    SendDatagramEvent evt;
    evt.setPayloadSize(B(0));
    return evt;
}

std::shared_ptr<const Schedule<SendDatagramEvent>> DatagramScheduleManager::initialAdminSchedule() const
{
    Schedule<SendDatagramEvent>* scheduleRawPtr = ScheduleFactory::createDatagramSchedule(par("initialAdminSchedule"));
    return std::shared_ptr<const Schedule<SendDatagramEvent>>(scheduleRawPtr);
}

int DatagramScheduleManager::numInitStages() const
{
    return inet::NUM_INIT_STAGES;
}

void DatagramScheduleManager::initialize(int stage)
{
    // Initalize after assignment of L3 addresses. Otherwise L3 addresses in
    // default schedule cannot be resolved.
    if (stage == INITSTAGE_APPLICATION_LAYER) {
        ScheduleManager<SendDatagramEvent>::initialize();
    }
}

} // namespace nesting
