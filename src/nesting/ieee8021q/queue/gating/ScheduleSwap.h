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

#ifndef __MAIN_SCHEDULESWAP_H_
#define __MAIN_SCHEDULESWAP_H_

#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"
#include "GateController.h"
#include "../../../application/ethernet/SchedEtherVLANTrafGen.h"
#include "../../clock/IClock.h"
#include "../../relay/FilteringDatabase.h"
#include "../../clock/IClockListener.h"
#include "../../Ieee8021q.h"

using namespace omnetpp;

namespace nesting {

/**
 * See the NED file for a detailed description
 */
class ScheduleSwap: public cSimpleModule, public IClockListener {
private:
    /** Current schedule. Is never null. */
    cXMLElement* scheduleXml;

    /** Index for the current entry in the schedule. */
    unsigned int scheduleIndex;

    /**
     * Clock reference, needed to get the current time and subscribe
     * clock events.
     */
    IClock* clock;
protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg);

    /** @see cSimpleModule::numInitStages() */
    virtual int numInitStages() const override;
public:
    /** @see IClockListener::tick(IClock*) */
    virtual void tick(IClock *clock) override;
};
}
#endif
