/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"
#include "irc_def.h"
#include "irc_user.h"
#include "irc_channel.h"
#include "irc_commands.h"
#include "irc_mode.h"

char chmode_t[] = CHAN_MODES;
char chusrmode_t[] = CHAN_USER_MODES;
char chparamode_t[] = CHAN_PARAM_MODES;
char usrmode_t[] = USER_MODES;

unsigned int mode_ischusrmode(char mode) {
    unsigned int i = 0;
    
    for(i = 0;i < strlen(chusrmode_t);i++) {
        if(mode == chusrmode_t[i])
            return 1;
    }
    
    return 0;
}

unsigned int mode_ischparamode(char mode) {
    unsigned int i = 0;
    
    for(i = 0;i < strlen(chparamode_t);i++) {
        if(mode == chparamode_t[i])
            return 1;
    }
    
    return 0;
}

struct modeparam* mode_getparam(struct modeparam *mp, char *modestr, unsigned int tok) {
    char *mode, *param;
    unsigned int i = 0, t = 0, s = 0, len;
    
    if((mp == NULL) || (blankstr(modestr)))
        return NULL;
    
    if(!tok)
        tok++;
    
    mp->set = 0;
    mp->mode = 0;
    clearstr(mp->param, MAX_LEN);
    
    len = strlen(modestr);
    mode = (char*)callocm(len+1, sizeof(char));
    if(mode == NULL)
        return NULL;
        
    strcpy(mode, modestr);
    
    strtok(mode, " ");
    for(i = 0;i < len;i++) {
        if(modestr[i] == ' ') {
            break;
        }
        if(modestr[i] == '+') {
            s = 1;
        }
        else if(modestr[i] == '-') {
            s = 0;
        }
        else if(mode_ischparamode(modestr[i])) {
            param = strtok(NULL, " ");
            if(++t == tok) {
                mp->set = s;
                mp->mode = modestr[i];
                if(param != NULL)
                    xstrcpy(mp->param, param, MAX_LEN);
                freem(mode);
                return mp;
            }
        }
    }
    
    freem(mode);
    return NULL;
}

/*Get the first instance of mode's paramater.*/
char* mode_get_firstinstparam(char *buf, char mode, char *modestr) {
    struct modeparam *tmp;
    unsigned int i = 0;
    
    if((buf == NULL) || (blankstr(modestr)))
        return NULL;
    
    if(!mode_ischparamode(mode))
        return NULL;
    
    tmp = (struct modeparam*)mallocm(sizeof(struct modeparam));
    if(tmp == NULL)
        return NULL;
    
    for(i = 0;mode_getparam(tmp, modestr, i) != NULL;i++) {
        if(tmp->mode == mode) {
            strcpy(buf, tmp->param);
            freem(tmp);
            return buf;
        }
    }
    
    freem(tmp);
    return NULL;
}

unsigned int mode_modecmp(struct modeparam match, char *modestr) {
    unsigned int i = 0, s = 0;
    struct modeparam *tmp;
    
    if(blankstr(modestr))
        return 0;
    
    if(blankstr(match.param)) {
        for(i = 0;i < strlen(modestr);i++) {
            if(modestr[i] == '+') {
                s = 1;
            }
            else if(modestr[i] == '-') {
                s = 0;
            }
            else if(modestr[i] == match.mode) {
                if(s == match.set)
                    return 1;
            }
        }
    }
    else {
        tmp = (struct modeparam*)mallocm(sizeof(struct modeparam));
        if(tmp == NULL)
            return 0;
        for(i = 0;mode_getparam(tmp, modestr, i) != NULL;i++) {
            if(tmp->set == match.set) {
                if(tmp->mode == match.mode) {
                    if(istrcmp(tmp->param, match.param)) {
                        freem(tmp);
                        return 1;
                    }
                }
            }
        }
        
        freem(tmp);
    }
    
    return 0;
}

struct banlist* mode_get_ban_handle(struct chanNode *chan, char *ban) {
    struct banlist *tmp;
    
    if((chan == NULL) || (blankstr(ban)))
        return NULL;
    
