#include <string.h>
#include <stdio.h>


// based on code from:
// http://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format?rq=1

const char *to_binary(unsigned int x) {
    static char bits[17];   // value remembered between calls
    bits[0] = '\0';
    unsigned int z;
    for (z = 1 << 15; z > 0; z >>= 1) { // goes through 16 digits of x
        strcat(bits, (x & z) ? "1" : "0");
    }
    return bits;
}


/*Return 1 iff bit i in x is set, 0 otherwise.
 *Remember that the bits are numbered 0 from the right:
 *b15b14b13 ... b3b2b1b0
*/

short is_set(unsigned short x, int bit) {
    // (x >> bit) shifts the relevant bit to the last digit
    // & 1 to extract the value of the last bit only
    return ((x >> bit) & 1);
}

/* Swap the first 8 bits and the last 8 bits of 16-bit value.
 *For example, 00000001 11001100
 *becomes 11001100 00000001
 */
 
unsigned short swap_bytes(unsigned short x) {
    // unsigned short is 16 bits, each half is 8 bits

    // Save the result of shifting the bottom half into the top half
    unsigned short bot_to_top = x << 8;

    // Save the result of shifting the top half into the bottom half
    unsigned short top_to_bot = x >> 8;

    // Combine the two results and return
    return (bot_to_top | top_to_bot);
}

/*A value has even parity if it has an even number of 1 bits.
 *A value has an odd parity if it has an odd number of 1 bits.
 *For example, 0110 has even parity, and 1110 has odd parity.
 *Return 1 iff x has even parity.
 */
 
int has_even_parity(unsigned int x) {
    // unsigned int is 32 bits

    // loop through all the digits in x
    int i;
    int odd_parity = 0;
    for(i=0; i<32; i++){
        // xoring an even number of 1 bits result in 0,
        // xoring an odd number of 1 bits result in 1.
        // (x & (1 << j)) mask all the digits except the ith digt
        // Then shift that digit to the bottom
        odd_parity ^= ((x & (1 << i)) >> i);
    }

    // Return the opposite boolean value of odd_parity to get even parity
    return (!odd_parity);
}

/*Does the following version of the function work?*/

int has_even_parity_tricky(unsigned short x) {  // was originally unsigned int
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return !(x & 1);      // was originally return !x
}

int main(void) {
    unsigned short sample = 0b1000000111001100;
    printf("Test value: %s\n", to_binary(sample));
    printf("Is bit 0 set? Got: %d\n", is_set(sample, 0));
    printf("Is bit 3 set? Got: %d\n", is_set(sample, 3));
    printf("Is bit 4 set? Got: %d\n", is_set(sample, 4));
    printf("Swapping the bytes gives: %s\n", to_binary(swap_bytes(sample)));
    printf("Even parity? Got: %d\n", has_even_parity(sample));
    printf("Even parity(tricky) Got: %d\n", has_even_parity_tricky(sample));
    
    return 0;
}
