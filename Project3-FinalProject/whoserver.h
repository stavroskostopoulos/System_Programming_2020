
/* Stavros Kostopoulos sdi1700068 */

#ifndef SERVER
#define SERVER

#define BFSIZE 11

typedef struct workerfd_list{
    int fd;
    struct workerfd_list* next;
    pthread_mutex_t mtx;
}workerfd_list;

typedef workerfd_list* workerfd_listPtr;

typedef struct{
    int* fds;
    int start;
    int end;
    int count;
}pool_t;

typedef pool_t* poolPtr;

pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
poolPtr pool;

//Circular buffer (pool) - Apo tis diafaneies tis selidas tou mathimatos
void initialize(poolPtr pool, int buffersize);
void place(poolPtr pool, int data, int buffersize);
int obtain(poolPtr pool, int buffersize);



int network_accept_any(int fds[], unsigned int count,  struct sockaddr *addr, socklen_t *addrlen);//A function to listen from multiple ports using select()


//A function to create a worker fd list node
workerfd_listPtr create_list_node(int fd);

//A function to attach a node in the worker fd list
void attach_fd_to_list(workerfd_listPtr root, workerfd_listPtr newnode);

//A function to destroy the worker fd list
void destroy_fd_list(workerfd_listPtr root);

//A function to print the content of the worker fd list
void print_fd_list(workerfd_listPtr root);




#endif
