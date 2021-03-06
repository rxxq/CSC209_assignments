#include <stdio.h>

/* Notice that we pass an array to a function we define a function parameter 
 * using the syntax int *arrayname where arrayname is a new local variable
 * by which we will access the array. Notice that we access the array elements
 * using arrayname[i]. Also notice that we must pass the size of the array
 * to the function. We will learn about this in lecture.
 */ 
void print_array(int *a, int size) {
    int i;
    for(i = 0; i < size; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

 
/* Return the sum of the elements of array a. */
int sum(int *a, int size) {
    int i;
    int a_sum = 0;
    for(i=0; i < size; i++){
        a_sum += a[i];
    }

    return a_sum;
}



/* Return the largest element in array a. */
int biggest(int *a, int size){
    int i;
    int a_biggest = a[0];
    for(i=1; i < size; i++){
        if(a[i] > a_biggest){
            a_biggest = a[i];
        }
    }

    return a_biggest;
}


/* Return the average of the elements in array a as a double.  
 * (Use sum to implement this.)
 */
double average(int *a, int size) {
    return (double)sum(a,size) / size;
}



/* Reverse the order of the elements in a in place. Try to do it 
 * without using a temporary array inside the function.
 * Note that we can modify the contents of the array a, and
 * these modifications are visible after the function returns.
 */
void reverse(int *a, int size) {
    int i;
    int temp;

    // Swap first and last, second and second last, etc.
    // Expect size/2 to always round down (truncate towards 0),
    // so that the middle element is not included in the loop
    for(i=0; i < (size/2); i++){
        temp = a[i];
        a[i] = a[(size-1) - i];
        a[(size-1) - i] = temp;
    }
}


int main() {
    
    /* Test sum */
    int a[3] = {1, 2, 3};
    printf("1. sum returned %d. Expecting 6.\n", sum(a, 3));
    
    int b[1]  = {10};
    printf("2. sum returned %d. Expecting 10.\n", sum(b, 1));
    
    /* Test biggest */
    printf("3. biggest returned %d. Expecting 3.\n", biggest(a, 3));
    printf("4. biggest returned %d. Expecting 10\n", biggest(b, 1));
    int c[4] = {4, -1, 0, 3};
    printf("5. biggest returned %d. Expecting 4\n", biggest(c, 1));
    int d[3] = {-4, -1, -5};
    printf("6. biggest returned %d. Expecting -1\n", biggest(d, 3));
    
    /* Test average */
    printf("7. average returned %f. Expecting 2.0\n", average(a, 3));
    printf("8. average returned %f. Expecting 10.0\n", average(b, 1));
    printf("9. average returned %f. Expecting 1.5\n", average(c, 4));
    printf("10. average returned %f. Expecting -3.3333\n", average(d, 3));
    
    /* Test reverse */
    printf("Reversing d - original: ");
    print_array(d, 3);
    printf("              reversed: ");
    reverse(d, 3);
    print_array(d, 3);

    printf("Reversing c - original: ");
    print_array(c, 4);
    printf("              reversed: ");
    reverse(c, 4);
    print_array(c, 4);

    printf("Reversing b - original: ");
    print_array(b, 1);
    printf("              reversed: ");
    reverse(b, 1);
    print_array(b, 1);
    return 0;
    
}
