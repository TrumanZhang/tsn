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

#include <omnetpp.h>

#include <vector>
#include <cmath>
#ifdef LOGLEVEL_DEBUG
#include <string>
#include <map>
#endif

#include "nesting/common/schedule/Schedule.h"
#include "nesting/common/schedule/IScheduleManagerListener.h"
#include "nesting/ieee8021q/clock/IClock.h"
#include "nesting/ieee8021q/clock/IClockListener.h"

namespace nesting {

enum CycleTimerState {
    CYCLE_IDLE,
    SET_CYCLE_START_TIME,
    START_CYCLE
};

// DELAY state merged with EXECUTE_CYCLE state
enum ListExecuteState {
    NEW_CYCLE,
    INIT,
    EXECUTE_CYCLE,
    END_OF_CYCLE
};

enum ListConfigState {
    CONFIG_PENDING,
    UPDATE_CONFIG,
    CONFIG_IDLE
};

template<typename T>
class ScheduleManager : public IClockListener {
protected:
    struct ScheduleManagerState {
        CycleTimerState cycleTimerState = CYCLE_IDLE;
        ListExecuteState listExecuteState = NEW_CYCLE;
        ListConfigState listConfigState = CONFIG_PENDING;
    };
    enum StateMachine {
        CYCLE_TIMER_STATE_MACHINE,
        LIST_EXECUTE_STATE_MACHINE,
        LIST_CONFIG_STATE_MACHINE
    };
protected:
    IClock* clock;
    T adminStates;
    T operStates;
    Schedule<T>* operSchedule;
    Schedule<T>* adminSchedule;
    unsigned listPointer;
    simtime_t cycleStartTime;
    simtime_t configChangeTime;
    ScheduleManagerState stateMachine;
    bool cycleStart = false;
    bool newConfigCT = false;
    bool enabled = true;
    bool configChange = false;
    bool configPending = false;
    int configChangeErrorCounter = 0;
    std::vector<IScheduleManagerListener<T>*> listeners;
public:
    ScheduleManager(IClock* clock, T adminStates) {
        this.adminStates = adminStates;
        this.operStates = adminStates;
    }

    virtual ~ScheduleManager() {}

    virtual void begin() {
        setCycleTimerState(CYCLE_IDLE);
        setListExecuteState(INIT);
        setListConfigState(CONFIG_IDLE);
        clock->subscribeTick(this, 0);
    }

    virtual void setEnabled(bool enabled) {
        this->enabled = enabled;
        if (enabled && configChange) {
            setListConfigState(CONFIG_PENDING);
            clock->subscribeTick(this, 0);
        } else if (!enabled) {
            setCycleTimerState(CYCLE_IDLE);
            setListExecuteState(INIT);
            setListConfigState(CONFIG_IDLE);
            clock->subscribeTick(this, 0);
        }
    }

    // Equivalent to getOperGateStates
    virtual void getOperStates() {
        return operStates;
    }

    // Equivalent to getAdminGateStates
    virtual void getAdminStates() {
        return adminStates;
    }

    virtual int getConfigChangeErrorCounter() {
        return configChangeErrorCounter;
    }
protected:
    virtual void setConfigChange(bool configChange) {
        this.configChange = configChange;
        if (configChange && enabled) {
            setListConfigState(CONFIG_PENDING);
            clock->subscribeTick(this, 0);
        }
    }

    virtual void setConfigPending(bool configPending) {
        this.configPending = configPending;
    }

    /**
     * @see IEEE802.1Q standard chapter 8.6.9.1.1 SetCycleStartTime()
     */
    virtual void setCycleStartTime() {
        simtime_t currentTime = clock->getTime();
        simtime_t operBaseTime = operSchedule->getBaseTime();
        simtime_t operCycleTime = operSchedule->getCycleTime();
        simtime_t operCycleTimeExtension = operSchedule->getCycleTimeExtension();

        // IEEE 802.1Q 8.6.9.1.1 case a)
        if (!configPending && operBaseTime >= currentTime) {
            cycleStartTime = operBaseTime;
        }
        // IEEE 802.1Q 8.6.9.1.1 case b)
        else if (!configPending && operBaseTime < currentTime) {
            unsigned n = std::ceil((currentTime - operBaseTime) / operCycleTime);
            cycleStartTime = operBaseTime + n * operCycleTime;
            assert(cycleStartTime >= currentTime);
        }
        // IEEE 802.1Q 8.6.9.1.1 case c)
        else if (configPending && configChangeTime > (currentTime + operCycleTime + operCycleTimeExtension)) {
            unsigned n = std::ceil((currentTime - operBaseTime) / operCycleTime);
            cycleStartTime = operBaseTime + n * operCycleTime;
            assert(cycleStartTime >= currentTime);
        }
        // IEEE 802.1Q 8.6.9.1.1 case d)
        else if (configPending && configChangeTime <= (currentTime + operCycleTime + operCycleTimeExtension)) {
            cycleStartTime = configChangeTime;
        }
    }

