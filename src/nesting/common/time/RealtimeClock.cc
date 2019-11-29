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

#include "RealtimeClock.h"

#include <cmath>

namespace nesting {

Define_Module(RealtimeClock);

RealtimeClock::RealtimeClock()
    : oscillator(nullptr)
    , localTime(SimTime(0.0))
    , nextTick(nullptr)
    , driftRate(0.0)
{
}

RealtimeClock::~RealtimeClock()
{
}

void RealtimeClock::initialize()
{
    oscillator = getModuleFromPar<IOscillator>(par("oscillatorModule"), this);
    oscillator->subscribeConfigChanges(*this);
}

void RealtimeClock::scheduleNextTimestamp()
{
    // Cancel next tick.
    if (nextTick != nullptr) {
        oscillator->unsubscribeTick(*this, *nextTick);
        nextTick = nullptr;
    }

    // We only have to schedule the next timestamp if the event queue isn't empty
    if (!scheduledEvents.empty()) {
        std::shared_ptr<RealtimeClockTimestamp>& nextTimestamp = scheduledEvents.front();
        simtime_t idleTime = nextTimestamp->getLocalTime() - getLocalTime();
        // We have to round up to the next highest tick
        uint64_t idleTicks = static_cast<uint64_t>(ceil(idleTime / timeIncrementPerTick()));
        oscillator->subscribeTick(*this, idleTicks);
    }
}

simtime_t RealtimeClock::timeIncrementPerTick() const
{
    return SimTime(1, SIMTIME_S) / (oscillator->getFrequency() + driftRate);
}

std::shared_ptr<const IClock2Timestamp> RealtimeClock::subscribeDelta(IClock2TimestampListener& listener, simtime_t delta, uint64_t kind)
{
    // TODO
    return nullptr;
}

std::shared_ptr<const IClock2Timestamp> RealtimeClock::subscribeTimestamp(IClock2TimestampListener& listener, simtime_t timestamp, uint64_t kind)
{
    // TODO
    return nullptr;
}

void RealtimeClock::subscribeConfigChanges(IClock2ConfigListener& listener)
{
    configListeners.insert(&listener);
}

void RealtimeClock::unsubscribeConfigChanges(IClock2ConfigListener& listener)
{
    configListeners.erase(&listener);
}

simtime_t RealtimeClock::getLocalTime()
{
    // TODO
    return SimTime::ZERO;
}

void RealtimeClock::setLocalTime(simtime_t time)
{
    // TODO
}

double RealtimeClock::getClockResolution() const
{
    // TODO
    return 0.0;
}

double RealtimeClock::setClockResolution(double clockResolution)
{
    // TODO
    return 0.0;
}

double RealtimeClock::getDriftRate() const
{
    return driftRate;
}

void RealtimeClock::setDriftRate(double drift)
{
    // TODO
}

void RealtimeClock::onOscillatorTick(IOscillator& oscillator, const IOscillatorTick& tick)
{
    Enter_Method("oscillatorTick");

    // Precondition: There should be at least one scheduled event. Otherwise
    // this method shouldn't have been triggered.
    assert(!scheduledEvents.empty());

    // Pop next event from queue
    std::shared_ptr<const RealtimeClockTimestamp> currentEvent = scheduledEvents.front();
    scheduledEvents.pop_front();

    // Invariant: There must not be any timestamp event scheduled with
    // timestamp in the past.
    assert(localTime <= currentEvent->getLocalSchedulingTime());

    // Update local time
    localTime = currentEvent->getLocalSchedulingTime();
}

void RealtimeClock::onOscillatorFrequencyChange(IOscillator& oscillator, double oldFrequency, double newFrequency)
{
    Enter_Method_Silent();

    for (auto listener : configListeners) {
        listener->onClockResolutionChange(*this, oldFrequency, newFrequency);
    }
}

RealtimeClockTimestamp::RealtimeClockTimestamp(IClock2TimestampListener& listener, simtime_t localTime, simtime_t localSchedulingTime, uint64_t kind)
    : listener(listener)
    , localTime(localTime)
    , localSchedulingTime(localSchedulingTime)
    , kind(kind)
{
}

simtime_t RealtimeClockTimestamp::getLocalTime() const
{
    return localTime;
}

simtime_t RealtimeClockTimestamp::getLocalSchedulingTime() const
{
    return localSchedulingTime;
}

uint64_t RealtimeClockTimestamp::getKind() const
{
    return kind;
}

IClock2TimestampListener& RealtimeClockTimestamp::getListener()
{
    return listener;
}

} //namespace
