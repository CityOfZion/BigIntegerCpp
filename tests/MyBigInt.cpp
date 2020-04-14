#include "MyBigInt.h"

byte_array MyBigIntImp::GetRandomByteArray(Random random, int size) {
    byte_array value(size);
    random.NextBytes(value);
    return value;
}

std::string MyBigIntImp::Print(byte_array bytes) {
    std::string ret("make");
    for (auto& b : bytes)
        ret += static_cast<char>(b) + std::string(" ");
    return ret;
}

BigInteger MyBigIntImp::DoUnaryOperatorMine(BigInteger num1, std::string op) {
    return BigInteger();
}

BigInteger MyBigIntImp::DoTertanaryOperatorMine(BigInteger num1, BigInteger num2, BigInteger num3, std::string op) {
    return BigInteger();
}
