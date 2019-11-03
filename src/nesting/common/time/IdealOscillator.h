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

#ifndef __NESTINGNG_IDEALOSCILLATOR_H_
#define __NESTINGNG_IDEALOSCILLATOR_H_

#include <omnetpp.h>

#include <list>
#include <cstdint>
#include <iostream>

#include "nesting/common/time/IOscillator.h"

using namespace omnetpp;

namespace nesting {

/**
 * TODO - Generated class
 */
class IdealOscillator : public cSimpleModule, public IOscillator
{
public:
    struct TickEvent {
        TickEvent(IOscillatorListener* listener, uint64_t tick, uint64_t kind);
        IOscillatorListener* listener;
        uint64_t tick;
        uint64_t kind;
        bool operator<(const TickEvent& tickEvent) const;
        bool operator==(const TickEvent& tickEvent) const;
        bool operator!=(const TickEvent& tickEvent) const;
        friend std::ostream& operator<<(std::ostream& stream, const IdealOscillator::TickEvent* tickEvent);
    };
protected:
    bool tickEventNow;
    simtime_t tickRate;
    uint64_t lastTick;
    simtime_t timeOfLastTick;
    std::list<TickEvent*> scheduledEvents; // TODO use shared ptr
    cMessage tickMessage;
public:
    IdealOscillator();
    virtual ~IdealOscillator();
    virtual void subscribeTick(IOscillatorListener* listener, uint64_t idleTicks, uint64_t kind = 0) override;
    virtual void unsubscribeTicks(IOscillatorListener* listener, uint64_t kind) override;
    virtual uint64_t getCurrentTick() const override;
protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;

    /**
     * Should be called after update of event queue. (Re)schedules a
     * self-message for the next event in the event-queue. This method is
     * idempotent.
     */
    virtual void scheduleNextTick();
};

std::ostream& operator<<(std::ostream& stream, const IdealOscillator::TickEvent* tickEvent);


} //namespace
#endif
