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
#include "irc_commands.h"
#include "irc_tools.h"
#include "irc_channel.h"
#include "irc_user.h"
#include "irc_botcmd.h"
#include "mod-irc_relay.h"

struct relayNode* relayed_chan(struct chanNode *to_chan, struct chanNode *from_chan) {
    struct relayNode *relay;
    
    if((to_chan == NULL) || (from_chan == NULL))
        return NULL;
    
    for(relay = relay_first;relay != NULL;relay = relay->next) {
        if((istrcmp(to_chan->name, relay->to_chan->name)) && (istrcmp(from_chan->name, relay->from_chan->name)))
            return relay;
    }
    
    return NULL;
}

struct relayNode* relayed_user(struct userNode *to_user, struct chanNode *from_chan) {
    struct relayNode *relay;
    
    if((to_user == NULL) || (from_chan == NULL))
        return NULL;
    
    for(relay = relay_first;relay != NULL;relay = relay->next) {
        if((istrcmp(to_user->nick, relay->to_user->nick)) && (istrcmp(from_chan->name, relay->from_chan->name)))
            return relay;
    }
    
    return NULL;
}

static unsigned int is_relay(struct chanNode *chan) {
    struct relayNode *relay;
    
    if(chan == NULL)
        return 0;
    
    for(relay = relay_first;relay != NULL;relay = relay->next) {
        if(istrcmp(chan->name, relay->from_chan->name))
            return 1;
    }
    
    return 0;
}

static void relay_all(struct chanNode *chan, char *user, char *message) {
    struct relayNode *relay;
    
    if((chan == NULL) || (blankstr(user)) || (blankstr(message)))
        return ;
    
    for(relay = relay_first;relay != NULL;relay = relay->next) {
        if(istrcmp(chan->name, relay->from_chan->name)) {
            if(relay->to_chan != NULL)
                irc_privmsg_chan(relay->to_chan->name, 0, "[%s] <%s> %s", relay->from_chan->name, user, message);
            else
                irc_privmsg_user(relay->to_user->nick, 0, "[%s] <%s> %s", relay->from_chan->name, user, message);
        }
    }
    
    return ;
}

void relay_message(char *raw, char *chan_name, char *user) {
    char *message;
    struct chanNode *chan;
    
    if((blankstr(raw)) || (blankstr(chan_name)) || (blankstr(user)))
        return ;
    
    if(!chan_prefix(chan_name[0]))
        return ;
    
    chan = chan_get_handle(chan_name);
    if(chan == NULL)
        return ;
    
    if(!is_relay(chan))
        return ;
    
    message = (char*)callocm(strlen(raw)+1, sizeof(char));
    if(message == NULL)
        return ;
    
    if(irc_get_msg(message, raw) == NULL) {
        freem(message);
        return ;
    }
    
    relay_all(chan, user, message);
    
    freem(message);
    
    return ;
}

static unsigned int register_relay(struct relayNode *relay) {
    if(relay == NULL)
        return 0;
        
    init_handle(&relay_first);
    init_handle(&relay_last);
    
    LL_ADDNODE(relay, relay_first, relay_last)
    
    relay_count++;
    return 1;
}

static unsigned int unregister_relay(struct relayNode *relay) {
    if(relay == NULL)
        return 0;
    
    LL_DELNODE(relay, relay_first, relay_last)
    
    relay_count--;
    freem(relay);
    return 1;
}

void relay_free_data(void) {
    struct relayNode *temp_relay, *temp_relay2;
    
    init_handle(&relay_first);
    init_handle(&relay_last);
    
    temp_relay = relay_last;
    while(temp_relay != NULL) {
        temp_relay2 = temp_relay->prev;
        freem(temp_relay);
        if(temp_relay2 == NULL) break;
        temp_relay2->next = NULL;
        temp_relay = temp_relay2;
    }
    
    relay_first = NULL;
    relay_last = NULL;
    
    relay_count = 0;
    return ;
}

BOT_CMD(relay) {
    struct userNode *to_user = NULL;
    struct chanNode *to_chan = NULL, *from_chan = NULL;
    struct relayNode *relay;
    
    if(chan_prefix(argv[0][0])) {
        to_chan = chan_get_handle(argv[1]);
        if(to_chan == NULL)
            return 0;
        
        if(to_chan->me == NULL)
            return 0;
    }
    else {
        to_user = user_get_handle(argv[1]);
        if(to_user == NULL)
            return 0;
    }
    
    from_chan = chan_get_handle(argv[0]);
    if(from_chan == NULL)
        return 0;
    
    if(from_chan->me == NULL)
        return 0;
    
    if(to_chan != NULL) {
        if(istrcmp(to_chan->name, from_chan->name)) {
            irc_notice_user(user->nick, 0, "Nice try. :P");
            return 0;
        }
        
        if(is_relay(to_chan)) {
            irc_notice_user(user->nick, 0, "Two-way relays are forbidden.");
            return 0;
        }
        
        if(relayed_chan(to_chan, from_chan) != NULL) {
            irc_notice_user(user->nick, 0, "This relay is already setup.");
            return 0;
        }
    }
    else {
        if(relayed_user(to_user, from_chan) != NULL) {
            irc_notice_user(user->nick, 0, "This relay is already setup.");
            return 0;
        }
    }
    
    relay = (struct relayNode*)mallocm(sizeof(struct relayNode));
    if(relay == NULL)
        return 0;
        
    relay->next = NULL;
    relay->prev = NULL;
    relay->to_user = to_user;
    relay->to_chan = to_chan;
    relay->from_chan = from_chan;
    if(!register_relay(relay))
        return 0;
        
    irc_notice_user(user->nick, 0, "Relay setup to %s from %s", to_chan != NULL ? to_chan->name : to_user->nick, from_chan->name);
    
    return 1;
}

BOT_CMD(relays) {
    struct relayNode *relay;
    
    init_handle(&relay_first);
    init_handle(&relay_last);
    
    if(relay_first == NULL) {
        irc_notice_user(user->nick, 0, "No relays setup.");
        return 1;
    }
    
    for(relay = relay_first;relay != NULL;relay = relay->next)
        irc_notice_user(user->nick, 0, "%s to %s", relay->from_chan->name, relay->to_chan != NULL ? relay->to_chan->name : relay->to_user->nick);
    
    return 1;
}

BOT_CMD(unrelay) {
    struct relayNode *relay;
    struct chanNode *to_chan, *from_chan;
    struct userNode *to_user;
    
    init_handle(&relay_first);
    init_handle(&relay_last);
    
    if(!(from_chan = chan_get_handle(argv[0])))
        return 0;
        
    if(chan_prefix(argv[0][0])) {
        if(!(to_chan = chan_get_handle(argv[1])))
            return 0;
            
        relay = relayed_chan(to_chan, from_chan);
    }
    else {
        if(!(to_user = user_get_handle(argv[1])))
            return 0;
        
        relay = relayed_user(to_user, from_chan);
    }
    
    if(relay == NULL)
        return 0;
    
    if(!unregister_relay(relay))
        return 0;
    
    irc_notice_user(user->nick, 0, "Relay unregistered.");
    
    return 1;
}

BOT_CMD(unrelayall) {
    relay_free_data();
    irc_notice_user(user->nick, 0, "All relays have been unregistered.");
    
    return 1;
}
