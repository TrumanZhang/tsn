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

#ifndef NESTING_COMMON_SCHEDULE_SCHEDULE_H_
#define NESTING_COMMON_SCHEDULE_SCHEDULE_H_

#include <omnetpp.h>
#include <bitset>
#include <vector>
#include <iostream>
#include <memory>

using namespace omnetpp;

namespace nesting {

/**
 * A schedule is more or less a list consisting of tuples of scheduled objects and time intervals.
 * For more information about terminologies and context see chapters 8.6.8.4 and 8.6.9 of the IEEE802.1Q standard.
 */
template<typename T>
class Schedule : public cObject {
    /** Tuple consisting of a time interval and a scheduled object. */
    struct ControlListEntry {
        simtime_t timeInterval;
        T scheduledObject;
    };
protected:
    /** The control list contains tuples of time intervals and scheduled objects. */
    std::vector<ControlListEntry> controlList;

    /**
     * The base time is considered when starting a schedule. Valid starting
     * points for a schedule are (baseTime + N * cycleTime) where N is a
     * positive integer.
     */
    simtime_t baseTime;

    simtime_t cycleTime;

    simtime_t cycleTimeExtension;

    /**
     * Sum of all time intervals from the control list. The IEEE802.1Q
     * standard explicitely specifies that this number can be different from
     * the cycle time.
     */
    simtime_t sumTimeIntervals;
public:
    virtual ~Schedule() {}

    Schedule()
        : baseTime(SimTime::ZERO)
        , cycleTime(SimTime::ZERO)
        , cycleTimeExtension(SimTime::ZERO)
        , sumTimeIntervals(SimTime::ZERO)
    {}

    virtual void setBaseTime(simtime_t baseTime) {
        this->baseTime = baseTime;
    }

    virtual simtime_t getBaseTime() const {
        return baseTime;
    }

    virtual void setCycleTime(simtime_t cycleTime) {
        this->cycleTime = cycleTime;
    }

    virtual simtime_t getCycleTime() const {
        return cycleTime;
    }

    virtual void setCycleTimeExtension(simtime_t cycleTimeExtension) {
        this->cycleTimeExtension = cycleTimeExtension;
    }

    virtual simtime_t getCycleTimeExtension() const {
        return cycleTimeExtension;
    }

    virtual unsigned getControlListLength() const {
        return controlList.size();
    }

    virtual const T& getScheduledObject(unsigned index) const {
        return controlList[index].scheduledObject;
    }

    virtual simtime_t getTimeInterval(unsigned index) const {
        return controlList[index].timeInterval;
    }

    virtual bool isEmpty() const {
        return controlList.empty();
    }

    virtual void addControlListEntry(simtime_t timeInterval, T scheduledObject) {
        if (timeInterval < SimTime::ZERO) {
            throw cRuntimeError("Control list entries only allow positive time intervals.");
        }
        sumTimeIntervals += timeInterval;
        ControlListEntry entry;
        entry.timeInterval = timeInterval;
        entry.scheduledObject = scheduledObject;
        controlList.push_back(entry);
    }

    virtual simtime_t getSumTimeIntervals() const {
        return sumTimeIntervals;
    }

    virtual std::string str() const override
    {
        std::ostringstream buffer;
        buffer << "baseTime=" << getBaseTime() 
            << ", cycleTime=" << getCycleTime()
            << ", cycleTimeExtension=" << getCycleTimeExtension()
            << ", controlListLength=" << getControlListLength()
            << ", sumTimeIntervals=" << getSumTimeIntervals();
        return buffer.str();
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Schedule<T>& schedule)
{
    return stream << "Schedule[" << schedule.str() << "]";
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<const Schedule<T>>& schedule)
{
    return stream << *schedule;
}

} // namespace nesting

#endif /* NESTING_COMMON_SCHEDULE_SCHEDULE_H_ */
