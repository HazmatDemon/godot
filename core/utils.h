#ifndef UTILS_H
#define UTILS_H

#include "core/variant.h"
#include <sstream>

class UTILS {

public:
    static String int_to_hex(int64_t number, int size);
    static int64_t hex_to_int(const String &hex);

    static double hex_to_float(const String &hex);
    static String float_to_hex(double number);

	static String str_to_hex(const String &str, int size);
	static String hex_to_str(const String &hex, int size);
};

#endif
