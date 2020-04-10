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

#include "inet/common/ModuleAccess.h"

#include <omnetpp.h>

#include <string>
#include <memory>

using namespace omnetpp;
using namespace inet;

namespace nesting {

enum class CycleTimerState {
    UNDEFINED,
    CYCLE_IDLE,
    SET_CYCLE_START_TIME,
    WAIT_TO_START_CYCLE, // additional state to model waiting period
    START_CYCLE
};

enum class ListExecuteState {
    UNDEFINED,
    NEW_CYCLE,
    INIT,
    EXECUTE_CYCLE,
    DELAY,
    END_OF_CYCLE
};

enum class ListConfigState {
    UNDEFINED,
    CONFIG_PENDING,
    UPDATE_CONFIG,
    CONFIG_IDLE
};

template<typename T>
class ScheduleManager : public cSimpleModule, public IClock2::TimestampListener {
public:
    class IScheduleManagerListener {
    public:
        virtual ~IScheduleManagerListener() {};
        virtual void onStateChange(T oldState, T newState) = 0;
        virtual void onCycleTimerStateChanged(CycleTimerState cycleTimerState) = 0;
        virtual void onListExecuteStateChanged(ListExecuteState listExecuteState) = 0;
        virtual void onListConfigStateChanged(ListConfigState listConfigState) = 0;
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
    int configChangeErrorCounter = 0;
    std::shared_ptr<const Schedule<T>> operSchedule = nullptr;

    // Variables belonging to CycleTimer state machine
    CycleTimerState cycleTimerState = CycleTimerState::UNDEFINED;
    simtime_t cycleStartTime = SimTime::ZERO;

    // Variables belonging to ListExecute state machine
    ListExecuteState listExecuteState = ListExecuteState::UNDEFINED;
    uint64_t listPointer;
    simtime_t exitTimer = SimTime::ZERO;
    T operState;
    simtime_t timeInterval;

    // Variables shared between state machines
    bool configChange = false;
    bool cycleStart = false;
    bool newConfigCT = false;

    std::vector<IScheduleManagerListener*> listeners;
protected:
    virtual void initialize() override
    {
        clock = getModuleFromPar<IClock2>(par("clockModule"), this);

        adminState = defaultAdminState();
        operState = defaultAdminState();

        operSchedule = defaultAdminSchedule();
        adminSchedule = defaultAdminSchedule();
        
        begin();
    }

    virtual void finish() override
    {
        cancelEvent(&updateCycleTimerMsg);
        cancelEvent(&updateListExecuteMsg);
        cancelEvent(&updateListConfigMsg);
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

        if (enabled) {
            if (cycleTimerState == CycleTimerState::CYCLE_IDLE) {
                setCycleStart(false);
                setNewConfigCT(false);
                cycleTimerState = CycleTimerState::SET_CYCLE_START_TIME;
                scheduleAt(simTime(), &updateCycleTimerMsg);
                EV_INFO << "CycleTimer transitioned to SET_CYCLE_START_TIME state." << std::endl;
             } else if (cycleTimerState == CycleTimerState::SET_CYCLE_START_TIME) {
                setCycleStartTime();
                simtime_t idleTime = cycleStartTime - clock->updateAndGetLocalTime();
                if (idleTime <= SimTime::ZERO) {
                    cycleTimerState = CycleTimerState::START_CYCLE;
                    scheduleAt(simTime(), &updateCycleTimerMsg);
                    EV_INFO << "CycleTimer transitioned to START_CYCLE state." << std::endl;
                } else {
                    cycleTimerState = CycleTimerState::WAIT_TO_START_CYCLE;
                    nextCycleTimerUpdate = clock->subscribeTimestamp(*this, cycleStartTime, CYCLE_TIMER_EVENT);
                    EV_INFO << "CycleTimer transitioned to WAIT_TO_START_CYCLE state until t="
                            << nextCycleTimerUpdate->getLocalTime() << " (local time)." << std::endl;
                }
            } else if (cycleTimerState == CycleTimerState::WAIT_TO_START_CYCLE) {
                cycleTimerState = CycleTimerState::START_CYCLE;
                scheduleAt(simTime(), &updateCycleTimerMsg);
                EV_INFO << "CycleTimer transitioned to START_CYCLE state." << std::endl;
            } else if (cycleTimerState == CycleTimerState::START_CYCLE) {
                setCycleStart(true);
                cycleTimerState = CycleTimerState::SET_CYCLE_START_TIME;
                simtime_t minClockInterval = SimTime(1, SIMTIME_S) / clock->getClockRate();
                nextCycleTimerUpdate = clock->subscribeDelta(*this, minClockInterval, CYCLE_TIMER_EVENT);
                EV_INFO << "CycleTimer transitioned to SET_CYCLE_START_TIME state." << std::endl;
            }
        }
    }

