#include <bitset>
#include "MyBigInt.h"
#include "BitArray.h"

byte_array MyBigIntImp::GetRandomByteArray(Random random, int size) {
    byte_array value(size);
    random.NextBytes(value);
    return value;
}

std::string MyBigIntImp::Print(byte_array bytes) {
    std::string ret("make ");
    for (auto& b : bytes)
        ret += static_cast<char>(b) + std::string(" ");
    ret += "endmake ";
    return ret;
}

BigInteger MyBigIntImp::DoUnaryOperatorMine(BigInteger num1, std::string op) {
    auto bytes1 = num1.ToByteArray();
    int factor;
    double result;

    if (op == "uSign") {
    } else if (op == "u~") {
    } else if (op == "uLog10") {
    } else if (op == "uLog") {
    } else if (op == "uAbs") {
    } else if (op == "u--") {
    } else if (op == "u++") {
    } else if (op == "uNegate") {
    } else if (op == "u-") {
    } else if (op == "u+") {
    } else if (op == "uMultiply") {
    } else if (op == "u*") {
    } else {
        throw std::invalid_argument("Invalid operation found");
    }
}

BigInteger MyBigIntImp::DoTertanaryOperatorMine(BigInteger num1, BigInteger num2, BigInteger num3, std::string op) {
    return BigInteger();
}

BigInteger MyBigIntImp::DoBinaryOperatorMine(BigInteger num1, BigInteger num2, std::string op) {
    BigInteger num3;
    return DoBinaryOperatorMine(num1, num2, op, &num3);
}

BigInteger MyBigIntImp::DoBinaryOperatorMine(BigInteger num1, BigInteger num2, std::string op, BigInteger* num3) {
    auto bytes1 = num1.ToByteArray();
    auto bytes2 = num2.ToByteArray();
    *num3 = 0;

    if (op == "bMin") {

    } else if (op == "bMax") {
    } else if (op == "b>>") {
    } else if (op == "b<<") {
        return BigInteger{ShiftLeft(bytes1, bytes2)};
    } else if (op == "b^") {
    } else if (op == "b|") {
    } else if (op == "b&") {
    } else if (op == "bLog") {
    } else if (op == "bGCD") {
    } else if (op == "bPow") {
    } else if (op == "bDivRem") {
    } else if (op == "bRemainder") {
    } else if (op == "b%") {
    } else if (op == "bDivide") {
    } else if (op == "b/") {
    } else if (op == "bMultiply") {
    } else if (op == "b*") {
    } else if (op == "bSubtract") {
    } else if (op == "b-") {
    } else if (op == "bAdd") {
    } else if (op == "b+") {

    } else {
        throw std::invalid_argument("Invalid operation found");
    }
    return BigInteger();
}

byte_array MyBigIntImp::ShiftLeft(byte_array bytes1, byte_array bytes2) {
    byte_array tmp1{8};

    int byteShift = (int)BigInteger{Divide(Copy(bytes2), tmp1)};
    signed char bitShift = (signed char)(int)BigInteger{Remainder(bytes2, tmp1)};

    for (int i = 0; i < abs(bitShift); i++) {
        if (bitShift < 0)
            bytes1 = ShiftRight(bytes1);
        else
            bytes1 = ShiftLeftGrow(bytes1);
    }

    if (byteShift < 0) {
        byteShift = -byteShift;
        if (byteShift >= bytes1.size()) {
            if ((bytes1[bytes1.size() - 1] & 0x80) != 0)
                bytes1 = byte_array{0xff};
            else
                bytes1 = byte_array{0x0};
        } else {
            byte_array temp;
            for (int i = byteShift; i < bytes1.max_size(); i++) {
                temp.push_back(bytes1[i]);
            }
            bytes1 = temp;
        }
    } else {
        byte_array temp;
        for (int i = 0; i < byteShift; i++)
            temp.push_back((byte)0);
        for (int i = 0; i < bytes1.size(); i++)
            temp.push_back(bytes1[i]);
        bytes1 = temp;
    }
    return bytes1;
}

byte_array MyBigIntImp::ShiftRight(byte_array bytes) {
    byte_array bresult;
    for (int i = 0; i < bytes.size(); i++) {
        auto newbyte = bytes[i];
        newbyte = (byte)(newbyte / 2);
        if ((i != (bytes.size() - 1)) && ((bytes[i + 1] & 0x1) == 1))
            newbyte += 128;
        if ((i == (bytes.size()-1)) && ((bytes[bytes.size()-1] & 0x80) != 0))
            newbyte += 128;
        bresult.push_back(newbyte);
    }
    return byte_array();
}

