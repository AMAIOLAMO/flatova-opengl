#include <core/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

b8 try_load_file_text(const char *file_path, char **out_content, size_t *out_size) {
    FILE *file = fopen(file_path, "r");

    *out_content = NULL;
    *out_size = 0;

    if(file == NULL)
        return false;

    #define READ_CHUNK_SIZE 4
    
    char buf[READ_CHUNK_SIZE + 1] = {0};

    while(true) {
        size_t read_size = fread(&buf, sizeof(buf[0]), READ_CHUNK_SIZE, file);

        if(ferror(file)) {
            free(*out_content);
            *out_content = NULL;
            return false;
        }
        // else

        size_t new_size = read_size + *out_size + 1;

        *out_content = realloc(*out_content, new_size);

        memcpy(*out_content + *out_size, buf, read_size);
        (*out_content)[new_size - 1] = '\0';

        (*out_size) += read_size;

        if(feof(file))
            break;
    }

    fclose(file);
    return true;
}

void file_text_free(char *content) {
    free(content);
}
