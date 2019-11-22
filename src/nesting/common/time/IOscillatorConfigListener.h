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

#ifndef NESTING_COMMON_TIME_IOSCILLATORCONFIGLISTENER_H_
#define NESTING_COMMON_TIME_IOSCILLATORCONFIGLISTENER_H_

#include "nesting/common/time/IOscillator.h"

namespace nesting {

class IOscillator;

class IOscillatorConfigListener {
public:
    virtual ~IOscillatorConfigListener() {};

    virtual void onOscillatorFrequencyChange(IOscillator& oscillator, double oldFrequency, double newFrequency) = 0;
};

} // namespace nesting

#endif /* NESTING_COMMON_TIME_IOSCILLATORCONFIGLISTENER_H_ */