byte_array MyBigIntImp::ShiftLeftGrow(byte_array bytes) {
    byte_array bresult;
    for (int i = 0; i < bytes.size(); i++) {
        auto newbyte = bytes[i];
        if (newbyte > 127)
            newbyte -= 128;
        newbyte = (byte)(newbyte * 2);
        if ((i != 0) && (bytes[i-1] >= 128))
            newbyte++;
        bresult.push_back(newbyte);
    }
    if ((bytes[bytes.size() - 1] > 63) && (bytes[bytes.size() - 1] < 128))
    {
        bresult.push_back((byte)0);
    }
    if ((bytes[bytes.size() - 1] > 127) && (bytes[bytes.size() - 1] < 192))
    {
        bresult.push_back((byte)0xFF);
    }
    return bresult;
}

byte_array MyBigIntImp::Copy(byte_array bytes) {
    byte_array ret;
    std::copy(bytes.begin(), bytes.end(), ret.begin());
    return ret;
}

byte_array MyBigIntImp::Divide(byte_array bytes1, byte_array bytes2) {
    bool numPos = ((bytes1[bytes1.size() - 1] & 0x80) == 0);
    bool denPos = ((bytes2[bytes2.size() - 1] & 0x80) == 0);

    if (!numPos)
    {
        bytes1 = Negate(bytes1);
    }
    if (!denPos)
    {
        bytes2 = Negate(bytes2);
    }

    bool qPos = (numPos == denPos);

    Trim(bytes1);
    Trim(bytes2);

    BitArray ba1{bytes1};
    BitArray ba2{bytes2};

    int ba11loc = 0;
    for (int i = ba1.size() - 1; i >= 0; i--)
    {
        if (ba1[i])
        {
            ba11loc = i;
            break;
        }
    }

    int ba21loc = 0;
    for (int i = ba2.size() - 1; i >= 0; i--)
    {
        if (ba2[i])
        {
            ba21loc = i;
            break;
        }
    }

    int shift = ba11loc - ba21loc;
    if (shift < 0)
    {
        return byte_array{ (byte)0 };
    }

    BitArray br{shift + 1, false};

    for (int i = 0; i < shift; i++)
    {
        bytes2 = ShiftLeftGrow(bytes2);
    }

    while (shift >= 0)
    {
        bytes2 = Negate(bytes2);
        bytes1 = Add(bytes1, bytes2);
        bytes2 = Negate(bytes2);
        if (bytes1[bytes1.size() - 1] < 128)
        {
            br.set(shift, true);
        }
        else
        {
            bytes1 = Add(bytes1, bytes2);
        }
        bytes2 = ShiftRight(bytes2);
        shift--;
    }
    auto result = GetBytes(br);

    if (!qPos)
    {
        result = Negate(result);
    }

    return result;
}

byte_array MyBigIntImp::GetBytes(BitArray ba) {
    // for whatever reason this returns a list with a count of 5 instead of 4
    //    List<byte> bb = new List<byte>(){1, 0, 2, 0x80};
    //    BitArray ba1 = new BitArray(bb.ToArray());
    //    var l = GetBytes(ba1);

    int length = ((ba.size()) / 8) + 1;

    byte_array mask{ 0 };

    for (int i = length - 1; i >= 0; i--)
    {
        for (int j = 7; j >= 0; j--)
        {
            mask = ShiftLeftGrow(mask);
            if ((8 * i + j < ba.size()) && (ba[8 * i + j]))
            {
                mask[0] |= (byte)1;
            }
        }
    }

    return mask;
}

byte_array MyBigIntImp::Remainder(byte_array bytes1, byte_array bytes2) {
    bool numPos = ((bytes1[bytes1.size() - 1] & 0x80) == 0);
    bool denPos = ((bytes2[bytes2.size() - 1] & 0x80) == 0);

    if (!numPos)
    {
        bytes1 = Negate(bytes1);
    }
    if (!denPos)
    {
        bytes2 = Negate(bytes2);
    }

    Trim(bytes1);
    Trim(bytes2);

    BitArray ba1{bytes1};
    BitArray ba2{bytes2};

    int ba11loc = 0;
    for (int i = ba1.size() - 1; i >= 0; i--)
    {
        if (ba1[i])
        {
            ba11loc = i;
            break;
        }
    }
    int ba21loc = 0;
    for (int i = ba2.size() - 1; i >= 0; i--)
    {
        if (ba2[i])
        {
            ba21loc = i;
            break;
        }
    }
    int shift = ba11loc - ba21loc;
    if (shift < 0)
    {
        if (!numPos)
        {
            bytes1 = Negate(bytes1);
        }
        return bytes1;
    }
    BitArray br{shift + 1, false};

    for (int i = 0; i < shift; i++)
    {
        bytes2 = ShiftLeftGrow(bytes2);
    }

    while (shift >= 0)
    {
        bytes2 = Negate(bytes2);
        bytes1 = Add(bytes1, bytes2);
        bytes2 = Negate(bytes2);
        if (bytes1[bytes1.size() - 1] < 128)
        {
            br.set(shift, true);
        }
        else
        {
            bytes1 = Add(bytes1, bytes2);
        }
        bytes2 = ShiftRight(bytes2);
        shift--;
    }

    if (!numPos)
    {
        bytes1 = Negate(bytes1);
    }
    return bytes1;
}

