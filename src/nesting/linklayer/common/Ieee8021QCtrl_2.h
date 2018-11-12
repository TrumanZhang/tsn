#include "VLANTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"

namespace nesting {
struct Ieee8021QCtrl_2 {
    VLANTagReq q1Tag;
    MacAddressReq macTag;
};

} // namespace nesting
