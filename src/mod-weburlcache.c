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
#include "irc_tools.h"

unsigned int urlmod_init(void) {
    #ifdef URLM_ENABLED
    event_hook(urlm_scan_text, 0, (EVENT_IRCRPL_TOPIC | EVENT_IRCPRIVMSG | EVENT_IRCNOTICE | EVENT_IRCQUIT), 0, NULL);
    #endif
    
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
            urlm_write_url(url);
            freem(url);
        }
    }
    
    freem(msg);
    freem(text);
    
    return ;
}

char url_strings[][9] = {"www.*", "http://*", "ftp://*"};
unsigned int urlm_get_url(char **bufp, char *text, unsigned int *position) {
    char *str;
    unsigned int text_len, url_type, segment_len, i, offset = 0, pos, match;
    
    if(blankstr(text))
        return 0;
    
    if(position == NULL) {
        pos = 0;
    }
    else {
        pos = *position;
        text = text+pos;
    }
    text_len = strlen(text);
    
    str = (char*)callocm(text_len+1, sizeof(char));
    if(str == NULL)
        return 0;
    
    xstrcpy(str, text, text_len+1);
    for(i = 0;i < text_len;i++) {
        if(text[i] == ' ')
            str[i] = 0;
    }
    
    for(offset = 0;offset < text_len;) {
        segment_len = strlen(str+offset);
        for(url_type = 0;url_type < 3;url_type++) {
            match = 0;
            if(segment_len < strlen(url_strings[url_type]))
                continue;
            
            for(i = 0;url_strings[url_type][i] != 0;i++) {
                if(tolower(str[i+offset]) == url_strings[url_type][i]) {
                    match++;
                }
                else if(match == strlen(url_strings[url_type])-1) {
                    if(url_strings[url_type][i] == '*') {
                        *bufp = (char*)callocm(segment_len+1, sizeof(char));
                        if(*bufp == NULL) {
                            freem(str);
                            return 0;
                        }
                        
                        xstrcpy(*bufp, str+offset, segment_len+1);
                        freem(str);
                        
                        *position = pos+offset+segment_len+1;
                        return segment_len;
                    }
                }
            }
        }
        offset += segment_len+1;
    }
    
    freem(str);
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
    
    return 1;
}