    /**
     * @see IEEE802.1Q standard chapter 8.6.9.3.1 SetConfigChangeTime()
     */
    virtual void setConfigChangeTime() {
        simtime_t currentTime = clock->getTime();
        simtime_t adminBaseTime = adminSchedule->getBaseTime();

        // IEEE 802.1Q 8.6.9.3.1 case a)
        if (adminBaseTime >= currentTime) {
            configChangeTime = adminBaseTime;
        }
        // IEEE 802.1Q 8.6.9.3.1 case b)
        else if (adminBaseTime < currentTime && !enabled) {
            unsigned n = std::ceil((currentTime - adminBaseTime) / adminCycleTime);
            configChangeTime = adminBaseTime + n * adminCycleTime;
            assert(configChangeTime >= currentTime);
        }
        // IEEE 802.1Q 8.6.9.3.1 case c)
        else if (adminBaseTime < currentTime && enabled) {
            unsigned n = std::ceil((currentTime - adminBaseTime) / adminCycleTime);
            configChangeTime = adminBaseTime + n * adminCycleTime;
            configChangeErrorCounter++;
            assert(configChangeTime >= currentTime);
        }
    }

    virtual void setNewConfigCT(bool newConfigCT) {
        this->newConfigCT = newConfigCT;
        if (newConfigCT) {
            setCycleTimerState(CYCLE_IDLE);
            clock->subscribeTick(this, 0);
        }
    }

    virtual void setCycleStart(bool cycleStart) {
        this->cycleStart = cycleStart;
        if (cycleStart) {
            setListExecuteState(NEW_CYCLE);
        }
        clock->subscribeTick(this, 0);
    }

    virtual void setCycleTimerState(CycleTimerState cycleTimerState) {
        stateMachine.cycleTimerState = cycleTimerState;
        notifyCycleTimerStateChanged(cycleTimerState);
#ifdef LOGLEVEL_DEBUG
        std::map<CycleTimerState, String> stateStringRepr = {
                {CYCLE_IDLE, "CYCLE_IDLE"},
                {SET_CYCLE_START_TIME, "SET_CYCLE_START_TIME"},
                {START_CYCLE, "START_CYCLE"}
        };
        EV_DEBUG << "ScheduleManager: Changed Cycle Timer state: " << stateStringRepr[cycleTimerState] << std::endl;
#endif
    }

    virtual void notifyCycleTimerStateChanged(CycleTimerState cycleTimerState) {
        for (IScheduleManagerListener<T>* listener : listeners) {
            listener->onCycleTimerStateChanged(cycleTimerState);
        }
    }

    virtual void setListExecuteState(ListExecuteState listExecuteState) {
        stateMachine.listExecuteState = listExecuteState;
        notifyListExecuteStateChanged(listExecuteState);
#ifdef LOGLEVEL_DEBUG
        std::map<ListExecuteState, String> stateStringRepr = {
                {NEW_CYCLE, "NEW_CYCLE"},
                {INIT, "INIT"},
                {EXECUTE_CYCLE, "EXECUTE_CYCLE"},
                {END_OF_CYCLE, "END_OF_CYCLE"}
        };
        EV_DEBUG << "ScheduleManager: Changed List Execute state: " << stateStringRepr[listExecuteState] << std::endl;
#endif
    }

    virtual void notifyListExecuteStateChanged(ListExecuteState listExecuteState) {
        for (IScheduleManagerListener<T>* listener : listeners) {
            listener->onListExecuteStateChanged(listExecuteState);
        }
    }

