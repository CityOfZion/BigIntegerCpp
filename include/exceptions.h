#pragma once

#include <exception>

struct DivideByZero : public std::exception {
    const char* what() const throw() {
        return "Divide by zero exception";
    }
};