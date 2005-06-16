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
#include "irc_socket.h"
#include "irc_channel.h"
#include "irc_user.h"

#include "irc_commands.h"

char raw[MAX_LEN];

int irc_user(char *user, char *server, char *fullname) {
    char *nuser;
    
    clearstr(raw, MAX_LEN);
    
    if((blankstr(user)) || (blankstr(server)) || (blankstr(fullname))) return -1;
    if((strlen(user) > 10) || (strlen(server) > 32) || (strlen(fullname) > 312)) return -1;
    
    if((nuser = (char*)callocm(11, sizeof(char))) == NULL)
        return 0;
    
    sprintf(raw, "USER %s 0 \"%s\" :%s", mklower(nuser, user), server, fullname);
    
    freem(nuser);
    return irc_send(raw, 1);
}

int irc_pass(char *pass) {
    clearstr(raw, MAX_LEN);
    
    if(blankstr(pass)) return -1;
    if(strlen(pass) > 256) return -1;
    
    sprintf(raw, "PASS %s", pass);
    return irc_send(raw, 1);
}

int irc_pong(char *ping) {
    char *data;
    char *splitFrmSp, *sepFrmCol;
	int pos = 0;
    
    clearstr(raw, MAX_LEN);
    
    if(blankstr(ping)) return -1;
    if(strlen(ping) > 510) return -1;
    
    if((data = (char*)callocm(513, sizeof(char))) == NULL)
        return 0;
        
    xstrcpy(data, ping, 513);
    xstrtok(data, " ", &pos);
    splitFrmSp = xstrtok(data, " ", &pos);
    if(!splitFrmSp) {
        freem(data);
        return -2;
    }
    
    xstrcpy(data, splitFrmSp, 513);
    sepFrmCol = xstrtok(data, ":", NULL);
    if(!sepFrmCol) {
        freem(data);
        return -2;
    }
    
    sprintf(raw, "PONG :%s", sepFrmCol);
    
    freem(data);
    return irc_send(raw, 1);
}

int irc_nick(char *nick) {
    clearstr(raw, MAX_LEN);
    
    if(blankstr(nick)) return -1;
    if((strlen(nick)+1) > MAX_NICKLEN) return -1;
    
    sprintf(raw, "NICK %s", nick);
    
    if((!istrcmp(nick, bot.nick)) && (!istrcmp(nick, bot.altnick)))
        nickchange = 1;
        
    return irc_raw(raw);
}

int irc_mode(char *target, char *format, ...) {
    char *modestr;
    va_list ap;
    
    clearstr(raw, MAX_LEN);
    
    if(blankstr(target)) return -1;
    if(strlen(target) > 200) return -1;
    
    if(!blankstr(format)) {
        modestr = (char*)callocm(301, sizeof(char));
        va_start(ap, format);
        #ifndef _MSC_VER /* M$VC++ */
        vsnprintf(modestr, 300, format, ap);
        #else
        _vsnprintf(modestr, 300, format, ap);
        #endif
        va_end(ap);
        sprintf(raw, "MODE %s %s", target, modestr);
        freem(modestr);
    }
    else {
        sprintf(raw, "MODE %s", target);
    }
    
    return irc_raw(raw);
}

int irc_join(char *chan, char *pass) {
    clearstr(raw, MAX_LEN);
    
    if(blankstr(chan)) return -1;
    if(strlen(chan) > MAX_CHANLEN) return -1;
    
    if(blankstr(pass)) {
        sprintf(raw, "JOIN %s", chan);
    }
    else {
        if(strlen(pass) > MAX_PASSLEN) return -1;
        sprintf(raw, "JOIN %s %s", chan, pass);
    }
    
    return irc_raw(raw);
}

int irc_part(char *chan, char *reason) {
    clearstr(raw, MAX_LEN);
    
    if(blankstr(chan)) return -1;
    if(strlen(chan) > MAX_CHANLEN) return -1;
    
    if(blankstr(reason)) {
        sprintf(raw, "PART %s", chan);
    }
    else {
        if(strlen(reason) > 88) return -1;
        sprintf(raw, "PART %s :%s", chan, reason);
    }
    
    return irc_raw(raw);
}

int irc_privmsg_user(char *target, unsigned int flags, char *msg, ...) {
	va_list ap;
	int ret;
    
    va_start(ap, msg);
    ret = irc_msg(target, MSG_PRIVMSG, flags, msg, ap);
    va_end(ap);

	return ret;
}

int irc_privmsg_chan(char *target, unsigned int flags, char *msg, ...) {
	va_list ap;
	int ret;
    
    va_start(ap, msg);
    ret = irc_msg(target, MSG_PRIVMSG, flags, msg, ap);
    va_end(ap);

	return ret;
}

