#include "utils.h"
#include <string>
#include <sstream>
#include "Random.h"

std::string BuildRandomNumber(int maxdigits, int seed)
{
    Random random(seed);

    // Ensure that we have at least 1 digit
    int numDigits = random.Next() % maxdigits + 1;

    std::string randNum;

    // We'll make some numbers negative
    while (randNum.size() < numDigits)
    {
        randNum += std::to_string(random.Next());
    }
    if (random.Next() % 2 == 0)
    {
        return "-" + randNum.substr(0, numDigits);
    }
    else
    {
        return randNum.substr(0, numDigits);
    }
}