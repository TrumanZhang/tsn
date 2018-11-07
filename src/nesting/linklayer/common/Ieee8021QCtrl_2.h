#include "inet/linklayer/ethernet/EtherFrame_m.h"

namespace nesting {
struct Ieee8021QCtrl_2 {
    Ieee802_1QHeader q1Header;
    EthernetMacHeader macHeader;
};

} // namespace nesting
