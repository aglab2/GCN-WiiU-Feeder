#pragma once

#include <stddef.h>
#include <string.h>

typedef unsigned short USHORT;
typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
typedef unsigned int DWORD;
typedef short SHORT;
typedef long LONG;
typedef void VOID;
typedef void* LPVOID;
typedef bool BOOLEAN;
typedef bool BOOL;
typedef LONG* PLONG;
typedef ULONG* PULONG;

#define FORCEINLINE inline
#define _Out_
#define _In_
#define _Function_class_(x)
#define CALLBACK

static inline void RtlZeroMemory(void* Destination, size_t Length)
{
    memset(Destination, 0, Length);
}

#define _countof(array) (sizeof(array) / sizeof((array)[0]))
#define scanf_s scanf
