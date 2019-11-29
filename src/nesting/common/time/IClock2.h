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

#ifndef NESTING_COMMON_TIME_ICLOCK2_H_
#define NESTING_COMMON_TIME_ICLOCK2_H_

#include <omnetpp.h>

#include <memory>
#include <cstdint>

#include "nesting/common/time/IClock2TimestampListener.h"
#include "nesting/common/time/IClock2ConfigListener.h"

using namespace omnetpp;

namespace nesting {

class IClock2Timestamp;

class IClock2 {
public:
    virtual ~IClock2() {};

    virtual std::shared_ptr<const IClock2Timestamp> subscribeDelta(IClock2TimestampListener& listener, simtime_t delta, uint64_t kind = 0) = 0;

    virtual std::shared_ptr<const IClock2Timestamp> subscribeTimestamp(IClock2TimestampListener& listener, simtime_t timestamp, uint64_t kind = 0) = 0;

    virtual void subscribeConfigChanges(IClock2ConfigListener& listener) = 0;

    virtual void unsubscribeConfigChanges(IClock2ConfigListener& listener) = 0;

    virtual simtime_t getTime() = 0;

    virtual void setTime(simtime_t time) = 0;

    virtual double getClockResolution() const = 0;

    virtual double setClockResolution(double clockResolution) = 0;

    virtual double getDrift() const = 0;

    virtual void setDrift(double drift) = 0;
};

class IClock2Timestamp {
    virtual simtime_t getTimestamp() = 0;

    virtual uint64_t getKind() = 0;
};

} /* namespace nesting */

#endif /* NESTING_COMMON_TIME_ICLOCK2_H_ */
