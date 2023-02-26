#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#undef errno
extern int errno;

// Heap pointers from linker script
extern unsigned char __heap_start;
extern unsigned char __heap_end;

// Pointer to current position in heap
static void* _cur_brk = NULL;

extern void putchar_(char c);

int _close(int file) {
    return -1;
}

int _fstat(int file, struct stat* st) {
    return 0;
}

int _isatty(int file) {
    return 0;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _getpid(void) {
    return 1;
}

int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

void _exit(int status) {
    _kill(status, -1);
    while(1) {
    }
}

caddr_t _sbrk_r(struct _reent* r, int incr) {
    char* _old_brk = _cur_brk;

    if(_cur_brk == NULL) _cur_brk = (void*)&__heap_start;

    if((_cur_brk + incr) > ((void*)&__heap_end)) {
        uint8_t i;

        char* msg = "HEAP FULL!\r\n";

        for(i = 0; i < strlen(msg); i++) {
            putchar_(msg[i]);
        }

        errno = ENOMEM;
        return (void*)-1;
    }

    _cur_brk += incr;

    return (caddr_t)_old_brk;
}

int _read(int file, char* ptr, int len) {
    return 0;
}

int _write(int file, char* ptr, int len) {
    for(uint32_t i = 0; i < len; i++) {
        if(ptr[i] == '\n') {
            putchar_('\r');
        }
        putchar_(ptr[i]);
    }
    return len;
}
