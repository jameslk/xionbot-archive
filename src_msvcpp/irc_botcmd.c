/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"
#include "irc_socket.h"
#include "irc_parse.h"
#include "irc_channel.h"
#include "irc_user.h"
#include "irc_commands.h"
#include "irc_botcmd.h"
#include "dcc.h"

#include "mod-irc_relay.h"

static struct botcmd_list* get_botcmd_handle(char *alias) {
    struct botcmd_list *bc;
    
    for(bc = bc_first;bc != NULL;bc = bc->next) {
        if(istrcmp(bc->alias, alias))
            return bc;
    }
    
    return NULL;
}

static struct botcmd_list* get_bch_by_func(botcmd_func func) {
    struct botcmd_list *bc;
    
    for(bc = bc_first;bc != NULL;bc = bc->next) {
        if(bc->func == func)
            return bc;
    }
    
    return NULL;
}

unsigned int register_bot_cmd(char *alias, botcmd_func func, unsigned int minarg, unsigned int cmd_type, unsigned long flags) {
    struct botcmd_list *bc;
    
    if(blankstr(alias))
        return 0;
    
    if(get_botcmd_handle(alias))
        return 0;
    
    if(!cmd_type)
        return 0;
    
    if((bc = get_bch_by_func(func)) != NULL) {
        if(bc->flags & BC_NOT_BINDABLE)
            return 0;
    }
    
    bc = (struct botcmd_list*)mallocm(sizeof(struct botcmd_list));
    if(bc == NULL)
        return 0;
    
    bc->next = NULL;
    bc->prev = NULL;
    xstrcpy(bc->alias, alias, MAX_MSGLEN);
    bc->func = func;
    bc->minarg = minarg;
    bc->cmd_type = cmd_type;
    bc->flags = flags;
    
    LL_ADDNODE(bc, bc_first, bc_last)
    
    bc_count++;
    return 1;
}

unsigned int unregister_bot_cmd(struct botcmd_list *bc) {
    if(bc == NULL)
        return 0;
    
    if(bc->flags & BC_NOT_UNBINDABLE)
        return 0;
    
    LL_DELNODE(bc, bc_first, bc_last)
    
    bc_count--;
    freem(bc);
    return 1;
}

unsigned int bot_cmd(char *alias, unsigned int cmd_type, char *nick, char *target, char *param) {
    struct botcmd_list *bc;
    unsigned int argc, s = 1;
    char **argv = NULL;
    struct userNode *user;
    struct chanNode *chan;
    struct chanlist *hChanList;
    
    if((blankstr(alias)) || (blankstr(nick)) || (blankstr(target)))
        return 0;
    
    bc = get_botcmd_handle(alias);
    if(bc == NULL)
        return 0;
    
    if(!(bc->cmd_type & cmd_type))
        return 0;
    
    argc = make_argument_array(&argv, param);
    if(argc < bc->minarg)
        return 0;
    
    switch(cmd_type) {
        case BC_PRIVMSG:
        case BC_PRIVCTCP:
            if((user = user_get_handle(nick)) == NULL) {
                if((user = user_adduser(nick)) == NULL) {
                    if(argc)
                        free_2d_array(argv, argc);
                    return 0;
                }
            }
            
            s = bc->func(alias, cmd_type, user, NULL, target, argc, argv);
            break;
        
        case BC_CHANMSG:
        case BC_CHANCTCP:
            if((user = user_get_handle(nick)) == NULL) {
                if(argc)
                    free_2d_array(argv, argc);
                return 0;
            }
            
            if((chan = chan_get_handle(target)) == NULL) {
                if(argc)
                    free_2d_array(argv, argc);
                return 0;
            }
            hChanList = user_get_chanlist(user, chan);
            
            s = bc->func(alias, cmd_type, user, chan, target, argc, argv);
            break;
        
        default:
            if(argc)
                free_2d_array(argv, argc);
            freem(argv);
            return 0;
    }
    
    if(argc)
        free_2d_array(argv, argc);
    
    if(!s)
        make_warning("A bot command failed.");
    return s;
}

