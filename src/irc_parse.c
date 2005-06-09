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
#include "conf.h"
#include "irc_user.h"
#include "irc_channel.h"
#include "irc_parse.h"
#include "irc_commands.h"
#include "irc_tools.h"
#include "irc_admin.h"
#include "irc_socket.h"
#include "irc_mode.h"
#include "irc_botcmd.h"

#include "mod-irc_relay.h"

#define NEW_IRC_STR (char*)callocm(strlen(raw)+1, sizeof(char))

unsigned int irc_parsenumeric(unsigned int numeric, char *raw) {
    unsigned int r = 0;
    
    switch(numeric) {
        case 001:
            irc_get_servname(bot.servname, raw);
            irc_userhost(me->nick);
            r = 1;
            break;
            
        case 005:
            irc_get_netname(bot.netname, raw);
            r = 1;
            break;
        
        case 302:
            r = irc_parse_302(raw);
            if(!r)
                make_warning("irc_parse_302 failed.");
            break;
            
        case 303:
            r = irc_parse_ison(raw);
            if(!r)
                make_warning("irc_parse_ison failed.");
            break;
            
        case 324:
            r = irc_parse_324(raw);
            if(!r)
                make_warning("irc_parse_324 failed.");
            break;
            
        case 332:
            r = irc_parse_332(raw);
            if(!r)
                make_warning("irc_parse_332 failed.");
            break;
        
        case 353:
            r = irc_parse_353(raw);
            if(!r)
                make_warning("irc_parse_353 failed.");
            break;
            
        case 367:
            r = irc_parse_367(raw);
            if(!r)
                make_warning("irc_parse_367 failed.");
            break;
            
        case 376:
        case 422:
            r = irc_start(2);
            break;
        
        case 433:
            if(istrcmp(bot.current_nick, bot.nick)) break;
            xstrcpy(bot.current_nick, bot.altnick, 32);
            r = irc_nick(bot.altnick);
            if(!r)
                make_warning("irc_nick (433) failed.");
            mynickison = 1;
            mkthread(irc_fightfornick, NULL);
            break;
        
        case 451:
            /* Start over. (got "Register First" message) */
            r = irc_start(1);
            break;
        
        default:
            r = 1;
    }
    
    return r;
}

#define PARSECMD(CMP) else if(istrcmp(cmdtype, CMP))
#define PARSECMD2(CMP) else if(istrcmp(cmdtype2, CMP))
unsigned int irc_parseraw(char *raw) {
    char *cmdtype, *cmdtype2;
    unsigned int r = 0;
    
    if(blankstr(raw))
        return 0;
    
    event_call(EVENT_IRCRAW, 1, raw);
    
    cmdtype = NEW_IRC_STR;
    cmdtype2 = NEW_IRC_STR;
    
    if((cmdtype == NULL) || (cmdtype2 == NULL))
        return 0;
    
    irc_get_cmdtype(cmdtype, raw);
    irc_get_cmdtype2(cmdtype2, raw);
    
    if(atoi(blankstr(cmdtype) ? "0" : cmdtype)) {
        r = irc_parsenumeric(atoi(cmdtype), raw);
        if(!r)
            make_warning("irc_parsenumeric failed.");
    }
    
    PARSECMD2("PING") {
        r = irc_parse_ping(raw);
        if(!r)
            make_warning("irc_parse_ping failed.");
    }
    PARSECMD2("PONG") {
        r = irc_parse_pong(raw);
        if(!r)
            make_warning("irc_parse_pong failed.");
    }
    PARSECMD("PRIVMSG") {
        r = irc_parse_privmsg(raw);
        if(!r)
            make_warning("irc_parse_privmsg failed.");
    }
    PARSECMD("NOTICE") {
        r = irc_parse_notice(raw);
        if(!r)
            make_warning("irc_parse_notice failed.");
    }
    PARSECMD("INVITE") {
        r = irc_parse_invite(raw);
        if(!r)
            make_warning("irc_parse_invite failed.");
    }
    PARSECMD("JOIN") {
        r = irc_parse_join(raw);
        if(!r)
            make_warning("irc_parse_join failed.");
    }
    PARSECMD("PART") {
        r = irc_parse_part(raw);
        if(!r)
            make_warning("irc_parse_part failed.");
    }
    PARSECMD("KICK") {
        r = irc_parse_kick(raw);
        if(!r)
            make_warning("irc_parse_kick failed.");
    }
    PARSECMD("MODE") {
        r = irc_parse_mode(raw);
        if(!r)
            make_warning("irc_parse_mode failed.");
    }
    PARSECMD("QUIT") {
        r = irc_parse_quit(raw);
        if(!r)
            make_warning("irc_parse_quit failed.");
    }
    PARSECMD("TOPIC") {
        r = irc_parse_topic(raw);
        if(!r)
            make_warning("irc_parse_topic failed.");
    }
    PARSECMD("NICK") {
        r = irc_parse_nick(raw);
        if(!r)
            make_warning("irc_parse_nick failed.");
    }
    
    else {
        r = 1;
    }
    
    freem(cmdtype);
    freem(cmdtype2);
    return r;
}

