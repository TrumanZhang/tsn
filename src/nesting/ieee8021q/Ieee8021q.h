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

#ifndef NESTING_IEEE8021Q_IEEE8021Q_H_
#define NESTING_IEEE8021Q_IEEE8021Q_H_

#include <bitset>

#include "inet/linklayer/ethernet/Ethernet.h"
#include "../linklayer/common/Ieee8021QCtrl_m.h"

using namespace std;
using namespace inet;

namespace nesting {

/**
 * Static class holding constants relevant to the IEEE 802.1Q standard.
 */
class Ieee8021q final {
public:
    const static int kEthernet2MinPayloadByteLength = 42;
    const static int kEthernet2MinPayloadBitLength =
            kEthernet2MinPayloadByteLength * 8;
    const static int kEthernet2MaximumTransmissionUnitByteLength = 1500;
    const static int kEthernet2MaximumTransmissionUnitBitLength =
            kEthernet2MaximumTransmissionUnitByteLength * 8;

    const static int kVLANTagByteLength = 4;
    const static int kVLANTagBitLength = kVLANTagByteLength * 8;

    const static int kDefaultPCPValue = 0;
    const static int kNumberOfPCPValues = 8;

    const static bool kDefaultDEIValue = false;

    const static int kMinValidVID = 1;
    const static int kMaxValidVID = 4094;
    const static int kDefaultVID = 1;

    const static int kMaxSupportedQueues = kNumberOfPCPValues;
    const static int kMinSupportedQueues = 1;

    const static int selfMessageSchedulingPriority = 1;

    const static int kFramePreemptionMinFinalPayloadSize = 60;
    const static int kFramePreemptionMinNonFinalPayloadSize = 60; //or 124,188,252

    /**
     * This method calculates the final packet size of a payload-packet after
     * going through the encapsulation processes of the lower layers of the
     * IEEE802.1Q switch.
     *
     * This method is meant as a temporary solution to calculate final packet
     * sizes for the functionality of some IEEE802.1Q relevant functionalities
     * like the credit based shaper. Changes to the LowerLayer-compound module
     * of the IEEE802.1Q ethernet switch can break the functionality of this
     * method.
     */
    static uint64_t getFinalEthernet2FrameBitLength(cPacket* packet) {
        Ieee8021QCtrl* ctrlInfo = check_and_cast<Ieee8021QCtrl*>(
                packet->getControlInfo());

        // Add payload length
        uint64_t bitLength = packet->getBitLength();

        // Add q-tag length
        if (ctrlInfo->isTagged()) {
            bitLength += kVLANTagBitLength;
        }

        // Add MAC header length
        bitLength += ETHER_MAC_FRAME_BYTES;

        // Add physical header length
        bitLength += PREAMBLE_BYTES;
        bitLength += SFD_BYTES;

        return bitLength;
    }
};

typedef bitset<static_cast<unsigned long>(Ieee8021q::kMaxSupportedQueues)> GateBitvector;

} // namespace nesting

#endif /* NESTING_IEEE8021Q_IEEE8021Q_H_ */
