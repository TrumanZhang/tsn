/*
 * SchedVLANEtherTrafGen.h
 *
 *
 */

#ifndef NESTING_APPLICATION_ETHERNET_SCHEDETHERVLANTRAFGEN_H_
#define NESTING_APPLICATION_ETHERNET_SCHEDETHERVLANTRAFGEN_H_

#include <omnetpp.h>
#include <memory>

#include "inet/applications/ethernet/EtherTrafGen.h"
#include "../../linklayer/common/Ieee8021QCtrl_m.h"
#include "../../ieee8021q/clock/IClock.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/InitStages.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/Protocol.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/Ieee802SapTag_m.h"
#include <omnetpp/cxmlelement.h>
#include <vector>
#include "../../common/schedule/HostSchedule.h"
#include "../../common/schedule/HostScheduleBuilder.h"

// TODO check if includes can be discarded

using namespace omnetpp;
using namespace inet;

namespace nesting {

/**
 * See the NED file for a detailed description
 */
class SchedEtherVLANTrafGen: public cSimpleModule, public IClockListener {
private:

    /** Current schedule. Is never null. */
    unique_ptr<HostSchedule<Ieee8021QCtrl_2>> currentSchedule;

    /**
     * Next schedule to load after the current schedule finishes it's cycle.
     * Can be null.
     */
    unique_ptr<HostSchedule<Ieee8021QCtrl_2>> nextSchedule;

    /** Index for the current entry in the schedule. */
    long int index = 0;

    IClock *clock;

protected:

    // receive statistics
    long TSNpacketsSent = 0;
    long packetsReceived = 0;
    simsignal_t sentPkSignal;
    simsignal_t rcvdPkSignal;
    int ssap = -1;
    int dsap = -1;

    int seqNum = 0;

    virtual void initialize(int stage) override;
    virtual void sendPacket();
    virtual void receivePacket(Packet *msg);
    virtual void handleMessage(cMessage *msg) override;

    virtual int numInitStages() const override;
    virtual int scheduleNextTickEvent();
public:
    virtual void tick(IClock *clock) override;

    /** Loads a new schedule into the gate controller. */
    virtual void loadScheduleOrDefault(cXMLElement* xml);
};

} // namespace nesting

#endif /* NESTING_APPLICATION_ETHERNET_SCHEDETHERVLANTRAFGEN_H_ */
