#include <iostream>
#include "../src/BigInteger.h"

int main()
{
    BigInteger a = BigInteger(1);
    BigInteger b = 2;
    BigInteger c;
    c = a + b;
    std::cout << "BigInteger c: " + c.ToString() << std::endl;

    BigInteger str = BigInteger::Parse("123");
    std::cout << "BigInteger str: " + str.ToString() << std::endl;

}
