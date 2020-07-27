#include "utils.h"

String UTILS::int_to_hex(int64_t number, int size = sizeof(int64_t) * 2) {
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(size) << std::hex << number;
	return String(stream.str().c_str());
}

int64_t UTILS::hex_to_int(const String &hex) {
	std::stringstream stream;
	uint64_t result;
	stream << std::hex << hex.utf8().ptr();
	stream >> result;
	return result;
}

String UTILS::float_to_hex(double number) {
	std::stringstream stream;
	stream << std::hex << *reinterpret_cast<uint64_t *>(&number);
	return String(stream.str().c_str());
}

double UTILS::hex_to_float(const String &hex) {
	double result;
	std::stringstream stream;
	stream << hex.utf8().ptr();
	stream >> std::hex >> *reinterpret_cast<uint64_t *>(&result) >> result;
	return result;
}