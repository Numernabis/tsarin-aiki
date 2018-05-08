# Systemy operacyjne - laboratorium
> ``#define QUESTION ((bb) || !(bb))``   — Shakespeare

## Materiały
[Starożytny skrypt manuala PL](http://www.linux.pl/man/)

[Starożytny skrypt manuala EN](http://man7.org/linux/man-pages/dir_all_alphabetic.html)

## Spis treści
1. [Zestaw 1 - biblioteki](#biblioteki)
2. [Zestaw 2 - pliki](#pliki)
3. [Zestaw 3 - procesy](#procesy)
4. [Zestaw 4 - sygnały](#sygnały)
5. [Zestaw 5 - strumienie](#strumienie)

---

## Biblioteki
### Dynamiczne
```c
#include <dlfcn.h>
void* dlopen(const char *filename, int flag);
//twiera bibliotekę, przygotowuje ją do użycia i zwraca wskaźnik/uchwyt na bibliotekę.

void* dlsym(void *handle, char *symbol);
//przegląda bibliotekę  szukając specyficznego symbolu.

void dlclose();
//zamyka bibliotekę.
```
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
W momencie jej wywołania tworzony jest nowy proces, będący potomnym dla tego,
w którym właśnie została wywołana funkcja fork. Jest on kopią procesu
macierzystego - otrzymuje duplikat obszaru danych, sterty i stosu
(a więc nie współdzieli danych). Funkcja fork jest wywoływana raz, lecz
zwraca wartość dwukrotnie - proces potomny otrzymuje wartość 0, a proces
macierzysty PID nowego procesu. Jest to konieczne nie tylko ze względu na
możliwość rozróżnienia procesów w kodzie programu: proces macierzysty musi
otrzymać PID nowego potomka, ponieważ nie istnieje żadna funkcja umożliwiająca
wylistowanie wszystkich procesów potomnych. W przypadku procesu potomnego nie
jest konieczne podawanie PID jego procesu macierzystego, ponieważ ten jest
określony jednoznacznie (i można go wydobyć np. za pomocą funkcji getppid).
Z kolei 0 jest bezpieczną wartością, ponieważ jest zarezerwowana dla procesu
demona wymiany i nie ma możliwości utworzenia nowego procesu o takim PID.

Po wywołaniu forka oba procesy (macierzysty i potomny) kontynuują swoje
działanie (od linii następnej po wywołaniu forka czyli efektem kodu:

```c
 pid_t getpid(void) - zwraca PID procesu wywołującego funkcję
 pid_t getppid(void) - zwraca PID procesu macierzystego
 uid_t getuid(void) - zwraca rzeczywisty identyfikator użytkownika UID
 uid_t geteuid(void) - zwraca efektywny identyfikator użytkownika UID
 gid_t getgid(void) - zwraca rzeczywisty identyfikator grupy GID
 gid_t getegid(void) - zwraca efektywny identyfikator grupy GID
```  

```c
int execl(char const *path, char const *arg0, ...)
//funkcja jako pierwszy argument przyjmuje ścieżkę do pliku,
//następne są argumenty wywołania funkcji, gdzie arg0 jest nazwą programu

int execle(char const *path, char const *arg0, ..., char const * const *envp)
//podobnie jak execl, ale pozwala na podanie w ostatnim argumencie
//tablicy ze zmiennymi środowiskowymi

int execlp(char const *file, char const *arg0, ...)
//również przyjmuje listę argumentów ale, nie podajemy tutaj ścieżki do pliku,
//lecz samą jego nazwę, zmienna środowiskowa PATH zostanie przeszukana w celu
//zlokalizowania pliku

int execv(char const *path, char const * const * argv)
//analogicznie do execl, ale argumenty podawane są w tablicy

int execve(char const *path, char const * const *argv, char const * const *envp)
//analogicznie do execle, również argumenty przekazujemy
//tutaj w tablicy tablic znakowych

int execvp(char const *file, char const * const *argv)
//analogicznie do execlp, argumenty w tablicy
```

```c
#include <sys/wait.h>
pid_t waitpid(pid_t pid, int *stat_loc, int options);
```

---

## Sygnały

---

## Strumienie

---
