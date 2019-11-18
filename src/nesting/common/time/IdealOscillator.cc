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

#include "IdealOscillator.h"

#include <algorithm>
#include <cmath>

namespace nesting {

Define_Module(IdealOscillator);

IdealOscillator::IdealOscillator()
    : frequency(1)
    , lastTick(0)
    , timeOfLastTick(SimTime::ZERO)
    , tickEventNow(false)
    , tickMessage(cMessage("TickMessage"))
{
}

IdealOscillator::~IdealOscillator()
{
}

void IdealOscillator::initialize()
{
    frequency = par("frequency").doubleValue();
    if (frequency < 0) {
        throw cRuntimeError("Frequency parameter must be positive.");
    } else if (frequency == 0) {
        throw cRuntimeError("Frequency parameter must not be zero.");
    }

    WATCH(frequency);
    WATCH(lastTick);
    WATCH(timeOfLastTick);
    WATCH_LIST(scheduledEvents);
}

void IdealOscillator::finish()
{
}

void IdealOscillator::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        tickEventNow = true;

        // Invariant: There has to be at least one event to be scheduled now.
        // Otherwise the self-message shouldn't exist.
        assert(!scheduledEvents.empty());

        std::shared_ptr<IdealOscillatorTick> tickEvent = scheduledEvents.front();
        scheduledEvents.pop_front();

        // Invariant: Monotonic increasing tick count
        assert(lastTick <= tickEvent->getTick());
        assert(timeOfLastTick <= simTime());
        // Invariant: (LastTick == CurrentTick) <=> (TimeOfLastTick == CurrentTime)
        assert((lastTick != tickEvent->getTick()) == (timeOfLastTick == simTime()));

        // Update last tick
        if (lastTick < tickEvent->getTick()) {
            timeOfLastTick = simTime();
            lastTick = tickEvent->getTick();
        }

        // Notify listener
        tickEvent->getListener().onOscillatorTick(*this, *tickEvent);

        scheduleNextTick();

        tickEventNow = false;
    }
}

simtime_t IdealOscillator::getTickInterval() const
{
    return simtime_t(1.0) / frequency;
}

void IdealOscillator::scheduleNextTick() {
    // Cancel current self message
    if (tickMessage.isScheduled()) {
        cancelEvent(&tickMessage);
    }

    // We don't have to do anything if there are no future events.
    if (scheduledEvents.empty()) {
        return;
    }

    std::shared_ptr<IdealOscillatorTick>& nextTickEvent = scheduledEvents.front();

    uint64_t currentTick = getTickCount();
    uint64_t nextScheduledTick = nextTickEvent->getTick();

    // Monotonic increasing ticks
    assert(nextScheduledTick >= currentTick);

    scheduleAt(timeOfLastTick + (nextScheduledTick - currentTick) * getTickInterval(), &tickMessage);
}

uint64_t IdealOscillator::getTickCount()
{
    Enter_Method_Silent();

    uint64_t currentTick;
    if (tickEventNow) { // Check tick event flag to prevent numeric errors. TODO: Might not be necessary. Flag can potentially be removed.
        currentTick = lastTick;
    } else {
        uint64_t elapsedTicks = std::floor((simTime() - timeOfLastTick) / getTickInterval());
        currentTick = lastTick + elapsedTicks;
    }

    // Postcondition: Monotonic increasing tick count
    assert(currentTick >= lastTick);

    return currentTick;
}

std::shared_ptr<const IOscillatorTick> IdealOscillator::subscribeTick(IOscillatorListener& listener, uint64_t idleTicks, uint64_t kind)
{
    Enter_Method_Silent();

    // Calculate current tick count
    uint64_t currentTick = getTickCount();

    // Create new tick event.
    std::shared_ptr<IdealOscillatorTick> tickEvent = std::make_shared<IdealOscillatorTick>(
            listener,
            currentTick + idleTicks,
            kind);

    // Find insert position of tick event with binary search.
    auto it = std::lower_bound(
            scheduledEvents.begin(),
            scheduledEvents.end(),
            tickEvent,
            [](std::shared_ptr<IdealOscillatorTick> left, std::shared_ptr<IdealOscillatorTick> right) { return *left < *right; }); // TODO move into own function and remove code duplication

    // Insert tick event in event queue.
    if (it == scheduledEvents.end() || **it != *tickEvent) {
        scheduledEvents.insert(it, tickEvent);
    }

    scheduleNextTick();

    return tickEvent;
}

