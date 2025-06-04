#include <core/common.h>

int main(void) {
    // STRING CMP
    assert(str_ends_with("shader/asd;lfkjasdf.vs", ".vs") == true);
    assert(str_ends_with("shader/asdvs", ".gs") == false);
    assert(str_ends_with("shader/asdvs", "") == true);
    assert(str_ends_with("", ".vs") == false);
    assert(str_ends_with("shader/asdvs", "dvs") == true);

    return 0;
}
