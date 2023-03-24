#ifndef ID_GENERATOR_HPP
#define ID_GENERATOR_HPP

#include "pch.hpp"

#include <unordered_map>
namespace ID {
static unsigned int idCounter = 0;

unsigned int Hash(unsigned int x) {

    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}
unsigned int GeneratorId() {
    idCounter++;
    return idCounter;
}
} // namespace ID

#endif