// Include windows for CreateThread or pthread for linux threads depending on build platform
#if defined _WIN32 || _WIN64
	#include <Windows.h>
#else
	#include <pthread.h>
#endif