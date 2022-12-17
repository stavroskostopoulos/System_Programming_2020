
/* Stavros Kostopoulos sdi1700068 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include  <fcntl.h>
#include  <error.h>
#include  <sys/wait.h>

#include <pthread.h>


#include<netinet/in.h>/*  for  sockaddr_in  */
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>/*  for  hton* */
#include <netdb.h>

#include "whoserver.h"


int network_accept_any(int fds[], unsigned int count,  struct sockaddr *addr, socklen_t *addrlen){//A function to listen from multiple ports using select()

    fd_set acceptfds;
    int maxfd, fd;
    unsigned int i;
    int status;

    FD_ZERO(&acceptfds);

    maxfd = -1;

    for (i = 0; i < count; i++) {
        FD_SET(fds[i], &acceptfds);
        if (fds[i] > maxfd)
            maxfd = fds[i];
    }

    status = select(maxfd + 1, &acceptfds, NULL, NULL, NULL);

    if (status < 0)
        return -1;

    fd = -1;

    for (i = 0; i < count; i++){
        if (FD_ISSET(fds[i], &acceptfds)) {
            fd = fds[i];
            break;
        }
    }

    if (fd == -1){
        return -1;
    }else{
        return accept(fd, addr, addrlen);
    }

}




void initialize(poolPtr pool, int buffersize){
	pool->fds = (int*)malloc(buffersize*sizeof(int));//create the slots of the pool
	pool->start = 0;
	pool->end = -1;
	pool->count = 0;
}

void place(poolPtr pool, int data, int buffersize){//place an element in the circular buffer(pool)
	pthread_mutex_lock(&mtx);
	while (pool->count >= buffersize) {
		//printf(">> Found Buffer Full \n");
		pthread_cond_wait(&cond_nonfull, &mtx);
	}
	pool->end = (pool->end + 1) % buffersize;
	pool->fds[pool->end] = data;
	pool->count++;
	pthread_mutex_unlock(&mtx);
}

int obtain(poolPtr pool, int buffersize){//Obtain an element from the circular buffer(pool)
	int data = 0;
	pthread_mutex_lock(&mtx);
	while (pool->count <= 0) {
		//printf(">> Found Buffer Empty \n");
		pthread_cond_wait(&cond_nonempty, &mtx);
		}
	data = pool->fds[pool->start];
	pool->start = (pool->start + 1) % buffersize;
	pool->count--;
	pthread_mutex_unlock(&mtx);
	return data;
}



//A function to create a worker fd list node
workerfd_listPtr create_list_node(int fd){
    workerfd_listPtr newNode;
    newNode = (workerfd_listPtr)malloc(sizeof(workerfd_list));

    newNode->fd = fd;

    newNode->next=NULL;

    pthread_mutex_init(&(newNode->mtx), NULL);

    return newNode;
}

//A function to attach a node in the worker fd list
void attach_fd_to_list(workerfd_listPtr root, workerfd_listPtr newnode){
    workerfd_listPtr temp=root;
    while( temp->next != NULL){//Go to the last node of the list

        temp = temp->next;

    }
    temp->next = newnode;

}

//A function to destroy the worker fd list
void destroy_fd_list(workerfd_listPtr root){

    if(root==NULL) return;

    destroy_fd_list(root->next);
    free(root);

}

//A function to print the content of the worker fd list
void print_fd_list(workerfd_listPtr root){

    if(root==NULL) return;

    print_fd_list(root->next);

    printf("%d\n", root->fd);

}
