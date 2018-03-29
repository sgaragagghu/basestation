//utils.c
#include "utils.h"
void dump_line(FILE * fp) { /* dump a line from fp */
    int ch; /* temporary storage for character */

    while (1) {
        ch = fgetc(fp); /* read char from fp, this removes it from the stream */

        if (ch == EOF)  /* break if we reached END-OF-FILE */
            break;

        if (ch == '\n') /* break if we reached a newline, because */
            break;      /* we have succesfully read a line */
    }
}
