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
    : tickRate(SimTime::getMaxTime())
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
    int frequency = par("frequency").intValue();
    if (frequency < 0) {
        throw cRuntimeError("Frequency parameter must be positive.");
    } else if (frequency == 0) {
        throw cRuntimeError("Frequency parameter must not be zero.");
    }

    tickRate = simtime_t(1.0) / frequency;

    WATCH(tickRate);
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

        IdealOscillatorTick* tickEvent = scheduledEvents.front();
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
        tickEvent->getListener()->onOscillatorTick(this, tickEvent);

        delete tickEvent;

        scheduleNextTick();

        tickEventNow = false;
    }
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

    IdealOscillatorTick* nextTickEvent = scheduledEvents.front();

    uint64_t currentTick = updateAndGetCurrentTick();
    uint64_t nextScheduledTick = nextTickEvent->getTick();

    // Monotonic increasing ticks
    assert(nextScheduledTick >= currentTick);

    scheduleAt(timeOfLastTick + (nextScheduledTick - currentTick) * tickRate, &tickMessage);
}

uint64_t IdealOscillator::updateAndGetCurrentTick()
{
    Enter_Method_Silent();

    uint64_t currentTick;
    if (tickEventNow) { // Check tick event flag to prevent numeric errors. TODO: Might not be necessary.
        currentTick = lastTick;
    } else {
        uint64_t elapsedTicks = std::floor((simTime() - timeOfLastTick) / tickRate);
        currentTick = lastTick + elapsedTicks;
    }

    // Postcondition: Monotonic increasing tick count
    assert(currentTick >= lastTick);

    return currentTick;
}

const IOscillatorTick* IdealOscillator::subscribeTick(IOscillatorListener* listener, uint64_t idleTicks, uint64_t kind)
{
    Enter_Method_Silent();

    // Calculate current tick count
    uint64_t currentTick = updateAndGetCurrentTick();

    // Create new tick event.
    IdealOscillatorTick* tickEvent = new IdealOscillatorTick();
    tickEvent->setListener(listener);
    tickEvent->setTick(currentTick + idleTicks);
    tickEvent->setKind(kind);

    // Find insert position of tick event with binary search.
    auto it = std::lower_bound(
            scheduledEvents.begin(),
            scheduledEvents.end(),
            tickEvent,
            [](IdealOscillatorTick* left, IdealOscillatorTick* right) { return *left < *right; }); // TODO move into own function and remove code duplication

    // Insert tick event in event queue.
    if (it == scheduledEvents.end() || **it != *tickEvent) {
        scheduledEvents.insert(it, tickEvent);
    }

    scheduleNextTick();

    return tickEvent;
}

void IdealOscillator::unsubscribeTick(IOscillatorListener* listener, const IOscillatorTick* tick)
{
    IdealOscillatorTick tickEvent;
    tickEvent.setListener(listener);
    tickEvent.setTick(tick->getTick());
    tickEvent.setKind(tick->getKind());

    // Find tick event
    auto it = std::lower_bound(
            scheduledEvents.begin(),
            scheduledEvents.end(),
            &tickEvent,
            [](IdealOscillatorTick* left, IdealOscillatorTick* right) { return *left < *right; } // TODO move into own function and remove code duplication
    );

    // Remove tick event if it's present within the event queue
    if (it != scheduledEvents.end() && **it == tickEvent) {
        scheduledEvents.erase(it);
    }

    scheduleNextTick();
}

void IdealOscillator::unsubscribeTicks(IOscillatorListener* listener, uint64_t kind)
{
    Enter_Method_Silent();

    // Remove tick events
    // TODO use erase-remove idiom
    for (auto it = scheduledEvents.begin(); it != scheduledEvents.end(); ) {
        if ((*it)->getListener() == listener && (*it)->getKind() == kind) {
            it = scheduledEvents.erase(it);
        } else {
            it++;
        }
    }

    scheduleNextTick();
}

IdealOscillatorTick::IdealOscillatorTick()
    : listener(nullptr)
    , tick(0)
    , kind(0)
{
}

IdealOscillatorTick::~IdealOscillatorTick()
{
}

IOscillatorListener* IdealOscillatorTick::getListener() const
{
    return listener;
}

void IdealOscillatorTick::setListener(IOscillatorListener* listener)
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
            return this->listener < tickEvent.listener;
        }
    }
    return false;
}

bool IdealOscillatorTick::operator==(const IdealOscillatorTick& tickEvent) const
{
    return this->listener == tickEvent.listener
            && this->tick == tickEvent.tick
            && this->kind == tickEvent.kind;
}

bool IdealOscillatorTick::operator!=(const IdealOscillatorTick& tickEvent) const
{
    return !(*this == tickEvent);
}

std::ostream& operator<<(std::ostream& stream, const IdealOscillatorTick* tickEvent)
{
    stream << "TickEvent[tick=\"" << tickEvent->tick
            << "\",kind=\"" << tickEvent->kind
            << "\",listener=\"" << tickEvent->listener << "\"]";
    return stream;
}

} //namespace
