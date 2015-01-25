// TODO: prune list of includes
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>        /* for getenv */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>    /* Internet domain header */

#include "filedata.h"
#include "wrapsock.h"
#include "filesync.h"

int do_sync(struct client_info* client);
int do_getfile(struct client_info* client);
int sync_to_client(struct client_info* client);

int main(){
    
    // Use init to clear the clients array
    init();
    // debug
    display_clients();

    // Create a socket
    int listen_sock = Socket(PF_INET, SOCK_STREAM, 0);
    
    // Boilerplate to allow server rerunning
    int optval = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
        (const void *)&optval , sizeof(int));

    // Set up the address/port information
    struct sockaddr_in listen_addr;
    listen_addr.sin_family = PF_INET;
    listen_addr.sin_port = htons(PORT);
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(listen_addr.sin_zero), '\0', sizeof(listen_addr.sin_zero));
    // Bind socket to address/port
    Bind(listen_sock, (struct sockaddr*) &listen_addr, sizeof(listen_addr));
    // Setup socket for listening
    Listen(listen_sock, MAXCLIENTS);

    // Init master_set (from filedata.h)
    FD_ZERO(&master_set);
    // Initially only read from listen_sock and stdin
    // (stdin needed to provide a way to exit the program)
    FD_SET(listen_sock, &master_set);
    FD_SET(fileno(stdin), &master_set);
    
    // read_set will be the copy of master_set actually used in select
    // (necessary because select modifies the fd_set when it returns)
    fd_set read_set;

    // Initialize the largest fd number we are interested in + 1
    int nfds = listen_sock + 1;

    printf("Server started. Press enter to shut down.\n");

    // Main loop
    while(1){ 
        // Copy master_set to read_set
        read_set = master_set;

        // Wait for something to be available to read
        // n_socks is the number of sockets available to read
        int n_socks = Select(nfds, &read_set, NULL, NULL, NULL);

        // We shoud read all the available sockets before calling select again
        // Do this by looping through all the fds until we have read all the 
        // sockets available to read.
        //(loop from highest to lowest, since it's more likely that our sockets
        // have high numbers)
        int sock;
        for(sock = (nfds-1); (sock >= 0) && (n_socks > 0); sock--){
            // Check if the current fd is in read_set
            if(FD_ISSET(sock, &read_set)){
                // we will be reading this socket, so decrement n_socks
                n_socks--;

                // we might need to access a client
                struct client_info* client; 

                // If the current socket is the listening socket
                if(sock == listen_sock){
                    // Accept the incoming connection and assign it to an fd
                    // The client address is NULL because we don't need it
                    int incoming = Accept(listen_sock, NULL, NULL);
                    
                    // Add this new socket to the master set and update nfds
                    FD_SET(incoming, &master_set);
                    if(incoming >= nfds){
                        nfds = incoming + 1;
                    }
                }
                // If the current socket is stdin, we should exit the program 
                else if(sock == fileno(stdin)){
                    // Read in and ignore anything in stdin
                    while(fgetc(stdin) != '\n');

                    // Close all sockets we were connected to
                    int i;
                    for(i=0; i<nfds; i++){
                        if(FD_ISSET(i, &master_set)){
                            close(i);
                        }
                    }
                    printf("Server successfully shut down\n");
                    return 0;
                }
                // If the current socket can be found in the client array, 
                // it is a sync message or a chunk of a file
                else if((client = find_client_by_socket(sock)) != NULL){
                    // Do sync or getfile depending on state of the client
                    if(client->STATE == SYNC){
                        // see do_sync function for details
                        int ret = do_sync(client);
                        if(ret == -1){
                            fprintf(stderr, "error sychronizing with client\n");
                            remove_client(client);
                            continue;
                        }
                        else if(ret == -2){
                            fprintf(stderr, "Client %s disconnected\n", client->userid);
                            remove_client(client);
                            continue;
                        }
                    }
                    else if(client->STATE == GETFILE){

                        // Recieve a chunk from socket and append into file
                        int ret = recv_chunk(client->sock, client->dirname,
                                             client->file_in_transit, client->filesize, &(client->bytes_left));
                        // int ret = do_getfile(client);
                        if(ret == -1){
                            fprintf(stderr, "error getting file from client\n");
                            remove_client(client);
                            continue;
                        }
                        else if(ret == -2){
                            fprintf(stderr, "Client %s disconnected\n", client->userid);
                            remove_client(client);
                            continue;
                        }

                        // Check if file transfer finished
                        if(client->bytes_left == 0){
                            // Set client state back to SYNC
                            client->STATE = SYNC;
                        }
                    }
                    else{
                        fprintf(stderr, "Invalid client state!\n");
                        remove_client(client);
                        continue;
                    }
                }
                // Otherwise, it must be a login message
                else{
                    // Read a login_message from the socket
                    struct login_message new_login;
                    int n_read = read(sock, &new_login, sizeof(new_login));
                    if(n_read == -1){
                        perror("read");
                        continue;
                    }
                    else if(n_read < sizeof(new_login)){
                        fprintf(stderr, "login failed\n");
                        // read less than expected, the socket might have
                        // closed or it's not a login_message
                        // Remove this socket from the master set of fds
                        FD_CLR(sock, &master_set);
                        // close the socket
                        close(sock);
                        // (no need to remove from clients array, since
                        // it's not added to it yet)
                        continue;
                    }
                    
                    // Add the client to the clients array
                    if(add_client(new_login, sock) == -1){
                        continue;
                    }

                    printf("Client logged in:");
                    display_clients();
                }
            }
        }
    }
}

