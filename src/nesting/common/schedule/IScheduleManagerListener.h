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

#ifndef NESTING_COMMON_SCHEDULE_ISCHEDULEMANAGERLISTENER_H_
#define NESTING_COMMON_SCHEDULE_ISCHEDULEMANAGERLISTENER_H_

#include <omnetpp.h>

#include "ScheduleManager.h"

namespace nesting {

class ScheduleManagerState;

template<typename T>
class IScheduleManagerListener {
public:
    virtual ~IScheduleManagerListener();

    virtual void onExecuteOperation(T operation);

    virtual void onCycleTimerStateChanged(CycleTimerState cycleTimerState);

    virtual void onListExecuteStateChanged(ListExecuteState listExecuteState);

    virtual void onListConfigStateChanged(ListConfigState listConfigState);
};

} /* namespace nesting */

#endif /* NESTING_COMMON_SCHEDULE_ISCHEDULEMANAGERLISTENER_H_ */
