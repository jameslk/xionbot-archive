/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <php_embed.h>

#include "main.h"
#include "irc_channel.h"

#include "php_global.h"
#include "php_event.h"

struct evnode {
    struct evnode *next;
    struct evnode *prev;
    
    /* For XionBot's Event Processor */
    unsigned long id;
    /* For Our Event Processor */
    unsigned long type;
    /* Can it get anymore confusing? */
    
    /* Callback function name */
    zval callback;
    
    char **targets;
    unsigned int targ_count;
    
    char *wildstr;
    
    unsigned long flags;
};

struct evnode *evlist[EVENT_TYPE_COUNT+1][2];
#define LFIRST(TYPE) evlist[TYPE][0]
#define LLAST(TYPE) evlist[TYPE][1]

#define USER_TARGETS_ONLY 0x000001
#define CHAN_TARGETS_ONLY 0x000002

#define define_const(NAME, NUM) REGISTER_LONG_CONSTANT(NAME, NUM, CONST_CS|CONST_PERSISTENT);
/* Used to initialize anything for our event processor and functions (such as exported constants) */
unsigned int phpm_eventinit(void) {
    int module_number = 0;
    unsigned int i;
    
    for(i = 0;i < EVENT_TYPE_COUNT;i++) {
        evlist[i][0] = NULL;
        evlist[i][1] = NULL;
    }
    
    define_const("ON_RAW",          ON_RAW);
    define_const("ON_CONNECT",      ON_CONNECT);
    define_const("ON_PING",         ON_PING);
    define_const("ON_PONG",         ON_PONG);
    define_const("ON_NICK",         ON_NICK);
    define_const("ON_ISON",         ON_ISON);
    define_const("ON_JOIN",         ON_JOIN);
    define_const("ON_PART",         ON_PART);
    define_const("ON_MESSAGE",      ON_MESSAGE);
    define_const("ON_NOTICE",       ON_NOTICE);
    define_const("ON_ACTION",       ON_ACTION);
    define_const("ON_CTCP",         ON_CTCP);
    define_const("ON_CTCP_REPLY",   ON_CTCP_REPLY);
    define_const("ON_MODE",         ON_MODE);
    define_const("ON_KICK",         ON_KICK);
    define_const("ON_BAN",          ON_BAN);
    define_const("ON_UNBAN",        ON_UNBAN);
    define_const("ON_OP",           ON_OP);
    define_const("ON_DEOP",         ON_DEOP);
    define_const("ON_HALFOP",       ON_OP);
    define_const("ON_DEHALFOP",     ON_DEOP);
    define_const("ON_VOICE",        ON_VOICE);
    define_const("ON_DEVOICE",      ON_DEVOICE);
    define_const("ON_INVITE",       ON_INVITE);
    define_const("ON_TOPIC",        ON_TOPIC);
    define_const("ON_DISCONNECT",   ON_DISCONNECT);
    define_const("ON_EXIT",         ON_EXIT);
    
    define_const("USER_TARGETS_ONLY", USER_TARGETS_ONLY);
    define_const("CHAN_TARGETS_ONLY", CHAN_TARGETS_ONLY);
    
    return 1;
}

static unsigned int is_event_target(struct evnode *event, char *target) {
    unsigned int i;
    
    if(event == NULL)
        return 0;
    
    if(target == NULL)
        return 1;
    
    if(event->flags && USER_TARGETS_ONLY) {
        if(chan_prefix(target[0]))
            return 0;
    }
    else if(event->flags && CHAN_TARGETS_ONLY) {
        if(!chan_prefix(target[0]))
            return 0;
    }
    
    if(event->targets == NULL)
        return 1;
    
    for(i = 0;i < event->targ_count;i++) {
        if(istrcmp(target, event->targets[i]))
            return 1;
    }
    
    return 0;
}

static unsigned int is_event_matchstr(struct evnode *event, char *regstr) {
    if(event == NULL)
        return 0;
    
    if(regstr == NULL)
        return 1;
    
    if(event->wildstr == NULL)
        return 1;
    
    if(matchstr(event->wildstr, regstr))
        return 1;
    
    return 0;
}

