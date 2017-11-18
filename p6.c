#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int fd[2];
} pipe_st; 

int main() {
    pipe_st arr[5]; // 5 pipe structures to communicate with 5 child processes
    pid_t child_pid[5];
    pid_t par_pid = getpid();
    srand(0);
    for(int i=0;i<5;i++) {
        int* curr_pipe = arr[i].fd;
        pipe(curr_pipe);
        pid_t rt = fork();
        if(rt == 0) { // child process
            close(curr_pipe[0]);
            printf("Child %d with pid %d created\n",i,getpid());
            int time = 30;
            if(i < 4) {
                char buff[10];
                sprintf(buff,"Child %d: Hello!\n",i);
                while(time > 0) {
                    write(curr_pipe[1],buff,strlen(buff)+1);
                    int sleep_time = rand()%3;
                    time -= sleep_time;
                    sleep(sleep_time);
                }
            } else { // 5th special child
                char buff[10],inp[20];
                while(time > 0) {
                    gets(inp);
                    sprintf(buff,"Child %d: %s\n",i,buff);
                    write(curr_pipe[1],buff,strlen(buff)+1);
                    int sleep_time = rand()%3;
                    time -= sleep_time;
                    sleep(sleep_time);
                }
            }
        } else if(rt > 0) { // parent process
            child_pid[i] = rt;
            close(curr_pipe[1]);
        } else {
            printf("Error encountered while creating child %d; Exiting\n",i);
            return -1;
        }
    }
    if(par_pid == getpid()) {
        fd_set inputfds;
        FD_ZERO(&inputfds);
        for(int i=0;i<5;i++) FD_SET(arr[i].fd[0],&inputfds);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = (500000);
        char buffer[100];
        for(;;) {
            int rv = select(FD_SETSIZE,&inputfds,0,0,&timeout);
            if(rv < 0) {
                printf("Failed on select\n");
                return -1;
            } else if (rv == 0) {
                printf("Timeout occured\n");
            } else {
                for(int i=0;i<5;i++) {
                    if(FD_ISSET(arr[i].fd[0],&inputfds)) {
                        printf("File desc. %d ready\n",arr[i].fd[0]);
                        int nread = read(arr[i].fd[0],buffer,100);
                        buffer[nread] = 0;
                        printf("%s\n",buffer);
                    }
                }
            }
        }
    }   
}


