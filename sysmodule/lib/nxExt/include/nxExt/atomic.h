/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#pragma once

#include <switch.h>

typedef struct
{
	size_t count;
	Mutex mutex;
} AtomicRef;

NX_INLINE size_t atomicRefIncrement(AtomicRef* ref)
{
	size_t r;
	mutexLock(&ref->mutex);
	r = ++ref->count;
	mutexUnlock(&ref->mutex);
	return r;
}

NX_INLINE size_t atomicRefDecrement(AtomicRef* ref)
{
	size_t r;
	mutexLock(&ref->mutex);
	if(ref->count > 0)
	{
		ref->count--;
	}
	r = ref->count;
	mutexUnlock(&ref->mutex);
	return r;
}
