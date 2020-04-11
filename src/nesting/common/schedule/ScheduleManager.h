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

#ifndef NESTING_COMMON_SCHEDULE_SCHEDULEMANAGER_H_
#define NESTING_COMMON_SCHEDULE_SCHEDULEMANAGER_H_

#include "nesting/common/time/IClock2.h"
#include "nesting/common/schedule/Schedule.h"
#include "nesting/common/schedule/CycleTimerState.h"
#include "nesting/common/schedule/ListExecuteState.h"
#include "nesting/common/schedule/ListConfigState.h"

#include "inet/common/ModuleAccess.h"

#include <omnetpp.h>

#include <string>
#include <memory>
#include <set>

using namespace omnetpp;
using namespace inet;

namespace nesting {

template<typename T>
class ScheduleManager : public cSimpleModule, public IClock2::TimestampListener {
public:
    class IStateListener {
    public:
        virtual ~IStateListener() {};
        virtual void onStateChange(T oldState, T newState) = 0;
        virtual void onCycleTimerStateChanged(CycleTimerState cycleTimerState) {};
        virtual void onListExecuteStateChanged(ListExecuteState listExecuteState) {};
        virtual void onListConfigStateChanged(ListConfigState listConfigState) {};
    };
protected:
    // Type values to differentiate different timestamp events from the clock module.
    static const uint64_t CYCLE_TIMER_EVENT = 0;
    static const uint64_t LIST_EXECUTE_EVENT = 1;
    static const uint64_t LIST_CONFIG_EVENT = 2;

    std::shared_ptr<const IClock2::Timestamp> nextCycleTimerUpdate = nullptr;
    std::shared_ptr<const IClock2::Timestamp> nextListExecuteUpdate = nullptr;
    std::shared_ptr<const IClock2::Timestamp> nextListConfigUpdate = nullptr;

    cMessage updateCycleTimerMsg = cMessage("updateCycleTimerStateMachine");
    cMessage updateListExecuteMsg = cMessage("updateListExecuteStateMachine");
    cMessage updateListConfigMsg = cMessage("updateListConfigStateMachine");

    IClock2* clock;

    /** If the GateManager isn't enabled no internal state transitions will take place. */
    bool enabled = true;

    // Default values
    T adminState;
    std::shared_ptr<const Schedule<T>> adminSchedule = nullptr;
    
    // Variables belonging to ListConfig state machine
    ListConfigState listConfigState = ListConfigState::UNDEFINED;
    bool configPending = false;
    simtime_t configChangeTime = SimTime::ZERO;
    uint64_t configChangeErrorCounter = 0;
    std::shared_ptr<const Schedule<T>> operSchedule = nullptr;

    // Variables belonging to CycleTimer state machine
    CycleTimerState cycleTimerState = CycleTimerState::UNDEFINED;
    simtime_t cycleStartTime = SimTime::ZERO;

    // Variables belonging to ListExecute state machine
    ListExecuteState listExecuteState = ListExecuteState::UNDEFINED;
    uint64_t listPointer = 0;
    simtime_t exitTimer = SimTime::ZERO;
    T operState;
    simtime_t timeInterval;

    // Variables shared between state machines
    bool configChange = false;
    bool cycleStart = false;
    bool newConfigCT = false;

    std::set<IStateListener*> listeners;
protected:
    virtual void initialize() override
    {
        clock = getModuleFromPar<IClock2>(par("clockModule"), this);

        adminState = initialAdminState();
        operState = initialAdminState();

        operSchedule = initialAdminSchedule();
        adminSchedule = initialAdminSchedule();
        
        begin();

        // Variables
        WATCH(enabled);
        WATCH(adminSchedule);
        WATCH(operSchedule);
        WATCH(adminState);
        WATCH(operState);
        WATCH(listPointer);
        WATCH(timeInterval);
        WATCH(cycleStartTime);
        WATCH(configChangeTime);
        WATCH(configChangeErrorCounter);

        // State machines
        WATCH(cycleTimerState);
        WATCH(listExecuteState);
        WATCH(listConfigState);
    }

    virtual void handleMessage(cMessage *msg) override
    {
        if (msg == &updateListConfigMsg) {
            updateListConfigState();
        } else if (msg == &updateListExecuteMsg) {
            updateListExecuteState();
        } else if (msg == &updateCycleTimerMsg) {
            updateCycleTimerState();
        } else {
            throw cRuntimeError("Can't handle this type of messages.");
        }
    }

