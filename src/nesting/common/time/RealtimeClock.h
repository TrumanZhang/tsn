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

#ifndef __NESTINGNG_REALTIMECLOCK_H_
#define __NESTINGNG_REALTIMECLOCK_H_

#include <omnetpp.h>

#include "nesting/common/time/IClock2.h"
#include "nesting/common/time/IClock2TimestampListener.h"
#include "nesting/common/time/IClock2ConfigListener.h"

using namespace omnetpp;

namespace nesting {

/**
 * TODO - Generated class
 */
class RealtimeClock : public cSimpleModule, public IClock2
{
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
public:
    virtual std::shared_ptr<const IClock2Timestamp> subscribeDelta(IClock2TimestampListener& listener, simtime_t delta, uint64_t kind = 0) override;
    virtual std::shared_ptr<const IClock2Timestamp> subscribeTimestamp(IClock2TimestampListener& listener, simtime_t timestamp, uint64_t kind = 0) override;
    virtual void subscribeConfigChanges(IClock2ConfigListener& listener) override;
    virtual void unsubscribeConfigChanges(IClock2ConfigListener& listener) override;
    virtual simtime_t getTime() override;
    virtual void setTime(simtime_t time) override;
    virtual double getClockRate() const override;
    virtual double setClockRate(double clockRate) override;
    virtual double getSkew() const override;
    virtual void setSkew(double skew) override;
};

} //namespace

#endif
