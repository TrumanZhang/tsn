#include "inet/linklayer/ethernet/EtherFrame_m.h"

namespace nesting {
struct Ieee8021QCtrl_2 {
    IntrusivePtr<Ieee802_1QHeader> q1Header;
    IntrusivePtr<EthernetMacHeader> macHeader;
};

} // namespace nesting