void botcmd_free_data(void) {
    struct botcmd_list *temp_bch, *temp_bch2;
    
    temp_bch = bc_last;
    while(temp_bch != NULL) {
        temp_bch2 = temp_bch->prev;
        freem(temp_bch);
        if(temp_bch2 == NULL) break;
        temp_bch2->next = NULL;
        temp_bch = temp_bch2;
    }
    
    bc_count = 0;
    return ;
}

static BOT_CMD(floodcheck) {
    extern q_floodcheck_msg;
    
    if((istrcmp(user->nick, bot.current_nick)) && (istrcmp(target, bot.current_nick))) {
        /* Got Flood Check Message */
        printf("*** Got Flood Check Message\n");
        if(q_floodcheck_msg)
            q_floodcheck_msg = 0;
    }
    
    if(irc_queue(NULL, 1)) {
        return 1;
    }
    else {
        make_error("irc_queue failed -- turning off flood checking.");
        bot.floodcheck = 0;
        return 0;
    }
}

static BOT_CMD(version) {
    switch(cmd_type) {
        case BC_PRIVMSG:
            irc_notice_user(user->nick, 0, XION_VERSION_MSG);
            break;
        
        case BC_CHANMSG:
            irc_privmsg_chan(chan->name, 0, "$b%s$b: %s", user->nick, XION_VERSION_MSG);
            break;
        
        case BC_PRIVCTCP:
        case BC_CHANCTCP:
            irc_ctcp_user(user->nick, MSG_CTCP_REPLY, "VERSION %s", XION_VERSION_MSG);
            break;
    }
    
    return 1;
}

static BOT_CMD(ping) {
    time_t t, delay;
    
    time(&t);
    irc_ctcp_user(user->nick, MSG_CTCP_REPLY, "PING %d", t);
    delay = t-atol(argv[0]);
    if(delay < 0)
        delay = 0;
    printf("*** PING %s: Ping delay of %d second(s).\n", user->nick, delay);
    
    return 1;
}

static BOT_CMD(help) {
    irc_notice_user(user->nick, 0, "Usage:");
    irc_notice_user(user->nick, 0, "In Channel: %c<command>", bot.ctrigger);
    if(bot.ptrigger == ';')
        irc_notice_user(user->nick, 0, "In Privmsg: <command>");
    else
        irc_notice_user(user->nick, 0, "In Privmsg: %c<command>", bot.ptrigger);
    if(cmd_type == BC_PRIVMSG) {
        if(bot.ptrigger == ';')
            irc_notice_user(user->nick, 0, "Type commands for a list of commands.");
        else
            irc_notice_user(user->nick, 0, "Type %ccommands for a list of commands.", bot.ptrigger);
    }
    else {
        irc_notice_user(user->nick, 0, "Type %ccommands for a list of commands.", bot.ctrigger);
    }
    
    return 1;
}

static BOT_CMD(dcc) {
    unsigned short port;
    char *notice;
    struct dccNode *dcc;
    
    if(istrcmp(argv[0], "CHAT") && istrcmp(argv[1], "chat")) {
        port = (short)atoi(argv[3]);
        if(!port)
            return 1;
        
        dcc = dcc_connect(user, argv[2], port);
        
        if(dcc == NULL)
            return 0;
        
        notice = (char*)calloc(27+strlen(dcc->user->nick)+strlen(dcc->ip), sizeof(char));
        if(notice == NULL)
            return 0;
        
        sprintf(notice, "DCC CHAT started with %s (%s).", dcc->user->nick, dcc->ip);
        make_notice(notice);
        freem(notice);
    }
    
    return 1;
}

