import unittest
from collections import abc
from pybiginteger import BigInteger


class BigIntegerTestCase(unittest.TestCase):
    def test_from_bytearray(self):
        b = BigInteger(b'\xc8\x01')
        self.assertEqual(456, b)

        b2 = BigInteger(b'8\xfe')
        self.assertEqual(-456, b2)

    def test_from_int(self):
        uint32_min = 0
        uint32_max = 4294967295
        int32_min = -2147483648

        b1 = BigInteger(uint32_min)
        self.assertEqual(uint32_min, b1)

        b2 = BigInteger(uint32_max)
        self.assertEqual(uint32_max, b2)

        b3 = BigInteger(int32_min)
        self.assertEqual(int32_min, b3)

        b4 = BigInteger(uint32_max + 1)
        self.assertEqual(uint32_max + 1, b4)

        uint64_max = 0xFFFFFFFFFFFFFFFF
        b5 = BigInteger(uint64_max)
        self.assertEqual(uint64_max, b5)

    def test_bytes_and_to_array(self):
        b = bytes(BigInteger(456))
        self.assertEqual(b'\xc8\x01', b)

        b = BigInteger(33022)
        bb = b.to_array()
        self.assertEqual(b'\xFE\x80\x00', bb)
        bb = b.to_array(is_unsigned=False, is_bigendian=False)
        self.assertEqual(b'\xFE\x80\x00', bb)

        bb = b.to_array(is_unsigned=False, is_bigendian=True)
        self.assertEqual(b'\x00\x80\xFE', bb)

        bb = b.to_array(is_unsigned=True, is_bigendian=False)
        self.assertEqual(b'\xFE\x80', bb)

        bb = b.to_array(is_unsigned=True, is_bigendian=True)
        self.assertEqual(b'\x80\xFE', bb)

        b = BigInteger(0x78)
        bb = b.to_array(is_unsigned=False, is_bigendian=False)
        self.assertEqual(b'\x78', bb)

        b = BigInteger(0x80)
        bb = b.to_array(is_unsigned=False, is_bigendian=False)
        self.assertEqual(b'\x80\x00', bb)

    def test_add(self):
        b = BigInteger(1)
        i = 12937123987123987123987123987123987123987123129387129387123987123987
        bi = BigInteger(12937123987123987123987123987123987123987123129387129387123987123987)
        # test __add__ overloads with value > long long
        r = b + i
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(12937123987123987123987123987123987123987123129387129387123987123988, r)

        r = b + bi
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(12937123987123987123987123987123987123987123129387129387123987123988, b + bi)

        b += i
        self.assertIsInstance(b, BigInteger)
        self.assertEqual(12937123987123987123987123987123987123987123129387129387123987123988, b)

        b = BigInteger(1)
        bi += b
        self.assertIsInstance(bi, BigInteger)
        self.assertEqual(12937123987123987123987123987123987123987123129387129387123987123988, bi)

        # test __radd__
        b = BigInteger(1)
        r = i + b
        self.assertIsInstance(r, int)
        self.assertEqual(12937123987123987123987123987123987123987123129387129387123987123988, r)

        i += b
        self.assertIsInstance(i, int)
        self.assertEqual(12937123987123987123987123987123987123987123129387129387123987123988, i)

    def test_subtract(self):
        b = BigInteger(1)
        i = 12937123987123987123987123987123987123987123129387129387123987123987
        bi = BigInteger(12937123987123987123987123987123987123987123129387129387123987123987)
        # test __sub__ overload with value > long long
        r = b - i
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(-12937123987123987123987123987123987123987123129387129387123987123986, r)

        r = b - bi
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(-12937123987123987123987123987123987123987123129387129387123987123986, r)

        b -= i
        self.assertIsInstance(b, BigInteger)
        self.assertEqual(-12937123987123987123987123987123987123987123129387129387123987123986, b)

        # test __rsub__
        b = BigInteger(1)
        r = i - b
        self.assertIsInstance(r, int)
        self.assertEqual(12937123987123987123987123987123987123987123129387129387123987123986, r)

        i -= b
        self.assertIsInstance(i, int)
        self.assertEqual(12937123987123987123987123987123987123987123129387129387123987123986, i)

    def test_multiply(self):
        b = BigInteger(2)
        i = 12937123987123987123987123987123987123987123129387129387123987123987
        bi = BigInteger(12937123987123987123987123987123987123987123129387129387123987123987)

        # test __mul__ with value > long long
        r = b * i  # big * int
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(25874247974247974247974247974247974247974246258774258774247974247974, r)

        r = b * bi  # big * big
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(25874247974247974247974247974247974247974246258774258774247974247974, r)

        bi *= 2  # big * int
        self.assertIsInstance(b, BigInteger)
        self.assertEqual(25874247974247974247974247974247974247974246258774258774247974247974, bi)

        # test __rmul__
        b = BigInteger(2)
        r = i * b
        self.assertIsInstance(r, int)
        self.assertEqual(25874247974247974247974247974247974247974246258774258774247974247974, r)

        i *= b
        self.assertIsInstance(i, int)
        self.assertEqual(25874247974247974247974247974247974247974246258774258774247974247974, i)

    def test_modulo(self):
        b1 = BigInteger(860593)
        b2 = BigInteger(-201)
        # test all __mod__/__rmod__ overloads
        r = b1 % b2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(112, r)
        r = b1 % -201
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(112, b1 % -201)
        r = 860593 % b2
        self.assertIsInstance(r, int)
        self.assertEqual(112, r)

        b1 = BigInteger(20195283520469175757)
        b2 = BigInteger(1048576)
        self.assertEqual(888269, b1 % b2)

        b1 = BigInteger(-18224909727634776050312394179610579601844989529623334093909233530432892596607)
        b2 = BigInteger(14954691977398614017)
        self.assertEqual(-3100049211437790421, b1 % b2)

    def test_comparison(self):
        b1 = BigInteger(2)
        self.assertTrue(b1 < 3)
        self.assertTrue(1 < b1)

    def test_divmod(self):
        b1 = BigInteger(9)
        b2 = BigInteger(4)
        # bigint + bigint
        quotient, remainder = divmod(b1, b2)
        self.assertIsInstance(quotient, BigInteger)
        self.assertIsInstance(remainder, BigInteger)
        self.assertEqual(2, quotient)
        self.assertEqual(1, remainder)

        # bigint + py int
        quotient, remainder = divmod(b1, 4)
        self.assertIsInstance(quotient, BigInteger)
        self.assertIsInstance(remainder, BigInteger)
        self.assertEqual(2, quotient)
        self.assertEqual(1, remainder)

        # pyint + bigint
        quotient, remainder = divmod(9, b2)
        self.assertIsInstance(quotient, int)
        self.assertIsInstance(remainder, int)
        self.assertEqual(2, quotient)
        self.assertEqual(1, remainder)

    def test_hash(self):
        """
        BigInteger should be hashable such that it can be used in dictionaries etc.

        Because we override __eq__ the __hash__ should return the same value for
        objects that are equal.
        """

        b1 = BigInteger(5)
        b2 = BigInteger(5)
        b3 = BigInteger(4)

        self.assertIsInstance(b1, abc.Hashable)
        self.assertEqual(b1, b2)
        self.assertEqual(hash(b1), hash(b2))
        self.assertNotEqual(b1, b3)
        self.assertNotEqual(hash(b1), hash(b3))

    def test_pow(self):
        base_bi = BigInteger(5)
        base_i = 5
        exp_bi = BigInteger(4)
        exp_i = 4

        r = pow(base_bi, exp_bi)
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(625, r)

        r = pow(base_bi, exp_i)
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(625, r)

        r = pow(base_i, exp_bi)
        self.assertIsInstance(r, int)
        self.assertEqual(625, r)

        mod_bi = BigInteger(3)
        mod_i = 3
        r = pow(base_bi, exp_bi, mod_bi)
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(1, r)

        r = pow(base_bi, exp_i, mod_bi)
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(1, r)

        r = pow(base_bi, exp_bi, mod_i)
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(1, r)

        # https://docs.python.org/3.9/reference/datamodel.html?highlight=__rpow__#object.__rpow__
        # "Note that ternary pow() will not try calling __rpow__() (the coercion rules would become too complicated)."
        self.assertRaises(TypeError, lambda f: pow(1 , exp_bi, mod_bi))
        self.assertRaises(TypeError, lambda f: pow(1 , exp_bi, 1))
        self.assertRaises(TypeError, lambda f: pow(1 , 2, mod_bi))

    def test_div(self):
        b = BigInteger(5)
        b2 = BigInteger(2)
        # Note: our BigInteger always exhibits floor division behaviour as per C#
        # test __truediv__
        r = b/b2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(2, r)
        r = b/2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(2, r)
        r = 5/b2
        self.assertEqual(2, r)
        self.assertIsInstance(r, int)
        # test __floordiv__
        r = b//b2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(2, r)
        r = b//2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(2, r)
        r = 5//b2
        self.assertEqual(2, r)
        self.assertIsInstance(r, int)

    def test_lshift(self):
        b = BigInteger(5)
        i = 5
        sb = BigInteger(2)
        si = 2

        r = b << sb # big << big
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(20, r)

        r = b << si # big << int
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(20, r)

        r = i << sb # int << big
        self.assertIsInstance(r, int)
        self.assertEqual(20, r)

        # C#'s BigInteger allows for negative shift values
        base_bi = BigInteger(8)
        base_i = 8
        neg_shift_bi = BigInteger(-1)
        neg_shift_i = BigInteger(-1)

        r = base_bi << neg_shift_bi
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(4, r)

        r = base_bi << neg_shift_i
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(4, r)

        r = base_i << neg_shift_bi
        self.assertIsInstance(r, int)
        self.assertEqual(4, r)

    def test_rshift(self):
        b = BigInteger(12)
        i = 12
        sb = BigInteger(2)
        si = 2

        r = b >> sb # big >> big
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(3, r)

        r = b >> si # big >> int
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(3, r)

        r = i >> sb # int >> big
        self.assertIsInstance(r, int)
        self.assertEqual(3, r)

        # C#'s BigInteger allows for negative shift values
        base_bi = BigInteger(3)
        base_i = 3
        neg_shift_bi = BigInteger(-1)
        neg_shift_i = BigInteger(-1)

        r = base_bi >> neg_shift_bi
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(6, r)

        r = base_bi >> neg_shift_i
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(6, r)

        r = base_i >> neg_shift_bi
        self.assertIsInstance(r, int)
        self.assertEqual(6, r)

    def test_and(self):
        bi = BigInteger(13)
        bi2 = BigInteger(5)
        i = 13
        i2 = 5

        r = bi & bi2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(5, r)

        r = bi & i2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(5, r)

        r = i & bi2
        self.assertIsInstance(r, int)
        self.assertEqual(5, r)

    def test_or(self):
        bi = BigInteger(9)
        bi2 = BigInteger(2)
        i = 9
        i2 = 2

        r = bi | bi2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(11, r)

        r = bi | i2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(11, r)

        r = i | bi2
        self.assertIsInstance(r, int)
        self.assertEqual(11, r)

    def test_xor(self):
        bi = BigInteger(9)
        bi2 = BigInteger(8)
        i = 9
        i2 = 8

        r = bi ^ bi2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(1, r)

        r = bi ^ i2
        self.assertIsInstance(r, BigInteger)
        self.assertEqual(1, r)

        r = i ^ bi2
        self.assertIsInstance(r, int)
        self.assertEqual(1, r)

    def test_unary_arithmetic(self):
        b = BigInteger(9)
        self.assertEqual(-9, -b)
        b = BigInteger(-9)
        self.assertEqual(9, -b)

        # making sure it doesn't blow up for not having implemented __pos__
        b = BigInteger(-9)
        self.assertEqual(-9, +b)

        # test __abs__
        self.assertEqual(9, abs(b))

        # test __invert__
        b = BigInteger(9)
        self.assertEqual(-10, ~b)

    def test_others(self):
        zero = BigInteger.zero()
        self.assertEqual(BigInteger(0), zero)

        one = BigInteger.one()
        self.assertEqual(BigInteger(1), one)

        m_one = BigInteger.minus_one()
        self.assertEqual(BigInteger(-1), m_one)

        self.assertTrue(zero.is_even())
        self.assertFalse(one.is_even())

        self.assertTrue(BigInteger(2).is_power_of_two())
        self.assertFalse(BigInteger(3).is_power_of_two())

        self.assertTrue(hasattr(BigInteger, "__version__"))
        self.assertTrue(hasattr(BigInteger, "__version_bindings__"))

    def test_log(self):
        l1 = BigInteger.log(2)
        l1_big = BigInteger.log(BigInteger(2))
        self.assertEqual(0.6931471805599453, l1)
        self.assertEqual(l1, l1_big)

        l2 = BigInteger.log(2, 10.1)
        l2_big = BigInteger.log(BigInteger(2), 10.1)
        self.assertEqual(0.29973472986061406, l2)
        self.assertEqual(l2, l2_big)

        l3 = BigInteger.log10(2)
        l3_big = BigInteger.log10(BigInteger(2))
        self.assertEqual(0.30102999566398114, l3)
        self.assertEqual(l3, l3_big)
