#pragma once
#ifndef _CXLIST_H
#define _CXLIST_H

#include <string.h>

// TODO: some macros are unsafe, ensure that they can account for some problems

// MACRO: returns the type of the each element
#define list_item_type(LIST_PTR) typeof( (LIST_PTR)->data[0] )

// MACRO: returns the sizeof the list's element items in bytes
#define list_sizeof_item(LIST_PTR) sizeof( (LIST_PTR)->data[0] )

// MACRO: refers to the last element of a list
#define list_tail(LIST_PTR) ( (LIST_PTR)->data[(LIST_PTR)->len - 1] )

// MACRO: refers to the head element of a list
#define list_head(LIST_PTR) ( (LIST_PTR)->data[0] )

// MACRO: appends an item in the list, 
#define list_append(LIST_PTR, ITEM) do { \
    (LIST_PTR)->len += 1; \
    while( (LIST_PTR)->cap < (LIST_PTR)->len ) { \
        (LIST_PTR)->cap = (LIST_PTR)->cap == 0 ? 2 : (LIST_PTR)->cap * 2; \
        (LIST_PTR)->data = realloc((LIST_PTR)->data, list_sizeof_item(LIST_PTR) * (LIST_PTR)->cap); \
    } \
    list_tail(LIST_PTR) = (ITEM); \
} while(0)

// MACRO: appends a variadic argument list
#define list_append_multi(LIST_PTR, ...) do { \
    list_item_type(LIST_PTR) v[] = { __VA_ARGS__ }; \
    for(size_t i = 0; i < (sizeof(v) / sizeof(v[0])); ++i) \
        list_append(LIST_PTR, v[i]); \
} while(0)

// MACRO: removes the element at index, but does not guarantee the list will stay in order
#define list_unordered_remove(LIST_PTR, INDEX) do { \
    (LIST_PTR)->data[INDEX] = (LIST_PTR)->data[(LIST_PTR)->len - 1]; \
    (LIST_PTR)->len -= 1; \
} while(0)

// MACRO: removes the element at index, but ensures the list will stay in order
#define list_remove(LIST_PTR, INDEX) do { \
    (LIST_PTR)->len -= 1; \
    memmove(&(LIST_PTR)->data[INDEX], \
        &(LIST_PTR)->data[(INDEX) + 1], \
        list_sizeof_item(LIST_PTR) * ((LIST_PTR)->len - (INDEX))); \
} while(0)

// MACRO: free's the data section within the list
#define list_free(LIST_PTR) free( (LIST_PTR)->data )

#endif // _CXLIST_H
