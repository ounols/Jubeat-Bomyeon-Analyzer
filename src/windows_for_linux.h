#pragma once


#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

typedef long DWORD;

static long timeGetTime() {
    // long ms; // Milliseconds
    // int s;  // Seconds
    // struct timespec spec;

    // clock_gettime(CLOCK_REALTIME, &spec);

    // s  = abs(spec.tv_sec * 1000);
    // ms = abs(round(spec.tv_nsec / 1.0e6)); // Convert nanoseconds to milliseconds

    // long result = ms + s;
    // return abs(result);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec*1000)+(tv.tv_usec/1000));
}

static void Sleep(int milliseconds){
    usleep(milliseconds * 1000);
    return;
}