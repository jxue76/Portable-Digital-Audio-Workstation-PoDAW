#include "Bass.hpp"
#include "StifKarp.h"

Bass::Bass() : Instrument("Bass", std::make_shared<stk::StifKarp>(), 36, 81, 1.0f) {
}