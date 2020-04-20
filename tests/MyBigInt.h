#pragma once
#include "BigInteger.h"
#include "Random.h"
#include "BitArray.h"
#include <string>
#include <vector>

class MyBigIntImp {
public:
    static BigInteger DoUnaryOperatorMine(BigInteger num1, std::string op);
    static BigInteger DoBinaryOperatorMine(BigInteger num1, BigInteger num2, std::string op);
    static BigInteger DoBinaryOperatorMine(BigInteger num1, BigInteger num2, std::string op, BigInteger* num3);
    static BigInteger DoTertanaryOperatorMine(BigInteger num1, BigInteger num2, BigInteger num3, std::string op);
    static byte_array Add(byte_array bytes1, byte_array bytes2);
    static byte_array Negate(byte_array bytes);
    static byte_array Multiply(byte_array bytes1, byte_array bytes2);
    static byte_array Divide(byte_array bytes1, byte_array bytes2);
    static byte_array Remainder(byte_array bytes1, byte_array bytes2);
    static byte_array Pow(byte_array bytes1, byte_array bytes2);
    static byte_array ModPow(byte_array bytes1, byte_array bytes2, byte_array bytes3);
    static byte_array GCD(byte_array bytes1, byte_array bytes2);
    static byte_array Max(const byte_array& bytes1, const byte_array& bytes2);
    static byte_array And(byte_array bytes1, byte_array bytes2);
    static byte_array Or(byte_array bytes1, byte_array bytes2);
    static byte_array Xor(byte_array bytes1, byte_array bytes2);
    static byte_array Not(const byte_array& bytes);
    static byte_array ShiftLeft(byte_array& bytes1, const byte_array& bytes2);
    static byte_array ShiftLeftGrow(byte_array bytes);
    static byte_array ShiftLeftDrop(const byte_array& bytes);
    static byte_array ShiftRight(byte_array bytes);
    static byte_array SetLength(const byte_array& bytes, int size);
    static byte_array Copy(byte_array bytes);
    static void NormalizeLengths(byte_array& bytes1, byte_array& bytes2);
    static void Trim(byte_array& bytes);
    static byte_array GetBytes(BitArray ba);
    static std::string Print(byte_array bytes);
    static std::string PrintFormatX(byte_array bytes);
    static std::string PrintFormatX2(byte_array bytes);
    static bool IsZero(byte_array& list);
    static byte_array GetNonZeroRandomByteArray(Random random, int size);
    static byte_array GetRandomByteArray(Random random, int size);
    static BigInteger ApproximateBigInteger(double value);
};