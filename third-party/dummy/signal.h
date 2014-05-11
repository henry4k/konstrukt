/**
 * This file just contains tools for signal and exception runner.
 * You don't want to use it directly.
 */
#ifndef __DUMMY_SIGNAL_H__
#define __DUMMY_SIGNAL_H__

#include <signal.h>


typedef void (*dummySignalHandler)(int);

void dummyPushSignalHandler( dummySignalHandler fn );
void dummyPopSignalHandler();

const char* dummySignalToAbortReason( int signal );

#endif
