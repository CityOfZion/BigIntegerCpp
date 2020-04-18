#include "stack_calc.h"

StackCalc::StackCalc(const std::string& _input) {
    std::istringstream iss (_input);
    input = string_array( std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>() );

    for (auto& elem : input)
    {
        operators.push(elem);
    }
}

bool StackCalc::DoNextOperation() {

    std::string op;
    bool ret = false;
    bool checkValues = false;

    BigInteger snnum1 = 0;
    BigInteger snnum2 = 0;
    BigInteger snnum3 = 0;
    BigInteger mynum1 = 0;
    BigInteger mynum2 = 0;
    BigInteger mynum3 = 0;

    if (operators.size() == 0)
    {
        return false;
    }
    op = operators.front();
    operators.pop();

    if (op.rfind("u") == 0)
    {
        checkValues = true;

        snnum1 = snCalc.top();
        snCalc.pop();
        snCalc.push(DoUnaryOperatorSN(snnum1, op));

        mynum1 = myCalc.top();
        myCalc.pop();
        myCalc.push(MyBigIntImp::DoUnaryOperatorMine(mynum1, op));

        ret = true;
    }
    else if (op.rfind("b") == 0)
    {
        checkValues = true;
        snnum1 = snCalc.top();
        snCalc.pop();
        snnum2 = snCalc.top();
        snCalc.pop();
        snCalc.push(DoBinaryOperatorSN(snnum1, snnum2, op, _snOut));

        mynum1 = myCalc.top();
        myCalc.pop();
        mynum2 = myCalc.top();
        myCalc.pop();
        myCalc.push(MyBigIntImp::DoBinaryOperatorMine(mynum1, mynum2, op, &_myOut));

        ret = true;
    }
    else if (op.rfind("t") == 0)
    {
        checkValues = true;
        snnum1 = snCalc.top();
        snCalc.pop();
        snnum2 = snCalc.top();
        snCalc.pop();
        snnum3 = snCalc.top();
        snCalc.pop();
        snCalc.push(DoTertanaryOperatorSN(snnum1, snnum2, snnum3, op));

        mynum1 = myCalc.top();
        myCalc.pop();
        mynum2 = myCalc.top();
        myCalc.pop();
        mynum3 = myCalc.top();
        myCalc.pop();
        myCalc.push(MyBigIntImp::DoTertanaryOperatorMine(mynum1, mynum2, mynum3, op));

        ret = true;
    }
    else
    {
        if (op.compare("make"))
        {
            snnum1 = DoConstruction();
            snCalc.push(snnum1);
            myCalc.push(snnum1);
        }
        else if (op.compare("Corruption"))
        {
            snCalc.push(-33);
            myCalc.push(-555);
        }
        else if (BigInteger::TryParse(op, snnum1))
        {
            snCalc.push(snnum1);
            myCalc.push(snnum1);
        }
        else
        {
            std::cout << "Failed to parse string " <<  op << std::endl;
        }

        ret = true;
    }

    if (checkValues)
    {
        if ((snnum1 != mynum1) || (snnum2 != mynum2) || (snnum3 != mynum3))
        {
            operators.push("Corruption");
        }
    }

    return ret;
}

BigInteger StackCalc::DoConstruction() {
    std::vector<unsigned char> bytes;
    BigInteger ret = BigInteger(0);
    std::string op = operators.front();
    operators.pop();

    //todo check, not sure if that works
    while (op.compare("endmake") != 0)
    {
        unsigned char chr = (unsigned char)op[0];
        bytes.emplace_back(chr);
        op = operators.front();
        operators.pop();
    }

    return BigInteger(bytes);
}
BigInteger StackCalc::DoBinaryOperatorSN(BigInteger num1, BigInteger num2, std::string op)
{
    BigInteger num3;

    return DoBinaryOperatorSN(num1, num2, op, num3);
}

BigInteger StackCalc::DoBinaryOperatorSN(BigInteger num1, BigInteger num2, std::string op, BigInteger& num3)
{
    num3 = BigInteger::Zero();
        if (op == "bMin") {
            return std::min(num1, num2);
        } else if (op == "bMax") {
            return std::max(num1, num2);
        } else if (op == "b>>") {
            return num1 >> (int)num2;
        } else if (op == "b<<") {
            return num1 << (int)num2;
        } else if (op == "b^") {
            return num1 ^ num2;
        } else if (op == "b|") {
            return num1 | num2;
        } else if (op == "b&") {
            return num1 & num2;
        } else if (op == "b%") {
            return num1 % num2;
        } else if (op == "b/") {
            return num1 / num2;
        } else if (op == "b*") {
            return num1 * num2;
        } else if (op == "b-") {
            return num1 - num2;
        } else if (op == "b+") {
            return num1 + num2;
        } else if (op == "bLog") {
            return MyBigIntImp::ApproximateBigInteger(BigInteger::Log(num1, (double)num2));
        } else if (op == "bGCD") {
            return BigInteger::GreatestCommonDivisor(num1, num2);
        } else if (op == "bPow") {
            int arg2 = (int)num2;
            return BigInteger::Pow(num1, arg2);
        } else if (op == "bDivRem") {
            return BigInteger::DivRem(num1, num2, num3);
        } else if (op == "bRemainder") {
            return BigInteger::Remainder(num1, num2);
        } else if (op == "bDivide") {
            return BigInteger::Divide(num1, num2);
        } else if (op == "bMultiply") {
            return BigInteger::Multiply(num1, num2);
        } else if (op == "bSubtract") {
            return BigInteger::Subtract(num1, num2);
        } else if (op == "bAdd") {
            return BigInteger::Add(num1, num2);
        } else {
            throw std::exception();//(string.Format("Invalid operation found: {0}", op));
    }
}

BigInteger StackCalc::DoUnaryOperatorSN(BigInteger num1, std::string op) {
    //            switch (op)
//            {
//                } else if (op == "uSign") {
//                    return BigInteger(num1.Sign());
//                } else if (op == "u~") {
//                    return (~(num1));
//                } else if (op == "uLog10") {
//                    return MyBigIntImp::ApproximateBigInteger(BigInteger::Log10(num1));
//                } else if (op == "uLog") {
//                    return MyBigIntImp::ApproximateBigInteger(BigInteger::Log(num1));
//                } else if (op == "uAbs") {
//                    return BigInteger::Abs(num1);
//                } else if (op == "uNegate") {
//                    return BigInteger::Negate(num1);
//                } else if (op == "u--") {
//                    return (--(num1));
//                } else if (op == "u++") {
//                    return (++(num1));
//                } else if (op == "u-") {
//                    return (-(num1));
//                } else if (op == "u+") {
//                    return (+(num1));
//                } else if (op == "uMultiply") {
//                    return BigInteger::Multiply(num1, num1);
//                } else if (op == "u*") {
//                    return num1 * num1;
//                default:
    throw std::exception(); //string.Format("Invalid operation found: {0}", op));
//            }

    return BigInteger();
}

BigInteger StackCalc::DoTertanaryOperatorSN(BigInteger num1, BigInteger num2, BigInteger num3, std::string op) {
    if (op == "tModPow") {
        return BigInteger::ModPow(num1, num2, num3);
    } else {
        throw std::exception(); // invalid operation found
    }
}