int irc_start(unsigned int next) {
    char **plines, *conftok;
    unsigned int i = 0, l = 0;
    struct userNode *user;
    
    switch(next) {
        case 1:
            /* Just connecting stuff */
            if(!istrcmp(bot.servpass, ";"))
                irc_pass(bot.servpass);
            
            irc_user(bot.username, bot.servaddr, bot.info);
            irc_nick(bot.nick);
            
            if(user_first != NULL)
                user_free_data();
            if(chan_first != NULL)
                chan_free_data();
            if(relay_first != NULL)
                relay_free_data();
            
            user = user_adduser(bot.nick);
            user->relate = 1;
            me = user;
            break;
            
        case 2:
            /* Auto-Perform stuff */
            if(!istrcmp(bot.current_nick, bot.altnick))
                xstrcpy(bot.current_nick, bot.nick, 32);
                
            l = get_confitem(&plines, 'P');
            if(!l)
                break;
            
            conftok = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
            if(conftok == NULL) {
                free_2d_array(plines, l);
                break;
            }
                
            for(i = 0;i < l;i++) {
                get_itemtok(conftok, plines[i], 2);
                irc_raw(conf_replace_alias(conftok));
            }
            
            freem(conftok);
            free_2d_array(plines, l);
            break;
    }
    
    event_call(EVENT_IRCSTART, 1, next);
    
    return 1;
}

PARSE_FUNC(ping) {
    char *ping;
    
    ping = NEW_IRC_STR;
    if(ping == NULL)
        return 0;
    
    irc_get_msg(ping, raw);
    if(!blankstr(ping))
        event_call(EVENT_IRCPING, 2, raw, ping);
    
    freem(ping);
    return irc_pong(raw);
}

PARSE_FUNC(pong) {
    char *pong;
    
    pong = NEW_IRC_STR;
    if(pong == NULL)
        return 0;
    
    irc_get_msg(pong, raw);
    if(!blankstr(pong))
        event_call(EVENT_IRCPONG, 2, raw, pong);
    
    freem(pong);
    return 1;
}

