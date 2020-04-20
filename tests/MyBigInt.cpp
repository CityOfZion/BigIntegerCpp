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
        if (IsZero(bytes1))
        {
            return BigInteger(0);
        }
        auto tmp = Max(bytes1, byte_array(0));
        if (IsZero(tmp))
        {
            return BigInteger(-1);
        }
        return BigInteger(1);
    } else if (op == "u~") {
        return BigInteger(Not(bytes1));
    } else if (op == "uLog10") {
        factor = static_cast<int>(BigInteger::Log(num1, 10));
        if (factor > 100)
        {
            BigInteger b10(10);
            for (int i = 0; i < factor - 100; i++)
            {
                num1 = num1 / b10;
            }
        }
        result = log10((double)num1);
        if (factor > 100)
        {
            for (int i = 0; i < factor - 100; i++)
            {
                result = result + 1;
            }
        }
        return ApproximateBigInteger(result);
    } else if (op == "uLog") {
        factor = static_cast<int>(BigInteger::Log(num1, 10));
        if (factor > 100)
        {
            BigInteger b10(10);
            for (int i = 0; i < factor - 100; i++)
            {
                num1 = num1 / b10;
            }
        }
        result = log((double)num1);
        if (factor > 100)
        {
            for (int i = 0; i < factor - 100; i++)
            {
                result = result + log(10);
            }
        }
        return ApproximateBigInteger(result);
    } else if (op == "uAbs") {
        if ((bytes1[bytes1.size() - 1] & 0x80) != 0)
        {
            bytes1 = Negate(bytes1);
        }
        return BigInteger(bytes1);
    } else if (op == "u--") {
        return BigInteger(Add(bytes1, byte_array{ 0xff }));
    } else if (op == "u++") {
        return BigInteger(Add(bytes1, byte_array{ 1 }));
    } else if (op == "uNegate" || op == "u-") {
        return BigInteger(Negate(bytes1));
    } else if (op == "u+") {
        return num1;
    } else if (op == "uMultiply" || "u*") {
        return BigInteger(Multiply(bytes1, bytes1));
    } else {
        throw std::invalid_argument("Invalid operation found");
    }
}

BigInteger MyBigIntImp::DoTertanaryOperatorMine(BigInteger num1, BigInteger num2, BigInteger num3, std::string op) {
    auto bytes1 = num1.ToByteArray();
    auto bytes2 = num2.ToByteArray();
    auto bytes3 = num3.ToByteArray();
    if (op == "tModPow")
        return BigInteger(ModPow(bytes1, bytes2, bytes3));
    else
        throw std::invalid_argument("Invalid operation found");
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
        return BigInteger(Negate(Max(Negate(bytes1), Negate(bytes2))));
    } else if (op == "bMax") {
        return BigInteger(Max(bytes1, bytes2));
    } else if (op == "b>>") {
        return BigInteger(ShiftLeft(bytes1, Negate(bytes2)));
    } else if (op == "b<<") {
        return BigInteger{ShiftLeft(bytes1, bytes2)};
    } else if (op == "b^") {
        return BigInteger(Xor(bytes1, bytes2));
    } else if (op == "b|") {
        return BigInteger(Or(bytes1, bytes2));
    } else if (op == "b&") {
        return BigInteger(And(bytes1, bytes2));
    } else if (op == "bLog") {
        return ApproximateBigInteger(log((double)num1) / log((double)num2));
    } else if (op == "bGCD") {
        return BigInteger(GCD(bytes1, bytes2));
    } else if (op == "bPow") {
        int arg2 = (int)num2;
        bytes2 = BigInteger(arg2).ToByteArray();
        return BigInteger(Pow(bytes1, bytes2));
    } else if (op == "bDivRem") {
        BigInteger ret = BigInteger(Divide(bytes1, bytes2));
        bytes1 = num1.ToByteArray();
        bytes2 = num2.ToByteArray();
        *num3 = BigInteger(Remainder(bytes1, bytes2));
        return ret;
    } else if (op == "bRemainder" || "b%") {
        return BigInteger(Remainder(bytes1, bytes2));
    } else if (op == "bDivide" || "b/") {
        return BigInteger(Divide(bytes1, bytes2));
    } else if (op == "bMultiply" || "b*") {
        return BigInteger(Multiply(bytes1, bytes2));
    } else if (op == "bSubtract" || "b-") {
        bytes2 = Negate(bytes2);
        return BigInteger(Add(bytes1, bytes2));
    } else if (op == "bAdd" || "b+") {
        return BigInteger(Add(bytes1, bytes2));
    } else {
        throw std::invalid_argument("Invalid operation found");
    }
}

