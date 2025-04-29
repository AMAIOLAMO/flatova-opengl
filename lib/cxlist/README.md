# CXLIST
a simple dynamic array implementation in C, with only a single header file.

## USAGE
It should work in most systems that includes "strings.h"(this is subject to change)

Simply just include the header `cxlist.h` in your project and you are good to go.

Every type of list should be a struct and should have `cap`, `len` and `data` fields, defined as follows
(Syntax below is subject to change in newer versions)

```c
typedef struct Example_t {
    size_t cap, len;
    <list_element_type> *data;
} Example;
```

for example, to define an integer list

```c
typedef struct Integers {
    size_t cap, len;
    int *data;
} Integers;
```

and simply just create the struct like such:
```c
Integers ints = { 0 }; // zero initialize the contents(all fields set to zero)
```

to append to the list:
```c
list_append( &ints, 20 );
list_append( &ints, 200 * 3 + 7 );
```

to append multiple items at once:
```c
list_append_multi( &ints, 20, 30, 200, 10 + 2 );
```

to remove an item in the list (without preserving order of elements):
```c
list_unordered_remove( &ints, 2 ); // removes an element at index 2(order may not be preserved)
```

to remove an item in the list (ensures the order of elements is preserved):
```c
list_remove( &ints, 2 );  // removes an element at index 2(order preserved)
```

to free the list:
```c
list_free( &ints );
```

returns the typeof element in the list(compile time):
```c
list_item_type( &ints );
```

returns the sizeof each element in bytes in the list(compile time):
```c
list_sizeof_item( &ints );
```

refers to the head element (the first element, 0th index)
```c
list_head( &ints );
```

refers to the last element
```c
list_tail( &ints );
```


## LICENSE
Licensed under MIT
