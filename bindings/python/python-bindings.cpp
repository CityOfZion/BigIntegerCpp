#include <pybind11/pybind11.h>
#include "../../include/public/bigintegercpp/BigInteger.h"
#include "../../include/public/bigintegercpp/version.h"
namespace py = pybind11;

BigInteger to_biginteger(py::int_& value, bool is_signed = true, bool is_bigendian = false) {
    if (value.is(py::int_(0)))
        return BigInteger::zero();

    size_t n_bytes = std::floor(((_PyLong_NumBits(value.ptr()) + 7) / 8));
    if (is_signed)
        n_bytes += 1;

    std::vector<unsigned char> buffer(n_bytes, 0);
    if (_PyLong_AsByteArray(reinterpret_cast<PyLongObject *>(value.ptr()), buffer.data(), n_bytes, !is_bigendian, is_signed) < 0) {
        throw std::invalid_argument("failed to cast");
    } else {
        if (!is_signed)
            return BigInteger(buffer);

        // signed cases in C# use the shortest 2 complement representation possible, Python doesn't, thus we convert
        unsigned char high_byte;
        if (value < py::int_(0))
            high_byte = 0xff;
        else
            high_byte = 0x00;

        unsigned long msb = buffer.size()-1;
        if (is_bigendian) {
            for(auto& b : buffer) {
                if (b != high_byte) {
                    msb = b;
                    break;
                }
            }
        } else {
            for (auto i = buffer.size()-1; i > 0; i--) {
                if (buffer[i] != high_byte) {
                    msb = static_cast<int>(buffer[i]);
                    break;
                }
            }
        }
        auto need_extra_byte = (msb & 0x80) != (high_byte & 0x80);
        if (need_extra_byte)
            return BigInteger(buffer);
        else {
            if (is_bigendian)
                return BigInteger(std::vector<unsigned char>(buffer.begin()+1, buffer.end()));
            else
                return BigInteger(std::vector<unsigned char>(buffer.begin(), buffer.begin() + buffer.size() - 1));
        }
    }
}

py::int_ to_py_int(const BigInteger& value, bool is_signed = true, bool is_bigendian = false) {
    const auto data = value.to_byte_array(is_signed, is_bigendian);

    auto obj = static_cast<PyObject*>(_PyLong_FromByteArray(data.data(),
            data.size(),
            !is_bigendian,
            is_signed));
    return py::reinterpret_steal<py::int_>(obj);
}

static constexpr auto VERSION_BINDINGS = "1.2";

