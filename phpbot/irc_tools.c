/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "irc_tools.h"
#include "irc_parse.h"
#include "irc_commands.h"

char* irc_get_host(char *buf, char *raw) {
    char *data;
    char *seperateFrmSp;
    char *host;
    
    if(buf == NULL)
        return buf;
    
    data = (char*)callocm(MAX_LEN, sizeof(char));
    if(data == NULL)
        return NULL;
    
    xstrcpy(data, raw, MAX_LEN);
    
    strtok(data, " ");
    seperateFrmSp = strtok(NULL, " ");
    if(seperateFrmSp == NULL) {
        freem(data);
        return NULL;
    }
    
    xstrcpy(data, seperateFrmSp, MAX_LEN);
    strtok(data, "@");
    host = strtok(NULL, "@");
    if(host == NULL) {
        freem(data);
        return NULL;
    }
    
    strcpy(buf, host);
    
    freem(data);
    return buf;
}

char* irc_get_mask(char *buf, char *raw) {
    char *data;
    char *seperateFrmSp;
    char *mask;
    
    if(buf == NULL)
        return buf;
    
    data = (char*)callocm(MAX_LEN, sizeof(char));
    if(data == NULL)
        return NULL;
    
    xstrcpy(data, raw, MAX_LEN);
    
    seperateFrmSp = strtok(data, " ");
    if(seperateFrmSp == NULL) {
        freem(data);
        return NULL;
    }
    
    xstrcpy(data, seperateFrmSp, MAX_LEN);
    mask = strtok(data, ":");
    if(mask == NULL) {
        freem(data);
        return NULL;
    }
    
    strcpy(buf, mask);
    
    freem(data);
    return buf;
}

char* irc_get_nick(char *buf, char *raw) {
    char *data;
    char *seperateFrmCol;
    char *nick;
    
    if(buf == NULL)
        return buf;
    
    data = (char*)callocm(MAX_LEN, sizeof(char));
    if(data == NULL)
        return NULL;
    
    xstrcpy(data, raw, MAX_LEN);
    
    seperateFrmCol = strtok(data, ":");
    if(seperateFrmCol == NULL) {
        freem(data);
        return NULL;
    }
    
    strcpy(data, seperateFrmCol);
    if(strchr(data, '!') == NULL)
        nick = strtok(data, " ");
    else
        nick = strtok(data, "!");
    if(nick == NULL) {
        freem(data);
        return NULL;
    }
    
    xstrcpy(buf, nick, MAX_NICKLEN);
    
    freem(data);
    return buf;
}

char* irc_get_target(char *buf, char *raw) {  
    char *data;
    char *target;
    
    if(buf == NULL)
        return buf;
    
    data = (char*)callocm(MAX_LEN, sizeof(char));
    if(data == NULL)
        return NULL;
    
    xstrcpy(data, raw, MAX_LEN);
    
    strtok(data, " ");
    strtok(NULL, " ");
    target = strtok(NULL, " ");
    if(target == NULL) {
        freem(data);
        return NULL;
    }
    
    if(*target == ':')
        target++;
    strcpy(buf, target);
    
    freem(data);
    return buf;
}

char* irc_get_msg(char *buf, char *raw) {
    char *str;
    
    if(buf == NULL)
        return buf;
    
    str = strstr(raw+1, " :");
    if(blankstr(str))
        *buf = 0;
    else
        strcpy(buf, str+2);
    
    return buf;
}

char* irc_get_cmdtype(char *buf, char *raw) {
    char *data;
    char *type;
    
    if(buf == NULL)
        return buf;
    
    data = (char*)callocm(MAX_LEN, sizeof(char));
    if(data == NULL)
        return NULL;
    
    xstrcpy(data, raw, MAX_LEN);
    strtok(data, " ");
    type = strtok(NULL, " ");
    if(type == NULL) {
        freem(data);
        return NULL;
    }
    
    strcpy(buf, type);
    
    freem(data);
    return buf;
}

char* irc_get_cmdtype2(char *buf, char *raw) {
    char *data;
    char *type;
    
    if(buf == NULL)
        return buf;
    
    data = (char*)callocm(MAX_LEN, sizeof(char));
    if(data == NULL)
        return NULL;
    
    xstrcpy(data, raw, MAX_LEN);
    type = strtok(data, " ");
    if(type == NULL) {
        freem(data);
        return NULL;
    }
    
    strcpy(buf, type);
    
    freem(data);
    return buf;
}

char* irc_get_servname(char *buf, char *raw) {
    char *servname;
    
    if(buf == NULL)
        return buf;
    
    if(blankstr(raw))
        return NULL;
    
    servname = (char*)callocm(256, sizeof(char));
    xstrcpy(servname, raw+1, 256);
    strcpy(buf, strtok(servname, " "));
    
    freem(servname);
    return buf;
}

char* irc_get_netname(char *buf, char *raw) {
    char *netname, *str;
    
    if(buf == NULL)
        return buf;
    
    if(strstr(raw, "NETWORK=") == NULL) return NULL;
    
    netname = (char*)callocm(33, sizeof(char));
    str = strstr(raw, "NETWORK=");
    if(blankstr(str)) {
        *buf = 0;
        return buf;
    }
    xstrcpy(netname, str+8, 32);
    strcpy(buf, strtok(netname, " "));
    
    freem(netname);
    return buf;
}

THREADFUNC(irc_fightfornick) {
    while(1) {
        if(mynickison) {
            irc_ison(bot.nick);
            waits(12);
        }
        else {
            break;
        }
    }
    
    mynickison = 0;
        
    return 0;
}

unsigned int irc_char_type(char c, unsigned int type) {
    /*  Types
        0 = ALPHA_CHARS
        1 = NUMERIC_CHARS
        2 = ACCESS_CHARS
        3 = SPECIAL_CHARS
    */
    char *match;
    unsigned int i = 0;
    
    switch(type) {
        case IS_TYPE_ALPHA:
            match = (char*)callocm(strlen(ALPHA_CHARS)+1, sizeof(char));
            if(match == NULL) return 0;
            strcpy(match, ALPHA_CHARS);
            break;
        
        case IS_TYPE_NUMERIC:
            match = (char*)callocm(strlen(NUMERIC_CHARS)+1, sizeof(char));
            if(match == NULL) return 0;
            strcpy(match, NUMERIC_CHARS);
            break;
        
        case IS_TYPE_ACCESS:
            match = (char*)callocm(strlen(ACCESS_CHARS)+1, sizeof(char));
            if(match == NULL) return 0;
            strcpy(match, ACCESS_CHARS);
            break;
        
        case IS_TYPE_SPECIAL:
            match = (char*)callocm(strlen(SPECIAL_CHARS)+1, sizeof(char));
            if(match == NULL) return 0;
            strcpy(match, SPECIAL_CHARS);
            break;
    }
    
    for(i = 0;i < strlen(match);i++) {
        if(c == match[i]) {
            freem(match);
            return 1;
        }
    }
    
    freem(match);
    return 0;
}
