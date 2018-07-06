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

#ifndef NESTING_IEEE8021Q_BRIDGE_VLANENCAP_VLANENCAP_H_
#define NESTING_IEEE8021Q_BRIDGE_VLANENCAP_VLANENCAP_H_

#include <omnetpp.h>

#include "inet/linklayer/common/Ieee802Ctrl.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "../common/Ieee8021QCtrl_m.h"
#include "../../ieee8021q/Ieee8021q.h"

using namespace omnetpp;
using namespace inet;

namespace nesting {

/**
 * See the NED file for a detailed description
 */
class VLANEncap : public cSimpleModule {
private:

  /** detailed prints for testing */
  bool verbose;
  /** Parameter from NED file. */
  bool tagUntaggedFrames;

  /** Parameter from NED file. */
  int pvid;
private:
  /**
   * Processes packets from lower level and possibly performs decapsulation
   * when the packet is of type Ether1QTag (is a VLAN Tag)
   * @param packet The packet that was received from lower level.
   */
  virtual void processPacketFromLowerLevel(cPacket* packet);

  /**
   * Processes packets from higher level and possibly performs
   * encapsulation when the control information says so.
   * @param packet The packet received from  higher level.
   */
  virtual void processPacketFromHigherLevel(cPacket* packet);
protected:
  /** Signal for encapsulation events. */
  simsignal_t encapPkSignal;

  /** Signal for decapsulation events. */
  simsignal_t decapPkSignal;

  /** Total amount of packets received from higher layer. */
  long totalFromHigherLayer;

  /** Total amount of packets received from lower layer. */
  long totalFromLowerLayer;

  /** Total amount of packets encapsulated. */
  long totalEncap;

  /** Total amount of packets decapsulated. */
  long totalDecap;
protected:
  /** @see cSimpleModule::initialize() */
  virtual void initialize() override;

  /** @see cSimpleModule::handleMessage(cMessage*) */
  virtual void handleMessage(cMessage *msg) override;

  /** @see cSimpleModule::refreshDisplay() */
  virtual void refreshDisplay() const override;
public:
  virtual ~VLANEncap() {};

  virtual int getPVID();
};

} // namespace nesting

#endif /* NESTING_IEEE8021Q_BRIDGE_VLANENCAP_VLANENCAP_H_ */
