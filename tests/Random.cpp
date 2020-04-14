#include <stdexcept>
#include <BigInteger.h>
#include "Random.h"

int Random::Next(int min, int max) {
    if (min > max)
        throw std::out_of_range("");
    std::uniform_int_distribution dist(min, max);
    return dist(engine);
}

void Random::NextBytes(byte_array& buffer) {
    for (auto& element : buffer)
        element = (byte)Next();
}
