#include <tinydir_ext.h>

int tinydir_is_nav(const tinydir_file file) {
    return file.is_dir && (
        strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0
    );
}
