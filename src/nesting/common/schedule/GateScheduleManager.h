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

#ifndef NESTING_COMMON_SCHEDULE_GATESCHEDULEMANAGER_H_
#define NESTING_COMMON_SCHEDULE_GATESCHEDULEMANAGER_H_

#include "nesting/common/schedule/ScheduleManager.h"
#include "nesting/common/schedule/Schedule.h"
#include "nesting/ieee8021q/Ieee8021q.h"

#include <memory>

namespace nesting {

class GateScheduleManager : public ScheduleManager<GateBitvector> {
public:
    GateScheduleManager();
protected:
    virtual const GateBitvector defaultAdminState() override;
    virtual std::shared_ptr<const Schedule<GateBitvector>> defaultAdminSchedule() override;
    virtual void setAdminSchedule(std::shared_ptr<const Schedule<GateBitvector>> adminSchedule) override;
};

} // namespace nesting

#endif
