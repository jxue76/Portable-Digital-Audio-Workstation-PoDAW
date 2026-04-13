#include "Guitar.hpp"
#include "Plucked.h"

Guitar::Guitar() : Instrument("Guitar", std::make_shared<stk::Plucked>(), 40, 88, 1.0f) {
}
