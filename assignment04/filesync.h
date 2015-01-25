#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "message.h"

// functions useful for syncing files between client and server

int list_files(char* dirname, char* filelist);

void file_to_path(char* path, char* dirname, char* filename);

int init_sync_msg(char* dirname, char* filename, struct sync_message* msg);

int is_empty_msg(struct sync_message msg);

int send_file(int sock, char* dirname, char* filename, size_t filesize);

int recv_chunk(int sock, char* dirname, char* filename, size_t file_size, size_t* bytes_left);