    virtual void setListConfigState(ListConfigState listConfigState) {
        stateMachine.listConfigState = listConfigState;
        notifyListConfigStateChanged(listConfigState);
#ifdef LOGLEVEL_DEBUG
        std::map<ListConfigState, String> stateStringRepr = {
                {CONFIG_PENDING, "CONFIG_PENDING"},
                {UPDATE_CONFIG, "UPDATE_CONFIG"},
                {START_CYCLE, "START_CYCLE"}
        };
        EV_DEBUG << "ScheduleManager: Changed Cycle Timer state: " << stateStringRepr[listConfigState] << std::endl;
#endif
    }

    virtual void notifyListConfigStateChanged(ListConfigState listConfigState) {
        for (IScheduleManagerListener<T>* listener : listeners) {
            listener->onListConfigStateChanged(listConfigState);
        }
    }

    // Equivalent to setGateStates
    virtual void executeOperation() {
        for (IScheduleManagerListener<T>* listener : listeners) {
            listener->onExecuteOperation(operStates);
        }
    }

    virtual void tick(IClock* clock) override {
        // Don't change state while state machine is disabled
        if (!enabled) {
            assert(stateMachine.cycleTimerState == CYCLE_IDLE);
            assert(stateMachine.listExecuteState == INIT);
            assert(stateMachine.listConfigState == CONFIG_IDLE);
            return;
        }

        // Update Cycle Timer state machine
        switch(stateMachine.cycleTimerState) {
        case CYCLE_IDLE:
            setCycleTimerState(SET_CYCLE_START_TIME);
            clock->subscribeTick(this, 0);
            break;
        case SET_CYCLE_START_TIME:
            setCycleStartTime();
            if (cycleStartTime <= clock->getTime()) {
                setCycleTimerState(START_CYCLE);
                clock->subscribeTick(this, 0);
            } else {
                unsigned ticksTillCycleStart = std::ceil((cycleStartTime - clock->getTime()) / clock->getClockRate());
                clock->subscribeTick(this, ticksTillCycleStart);
            }
            break;
        case START_CYCLE:
            setCycleStart(true);
            setCycleTimerState(SET_CYCLE_START_TIME);
            clock->subscribeTick(this, 0);
            break;
        }

        // Update List Execute state machine
        switch(stateMachine.listExecuteState) {
        case NEW_CYCLE:
            setCycleStart(false);
            listPointer = 0;
            setListExecuteState(EXECUTE_CYCLE);
            clock->subscribeTick(this, 0);
            break;
        case INIT:
            operStates = adminStates;
            listPointer = 0;
            setListExecuteState(END_OF_CYCLE);
            clock->subscribeTick(this, 0);
            break;
        case EXECUTE_CYCLE:
            operStates = operSchedule->getControlListEntry(listPointer);
            simtime_t exitTimer = operSchedule->getTimeInterval(listPointer);
            listPointer++;
            executeOperation();
            if (listPointer >= operSchedule->getControlListLength()) {
                setListExecuteState(END_OF_CYCLE);
                clock->subscribeTick(this, 0);
            } else {
                unsigned exitTimerTicks = std::ceil(exitTimer / clock->getClockRate());
                setListExecuteState(EXECUTE_CYCLE);
                clock->subscribeTick(this, exitTimerTicks);
            }
            break;
        case END_OF_CYCLE:
            // Do nothing
            break;
        }

        // Update List Config state machine
        switch(stateMachine.listConfigState) {
        case CONFIG_PENDING:
            setConfigChange(false);
            setConfigChangeTime();
            setConfigPending(true);
            setListConfigState(UPDATE_CONFIG);
            unsigned ticksTillUpdateConfig = 0;
            if (configChangeTime > clock->getTime()) {
                ticksTillUpdateConfig = std::ceil((clock->getTime() - configChangeTime) / clock->getClockRate());
            }
            clock->subscribeTick(this, 0);
            break;
        case UPDATE_CONFIG:
            operSchedule = adminSchedule;
            setNewConfigCT(true);
            setListConfigState(CONFIG_IDLE);
            clock->subscribeTick(this, 0);
            break;
        case CONFIG_IDLE:
            setConfigPending(false);
            break;
        }
    }
};

} /* namespace nesting */

#endif /* NESTING_COMMON_SCHEDULE_SCHEDULEMANAGER_H_ */