static BOT_CMD(say) {
    struct chanNode *send_chan;
    char *str;
    
    switch(cmd_type) {
        case BC_PRIVMSG:
            if(argc < 2)
                return 0;
            send_chan = chan_get_handle(argv[0]);
            if(send_chan == NULL)
                return 0;
            
            if(send_chan->me == NULL)
                return 0;
            
            if(!bind_argument_array(&str, argv, argc, 1))
                return 0;
                
            irc_privmsg_chan(send_chan->name, MSG_ALIAS, "[%s] %s", user->nick, str);
            break;
        
        case BC_CHANMSG:
            if(!bind_argument_array(&str, argv, argc, 0))
                return 0;
                
            irc_privmsg_chan(chan->name, MSG_ALIAS, "%s", str);
            break;
    }
    
    freem(str);
    return 1;
}

static BOT_CMD(commands) {
    struct botcmd_list *bc;
    
    switch(cmd_type) {
        case BC_PRIVMSG:
            for(bc = bc_first;bc != NULL;bc = bc->next) {
                if(bot.ptrigger != ';')
                    irc_notice_user(user->nick, 0, "%c%s", bot.ptrigger, bc->alias);
                else
                    irc_notice_user(user->nick, 0, bc->alias);
            }
            break;
        
        case BC_CHANMSG:
            for(bc = bc_first;bc != NULL;bc = bc->next)
                irc_privmsg_chan(chan->name, 0, "%c%s", bot.ctrigger, bc->alias);
            break;
    }
    
    return 1;
}

static BOT_CMD(bind) {
    struct botcmd_list *bc;
    
    bc = get_botcmd_handle(argv[1]);
    if(bc == NULL) {
        irc_notice_user(user->nick, 0, "%s is an unknown command.", argv[1]);
        return 0;
    }
    
    if(!register_bot_cmd(argv[0], bc->func, bc->minarg, bc->cmd_type, 0)) {
        irc_notice_user(user->nick, 0, "Unable to create bind.");
        return 0;
    }
    
    irc_notice_user(user->nick, 0, "%s has been binded to %s.", argv[0], argv[1]);
    
    return 1;
}

static BOT_CMD(unbind) {
    struct botcmd_list *bc;
    
    bc = get_botcmd_handle(argv[0]);
    if(bc == NULL) {
        irc_notice_user(user->nick, 0, "%s is an unknown command.", argv[0]);
        return 0;
    }
    
    if(!unregister_bot_cmd(bc)) {
        irc_notice_user(user->nick, 0, "Unable to unbind command.");
        return 0;
    }
    
    irc_notice_user(user->nick, 0, "%s has been unbinded.", argv[0]);
    return 1;
}

static BOT_CMD(chan) {
    struct chanNode *external_chan;
    
    if(argc > 0) {
        external_chan = chan_get_handle(argv[0]);
        if(external_chan == NULL)
            return 0;
            
        if(external_chan->me == NULL)
            return 0;
    }
    
    switch(cmd_type) {
        case BC_PRIVMSG:
            if(argc < 1)
                return 0;
                
            irc_notice_user(user->nick, 0, "%s info:", external_chan->name);
            irc_notice_user(user->nick, 0, "- Topic: %s", external_chan->topic);
            irc_notice_user(user->nick, 0, "- User Count: %d", external_chan->user_count);
            irc_notice_user(user->nick, 0, "- Ban Count: %d", external_chan->ban_count);
            if(external_chan->mode & MODE_KEY)
                irc_notice_user(user->nick, 0, "- Chan Key: %s", external_chan->key);
            break;
            
        case BC_CHANMSG:
            if(argc < 1)
                external_chan = chan;
            irc_privmsg_chan(chan->name, 0, "%s info:", external_chan->name);
            irc_privmsg_chan(chan->name, 0, "- Topic: %s", external_chan->topic);
            irc_privmsg_chan(chan->name, 0, "- User Count: %d", external_chan->user_count);
            irc_privmsg_chan(chan->name, 0, "- Ban Count: %d", external_chan->ban_count);
            if(external_chan->mode & MODE_KEY)
                irc_privmsg_chan(chan->name, 0, "- Chan Key: %s", external_chan->key);
            break;
    }
    
    return 1;
}

