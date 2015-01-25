#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

int test1(void) {
    List *head = create_node(3);
    head = add_node(head, 4);
    char *result = tostring(head);
    if(strcmp(result, "43") != 0) {
        printf("Test1 failed: expected 43 got %s\n", result);
        // free the list and the result string before exit
        freelist(head);
        free(result);
        return 0;
    } else {
        printf("Test1 succeeded\n");
        // free the list and result string before exit
        freelist(head);
        free(result);
        return 1;
    }
}

