#include <iostream>
#include "../include/public/bigintegercpp/BigInteger.h"
#include "gtest/gtest.h"

// Tests for simple arithmetic
TEST(simple_arithmetic, simple_add)
{
    BigInteger a = 1;
    BigInteger b = 2;
    BigInteger c;
    BigInteger result = 3;

    c = a + b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_sub)
{
    BigInteger a = 2;
    BigInteger b = 1;
    BigInteger c;
    BigInteger result = 1;

    c = a - b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mul)
{
    BigInteger a = 1000;
    BigInteger b = 600;
    BigInteger c;
    BigInteger result = 600000;

    c = a * b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mul_big)
{
    BigInteger a = static_cast<int64_t>(9899809879);
    BigInteger b = static_cast<int64_t>(9878980980);
    BigInteger c;
    BigInteger result = BigInteger::parse("97800033500257101420");

    c = a * b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mul_big_2)
{
    BigInteger a = static_cast<int64_t>(-9899809879);
    BigInteger b = static_cast<int64_t>(-9878980980);
    BigInteger c;
    BigInteger result = BigInteger::parse("97800033500257101420");

    c = a * b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mul_big_3)
{
    BigInteger a = static_cast<int64_t>(9899809879);
    BigInteger b = static_cast<int64_t>(-9878980980);
    BigInteger c;
    BigInteger result = BigInteger::parse("-97800033500257101420");

    c = a * b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mul_very_big)
{
    BigInteger a = BigInteger::parse(
            "1238738247813274897348578923475824379878934275982347985798425798234798579823475934298090342758907342895798342789");
    BigInteger b = BigInteger::parse(
            "923954789437582348237487248329848972342317846327478123487123847231848123489712389489237489172848127474750824858932");
    BigInteger c;
    BigInteger result = BigInteger::parse(
            "1144538136926594110508426131715315812591088805139030677194973063150474266174687878642596081926117042264662236011612211694612718410777133741810033035776122924787218639520327198437943918235712633298145207807217428086070304441348");

    c = a * b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mul_very_big_2)
{
    BigInteger a = BigInteger::parse(
            "-1238738247813274897348578923475824379878934275982347985798425798234798579823475934298090342758907342895798342789");
    BigInteger b = BigInteger::parse(
            "-923954789437582348237487248329848972342317846327478123487123847231848123489712389489237489172848127474750824858932");
    BigInteger c;
    BigInteger result = BigInteger::parse(
            "1144538136926594110508426131715315812591088805139030677194973063150474266174687878642596081926117042264662236011612211694612718410777133741810033035776122924787218639520327198437943918235712633298145207807217428086070304441348");

    c = a * b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mul_very_big_3)
{
    BigInteger a = BigInteger::parse(
            "-1238738247813274897348578923475824379878934275982347985798425798234798579823475934298090342758907342895798342789");
    BigInteger b = BigInteger::parse(
            "923954789437582348237487248329848972342317846327478123487123847231848123489712389489237489172848127474750824858932");
    BigInteger c;
    BigInteger result = BigInteger::parse(
            "-1144538136926594110508426131715315812591088805139030677194973063150474266174687878642596081926117042264662236011612211694612718410777133741810033035776122924787218639520327198437943918235712633298145207807217428086070304441348");

    c = a * b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_div)
{
    BigInteger a = 100;
    BigInteger b = 20;
    BigInteger c;
    BigInteger result = 5;

    c = a / b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_div_big)
{
    BigInteger a = static_cast<int64_t>(892374832784);
    BigInteger b = static_cast<int64_t>(3294723897);
    BigInteger c;
    BigInteger result = 270;

    c = a / b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_div_big_2)
{
    BigInteger a = static_cast<int64_t>(-892374832784);
    BigInteger b = static_cast<int64_t>(-3294723897);
    BigInteger c;
    BigInteger result = 270;

    c = a / b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_div_big_3)
{
    BigInteger a = static_cast<int64_t>(-892374832784);
    BigInteger b = static_cast<int64_t>(3294723897);
    BigInteger c;
    BigInteger result = -270;

    c = a / b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_div_very_big)
{
    BigInteger a = BigInteger::parse(
            "298374983274893789478932789479823798472138947823749871293847982317498712389479821374981723894789123784972193874982137894712897498123794871239847891238748912374923178941728934798127489123749231794872398479182734981273498123749127398471928347921834798127948712384718902380912734801237408912734071238407120983480912740891274801723897120398709812374");
    BigInteger b = BigInteger::parse(
            "238478321748973874082138237480273184723847802317082317481802476574657635453233243456788882472634234546246323874");
    BigInteger c;
    BigInteger result = BigInteger::parse(
            "1251161871178245320331256510682369180791525158725012933980595128404921766946080311082133420627430548511036178147087468260614630992362103288043972158192931498347387943570323657491356642757242544525626017851577351727852757690485109061016");

    c = a / b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_div_very_big_2)
{
    BigInteger a = BigInteger::parse(
            "-298374983274893789478932789479823798472138947823749871293847982317498712389479821374981723894789123784972193874982137894712897498123794871239847891238748912374923178941728934798127489123749231794872398479182734981273498123749127398471928347921834798127948712384718902380912734801237408912734071238407120983480912740891274801723897120398709812374");
    BigInteger b = BigInteger::parse(
            "238478321748973874082138237480273184723847802317082317481802476574657635453233243456788882472634234546246323874");
    BigInteger c;
    BigInteger result = BigInteger::parse(
            "-1251161871178245320331256510682369180791525158725012933980595128404921766946080311082133420627430548511036178147087468260614630992362103288043972158192931498347387943570323657491356642757242544525626017851577351727852757690485109061016");

    c = a / b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_div_very_big_3)
{
    BigInteger a = BigInteger::parse(
            "-298374983274893789478932789479823798472138947823749871293847982317498712389479821374981723894789123784972193874982137894712897498123794871239847891238748912374923178941728934798127489123749231794872398479182734981273498123749127398471928347921834798127948712384718902380912734801237408912734071238407120983480912740891274801723897120398709812374");
    BigInteger b = BigInteger::parse(
            "-238478321748973874082138237480273184723847802317082317481802476574657635453233243456788882472634234546246323874");
    BigInteger c;
    BigInteger result = BigInteger::parse(
            "1251161871178245320331256510682369180791525158725012933980595128404921766946080311082133420627430548511036178147087468260614630992362103288043972158192931498347387943570323657491356642757242544525626017851577351727852757690485109061016");

    c = a / b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_div_very_big_alt)
{
    BigInteger a = BigInteger::parse(
            "298374983274893789478932789479823798472138947823749871293847982317498712389479821374981723894789123784972193874982137894712897498123794871239847891238748912374923178941728934798127489123749231794872398479182734981273498123749127398471928347921834798127948712384718902380912734801237408912734071238407120983480912740891274801723897120398709812374");
    BigInteger b = BigInteger::parse(
            "238478321748973874082138237480273184723847802317082317481802476574657635453233243456788882472634234546246323874");
    BigInteger c;
    BigInteger result = 0;

    c = b / a;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mod)
{
    BigInteger a = 10;
    BigInteger b = 7;
    BigInteger c;
    BigInteger result = 3;

    c = a % b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mod_big)
{
    BigInteger a = static_cast<int64_t>(2834789327894723894);
    BigInteger b = 7123;
    BigInteger c;
    BigInteger result = 64;

    c = a % b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mod_big_2)
{
    BigInteger a = static_cast<int64_t>(2834789327894723894);
    BigInteger b = 7123;
    BigInteger c;
    BigInteger result = 7123;

    c = b % a;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mod_big_3)
{
    BigInteger a = static_cast<int64_t>(-2834789327894723894);
    BigInteger b = 7123;
    BigInteger c;
    BigInteger result = -64;

    c = a % b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mod_big_4)
{
    BigInteger a = static_cast<int64_t>(-2834789327894723894);
    BigInteger b = static_cast<int64_t>(-2834789327894723894);
    BigInteger c;
    BigInteger result = 0;

    c = a % b;

    ASSERT_EQ( c, result);
}

TEST(simple_arithmetic, simple_mod_big_5)
{
    BigInteger a = static_cast<int64_t>(-2834789327894723894);
    BigInteger b = static_cast<int64_t>(2834789327894723894);
    BigInteger c;
    BigInteger result = 0;

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
//    BigInteger a = BigInteger::parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
//    BigInteger b = BigInteger::parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
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
//    a = BigInteger::parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
//    //BigInteger a = BigInteger::parse("12");
//    b = BigInteger::parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
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
//    a = BigInteger::parse("12312312311111111111111111111111111892347982379487983479823789478923890428390236487263461237109248172408213784102894708636237462317423174087124871287489237489712394708912738072180347891111111111111111111111111111111111111111111111");
//    b = 3123123123123123;
//    c = a % b;
//    std::cout << "BigInteger a: " + a.ToString() << std::endl;
//    std::cout << "BigInteger b: " + b.ToString() << std::endl;
//    std::cout << "BigInteger c: " + c.ToString() << std::endl;
//}
