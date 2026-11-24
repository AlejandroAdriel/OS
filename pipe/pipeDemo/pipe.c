/*****************************************************************************
 Excerpt from "Linux Programmer's Guide - Chapter 6"
 (C)opyright 1994-1995, Scott Burkett
 ***************************************************************************** 
 MODULE: pipe.c
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>


int c;


int main(void)
{
        int     fd[2], nbytes;
        pid_t   childpid;
        char    string[] = "Hello, world!\n";
        char    readbuffer[80];
        int c;

        pipe(fd);

        // 0 hijo  , 1 papa, -1 error

        printf("Main program:"); 
        c=getchar();
        printf("M %c\n",c);


        if((childpid = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }
 
        if(childpid == 0)
        {
                int c;
                printf("Child program:"); 
                //c=getchar();
                //printf("C %c\n",c);
                /* Child process closes up input side of pipe */
                close(fd[0]); //close input 

                /* Send "string" through the output side of pipe */
                write(fd[1], string, (strlen(string)+1));
                exit(0);
        }
        else
        {
                int c;
                printf("Parent program:"); 
                //c=getchar();
                //printf("P %c\n",c);
                /* Parent process closes up output side of pipe */
                close(fd[1]); //close output

                /* Read in a string from the pipe */
                nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
                printf("Received string: %s", readbuffer);
        }

        return(0);
}

//problema raise condition cuando lanzas desde la misma terminal

