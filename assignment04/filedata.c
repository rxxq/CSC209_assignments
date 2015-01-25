#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "filedata.h"


struct client_info clients[MAXCLIENTS];

// set of all sockets connected to server
fd_set master_set;

void clear_files(struct file_info *files) {
    int j;
    for (j = 0; j < MAXFILES; j++) {
        files[j].filename[0] = '\0';
        files[j].mtime = 0;
    }
}

/* initialize dirs and clients */
void init(){
    int i;
    for(i = 0; i < MAXCLIENTS; i++) {
        init_single_client(&clients[i]);
    }
}

// Initialize the members of a single client_info
void init_single_client(struct client_info* client){
    client->userid[0] = '\0';
    client->dirname[0]= '\0';
    client->sock = -1;
    
    client->dirname[0] = '\0';
    clear_files(client->files);

    client->file_in_transit[0] = '\0';
    client->bytes_left = 0;
    return;
}

/* Adds a client to the client array. (login a client)
 * Update the socket for the client.
 * Sets client state to sync
 * If the client is already in the array, update the dirname field with the 
 * dir field from s.  If an empty slot is available add the client userid and
 * dir.
 * If the directory does not exist create it.
 * Return 0 if it was able to add or update the client, and -1 if there 
 * was no slot available, or the directory cannot be created*/
int add_client(struct login_message s, int socketfd) {
    int i;
    for(i = 0; i < MAXCLIENTS; i++) {
        if(clients[i].userid[0] != '\0') {
            if(strcmp(clients[i].userid, s.userid) == 0) {
                // client is already in array so check if dir matches
                if(strcmp(clients[i].dirname, s.dir) != 0) {
                    // if no match then update dirname and clear files
                    strncpy(clients[i].dirname, s.dir, MAXNAME);
                    clear_files(clients[i].files);
                    break;
                }
            } 
       } else {
            // an empty slot to place the client
            strncpy(clients[i].userid, s.userid, MAXNAME);
            strncpy(clients[i].dirname, s.dir, MAXNAME);
            clear_files(clients[i].files);
            break;
        }
    }// exit loop
    if(i == MAXCLIENTS){
        // ran to the end of the loop without finding a slot to place client
        fprintf(stderr, "Error: Too many clients\n");
        return -1;
    }

    // If code gets to here it's because the loop exited right after
    // the client was added/updated
    // client[i] should refer to the client that was just added/updated

    // init the socket and set state to sync
    clients[i].sock = socketfd;
    clients[i].STATE = SYNC;

    // check if the directory exists by trying to open it
    DIR* dir_test = opendir(s.dir);
    // the diretory dosen't exist, create it
    if(dir_test == NULL){
        // Create a string with the path to the new directory,
        // starting with "./"
        char path[MAXNAME + 2] = "./\0";
        strncpy(path, s.dir, MAXNAME);
        // Create the directory, with 700 permissions
        if(mkdir(path, S_IRWXU) == -1){
            perror("mkdir");
            return -1;
        }
    }
    return 0;
}

/* remove_client: logout a client
 * Remove the client's socket from the master set
 * Close the client's socket
 * Reset client's socket to an invalid value
 * Client info remains in clients array
 * (it will get updated next time client logs in)
 */
void remove_client(struct client_info* client){
    // Remove client's socket from master set
    FD_CLR(client->sock, &master_set);
    // close the socket
    close(client->sock);
    // Reset client's socket to an invalid value to mark client as logged out
    client->sock = -1;
}


/* check_file - check if filename is in dirname's list
 *
 * - return a pointer to the file info struct for filename
 *   The caller will use this pointer to check mtime for this file
 * - add filename and return pointer to the file info struct if filename 
 *   was not found in dirname 
 * - return NULL if there is no more space in contents to add filename
 */

struct file_info *check_file(struct file_info *files, char *filename) {
    int i;
    for(i = 0; i < MAXFILES; i++) {
        if(strcmp(files[i].filename, filename) == 0) {
            return &files[i];
        } else if(files[i].filename[0] == '\0') {
            strncpy(files[i].filename, filename, MAXNAME);
            return &files[i];
        }
    }
    return NULL;
}

// Look in the clients array to find the client with the specified
// socket. If not found, return NULL
struct client_info* find_client_by_socket(int sock){
    int i;
    for(i=0; i<MAXCLIENTS; i++){
        if(clients[i].sock == sock){
            return &clients[i];
        }
    }
    return NULL;
}


void display_clients() {
    int i = 0;
    while(clients[i].userid[0] != '\0') {
        printf("%s -  %s\n", clients[i].userid, clients[i].dirname);
        int j = 0;
        while(clients[i].files[j].filename[0] != '\0') {
            printf("    %s %ld\n", clients[i].files[j].filename, 
                (long int)clients[i].files[j].mtime);
            j++;
        }
        i++;
    }
}