static BOT_CMD(info) {
    irc_notice_user(user->nick, 0, "Bot Information:");
    irc_notice_user(user->nick, 0, "- Version: %s", XION_VERSION);
    irc_notice_user(user->nick, 0, "- Nick: %s", bot.nick);
    irc_notice_user(user->nick, 0, "- Alternate: %s", bot.altnick);
    irc_notice_user(user->nick, 0, "- Username: %s", bot.username);
    irc_notice_user(user->nick, 0, "- Info: %s", bot.info);
    irc_notice_user(user->nick, 0, "- Network Name: %s", bot.netname);
    irc_notice_user(user->nick, 0, "- Server Name: %s", bot.servname);
    irc_notice_user(user->nick, 0, "- Server Address: %s", bot.servaddr);
    irc_notice_user(user->nick, 0, "- Port: %d", bot.servport);
    irc_notice_user(user->nick, 0, "- Config: %s", bot.config);
    irc_notice_user(user->nick, 0, "- Max Reconnects: %d", bot.maxretry);
    irc_notice_user(user->nick, 0, "- Ping Timeout: %d seconds", bot.ping_timeout);
    irc_notice_user(user->nick, 0, "- Fresh Log: %s", bot.fresh_log ? "yes" : "no");
    irc_notice_user(user->nick, 0, "- Floodchecking: %s", bot.floodcheck ? "on" : "off");
    irc_notice_user(user->nick, 0, "- Chan Trigger: %c", bot.ctrigger);
    if(bot.ptrigger == ';')
        irc_notice_user(user->nick, 0, "- MSG Trigger: ");
    else
        irc_notice_user(user->nick, 0, "- MSG Trigger: %c", bot.ptrigger);
    irc_notice_user(user->nick, 0, "- Uptime: %ld seconds", time(NULL)-stats.start_time);
    irc_notice_user(user->nick, 0, "- Events: %d", stats.event_count);
    irc_notice_user(user->nick, 0, "- Memory Allocations: %d", stats.alloc_calls);
    irc_notice_user(user->nick, 0, "- Memory Freed: %d", stats.free_calls);
    
    return 1;
}

static BOT_CMD(raw) {
    char *str;
    
    /* Temporary password until we get an account system in place. */
    if(!istrcmp(argv[0], "k3wlz")) {
        irc_notice_user(user->nick, 0, "Incorrect password.");
        return 1;
    }
    
    if(!bind_argument_array(&str, argv, argc, 1))
        return 0;
    
    log_write("BotCMD Raw from %s: %s", user->nick, str);
    irc_raw(str);
    
    freem(str);    
    return 1;
}

EVENT_FUNC(cmd_ban_user) {
    char *cmd_nick;
    struct userNode *ban_user;
    struct chanNode *chan;
    
    ban_user = user_get_handle(dlist_v(event_data, 0, char*));
    if(ban_user == NULL)
        return ;
    
    cmd_nick = dlist_v(hook_data, 0, char*);
    if(blankstr(cmd_nick))
        return ;
    
    if(!istrcmp(ban_user->nick, cmd_nick)) {
        freem(cmd_nick);
        return ;
    }
    
    chan = dlist_v(hook_data, 1, struct chanNode*);
    
    if(chan->me == NULL) {
        freem(cmd_nick);
        return ;
    }
        
    irc_mode(chan->name, "+b *!*@%s", ban_user->host);
    
    freem(cmd_nick);
    return ;
}

