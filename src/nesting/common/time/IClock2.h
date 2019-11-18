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

#include "nesting/common/time/IClock2Listener.h"
#include "nesting/common/time/IOscillator.h"

namespace nesting {

class IClock2 {
public:
    virtual ~IClock2() {};

    virtual void subscribeDelta(IClock2Listener& listener, simtime_t delta) = 0;

    virtual void subscribeTimestamp(IClock2Listener& listener, simtime_t timestamp) = 0;

    virtual simtime_t getTime() = 0;

    virtual void setTime(simtime_t time) = 0;

    virtual uint64_t getEventCount() const = 0;

    virtual double getClockRate() const = 0;

    virtual double setClockRate(double clockRate) = 0;

    virtual double getSkew() const = 0;

    virtual void setSkew(double skew) = 0;
};

class IClock2Event {

};

} /* namespace nesting */

#endif /* NESTING_COMMON_TIME_ICLOCK2_H_ */
