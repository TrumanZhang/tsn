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

#ifndef __MAIN_FORWARDINGRELAYUNIT_H_
#define __MAIN_FORWARDINGRELAYUNIT_H_

#include <unordered_map>
#include <omnetpp.h>

#include "inet/common/ModuleAccess.h"
#include "../../linklayer/common/Ieee8021QCtrl_m.h"
#include "FilteringDatabase.h"

using namespace omnetpp;
using namespace inet;

namespace nesting {

/**
 * See the NED file for a detailed description
 */
class ForwardingRelayUnit : public cSimpleModule {
private:
  FilteringDatabase* fdb;
  int numberOfPorts;
  //TODO: Create parameter for filtering database aging
  simtime_t fdbAgingThreshold = 1000;
protected:
  virtual void initialize();
  virtual void handleMessage(cMessage* msg);
  virtual void processBroadcast(cPacket* packet);
  virtual void processMulticast(cPacket* packet);
  virtual void processUnicast(cPacket* packet);
  virtual cPacket* duplicatePacketWithCtrlInfo(cPacket* packet);

public:
  //TODO: Fix filtering database aging parameter!
//  ForwardingRelayUnit() : fdb(1000) {};
};

} // namespace nesting

#endif