#define TARG_CHAN 0
#define TARG_PRIV 1
#define TARG_CCTCP 2
#define TARG_PCTCP 3
PARSE_FUNC(privmsg) {
    char *target, *nick, *message;
    char *alias, *params;
    unsigned int len, msg_len, targtype = 0; /* 0 = Channel, 1 = Private, 2 = Channel CTCP, 3 = Private CTCP */
    
    len = strlen(raw)+1;
    
    nick = NEW_IRC_STR;
    target = NEW_IRC_STR;
    message = NEW_IRC_STR;
    
    if((nick == NULL) || (target == NULL) || (message == NULL))
        return 0;
    
    irc_get_nick(nick, raw);
    irc_get_target(target, raw);
    irc_get_msg(message, raw);
    
    event_call(EVENT_IRCPRIVMSG, 4, raw, nick, target, message);
    relay_message(raw, target, nick);
    
    if(blankstr(message))
        goto ignore;
    
    msg_len = strlen(message);
    
    if(!chan_prefix(target[0]))
        targtype = TARG_PRIV;
    
    alias = message;
    if(message[0] == C_EVENT) {
        if(message[msg_len-1] == C_EVENT)
            message[msg_len-1] = 0;
        
        alias = message+1;
        
        if(!alias[0])
            goto ignore;
        
        if(!targtype)
            targtype = TARG_CCTCP;
        else
            targtype = TARG_PCTCP;
    }
    
    alias = xstrtok(alias, " ", NULL);
    if((!targtype) || (bot.ptrigger != ';')) {
        if(alias[0] != (targtype ? bot.ptrigger : bot.ctrigger))
            goto ignore;
        
        alias++;
        if(!alias[0])
            goto ignore;
    }
    
    len = strlen(alias)+2;
    if(message[len])
        params = message+len;
    else
        params = NULL;
    
    switch(targtype) {
        case TARG_PRIV:
            bot_cmd(alias, BC_PRIVMSG, nick, nick, params);
            break;
        
        case TARG_CCTCP:
            bot_cmd(alias, BC_CHANCTCP, nick, target, params);
            break;
        
        case TARG_PCTCP:
            bot_cmd(alias, BC_PRIVCTCP, nick, nick, params);
            break;
        
        case TARG_CHAN:
        default:
            bot_cmd(alias, BC_CHANMSG, nick, target, params);
            break;
    } 
    
    multi_free(nick, target, message, NULL);
    return 1;
    
    ignore:
    multi_free(nick, target, message, NULL);
    return 2;
}

PARSE_FUNC(notice) {
    char *nick, *target, *message;
    
    nick = NEW_IRC_STR;
    message = NEW_IRC_STR;
    target = NEW_IRC_STR;
    
    if((nick == NULL) || (message == NULL) || (target == NULL))
        return 0;
    
    irc_get_nick(nick, raw);
    irc_get_target(target, raw);
    irc_get_msg(message, raw);
    event_call(EVENT_IRCNOTICE, 4, raw, nick, target, message);
    
    freem(nick);
    freem(target);
    freem(message);    
    return 1;
}

PARSE_FUNC(invite) {
    char *inviter, *target, *chan;
    
    inviter = NEW_IRC_STR;
    target = NEW_IRC_STR;
    chan = NEW_IRC_STR;
    if((inviter == NULL) || (target == NULL) || (chan == NULL))
        return 0;
    
    irc_get_nick(inviter, raw);
    irc_get_target(target, raw);
    irc_get_msg(chan, raw);
    event_call(EVENT_IRCINVITE, 4, raw, inviter, target, chan);
    
    freem(inviter);
    freem(target);
    freem(chan);
    return 1;
}

PARSE_FUNC(join) {
    char *temp;
    struct chanNode *chan;
    struct userNode *user;
    struct chanlist *hChanList;
    struct userlist *hUserList;
    
    if(blankstr(raw))
        return 0;
        
    temp = NEW_IRC_STR;
    if(temp == NULL)
        return 0;
    
    irc_get_nick(temp, raw);
    user = user_get_handle(temp);
    if(!user) {
        user = user_adduser(temp);
        if(!user) {
            freem(temp);
            return 0;
        }
    }
    
    clearstr(temp, strlen(raw)+1);
    irc_get_target(temp, raw);
    
    if(istrcmp(user->nick, bot.current_nick)) {
        chan = chan_addchan(temp);
        if(chan == NULL)
            return 0;
        chan->chantype = temp[0];
        
        hUserList = chan_add_chanuser(user, chan);
        if(hUserList == NULL)
            return 0;
        hChanList = user_add_userchan(user, chan);
        if(hChanList == NULL)
            return 0;
        hChanList->chanuser_info = hUserList;
        
        chan->me = hUserList;
        
        irc_mode(chan->name, NULL);
        irc_mode(chan->name, "+b");
    }
    else {
        if(!(chan = chan_get_handle(temp))) {
            freem(temp);
            return 0;
        }
        
        hUserList = chan_add_chanuser(user, chan);
        hChanList = user_add_userchan(user, chan);
        hChanList->chanuser_info = hUserList;
        
        user_fillmask(user, irc_get_mask(temp, raw));
    }
    
    user->relate = 1;
    
    event_call(EVENT_IRCJOIN, 3, raw, user->nick, chan->name);
    
    freem(temp);
    return 1;
}

