#define _GNU_SOURCE
#include <err.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>

static int              /* Start function for cloned child */
childFunc(void *arg)
{
    sleep(1);
    printf("I am inside child thread \n");
    printf("Child: thread id is %d\n", gettid());
    printf("Child: parent pid is %d\n", getpid());
    sleep(2);

    while (1); 
    return 0;           /* Child terminates now */
}

#define STACK_SIZE (1024 * 1024)    /* Stack size for cloned child */

int
main(int argc, char *argv[])
{
    char            *stack;         /* Start of stack buffer */
    char            *stackTop;      /* End of stack buffer */
    pid_t           pid;

    /* Allocate memory to be used for the stack of the child. */

    stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED)
        err(EXIT_FAILURE, "mmap");

    stackTop = stack + STACK_SIZE;  /* Assume stack grows downward */

    // Disable the buffer for printf 
    setbuf(stdout, NULL);

    int clone_flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_PARENT_SETTID | SIGCHLD;
    pid = clone(childFunc, stackTop, clone_flags, NULL);
    if (pid == -1)
        err(EXIT_FAILURE, "clone");

    printf("clone() returned %d\n", pid);

    printf("Waiting for thread to finish execution...\n");
    sleep(5);

    printf("I am inside parent thread \n");
    printf("Parent: thread id is %d\n", gettid());
    printf("Parent: process id is %d\n", getpid());

    /* Parent falls through to here */

    sleep(1);           /* Give child time to change its hostname */

    while(1);

    if (waitpid(pid, NULL, 0) == -1)    /* Wait for child */
        err(EXIT_FAILURE, "waitpid");

    printf("child has terminated\n");

    exit(EXIT_SUCCESS);
}