// Read and reply to a sync_message from a client
// If a file needs to be sent to the client, send it
// If a file needs to be recieved from the client,
// set the client state to GETFILE
// Return -1 for error
// Return -2 for socket closed
int do_sync(struct client_info* client){
    struct sync_message msg;
    int bytes_read;
    // Read a sync_message from the client's socket
    bytes_read = read(client->sock, &msg, sizeof(msg));
    if(bytes_read == -1){
        // error
        perror("read");
        return -1;
    }
    else if(bytes_read == 0){
        // Socket closed by client
        return -2;
    }
    else if(bytes_read != sizeof(msg)){
        fprintf(stderr, "invalid sync message\n");
        return -1;
    }
    fprintf(stderr, "Recieved sync message: %s %d %d\n", msg.filename, (int)msg.mtime, (int)msg.size);

    // Check if the message is empty
    if(is_empty_msg(msg)){
        // Sync any new files to the client
        return sync_to_client(client);
    }
    
    // Find the file info entry this sync message is referring to
    struct file_info* finfo;
    // (check_file also handles adding finfo if it doesnt exit)
    finfo = check_file(client->files, msg.filename);
    if(finfo == NULL){
        fprintf(stderr, "No more space in file list\n");
        return -1;
    }
    // Put the info in the sync msg into the finfo
    strncpy(finfo->filename, msg.filename, MAXNAME);
    finfo->mtime = msg.mtime;

    // gather info about local file
    struct sync_message reply;
    if(init_sync_msg(client->dirname, msg.filename, &reply) == -1){
        fprintf(stderr, "tried to sync a non-regular file!\n");
        return -1;
    }

    // send reply to client
    if(write(client->sock, &reply, sizeof(reply)) == -1){
        perror("write");
        return -1;
    }
    fprintf(stderr, "Sent sync message: %s %d %d\n", reply.filename, (int)reply.mtime, (int)reply.size);

    int ret;
    // Decide if need to send or recieve the file
    if(reply.mtime > msg.mtime){
        // Server version is newer, need to send file to client
        ret = send_file(client->sock, client->dirname, reply.filename, reply.size);
        if(ret < 0){ // deal with errors
            return ret;
        }
    }
    else if(reply.mtime < msg.mtime){
        // Client version is newer, need to recieve file from client 
        // set the client's file_in_transit, bytes_left, and stat
        strncpy(client->file_in_transit, msg.filename, MAXNAME);
        client->file_in_transit[MAXNAME-1] = '\0';
        client->filesize = msg.size; // size of file in transit
        client-> bytes_left = msg.size;
        client->STATE = GETFILE;
    }

    return 0;
}

// If there is a file on the server that is not on the client,
// Reply with non-empty sync_message, update client file list,
// and send the file to the client
// Else, send an empty sync_message
int sync_to_client(struct client_info* client){
    // TODO
    // loop through files in client->dirname using opendir and readdir
        // Check if they exist in client->filelist by
        // looping through it and comparing the file names
        // If it's not found, send it to client using send_file, return
    // if exit loop without finding a file to send, send empty sync message

    // Dummy code:
    fprintf(stderr, "empty sync message\n");
    // send an empty sync message
    struct sync_message msg;
    if(init_sync_msg(NULL, NULL, &msg) == -1){
        return -1;
    }
    if(write(client->sock, &msg, sizeof(msg)) != sizeof(msg)){
        return -1;
    }
    fprintf(stderr, "sent empty sync message\n");
    return 0;
}

// Read a chunk of a file from the client
// Open the local version of the file and append the new data to it
// Return -1 for error
// Return -2 for socket closed
int do_getfile(struct client_info* client){
    // Allocate memory to store a chunk
    void* buf;
    if((buf = malloc(CHUNKSIZE)) == NULL){
        perror("malloc");
        return -1;
    }
    
    // Read a chunk from the client's socket
    int bytes_read = read(client->sock, buf, CHUNKSIZE);

    if(bytes_read == -1){
        // error
        perror("read");
        free(buf);
        return -1;
    }
    else if(bytes_read == 0 && client->bytes_left != 0){
        // Socket closed by client
        free(buf);
        return -2;
    }
    else if(bytes_read != CHUNKSIZE && bytes_read != client->bytes_left){
        // got unexpected number of bytes, could be an error, but still try to wrte it.
        fprintf(stderr, "unexpected number of bytes during file transfer\n");
    }
    
    // Open the local file to append to
    // If dosen't exist, create it
    int appendfd = open(client->file_in_transit, O_WRONLY | O_APPEND | O_CREAT);
    
    // Write the bytes read to the file
    if(write(appendfd, buf, bytes_read) == -1){
        // error
        perror("write");
        free(buf);
        close(appendfd);
        return -1;
    }

    // Update the bytes_left
    client->bytes_left -= bytes_read;
    
    // Cleanup
    close(appendfd);
    free(buf);
    return 0;
}