PARSE_FUNC(part) {
    char *temp;
    struct chanNode *chan;
    struct userNode *user;
    
    if(blankstr(raw))
        return 0;
    
    temp = NEW_IRC_STR;
    if(temp == NULL)
        return 0;
    
    irc_get_nick(temp, raw);
    user = user_get_handle(temp);
    
    clearstr(temp, strlen(raw)+1);
    irc_get_target(temp, raw);
    chan = chan_get_handle(temp);
    
    if(!user || !chan) {
        freem(temp);
        return 0;
    }
    
    clearstr(temp, strlen(raw)+1);
    irc_get_msg(temp, raw);
    
    event_call(EVENT_IRCPART, 4, raw, user->nick, chan->name, temp);
    
    if(istrcmp(user->nick, bot.current_nick)) {
        user_del_userchan(user_get_chanlist(user, chan));
        chan_delchan(chan);
    }
    else {
        chan_del_chanuser(chan_get_userlist(user, chan));
        user_del_userchan(user_get_chanlist(user, chan));
        
        if(user->chanlist_first == NULL)
            user_deluser(user);
            /* User is not in anymore channels so let's remove his
            node since it is irrelevent to us anymore and to
            prevent abuse */
    }
    
    freem(temp);
    return 1;
}

PARSE_FUNC(kick) {
    char *temp, *kicker, *chan_name;
    unsigned int key_set;
    struct chanNode *chan;
    struct userNode *user;
    int pos = 0;
    
    if(blankstr(raw))
        return 0;
    
    temp = NEW_IRC_STR;
    if(temp == NULL)
        return 0;
    
    strcpy(temp, raw);
    
    xstrtok(temp, " ", &pos);
    xstrtok(temp, " ", &pos);
    xstrtok(temp, " ", &pos);
    user = user_get_handle(xstrtok(temp, " ", &pos));
    
    clearstr(temp, strlen(raw)+1);
    irc_get_target(temp, raw);
    chan = chan_get_handle(temp);
    
    if(!user || !chan) {
        freem(temp);
        return 0;
    }
    
    clearstr(temp, strlen(raw)+1);
    irc_get_msg(temp, raw);
    
    kicker = NEW_IRC_STR;
    if(kicker == NULL) {
        freem(temp);
        return 0;
    }
    
    irc_get_nick(kicker, raw);
    event_call(EVENT_IRCKICK, 5, raw, user->nick, kicker, chan->name, temp);
    freem(kicker);
    
    if(istrcmp(user->nick, bot.current_nick)) {
        chan_name = (char*)callocm(MAX_CHANLEN, sizeof(char));
        if(chan_name == NULL) {
            freem(temp);
            return 0;
        }
        xstrcpy(chan_name, chan->name, MAX_CHANLEN);
        if(chan->mode & MODE_KEY) {
            clearstr(temp, strlen(raw)+1);
            xstrcpy(temp, chan->key, strlen(raw)+1);
            key_set = 1;
        }
        else {
            key_set = 0;
        }
        chan_delchan(chan);
        irc_join(chan_name, key_set ? temp : NULL);
        freem(chan_name);
    }
    else {
        chan_del_chanuser(chan_get_userlist(user, chan));
        user_del_userchan(user_get_chanlist(user, chan));
        
        if(user->chanlist_first == NULL)
            user_deluser(user);
    }
    
    freem(temp);
    return 1;
}

