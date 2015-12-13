#ifndef __THREADS_H__
#define __THREADS_H__
// Get thread functions based on OS

#if defined _WIN32 || defined _WIN64
    #define WINDOWS
    #include <Windows.h>
#else
    #include <pthread.h>
#endif

// Define a thread
#ifdef WINDOWS
    #define thread HANDLE
#else
    #define thread pthread_t   
#endif

// Define thread function declaration
#ifdef WINDOWS
    #define thread_func DWORD WINAPI
#else
    #define thread_func void*
#endif

// Thread Argument type
#ifdef WINDOWS
    #define thread_arg LPVOID
#else
    #define thread_arg void*
#endif

// Define function to create threads
#ifdef WINDOWS
    #define create_thread(ptThread, FUNCTION, ARG) ptThread = CreateThread(NULL, 0, FUNCTION, ARG, 0, NULL);
#else
    #define create_thread(ptThread, FUNCTION, ARG) pthread_create(&ptThread, NULL, FUNCTION, ARG);
#endif

// Define mutex object for shared resources
#ifdef WINDOWS
	#define Mutex HANDLE
#else
	#define Mutex pthread_mutex_t
#endif

// Define create_mutex function
#ifdef WINDOWS
	#define init_mutex(mutex_obj) mutex_obj = CreateMutex(NULL, FALSE, NULL)
#else
	#define init_mutex(mutex_obj) pthread_mutex_init(&mutex_obj, NULL)
#endif

// Define lock mutex function
#ifdef WINDOWS
	#define lock_mutex(mutex) WaitForSingleObject(mutex, INFINITE)
#else
	#define lock_mutex(mutex) pthread_mutex_lock(&mutex)
#endif

// Define release mutex
#ifdef WINDOWS
	#define release_mutex(mutex) ReleaseMutex(mutex);
#else
	#define release_mutex(mutex) pthread_mutex_unlock(&mutex);
#endif

// Define destroy mutex
#ifdef WINDOWS
	#define destroy_mutex(mutex) CloseHandle(mutex)
#else
	#define destroy_mutex(mutex) pthread_mutex_destroy(&mutex)
#endif

#undef WINDOWS
#endif
