#ifndef ID_GENERATOR_HPP
#define ID_GENERATOR_HPP

#include "pch.hpp"

namespace ID {
static unsigned int idCounter = 0;

unsigned int GeneratorID() {
    return idCounter++;
}
} // namespace ID

#endif