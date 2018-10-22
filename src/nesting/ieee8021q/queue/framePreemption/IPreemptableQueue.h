/*
 * IPreemptableQueue.h
 *
 *  Created on: Jan 17, 2018
 *      Author: michel
 */

#ifndef NESTING_IEEE8021Q_QUEUE_FRAMEPREEMPTION_IPREEMPTABLEQUEUE_H_
#define NESTING_IEEE8021Q_QUEUE_FRAMEPREEMPTION_IPREEMPTABLEQUEUE_H_

namespace nesting {

class IClock;

/**
 * This class is an interface for marking a component as express or preemptable queue
 *
 * @see IClock
 */
class IPreemptableQueue {
public:
    virtual ~IPreemptableQueue() {
    }
    ;

    virtual bool isExpressQueue() = 0;
};

} // namespace nesting

#endif /* NESTING_IEEE8021Q_QUEUE_FRAMEPREEMPTION_IPREEMPTABLEQUEUE_H_ */

