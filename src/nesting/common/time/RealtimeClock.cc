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

namespace nesting {

Define_Module(RealtimeClock);

RealtimeClock::RealtimeClock()
    : oscillator(nullptr)
    , localTime(SimTime(0.0))
    , nextOscillatorTick(nullptr)
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

void RealtimeClock::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

void RealtimeClock::scheduleNextOscillatorTick()
{
    // TODO
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

double RealtimeClock::getDrift() const
{
    // TODO
    return 0.0;
}

void RealtimeClock::setDrift(double drift)
{
    // TODO
}

void RealtimeClock::onOscillatorTick(IOscillator& oscillator, const IOscillatorTick& tick)
{
    // TODO
}

void RealtimeClock::onOscillatorFrequencyChange(IOscillator& oscillator, double oldFrequency, double newFrequency)
{
    // TODO
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

IClock2TimestampListener& RealtimeClockTimestamp::getListener()
{
    return listener;
}

} //namespace
