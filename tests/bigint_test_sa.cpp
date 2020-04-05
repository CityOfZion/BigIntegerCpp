#include <iostream>
#include "../src/BigInteger.h"
#include "gtest/gtest.h"

// Tests for simple arithmetic
TEST(add, simple_add)
{
    BigInteger a = 1;
    BigInteger b = 2;
    BigInteger c;
    BigInteger result = 3;

    c = a + b;

    ASSERT_EQ( c, result);
}

TEST(add, simple_sub)
{
    BigInteger a = 2;
    BigInteger b = 1;
    BigInteger c;
    BigInteger result = 1;

    c = a - b;

    ASSERT_EQ( c, result);
}

TEST(add, simple_mul)
{
    BigInteger a = 1000;
    BigInteger b = 600;
    BigInteger c;
    BigInteger result = 600000;

    c = a * b;

    ASSERT_EQ( c, result);
}

TEST(add, simple_div)
{
    BigInteger a = 100;
    BigInteger b = 20;
    BigInteger c;
    BigInteger result = 5;

    c = a / b;

    ASSERT_EQ( c, result);
}

TEST(add, simple_mod)
{
    BigInteger a = 10;
    BigInteger b = 7;
    BigInteger c;
    BigInteger result = 3;

    c = a % b;

    ASSERT_EQ( c, result);
}

//int main()
//{
//    //BigInteger a = BigInteger(1);
//    //BigInteger b = 2;
//    //BigInteger c;
//    //c = a + b;
//    //std::cout << "BigInteger c: " + c.ToString() << std::endl;
//    //BigInteger x = 22345678903874887;
//    //BigInteger y = 22345678903874887;
//
//    //std::cout << (x+y).ToString() << std::endl;
//    BigInteger a = BigInteger::Parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
//    BigInteger b = BigInteger::Parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
//    BigInteger c = a - b;
//    std::cout << "1 BigInteger c: " + c.ToString() << std::endl;
//    a = 2;
//    b = 1;
//    c = b - a;
//    std::cout << "2 BigInteger c: " + c.ToString() << std::endl;
//
//    a = 2;
//    b = 100;
//    c = b * a;
//    std::cout << "3 BigInteger c: " + c.ToString() << std::endl;
//
//    a = BigInteger::Parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
//    //BigInteger a = BigInteger::Parse("12");
//    b = BigInteger::Parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
//    std::cout << "BigInteger a: " + a.ToString() << std::endl;
//    std::cout << "BigInteger b: " + b.ToString() << std::endl;
//    c = b * a;
//    std::cout << "4 BigInteger c: " + c.ToString() << std::endl;
//    // 151593034446338230123456790123456809361121486146582485044605332829447333509877004402868765412357088523939654860927276555509629039049010997338708004428022128138390456337638175745687017182369547359633954112658468586386231463216489165475402187249743744197488216400084295682824798754817741266463856976488446732504114675063141136203037642617899990661705406601576722400853170762374281792515125742924632587292774083379480987654320987654320987654320987654320987654321
//    BigInteger d = c / a;
//    std::cout << "BigInteger d: " + d.ToString() << std::endl;
//    a = 30;
//    b = 5;
//    c = b / a;
//    std::cout << "BigInteger a: " + a.ToString() << std::endl;
//    std::cout << "BigInteger b: " + b.ToString() << std::endl;
//    std::cout << "BigInteger c: " + c.ToString() << std::endl;
//    c = a / b;
//    std::cout << "BigInteger a: " + a.ToString() << std::endl;
//    std::cout << "BigInteger b: " + b.ToString() << std::endl;
//    std::cout << "BigInteger c: " + c.ToString() << std::endl;
//    c = a * b;
//    std::cout << "BigInteger a: " + a.ToString() << std::endl;
//    std::cout << "BigInteger b: " + b.ToString() << std::endl;
//    std::cout << "BigInteger c: " + c.ToString() << std::endl;
//    BigInteger x = 2;
//    BigInteger y = 3;
//    BigInteger z = x*y;
//    std::cout << "BigInteger x: " + x.ToString() << std::endl;
//    std::cout << "BigInteger y: " + y.ToString() << std::endl;
//    std::cout << "BigInteger z: " + z.ToString() << std::endl;
//    a = 10;
//    b = 5;
//    c = a % b;
//    std::cout << "BigInteger a: " + a.ToString() << std::endl;
//    std::cout << "BigInteger b: " + b.ToString() << std::endl;
//    std::cout << "BigInteger c: " + c.ToString() << std::endl;
//    a = 10;
//    b = 5;
//    c = b % a;
//    std::cout << "BigInteger a: " + a.ToString() << std::endl;
//    std::cout << "BigInteger b: " + b.ToString() << std::endl;
//    std::cout << "BigInteger c: " + c.ToString() << std::endl;
//    a = 10;
//    b = 7;
//    c = a % b;
//    std::cout << "BigInteger a: " + a.ToString() << std::endl;
//    std::cout << "BigInteger b: " + b.ToString() << std::endl;
//    std::cout << "BigInteger c: " + c.ToString() << std::endl;
//    a = BigInteger::Parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
//    b = 3123123123123123;
//    c = a % b;
//    std::cout << "BigInteger a: " + a.ToString() << std::endl;
//    std::cout << "BigInteger b: " + b.ToString() << std::endl;
//    std::cout << "BigInteger c: " + c.ToString() << std::endl;
//}
