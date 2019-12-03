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

#ifndef __NESTINGNG_REALTIMECLOCK_H_
#define __NESTINGNG_REALTIMECLOCK_H_

#include <omnetpp.h>

#include <list>
#include <set>

#include "inet/common/ModuleAccess.h"

#include "nesting/common/time/IClock2.h"
#include "nesting/common/time/IClock2TimestampListener.h"
#include "nesting/common/time/IClock2ConfigListener.h"
#include "nesting/common/time/IOscillator.h"
#include "nesting/common/time/IOscillatorTickListener.h"
#include "nesting/common/time/IOscillatorConfigListener.h"

using namespace omnetpp;
using namespace inet;

namespace nesting {

class RealtimeClockTimestamp;

/**
 * TODO - Generated class
 */
class RealtimeClock : public cSimpleModule, public IClock2, public IOscillatorTickListener, public IOscillatorConfigListener
{
protected:
    IOscillator* oscillator;
    simtime_t localTime;
    uint64_t lastTick;
    double driftRate;
    std::set<IClock2ConfigListener*> configListeners;
    std::list<std::shared_ptr<RealtimeClockTimestamp>> scheduledEvents;
    std::shared_ptr<IOscillatorTick> nextTick;
    /** 
     * If the clockRate + driftRate is smaller than this threshold, the clock
     * is stopped instead of running really slow to prevent numeric errors.
     */
    const double minEffectiveClockRate = 1e-12;
protected:
    virtual void initialize();
    virtual void scheduleNextTimestamp();
    virtual simtime_t timeIncrementPerTick() const;
public:
    RealtimeClock();
    virtual ~RealtimeClock();
    virtual std::shared_ptr<const IClock2Timestamp> subscribeDelta(IClock2TimestampListener& listener, simtime_t delta, uint64_t kind = 0) override;
    virtual std::shared_ptr<const IClock2Timestamp> subscribeTimestamp(IClock2TimestampListener& listener, simtime_t time, uint64_t kind = 0) override;
    virtual void subscribeConfigChanges(IClock2ConfigListener& listener) override;
    virtual void unsubscribeConfigChanges(IClock2ConfigListener& listener) override;
    virtual simtime_t updateAndGetLocalTime() override;
    virtual void setLocalTime(simtime_t time) override;
    virtual double getClockRate() const override;
    virtual void setClockRate(double clockRate) override;
    virtual double getDriftRate() const override;
    virtual void setDriftRate(double driftRate) override;
    virtual void onTick(IOscillator& oscillator, const IOscillatorTick& tick) override;
    virtual void onFrequencyChange(IOscillator& oscillator, double oldFrequency, double newFrequency) override;
    virtual bool isStopped();
};

class RealtimeClockTimestamp : public IClock2Timestamp
{
protected:
    simtime_t localTime;
    uint64_t kind;
    IClock2TimestampListener& listener;
public:
    RealtimeClockTimestamp(IClock2TimestampListener& listener, simtime_t localTime, uint64_t kind);
    virtual simtime_t getLocalTime() const override;
    virtual uint64_t getKind() const override;
    virtual IClock2TimestampListener& getListener() const;
    bool operator==(const RealtimeClockTimestamp& other) const;
    bool operator!=(const RealtimeClockTimestamp& other) const;
    bool operator<(const RealtimeClockTimestamp& other) const;
};

} //namespace

#endif
