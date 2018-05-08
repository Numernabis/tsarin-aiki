# Systemy operacyjne - laboratorium
> ``#define QUESTION ((bb) || !(bb))``   — Shakespeare

## Materiały
[Starożytny skrypt manuala PL](http://www.linux.pl/man/)

[Starożytny skrypt manuala EN](http://man7.org/linux/man-pages/dir_all_alphabetic.html)

## Spis treści
1. [Zestaw 1 - biblioteki](## Biblioteki)
2. [Zestaw 2 - pliki](## Pliki)
3. [Zestaw 3 - procesy](## Procesy)
4. [Zestaw 4 - sygnały](## Sygnały)
5. [Zestaw 5 - strumienie](## Strumienie)
---

## Biblioteki
### Dynamiczne
```c
#include <dlfcn.h>
void* dlopen(const char *filename, int flag);
```
Otwiera bibliotekę, przygotowuje ją do użycia i zwraca wskaźnik/uchwyt na bibliotekę.
```c
void* dlsym(void *handle, char *symbol);
```
Przegląda bibliotekę  szukając specyficznego symbolu.
```c
void dlclose();
```
Zamyka bibliotekę.
---

## Pliki
### Funkcje systemowe
```c
<fcntl.h>
<unistd.h>
<sys/types.h>
<sys/stat.h>

int open(const char *pathname, int flags[, mode_t mode]);
int creat(const char *pathname, mode_t mode);
int read(int fd, void *buf, size_t count);
int write(int fd, void *buf, size_t count);
long lseek(int fd, off_t offset, int whence);
int close(int fd);
```

### Funkcje biblioteki standardowej wejścia-wyjścia
```c
FILE * fopen( const char * filename, const char * mode );
size_t fread(void * ptr, size_t size, size_t count, FILE * file);
size_t fwrite(const void * ptr, size_t size, size_t count, FILE * file);
int fseek(FILE * file, long int offset, int mode);
int fsetpos(FILE* file, fpos_t* pos);
int fgetpos(FILE* file, fpos_t* pos);
int fclose (FILE * stream);
```

### Katalogi
```c
#include <dirent.h>
DIR* opendir(const char* dirname);
int closedir(DIR* dirp)
struct dirent* readdir(DIR* dirp)
void rewinddir(DIR* dirp)
void seekdir(DIR* dirp, long int loc)

int mkdir (const char *path, mode_t mode);
int rmdir (const char *path);
int chdir (const char *path);
char *getcwd (char *folder_name, ssize_t size);
int chmod (const char *path, mode_t new_mode);
int chown (const char *path, uid_t id_owner, gid_t id_group);
int link (const char *path, const char *new_path);
```

### Staty
```c
int stat (const char *path, struct stat *buf);
int lstat(const char *path, struct stat *buf);
int nftw(const char *dir, int(*fn) (const char *,const struct stat *, int, struct FTW *), int nopend, int flags)

struct stat {
   dev_t     st_dev;         // ID of device containing file
   ino_t     st_ino;         // Inode number
   mode_t    st_mode;        // File type and mode
   nlink_t   st_nlink;       // Number of hard links
   uid_t     st_uid;         // User ID of owner
   gid_t     st_gid;         // Group ID of owner
   dev_t     st_rdev;        // Device ID (if special file)
   off_t     st_size;        // Total size, in bytes
   blksize_t st_blksize;     // Block size for filesystem I/O
   blkcnt_t  st_blocks;      // Number of 512B blocks allocated
   time_t    st_atim;        // Time of last access
   time_t    st_mtim;        // Time of last modification
   time_t    st_ctim;        // Time of last status change
};
```
---

## Procesy

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

```c
int execl(char const *path, char const *arg0, ...)
//funkcja jako pierwszy argument przyjmuje ścieżkę do pliku, następne są argumenty wywołania funkcji, gdzie arg0 jest nazwą programu
int execle(char const *path, char const *arg0, ..., char const * const *envp)
//podobnie jak execl, ale pozwala na podanie w ostatnim argumencie tablicy ze zmiennymi środowiskowymi
int execlp(char const *file, char const *arg0, ...)
//również przyjmuje listę argumentów ale, nie podajemy tutaj ścieżki do pliku, lecz samą jego nazwę, zmienna środowiskowa PATH zostanie przeszukana w celu zlokalizowania pliku
int execv(char const *path, char const * const * argv)
//analogicznie do execl, ale argumenty podawane są w tablicy
int execve(char const *path, char const * const *argv, char const * const *envp)
//analogicznie do execle, również argumenty przekazujemy tutaj w tablicy tablic znakowych
int execvp(char const *file, char const * const *argv)
```
The exec() family of functions replaces the current process image
with a new process image.  The functions described in this manual
page are front-ends for execve(2).
The execv(), execvp(), and execvpe() functions provide an array of
pointers to null-terminated strings that represent the argument list
available to the new program.  The first argument, by convention,
should point to the filename associated with the file being executed.
The array of pointers must be terminated by a null pointer.

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

---

## Sygnały

---

## Strumienie

---
