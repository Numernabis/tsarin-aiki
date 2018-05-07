# Procesy

## Fork

```c
#include <sys/types.h>
#include <unistd.h>
pid_t fork(void);
```
fork() creates a new process by duplicating the calling process.  The
  new process is referred to as the child process.  The calling process
  is referred to as the parent process. The child process and the parent
  process run in separate memory spaces.

```c
pid_t getpid(void);
```  
getpid() returns the process ID (PID) of the calling process.

## Exec

```c
int execvp(const char *file, char *const argv[]);
```

The exec() family of functions replaces the current process image
with a new process image.  The functions described in this manual
page are front-ends for execve(2).
The execv(), execvp(), and execvpe() functions provide an array of
pointers to null-terminated strings that represent the argument list
available to the new program.  The first argument, by convention,
should point to the filename associated with the file being executed.
The array of pointers must be terminated by a null pointer.

## Wait

```c
#include <sys/wait.h>
pid_t waitpid(pid_t pid, int *stat_loc, int options);
```

The wait() and waitpid() functions shall obtain status information
pertaining to one of the caller's child processes.

```c
WEXITSTATUS(stat_val)
```

If the value of WIFEXITED(stat_val) is non-zero, this macro
evaluates to the low-order 8 bits of the status argument that
the child process passed to \_exit() or exit(), or the value the
child process returned from main().