    virtual void updateListExecuteState()
    {
        // Cancel outdated timestamp events
        if (nextListExecuteUpdate != nullptr) {
            clock->unsubscribeTimestamp(*this, *nextListExecuteUpdate);
        }

        if (enabled) {
            if (listExecuteState == ListExecuteState::NEW_CYCLE) {
                setCycleStart(false);
                listPointer = 0;
                listExecuteState = ListExecuteState::EXECUTE_CYCLE;
                scheduleAt(simTime(), &updateListExecuteMsg);
                EV_INFO << "ListExecute transitioned to EXECUTE_CYCLE state." << std::endl;
            } else if (listExecuteState == ListExecuteState::EXECUTE_CYCLE) {
                T oldState = operState;
                executeOperation(listPointer);
                simtime_t exitTimer = timeInterval;
                notifyStateChanged(oldState, operState);
                listPointer++;
                if (exitTimer > SimTime::ZERO && listPointer < operSchedule->getControlListLength()) {
                    simtime_t minClockInterval = SimTime(1, SIMTIME_S) / clock->getClockRate();
                    listExecuteState = ListExecuteState::EXECUTE_CYCLE;
                    nextListExecuteUpdate = clock->subscribeDelta(*this, minClockInterval, LIST_EXECUTE_EVENT);
                    EV_INFO << "ListExecute transitioned to EXECUTE_CYCLE state." << std::endl;
                } else if (exitTimer <= SimTime::ZERO && listPointer < operSchedule->getControlListLength()) {
                    listExecuteState = ListExecuteState::DELAY;
                    nextListExecuteUpdate = clock->subscribeDelta(*this, exitTimer, LIST_EXECUTE_EVENT);
                    EV_INFO << "ListExecute transitioned to DELAY state." << std::endl;
                } else {
                    assert(listPointer >= operSchedule->getControlListLength());
                    listExecuteState = ListExecuteState::END_OF_CYCLE;
                    scheduleAt(simTime(), &updateListExecuteMsg);
                    EV_INFO << "ListExecute transitioned to END_OF_CYCLE state." << std::endl;
                }
            } else if (listExecuteState == ListExecuteState::DELAY) {
                listExecuteState = ListExecuteState::EXECUTE_CYCLE;
                scheduleAt(simTime(), &updateListExecuteMsg);
                EV_INFO << "ListExecute transitioned to EXECUTE_CYCLE state." << std::endl;
            } else if (listExecuteState == ListExecuteState::INIT) {
                T oldState = operState;
                operState = adminState;
                notifyStateChanged(oldState, operState);
                exitTimer = SimTime::ZERO;
                listPointer = 0;
                scheduleAt(simTime(), &updateListExecuteMsg);
                EV_INFO << "ListExecute transitioned to END_OF_CYCLE state." << std::endl;
            }
        }
    }

    virtual void updateListConfigState()
    {
        // Cancel outdated timestamp events
        if (nextListConfigUpdate != nullptr) {
            clock->unsubscribeTimestamp(*this, *nextListConfigUpdate);
        }

        if (enabled) {
            // TODO
        }
    }

    virtual void begin()
    {
        cycleTimerState = CycleTimerState::CYCLE_IDLE;
        EV_INFO << "Set CycleTimer to CYLE_IDLE state." << std::endl;
        scheduleAt(simTime(), &updateCycleTimerMsg);

        listExecuteState = ListExecuteState::INIT;
        EV_INFO << "Set ListExecute state to INIT state." << std::endl;
        scheduleAt(simTime(), &updateListExecuteMsg);

        listConfigState = ListConfigState::CONFIG_IDLE;
        EV_INFO << "Set ListConfig state to CONFIG_IDLE state." << std::endl;
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
            assert(cycleStartTime >= currentTime);
        }
        // IEEE 802.1Q 8.6.9.1.1 case c)
        else if (configPending && configChangeTime > (currentTime + operCycleTime + operCycleTimeExtension)) {
            unsigned n = static_cast<uint64_t>(std::ceil((currentTime - operBaseTime) / operCycleTime));
            cycleStartTime = operBaseTime + n * operCycleTime;
            assert(cycleStartTime >= currentTime);
        }
        // IEEE 802.1Q 8.6.9.1.1 case d)
        else if (configPending && configChangeTime <= (currentTime + operCycleTime + operCycleTimeExtension)) {
            cycleStartTime = configChangeTime;
        }
    }

    virtual void setCycleStart(bool cycleStart)
    {
        // TODO
        this->cycleStart = cycleStart;
    }

    virtual void setNewConfigCT(bool newConfigCT)
    {
        // TODO
        this->newConfigCT = newConfigCT;
    }

    virtual void setConfigChange(bool configChange)
    {
        // TODO
        this->configChange = configChange;
    }

    virtual void executeOperation(uint64_t listPointer)
    {
        // TODO
    }

    virtual void notifyStateChanged(T oldState, T newState)
    {
        // TODO
    }
    
    virtual const T defaultAdminState() = 0;

    virtual std::shared_ptr<const Schedule<T>> defaultAdminSchedule() = 0;
public:
    virtual ~ScheduleManager() {}

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
        this->adminSchedule = adminSchedule;
    }

    virtual void setEnabled(bool enabled)
    {
        // TODO
    }

    virtual bool isEnabled()
    {
        return enabled;
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
};

} // namespace nesting

#endif
