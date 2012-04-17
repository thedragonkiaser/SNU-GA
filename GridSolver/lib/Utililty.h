#pragma once

#include <sstream>
#include <string>

namespace Utility {
#if defined(linux)
    typedef unsigned long time_t;
#endif
	extern time_t getMilliSec();

	template<typename T>
	T strTo(std::string s) {
		std::istringstream ss(s);
		T val;
		ss >> val;
		return val;
	}

	template<typename T>
	std::string toStr(T val) {
		std::stringstream ss;
		ss << val;
		return ss.str();
	}
}
