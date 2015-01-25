/* CSC209 Assignment 2
 * Name: Xiuqi Xia
 * Student ID: 998269449
 * Login: c4xiaxiu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FILENAME_LEN 255    // longest allowed filename
#define OUTPUT_EXT ".enc"       // extention to add to output file
#define OUTPUT_EXT_LEN 4        // remember to count the dot in the extension
#define ALPHA_LEN 26            // number of letters in the alphabet

char apply_shift(char c, int shift);


int main(int argc, char** argv){
    const char* doc_str = 
    "\
    Usage: encrypt [FILENAME] [SHIFT_AMOUNT]\n\
    Use the Caesar cipher to encrypt text from a file.\n\
    FILENAME is the name of the file with text to be encrypted.\n\
    SHIFT_AMOUNT is the number of alphabet positions to shift each letter\n\
    Output is written to FILENAME.enc\n\
    REQ: FILENAME must be the name of a valid text file\n\
         SHIFT_AMOUNT must be a valid integer from 1 to 25 (inclusive)\n";

    // If the number of arguments is wrong, print error message and exit
    if(argc != 3){
        fputs("Invalid arguments!\n", stderr);
        fputs(doc_str, stderr);
        return 1;
    }
    // Assign meaningful names to the arguments
    const char* filename_arg = argv[1];
    const char* shift_arg = argv[2];
    
    // Try to convert the shift argument into an integer
    // If that fails, or if the integer is not in 1 to 25 (inclusive),
    // print error message and exit
    int shift = strtol(shift_arg, NULL, 10);// returns 0 if no numbers entered
    if((shift < 1) || (shift > 25)){
        fputs("Invalid shift amount!\n", stderr);
        fputs(doc_str, stderr);
        return 1;
    }

    // Try to read the file using filename
    // If it fails, print error message and exit
    FILE* input_file = fopen(filename_arg, "r");
    if(input_file == NULL){
        perror(filename_arg);
        return 1;
    }

    // Generate the filename for the output file
    char output_filename[MAX_FILENAME_LEN + 1]; // +1 for the '\0'
    // Need to initialize empty string for strncat to work properly
    output_filename[0] = '\0';
    // Concat the filename, then concat the file extension for the output file
    strncat(output_filename, filename_arg, (MAX_FILENAME_LEN - OUTPUT_EXT_LEN));
    strncat(output_filename, OUTPUT_EXT, OUTPUT_EXT_LEN);

    // Try to open the output file for writing
    // If it fails, print error and exit
    FILE* output_file = fopen(output_filename, "w");
    if(output_file == NULL){
        perror(output_filename);
        return 1;
    }

    // Go through each char in input_file, encrypt it by applying shift,
    // and write it into the output_file
    char c;
    while((c = fgetc(input_file)) != EOF){
        // Catch unexpected errors with reading the input file
        if(ferror(input_file)){
            perror(filename_arg);
            return 1;
        }
        // Try to write the encrypted char, if it fails, print error and exit
        if(fputc(apply_shift(c, shift), output_file) == EOF){
            perror(output_filename);
            return 1;
        }
    }

    fclose(input_file);
    fclose(output_file);
    return 0;
}


char apply_shift(char c, int shift){
/*  Return the result of adding shift to c,
    with wraparound to the beginning of the alphabet.
    The case of the character remains unchanged.
    Non-alphabetical characters are unchanged.
*/
    // Determine the offset to convert ASCII code of c to
    // its position in the alphabet
    int offset;
    if(isupper(c)){
        offset = 'A';
    }
    else if(islower(c)){
        offset = 'a';
    }
    else{
        // If c is neither upper nor lower case, it's not a letter,
        // so return c unchanged
        return c;
    }

    // Determine the position of c in the alphabet
    int position = c - offset;

    // Find the new position of c in the alphabet after adding shift
    position += shift;
    position = position % ALPHA_LEN; // apply wraparound

    // Convert position to ASCII code and return
    c = (char)(position + offset);
    return c;
}

