#pragma once

#include "public/bigintegercpp/BigInteger.h"

struct FastReducer {
public:
    FastReducer(uint_array modulus);

    int Reduce(uint_array, int length);

private:
    uint_array _modulus;
    uint_array _mu;
    uint_array _q1;
    uint_array _q2;
    int _muLength;

    int DivMul(uint_array& left, int leftLength, uint_array& right, int rightLength, uint_array& bits, int k);

    int SubMod(uint_array left, int leftLength, uint_array right, int rightLength, uint_array modulus, int k);

};