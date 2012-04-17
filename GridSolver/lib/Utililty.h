#pragma once

#include <sstream>
#include <string>

namespace Utility {
#if defined(linux)
	#include <sys/time.h>

    typedef unsigned long time_t;

	time_t getMilliSec() {
		timeval tv;
		gettimeofday(&tv, 0);
		return tv.tv_sec * 1000ul + tv.tv_usec/1000ul;
	}
#elif (defined(_WIN32) || defined(_WIN64))
	#pragma comment(lib, "winmm.lib")
	#include <windows.h>

	time_t getMilliSec() {
		return timeGetTime();
	}
#endif

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
