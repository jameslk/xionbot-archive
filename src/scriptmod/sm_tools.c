/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <string.h>

#include "../main.h"
#include "../tools.h"
#include "script.h"
#include "sm_tools.h"

XBOOLEAN sm_setstr(char *str) {
    unsigned int i = 0, x = 0;
    
    if(blankstr(str))
        return XFALSE;
    
    for(i = 0;i < strlen(str);i++) {
        switch(str[x]) {
            case '\0':
                str[i] = '\0';
                return XTRUE;
                break;
            
            case ' ':
            case '\r':
            case '\n':
            case '\t':
                x++;
                switch(str[x]) {
                    case ' ':
                    case '\r':
                    case '\n':
                    case '\t':
                        i--;
                        break;
                    
                    default:
                        str[i] = tolower(str[x]);
                        break;
                }
                x++;
                break;
            
            default:
                str[i] = tolower(str[x]);
                x++;
                break;
        }
    }
    
    return XTRUE;
}