/* str_arg_types: s = string, l = long, d = double, b = bool, n = null, e = empty string */
static unsigned int ev_call_functions(unsigned int type, char *target, char *regstr, char *str_arg_types, ...) {
    unsigned int argc, i;
    va_list ap;
    zval ***argv, *discard;
    struct evnode *event;
    
    argc = 0;
    argv = NULL;    
    if(str_arg_types != NULL) {
        argc = strlen(str_arg_types);
    
        argv = (zval***)safe_emalloc(argc, sizeof(zval**), 0);
    
        va_start(ap, str_arg_types);
        for(i = 0;i < argc;i++) {
            argv[i] = emalloc(sizeof(zval*));
            MAKE_STD_ZVAL(*argv[i]);
            switch(str_arg_types[i]) {
                case 's':
                    ZVAL_STRING(*argv[i], va_arg(ap, char*), 1);
                    break;
                
                case 'l':
                    ZVAL_LONG(*argv[i], va_arg(ap, long));
                    break;
                
                case 'd':
                    ZVAL_DOUBLE(*argv[i], va_arg(ap, double));
                    break;
                
                case 'b':
                    ZVAL_BOOL(*argv[i], va_arg(ap, char));
                    break;
                
                case 'n':
                    ZVAL_NULL(*argv[i]);
                    break;
                
                case 'e':
                    ZVAL_EMPTY_STRING(*argv[i]);
                    break;
            }
        }
        va_end(ap);
    }
    
    for(event = LFIRST(type);event != NULL;event = event->next) {
        if((is_event_target(event, target)) && (is_event_matchstr(event, regstr))) {
            if(call_user_function_ex(CG(function_table), NULL, &event->callback,
                                        &discard, argc, argv, 0, NULL TSRMLS_CC) != SUCCESS)
                return 0;
        }
    }
    
    return 1;
}

static unsigned int ev_parse_privmsg(datalist data) {
    char *nick, *target, *ctcp_cmd, *message;
    unsigned int msg_len, r = 0, fix = 0;
    
    if(data == NULL)
        return 0;
    
    nick = dlist_v(data, 1, char*);
    target = dlist_v(data, 2, char*);
    message = dlist_v(data, 3, char*);
    
    if(message == NULL)
        return 1;
    
    msg_len = strlen(message);
    
    if(message[0] != C_EVENT) {
        /* Normal Message. */
        return ev_call_functions(ON_MESSAGE, target, message, "sss", nick, target, message);
    }
    else if(msg_len > 7) {
        if(!strncmp(message+1, "ACTION", 6)) {
            /* Action Message */
            if(message[msg_len-1] == C_EVENT) {
                message[msg_len-1] = 0;
                fix = 1;
            }
            
            r = ev_call_functions(ON_ACTION, target, message+8, "sss", nick, target, message+8);
            if(fix)
                message[msg_len-1] = C_EVENT;
            
            return r;
        }
    }
    
    /* CTCP Message */
    if((!message[1]) || (message[1] == C_EVENT))
        return 1;
    
    ctcp_cmd = (char*)callocm(msg_len+1, sizeof(char));
    if(ctcp_cmd == NULL)
        return 0;
            
    if(message[msg_len-1] == C_EVENT) {
        message[msg_len-1] = 0;
        fix = 1;
    }
            
    strcpy(ctcp_cmd, message+1);
    ctcp_cmd = xstrtok(ctcp_cmd, " ", &r, 1);
    msg_len = strlen(ctcp_cmd)+2;
    
    if(message[msg_len])
        r = ev_call_functions(ON_CTCP, target, message+1, "ssss", nick, target, ctcp_cmd, message+msg_len);
    else
        r = ev_call_functions(ON_CTCP, target, message+1, "ssse", nick, target, ctcp_cmd, NULL);
            
    if(fix)
        message[msg_len] = C_EVENT;
    
    freem(ctcp_cmd);    
    return r;
}