    for(tmp = chan->banlist_first;tmp != NULL;tmp = tmp->next) {
        if(istrcmp(tmp->ban, ban)) {
            return tmp;
        }
    }
    
    return NULL;
}

struct banlist* mode_addban(struct chanNode *chan, char *ban) {
    struct banlist *tmp;
    
    if(blankstr(ban))
        return NULL;
    
    tmp = (struct banlist*)mallocm(sizeof(struct banlist));
    if(tmp == NULL)
        return NULL;
    
    tmp->next = NULL;
    tmp->prev = NULL;
    xstrcpy(tmp->ban, ban, MAX_MASKLEN);
    clearstr(tmp->setby, MAX_MSGLEN);
    tmp->date = 0;
    
    LL_ADDNODE(tmp, chan->banlist_first, chan->banlist_last)
    
    chan->ban_count++;
    
    return tmp;
}

unsigned int mode_delban(struct chanNode *chan, char *ban) {
    struct banlist *tmp;
    
    if((chan == NULL) || (blankstr(ban)))
        return 0;
        
    tmp = mode_get_ban_handle(chan, ban);
    if(tmp == NULL)
        return 0;
    
    LL_DELNODE(tmp, chan->banlist_first, chan->banlist_last)
    
    chan->ban_count--;
    
    freem(tmp);
    return 1;
}

unsigned int mode_chanuser_mode(struct userNode *user, struct chanNode *chan, unsigned int mode) {
    struct userlist *hUserList;
    unsigned int retn_val = 0;
    
    if((user == NULL) || (chan == NULL))
        return 0;
    
    hUserList = chan_get_userlist(user, chan);
    if(hUserList == NULL)
        return 0;
    
    if(hUserList->chan_mode & mode)
        retn_val = 1;
    
    return retn_val;
}

unsigned int mode_remove_user_bans(struct userNode *user, struct chanNode *chan, unsigned int override) {
    struct banlist *ban, *next;
    
    if((user == NULL) || (chan == NULL))
        return 0;
    
    if(chan->me == NULL)
        return 0;
    
    if(user->hostmask == NULL)
        return 0;
    
    if(chan->banlist_first == NULL)
        return 1;
    
    if(((chan->me->chan_mode & MODE_OP) < 1) || ((chan->me->chan_mode & MODE_HALFOP) < 1)) {
        if(!override)
            return 1;
    }
    
    for(ban = chan->banlist_first;ban != NULL;ban = next) {
        next = ban->next;
        if(matchstr(ban->ban, user->hostmask)) {
            irc_mode(chan->name, "-b %s", ban->ban);
            mode_delban(chan, ban->ban);
        }
    }
    
    return 1;
}

