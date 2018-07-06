/*
 * HostSchedule.h
 *
 *  Created on: 15.07.2017
 *      Author: Patrick
 */

#ifndef NESTING_COMMON_SCHEDULE_HOSTSCHEDULE_H_
#define NESTING_COMMON_SCHEDULE_HOSTSCHEDULE_H_

#include <omnetpp.h>
#include <vector>

using namespace omnetpp;
using namespace std;

namespace nesting {

/**
 * A host-schedule is an array of entries, that consist of a timestamp, a size in bytes and an Ieee8021QCtrl-info containing values
 * such as destination address and PCP.
 */
template<typename T>
class HostSchedule {
protected:
  /**
   * Schedule entries, that consist of a length in abstract time units and
   * a scheduled object.
   */
  vector<tuple<int, int, T>> entries;

  /**
   * Total cycletime of this schedule.
   */
  int cycle = 0;

public:
  HostSchedule() {}

  virtual ~HostSchedule() {};

  /** Returns the number of entries of the schedule. */
  virtual unsigned int size() const {
    return entries.size();
  }

  /** Sets the Cycletime of this schedule. */
  virtual void setCycle(unsigned int cycleLength) {
    cycle = cycleLength;
  }

  /** Returns the number of entries of the schedule. */
  virtual unsigned int getCycle() const {
    return cycle;
  }

  /** Returns the scheduled object at a given index. */
  virtual T getScheduledObject(unsigned int index) const {
    return get < 2 > (entries[index]);
  }

  /**
   * Returns the time when the scheduled object is supposed to be sent.
   */
  virtual unsigned int getTime(unsigned int index) const {
    return get < 0 > (entries[index]);
  }

  /**
   *  Return the size of the scheduled object at a given index.
   */
  virtual unsigned int getSize(unsigned int index) const {
    return get < 1 > (entries[index]);
  }

  /** Returns true if the schedule contains no entries. Otherwise false. */
  virtual bool isEmpty() const {
    return (entries.size() == 0);
  }

  /**
   * Adds a new entry add the end of the schedule.
   *
   * @param time          The length of the schedule entry in abstract
   *                        time units.
   * @param size          The size of the scheduled entry in bytes.
   *
   * @param scheduledObject The schedule objects associated with the
   *                        scheduled entry.
   */
  virtual void addEntry(int time, int size, T scheduledObject) {
    entries.push_back(make_tuple(time, size, scheduledObject));
  }
};

} // namespace nesting

#endif /* NESTING_COMMON_SCHEDULE_HOSTSCHEDULE_H_ */
