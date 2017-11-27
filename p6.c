#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

typedef struct {
    int fd[2];
} pipe_st; 

FILE *fd;

char* timestamp()
{
    struct timeval tv;
    struct tm* ptm;
    char time_string[40];
    long milliseconds;
    char *time_s = (char *)malloc(40);

    gettimeofday (&tv, NULL);
    ptm = localtime (&tv.tv_sec);
    strftime (time_string, sizeof (time_string), "0:0:%S", ptm);
    milliseconds = tv.tv_usec / 1000;

    sprintf (time_s, "%s.%03ld", time_string, milliseconds);
    return time_s;
}

void writeTofile(char *buffer) {
    fd = fopen("output.txt", "a+");

    if(fd == NULL) {
        printf("Error while opening output file for writing\n");
        return;
    }

    char *ts_buff = timestamp();
    fprintf(fd, "%s : %s\n", ts_buff, buffer);
    free(ts_buff);
    fclose(fd);
}

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
            printf("Child %d with pid %d created\n",i,getpid());
            if(i < 4) {
                close(0);
                close(curr_pipe[0]);
                char buff[500] = {0};
                while(1) {
                    char *ts_buff = timestamp();
                    sprintf(buff,"%s :Child %d: Hello!\n",ts_buff, i);
                    free(ts_buff);
                    write(curr_pipe[1],buff,strlen(buff)+1);
                    int sleep_time = rand()%3;
                    //printf("Child %d sleeping\n",i);
                    sleep(sleep_time);
                }
            } else { // 5th special child
                char buff[500] = {0}, inp[60] = {0};
                while(1) {
                    gets(inp);
                    char *ts_buff = timestamp();
                    sprintf(buff,"%s :Child %d: %s\n",ts_buff,i,inp);
                    free(ts_buff);
                    write(curr_pipe[1],buff,strlen(buff)+1);
                    int sleep_time = rand()%3;

                    //printf("Child %d sleeping\n",i);
                    sleep(sleep_time);
                }
            }
        } else if(rt > 0) { // parent process
            child_pid[i] = rt;
            close(curr_pipe[1]);
            //printf("Parent expecting data on %d for child %d\n",curr_pipe[0],rt);
        } else {
            //printf("Error encountered while creating child %d; Exiting\n",i);
            return -1;
        }
    }
    printf("All child processes created\n");
    assert(par_pid == getpid());

    //printf("parent here\n");
    //wait(0);
    //return 0;
    if(par_pid == getpid()) {
        close(0);
        time_t start_time = time(0);

        fd_set base,inputfds;
        FD_ZERO(&base);
        for(int i=0;i<5;i++) FD_SET(arr[i].fd[0],&base);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = (500000);
        char buffer[500];
        for(;;) {
            int curr_time = time(0);
            if(curr_time > start_time + 30) {
                for(int i=0;i<5;i++) kill(child_pid[i],SIGKILL);
                return 0;
            }
            inputfds = base;
            int done = 0;
            int rv = select(FD_SETSIZE,&inputfds,0,0,&timeout);
            if(rv < 0) {
                printf("Failed on select\n");
                return -1;
            } else if (rv == 0) {
                //printf("Timeout occured\n");
            } else {
                for(int i=0;i<5;i++) {
                    if(FD_ISSET(arr[i].fd[0],&inputfds)) {
                        //printf("File desc. %d ready\n",arr[i].fd[0]);
                        int nread = read(arr[i].fd[0],buffer,100);
                        buffer[nread] = 0;
                        
                        // Write to output.txt;
                        writeTofile(buffer);
                        //printf("%s\n",buffer);
                    }
                }
            }
        }
    }
    return 0;
}


