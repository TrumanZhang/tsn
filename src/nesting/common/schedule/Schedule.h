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

#ifndef NESTING_IEEE8021Q_QUEUE_GATING_SCHEDULE_H_
#define NESTING_IEEE8021Q_QUEUE_GATING_SCHEDULE_H_

#include <omnetpp.h>
#include <bitset>
#include <vector>
#include <iostream>

using namespace omnetpp;

namespace nesting {

/**
 * A schedule is more or less a list consisting of tuples of scheduled objects and time intervals.
 * For more information about terminologies and context see chapters 8.6.8.4 and 8.6.9 of the IEEE802.1Q standard.
 */
template<typename T>
class Schedule {
protected:
    std::vector<std::tuple<simtime_t, T>> controlList;
    simtime_t baseTime = SimTime::ZERO;
    simtime_t cycleTime = SimTime::ZERO;
    simtime_t cycleTimeExtension = SimTime::ZERO;
    simtime_t sumTimeIntervals = SimTime::ZERO;
public:
    Schedule() {}

    virtual ~Schedule() {}

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

    virtual unsigned int getControlListLength() const {
        return controlList.size();
    }

    virtual const T& getControlListEntry(unsigned int controlListIndex) const {
        return std::get<1>(controlList[controlListIndex]);
    }

    virtual simtime_t getTimeInterval(unsigned int controlListIndex) const {
        return std::get<0>(controlList[controlListIndex]);
    }

    virtual bool isEmpty() const {
        return controlList.empty();
    }

    virtual void addControlListEntry(simtime_t timeInterval, T controlListEntry) {
        sumTimeIntervals += timeInterval;
        controlList.push_back(std::make_tuple(timeInterval, controlListEntry));
    }

    virtual simtime_t getSumTimeIntervals() const {
        return sumTimeIntervals;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Schedule<T>& schedule)
{
    return stream << "Schedule[baseTime=" << schedule.getBaseTime() 
            << ", cycleTime=" << schedule.getCycleTime()
            << ", cycleTimeExtension=" << schedule.getCycleTimeExtension()
            << ", controlListLength=" << schedule.getControlListLength()
            << ", sumTimeIntervals=" << schedule.getSumTimeIntervals() << "]";
}

} // namespace nesting

#endif /* NESTING_IEEE8021Q_QUEUE_GATING_SCHEDULE_H_ */
