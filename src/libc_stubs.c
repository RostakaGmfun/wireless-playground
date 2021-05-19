#include <stddef.h>
#include <sys/time.h>

int _write(int fd, const char *buf, int count)
{
    return -1;
}

int _read()
{
    return 0;
}

int _close()
{
    return 0;
}

int _isatty()
{
    return 0;
}

int _lseek()
{
    return 0;
}

int _fstat()
{
    return 0;
}

int _sbrk()
{
    return 0;
}

int _kill()
{
    return 0;
}

void _exit(int arg)
{
    (void)arg;
    while (1);
}

// Needed by ext_printf module
void _putchar(char character)
{
    (void)character;
}

int _getpid(void)
{
    return 0;
}

int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    if (NULL != tv) {
        tv->tv_sec = 0;
        tv->tv_usec = 0;
    }

    return 0;
}
