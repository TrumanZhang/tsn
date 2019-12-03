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

#include "nesting/common/time/RealtimeClock.h"

#include <cmath>
#include <algorithm>

namespace nesting {

Define_Module(RealtimeClock);

RealtimeClock::RealtimeClock()
    : oscillator(nullptr)
    , localTime(SimTime(0.0))
    , nextTick(nullptr)
    , driftRate(0.0)
    , lastTick(0)
{
}

RealtimeClock::~RealtimeClock()
{
}

void RealtimeClock::initialize()
{
    oscillator = getModuleFromPar<IOscillator>(par("oscillatorModule"), this);
    oscillator->subscribeConfigChanges(*this);
    lastTick = oscillator->updateAndGetTickCount();

    WATCH(localTime);
    WATCH(driftRate);
    WATCH(lastTick);
    WATCH_LIST(scheduledEvents);
}

void RealtimeClock::scheduleNextTimestamp()
{
    // Cancel next tick.
    if (nextTick != nullptr) {
        oscillator->unsubscribeTick(*this, *nextTick);
        nextTick = nullptr;
    }

    // We only have to schedule the next timestamp if the event queue isn't empty
    if (!scheduledEvents.empty() && !isStopped()) {
        std::shared_ptr<RealtimeClockTimestamp>& nextTimestamp = scheduledEvents.front();
        simtime_t idleTime = nextTimestamp->getLocalTime() - updateAndGetLocalTime();
        // We have to round up to the next highest tick
        uint64_t idleTicks = static_cast<uint64_t>(std::ceil(idleTime / timeIncrementPerTick()));
        oscillator->subscribeTick(*this, idleTicks);
    }
}

simtime_t RealtimeClock::timeIncrementPerTick() const
{
    return SimTime(1, SIMTIME_S) / (oscillator->getFrequency() + driftRate);
}

std::shared_ptr<const IClock2Timestamp> RealtimeClock::subscribeDelta(IClock2TimestampListener& listener, simtime_t delta, uint64_t kind)
{
    Enter_Method_Silent();
    simtime_t currentTime = updateAndGetLocalTime();
    simtime_t eventTime = currentTime + delta;
    return subscribeTimestamp(listener, eventTime, kind);
}

std::shared_ptr<const IClock2Timestamp> RealtimeClock::subscribeTimestamp(IClock2TimestampListener& listener, simtime_t eventTime, uint64_t kind)
{
    Enter_Method_Silent();
    std::shared_ptr<RealtimeClockTimestamp> event = std::make_shared<RealtimeClockTimestamp>(listener, eventTime, kind);
    auto it = std::lower_bound(scheduledEvents.begin(), scheduledEvents.end(), event);
    if (it == scheduledEvents.end() || **it != *event) {
        scheduledEvents.insert(it, event);
    }
    scheduleNextTimestamp();
    return event;
}

void RealtimeClock::subscribeConfigChanges(IClock2ConfigListener& listener)
{
    configListeners.insert(&listener);
}

void RealtimeClock::unsubscribeConfigChanges(IClock2ConfigListener& listener)
{
    configListeners.erase(&listener);
}

simtime_t RealtimeClock::updateAndGetLocalTime()
{
    uint64_t elapsedTicks = oscillator->updateAndGetTickCount() - lastTick;
    localTime += elapsedTicks * timeIncrementPerTick();
    lastTick += elapsedTicks;
    return localTime;
}

void RealtimeClock::setLocalTime(simtime_t newTime)
{
    // Update time
    simtime_t oldTime = localTime;
    localTime = newTime;

    // If the new local time is in the future, then we have to fast forward all
    // events that are scheduled before the new time value.
    if (oldTime < localTime) {
        auto bound = std::upper_bound(
                scheduledEvents.begin(), 
                scheduledEvents.end(), 
                localTime,
                [](simtime_t time, std::shared_ptr<RealtimeClockTimestamp> event) {
                    return time < event->getLocalTime();
                });
        // Notify listeners
        for (auto it = scheduledEvents.begin(); it != bound; it++) {
            std::shared_ptr<RealtimeClockTimestamp> event = *it;
            event->getListener().onTimestamp(*this, *event);
        }
        // Remove events
        scheduledEvents.erase(scheduledEvents.begin(), bound);
    }

    // Notify config listeners
    for (IClock2ConfigListener* listener : configListeners) {
        listener->onPhaseJump(*this, oldTime, newTime);
    }
}

double RealtimeClock::getClockRate() const
{
    return oscillator->getFrequency();
}

void RealtimeClock::setClockRate(double clockRate)
{
    oscillator->setFrequency(clockRate);
}

double RealtimeClock::getDriftRate() const
{
    return driftRate;
}

void RealtimeClock::setDriftRate(double driftRate)
{
    // Update drift rate
    double oldDriftRate = this->driftRate;
    this->driftRate = driftRate;

    // Reschedule next event
    scheduleNextTimestamp();

    // Notify listeners
    for (IClock2ConfigListener* listener : configListeners) {
        listener->onDriftRateChange(*this, oldDriftRate, driftRate);
    }
}

bool RealtimeClock::isStopped()
{
    return oscillator->getFrequency() + driftRate < minEffectiveClockRate;
}

void RealtimeClock::onTick(IOscillator& oscillator, const IOscillatorTick& tick)
{
    Enter_Method("tick");

    // Precondition: There should be at least one scheduled event. Otherwise
    // this method shouldn't have been triggered.
    assert(!scheduledEvents.empty());

    // Invariant
    assert(&oscillator == this->oscillator);

    // Pop next event from queue
    std::shared_ptr<RealtimeClockTimestamp> currentEvent = scheduledEvents.front();
    scheduledEvents.pop_front();

    // Invariant: There must not be any timestamp event scheduled with
    // timestamp in the past.
    assert(localTime <= currentEvent->getLocalTime());

    // Update local time
    localTime = currentEvent->getLocalTime();
    lastTick = this->oscillator->updateAndGetTickCount();

    // Notify listener
    currentEvent->getListener().onTimestamp(*this, *currentEvent);
}

void RealtimeClock::onFrequencyChange(IOscillator& oscillator, double oldFrequency, double newFrequency)
{
    Enter_Method_Silent();
    
    // Reschedule next event
    scheduleNextTimestamp();

    // Notify listeners
    for (IClock2ConfigListener* listener : configListeners) {
        listener->onClockRateChange(*this, oldFrequency, newFrequency);
    }
}

RealtimeClockTimestamp::RealtimeClockTimestamp(IClock2TimestampListener& listener, simtime_t localTime, uint64_t kind)
    : listener(listener)
    , localTime(localTime)
    , kind(kind)
{
}

simtime_t RealtimeClockTimestamp::getLocalTime() const
{
    return localTime;
}

uint64_t RealtimeClockTimestamp::getKind() const
{
    return kind;
}

IClock2TimestampListener& RealtimeClockTimestamp::getListener() const
{
    return listener;
}

bool RealtimeClockTimestamp::operator==(const RealtimeClockTimestamp& other) const
{
    return this->localTime == other.localTime
            && this->kind == other.kind
            && &(this->listener) == &(other.listener);
}

bool RealtimeClockTimestamp::operator!=(const RealtimeClockTimestamp& other) const
{
    return !(*this == other);
}

bool RealtimeClockTimestamp::operator<(const RealtimeClockTimestamp& other) const
{
    if (this->localTime < other.localTime) {
        return true;
    } else if (this->localTime == other.localTime) {
        if (this->kind < other.kind) {
            return true;
        } else if (this->kind == other.kind) {
            return &(this->listener) < &(other.listener);
        }
    }
    return false;
}

} //namespace
