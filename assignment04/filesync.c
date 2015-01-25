#include "filesync.h"

// Fill in filelist with an array of strings, consiting of names of files
// in the directory specified by dirname
// Filelist is a two-dimensional array (not array of char*)
// with space for MAXFILES strings of MAXNAME size
// Design note: A 2D array was used because it can be statically allocated
//              There's not much benefit to using dynamic mem allocation
//              Since the length of strings in the rest of the program
//              is assumed to be MAXSIZE anyway.
// Return -1 if error
int list_files(char* dirname, char* filelist){
    // Open a directory stream
    DIR* dir;
    if((dir = opendir(dirname)) == NULL){
        perror("opendir");
        return -1;
    }
    
    // Read each filename and add it to filelist
    struct dirent* entry;
    int i;
    for(i=0; i<MAXFILES && (entry=readdir(dir)) != NULL; i++){
        // Using pointer arithmetic to refer to each row in filelist
        // Since compiler dosen't know this is a 2D array
        strncpy(filelist+(i*MAXNAME), entry->d_name, MAXNAME);
        // Make sure filename is null terminated
        *(filelist+((i*MAXNAME)+MAXNAME-1)) = '\0'; 
    }
    // If loop exited without filling up all rows of filename,
    // Make sure the first empty row starts with '\0'
    if(i<MAXFILES){
        *(filelist+(i*MAXNAME)) = '\0';
    }

    closedir(dir);
    return 0;
}

// Combine dirname and filename to form a path (dirname/filename)
// Fill in path with the result
// Path must be at least [MAXNAME * 2] long
// dirname and filename must be '\0' terminated
void file_to_path(char* path, char* dirname, char* filename){
    strncpy(path, dirname, MAXNAME);
    strncat(path, "/", 2);
    strncat(path, filename, MAXNAME);
}

// Initialize msg with info about the specified file in the specified dir
// If the file cannot be found, init msg with mtime and size set to 0,
// but with filename filled in.
// If filename and dirname are NULL, initialize msg as an empty message
// Return -1 if the file is not a regular file
int init_sync_msg(char* dirname, char* filename, struct sync_message* msg){
    // option to initialize empty message
    if(dirname == NULL && filename == NULL){
        msg->filename[0] = '\0';
        msg->mtime = 0;
        msg->size = 0;
        return 0;
    }

    // Create a path string
    char path[MAXNAME * 2];
    file_to_path(path, dirname, filename);
    
    // Get file info using stat
    struct stat info;
    // If the file can't be found
    if(stat(path, &info) == -1){
        // init msg with mtime and size set to 0, but filename filled in
        strncpy(msg->filename, filename, MAXNAME);
        msg->mtime = 0;
        msg->size = 0;
        return 0;
    }
    // If the file isn't a regular file
    if(!S_ISREG(info.st_mode)){
        return -1;
    }
    // init msg with the necessary info
    strncpy(msg->filename, filename, MAXNAME);
    msg->mtime = info.st_mtime;
    msg->size = info.st_size;

    return 0;
}

// Test if msg is empty
// Return 1 or 0 for True/False
int is_empty_msg(struct sync_message msg){
    if(msg.filename[0] == '\0' && msg.mtime == 0 && msg.size == 0)
        return 1;
    else
        return 0;
}

// Send the entire file in the directory through the socket,
// One chunksize at a time
// Return -1 if error
// Return -2 if socket closed prematurely
int send_file(int sock, char* dirname, char * filename, size_t filesize){
    // Allocate memory to store a chunk
    void* buf;
    if((buf = malloc(CHUNKSIZE)) == NULL){
        perror("malloc");
        return -1;
    }

    // Create a path string
    char path[MAXNAME * 2];
    file_to_path(path, dirname, filename);

    // Open the file to send
    int ftosend = open(path, O_RDONLY);
    if(ftosend == -1){
        perror("open");
        free(buf);
        return -1;
    }

    // Keep track of the number of bytes left to send
    int bytes_left = filesize;
    // Number of bytes read/sent by read/write
    int bytes_read, bytes_sent;

    // Send file in chunks until all bytes sent
    while(bytes_left > 0){
        // Read a chunk from the file
        bytes_read = read(ftosend, buf, CHUNKSIZE);
        if(bytes_read == -1){
            perror("read");
            free(buf);
            close(ftosend);
            return -1;
        }
        else if(bytes_read < CHUNKSIZE && bytes_read < bytes_left){
            fprintf(stderr, "read less than expected from file\n");
            free(buf);
            close(ftosend);
            return -1;
        }

        // Write the chunk to the socket
        bytes_sent = write(sock, buf, bytes_read);
        if(bytes_sent == -1){
            perror("write");
            free(buf);
            close(ftosend);
            return -1;
        }
        else if(bytes_sent < bytes_read){
            fprintf(stderr, "unable to write some bytes to socket\n");
            free(buf);
            close(ftosend);
            return -1;
        }

        // update bytes_left
        bytes_left -= bytes_sent;
    }
    
    close(ftosend);
    free(buf);
    return 0;
}

// Recieve one chunk from the socket, append to the file in the directory
// bytes_left will be decremented by the number of bytes actually read
// Return 0 for normal execution
// Return -1 if error
// Return -2 if socket closed prematurely
int recv_chunk(int sock, char* dirname, char* filename, size_t file_size, size_t* bytes_left){

    // If no bytes left to read of a nonempty file, return immediately
    if(*bytes_left == 0 && file_size != 0){
        return 0;
    }

    // Create a path string
    char path[MAXNAME * 2];
    file_to_path(path, dirname, filename);

    int appendfd;
    // If this is the first chunk recieved, open and overwrite the file
    // If file dosen't exist locally, create it with 700 permission
    if(*bytes_left == file_size){
        appendfd = open(path, O_WRONLY | O_CREAT, S_IRWXU);
    }
    // Otherwise, open the file in append mode
    else{
        appendfd = open(path, O_WRONLY | O_APPEND);
    }

    // if the filesize is zero, it would have been created by this point
    if(file_size == 0){
        close(appendfd);
        return 0;
    }

    // Allocate memory to store a chunk
    void* buf;
    if((buf = malloc(CHUNKSIZE)) == NULL){
        perror("malloc");
        close(appendfd);
        return -1;
    }

    // Read a chunk from the socket
    int bytes_read = read(sock, buf, CHUNKSIZE);

    if(bytes_read == -1){
        // error
        perror("read");
        free(buf);
        close(appendfd);
        return -1;
    }
    else if(bytes_read == 0 && *bytes_left != 0){
        // Socket closed
        free(buf);
        close(appendfd);
        return -2;
    }
    else if(bytes_read != CHUNKSIZE && bytes_read != *bytes_left){
        // got unexpected number of bytes, probably an error
        fprintf(stderr, "unexpected number of bytes during file transfer\n");
        free(buf);
        close(appendfd);
        return -1;
    }
    
    // Write the bytes read to the file
    if(write(appendfd, buf, bytes_read) == -1){
        // error
        perror("write");
        free(buf);
        close(appendfd);
        return -1;
    }

    // Update the bytes_left
    *bytes_left -= bytes_read;

    // Cleanup
    close(appendfd);
    free(buf);
    return 0;
}

