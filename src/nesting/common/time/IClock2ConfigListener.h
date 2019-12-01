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

#ifndef NESTING_COMMON_TIME_ICLOCK2CONFIGLISTENER_H_
#define NESTING_COMMON_TIME_ICLOCK2CONFIGLISTENER_H_

#include <omnetpp.h>

#include "nesting/common/time/IClock2.h"

using namespace omnetpp;

namespace nesting {

class IClock2;

class IClock2ConfigListener {
public:
    virtual ~IClock2ConfigListener() {};

    virtual void onClockRateChange(IClock2& clock, double oldClockRate, double newClockRate) = 0;

    virtual void onDriftRateChange(IClock2& clock, double oldDriftRate, double newDriftRate) = 0;

    virtual void onPhaseJump(IClock2& clock, simtime_t oldTime, simtime_t newTime) = 0;
};

} // namespace nesting

#endif /* NESTING_COMMON_TIME_ICLOCK2CONFIGLISTENER_H_ */