unsigned int mode_chan(struct chanNode *chan, char *modestr) {
    char *value;
    unsigned int i = 0, s = 0;
    
    if((chan == NULL) || (blankstr(modestr)))
        return 0;
    
    value = (char*)callocm(MAX_LEN, sizeof(char));
    if(value == NULL)
        return 0;
    
    for(i = 0;i < strlen(modestr);i++) {
        clearstr(value, MAX_LEN);
        switch(modestr[i]) {
            case ' ':
                freem(value);
                return 1;
                break;
                
            case '+':
                s = 1;
                break;
            
            case '-':
                s = 0;
                break;
            
            case 't':
                if(s) chan->mode |= MODE_OP_TOPIC;
                else chan->mode &= ~MODE_OP_TOPIC;
                break;
            
            case 'n':
                if(s) chan->mode |= MODE_NO_EXT_MSG;
                else chan->mode &= ~MODE_NO_EXT_MSG;
                break;
                
            case 'i':
                if(s) chan->mode |= MODE_INVITE;
                else chan->mode &= ~MODE_INVITE;
                break;
            
            case 'm':
                if(s) chan->mode |= MODE_MODERATED;
                else chan->mode &= ~MODE_MODERATED;
                break;
                
            case 'k':
                if(s) {
                    mode_get_firstinstparam(value, 'k', modestr);
                    chan->mode |= MODE_KEY;
                    xstrcpy(chan->key, value, MAX_PASSLEN);
                }
                else {
                    chan->mode &= ~MODE_KEY;
                }
                break;
            
            case 'l':
                if(s) {
                    mode_get_firstinstparam(value, 'l', modestr);
                    chan->mode |= MODE_LIMIT;
                    chan->userlimit = atoi(value);
                }
                else {
                    chan->mode &= ~MODE_LIMIT;
                }
                break;
            
            case 'p':
                if(s) chan->mode |= MODE_PRIVATE;
                else chan->mode &= ~MODE_PRIVATE;
                break;
            
            case 's':
                if(s) chan->mode |= MODE_SECRET;
                else chan->mode &= ~MODE_SECRET;
                break;
            
            case 'c':
                if(s) chan->mode |= MODE_NOCOLORS;
                else chan->mode &= ~MODE_NOCOLORS;
                break;
            
            case 'C':
                if(s) chan->mode |= MODE_NOCTCPS;
                else chan->mode &= ~MODE_NOCTCPS;
                break;
            
            case 'D':
                if(s) chan->mode |= MODE_DELAYED_J;
                else chan->mode &= ~MODE_DELAYED_J;
                break;
            
            case 'd':
                if(s) chan->mode |= MODE_DELAYED_U;
                else chan->mode &= ~MODE_DELAYED_U;
                break;
            
            default:
                /*Unknown*/
                break;
        }
    }
    
    freem(value);    
    return 1;
}

unsigned int mode_chanparam(struct chanNode *chan, char *setter, char *modestr) {
    unsigned int mode = 0, i = 0, tok = 0;
    char *temp;
    struct banlist *ban;
    struct modeparam mp;
    struct userlist *hUserList;
    
    if((!chan) || (blankstr(setter)))
        return 0;
    
    temp = (char*)callocm(MAX_LEN, sizeof(char));
    if(temp == NULL)
        return 0;
    
    for(i = 0;i < strlen(modestr);i++) {
        if(modestr[i] != ' ') {
            if((modestr[i] == '+') || (modestr[i] == '-'))
                continue;
            
            if(mode_getparam(&mp, modestr, ++tok) == NULL) {
                freem(temp);
                return 0;
            }
            
            if(mp.mode != 'b') {
                hUserList = chan_get_userlist(user_get_handle(mp.param), chan);
                if(hUserList == NULL)
                    continue;
                
                mode = hUserList->chan_mode;
            }
            switch(mp.mode) {
                case 'o':
                    mode = mp.set ? (mode | MODE_OP) : (mode & ~MODE_OP);
                    if(hUserList == chan->me)
                        mode_remove_user_bans(me, chan, 1);
                    break;
                
                case 'h':
                    mode = mp.set ? (mode | MODE_HALFOP) : (mode & ~MODE_HALFOP);
                    if(hUserList == chan->me)
                        mode_remove_user_bans(me, chan, 1);
                    break;
                
                case 'v':
                    mode = mp.set ? (mode | MODE_VOICE) : (mode & ~MODE_VOICE);
                    break;
                
                case 'b':
                    if(mp.set) {
                        if(matchstr(mp.param, me->hostmask))
                            irc_mode(chan->name, "-b %s", mp.param);
                        ban = mode_addban(chan, mp.param);
                        xstrcpy(ban->setby, setter, MAX_LEN);
                        ban->date = time(NULL);
                    }
                    else {
                        mode_delban(chan, mp.param);
                    }
                    break;
            }
            if(mp.mode != 'b')
                hUserList->chan_mode = mode;
        }
        else {
            break;
        }
    }
    
    freem(temp);
    return 1;
}

void mode_free_banlist(struct chanNode *chan) {
    struct banlist *temp_list, *temp_list2;
    
    temp_list = chan->banlist_last;
    while(temp_list != NULL) {
        temp_list2 = temp_list->prev;
        freem(temp_list);
        if(temp_list2 == NULL) break;
        temp_list2->next = NULL;
        temp_list = temp_list2;
    }
    
    chan->ban_count = 0;
    return ;
}
