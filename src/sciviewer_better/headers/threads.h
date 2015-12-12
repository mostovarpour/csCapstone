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
    #define thread_func void
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

#undef WINDOWS
#endif
