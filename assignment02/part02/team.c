//
// CSC209 team.c
//
// Program for maintaining a personal team.
//
// Uses a linked list to hold the team players.
//
// Author: Xiuqi Xia
// Student ID: 998269449
// CDF login: c4xiaxiu
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>


//**********************************************************************
// Program-wide Constants
//

#define MAX_LENGTH 1023
#define NULL_CHAR '\0'
#define NEWLINE '\n'
#define GOALKEEPER 'G'
#define DEFENDER 'D'
#define MIDFIELDER 'M'
#define STRIKER 'S'
#define VALID_POS_CHAR "%1[GDMS]"   // scanf format string to read
                                    // one valid position char from input


//**********************************************************************
// Linked List Definitions
//  Define your linked list node and pointer types
//  here for use throughout the file.
//
struct player_node{
    char* family_name;
    char* first_name;
    char position;
    int value;
    struct player_node* next;
}typedef p_node;

// A pair of p_nodes
struct current_previous_pair{
    p_node* current;
    p_node* previous;
}typedef cp_pair;

//**********************************************************************
// Linked List Function Declarations
//
// Functions that modify the linked list.
//   Declare your linked list functions here.
//
p_node* new_player(char* family_name,
                   char* first_name,
                   char position,
                   int value);
void free_player(p_node* player);
void free_team(p_node* head);
void print_player(p_node* player);
void print_team(p_node* head);
cp_pair search_by_fam_name(char* fam_name, p_node* head);
p_node* insert_player(p_node* head, p_node* insertee); 
p_node* delete_player(p_node* head, cp_pair delete_location);
p_node* search_by_value(int value, p_node* head);

//**********************************************************************
// Support Function Declarations
//

void safegets (char s[], int arraySize);        // gets without buffer overflow
void familyNameDuplicate (char familyName[]);   // marker/tester friendly
void familyNameFound (char familyName[]);       //   functions to print
void familyNameNotFound (char familyName[]);    //     messages to user
void familyNameDeleted (char familyName[]);
void printTeamEmpty (void);
void printTeamTitle(void);
void printNoPlayersWithLowerValue(int value);


//**********************************************************************
// Main Program
//

int main (void)
{
    const char bannerString[]
        = "Personal Team Maintenance Program.\n\n";
    const char commandList[]
        = "Commands are I (insert), D (delete), S (search by name),\n"
          "  V (search by value), P (print), Q (quit).\n";

    // Declare linked list head.
    //   ADD STATEMENT(S) HERE TO DECLARE LINKED LIST HEAD.


    // announce start of program
    printf("%s",bannerString);
    printf("%s",commandList);

    char response;
    char input[MAX_LENGTH+1];
    char fam_name[MAX_LENGTH+1];    // The fields in p_node are not
    char first_name[MAX_LENGTH+1];  //  actually this long, these are just for
    char pos;                       //  temporarily storing user input
    int value;

    p_node* team = NULL;        // Linked-list of players
    p_node* insertee;           // A new node to be inserted
    cp_pair search_result;      // Result of search_by_fam_name
    p_node* result_list;        // Result of search_by_value
                                // ^(this is a linked-list, not just a node)
    do
    {
        printf("\nCommand?: ");
        safegets(input,MAX_LENGTH+1);
        // Response is first char entered by user.
        // Convert to uppercase to simplify later comparisons.
        response = toupper(input[0]);

        if (response == 'I')
        {
            // Insert a player entry into the linked list.
            // Maintain the list in correct order (G, D, M, S).

            // USE THE FOLLOWING PRINTF STATEMENTS WHEN PROMPTING FOR DATA:

            /***** USER INPUT *****/
            printf("  family name: ");
            safegets(fam_name, MAX_LENGTH+1);
            
            printf("  first name: ");
            safegets(first_name, MAX_LENGTH+1);
            
            printf("  position: ");
            // Store the entire input in a string first,
            // to ensure that no characters remain in stdin.
            safegets(input, MAX_LENGTH+1);
            input[0] = toupper(input[0]); // simplify later comparisons
            // Try to read one character, in the set of valid characters
            // If cannot read a valid char, restart the loop
            if(sscanf(input, VALID_POS_CHAR, &pos) != 1){
                printf("Invalid input! Position must be either "
                       "'G', 'D', 'M', or 'S'\n");
                continue;
            }

            printf("  value: ");
            safegets(input, MAX_LENGTH+1);
            // Try to read an int from the input
            // If cannot read an int, restart the loop
            if(sscanf(input, "%d", &value) != 1){
                printf("Invalid input! Value must be an integer.\n");
                continue;
            }
            
            /***** CREATE NEW NODE AND INSERT IT *****/
            // Check if the family name already exists in the team.
            // If it does, print error message and restart the look.
            search_result = search_by_fam_name(fam_name, team); 
            if(search_result.current != NULL){
                familyNameDuplicate(fam_name);
                continue;
            }     
            // Create a new node using the information collected
            // if it fails, print error message and exit
            insertee = new_player(fam_name, first_name, pos, value);
            if(insertee == NULL){
                perror("new_player");
                return 1;
            }
            // Insert the new node into its correct place in the list
            team = insert_player(team, insertee);
        }

        else if (response == 'D')
        {
            // Find the node to delete by searching by family name
            printf("\nEnter family name for entry to delete: ");
            safegets(fam_name, MAX_LENGTH+1);
            search_result = search_by_fam_name(fam_name, team);
            // If a match was not found for the family name,
            // print error message and restart menu loop.
            if(search_result.current == NULL){
                familyNameNotFound(fam_name);
                continue;
            }

            // Delete the search result from the list.
            team = delete_player(team, search_result);
            familyNameDeleted(fam_name);
        }

        else if (response == 'S')
        {
            // Get family name from user input
            printf("\nEnter family name to search for: ");
            safegets(fam_name, MAX_LENGTH+1);

            // Search for a player by family name.
            search_result = search_by_fam_name(fam_name, team);
            if(search_result.current != NULL){
                familyNameFound(fam_name);
                print_player(search_result.current);
            }
            else{
                familyNameNotFound(fam_name);
            }

        }

        else if (response == 'V')
        {
            // Get the value from user input
            printf("\nEnter value: ");
            // Store the entire input in a string first,
            // to ensure that no characters remain in stdin
            safegets(input, MAX_LENGTH+1);
            // Try to read an int from the input
            // If cannot read an int, restart the loop
            if(sscanf(input, "%d", &value) != 1){
                printf("Invalid input! Value must be an integer.\n");
                continue;
            }

            // Search for players that are worth less than or equal a value.
            result_list = search_by_value(value, team);
            if(result_list == NULL){
                printNoPlayersWithLowerValue(value);
            }
            else{
                print_team(result_list);
                // Rememeber to free result_list
                free_team(result_list);
            }
        }
        else if (response == 'P')
        {
            // Print the team.
            printTeamTitle();
            print_team(team);

        }
        else if (response == 'Q')
        {
            ; // do nothing, we'll catch this case below
        }
        else
        {
            // do this if no command matched ...
            printf("\nInvalid command.\n%s\n",commandList);
        }
    } while (response != 'Q');

    // Delete the whole linked list that hold the team.
    free_team(team);
    team = NULL;

    // Print the linked list to confirm deletion.
    //   ADD STATEMENT(S) HERE
    print_team(team);
    printTeamEmpty();

    return 0;
}