static BOT_CMD(ban) {
    struct userNode *arg_user;
    struct chanNode *arg_chan;
    unsigned int retn_val;
    char *nick;
    
    nick = (char*)callocm(MAX_NICKLEN, sizeof(char));
    if(nick == NULL)
        return 0;
    
    switch(cmd_type) {
        case BC_PRIVMSG:
            if(argc < 2)
                return 0;
            xstrcpy(nick, argv[1], MAX_NICKLEN);
            arg_chan = chan_get_handle(argv[0]);
            arg_user = user_get_handle(nick);
            if(arg_chan == NULL)
                return 0;
            break;
            
        case BC_CHANMSG:
            xstrcpy(nick, argv[0], MAX_NICKLEN);
            arg_chan = chan;
            arg_user = user_get_handle(nick);
            break;
    }
            
    if((arg_user == NULL) || (blankstr(arg_user->host))) {
            retn_val = event_hook(cmd_ban_user, EVFLAG_CALL_ONCE | EVFLAG_NEW_EVENT, EVENT_IRCRPL_USERHOST, 2, nick, arg_chan) != NULL ? 1 : 0;
            irc_userhost(nick);
    }
    else if(arg_user != NULL) {
        irc_mode(arg_chan->name, "+b *!*@%s", arg_user->host);
        freem(nick);
    }
    
    return retn_val;
}

static BOT_CMD(uptime) {
    char *uptime;
    struct dur_t dur;
    
    if(!duration(&dur, time(NULL) - stats.start_time))
        return 0;
    
    uptime = (char*)callocm(64, sizeof(char));
    if(uptime == NULL)
        return 0;
    
    if(dur.years)
        sprintf(uptime, "%d years", dur.years);
    if(dur.months)
        sprintf(uptime, "%s %d months", uptime, dur.months);
    if(dur.weeks)
        sprintf(uptime, "%s %d weeks", uptime, dur.weeks);
    if(dur.days)
        sprintf(uptime, "%s %d days", uptime, dur.days);
    if(dur.hours)
        sprintf(uptime, "%s %d hours", uptime, dur.hours);
    if(dur.minutes)
        sprintf(uptime, "%s %d minutes", uptime, dur.minutes);
    if(dur.seconds)
        sprintf(uptime, "%s %d seconds", uptime, dur.seconds);
    
    if(uptime[0] == ' ')
        uptime++;
    
    switch(cmd_type) {
        case BC_PRIVMSG:
            irc_notice_user(user->nick, 0, "Uptime: %s", uptime);
            break;
        
        case BC_CHANMSG:
            irc_privmsg_chan(chan->name, 0, "Uptime: %s", uptime);
            break;
    }
    
    free(uptime);    
    return 1;
}

/* Register all our bot commands in main. */
unsigned int bot_cmd_init(void) {
    unsigned int s = 1;
    
    s &= register_bot_cmd("floodcheck", bot_cmd_floodcheck, 0, BC_PRIVCTCP, (BC_NOT_UNBINDABLE|BC_NOT_BINDABLE));
    
    s &= register_bot_cmd("version", bot_cmd_version, 0, (BC_PRIVMSG|BC_CHANMSG|BC_PRIVCTCP|BC_CHANCTCP), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("ping", bot_cmd_ping, 1, (BC_PRIVCTCP|BC_CHANCTCP), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("dcc", bot_cmd_dcc, 4, BC_PRIVCTCP, (BC_NOT_UNBINDABLE|BC_NOT_BINDABLE));
    
    s &= register_bot_cmd("help", bot_cmd_help, 0, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("say", bot_cmd_say, 1, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("commands", bot_cmd_commands, 0, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("bind", bot_cmd_bind, 2, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("unbind", bot_cmd_unbind, 1, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("chan", bot_cmd_chan, 0, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("info", bot_cmd_info, 0, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("raw", bot_cmd_raw, 2, BC_PRIVMSG, BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("ban", bot_cmd_ban, 1, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("uptime", bot_cmd_uptime, 0, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    
    s &= register_bot_cmd("relay", bot_cmd_relay, 2, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("relays", bot_cmd_relays, 0, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("unrelay", bot_cmd_unrelay, 2, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    s &= register_bot_cmd("unrelayall", bot_cmd_unrelayall, 0, (BC_PRIVMSG|BC_CHANMSG), BC_NOT_UNBINDABLE);
    
    if(!s) {
        make_warning("Unable to initialize a bot command.");
        return 0;
    }
    
    return 1;
}
