#include "lib.h"
#include "assert.h"

void test_inserer(){
    int solution[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    inserer(solution, 3, 7);

    int expected[] = {0, 1, 3, 4, 5, 6, 2, 7, 8, 9};
    assert( memcmp(solution, expected, 10 * sizeof(int)) == 0 );
    printf("Test passed!\n");
}