//**********************************************************************
// Support Function Definitions

// Function to get a line of input without overflowing target char array.
void safegets (char s[], int arraySize)
{
    int i = 0, maxIndex = arraySize-1;
    char c;
    while (i < maxIndex && (c = getchar()) != NEWLINE)
    {
        s[i] = c;
        i = i + 1;
    }
    s[i] = NULL_CHAR;
}

// Function to call when user is trying to insert a family name
// that is already in the book.
void familyNameDuplicate (char familyName[])
{
    printf("\nAn entry for <%s> is already in the team!\n"
           "New entry not entered.\n",familyName);
}

// Function to call when a player with this family name was found in the team.
void familyNameFound (char familyName[])
{
    printf("\nThe player with family name <%s> was found in the team.\n",
             familyName);
}

// Function to call when a player with this family name was
// not found in the team.
void familyNameNotFound (char familyName[])
{
    printf("\nThe player with family name <%s> is not in the team.\n",
             familyName);

}

// Function to call when a family name that is to be deleted
// was found in the team.
void familyNameDeleted (char familyName[])
{
    printf("\nDeleting player with family name <%s> from the team.\n",
             familyName);
}

// Function to call when printing an empty team.
void printTeamEmpty (void)
{
    printf("\nThe team is empty.\n");
}

// Function to call to print title when whole team being printed.
void printTeamTitle (void)
{
    printf("\nMy Team: \n");
}

// Function to call when no player in the team has
// lower or equal value to the given value
void printNoPlayersWithLowerValue(int value)
{
    printf("\nNo player(s) in the team is worth less than or equal to <%d>.\n",
        value);
}

//**********************************************************************
// Add your functions below this line.


// Allocate memory for and return a pointer to a new player_node.
// If memory allocation fails, no memory is allocated,
// and a NULL pointer is returned
p_node* new_player(char* family_name, char* first_name,
                   char position, int value){

    // Allocate memory for the node. If it fails, return NULL
    p_node* new_node = malloc(sizeof(p_node));
    if(new_node == NULL) return NULL;

    // Allocate and copy the strings for family name and first name
    // If it fails, free any parts of the node thave have already been
    // allocated before returning NULL
    new_node->family_name = strdup(family_name);
    if(new_node->family_name == NULL){
        free(new_node);
        return NULL;
    }
    new_node->first_name = strdup(first_name);
    if(new_node->first_name == NULL){
        free(new_node->family_name);
        free(new_node);
        return NULL;
    }

    // Initialize the rest of the attributes by value
    new_node->position = position;
    new_node->value = value;
    new_node->next = NULL;

    return new_node;
}

