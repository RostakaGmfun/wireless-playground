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
