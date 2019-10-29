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

        // Invariant: There has to be at least one event to be scheduled next
        assert(!scheduledEvents.empty());

        TickEvent* tickEvent = scheduledEvents.front();
        scheduledEvents.pop_front();

        // Invariant: Monotonic increasing tick count
        assert(lastTick <= tickEvent->tick);
        assert(timeOfLastTick <= simTime());

        // Update last tick
        if (lastTick < tickEvent->tick) {
            timeOfLastTick = simTime();
            lastTick = tickEvent->tick;
        }

        // Notify listener
        tickEvent->listener->onTick(this, tickEvent->kind);

        delete tickEvent;

        tickEventNow = false;
    }
}

uint64_t IdealOscillator::getCurrentTick()
{
    uint64_t currentTick;
    if (tickEventNow) {
        currentTick = lastTick;
    } else {
        uint64_t elapsedTicks = std::floor((simTime() - timeOfLastTick) / tickRate);
        currentTick = lastTick + elapsedTicks;
    }

    // Postcondition: Monotonic increasing tick count
    assert(currentTick >= lastTick);

    return currentTick;
}

void IdealOscillator::subscribeTick(IOscillatorListener* listener, uint64_t idleTicks, uint64_t kind)
{
    Enter_Method_Silent();

    // Calculate current tick count
    uint64_t currentTick = getCurrentTick();

    EV_INFO << "Subscribe tick " << currentTick + idleTicks << " on listener "
            << listener << " of kind " << kind << "." << std::endl;

    // Create new tick event.
    TickEvent* tickEvent = new TickEvent(listener, currentTick + idleTicks, kind);

    // Find insert position of tick event with binary search.
    auto it = lower_bound(
            scheduledEvents.begin(),
            scheduledEvents.end(),
            tickEvent,
            [](TickEvent* left, TickEvent* right) { return *left < *right; });

    // If tick event is inserted at the front of the scheduled event queue, a
    // self message must be (re)scheduled.
    if (it == scheduledEvents.begin()) {
        if (tickMessage.isScheduled()) {
            cancelEvent(&tickMessage);
        }
        if (idleTicks == 0) {
            scheduleAt(simTime(), &tickMessage);
        } else {
            scheduleAt((currentTick - lastTick + idleTicks) * tickRate, &tickMessage);
        }
    }

    // Insert tick event in event queue.
    if (it == scheduledEvents.end() || **it != *tickEvent) {
        scheduledEvents.insert(it, tickEvent);
    }
}

void IdealOscillator::unsubscribeTicks(IOscillatorListener* listener, uint64_t kind)
{
    EV_INFO << "Unsubscribe ticks from listener " << listener << " of kind "
            << kind << "." << std::endl;

    // Remove tick events
    bool removedFrontEvent = false;
    for (auto it = scheduledEvents.begin(); it != scheduledEvents.end(); ) {
        if ((*it)->listener == listener && (*it)->kind == kind) {
            if (it == scheduledEvents.begin()) {
                removedFrontEvent = true;
            }
            it = scheduledEvents.erase(it);
        } else {
            it++;
        }
    }

    // If front event was removed and therefore the respective tick message
    // was canceled we must schedule a new tick (self) message.
    if (removedFrontEvent) {
        cancelEvent(&tickMessage);
        if (!scheduledEvents.empty()) {
            TickEvent* nextTickEvent = scheduledEvents.front();
            uint64_t currentTick = getCurrentTick();
            if (currentTick == lastTick) {
                scheduleAt(simTime(), &tickMessage);
            } else {
                scheduleAt((nextTickEvent->tick - currentTick) * tickRate, &tickMessage);
            }
        }
    }
}

IdealOscillator::TickEvent::TickEvent(IOscillatorListener* listener, uint64_t tick, uint64_t kind)
    : listener(listener)
    , tick(tick)
    , kind(kind)
{
}

bool IdealOscillator::TickEvent::operator<(const TickEvent& tickEvent) const
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

bool IdealOscillator::TickEvent::operator==(const TickEvent& tickEvent) const
{
    return this->listener == tickEvent.listener
            && this->tick == tickEvent.tick
            && this->kind == tickEvent.kind;
}

bool IdealOscillator::TickEvent::operator!=(const TickEvent& tickEvent) const
{
    return !(*this == tickEvent);
}

} //namespace