PARSE_FUNC(mode) {
    char *temp, *setter;
    struct chanNode *chan;
    struct userNode *user;
    
    if(blankstr(raw))
        return 0;
    
    temp = NEW_IRC_STR;
    if(temp == NULL)
        return 0;
    
    setter = NEW_IRC_STR;
    if(temp == NULL) {
        freem(temp);
        return 0;
    }
    
    irc_get_nick(setter, raw);
    
    clearstr(temp, strlen(raw)+1);
    irc_get_target(temp, raw);
    
    if(chan_prefix(*temp)) {
        chan = chan_get_handle(temp);
        if(chan == NULL) {
            freem(temp);
            return 0;
        }
        
        if(!mode_chan(chan, (strstr(raw, temp)+strlen(temp)+1))) {
            freem(temp);
            freem(setter);
            return 0;
        }
        if(!mode_chanparam(chan, setter, (strstr(raw, temp)+strlen(temp)+1))) {
            freem(temp);
            freem(setter);
            return 0;
        }
        
        if((user = user_get_handle(setter)) != NULL) {
            clearstr(temp, strlen(raw)+1);
            irc_get_mask(temp, raw);
            user_fillmask(user, temp);
        }
    }
    
    event_call(EVENT_IRCMODE, 2, raw, setter);
    
    freem(temp);
    freem(setter);
    return 1;
}

PARSE_FUNC(quit) {
    char *nick;
    struct userNode *user;
    
    if(blankstr(raw))
        return 0;
    
    nick = NEW_IRC_STR;
    if(nick == NULL)
        return 0;
    
    irc_get_nick(nick, raw);
    
    if(!(user = user_get_handle(nick))) {
        freem(nick);
        return 0;
    }
    
    event_call(EVENT_IRCQUIT, 2, raw, user->nick);
    
    if(!user_deluser(user)) {
        freem(nick);
        return 0;
    }
    
    freem(nick);    
    return 1;
}

PARSE_FUNC(topic) {
    char *temp;
    struct chanNode *chan;
    struct userNode *user;
    
    if(blankstr(raw))
        return 0;
    
    temp = (char*)callocm(513, sizeof(char));
    if(temp == NULL)
        return 0;
    
    chan = chan_get_handle(irc_get_target(temp, raw));
    if(!chan)
        return 0;
    
    irc_get_msg(temp, raw);
    xstrcpy(chan->topic, temp, MAX_TOPICLEN);
    
    clearstr(temp, strlen(raw)+1);
    user = user_get_handle(irc_get_nick(temp, raw));
    if(!user) {
        freem(temp);
        return 0;
    }
        
    clearstr(temp, strlen(raw)+1);
    user_fillmask(user, irc_get_mask(temp, raw));
    
    event_call(EVENT_IRCTOPIC, 3, raw, user->nick, chan->name, chan->topic);
    
    freem(temp);        
    return 1;
}

PARSE_FUNC(nick) {
    char *temp, *oldnick;
    struct userNode *user;
    
    if(blankstr(raw))
        return 0;
    
    temp = NEW_IRC_STR;
    if(temp == NULL)
        return 0;
    
    irc_get_nick(temp, raw);
    
    if(istrcmp(temp, bot.current_nick)) {
        xstrcpy(bot.current_nick, (strstr(raw, " :")+2), MAX_NICKLEN);
        
        if((!istrcmp(bot.current_nick, bot.nick)) && (nickchange != 1)) {
            mynickison = 1;
            mkthread(irc_fightfornick, NULL);
        }
        nickchange = 0;
    }
    
    if(!(user = user_get_handle(temp)))
        user = user_adduser(temp);
    
    oldnick = (char*)callocm(strlen(user->nick)+1, sizeof(char));
    if(oldnick == NULL) {
        freem(temp);
        return 0;
    }
    
    strcpy(oldnick, user->nick);
    
    xstrcpy(user->nick, (strstr(raw, " :")+2), MAX_NICKLEN);
    clearstr(temp, strlen(raw)+1);
    user_fillmask(user, irc_get_mask(temp, raw));
    
    event_call(EVENT_IRCNICK, 3, raw, user->nick, oldnick);
    
    freem(temp);
    freem(oldnick);
    return 1;
}

