/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "irc_def.h"
#include "irc_user.h"
#include "irc_channel.h"
#include "irc_mode.h"
#include "time.h"

chusr_t* chusr_getlast(chusr_t *usrfirst) {
    chusr_t *tmp;
    
    if(usrfirst == NULL)
        return NULL;
    
    for(tmp = usrfirst;tmp->next != NULL;tmp = tmp->next);
    return tmp;
}

chusr_t* chusr_nametoptr(chusr_t *usrfirst, char *nick) {
    chusr_t *tmp;
    
    if((usrfirst == NULL) || (blankstr(nick)))
        return NULL;
    
    for(tmp = usrfirst;tmp != NULL;tmp = tmp->next) {
        if(istrcmp(tmp->nick, nick)) {
            return tmp;
        }
    }
    
    return NULL;
}

unsigned int chusr_topaccess(chusr_t *usr) {
    if(usr == NULL)
        return 0;
    
    if(usr->mode & MODE_OP)
        return 3;
    else if(usr->mode & MODE_HALFOP)
        return 2;
    else if(usr->mode & MODE_VOICE)
        return 1;
    else
        return 0;
}

chusr_t* chusr_addusr(struct chanNode *chan, char *nick) {
    chusr_t *tmp;
    
    if((chan == NULL) || (blankstr(nick)))
        return NULL;
    
    tmp = (chusr_t*)malloc(sizeof(chusr_t));
    if(tmp == NULL)
        return NULL;
        
    clearstr(tmp->nick, MAX_NICKLEN);
    clearstr(tmp->mask, MAX_MASKLEN);
    
    tmp->next = NULL;
    tmp->chan = chan;
    xstrcpy(tmp->nick, nick, MAX_NICKLEN);
        
    if(chan->firstusr == NULL) {
        tmp->prev = NULL;
        chan->firstusr = tmp;
    }
    else {
        tmp->prev = chusr_getlast(chan->firstusr);
        tmp->prev->next = tmp;
    }
    
    chan->usercount++;
    
    return tmp;
}

unsigned int chusr_remusr(chusr_t *chusr) {
    if(chusr == NULL)
        return 0;
    
    if(chusr->next == NULL) {
        if(chusr->prev != NULL)
            chusr->prev->next = NULL;
        else
            chusr->chan->firstusr = NULL;
    }
    else {
        if(chusr->prev == NULL) {
            chusr->chan->firstusr = chusr->next;
            chusr->next->prev = NULL;
        }
        else {
            chusr->next->prev = chusr->prev;
            chusr->prev->next = chusr->next;
        }
    }
    
    chusr->chan->usercount--;
    
    free(chusr);
    return 0;
}

unsigned int chusr_usrmodes(struct chanNode *chan, char *modestr, char *nick) {
    char *temp;
    chusr_t *usr;
    blist_t *ban;
    mp_t mp;
    unsigned int i = 0;
    
    if((chan == NULL) || (blankstr(modestr)) || (blankstr(nick)))
        return 0;
    
    temp = (char*)allocm(513, sizeof(char));
    if(temp == NULL)
        return 0;
    
    for(i = 0;i < strlen(modestr);i++) {
        if(modestr[i] != ' ') {
            if((modestr[i] == '+') || (modestr[i] == '-'))
                continue;
            
            mode_getparam(&mp, modestr, i);
            switch(mp.mode) {
                case 'o':
                    usr = chusr_nametoptr(chan->firstusr, mp.param);
                    if(usr != NULL)
                        usr->mode = mp.set ? usr->mode | MODE_OP : usr->mode & ~MODE_OP;
                    break;
                
                case 'h':
                    usr = chusr_nametoptr(chan->firstusr, mp.param);
                    if(usr != NULL)
                        usr->mode = mp.set ? usr->mode | MODE_HALFOP : usr->mode & ~MODE_HALFOP;
                    break;
                
                case 'v':
                    usr = chusr_nametoptr(chan->firstusr, mp.param);
                    if(usr != NULL)
                        usr->mode = mp.set ? usr->mode | MODE_VOICE : usr->mode & ~MODE_VOICE;
                    break;
                
                case 'b':
                    if(mp.set) {
                        ban = mode_addban(chan, mp.param);
                        xstrcpy(ban->setby, nick, MAX_LEN);
                        ban->date = time(NULL);
                    }
                    else {
                        mode_remban(chan, mp.param);
                    }
                    break;
            }
        }
        else {
            break;
        }
    }
    
    free(temp);
    return 1;
}
