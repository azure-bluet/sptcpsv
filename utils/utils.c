#include <stdlib.h>
#include <string.h>
#include <time.h>

char* strtm (void)
{
    time_t t;
    char* h = asctime (localtime ((t = time (NULL), &t)));
    h [24] = 0;
    if (h [8] == ' ') h [8] = '0';
    return h;
}

char* strtmu (void)
{
    time_t t;
    char* r = malloc (21);
    char* h = asctime (localtime ((t = time (NULL), &t)));
    memcpy (r, h+4, 20);
    r [20] = 0;
    if (r [4] == ' ') r [4] = '0';
    r [3] = r [6] = r [9] = r [12] = r [15] = '-';
    return r;
}

#ifdef MAIN_UTL
#include <stdio.h>
int main (void)
{
    char* a;
    char* b;
    printf ("%s\n%s\n", a = strtm (), b = strtmu ());
    return 0;
}
#endif