void IdealOscillator::unsubscribeTick(IOscillatorListener& listener, const IOscillatorTick& tick)
{
    std::shared_ptr<IdealOscillatorTick> tickEvent = std::make_shared<IdealOscillatorTick>(listener, tick.getTick(), tick.getKind());

    // Find tick event
    auto it = std::lower_bound(
            scheduledEvents.begin(),
            scheduledEvents.end(),
            tickEvent,
            [](std::shared_ptr<IdealOscillatorTick> left, std::shared_ptr<IdealOscillatorTick> right) { return *left < *right; } // TODO move into own function and remove code duplication
    );

    // Remove tick event if it's present within the event queue
    if (it != scheduledEvents.end() && **it == *tickEvent) {
        scheduledEvents.erase(it);
    }

    scheduleNextTick();
}

void IdealOscillator::unsubscribeTicks(IOscillatorListener& listener, uint64_t kind)
{
    Enter_Method_Silent();

    scheduledEvents.erase(
            std::remove_if(
                    scheduledEvents.begin(),
                    scheduledEvents.end(),
                    [&](std::shared_ptr<IdealOscillatorTick> tickEvent) {
                        return &(tickEvent->getListener()) == &listener
                                && tickEvent->getKind() == kind;
                    }
            ), scheduledEvents.end()
    );

    scheduleNextTick();
}

void IdealOscillator::unsubscribeTicks(IOscillatorListener& listener)
{
    Enter_Method_Silent();

    scheduledEvents.erase(
            std::remove_if(
                    scheduledEvents.begin(),
                    scheduledEvents.end(),
                    [&](std::shared_ptr<IdealOscillatorTick> tickEvent) { return &(tickEvent->getListener()) == &listener; }
            ), scheduledEvents.end()
    );

    scheduleNextTick();
}

bool IdealOscillator::isScheduled(IOscillatorListener& listener, const IOscillatorTick& tickEvent) const
{
    std::shared_ptr<IdealOscillatorTick> idealOscillatorTick = std::make_shared<IdealOscillatorTick>(listener, tickEvent);
    auto it = std::lower_bound(
            scheduledEvents.begin(),
            scheduledEvents.end(),
            idealOscillatorTick,
            [](std::shared_ptr<IdealOscillatorTick> left, std::shared_ptr<IdealOscillatorTick> right) { return *left < *right; } // TODO move into own function and remove code duplication
    );
    return it != scheduledEvents.end() && **it == *idealOscillatorTick;
}

double IdealOscillator::getFrequency() const
{
    return frequency;
}

void IdealOscillator::setFrequency(double frequency)
{
    // Update frequency and reschedule next tick event.
    this->frequency = frequency;
    scheduleNextTick();
}

IdealOscillatorTick::IdealOscillatorTick(IOscillatorListener& listener, uint64_t tick, uint64_t kind)
    : listener(listener)
    , tick(tick)
    , kind(kind)
{
}

IdealOscillatorTick::IdealOscillatorTick(IOscillatorListener& listener, const IOscillatorTick& tickEvent)
    : listener(listener)
    , tick(tickEvent.getTick())
    , kind(tickEvent.getKind())
{
}

IdealOscillatorTick::~IdealOscillatorTick()
{
}

IOscillatorListener& IdealOscillatorTick::getListener() const
{
    return listener;
}

void IdealOscillatorTick::setListener(IOscillatorListener& listener)
{
    this->listener = listener;
}

uint64_t IdealOscillatorTick::getTick() const
{
    return tick;
}

void IdealOscillatorTick::setTick(uint64_t tick)
{
    this->tick = tick;
}

uint64_t IdealOscillatorTick::getKind() const
{
    return kind;
}

void IdealOscillatorTick::setKind(uint64_t kind)
{
    this->kind = kind;
}

bool IdealOscillatorTick::operator<(const IdealOscillatorTick& tickEvent) const
{
    if (this->tick < tickEvent.tick) {
        return true;
    } else if (this->tick == tickEvent.tick) {
        if (this->kind < tickEvent.kind) {
            return true;
        } else if (this->kind == tickEvent.kind) {
            return &(this->listener) < &(tickEvent.listener);
        }
    }
    return false;
}

bool IdealOscillatorTick::operator==(const IdealOscillatorTick& tickEvent) const
{
    return &(this->listener) == &(tickEvent.listener)
            && this->tick == tickEvent.tick
            && this->kind == tickEvent.kind;
}

bool IdealOscillatorTick::operator!=(const IdealOscillatorTick& tickEvent) const
{
    return !(*this == tickEvent);
}

std::ostream& operator<<(std::ostream& stream, const IdealOscillatorTick* tickEvent)
{
    stream << "TickEvent[tick=\"" << tickEvent->getTick()
            << "\",kind=\"" << tickEvent->getKind()
            << "\",listener=\"" << &(tickEvent->getListener()) << "\"]";
    return stream;
}

} //namespace
