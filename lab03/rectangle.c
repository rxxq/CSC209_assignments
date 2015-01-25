#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rectangle.h"

/* create_point dynamically allocates memory (using malloc) to store a point, 
 * and gives it initial values. Returns a pointer to the newly created structure
 */
struct point *create_point(int x, int y) {
    struct point *p = malloc(sizeof(struct point));
    if (p == NULL) {
        perror("Error allocating space for point.");
        exit(1);
    }
    p->x = x;
    p->y = y;
    return p;
}

/* create_rectangle dynamically allocates memory to store a rectangle, gives it
 * initial values, and returns a pointer to the newly created rectangle.
 */
struct rectangle *create_rectangle(struct point ul, struct point lr, 
                                   char *label) {

    struct rectangle *r = malloc(sizeof(struct rectangle));
    /* TASK 1: fill in the rest of this function */
    
    int label_size;

    r->upperleft = ul;
    r->lowerright = lr;

    label_size = strlen(label) + 1;

    r->label = (char*)malloc(sizeof(char) * label_size);
    if(r->label == NULL){
        perror("Could not allocate memory for label\n");
        exit(1);
    }

    strncpy(r->label, label, label_size);
    r->label[label_size] = '\0';

    return r;
}

/* TASK 2: Write two versions of the function to calculate the area of the
 *  rectangle.
 */

/* Compute the area of the rectangle r. Assume all point values are positive. 
 * area1 takes the struct itself as the parameter, and returns the area
 * of the rectangle
 */
int area1(struct rectangle r) {
    /* TASK 2: complete this function and replace the return statement */

    int length = r.lowerright.x - r.upperleft.x;
    int height = r.lowerright.y - r.upperleft.y;

    return length * height;
}

/* Compute the area of the rectangle r. Assume all point values are positive. 
 * area2 takes a pointer to the struct as a parameter, and returns the area
 * of the rectangle
 */
int area2(struct rectangle *r) {
    /* TASK 2: complete this function and replace the return statement */

    int length = r->lowerright.x - r->upperleft.x;
    int height = r->lowerright.y - r->upperleft.y;

    return length * height;
}

/* change_label changes the label in r to newlabel
 * Notice that change_label takes a pointer to the struct as an argument 
 */
void change_label(struct rectangle *r, char *newlabel) {
    /* TASK 3: Complete function.*/
    int old_label_size = strlen(r->label) + 1;
    int new_label_size = strlen(newlabel) + 1;
    
    if (old_label_size < new_label_size){
        free(r->label);
        r->label = (char*)malloc(sizeof(char) * new_label_size);
        if (r->label == NULL){
            perror("unable to allocate memory for label\n");
            exit(1);
        }
    }

    strncpy(r->label, newlabel, new_label_size);
    r->label[new_label_size] = '\0';

}

void print_rectangle(struct rectangle *r) {
    printf("(%d, %d) (%d, %d) %s\n", r->upperleft.x, r->upperleft.y, 
        r->lowerright.x, r->lowerright.y, r->label);
}
