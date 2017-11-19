#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

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
            if(i < 4) {
                int time = 30;
                char buff[30];
                sprintf(buff,"Child %d: Hello!\n",i);
                printf("%s\n : time %d\n",buff,time);
                while(time > 0) {
                    write(curr_pipe[1],buff,strlen(buff)+1);
                    int sleep_time = rand()%3;
                    time -= sleep_time;
                    printf("Child %d sleeping\n",i);
                    sleep(sleep_time);
                }
            } else { // 5th special child
                int time = 30;
                char buff[50],inp[30];
                while(time > 0) {
                    gets(inp);
                    sprintf(buff,"Child %d: %s\n",i,buff);
                    write(curr_pipe[1],buff,strlen(buff)+1);
                    int sleep_time = rand()%3;
                    time -= sleep_time;
                    printf("Child %d sleeping\n",i);
                    sleep(sleep_time);
                }
            }
            close(curr_pipe[1]);
            printf("child %d exiting\n",i);
            return 0; // child process exits
        } else if(rt > 0) { // parent process
            child_pid[i] = rt;
            close(curr_pipe[1]);
            printf("Parent expecting data on %d for child %d\n",curr_pipe[0],rt);
        } else {
            printf("Error encountered while creating child %d; Exiting\n",i);
            return -1;
        }
    }
    printf("All child processes created\n");
    assert(par_pid == getpid());

    //printf("parent here\n");
    //wait(0);
    //return 0;
    if(par_pid == getpid()) {
        fd_set base,inputfds;
        FD_ZERO(&base);
        for(int i=0;i<5;i++) FD_SET(arr[i].fd[0],&base);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = (500000);
        char buffer[100];
        for(;;) {
            inputfds = base;
            int done = 0;
            for(int i=0;i<5;i++) {
                if(kill(child_pid[i],0) == ESRCH) done++;
            }
            if(done == 5) return 0; // all child processes have terminated
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
                        printf("%s : (%d bytes)\n",buffer,nread);
                    }
                }
            }
        }
    }
}


