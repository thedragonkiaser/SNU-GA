#include "Utililty.h"

#if defined(linux)
	#include <sys/time.h>
#elif (defined(_WIN32) || defined(_WIN64))
	#pragma comment(lib, "winmm.lib")
	#include <windows.h>
#endif

namespace Utility {
	time_t getMilliSec() {
#if defined(linux)
		timeval tv;
		gettimeofday(&tv, 0);
		return tv.tv_sec * 1000ul + tv.tv_usec/1000ul;
#elif (defined(_WIN32) || defined(_WIN64))
		return timeGetTime();
#endif
	}
}
