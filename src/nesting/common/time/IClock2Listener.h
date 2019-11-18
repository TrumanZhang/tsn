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

#ifndef NESTING_COMMON_TIME_ICLOCK2LISTENER_H_
#define NESTING_COMMON_TIME_ICLOCK2LISTENER_H_

namespace nesting {

class IClock2Listener {
    virtual ~IClock2Listener() {};

    virtual void onClockTimestampPass(IClock2& clock) {};

    virtual void onClockRateChange(IClock2& clock, double oldClockRate, double newClockRate) {};

    virtual void onClockSkewChange(IClock2& clock, double oldClockSkew, double newClockSkew) {};
};

} /* namespace nesting */

#endif /* NESTING_COMMON_TIME_ICLOCK2LISTENER_H_ */