static unsigned int ev_parse_notice(datalist data) {
    char *nick, *target, *ctcp_reply, *notice;
    unsigned int len, r = 0, fix = 0;
    
    if(data == NULL)
        return 0;
    
    nick = dlist_v(data, 1, char*);
    target = dlist_v(data, 2, char*);
    notice = dlist_v(data, 3, char*);
    
    if(notice == NULL)
        return 1;
    
    len = strlen(notice);
    
    if(notice[0] != C_EVENT) {
        /* Normal Notice. */
        return ev_call_functions(ON_NOTICE, target, notice, "sss", nick, target, notice);
    }
    
    /* CTCP Reply */
    if((!notice[1]) || (notice[1] == C_EVENT))
        return 1;
    
    ctcp_reply = (char*)callocm(len+1, sizeof(char));
    if(ctcp_reply == NULL)
        return 0;
            
    if(notice[len-1] == C_EVENT) {
        notice[len-1] = 0;
        fix = 1;
    }
            
    strcpy(ctcp_reply, notice+1);
    ctcp_reply = xstrtok(ctcp_reply, " ", &r, 1);
    len = strlen(ctcp_reply)+2;
    
    if(notice[len])
        r = ev_call_functions(ON_CTCP, target, notice+1, "ssss", nick, target, ctcp_reply, notice+len);
    else
        r = ev_call_functions(ON_CTCP, target, notice+1, "ssse", nick, target, ctcp_reply, NULL);
            
    if(fix)
        notice[len] = C_EVENT;
    
    freem(ctcp_reply);    
    return 1;
}

