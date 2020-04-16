#pragma once
#include <vector>
#include <bitset>

// rough BitArray class to have some C# signature similarity for helping with porting test cases.
class BitArray {
public:
    BitArray(std::vector<unsigned char>);
    BitArray(int size, bool default_value);
    bool operator[] (int x);
    size_t size() { return _size;}
    void set(int idx, bool value);
private:
    std::vector<std::bitset<8>> bs;
    int _size;
};