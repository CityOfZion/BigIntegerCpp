#include <stdexcept>
#include "BitArray.h"

BitArray::BitArray(std::vector<unsigned char> bytes) {
    _size = 8 * bytes.size();
    for (auto& b : bytes)
        bs.push_back(std::bitset<8>(b));
}

BitArray::BitArray(int size, bool default_value) {
    _size = size;
    int count = (size / 8) + 1;
    while (count > 0) {
        bs.push_back(std::bitset<8>(default_value));
        count--;
    }
}

bool BitArray::operator[](int idx) {
    if (idx > _size-1)
        throw std::out_of_range("");
    int v_idx = idx/8;
    int bs_idx = idx % 8;
    return (bool)bs[v_idx][bs_idx];
}

void BitArray::set(int idx, bool value) {
    if (idx > _size-1)
        throw std::out_of_range("");
    int v_idx = idx/8;
    int bs_idx = idx % 8;
    bs[v_idx][bs_idx] = value;
}