/* Core processor of events passed from irc to user functions. */
EVENT_FUNC(phpm_parse_event) {
    unsigned int r;
    
    switch(event_id) {
        case EVENT_IRCRAW:
            r = ev_call_functions(ON_RAW, NULL, NULL, "s", dlist_v(event_data, 0, char*));
            break;
        
        case EVENT_IRCSTART:
            if(dlist_v(event_data, 0, int) == 2)
                r = ev_call_functions(ON_CONNECT, NULL, NULL, NULL, NULL);
            break;
        
        case EVENT_IRCPING:
            r = ev_call_functions(ON_PING, NULL, dlist_v(event_data, 1, char*),
                                                "ss", dlist_v(event_data, 1, char*),
                                                dlist_v(event_data, 0, char*));
            break;
        
        case EVENT_IRCPONG:
            r = ev_call_functions(ON_PONG, NULL, dlist_v(event_data, 1, char*),
                                                "ss", dlist_v(event_data, 1, char*),
                                                dlist_v(event_data, 0, char*));
            break;
        
        case EVENT_IRCNICK:
            r = ev_call_functions(ON_NICK, NULL, dlist_v(event_data, 1, char*),
                                                "sss", dlist_v(event_data, 1, char*),
                                                dlist_v(event_data, 2, char*),
                                                dlist_v(event_data, 0, char*));
            break;
        
        case EVENT_IRCISON:
            if(blankstr(dlist_v(event_data, 1, char*)))
                r = ev_call_functions(ON_ISON, NULL, NULL,
                                                    "es", NULL,
                                                    dlist_v(event_data, 0, char*));
            else
                r = ev_call_functions(ON_ISON, NULL, dlist_v(event_data, 1, char*),
                                                    "ss", dlist_v(event_data, 1, char*),
                                                    dlist_v(event_data, 0, char*));
            
            break;
        
        case EVENT_IRCJOIN:
            ev_call_functions(ON_JOIN, dlist_v(event_data, 2, char*), NULL,
                                            "sss", dlist_v(event_data, 1, char*),
                                            dlist_v(event_data, 2, char*),
                                            dlist_v(event_data, 0, char*));
            break;
        
        case EVENT_IRCPART:
            if(blankstr(dlist_v(event_data, 3, char*)))
                ev_call_functions(ON_PART, dlist_v(event_data, 2, char*), NULL,
                                                "sses", dlist_v(event_data, 1, char*),
                                                dlist_v(event_data, 2, char*),
                                                NULL,
                                                dlist_v(event_data, 0, char*));
            else
                ev_call_functions(ON_PART, dlist_v(event_data, 2, char*), NULL,
                                                "ssss", dlist_v(event_data, 1, char*),
                                                dlist_v(event_data, 2, char*),
                                                dlist_v(event_data, 3, char*),
                                                dlist_v(event_data, 0, char*));
            
            break;
        
        case EVENT_IRCPRIVMSG:
            r = ev_parse_privmsg(event_data);
            break;
        
        case EVENT_IRCNOTICE:
            r = ev_parse_notice(event_data);
            break;
        
        case EVENT_IRCMODE:
            break;
        
        case EVENT_IRCKICK:
            if(blankstr(dlist_v(event_data, 3, char*)))
                r = ev_call_functions(ON_KICK, dlist_v(event_data, 2, char*), NULL,
                                                        "ssses", dlist_v(event_data, 1, char*),
                                                        dlist_v(event_data, 2, char*),
                                                        dlist_v(event_data, 3, char*),
                                                        NULL,
                                                        dlist_v(event_data, 0, char*));
            else
                r = ev_call_functions(ON_KICK, dlist_v(event_data, 2, char*), NULL,
                                                        "sssss", dlist_v(event_data, 1, char*),
                                                        dlist_v(event_data, 2, char*),
                                                        dlist_v(event_data, 3, char*),
                                                        dlist_v(event_data, 4, char*),
                                                        dlist_v(event_data, 0, char*));
            
            break;
        
        case EVENT_IRCINVITE:
            r = ev_call_functions(ON_INVITE, dlist_v(event_data, 3, char*), NULL,
                                                    "sss", dlist_v(event_data, 1, char*),
                                                    dlist_v(event_data, 3, char*),
                                                    dlist_v(event_data, 0, char*));
            break;
        
        case EVENT_IRCTOPIC:
            if(blankstr(dlist_v(event_data, 3, char*)))
                r = ev_call_functions(ON_TOPIC, dlist_v(event_data, 2, char*), NULL,
                                                        "sses", dlist_v(event_data, 1, char*),
                                                        dlist_v(event_data, 2, char*),
                                                        NULL,
                                                        dlist_v(event_data, 0, char*));
            else
                r = ev_call_functions(ON_TOPIC, dlist_v(event_data, 2, char*), NULL,
                                                        "ssss", dlist_v(event_data, 1, char*),
                                                        dlist_v(event_data, 2, char*),
                                                        dlist_v(event_data, 3, char*),
                                                        dlist_v(event_data, 0, char*));
            
            break;
        
        case EVENT_IRCQUIT:
            if(blankstr(dlist_v(event_data, 1, char*)))
                r = ev_call_functions(ON_DISCONNECT, NULL, NULL,
                                                            "es", NULL,
                                                            dlist_v(event_data, 0, char*));
            else
                r = ev_call_functions(ON_DISCONNECT, NULL, NULL,
                                                            "ss", dlist_v(event_data, 1, char*),
                                                            dlist_v(event_data, 0, char*));
            
            break;
        
        case EVENT_EXIT:
            ev_call_functions(ON_EXIT, NULL, NULL, NULL, NULL);
            break;
    }
    
    if(!r)
        make_error("phpm_parse_event failed somewhere.");
    
    return ;
}

const long event_type_conversion[] = {
        /* ON_RAW */        EVENT_IRCRAW,
        /* ON_CONNECT */    EVENT_IRCSTART,
        /* ON_PING */       EVENT_IRCPING,
        /* ON_PONG */       EVENT_IRCPONG,
        /* ON_NICK */       EVENT_IRCNICK,
        /* ON_ISON */       EVENT_IRCISON,
        /* ON_JOIN */       EVENT_IRCJOIN,
        /* ON_PART */       EVENT_IRCPART,
        /* ON_MESSAGE */    EVENT_IRCPRIVMSG,
        /* ON_NOTICE */     EVENT_IRCNOTICE,
        /* ON_ACTION */     EVENT_IRCPRIVMSG,
        /* ON_CTCP */       EVENT_IRCPRIVMSG,
        /* ON_CTCP_REPLY */ EVENT_IRCNOTICE,
        /* ON_MODE */       EVENT_IRCMODE,
        /* ON_KICK */       EVENT_IRCKICK,
        /* ON_BAN */        EVENT_IRCMODE,
        /* ON_UNBAN */      EVENT_IRCMODE,
        /* ON_OP */         EVENT_IRCMODE,
        /* ON_DEOP */       EVENT_IRCMODE,
        /* ON_HALFOP */     EVENT_IRCMODE,
        /* ON_DEHALFOP */   EVENT_IRCMODE,
        /* ON_VOICE */      EVENT_IRCMODE,
        /* ON_DEVOICE */    EVENT_IRCMODE,
        /* ON_INVITE */     EVENT_IRCINVITE,
        /* ON_TOPIC */      EVENT_IRCTOPIC,
        /* ON_DISCONNECT */ EVENT_IRCQUIT,
        /* ON_EXIT */       EVENT_EXIT};
