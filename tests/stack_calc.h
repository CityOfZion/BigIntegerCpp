#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <sstream>
#include <iostream>
#include <iterator>
#include <list>

#include "../include/public/bigintegercpp/BigInteger.h"
#include "MyBigInt.h"

using string_array = std::vector<std::string>;

class StackCalc
{
public:
    string_array input;
    std::stack<BigInteger> myCalc;
    std::stack<BigInteger> snCalc;
    std::queue<std::string> operators;

    StackCalc(const std::string& _input);
    bool DoNextOperation();
    void VerifyOutParameter();

private:
    BigInteger _snOut{0};
    BigInteger _myOut{0};

    BigInteger DoConstruction();
    BigInteger DoUnaryOperatorSN(BigInteger num1, std::string op);
    BigInteger DoBinaryOperatorSN(BigInteger num1, BigInteger num2, std::string op, BigInteger& num3);
    BigInteger DoBinaryOperatorSN(BigInteger num1, BigInteger num2, std::string op);
    BigInteger DoTertanaryOperatorSN(BigInteger num1, BigInteger num2, BigInteger num3, std::string op);
};