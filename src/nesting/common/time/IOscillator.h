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

#ifndef NESTING_COMMON_TIME_IOSCILLATOR_H_
#define NESTING_COMMON_TIME_IOSCILLATOR_H_

#include <cstdint>
#include <memory>

#include "nesting/common/time/IOscillatorTickListener.h"
#include "nesting/common/time/IOscillatorConfigListener.h"

namespace nesting {

class IOscillatorTick;
class IOscillatorTickListener;
class IOscillatorTickConfig;

/** Interface for oscillator implementations. */
class IOscillator {
public:
    virtual ~IOscillator() {};

    /**
     * Subscribes a tick event for a given listener. The kind value can be used
     * to create inverse mappings of oscillator ticks within subscribers.
     */
    virtual std::shared_ptr<const IOscillatorTick> subscribeTick(IOscillatorTickListener& listener, uint64_t idleTicks, uint64_t kind = 0) = 0;

    /**
     * Unsubscribes a listener from a tick event.
     */
    virtual void unsubscribeTick(IOscillatorTickListener& listener, const IOscillatorTick& tick) = 0;

    /**
     * Unsubscribes a listener from all tick events of a certain kind value.
     */
    virtual void unsubscribeTicks(IOscillatorTickListener& listener, uint64_t kind) = 0;

    /**
     * Unsubscribes all ticks scheduled for a given listener.
     */
    virtual void unsubscribeTicks(IOscillatorTickListener& listener) = 0;

    virtual void subscribeConfigChanges(IOscillatorConfigListener& listener) = 0;

    virtual void unsubscribeConfigChanges(IOscillatorConfigListener& listener) = 0;

    /**
     * Returns true if a given tick event is scheduled for a given listener.
     */
    virtual bool isTickScheduled(IOscillatorTickListener& listener, const IOscillatorTick& tick) const = 0;

    /**
     * Returns the frequency of the oscillator module.
     *
     * @return Oscillator frequency in Hz.
     */
    virtual double getFrequency() const = 0;

    /**
     * Adjusts the frequency of the oscillator. Calling causes a rescheduling
     * of future tick events.
     *
     * @param frequency New oscillator frequency in Hz.
     */
    virtual void setFrequency(double frequency) = 0;

    /**
     * Updates the internal state (tick counter) and returns its value. This
     * method can't be constant because of potential stochastic
     * implementations.
     */
    virtual uint64_t getTickCount() = 0;
};

/** Interface for oscillator ticks. */
class IOscillatorTick {
public:
    virtual ~IOscillatorTick() {};

    virtual uint64_t getTick() const = 0;

    virtual uint64_t getKind() const = 0;
};

} // namespace nesting

#endif /* NESTING_COMMON_TIME_IOSCILLATOR_H_ */