    virtual void updateCycleTimerState()
    {
        // Cancel outdated timestamp events
        if (nextCycleTimerUpdate != nullptr) {
            clock->unsubscribeTimestamp(*this, *nextCycleTimerUpdate);
        }

        EV_DEBUG << "Handle CycleTimerState " << cycleTimerState << "." << std::endl;

        if (enabled) {
            if (cycleTimerState == CycleTimerState::CYCLE_IDLE) {
                setCycleStart(false);
                setNewConfigCT(false);
                cycleTimerState = CycleTimerState::SET_CYCLE_START_TIME;
                scheduleAt(simTime(), &updateCycleTimerMsg);
                
             } else if (cycleTimerState == CycleTimerState::SET_CYCLE_START_TIME) {
                setCycleStartTime();
                simtime_t idleTime = cycleStartTime - clock->updateAndGetLocalTime();
                if (idleTime <= SimTime::ZERO) {
                    cycleTimerState = CycleTimerState::START_CYCLE;
                    scheduleAt(simTime(), &updateCycleTimerMsg);
                } else {
                    cycleTimerState = CycleTimerState::WAIT_TO_START_CYCLE;
                    nextCycleTimerUpdate = clock->subscribeTimestamp(*this, cycleStartTime, CYCLE_TIMER_EVENT);
                }
            } else if (cycleTimerState == CycleTimerState::WAIT_TO_START_CYCLE) {
                cycleTimerState = CycleTimerState::START_CYCLE;
                scheduleAt(simTime(), &updateCycleTimerMsg);
            } else if (cycleTimerState == CycleTimerState::START_CYCLE) {
                setCycleStart(true);
                cycleTimerState = CycleTimerState::SET_CYCLE_START_TIME;
                simtime_t minClockInterval = SimTime(1, SIMTIME_S) / clock->getClockRate();
                nextCycleTimerUpdate = clock->subscribeDelta(*this, minClockInterval, CYCLE_TIMER_EVENT);
            }
        }  
    }

    virtual void updateListExecuteState()
    {
        // Cancel outdated timestamp events
        if (nextListExecuteUpdate != nullptr) {
            clock->unsubscribeTimestamp(*this, *nextListExecuteUpdate);
        }

        EV_DEBUG << "Handle ListExecuteState " << listExecuteState << "." << std::endl;

        if (enabled) {
            if (listExecuteState == ListExecuteState::NEW_CYCLE) {
                setCycleStart(false);
                listPointer = 0;
                listExecuteState = ListExecuteState::EXECUTE_CYCLE;
                scheduleAt(simTime(), &updateListExecuteMsg);
            } else if (listExecuteState == ListExecuteState::EXECUTE_CYCLE) {
                T oldState = operState;
                executeOperation(listPointer);
                simtime_t exitTimer = timeInterval;
                notifyStateChanged(oldState, operState);
                listPointer++;
                if (exitTimer <= SimTime::ZERO && listPointer < operSchedule->getControlListLength()) {
                    simtime_t minClockInterval = SimTime(1, SIMTIME_S) / clock->getClockRate();
                    listExecuteState = ListExecuteState::EXECUTE_CYCLE;
                    nextListExecuteUpdate = clock->subscribeDelta(*this, minClockInterval, LIST_EXECUTE_EVENT);
                } else if (exitTimer > SimTime::ZERO && listPointer < operSchedule->getControlListLength()) {
                    listExecuteState = ListExecuteState::DELAY;
                    nextListExecuteUpdate = clock->subscribeDelta(*this, exitTimer, LIST_EXECUTE_EVENT);
                } else {
                    assert(listPointer >= operSchedule->getControlListLength());
                    listExecuteState = ListExecuteState::END_OF_CYCLE;
                    scheduleAt(simTime(), &updateListExecuteMsg);
                }
            } else if (listExecuteState == ListExecuteState::DELAY) {
                listExecuteState = ListExecuteState::EXECUTE_CYCLE;
                scheduleAt(simTime(), &updateListExecuteMsg);
            } else if (listExecuteState == ListExecuteState::INIT) {
                T oldState = operState;
                operState = adminState;
                notifyStateChanged(oldState, operState);
                exitTimer = SimTime::ZERO;
                listPointer = 0;
                listExecuteState = ListExecuteState::END_OF_CYCLE;
                scheduleAt(simTime(), &updateListExecuteMsg);
            }
        }
    }

