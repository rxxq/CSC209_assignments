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
#include <netdb.h>

#include "wrapsock.h"
#include "message.h"
#include "filesync.h"

int main(int argc, char* argv[]){

/*
    // Get a list of files in directory
    char filelist[MAXFILES][MAXNAME];
    list_files("my_dir", filelist);
    printf("%s\n%s\n", filelist[0], filelist[1]);
*/

    /* Get info from argv
    1. address of the server (eg: greywolf)
    2. port of the server (use 59449)
    3. userID
    4. directory to synchronize
    5. synchronization interval in seconds
    */
    if(argc != 6){
        printf("invalid parameters\n");
        printf("usage: dbclient servername port userID directory syncinterval\n");
        printf("example: dbclient greywolf 59449 my_name my_dir 10\n");
        return 1;
    }
    
    char* server_name = argv[1];
    int port = atoi(argv[2]);
    char* userid = argv[3];
    char* dirname = argv[4];
    int sync_interval = atoi(argv[5]);
    
    int sock;
    struct hostent* server_hostent;
    struct sockaddr_in server_address;
    
    // get info about the server address
    if((server_hostent = gethostbyname(server_name)) == NULL){
        perror("gethostbyname");
        return 1;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr = *((struct in_addr *)server_hostent->h_addr);
    
    // create socket
    sock = Socket(PF_INET, SOCK_STREAM, 0);
    
    // connect to the server
    Connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));

    // Create a login message
    struct login_message login;
    strncpy(login.userid, userid, MAXNAME);
    strncpy(login.dir, dirname, MAXNAME);

    // Send the login message
    if(write(sock, &login, sizeof(login)) == -1){
        perror("write");
        return 1;
    }
    
    // variables used in main loop
    DIR* dirstream;
    struct dirent* entry;
    char filename[MAXNAME];
    struct sync_message msg;
    struct sync_message reply;
    int ret;

    // Main loop (runs every sync_interval seconds)
    do{
        // Open a directory stream
        if((dirstream = opendir(dirname)) == NULL){
            perror("opendir");
            return -1;
        }
        // For each file in the directory
        while((entry = readdir(dirstream)) != NULL){
            // Get the name of the file
            strncpy(filename, entry->d_name, MAXNAME);
            // Make sure filename is null-terminated
            filename[MAXNAME-1] = '\0';
            
            // Try to make a sync_message for this file
            ret = init_sync_msg(dirname, filename, &msg);
            if(ret < 0){
                // not a regular file or error occured,
                // so skip to the next file
                continue;
            }

            // Send the msg to the server
            if(write(sock, &msg, sizeof(msg)) == -1){
                perror("write");
                continue; //skip to next file
            }

            // Read the reply from the server
            ret = read(sock, &reply, sizeof(reply));
            if(ret == -1){
                perror("read");
                continue; //skip to next file
            }
            else if(ret == 0){
                fprintf(stderr, "Socket closed by the server\n");
                close(sock);
                closedir(dirstream);
                return 1;
            }
            printf("reply: %s %d %d\n", reply.filename, (int)reply.mtime, (int)reply.size);

            // Decide if need to send or recieve the file
            if(reply.mtime > msg.mtime){
                // Server version is newer, need to recieve file from server
                size_t bytes_left = reply.size;
                // Keep recieving chunks until no more bytes to recieve
                while(bytes_left > 0){
                    ret = recv_chunk(sock, dirname, reply.filename, reply.size, &bytes_left);
                    if(ret < 0){ // deal with errors
                        //TODO
                        return 1;
                    }
                }
            }
            else if(reply.mtime < msg.mtime){
                // Client version is newer, need to send file to server
                ret = send_file(sock, dirname, msg.filename, msg.size);
                if(ret < 0){ // deal with errors
                    //TODO
                    return 1;
                }
            }

        }
        // cleanup
        closedir(dirstream);
        
        // Get new files from the server
        // Loop until an empty reply recieved from the server
        while(1){
            // init an empty sync message
            // Not checking return val since this function was not programmed
            // to return error when initing an empty message
            init_sync_msg(NULL, NULL, &msg);

            // Send the empty sync message
            if(write(sock, &msg, sizeof(msg)) == -1){
                perror("write");
                continue; // try sending empty msg again
            }
            
            // Read the reply from the server
            ret = read(sock, &reply, sizeof(reply));
            if(ret == -1){
                perror("read");
                continue; // try sending empty msg again
            }
            else if(ret == 0){
                fprintf(stderr, "Socket closed by the server\n");
                close(sock);
                return 1;
            }
            // If the reply was empty too, exit this loop
            else if(is_empty_msg(reply)){
                break;
            }
            
            // reply was not empty, prepare to recieve the fileO
            size_t bytes_left = reply.size;
            // Keep recieving chunks until no more bytes to recieve
            while(bytes_left > 0){
                ret = recv_chunk(sock, dirname, reply.filename, reply.size, &bytes_left);
                if(ret < 0){ // deal with errors
                    //TODO
                    return 1;
                }
            }
        }

    }while(sleep(sync_interval) == 0);
    // ^ If slept for the whole duration should return 0


    close(sock);
    return 0;
}
