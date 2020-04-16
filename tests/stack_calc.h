#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <sstream>
#include <iostream>
#include <iterator>
#include <list>

#include "../src/BigInteger.h"
#include "MyBigInt.h"

using string_array = std::vector<std::string>;

class StackCalc
{
    public:
        string_array input;
        std::stack<BigInteger> myCalc;
        std::stack<BigInteger> snCalc;
        std::queue<std::string> operators;

        StackCalc(std::string _input)
        {
            std::istringstream iss (_input);
            input = string_array( std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>() );

            for (auto& elem : input)
            {
                operators.push(elem);
            }
        }

        bool DoNextOperation()
        {
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

            if (op.rfind("u") == 0)
            {
                checkValues = true;

                snnum1 = snCalc.top();
                snCalc.push(DoUnaryOperatorSN(snnum1, op));

                mynum1 = myCalc.top();
                myCalc.push(MyBigIntImp::DoUnaryOperatorMine(mynum1, op));

                ret = true;
            }
            else if (op.rfind("b") == 0)
            {
                checkValues = true;
// TODO
                //
//                snnum1 = snCalc.top();
//                snnum2 = snCalc.top();
//                snCalc.push(DoBinaryOperatorSN(snnum1, snnum2, op, out _snOut));
//
//                mynum1 = myCalc.top();
//                mynum2 = myCalc.top();
//                myCalc.push(MyBigIntImp::DoBinaryOperatorMine(mynum1, mynum2, op, out _myOut));
//
//                ret = true;
            }
            else if (op.rfind("t") == 0)
            {
                checkValues = true;
                snnum1 = snCalc.top();
                snnum2 = snCalc.top();
                snnum3 = snCalc.top();
// TODO
//                snCalc.push(DoTertanaryOperatorSN(snnum1, snnum2, snnum3, op));

                mynum1 = myCalc.top();
                mynum2 = myCalc.top();
                mynum3 = myCalc.top();
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
                //TODO
//                else if (BigInteger::TryParse(op, out snnum1))
//                {
//                    snCalc.push(snnum1);
//                    myCalc.push(snnum1);
//                }
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

    private:
        BigInteger snOut = 0;
        BigInteger myOut = 0;

        BigInteger DoConstruction()
        {
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

        BigInteger DoUnaryOperatorSN(BigInteger num1, std::string op)
        {
//            switch (op)
//            {
//                case "uSign":
//                    return BigInteger(num1.Sign());
//                case "u~":
//                    return (~(num1));
//                case "uLog10":
//                    return MyBigIntImp::ApproximateBigInteger(BigInteger::Log10(num1));
//                case "uLog":
//                    return MyBigIntImp::ApproximateBigInteger(BigInteger::Log(num1));
//                case "uAbs":
//                    return BigInteger::Abs(num1);
//                case "uNegate":
//                    return BigInteger::Negate(num1);
//                case "u--":
//                    return (--(num1));
//                case "u++":
//                    return (++(num1));
//                case "u-":
//                    return (-(num1));
//                case "u+":
//                    return (+(num1));
//                case "uMultiply":
//                    return BigInteger::Multiply(num1, num1);
//                case "u*":
//                    return num1 * num1;
//                default:
                    throw new std::exception(); //string.Format("Invalid operation found: {0}", op));
//            }
        }
};