/* bool bind_event(int event_type, callback event_func [, string targets, string wildstr, long flags]) */
ZEND_FUNCTION(bind_event) {
    struct evnode *event;
    long event_type, flags;
    zval *event_func;
    char *event_func_name, *wildstr, *targets;
    int wildstr_len, targets_len, i, ii, pos;
    
    if(ZEND_NUM_ARGS() < 2)
		WRONG_PARAM_COUNT;
    
    flags = 0;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz|ssl", &event_type, &event_func,
                                                                    &targets, &targets_len,
                                                                    &wildstr, &wildstr_len,
                                                                    &flags) != SUCCESS)
		RETURN_FALSE;
	
	if((event_type < 0) || (flags < 0))
	    RETURN_FALSE;
	
	if(strlen(Z_STRVAL_P(event_func)) > 512)
        RETURN_FALSE;
	
	if(!zend_is_callable(event_func, 0, &event_func_name)) {
        efree(event_func_name);
        RETURN_FALSE;
    }
    efree(event_func_name);
    
    if(!event_hook(phpm_parse_event, 0, event_type_conversion[event_type], 0, NULL))
        RETURN_FALSE;
    
    event = (struct evnode*)mallocm(sizeof(struct evnode));
    if(event == NULL) {
        make_error("Failed to allocate memory for PHP bind_event.");
        RETURN_FALSE
    }
    
    event->id = event_type_conversion[event_type];
    event->type = event_type;
    ZVAL_STRING(&event->callback, Z_STRVAL_P(event_func), 1);
    event->flags = flags;
    
    if(targets_len < 1) {
        event->targets = NULL;
        event->targ_count = 0;
    }
    else {
        event->targ_count = 0;
        
        strrtok(targets, " ");
        
        while(targets[0] == ',')
            targets++;
        if(!targets[0]) {
            event->targets = NULL;
            goto end;
        }
        
        for(i = 0, targets_len = 1;targets[i];i++) {
            if((targets[i] == ',') && (targets[i+1] != 0))
                targets_len++;
        }
        
        event->targets = (char**)mallocm(targets_len*sizeof(char*));
        if(event->targets == NULL) {
            make_warning("Failed to allocate memory for bind_event: targets, skipping.");
            goto end;
        }
        
        event->targ_count = targets_len;        
        for(i = 0, pos = 0;i < targets_len;i++) {
            for(ii = 0;targets[ii+pos];ii++) {
                if(targets[ii+pos] == ',')
                    break;
            }
            if(!ii)
                continue;
            
            event->targets[i] = (char*)callocm(ii+1, sizeof(char));
            xstrcpy(event->targets[i], targets+pos, ii+1);
            pos += ii+1;
        }
    }
    
    if(wildstr_len < 1) {
        event->wildstr = NULL;
    }
    else {
        event->wildstr = (char*)callocm(wildstr_len+1, sizeof(char));
        if(event->wildstr != NULL)
            xstrcpy(event->wildstr, wildstr, wildstr_len+1);
        else
            make_warning("Failed to allocate memory for bind_event: wildstr, skipping.");
    }
    
    end:
    LL_ADDNODE(event, LFIRST(event_type), LLAST(event_type))
    
    RETURN_TRUE;
}

/* bool unbind_event(?) */
ZEND_FUNCTION(unbind_event) {
    RETURN_TRUE;
}