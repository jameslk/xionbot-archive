/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "subconf.h"
#include "conf.h"

char* irc_subconftok(char *buf, unsigned char citem, char *subname, unsigned int tok) {
    unsigned long i = 0, lcount = 0;
    char **lines = NULL;
    char *conftok = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    
    lcount = get_confitem(&lines, citem);
    for(i = 0;i < lcount;i++) {
        if(istrcmp(get_itemtok(conftok, lines[i], 2), subname)) {
            get_itemtok(conftok, lines[i], tok);
            strncpy(buf, conftok, CONF_MAX_ITEMLEN);
            freem(conftok);
            free_2d_array(lines, lcount);
            return buf;
        }
    }
    
    freem(conftok);
    free_2d_array(lines, lcount);
    return NULL;
}
