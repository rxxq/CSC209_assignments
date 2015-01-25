#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rectangle.h"

int main(void) {    
    
    char *str1 = "Big rectangle";
    char *str2 = "Square";
  
    struct point *p1 = create_point(10, 10);
    struct point *p2 = create_point(100, 100);
    
    struct rectangle *r1 = create_rectangle(*p1, *p2, str1);
    print_rectangle(r1);
    printf("    expecting: (10, 10) (100, 100) Big rectangle\n");
    
    free(p2);
    p2 = create_point(20, 20);
    
    struct rectangle r2;
    r2.label = (char*)malloc(NAMESIZE);
    strncpy(r2.label, str2, NAMESIZE);
    r2.label[NAMESIZE] = '\0';
    r2.upperleft = *p1;
    r2.lowerright = *p2;

    free(p1);
    free(p2);
    
    print_rectangle(&r2);
    printf("    expecting: (10, 10) (20, 20) Square\n");
    

    /* TASK 2: Add code to calculate the area of r1 and r2 using both versions
     * of the function on each rectangle.
     */
    printf("r1 area1: %d\n", area1(*r1));
    printf("    expecting: 8100\n");

    printf("r2 area1: %d\n", area1(r2));
    printf("    expecting: 100\n");

    printf("r1 area2: %d\n", area2(r1));
    printf("    expecting: 8100\n");

    printf("r2 area2: %d\n", area2(&r2));
    printf("    expecting: 100\n");

    /* TASK 3: Add code to call change_label on r1 and change it to 
     *   "newname rectangle"    
     */
    change_label(r1, "very long name that exceeds the 20 character space");
    print_rectangle(r1);
    printf("    expecting: (10, 10) (100, 100) very long name that exceeds the 20 character space\n");

    /* TASK 3: Add code to call change_label on r2 and change it to 
     *   "newname square"    
     */
    change_label(&r2, "newname square");
    print_rectangle(&r2);
    printf("    expecting: (10, 10) (20, 20) newname square\n");


    
    free(r1);
    return 0;
}
