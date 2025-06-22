#pragma once
#ifndef _FL_CORE_TYPES_H
#define _FL_CORE_TYPES_H

typedef unsigned char b8;
typedef unsigned int  u32;
typedef unsigned long u64;
typedef char* c_str;
typedef const char* literal_str;

#ifndef true
    #define true 0x1
#endif

#ifndef false
    #define false 0x0
#endif

#ifndef NULL
    #define NULL 0x0
#endif


#endif // _FL_CORE_TYPES_H