    virtual void updateListConfigState()
    {
        // Cancel outdated timestamp events
        if (nextListConfigUpdate != nullptr) {
            clock->unsubscribeTimestamp(*this, *nextListConfigUpdate);
        }

        EV_DEBUG << "Handle ListConfigState " << listConfigState << "." << std::endl;

        if (enabled) {
            if (listConfigState == ListConfigState::CONFIG_PENDING) {
                setConfigChange(false);
                setConfigChangeTime();
                configPending = true;
                listConfigState = ListConfigState::UPDATE_CONFIG;
                nextListConfigUpdate = clock->subscribeTimestamp(*this, configChangeTime, LIST_CONFIG_EVENT);
            } else if (listConfigState == ListConfigState::UPDATE_CONFIG) {
                operSchedule = adminSchedule;
                setNewConfigCT(true);
                listConfigState = ListConfigState::CONFIG_IDLE;
                scheduleAt(simTime(), &updateListConfigMsg);
            } else if (listConfigState == ListConfigState::CONFIG_IDLE) {
                configPending = false;
            }
        }
    }

    virtual void begin()
    {
        cycleTimerState = CycleTimerState::CYCLE_IDLE;
        EV_DEBUG << "Set CycleTimer state to " << cycleTimerState << "." << std::endl;
        scheduleAt(simTime(), &updateCycleTimerMsg);

        listExecuteState = ListExecuteState::INIT;
        EV_DEBUG << "Set ListExecute state to " << listExecuteState << "." << std::endl;
        scheduleAt(simTime(), &updateListExecuteMsg);

        listConfigState = ListConfigState::CONFIG_IDLE;
        EV_DEBUG << "Set ListConfig state to " << listConfigState << "." << std::endl;
        scheduleAt(simTime(), &updateListConfigMsg);
    }

    virtual void setCycleStartTime()
    {
        assert(operSchedule != nullptr);

        simtime_t currentTime = clock->updateAndGetLocalTime();
        simtime_t operBaseTime = operSchedule->getBaseTime();
        simtime_t operCycleTime = operSchedule->getCycleTime();
        simtime_t operCycleTimeExtension = operSchedule->getCycleTimeExtension();

        // IEEE 802.1Q 8.6.9.1.1 case a)
        if (!configPending && operBaseTime >= currentTime) {
            cycleStartTime = operBaseTime;
        }
        // IEEE 802.1Q 8.6.9.1.1 case b)
        else if (!configPending && operBaseTime < currentTime) {
            uint64_t n = static_cast<uint64_t>(std::ceil((currentTime - operBaseTime) / operCycleTime));
            cycleStartTime = operBaseTime + n * operCycleTime;
        }
        // IEEE 802.1Q 8.6.9.1.1 case c)
        else if (configPending && configChangeTime > (currentTime + operCycleTime + operCycleTimeExtension)) {
            unsigned n = static_cast<uint64_t>(std::ceil((currentTime - operBaseTime) / operCycleTime));
            cycleStartTime = operBaseTime + n * operCycleTime;
        }
        // IEEE 802.1Q 8.6.9.1.1 case d)
        else if (configPending && configChangeTime <= (currentTime + operCycleTime + operCycleTimeExtension)) {
            cycleStartTime = configChangeTime;
        }

        assert(cycleStartTime >= currentTime);
    }

    virtual void setCycleStart(bool cycleStart)
    {
        this->cycleStart = cycleStart;
        if (cycleStart) {
            listExecuteState = ListExecuteState::NEW_CYCLE;
            EV_DEBUG << "Set ListExecute state to " << listExecuteState << "." << std::endl;
            scheduleAt(simTime(), &updateListExecuteMsg);
        }
    }

    virtual void setNewConfigCT(bool newConfigCT)
    {
        this->newConfigCT = newConfigCT;
        if (newConfigCT) {
            cycleTimerState = CycleTimerState::CYCLE_IDLE;
            scheduleAt(simTime(), &updateCycleTimerMsg);
            EV_DEBUG << "Set CycleTimer state to " << cycleTimerState << "." << std::endl;
        }
    }

    virtual void executeOperation(uint64_t listPointer)
    {
        if (operSchedule->getControlListLength() <= 0) {
            // If control list has no entries we have to fall back on default values.
            operState = adminState;
            timeInterval = operSchedule->getCycleTime();
        } else {
            assert(listPointer < operSchedule->getControlListLength());
            operState = operSchedule->getScheduledObject(listPointer);
            timeInterval = operSchedule->getTimeInterval(listPointer);
            // According to IEEE 802.1Qbv chapter 8.6.9.2.1 case a) time
            // interval values of zero will be set to one time unit.
            if (timeInterval <= SimTime::ZERO) {
                simtime_t minClockInterval = SimTime(1, SIMTIME_S) / clock->getClockRate();
                timeInterval = minClockInterval;
            }
        }
    }

    virtual void notifyStateChanged(T oldState, T newState)
    {
        for (auto listener : listeners) {
            listener->onStateChange(oldState, newState);
        }
    }

