// ! FOR COMPILEING THE CODE: gcc zamanOlc.c -o zamanOlc
// ! TO RUN EXECUTEABLE PROGRAM: ./zamanOlc <terminal command> <parameter (optional)>
/* * * * * * * * * * * * * * *
########  ######## 
#       # #
########  ########
#       # #      #
########  ########

Coded by Berat Göktuğ Özdemir
02 November 2019
* * * * * * * * * * * * * * */

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>

// # Helper Methods # \\
unsigned long getTimeMS();
char *concatenate(const char *, const char *);

int main(int argc, char **argv)
{
    // * Size of allocated Shared Memory
    const int SIZE = 4096;

    // * Name of Shared Memory Object and name of the binary folder
    const char *name = "SM", *binaryPath = "/bin/";

    // * Shared Memory File Descriptor
    int fd;

    // * Pointer of the Shared Memory Object
    char *ptr;

    // * Process ID
    pid_t pid;

    if (argc < 2)
    {
        fprintf(stderr, "USAGE: ./<output name> <command> <parameter>(opt.)\n");
        return 1;
    }

    // * Create a new Child Process
    pid = fork();

    if (pid < 0) // * Error occurred
    {
        fprintf(stderr, "Fork Failed!\n");

        return 1;
    }
    else if (pid == 0) // * Child Process
    {
        // ? Create the Shared Memory Object
        fd = shm_open(name, O_CREAT | O_RDWR, 0666);

        // ? Configure the size of the Shared Memory Object
        ftruncate(fd, SIZE);

        // ? Memory map the Shared Memory Object
        ptr = (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        // ? Write time to the Shared Memory Object
        sprintf(ptr, "%lu", getTimeMS());

        // ? Concatenate the given arguments in a single string
        char *path = concatenate(binaryPath, argv[1]);

        execv(path, argv + 1);

        // ? Remove the string of argument
        free(path);

        fprintf(stderr, "execv() failed\n");
        exit(1);
    }
    else // * Parent Process
    {
        // ? The Parent Process will wait until the Child Process is completed.
        wait(NULL);
        fd = shm_open(name, O_CREAT | O_RDWR, 0666);

        // ? Configure the size of the Shared Memory Object
        ftruncate(fd, SIZE);

        // ? Memory map the Shared Memory Object
        ptr = (char *)
            mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        // ? Calculate the Elapsed Time
        unsigned long startTime = strtoul((char *)ptr, NULL, 10);
        unsigned long stopTime = getTimeMS();

        printf("Geçen süre %lu ms' dir\n", stopTime - startTime);

        // ? Remove The shared Memory Object
        shm_unlink(name);

        return 0;
    }
}

// * Returns the time in type of milliseconds(ms)
unsigned long getTimeMS()
{
    // ? tv_sec = seconds
    // ? tv_usec = microseconds

    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (tv.tv_usec / 1000) + (tv.tv_sec * 1000);
}

// * Returns a string including parameters in a line
char *concatenate(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}
