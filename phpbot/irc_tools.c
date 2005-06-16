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
#include <time.h>
#include <winsock2.h>

#include "main.h"
#include "irc_tools.h"
#include "irc_parse.h"
#include "irc_commands.h"
#include "irc_socket.h"

char* irc_get_host(char *buf, char *raw) {
    char *data;
    char *seperateFrmSp;
    char *host;
    int pos = 0;
    
    if(buf == NULL)
        return buf;
    
    data = (char*)callocm(MAX_LEN, sizeof(char));
    if(data == NULL)
        return NULL;
    
    xstrcpy(data, raw, MAX_LEN);
    
    xstrtok(data, " ", &pos);
    seperateFrmSp = xstrtok(data, " ", &pos);
    if(seperateFrmSp == NULL) {
        freem(data);
        return NULL;
    }
    
    xstrcpy(data, seperateFrmSp, MAX_LEN);
    pos = 0;
    xstrtok(data, "@", &pos);
    host = xstrtok(data, "@", &pos);
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
    
    seperateFrmSp = xstrtok(data, " ", NULL);
    if(seperateFrmSp == NULL) {
        freem(data);
        return NULL;
    }
    
    xstrcpy(data, seperateFrmSp, MAX_LEN);
    mask = xstrtok(data, ":", NULL);
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
    
    seperateFrmCol = xstrtok(data, ":", NULL);
    if(seperateFrmCol == NULL) {
        freem(data);
        return NULL;
    }
    
    strcpy(data, seperateFrmCol);
    if(strchr(data, '!') == NULL)
        nick = xstrtok(data, " ", NULL);
    else
        nick = xstrtok(data, "!", NULL);
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
    int pos = 0;
    
    if(buf == NULL)
        return buf;
    
    data = (char*)callocm(MAX_LEN, sizeof(char));
    if(data == NULL)
        return NULL;
    
    xstrcpy(data, raw, MAX_LEN);
    
    xstrtok(data, " ", &pos);
    xstrtok(data, " ", &pos);
    target = xstrtok(data, " ", &pos);
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
    int pos = 0;
    
    if(buf == NULL)
        return buf;
    
    data = (char*)callocm(MAX_LEN, sizeof(char));
    if(data == NULL)
        return NULL;
    
    xstrcpy(data, raw, MAX_LEN);
    xstrtok(data, " ", &pos);
    type = xstrtok(data, " ", &pos);
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
    type = xstrtok(data, " ", NULL);
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
    strcpy(buf, xstrtok(servname, " ", NULL));
    
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
    strcpy(buf, xstrtok(netname, " ", NULL));
    
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

THREADFUNC(irc_pingserver) {
    char time_str[MAX_MSGLEN];
    int i;
    
    while(1) {
        if(recieved_ping) {
            recieved_ping = 0;
            waits(bot.ping_timeout);
        }
        else if(bot.connected) {
            sprintf(time_str, "%ld", time(NULL));
            irc_ping(time_str);
            waits(bot.ping_timeout);
            if(!recieved_ping) {
                irc_disconnect();
                waits(3);
                if((i = irc_connect(bot.servaddr, bot.servport)) != 0) {
                        printf("Failed to connect. ERROR %d:%d\n\n", WSAGetLastError(), i);
                        log_write("Connecting failed with code: %d:%d", WSAGetLastError(), i);
                        printf("Press Enter key to continue.");
                        getchar();
                        exit(1);
                }
                break;
            }
        }
        else {
            break;
        }
    }
    
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

char ctrlcodes[] = {C_BOLD, C_ULINE, C_COLOR, C_REVERSE};
unsigned int irc_strip_ctrlcodes(char **bufp, const char *text) {
    char *buf;
    unsigned int text_len;
    
    *bufp = NULL;
    
    if(blankstr(text))
        return 0;
    
    text_len = strlen(text);
    
    if(text_len < 2)
        return 0;
    
    buf = (char*)callocm(text_len+1, sizeof(char*));
    
    xstrcpy(buf, text, text_len+1);
    
    if(blankstr(strrtok(buf, ctrlcodes))) {
        freem(buf);
        return 0;
    }
    
    *bufp = buf;
    
    return text_len+1;
}
