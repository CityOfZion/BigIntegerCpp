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
    // From the C# source
    // Fills the byte array with random bytes [0..0x7f].  The entire array is filled.
    for (auto& element : buffer)
        element = static_cast<byte>(Next(0, 0x7f));
}

double Random::NextDouble() {
    return Next() * (1.0 / std::numeric_limits<int>::max());
}
