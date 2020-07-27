#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <iomanip>
#include <iostream>
#include <cwchar>

#include "core/variant.h"

struct UTILS {

public:
    static String int_to_hex(int64_t number, int size);
    static int64_t hex_to_int(const String &hex);

    static double hex_to_float(const String &hex);
    static String float_to_hex(double number);

};

#endif // UTILS_H