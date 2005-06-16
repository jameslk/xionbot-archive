/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conf.h"
#include "main.h"

char* config_line(char *buf, unsigned long line) {
    char *fbuf, *l;
    FILE *fptr;
    unsigned long fsize, i;
    int pos = 0;
    
    #if defined(PLATFORM_WINDOWS)
    fptr = fopen(bot.config, "r+t");
    #else
    fptr = fopen(bot.config, "r+");
    #endif
    if(!fptr) {
        make_error("Couldn't open config file.");
        return NULL;
    }
    
    fseek(fptr, 0, SEEK_END);
    fsize = ftell(fptr);
    if((fbuf = (char*)callocm((fsize+1), sizeof(char))) == NULL) {
        return NULL;
    }
    rewind(fptr);
    fread(fbuf, sizeof(char), fsize, fptr);
    fclose(fptr);
    
    if(line == 0) line++;
    
    if((l = xstrtok(fbuf, "\n", &pos)) == NULL) {
        freem(fbuf);
        return NULL;
    }
    for(i = 1;l != NULL;i++) {
        if(i == line) {
            xstrcpy(buf, l, CONF_MAX_ITEMLEN);
            freem(fbuf);
            return buf;
        }
        l = xstrtok(fbuf, "\n", &pos);
    }
    
    freem(fbuf);
    return NULL;
}

unsigned long get_confitemcount(unsigned char citem) {
    char *item = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    unsigned long x = 0, y = 0;
    
    for(x = 0;config_line(item, x) != NULL;x++) {
        if(item[0] == citem) y++;
    }
    
    freem(item);
    return y;
}

unsigned long get_confitem(char ***buf, unsigned char citem) {
    char *item = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    unsigned long i = 0, n = 0, count = 0, itemlen = 0;
    
    count = get_confitemcount(citem);
    if(!count)
        return 0;
    
    *buf = (char**)mallocm(count*sizeof(char*));
    
    for(i = 0;config_line(item, i) != NULL;i++) {
        if(item[0] == citem) {
            itemlen = strlen(item)+1;
            if(itemlen > CONF_MAX_ITEMLEN)
                itemlen =  CONF_MAX_ITEMLEN;
            buf[0][n] = (char*)callocm(itemlen, sizeof(char));
            xstrcpy(buf[0][n++], item, CONF_MAX_ITEMLEN);
        }
    }
    
    freem(item);    
    return n;
}

char* get_itemtok(char *buf, char* item, unsigned int tok) {
    char *retval;
    char *temp = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    char *temp2 = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    unsigned int i = 0, x = 0;
    int pos = 0;
    
    xstrcpy(temp, item, (CONF_MAX_ITEMLEN-1));
    strcat(temp, ":");
    
    for(i = 0;i < strlen(temp);i++) {
        if(temp[i] == ':') {
            x++;
            if(x == 2) {
                sprintf(temp2,":;%s",temp+i);
                memmove(temp+(i-1),temp2,strlen(temp2));
                x = 0;
            }
        }
        else {
            x = 0;
        }
    }
    
    if(tok == 0) tok++;
    
    retval = xstrtok(temp, ":", &pos);
    for(i = 1;retval != NULL;i++) {
        if(i == tok) {
            xstrcpy(buf, retval, CONF_MAX_ITEMLEN);
            freem(temp);
            freem(temp2);
            return buf;
        }
        retval = xstrtok(temp, ":", &pos);
    }
    
    freem(temp);
    freem(temp2);
    return NULL;
}

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