PARSE_FUNC(ison) {
    char *temp = (char*)callocm(MAX_LEN, sizeof(char));
    
    if(temp == NULL)
        return 0;
    
    if(!*(char*)irc_get_msg(temp, raw)) {
        if(!istrcmp(bot.current_nick, bot.nick)) {
            if(istrcmp(lastison, bot.nick)) {
                mynickison = 0;
                irc_nick(bot.nick);
            }
        }
    }
    
    event_call(EVENT_IRCISON, 2, raw, temp);
    
    freem(temp);
    return 1;
}

/* RPL_USERHOST */
PARSE_FUNC(302) {
    char *temp, *nick, *hostmask;
    struct userNode *user;
    unsigned int len, i, oper = 0;
    int pos = 0;
    
    if(blankstr(raw))
        return 0;
    
    temp = NEW_IRC_STR;
    if(temp == NULL)
        return 0;
    
    irc_get_msg(temp, raw);
    if(blankstr(temp)) {
        freem(temp);
        return 1;
    }
    
    nick = xstrtok(temp, "=", &pos);
    len = strlen(nick)-1;
    if(nick[len] == '*') {
        nick[len] = 0;
        oper = 1;
    }
    if(len+1 > MAX_NICKLEN)
        nick[MAX_NICKLEN] = 0;
    
    user = user_get_handle(nick);
    if(user == NULL) {
        user = user_adduser(nick);
        if(user == NULL) {
            freem(temp);
            return 0;
        }
        user->relate = 0;
    }
    
    hostmask = xstrtok(temp, "=", &pos);
    if((*hostmask == '+') || (*hostmask == '-'))
        hostmask++;
    
    len = strlen(hostmask);
    for(i = 0;i < len;i++) {
        if(hostmask[i] == ' ') {
            hostmask[i] = 0;
            break;
        }
    }
    if(len > MAX_HOSTMASKLEN)
        hostmask[MAX_HOSTMASKLEN] = 0;
    
    sprintf(temp, "%s!%s", nick, hostmask);
    hostmask = temp;
    
    user_fillmask(user, hostmask);
    if(oper)
        user->user_mode |= MODE_OPER;
        
    event_call(EVENT_IRCRPL_USERHOST, 2, user->nick, user->hostmask);
    
    freem(temp);
    return 1;
}

/* RPL_TOPIC */
PARSE_FUNC(332) {
    char *temp, *temp2, *temp3;
    int pos = 0;
    
    if(blankstr(raw))
        return 0;
    
    temp = NEW_IRC_STR;
    if(temp == NULL)
        return 0;
    
    strcpy(temp, raw);
    
    xstrtok(temp, " ", &pos);
    xstrtok(temp, " ", &pos);
    temp3 = xstrtok(temp, " ", &pos);
    if(!istrcmp(temp3, bot.current_nick)) {
        freem(temp);
        return 1;
    }
    
    temp3 = xstrtok(temp, " ", &pos);
    if(chan_get_handle(temp3) == NULL) {
        freem(temp);
        return 1;
    }
    
    temp2 = strstr(raw, " :");
    temp2 += 2;
    xstrcpy(chan_get_handle(temp3)->topic, temp2, MAX_LEN);
    
    event_call(EVENT_IRCRPL_TOPIC, 2, raw, temp2);
    
    freem(temp);
    return 1;
}

/* RPL_CHANNELMODEIS */
PARSE_FUNC(324) {
    char *temp, *tptr;
    struct chanNode *chan;
    int pos = 0;
    
    if(blankstr(raw))
        return 0;
    
    temp = NEW_IRC_STR;
    if(temp == NULL)
        return 0;
    
    xstrcpy(temp, raw, strlen(raw));
    xstrtok(temp, " ", &pos);
    xstrtok(temp, " ", &pos);
    xstrtok(temp, " ", &pos);
    tptr = xstrtok(temp, " ", &pos);
    
    if(chan_prefix(*tptr)) {
        chan = chan_get_handle(tptr);
        if(chan == NULL) {
            freem(temp);
            return 0;
        }
        mode_chan(chan, (strstr(raw, tptr)+strlen(tptr)+1));
    }
    
    event_call(EVENT_IRCRPL_CHANNELMODEIS, 1, raw);
    
    freem(temp);
    return 1;
}

