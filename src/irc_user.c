/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "irc_channel.h"
#include "irc_user.h"

struct userNode* user_get_handle(char *nick) {
    struct userNode *user;
    
    if(blankstr(nick))
        return NULL;
    
    for(user = user_first;user != NULL;user = user->next) {
        if(istrcmp(user->nick, nick))
            return user;
    }
    
    return NULL;
}

static void init_user_struct(struct userNode *user) {
    user->next = NULL;
    user->prev = NULL;
    user->chanlist_first = NULL;
    user->chanlist_last = NULL;
    clearstr(user->host, MAX_MASKLEN);
    clearstr(user->hostmask, MAX_HOSTMASKLEN);
    clearstr(user->info, MAX_INFOLEN);
    clearstr(user->nick, MAX_NICKLEN);
    clearstr(user->user, MAX_USERLEN);
    user->user_mode = 0;
    user->chan_count = 0;
    
    return ;
}

struct userNode* user_adduser(char *nick) {
    struct userNode *user;
    
    if(blankstr(nick))
        return NULL;
    
    if(user_get_handle(nick))
        return NULL;
    
    user = (struct userNode*)mallocm(sizeof(struct userNode));
    if(user == NULL)
        return NULL;
    
    init_user_struct(user);
    xstrcpy(user->nick, nick, MAX_MSGLEN);
    
    if(istrcmp(user->nick, bot.current_nick))
        me = user;
    
    LL_ADDNODE(user, user_first, user_last)
    
    user_count++;
    return user;
}

unsigned int user_deluser(struct userNode *user) {
    struct chanlist *hChanList;
    
    if(user == NULL)
        return 0;
    
    /* Remove the user from each channel's user list
        then recycle user's own channel list. */
    for(hChanList = user->chanlist_first;hChanList != NULL;hChanList = hChanList->next)
        chan_del_chanuser(hChanList->chanuser_info);
    
    user_free_chanlist(user);
    
    LL_DELNODE(user, user_first, user_last)
    
    user_count--;
    freem(user);
    return 1;
}

struct chanlist* user_get_chanlist(struct userNode *user, struct chanNode *chan) {
    struct chanlist *hChanList;
    
    if((user == NULL) || (chan == NULL))
        return NULL;
    
    for(hChanList = user->chanlist_first;hChanList != NULL;hChanList = hChanList->next) {
        if(istrcmp(hChanList->chan->name, chan->name))
            return hChanList;
    }
    
    return NULL;
}

struct chanlist* user_add_userchan(struct userNode *user, struct chanNode *chan) {
    struct chanlist *hChanList;
    
    if((user == NULL) || (chan == NULL))
        return NULL;
    
    if(user_get_chanlist(user, chan))
        return NULL;
    
    hChanList = (struct chanlist*)mallocm(sizeof(struct chanlist));
    if(!hChanList)
        return NULL;
    
    hChanList->next = NULL;
    hChanList->prev = NULL;
    hChanList->user = user;
    hChanList->chan = chan;
    hChanList->chanuser_info = NULL;
    
    LL_ADDNODE(hChanList, user->chanlist_first, user->chanlist_last)
    
    user->chan_count++;
    return hChanList;
}

unsigned int user_del_userchan(struct chanlist *hChanList) {
    struct userNode *user;
    
    if(hChanList == NULL)
        return 0;
        
    user = hChanList->user;
    if(user == NULL)
        return 0;
    
    LL_DELNODE(hChanList, user->chanlist_first, user->chanlist_last)
    
    user->chan_count--;
    freem(hChanList);
    return 1;
}

unsigned int user_fillmask(struct userNode *user, char *hostmask) {
    unsigned int i, x, len, len2, len3;
    if((user == NULL) || (blankstr(hostmask)))
        return 0;
    
    len = strlen(hostmask);
    len2 = strlen(user->nick);
    for(i = len2+1, x = 0;x < (len2+MAX_USERLEN);i++, x++) {
        if(hostmask[i] == '@')
            break;
        user->user[x] = hostmask[i];
    }
    user->user[x] = 0;
    
    len3 = len2+strlen(user->user)+2;
    for(x = 0, i++;x < (len3+MAX_MASKLEN);i++, x++) {
        if(!hostmask[i])
            break;
        user->host[x] = hostmask[i];
    }
    user->host[x] = 0;
    
    xstrcpy(user->hostmask, hostmask, MAX_HOSTMASKLEN);
    
    return 1;
}

void user_free_chanlist(struct userNode *user) {
    struct chanlist *temp_list, *temp_list2;
    
    temp_list = user->chanlist_last;
    while(temp_list != NULL) {
        temp_list2 = temp_list->prev;
        freem(temp_list);
        if(temp_list2 == NULL) break;
        temp_list2->next = NULL;
        temp_list = temp_list2;
    }
    
    user->chanlist_first = NULL;
    user->chanlist_last = NULL;
    
    user->chan_count = 0;
    return ;
}

void user_free_data(void) {
    struct userNode *temp_user, *temp_user2;
    
    temp_user = user_last;
    while(temp_user != NULL) {
        temp_user2 = temp_user->prev;
        user_free_chanlist(temp_user);
        freem(temp_user);
        if(temp_user2 == NULL) break;
        temp_user2->next = NULL;
        temp_user = temp_user2;
    }
    
    user_first = NULL;
    user_last = NULL;
    
    user_count = 0;
    return ;
}
