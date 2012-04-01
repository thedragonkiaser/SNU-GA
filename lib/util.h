#if !defined(_UTIL_H_)
#define _UTIL_H_

#include <sstream>
#include <string>

namespace MyUtil {
#if defined(linux)
    typedef long long time_t;
	time_t getTime() {
		timeval tv;
		gettimeofday(&tv, 0);
		return tv.tv_sec * 1000000LL + tv.tv_usec;
	}
#elif defined(_WIN32) || defined(_WIN64)
	#pragma comment(lib, "winmm.lib")
	#include <windows.h>

	time_t getTime() { return timeGetTime(); }
#endif

	template<typename T>
	T strTo(std::string& s) {
		std::istringstream ss(s);

		T val;
		ss >> val;
		return val;
	}

	template<typename T>
	std::string fromStr(T val) {
		std::stringstream ss;
		ss << val;
		return ss.str();
	}
}
#endif