/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>

#include "main.h"
#include "php_log.h"

unsigned int phpm_log_write(const char *format, va_list ap) {
    FILE *fptr;
    
    fptr = fopen(XION_PHPLOG, "a");
    if(fptr == NULL) {
        make_error("Failed to write to the php log.");
        return 0;
    }
    
    vfprintf(fptr, format, ap);
    
    fclose(fptr);
    return 1;
}
