#include <time.h>
#include <unistd.h>
#include "message.h"

// Values for state
#define SYNC 1
#define GETFILE 2

struct file_info {
    char filename[MAXNAME];
    time_t mtime;   /* Last modified time as reporte by the client */
};

struct client_info {
    int sock;
    char userid[MAXNAME];
    char dirname[MAXNAME];
    struct file_info files[MAXFILES];
    int STATE;
    char file_in_transit[MAXFILES]; // name of file currently being sent by client
    size_t filesize; // size of file in transit
    size_t bytes_left; // bytes left in file_in_transit
};
extern struct client_info clients[MAXCLIENTS];

// set of all sockets connected to server
extern fd_set master_set;

void init();
void init_single_client(struct client_info* client);
int add_client(struct login_message s, int socketfd);
void remove_client(struct client_info* client);
struct file_info *check_file(struct file_info *files, char *filename);
struct client_info* find_client_by_socket(int sock);

void display_clients();