    virtual void setConfigChangeTime()
    {
        simtime_t currentTime = clock->updateAndGetLocalTime();
        simtime_t adminBaseTime = adminSchedule->getBaseTime();
        simtime_t adminCycleTime = adminSchedule->getCycleTime();

        // IEEE 802.1Q 8.6.9.3.1 case a)
        if (adminBaseTime >= currentTime) {
            configChangeTime = adminSchedule->getBaseTime();
        }
        // IEEE 802.1Q 8.6.9.3.1 case b)
        else if (adminBaseTime < currentTime && !enabled) {
            uint64_t n = static_cast<uint64_t>(std::ceil((currentTime - adminBaseTime) / adminCycleTime));
            configChangeTime = adminBaseTime + n * adminCycleTime;
        }
        // IEEE 802.1Q 8.6.9.3.1 case c)
        else if (adminBaseTime < currentTime && enabled) {
            configChangeErrorCounter++;
            uint64_t n = static_cast<uint64_t>(std::ceil((currentTime - adminBaseTime) / adminCycleTime));
            configChangeTime = adminBaseTime + n * adminCycleTime;
        }

        assert(configChangeTime >= currentTime);
    }

    virtual const T initialAdminState() = 0;

    virtual std::shared_ptr<const Schedule<T>> initialAdminSchedule() = 0;
public:
    virtual ~ScheduleManager() {
        cancelEvent(&updateCycleTimerMsg);
        cancelEvent(&updateListExecuteMsg);
        cancelEvent(&updateListConfigMsg);
    }

    virtual void setAdminState(T adminState)
    {
        this->adminState = adminState;
    }

    virtual const T& getAdminState() const
    {
        return this->adminState;
    }

    virtual const T& getOperState() const
    {
        return this->operState;
    }

    virtual void setAdminSchedule(std::shared_ptr<const Schedule<T>> adminSchedule)
    {
        if (adminSchedule->getCycleTime() <= SimTime::ZERO) {
            throw cRuntimeError("Can't load schedule with cycle time of zero.");
        }
        this->adminSchedule = adminSchedule;
    }

    virtual void setEnabled(bool enabled)
    {
        this->enabled = enabled;
        if (!enabled) {
            cycleTimerState = CycleTimerState::CYCLE_IDLE;
            scheduleAt(simTime(), &updateCycleTimerMsg);
            EV_DEBUG << "Set CycleTimer state to " << cycleTimerState << "." << std::endl;

            listExecuteState = ListExecuteState::INIT;
            scheduleAt(simTime(), &updateListExecuteMsg);
            EV_DEBUG << "Set ListExecute state to " << listExecuteState << "." << std::endl;

            listConfigState = ListConfigState::CONFIG_IDLE;
            scheduleAt(simTime(), &updateListConfigMsg);
            EV_DEBUG << "Set ListConfig state to " << listConfigState << "." << std::endl;
        } else {
            listConfigState = ListConfigState::CONFIG_PENDING;
            scheduleAt(simTime(), &updateListConfigMsg);
            EV_DEBUG << "Set ListConfig state to " << listConfigState << "." << std::endl;
        }
    }

    virtual bool isEnabled()
    {
        return enabled;
    }

    virtual void setConfigChange(bool configChange)
    {
        this->configChange = configChange;
        if (configChange) {
            listConfigState = ListConfigState::CONFIG_PENDING;
            scheduleAt(simTime(), &updateListConfigMsg);
            EV_DEBUG << "Set ListConfig state to " << listConfigState << "." << std::endl;
        }
    }

    virtual bool isConfigChange()
    {
        return configChange;
    }

    virtual uint64_t getConfigChangeErrorCounter()
    {
        return configChangeErrorCounter;
    }

    virtual void onTimestamp(IClock2& clock, std::shared_ptr<const IClock2::Timestamp> timestamp) override
    {
        Enter_Method("timestamp");
        switch (timestamp->getKind()) {
        case CYCLE_TIMER_EVENT:
            scheduleAt(simTime(), &updateCycleTimerMsg);
            break;
        case LIST_EXECUTE_EVENT:
            scheduleAt(simTime(), &updateListExecuteMsg);
            break;
        case LIST_CONFIG_EVENT:
            scheduleAt(simTime(), &updateListConfigMsg);
            break;
        default:
            throw cRuntimeError("Invalid timestamp event.");
        }
    }

    virtual void subscribeStateChanges(IStateListener& listener)
    {
        listeners.insert(&listener);
    }

    virtual void unsubscribeStateChanges(IStateListener& listener)
    {
        listeners.erase(&listener);
    }
};

} // namespace nesting

#endif
