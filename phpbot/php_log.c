/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
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
