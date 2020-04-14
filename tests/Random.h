#pragma once
#include <random>

class Random {
public:
    Random() =default;
    Random(int seed) : engine{seed} {};
    int Next() { return engine(); };
    int Next(int min, int max);

    void NextBytes(std::vector<unsigned char>& vector);

private:
    const unsigned long MSEED = 161803398;
    std::mt19937 engine{MSEED};
};
