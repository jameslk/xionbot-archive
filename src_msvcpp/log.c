/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <time.h>

#include "main.h"

unsigned int log_write(char *format, ...) {
    FILE *fptr;
    va_list ap;
    time_t rawtime;
    struct tm *t;
    char *ext_format;
    
    ext_format = (char*)callocm(strlen(format)+24, sizeof(char));
    if(ext_format == NULL)
        return 0;
    
    fptr = fopen(XION_LOG, "a");
    if(fptr == NULL) {
        fprintf(stderr, "Could not open %s for writing.", XION_LOG);
        freem(ext_format);
        return 0;
    }
    
    rawtime = time(NULL);
    t = localtime(&rawtime);
    
    sprintf(ext_format, "[%02d:%02d:%02d %02d/%02d/%04d] %s\n", t->tm_hour, t->tm_min, t->tm_sec, (t->tm_mon+1), t->tm_mday, (t->tm_year+1900), format);
    
    va_start(ap, format);
    vfprintf(fptr, ext_format, ap);
    va_end(ap);
    
    fclose(fptr);
    freem(ext_format);
    return 1;
}

unsigned int log_clean(void) {
    FILE *fptr;
    
    if(bot.fresh_log) {
        fptr = fopen(XION_LOG, "w");
        if(!fptr)
            return 0;
        fclose(fptr);
    }
    
    return 1;
}
