#include <pybind11/pybind11.h>

#include <pybind11/stl.h>
#include "../src/BigInteger.h"
#include <chrono>

namespace py = pybind11;

BigInteger to_biginteger(py::int_& value) {
    /* The following code converts a py::int to byte array following the example here
     * https://github.com/CityOfZion/neo-python/blob/f343df6c50c5bd39eb0d812c8dc011f717a7df93/neo/Core/BigInteger.py#L20
     * Based on the bug described here https://github.com/CityOfZion/neo-python/pull/963
     *
     * This is because C#'s ToByteArray can add extra bytes. However, the above code only supports
     * signed data and little endian..
     *
     * We should port the full routine from https://github.com/dotnet/runtime/blob/f185bd5571f2500b4843d145418c4418747246b6/src/libraries/System.Runtime.Numerics/src/System/Numerics/BigInteger.cs#L1230-L1411
     * for the py:int_ object to output the same byte array.
     * From this byte array we can then easily construct our BigInteger.
     *
     * This approach is partially implemented and shows a 100x speed increase compared to the current
     * py:int_ to string => biginteger.parse(string) approach.
     */

// TODO: see above. Cleanup and extend
//    if (value.is(py::int_(0))) {
//        return BigInteger::Zero();
//    } else if (value < py::int_(0)) {
//        size_t n_bytes = 1 + std::floor(((_PyLong_NumBits(value.ptr()) + 7) / 8));
//        std::vector<unsigned char> buffer(n_bytes, 0);
//        if (_PyLong_AsByteArray((PyLongObject *)value.ptr(), buffer.data(), n_bytes, 1, 1) < 0) {
//            throw std::invalid_argument("failed to cast");
//        } else {
//            int msb = buffer.size()-1;
//            for (int i = buffer.size()-1; i > 0; i--) {
//                if (buffer[i] == 0xFF) {
//                    msb = i;
//                    break;
//                }
//            }
//            bool needExtraByte = (buffer[msb] & 0x80) != (0xFF & 0x80);
//
//            if (needExtraByte)
//                return BigInteger(buffer);
//            else
//                return BigInteger(std::vector<unsigned char>(buffer.begin(), buffer.begin() + buffer.size() - 1));
//        }
//    } else {
//        size_t n_bits = _PyLong_NumBits(value.ptr());
//        if (n_bits == -1) { // overflow
//            size_t n_bytes = 1 + std::floor(((_PyLong_NumBits(value.ptr()) + 7) / 8));
//            std::vector<unsigned char> buffer(n_bytes, 0);
//            if (_PyLong_AsByteArray((PyLongObject *)value.ptr(), buffer.data(), n_bytes, 1, 1) < 0)
//                throw std::invalid_argument("failed to cast");
//            else
//                return BigInteger(buffer);
//        } else {
//            size_t n_bytes = std::floor(((_PyLong_NumBits(value.ptr()) + 7) / 8));
//            std::vector<unsigned char> buffer(n_bytes, 0);
//            if (_PyLong_AsByteArray((PyLongObject *)value.ptr(), buffer.data(), n_bytes, 1, 1) < 0) {
//                throw std::invalid_argument("failed to cast");
//            } else {
//                return BigInteger(buffer);
//            }
//        }
//    }

    if (value.is(py::int_(0))) {
        return BigInteger::Zero();
    } else if (value < py::int_(0)) {
        try {
            auto i = value.cast<int64_t>();
            return BigInteger(i);
        } catch (const py::cast_error&) {}
    } else {
        try {
            auto i = value.cast<uint64_t>();
            return BigInteger(i);
        } catch (const py::cast_error&) {}
    }

    PyObject* source = value.ptr();
    FILE* fp;
    fp = tmpfile();
    auto res = PyObject_Print(source, fp, Py_PRINT_RAW);
    if (res == -1)
        throw std::invalid_argument("Failed to convert value");
    auto size = ftell(fp);
    rewind(fp);
    std::vector<unsigned char> v_data(size);
    fread(&v_data[0], 1, size, fp);
    fclose(fp);

    return BigInteger::Parse(std::string(v_data.begin(), v_data.end()));
}


