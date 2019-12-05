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

#include "nesting/common/time/IdealClock.h"

namespace nesting {

Define_Module(IdealClock);

IdealClock::IdealClock()
    : oscillator(nullptr)
    , lastTick(0)
    , time(SimTime::ZERO)
{
}

void IdealClock::initialize()
{
    oscillator = getModuleFromPar<IdealOscillator>(par("oscillatorModule"), this);
}

simtime_t IdealClock::getTime()
{
    uint64_t idleTicks = oscillator->updateAndGetTickCount() - lastTick;
    time += idleTicks * getClockRate();
    return time;
}

simtime_t IdealClock::getClockRate()
{
    return SimTime(1, SIMTIME_S) / oscillator->getFrequency();
}

void IdealClock::subscribeTick(IClockListener* listener, unsigned idleTicks, short kind)
{
    auto tick = oscillator->subscribeTick(*this, idleTicks, kind);
    tickToListenerTable[tick] = listener;
}

void IdealClock::unsubscribeTicks(IClockListener* listener)
{
    for (auto it = tickToListenerTable.begin(); it != tickToListenerTable.end(); it++) {
        if (it->second == listener) {
            oscillator->unsubscribeTick(*this, *(it->first));
            it = tickToListenerTable.erase(it);
        }
    }
}

void IdealClock::onTick(IOscillator& oscillator, std::shared_ptr<const IOscillator::Tick> tick)
{
    IClockListener* listener = tickToListenerTable[tick];
    listener->tick(this, tick->getKind());
    tickToListenerTable.erase(tick);
}

} // namespace nesting
