#pragma once
#ifndef _FL_CORE_UTILS_H
#define _FL_CORE_UTILS_H

#include <stddef.h>
#include <core/types.h>

// allocates a buffer to the buffer referenced by "out_content", and sets the "out_size" to be content size.
// returns true if file loaded succesfully, false otherwise.
b8 try_load_file_text(const char *file_path, char **out_content, size_t *out_size);

// frees the buffer "out_content" returned by try_load_file_text
void file_text_free(char *content);



#endif // _FL_CORE_UTILS_H
