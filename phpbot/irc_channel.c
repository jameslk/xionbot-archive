/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "irc_user.h"
#include "irc_channel.h"
#include "irc_mode.h"

struct chanNode* chan_get_handle(char *name) {
    struct chanNode *chan;
    
    if(blankstr(name))
        return NULL;
    
    for(chan = chan_first;chan != NULL;chan = chan->next) {
        if(istrcmp(name, chan->name)) {
            return chan;
        }
    }
    
    return NULL;
}

/* Check to see if I am in a channel with a prefix of type. */
unsigned int chan_prefix(char type) {
    struct chanNode *tmp;
    
    if(chan_first == NULL)
        return 0;
        
    for(tmp = chan_first;tmp != NULL;tmp = tmp->next) {
        if(tmp->chantype == type)
            return 1;
    }
    
    return 0;
}

static void init_chan_struct(struct chanNode *chan) {
    chan->next = NULL;
    chan->prev = NULL;
    
    clearstr(chan->name, MAX_LEN);
    chan->chantype = 0;
    clearstr(chan->topic, MAX_LEN);
    
    chan->mode = 0;
    
    chan->userlimit = 0;
    clearstr(chan->key, MAX_PASSLEN);
    
    chan->ban_count = 0;
    chan->banlist_first = NULL;
    chan->banlist_last = NULL;
    
    chan->user_count = 0;
    chan->userlist_first = NULL;
    chan->userlist_last = NULL;
    chan->me = NULL;
    
    return ;
}

struct chanNode* chan_addchan(char *chan_name) {
    struct chanNode *chan;
    
    if(blankstr(chan_name))
        return NULL;
    
    if(chan_get_handle(chan_name))
        return NULL;
    
    chan = (struct chanNode*)mallocm(sizeof(struct chanNode));
    if(chan == NULL) {
        make_error("Failed to allocate space for chan_addchan.");
        return NULL;
    }
    
    init_chan_struct(chan);
    xstrcpy(chan->name, chan_name, MAX_CHANLEN);
    
    LL_ADDNODE(chan, chan_first, chan_last)
    
    chan_count++;
    
    return chan;
}

unsigned int chan_delchan(struct chanNode *chan) {
    struct userlist *hUserList;
    
    if(chan == NULL)
        return 0;
        
    /* Remove the channel from each user's channel list
        then recycle channel's own user list and ban list. */
    for(hUserList = chan->userlist_first;hUserList != NULL;hUserList = hUserList->next)
        user_del_userchan(user_get_chanlist(hUserList->user, chan));
    
    chan_free_userlist(chan);
    mode_free_banlist(chan);
    
    LL_DELNODE(chan, chan_first, chan_last)
    
    chan_count--;
    
    freem(chan);
    return 1;
}

struct userlist* chan_get_userlist(struct userNode *user, struct chanNode *chan) {
    struct userlist *hUserList;
    
    if((user == NULL) || (chan == NULL))
        return NULL;
    
    for(hUserList = chan->userlist_first;hUserList != NULL;hUserList = hUserList->next) {
        if(istrcmp(hUserList->user->nick, user->nick))
            return hUserList;
    }
    
    return NULL;
}

struct userlist* chan_add_chanuser(struct userNode *user, struct chanNode *chan) {
    struct userlist *hUserList;
    
    if((user == NULL) || (chan == NULL))
        return NULL;
    
    if(user_get_chanlist(user, chan))
        return NULL;
    
    hUserList = (struct userlist*)mallocm(sizeof(struct userlist));
    if(!hUserList)
        return NULL;
    
    hUserList->next = NULL;
    hUserList->prev = NULL;
    hUserList->user = user;
    hUserList->chan = chan;
    hUserList->chan_mode = 0;
    
    LL_ADDNODE(hUserList, chan->userlist_first, chan->userlist_last)
    
    chan->user_count++;
    return hUserList;
}

unsigned int chan_del_chanuser(struct userlist *hUserList) {
    struct chanNode *chan;
    
    if(!hUserList)
        return 0;
        
    chan = hUserList->chan;
    if(!chan)
        return 0;
    
    LL_DELNODE(hUserList, chan->userlist_first, chan->userlist_last)
    
    chan->user_count--;
    freem(hUserList);
    return 1;
}

void chan_free_userlist(struct chanNode *chan) {
    struct userlist *temp_list, *temp_list2;
    
    temp_list = chan->userlist_last;
    while(temp_list != NULL) {
        temp_list2 = temp_list->prev;
        freem(temp_list);
        if(temp_list2 == NULL) break;
        temp_list2->next = NULL;
        temp_list = temp_list2;
    }
    
    chan->userlist_first = NULL;
    chan->userlist_last = NULL;
    
    chan->user_count = 0;
    return ;
}

void chan_free_data(void) {
    struct chanNode *temp_chan, *temp_chan2;
    
    temp_chan = chan_last;
    while(temp_chan != NULL) {
        temp_chan2 = temp_chan->prev;
        mode_free_banlist(temp_chan);
        chan_free_userlist(temp_chan);
        freem(temp_chan);
        if(temp_chan2 == NULL) break;
        temp_chan2->next = NULL;
        temp_chan = temp_chan2;
    }
    
    chan_first = NULL;
    chan_last = NULL;
    
    chan_count = 0;
    return ;
}
