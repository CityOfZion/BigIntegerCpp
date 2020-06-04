#pragma once
#include <random>

class Random {
public:
    Random() : engine(MSEED) {};
    Random(int seed) : engine(seed) {};
    int Next() { return abs(static_cast<int>(engine())); };
    int Next(int min, int max);

    double NextDouble();
    void NextBytes(std::vector<unsigned char>& vector);

private:
    const unsigned long MSEED = 161803398;
    std::mt19937 engine;
};
