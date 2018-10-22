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

#ifndef __MAIN_ETHERVLANTAGGEDTRAFGEN_H_
#define __MAIN_ETHERVLANTAGGEDTRAFGEN_H_

#include <omnetpp.h>

#include "inet/applications/ethernet/EtherTrafGen.h"
#include "../../linklayer/common/Ieee8021QCtrl_m.h"

using namespace omnetpp;
using namespace inet;

namespace nesting {

/** See the NED file for a detailed description */
class EtherVLANTaggedTrafGen: public EtherTrafGen {
private:
    // Parameters from NED file
    cPar* vlanTagEnabled;
    cPar* pcp;
    cPar* dei;
    cPar* vid;
protected:
    virtual void initialize(int stage) override;
    virtual void sendBurstPackets() override;
};

} // namespace nesting

#endif