// Free the memory allocated for one player node
void free_player(p_node* player){
    // Free the malloc'ed strings inside player
    free(player->family_name);
    free(player->first_name);
    // Free player itself
    free(player);
    return;
}


// Free the memory of all the nodes in the linked list, starting at head
void free_team(p_node* head){
    // base case: head is already an empty list, do nothing
    if(head == NULL){
        return;
    }
    // Recursive decomposition
    // First free all the nodes after this node
    free_team(head->next);
    // Free the current node
    free_player(head);
    return;
}


// Print the family name, first name, position, and value of a player node
void print_player(p_node* player){
    printf("%s\n%s\n%c\n%d\n", player->family_name,
                               player->first_name,
                               player->position,
                               player->value);
}


// Print each node in the list starting at head
void print_team(p_node* head){
    // Base case: head is an empty list, do nothing
    if(head == NULL){
        return;
    }
    // Recursive decomposition
    // First print the head, then print the rest of the list
    printf("\n");
    print_player(head);
    print_team(head->next);
}


// Search the list starting at head and return a cp_pair containing:
// current: The node with the matching family name, or NULL if no match found
// previous: The node previous to the node with the matching family name,
//           or NULL if no match found, or if curr is already the first node.
cp_pair search_by_fam_name(char* fam_name, p_node* head){
    
    // Initialize current and previous to NULL
    cp_pair result = {NULL, NULL};

    // Base case: list is empty
    if(head == NULL){
        // fam_name does not exist in the list, so return NULL result
        return result;
    }
    // Base case: family name of head matches
    else if(strcmp(head->family_name, fam_name) == 0){
        // Return head as the current node (NULL as previous node)
        result.current = head;
        return result;
    }
    // Recursive decomposition:
    else{
        // Search in the rest of the list
        result = search_by_fam_name(fam_name, head->next);
        // if the current node in the result is set,
        // but the previous node in the result is not set,
        // head must be the previous node.
        if(result.current != NULL && result.previous == NULL){
            result.previous = head;
        }
        return result;
    }
}


// Insert insertee into the list (starting at head) at the correct position
// and return the head of the new list.
// REQ: The list starting at head must already be in the correct order
//      (goalkeepers, defenders, midfielders, strikers)
p_node* insert_player(p_node* head, p_node* insertee){

    // Base case: Correct position is to insert before head
    // (link insertee to head, return insertee as new head)
    
    // If list is empty then this is the only valid position to insert.
    // (strikers can only be inserted here)
    if(head == NULL){
        insertee->next = head;
        return insertee;
    }
    // A midfielder can be inserted if the list starts with a striker
    else if((insertee->position == MIDFIELDER) && 
            (head->position == STRIKER)){
        insertee->next = head;
        return insertee;
    }
    // A defender can be inserted if the list starts with
    // a midfielder or a striker
    else if((insertee->position == DEFENDER) && 
            ((head->position == MIDFIELDER) ||
             (head->position == STRIKER))){
        insertee->next = head;
        return insertee;
    }
    // A goalkeeper can be inserted if the list starts with any non-goalkeeper
    else if((insertee->position == GOALKEEPER) &&
            (head->position != GOALKEEPER)){
        insertee->next = head;
        return insertee;
    }

    // Recursive decomposition:
    // Correct position is not before head
    else{
        // Insert insertee into the rest of the list
        p_node* rest_of_list = head->next;
        rest_of_list = insert_player(rest_of_list, insertee);
        // link head to the rest of the list and return it
        head->next = rest_of_list;
        return head;
    }
}


// Free and remove the node indicated by delete_location
// from the list starting at head. Return the head of the modified list.
// REQ: the current and previous nodes in delete_location must be adjacent
//      nodes that exist in the list starting at head.
p_node* delete_player(p_node* head, cp_pair delete_location){
    // If the current node is NULL, do nothing
    if(delete_location.current == NULL){
        return head;
    }
    // If the previous node is NULL, we should delete the head
    else if(delete_location.previous == NULL){
        // Remember the node after head
        p_node* next = head->next;
        // Make sure head is freed
        free_player(head);
        // Return next as the new head
        return next;
    }
    // Both current and previous node exists
    else{
        // Link the previous node to the node after current
        delete_location.previous->next = delete_location.current->next;
        // Free the memory for the current node
        free_player(delete_location.current);
        // Return the original head of the list
        return head;
    }
}


// Return a list of all players in the list starting at head
// that are worth less than or equal to value.
// NOTE: the list returned by this function needs to be freed after use.
p_node* search_by_value(int value, p_node* head){
    
    p_node* result_list = NULL;
    p_node* temp;

    // Go through the list starting at head
    while(head != NULL){
        // Check if the current node qualifies to be added to result
        if(head->value <= value){
            // Make a temporary copy of the current node
            temp = new_player(head->family_name, head->first_name,
                              head->position, head->value);
            // Insert the node into the result_list
            result_list = insert_player(result_list, temp);
        }
        // Advance to next node
        head = head->next;
    }

    return result_list;
}