byte_array MyBigIntImp::ShiftLeft(byte_array& bytes1, const byte_array& bytes2) {
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
            for (int i = byteShift; i < bytes1.size(); i++) {
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
    return bresult;
}

byte_array MyBigIntImp::ShiftLeftGrow(byte_array bytes) {
    byte_array bresult;
    for (int i = 0; i < bytes.size(); i++) {
        auto newbyte = bytes[i];
        if (newbyte > 127)
            newbyte -= 128;
        newbyte = static_cast<byte>((newbyte * 2));
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
    byte_array ret(bytes.size());
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

void MyBigIntImp::Trim(byte_array& bytes) {
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

void MyBigIntImp::NormalizeLengths(byte_array& bytes1, byte_array& bytes2) {
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

double round_up(double value, int decimal_places) {
    const double multiplier = std::pow(10.0, decimal_places);
    return std::ceil(value * multiplier) / multiplier;
}

BigInteger MyBigIntImp::ApproximateBigInteger(double value) {
    //Special case values;
    if (BigInteger::double_IsNaN(value))
    {
        return BigInteger(-101);
    }
    if (value == -std::numeric_limits<double>::infinity())
    {
        return BigInteger(-102);
    }
    if (value == std::numeric_limits<double>::infinity())
    {
        return BigInteger(-103);
    }

    BigInteger result(round_up(value, 0));

    if (result != BigInteger::Zero())
    {
        bool pos = (value > 0);
        if (!pos)
        {
            value = -value;
        }

        int size = static_cast<int>(floor(log10(value)));

        //keep only the first 17 significant digits;
        if (size > 17)
        {
            result = result - (result % BigInteger::Pow(10, size - 17));
        }

        if (!pos)
        {
            value = -value;
        }
    }

    return result;
}

bool MyBigIntImp::IsZero(byte_array& list) {
    for (auto& b : list)
        if (b != 0)
            return false;
    return true;
}

byte_array MyBigIntImp::Max(const byte_array& bytes1, const byte_array& bytes2) {
    bool b1Pos = ((bytes1[bytes1.size() - 1] & 0x80) == 0);
    bool b2Pos = ((bytes2[bytes2.size() - 1] & 0x80) == 0);

    if (b1Pos != b2Pos)
    {
        if (b1Pos)
        {
            return bytes1;
        }
        if (b2Pos)
        {
            return bytes2;
        }
    }

    auto sum = Add(bytes1, Negate(Copy(bytes2)));

    if ((sum[sum.size() - 1] & 0x80) != 0)
    {
        return bytes2;
    }

    return bytes1;
}

byte_array MyBigIntImp::Not(const byte_array& bytes) {
    byte_array bnew;

    for (auto& b : bytes)
        bnew.push_back(static_cast<byte>((b ^ 0xFF)));

    return bnew;
}

byte_array MyBigIntImp::Or(byte_array bytes1, byte_array bytes2) {
    byte_array bnew;
    NormalizeLengths(bytes1, bytes2);

    for (int i = 0; i < bytes1.size(); i++)
    {
        bnew.push_back(static_cast<byte>((bytes1[i] | bytes2[i])));
    }

    return bnew;
}

byte_array MyBigIntImp::And(byte_array bytes1, byte_array bytes2) {
    byte_array bnew;
    NormalizeLengths(bytes1, bytes2);

    for (int i = 0; i < bytes1.size(); i++)
    {
        bnew.push_back(static_cast<byte>((bytes1[i] & bytes2[i])));
    }

    return bnew;
}

byte_array MyBigIntImp::GCD(byte_array bytes1, byte_array bytes2) {
    byte_array temp;

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

    while (!IsZero(bytes2))
    {
        temp = Copy(bytes2);
        bytes2 = Remainder(bytes1, bytes2);
        bytes1 = temp;
    }
    return bytes1;
}

byte_array MyBigIntImp::Pow(byte_array bytes1, byte_array bytes2) {
    if (IsZero(bytes2))
    {
        return byte_array { 1 };
    }

    BitArray ba2(bytes2);
    int last1 = 0;
    byte_array result;

    for (int i = ba2.size() - 1; i >= 0; i--)
    {
        if (ba2[i])
        {
            last1 = i;
            break;
        }
    }

    for (int i = 0; i <= last1; i++)
    {
        if (ba2[i])
        {
            if (result.empty())
            {
                result = bytes1;
            }
            else
            {
                result = Multiply(result, bytes1);
            }
            Trim(bytes1);
            Trim(result);
        }
        if (i != last1)
        {
            bytes1 = Multiply(bytes1, bytes1);
            Trim(bytes1);
        }
    }
    return (result.empty()) ? byte_array { 1 } : result;
}

byte_array MyBigIntImp::Xor(byte_array bytes1, byte_array bytes2) {
    byte_array bnew;
    NormalizeLengths(bytes1, bytes2);

    for (int i = 0; i < bytes1.size(); i++)
    {
        bnew.push_back(static_cast<byte>((bytes1[i] ^ bytes2[i])));
    }
    return bnew;
}

byte_array MyBigIntImp::Multiply(byte_array bytes1, byte_array bytes2) {
    NormalizeLengths(bytes1, bytes2);
    byte_array bresult;

    for (int i = 0; i < bytes1.size(); i++)
    {
        bresult.push_back(static_cast<byte>(0x00));
        bresult.push_back(static_cast<byte>(0x00));
    }

    NormalizeLengths(bytes2, bresult);
    NormalizeLengths(bytes1, bresult);
    BitArray ba2(bytes2);
    for (int i = ba2.size() - 1; i >= 0; i--)
    {
        if (ba2[i])
        {
            bresult = Add(bytes1, bresult);
        }

        if (i != 0)
        {
            bresult = ShiftLeftDrop(bresult);
        }
    }
    bresult = SetLength(bresult, bytes2.size());

    return bresult;
}

byte_array MyBigIntImp::ShiftLeftDrop(const byte_array& bytes) {
    byte_array bresult;

    for (int i = 0; i < bytes.size(); i++)
    {
        byte newbyte = bytes[i];

        if (newbyte > 127)
        {
            newbyte -= 128;
        }
        newbyte = static_cast<byte>((newbyte * 2));
        if ((i != 0) && (bytes[i - 1] >= 128))
        {
            newbyte++;
        }

        bresult.push_back(newbyte);
    }

    return bresult;
}

byte_array MyBigIntImp::SetLength(const byte_array& bytes, int size) {
    byte_array bresult;

    for (int i = 0; i < size; i++)
    {
        bresult.push_back(bytes[i]);
    }

    return bresult;
}

byte_array MyBigIntImp::ModPow(byte_array bytes1, byte_array bytes2, byte_array bytes3) {
    if (IsZero(bytes2))
    {
        return Remainder(byte_array{ 1 }, bytes3);
    }

    BitArray ba2(bytes2);
    int last1 = 0;
    byte_array result;

    for (int i = ba2.size() - 1; i >= 0; i--)
    {
        if (ba2[i])
        {
            last1 = i;
            break;
        }
    }

    bytes1 = Remainder(bytes1, Copy(bytes3));
    for (int i = 0; i <= last1; i++)
    {
        if (ba2[i])
        {
            if (result.empty())
            {
                result = bytes1;
            }
            else
            {
                result = Multiply(result, bytes1);
                result = Remainder(result, Copy(bytes3));
            }
            Trim(bytes1);
            Trim(result);
        }
        if (i != last1)
        {
            bytes1 = Multiply(bytes1, bytes1);
            bytes1 = Remainder(bytes1, Copy(bytes3));
            Trim(bytes1);
        }
    }
    return (result.empty()) ? Remainder(byte_array { 1 }, bytes3) : result;
}
