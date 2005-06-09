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

#include "main.h"
#include "mod-weburlcache.h"

unsigned int urlmod_init(void) {
    event_hook(urlm_scan_text, 0, (EVENT_IRCPRIVMSG | EVENT_IRCNOTICE | EVENT_IRCQUIT), 0, NULL);
    
    return 1;
}

EVENT_FUNC(urlm_scan_text) {
    char *raw, *msg, *text, *url;
    unsigned int size = 1, pos = 0;
    
    raw = dlist_v(event_data, 0, char*);
    if(blankstr(raw))
        return ;
    
    msg = (char*)callocm(strlen(raw)+1, sizeof(char));
    if(msg == NULL) {
        return ;
    }
    
    irc_get_msg(msg, raw);
    irc_strip_ctrlcodes(&text, msg);
    
    while(size) {
        size = urlm_get_url(&url, text, &pos);
        if(size) {
            print_svar(url);
            urlm_write_url(url);
            freem(url);
        }
    }
    
    return ;
}

char url_strings[][8] = {"www.*", "http://*", "ftp://*"};
unsigned int urlm_get_url(char **bufp, const char *text, unsigned int *position) {
    char *buf;
    const char *str;
    unsigned int text_len, url_type, i, ii, next = 0, match = 0, pos;
    
    if(blankstr(text))
        return 0;
    
    if(position == NULL) {
        pos = 0;
    }
    else {
        pos = *position;
        str = text+pos;
    }
    text_len = strlen(text)-pos;
    
    buf = (char*)callocm(text_len+1, sizeof(char));
    if(buf == NULL)
        return 0;
        
    for(url_type = 0;url_type < 3;url_type++) {
        for(i = 0, ii = 0;i < text_len;i++) {
            if(match) {
                if(str[i] == ' ') {
                    *bufp = buf;
                    *position = pos+i;
                    return ii;
                }
                buf[ii++] = str[i];
            }
            else if(url_strings[url_type][next] == '*') {
                if(str[i] == ' ') {
                    freem(buf);
                    return 0;
                }
                match = 1;
                buf[ii++] = str[i];
            }
            else if(tolower(str[i]) == url_strings[url_type][next]) {
                next++;
                buf[ii++] = str[i];
            }
        }
        if(match) {
            *bufp = buf;
            *position = pos+i;
            return ii;
        }
        clearstr(buf, text_len+1);
    }
    
    freem(buf);
    return 0;
}

unsigned int urlm_write_url(char *url) {
    FILE *fptr;
    unsigned int url_len;
    
    if(blankstr(url))
        return 0;
    
    url_len = strlen(url);
    
    fptr = fopen(URLM_FILE, "a");
    if(!fptr)
        return 0;
    
    if(fwrite(url, sizeof(char), url_len, fptr) != url_len)
        return 0;
    
    if(fwrite("\n", sizeof(char), 1, fptr) != 1)
        return 0;
    
    fclose(fptr);
}