PYBIND11_MODULE(pybiginteger, m) {
    m.doc() = "A C++ port of the C# BigInteger class";
    py::class_<BigInteger>(m, "BigInteger")
            .def_property_readonly_static("__version__", [](py::object self) { return BIGINTEGERCPP_VERSION; }, R"(C++ code version)")
            .def_property_readonly_static("__version_bindings__", [](py::object self){ return VERSION_BINDINGS; }, R"(Binding version)")
            .def(py::init([](py::int_& i) {
              return to_biginteger(i);
            }), py::arg("value"))
            .def(py::init([](py::buffer const b) {
              py::buffer_info info = b.request();
              if (info.format != py::format_descriptor<unsigned char>::format() || info.ndim != 1)
                  throw std::runtime_error("Incompatible buffer format!");

              auto data_ptr = static_cast<unsigned char *>(info.ptr);
              std::vector<unsigned char> data(data_ptr, data_ptr + info.size);
              return BigInteger(data);
            }), py::arg("value"))
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
                h = (h * 16777619) ^ self.get_hash_code();
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
                return to_py_int(bigi_result);
            })
            .def("__sub__", [](BigInteger& self, BigInteger& other) {
                return self - other;
            })
            .def("__sub__", [](BigInteger& self, py::int_& other) {
                return self - to_biginteger(other);
            })
            .def("__rsub__", [](BigInteger& other, py::int_& self) {
                auto bigi_result = to_biginteger(self) - other;
                return to_py_int(bigi_result);
            })
            .def("__mul__", [](BigInteger& self, BigInteger& other) {
                return self * other;
            })
            .def("__mul__", [](BigInteger& self, py::int_& other) {
                return self * to_biginteger(other);
            })
            .def("__rmul__", [](BigInteger& other, py::int_& self) {
                auto bigi_result = to_biginteger(self) * other;
                return to_py_int(bigi_result);
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
                return to_py_int(bigi_result);
            }, R"(
                Note: this always uses the modulo operation of the BigInteger class.
                C# uses truncated division whereas Python uses floored division.
                For more information see:
                https://en.wikipedia.org/wiki/Modulo_operation)")
            .def("__divmod__", [](BigInteger& self, BigInteger& other) {
                BigInteger remainder;
                auto div = BigInteger::div_rem(self, other, remainder);
                return py::make_tuple(div, remainder);
            })
            .def("__divmod__", [](BigInteger& self, py::int_& other) {
                BigInteger remainder;
                auto big_other = to_biginteger(other);
                auto div = BigInteger::div_rem(self, big_other, remainder);
                return py::make_tuple(div, remainder);
            })
            .def("__rdivmod__", [](BigInteger& other, py::int_& self) {
                BigInteger remainder;
                auto big_self = to_biginteger(self);
                auto div = BigInteger::div_rem(big_self, other, remainder);
                return py::make_tuple(py::int_(py::str(div.to_string())), py::int_(py::str(remainder.to_string())));
            })
            .def("__pow__", [](BigInteger& base, BigInteger& exp) {
                return BigInteger::pow(base, exp);
            })
            .def("__pow__", [](BigInteger& base, py::int_& exp) {
                try {
                    auto i = exp.cast<int>();
                    return BigInteger::pow(base, exp);
                } catch (const py::cast_error&) {
                    throw std::overflow_error("Exponent argument size exceeds C++ type 'int'");
                }
            })
            .def("__rpow__", [](BigInteger& exp, py::int_& base) {
                return to_py_int(BigInteger::pow(to_biginteger(base), exp));
            })
            .def("__pow__", [](BigInteger& base, BigInteger& exp, BigInteger& modulus) {
                return BigInteger::mod_pow(base, exp, modulus);
            })
            .def("__pow__", [](BigInteger& base, py::int_& exp, BigInteger& modulus) {
                return BigInteger::mod_pow(base, to_biginteger(exp), modulus);
            })
            .def("__pow__", [](BigInteger& base, BigInteger& exp, py::int_& modulus) {
                return BigInteger::mod_pow(base, exp, to_biginteger(modulus));
            })
            .def("__truediv__", [](BigInteger& self, BigInteger& other) {
                return self / other;
            })
            .def("__truediv__", [](BigInteger& self, py::int_& other) {
                return self / to_biginteger(other);
            })
            .def("__rtruediv__", [](BigInteger& self, py::int_& other) {
                auto bigi_result = to_biginteger(other) / self;
                return to_py_int(bigi_result);
            })
            .def("__floordiv__", [](BigInteger& self, BigInteger& other) {
                return self / other;
            })
            .def("__floordiv__", [](BigInteger& self, py::int_& other) {
                return self / to_biginteger(other);
            })
            .def("__rfloordiv__", [](BigInteger& self, py::int_& other) {
                auto bigi_result = to_biginteger(other) / self;
                return to_py_int(bigi_result);
            })
            .def("__lshift__", [](BigInteger& self, BigInteger& other) {
                return self << other;
            })
            .def("__lshift__", [](BigInteger& self, py::int_& other) {
                return self << other.cast<int>();
            })
            .def("__rlshift__", [](BigInteger& shift, py::int_& base) {
                auto bigi_result =  to_biginteger(base) << shift;
                return to_py_int(bigi_result);
            })
            .def("__rshift__", [](BigInteger& self, BigInteger& other) {
                return self >> other;
            })
            .def("__rshift__", [](BigInteger& self, py::int_& other) {
                return self >> other.cast<int>();
            })
            .def("__rrshift__", [](BigInteger& shift, py::int_& base) {
                auto bigi_result =  to_biginteger(base) >> shift;
                return to_py_int(bigi_result);
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
                return to_py_int(bigi_result);
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
                return to_py_int(bigi_result);
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
                return to_py_int(bigi_result);
            })
            .def("__neg__", [](BigInteger& self) { return -self; })
            .def("__pos__", [](BigInteger& self) { return +self;})
            .def("__abs__", [](BigInteger& self) { return BigInteger::abs(self);})
            .def("__invert__", [](BigInteger& self) { return ~self; })
            .def("__bytes__", [](BigInteger& self) {
                auto v_bytes = self.to_byte_array();
                return py::bytes(std::string(v_bytes.begin(), v_bytes.end()));
            }, R"(
                Return the value of the BigInteger in little endian order.
                For more information on the byte representation see:
                https://docs.microsoft.com/en-us/dotnet/api/system.numerics.biginteger.tobytearray?view=netcore-3.1)")
            .def("to_array",
                 [](BigInteger& self, bool is_unsigned, bool is_bigendian) {
                     auto v_bytes = self.to_byte_array(is_unsigned, is_bigendian);
                     return py::bytes(std::string(v_bytes.begin(), v_bytes.end()));
                 }, R"(
                Return the value of the BigInteger.
                For more information on the options see:
                https://docs.microsoft.com/en-us/dotnet/api/system.numerics.biginteger.tobytearray?view=netcore-3.1#System_Numerics_BigInteger_ToByteArray_System_Boolean_System_Boolean_)",
                 py::arg("is_unsigned") = false,
                 py::arg("is_bigendian") = false)
            .def("__int__", [](BigInteger& self) { return to_py_int(self); })   // support int()
            .def("__index__", [](BigInteger& self) { return to_py_int(self); }) // support range()
            .def("__str__", &BigInteger::to_string)
            .def_static("zero", &BigInteger::zero)
            .def_static("one", &BigInteger::one)
            .def_static("minus_one", &BigInteger::minus_one)
            .def("is_power_of_two", &BigInteger::is_power_of_two)
            .def("is_even", &BigInteger::is_even)
            .def_static("log", [](const BigInteger& value) { return BigInteger::log(value); }, py::arg("value"))
            .def_static("log", [](py::int_& value) { return BigInteger::log(to_biginteger(value)); }, py::arg("value"))
            .def_static("log", [](const BigInteger& value, double base_value) {
                return BigInteger::log(value, base_value);
                }, py::arg("value"), py::arg("base_value"))
            .def_static("log", [](py::int_& value, double base_value) {
                return BigInteger::log(to_biginteger(value), base_value);
                }, py::arg("value"), py::arg("base_value"))
            .def_static("log10", &BigInteger::log10, py::arg("value"))
            .def_static("log10", [](py::int_& value) { return BigInteger::log10(to_biginteger(value)); }, py::arg("value"))
            .def_property_readonly("sign", &BigInteger::sign);
}