PYBIND11_MODULE(pybiginteger, m) {
    m.doc() = "A C++ port of the C# BigInteger class";
    py::class_<BigInteger>(m, "BigInteger")
            .def(py::init<int32_t>())
            .def(py::init<uint32_t>())
            .def(py::init<int64_t>())
            .def(py::init<uint64_t>())
            .def(py::init([](py::int_& i) {
                return to_biginteger(i);
            }))
            .def(py::init([](py::buffer const b) {
                py::buffer_info info = b.request();
                if (info.format != py::format_descriptor<unsigned char>::format() || info.ndim != 1)
                    throw std::runtime_error("Incompatible buffer format!");

                auto data_ptr = static_cast<unsigned char *>(info.ptr);
                std::vector<unsigned char> data(data_ptr, data_ptr + info.size);
                return BigInteger(data);
            }))
            .def("__lt__", [](BigInteger& self, BigInteger& other) {
                return self < other;
            })
            .def("__lt__", [](BigInteger& self, py::int_& other) {
                return self < to_biginteger(other);
            })
            .def("__le__", [](BigInteger& self, BigInteger& other) {
                return self <= other;
            })
            .def("__le__", [](BigInteger& self, py::int_& other) {
                return self <= to_biginteger(other);
            })
            .def("__eq__", [](BigInteger& self, BigInteger& other) {
                return self == other;
            })
            .def("__eq__", [](BigInteger& self, py::int_& other) {
                return self == to_biginteger(other);
            })
            .def("__hash__", [](BigInteger& self) {
                auto h = 2166136261;
                h = (h * 16777619) ^ self.GetHashCode();
                return h;
            })
            .def("__gt__", [](BigInteger& self, BigInteger& other) {
                return self > other;
            })
            .def("__gt__", [](BigInteger& self, py::int_& other) {
                return self > to_biginteger(other);
            })
            .def("__ge__", [](BigInteger& self, BigInteger& other) {
                return self >= other;
            })
            .def("__ge__", [](BigInteger& self, py::int_& other) {
                return self >= to_biginteger(other);
            })
            .def("__add__", [](BigInteger& self, BigInteger& other) {
                return self + other;
            })
            .def("__add__", [](BigInteger& self, py::int_& other) {
                return to_biginteger(other) + self;
            })
            .def("__radd__", [](BigInteger& other, py::int_& self) {
                auto bigi_result = other + to_biginteger(self);
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__sub__", [](BigInteger& self, BigInteger& other) {
                return self - other;
            })
            .def("__sub__", [](BigInteger& self, py::int_& other) {
                return self - to_biginteger(other);
            })
            .def("__rsub__", [](BigInteger& other, py::int_& self) {
                auto bigi_result = to_biginteger(self) - other;
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__mul__", [](BigInteger& self, BigInteger& other) {
                return self * other;
            })
            .def("__mul__", [](BigInteger& self, py::int_& other) {
                return self * to_biginteger(other);
            })
            .def("__rmul__", [](BigInteger& other, py::int_& self) {
                auto bigi_result = to_biginteger(self) * other;
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__mod__", [](BigInteger& self, BigInteger& other) {
                return self % other;
            })
            .def("__mod__", [](BigInteger& self, py::int_& other) {
                return self % to_biginteger(other);
            }, R"(
                Note: this always uses the modulo operation of the BigInteger class.
                C# uses truncated division whereas Python uses floored division.
                For more information see:
                https://en.wikipedia.org/wiki/Modulo_operation)")
            .def("__rmod__", [](BigInteger& other, py::int_& self) {
                auto bigi_result = to_biginteger(self) % other;
                return py::int_(py::str(bigi_result.ToString()));
            }, R"(
                Note: this always uses the modulo operation of the BigInteger class.
                C# uses truncated division whereas Python uses floored division.
                For more information see:
                https://en.wikipedia.org/wiki/Modulo_operation)")
            .def("__divmod__", [](BigInteger& self, BigInteger& other) {
                BigInteger remainder;
                auto div = BigInteger::DivRem(self, other, remainder);
                return py::make_tuple(div, remainder);
            })
            .def("__divmod__", [](BigInteger& self, py::int_& other) {
                BigInteger remainder;
                auto big_other = to_biginteger(other);
                auto div = BigInteger::DivRem(self, big_other, remainder);
                return py::make_tuple(div, remainder);
            })
            .def("__rdivmod__", [](BigInteger& other, py::int_& self) {
                BigInteger remainder;
                auto big_self = to_biginteger(self);
                auto div = BigInteger::DivRem(big_self, other, remainder);
                return py::make_tuple(py::int_(py::str(div.ToString())), py::int_(py::str(remainder.ToString())));
            })
            .def("__pow__", [](BigInteger& base, BigInteger& exp) {
                return BigInteger::Pow(base, exp);
            })
            .def("__pow__", [](BigInteger& base, py::int_& exp) {
                try {
                    auto i = exp.cast<int>();
                    return BigInteger::Pow(base, exp);
                } catch (const py::cast_error&) {
                    throw std::overflow_error("Exponent argument size exceeds C++ type 'int'");
                }
            })
            .def("__rpow__", [](BigInteger& exp, py::int_& base) {
                return py::int_(py::str(BigInteger::Pow(to_biginteger(base), exp).ToString()));
            })
            .def("__pow__", [](BigInteger& base, BigInteger& exp, BigInteger& modulus) {
                return BigInteger::ModPow(base, exp, modulus);
            })
            .def("__pow__", [](BigInteger& base, py::int_& exp, BigInteger& modulus) {
                return BigInteger::ModPow(base, to_biginteger(exp), modulus);
            })
            .def("__pow__", [](BigInteger& base, BigInteger& exp, py::int_& modulus) {
                return BigInteger::ModPow(base, exp, to_biginteger(modulus));
            })
            .def("__truediv__", [](BigInteger& self, BigInteger& other) {
                return self / other;
            })
            .def("__truediv__", [](BigInteger& self, py::int_& other) {
                return self / to_biginteger(other);
            })
            .def("__rtruediv__", [](BigInteger& self, py::int_& other) {
                auto bigi_result = to_biginteger(other) / self;
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__floordiv__", [](BigInteger& self, BigInteger& other) {
                return self / other;
            })
            .def("__floordiv__", [](BigInteger& self, py::int_& other) {
                return self / to_biginteger(other);
            })
            .def("__rfloordiv__", [](BigInteger& self, py::int_& other) {
                auto bigi_result = to_biginteger(other) / self;
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__lshift__", [](BigInteger& self, BigInteger& other) {
                return self << other;
            })
            .def("__lshift__", [](BigInteger& self, py::int_& other) {
                return self << other.cast<int>();
            })
            .def("__rlshift__", [](BigInteger& shift, py::int_& base) {
                auto bigi_result =  to_biginteger(base) << shift;
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__rshift__", [](BigInteger& self, BigInteger& other) {
                return self >> other;
            })
            .def("__rshift__", [](BigInteger& self, py::int_& other) {
                return self >> other.cast<int>();
            })
            .def("__rrshift__", [](BigInteger& shift, py::int_& base) {
                auto bigi_result =  to_biginteger(base) >> shift;
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__and__", [](BigInteger& self, BigInteger& other) {
                return self & other;
            })
            .def("__and__", [](BigInteger& self, py::int_& other) {
                auto other_bi = to_biginteger(other);
                return self & other_bi;
            })
            .def("__rand__", [](BigInteger& self, py::int_& other) {
                auto other_bi = to_biginteger(other);
                auto bigi_result =  self & other_bi;
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__or__", [](BigInteger& self, BigInteger& other) {
                return self | other;
            })
            .def("__or__", [](BigInteger& self, py::int_& other) {
                auto other_bi = to_biginteger(other);
                return self | other_bi;
            })
            .def("__ror__", [](BigInteger& self, py::int_& other) {
                auto other_bi = to_biginteger(other);
                auto bigi_result =  self | other_bi;
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__xor__", [](BigInteger& self, BigInteger& other) {
                return self ^ other;
            })
            .def("__xor__", [](BigInteger& self, py::int_& other) {
                auto other_bi = to_biginteger(other);
                return self ^ other_bi;
            })
            .def("__rxor__", [](BigInteger& self, py::int_& other) {
                auto other_bi = to_biginteger(other);
                auto bigi_result =  self ^ other_bi;
                return py::int_(py::str(bigi_result.ToString()));
            })
            .def("__neg__", [](BigInteger& self) { return -self; })
            .def("__pos__", [](BigInteger& self) { return +self;})
            .def("__abs__", [](BigInteger& self) { return BigInteger::Abs(self);})
            .def("__invert__", [](BigInteger& self) { return ~self; })
            .def("__bytes__", [](BigInteger& self) {
                auto v_bytes = self.ToByteArray();
                return py::bytes(std::string(v_bytes.begin(), v_bytes.end()));
            }, R"(
                Return the value of the BigInteger in little endian order.
                For more information on the byte representation see:
                https://docs.microsoft.com/en-us/dotnet/api/system.numerics.biginteger.tobytearray?view=netcore-3.1)")
            .def("to_bytes",
                 [](BigInteger& self, bool is_unsigned, bool is_bigendian) {
                     auto v_bytes = self.ToByteArray(is_unsigned, is_bigendian);
                     return py::bytes(std::string(v_bytes.begin(), v_bytes.end()));
                 }, R"(
                Return the value of the BigInteger.
                For more information on the options see:
                https://docs.microsoft.com/en-us/dotnet/api/system.numerics.biginteger.tobytearray?view=netcore-3.1#System_Numerics_BigInteger_ToByteArray_System_Boolean_System_Boolean_)",
                 py::arg("is_unsigned") = false,
                 py::arg("is_bigendian") = false)
            .def("__int__", [](BigInteger& self) { return py::int_(py::str(self.ToString())); })   // support int()
            .def("__index__", [](BigInteger& self) { return py::int_(py::str(self.ToString())); }) // support range()
            .def("__str__", &BigInteger::ToString);
}