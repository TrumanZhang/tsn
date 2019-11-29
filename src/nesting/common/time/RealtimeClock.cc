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

void RealtimeClock::initialize()
{
    // TODO - Generated method body
}

void RealtimeClock::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
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
    // TODO
}

void RealtimeClock::unsubscribeConfigChanges(IClock2ConfigListener& listener)
{
    // TODO
}

simtime_t RealtimeClock::getTime()
{
    // TODO
    return SimTime::ZERO;
}

void RealtimeClock::setTime(simtime_t time)
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

} //namespace