byte_array MyBigIntImp::Negate(byte_array bytes) {
    bool carry;
    byte_array bnew;
    bool bsame;

    for (int i = 0; i < bytes.size(); i++)
    {
        bytes[i] ^= 0xFF;
    }
    carry = false;
    for (int i = 0; i < bytes.size(); i++)
    {
        int temp = (i == 0 ? 0x01 : 0x00) + bytes[i];
        if (carry)
        {
            temp++;
        }
        carry = false;

        if (temp > 0xFF)//byte.MaxValue)
        {
            temp -= 0xFF + 1;
            carry = true;
        }

        bnew.push_back((byte)temp);
    }

    bsame = ((bnew[bnew.size() - 1] & 0x80) != 0);
    bsame &= ((bnew[bnew.size() - 1] & 0x7f) == 0);
    for (int i = bnew.size() - 2; i >= 0; i--)
    {
        bsame &= (bnew[i] == 0);
    }
    if (bsame)
    {
        bnew.push_back((byte)0);
    }

    return bnew;
}

void MyBigIntImp::Trim(byte_array bytes) {
    while (bytes.size() > 1)
    {
        if ((bytes[bytes.size() - 1] & 0x80) == 0)
        {
            if ((bytes[bytes.size() - 1] == 0) & ((bytes[bytes.size() - 2] & 0x80) == 0))
            {
                //bytes.RemoveAt(bytes.Count - 1);
                bytes.pop_back();
            }
            else
            {
                break;
            }
        }
        else
        {
            if ((bytes[bytes.size() - 1] == 0xFF) & ((bytes[bytes.size() - 2] & 0x80) != 0))
            {
                //bytes.RemoveAt(bytes.Count - 1);
                bytes.pop_back();
            }
            else
            {
                break;
            }
        }
    }
}

byte_array MyBigIntImp::Add(byte_array bytes1, byte_array bytes2) {
    byte_array bnew;
    bool num1neg = (bytes1[bytes1.size() - 1] & 0x80) != 0;
    bool num2neg = (bytes2[bytes2.size() - 1] & 0x80) != 0;
    byte extender = 0;
    bool bnewneg;
    bool carry;

    NormalizeLengths(bytes1, bytes2);

    carry = false;
    for (int i = 0; i < bytes1.size(); i++)
    {
        int temp = bytes1[i] + bytes2[i];

        if (carry)
        {
            temp++;
        }
        carry = false;

        if (temp > 0xFF)// byte.MaxValue)
        {
            temp -= 0xFF + 1;
            carry = true;
        }

        bnew.push_back((byte)temp);
    }
    bnewneg = (bnew[bnew.size() - 1] & 0x80) != 0;

    if ((num1neg == num2neg) & (num1neg != bnewneg))
    {
        if (num1neg)
        {
            extender = 0xff;
        }
        bnew.push_back(extender);
    }

    return bnew;
}

void MyBigIntImp::NormalizeLengths(byte_array bytes1, byte_array bytes2) {
    bool num1neg = (bytes1[bytes1.size() - 1] & 0x80) != 0;
    bool num2neg = (bytes2[bytes2.size() - 1] & 0x80) != 0;
    byte extender = 0;

    if (bytes1.size() < bytes2.size())
    {
        if (num1neg)
        {
            extender = 0xff;
        }
        while (bytes1.size() < bytes2.size())
        {
            bytes1.push_back(extender);
        }
    }
    if (bytes2.size() < bytes1.size())
    {
        if (num2neg)
        {
            extender = 0xff;
        }
        while (bytes2.size() < bytes1.size())
        {
            bytes2.push_back(extender);
        }
    }
}

BigInteger MyBigIntImp::ApproximateBigInteger(double value) {
    //TODO implement
    //Special case values;
//    if (double.IsNaN(value))
//    {
//        return BigInteger(-101);
//    }
//    if (double.IsNegativeInfinity(value))
//    {
//        return new BigInteger(-102);
//    }
//    if (double.IsPositiveInfinity(value))
//    {
//        return new BigInteger(-103);
//    }
//
//    BigInteger result = new BigInteger(Math.Round(value, 0));
//
//    if (result != 0)
//    {
//        bool pos = (value > 0);
//        if (!pos)
//        {
//            value = -value;
//        }
//
//        int size = (int)Math.Floor(Math.Log10(value));
//
//        //keep only the first 17 significant digits;
//        if (size > 17)
//        {
//            result = result - (result % BigInteger.Pow(10, size - 17));
//        }
//
//        if (!pos)
//        {
//            value = -value;
//        }
//    }
//
//    return result;
return BigInteger::Zero(); // remove this, just for compilation
}