/* RPL_NAMREPLY */
PARSE_FUNC(353) {
    unsigned int isnick;
    char *temp, *temp2;
    struct chanNode *chan;
    struct userNode *user;
    struct chanlist *hChanList;
    struct userlist *hUserList;
    int pos = 0;
    
    if(blankstr(raw))
        return 0;
    
    temp = NEW_IRC_STR;
    if(temp == NULL) {
        return 0;
    }
    
    xstrcpy(temp, (strstr(raw, bot.current_nick)+strlen(bot.current_nick)+3), strlen(raw)+1);
    chan = chan_get_handle(xstrtok(temp, " ", NULL));
    if(!chan) {
        freem(temp);
        return 0;
    }
    
    clearstr(temp, strlen(raw)+1);
    strcpy(temp, strstr(raw, " :")+2);
    
    for(temp2 = xstrtok(temp, " ", &pos);temp2 != NULL;temp2 = xstrtok(temp, " ", &pos)) {
        isnick = irc_char_type(*temp2, IS_TYPE_ALPHA);
        isnick = isnick || irc_char_type(*temp2, IS_TYPE_NUMERIC);
        isnick = isnick || irc_char_type(*temp2, IS_TYPE_SPECIAL);
        
        if(isnick) {
            if(!user_get_handle(temp2)) {
                if(!(user = user_adduser(temp2))) {
                    freem(temp);
                    return 0;
                }
                hUserList = chan_add_chanuser(user, chan);
                hChanList = user_add_userchan(user, chan);
                hChanList->chanuser_info = hUserList;
            }
        }
        else {
            if(!(user = user_get_handle(temp2+1))) {
                if(!(user = user_adduser(temp2+1))) {
                    freem(temp);
                    return 0;
                }
            }
            
            if(!(hUserList = chan_get_userlist(user, chan)))
                hUserList = chan_add_chanuser(user, chan);
            if(!(hChanList = user_get_chanlist(user, chan)))
                hChanList = user_add_userchan(user, chan);
            hChanList->chanuser_info = hUserList;
            
            switch(*temp2) {
                case C_OP:
                    hUserList->chan_mode |= MODE_OP;
                    break;
                
                case C_HALFOP:
                    hUserList->chan_mode |= MODE_HALFOP;
                    break;
                
                case C_VOICE:
                    hUserList->chan_mode |= MODE_VOICE;
            }
        }
    }
    
    event_call(EVENT_IRCRPL_NAMEREPLY, 2, raw, chan->name);
    
    freem(temp);
    return 1;
}

/* RPL_BANLIST */
PARSE_FUNC(367) {
    char *temp, *temp2;
    struct chanNode *chan;
    struct banlist *ban;
    int pos = 0;
    
    if(blankstr(raw))
        return 0;
    
    temp = NEW_IRC_STR;
    if(temp == NULL)
        return 0;
    
    strcpy(temp, raw);
    
    xstrtok(temp, " ", &pos);
    xstrtok(temp, " ", &pos);
    xstrtok(temp, " ", &pos);
    chan = chan_get_handle(xstrtok(temp, " ", &pos));
    if(chan == NULL)
        return 0;
    
    temp2 = xstrtok(temp, " ", &pos);
    mode_delban(chan, temp2);
    ban = mode_addban(chan, temp2);
    if(ban == NULL)
        return 0;
    
    xstrcpy(ban->setby, xstrtok(temp, " ", &pos), MAX_LEN);
    ban->date = atoi(xstrtok(temp, " ", &pos));
    
    event_call(EVENT_IRCRPL_BANLIST, 3, raw, chan->name, ban);
    
    freem(temp);
    return 1;
}