int irc_action_user(char *target, unsigned int flags, char *msg, ...) {
	va_list ap;
	int ret;
    
    va_start(ap, msg);
    ret = irc_msg(target, MSG_ACTION, flags, msg, ap);
    va_end(ap);

	return ret;
}

int irc_action_chan(char *target, unsigned int flags, char *msg, ...) {
	va_list ap;
	int ret;
    
    va_start(ap, msg);
    ret = irc_msg(target, MSG_ACTION, flags, msg, ap);
    va_end(ap);

	return ret;
}

int irc_notice_user(char *target, unsigned int flags, char *msg, ...) {
	va_list ap;
	int ret;
    
    va_start(ap, msg);
    ret = irc_msg(target, MSG_NOTICE, flags, msg, ap);
    va_end(ap);

	return ret;
}

int irc_notice_chan(char *target, unsigned int flags, char *msg, ...) {
	va_list ap;
	int ret;
    
    va_start(ap, msg);
    ret = irc_msg(target, MSG_NOTICE, flags, msg, ap);
    va_end(ap);

	return ret;
}

int irc_ctcp_user(char *target, unsigned int flags, char *msg, ...) {
	va_list ap;
	int ret;
    
    va_start(ap, msg);
    ret = irc_msg(target, MSG_CTCP, flags, msg, ap);
    va_end(ap);

	return ret;
}

int irc_ctcp_chan(char *target, unsigned int flags, char *msg, ...) {
	va_list ap;
	int ret;
    
    va_start(ap, msg);
    ret = irc_msg(target, MSG_CTCP, flags, msg, ap);
    va_end(ap);

	return ret;
}

int irc_msg(char *target, unsigned int msg_type, unsigned int flags, char *msg, va_list ap) {
    char str1[MAX_LEN];
    char str2[MAX_MSGLEN];
    
    if(target == NULL) return -1;
    if(blankstr(msg)) return -1;
    
    xstrcpy(str1, msg, MAX_MSGLEN);
    replace_alias(str1);
    
    #ifndef _MSC_VER
    vsnprintf(str2, (MAX_MSGLEN-1), str1, ap);
    #else /* M$VC++ */
    _vsnprintf(str2, (MAX_MSGLEN-1), str1, ap);
    #endif
    
    clearstr(str1, MAX_LEN);
    
    if(flags & MSG_ALIAS)
        replace_alias(str2);
    
    switch(msg_type) {
        case MSG_PRIVMSG:
            sprintf(str1, "PRIVMSG %s :%s", target, str2);
            break;
        
        case MSG_ACTION:
            sprintf(str1, "PRIVMSG %s :%cACTION %s%c", target, C_EVENT, str2, C_EVENT);
            break;
        
        case MSG_NOTICE:
            sprintf(str1, "NOTICE %s :%s", target, str2);
            break;
            
        case MSG_CTCP:
            if(flags & MSG_CTCP_REPLY)
                sprintf(str1, "NOTICE %s :%c%s%c", target, C_EVENT, str2, C_EVENT);
            else
                sprintf(str1, "PRIVMSG %s :%c%s%c", target, C_EVENT, str2, C_EVENT);
            break;
        
        default:
            sprintf(str1, "PRIVMSG %s :%s", target, str2);
            break;
    }
    
    if(flags & MSG_URGENT)
        return irc_send(str1, 1);
    else
        return irc_raw(str1);
}

int irc_ison(char *nicks) {
    clearstr(raw, MAX_LEN);
    
    if(blankstr(nicks)) return -1;
    if(strlen(nicks) > 496) return -1;
    
    sprintf(raw, "ISON %s", nicks);
    xstrcpy(lastison, nicks, 497);
    
    return irc_raw(raw);
}

int irc_userhost(char *nick) {
    clearstr(raw, MAX_LEN);
    
    if(blankstr(nick))
        return -1;
    if(strlen(nick) > MAX_NICKLEN)
        return -1;
    
    sprintf(raw, "USERHOST %s", nick);
    
    return irc_raw(raw);
}

int irc_quit(char *quitmsg) {
    unsigned int r1 = 0, r2 = 0;
    
    clearstr(raw, MAX_LEN);
    
    sprintf(raw, "JOIN 0");
    r1 = irc_send(raw, 1);
    
    memset(raw, '\0', 512);
    if(blankstr(quitmsg)) {
        if(strlen(quitmsg) > 464) return -1;
        sprintf(raw, "QUIT :%s", quitmsg);
    }
    else {
        sprintf(raw, "QUIT");
    }
    
    bot.current_try = (bot.maxretry+1);
    r2 = irc_send(raw, 1);
    
    if(r1 != 1) return r1;
    else if(r2 != 1) return r2;
    
    return (r1 & r2);
}
