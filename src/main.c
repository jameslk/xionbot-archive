/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "main.h"
#include "conf.h"
#include "irc_socket.h"
#include "irc_parse.h"
#include "irc_commands.h"
#include "irc_admin.h"

#include "irc_user.h"
#include "irc_channel.h"
#include "irc_mode.h"

#include "irc_botcmd.h"

#include "mod-irc_relay.h"
#include "mod-weburlcache.h"

/* A list for keeping track of everything
   that needs to be initiliazed. */
struct handle_list {
    struct handle_list *next;
    void **handle;
} *handle_first, *handle_last;

int main(int argc, char *argv[]) {
    char console[MAX_LEN];
    unsigned int err = 0;
    extern unsigned long alloc_calls;
    extern unsigned long free_calls;
        
    handle_first = (struct handle_list*)mallocm(sizeof(struct handle_list));
    if(handle_first == NULL) {
        make_error("Failed to allocate memory for internal handle list!");
        getchar();
        return 1;
    }
    handle_first->next = NULL;
    handle_first->handle = (void*)&handle_first;
    handle_last = handle_first;
    
    dispsplash();
    
    printf("Initializing... ");
    if(!init()) {
        make_error("Initialization failed (init() is 0).");
        free_xion_memory();
        getchar();
        return 1;
    }
    bot_cmd_init();
    printf("Done.\n");
    
    if(!log_clean())
        make_warning("Failed to clear log file.");
    log_write("*** Program started. Executed as: %s", argv[0]);
    
    printf("Connecting to %s:%d... ", bot.servaddr, bot.servport);
    if((err = irc_connect(bot.servaddr, bot.servport)) != 0) {
        printf("Failed to connect. ERROR %d\n\n", err);
        log_write("Connecting failed with code: %d", err);
        printf("Press Enter key to continue.");
        getchar();
        return 0;
    }
    printf("Connected.\n");
    
    signal(SIGINT, extern_exit);
    
    while(1) {
        fgets(console, 512, stdin);
        if(console[0] == '\n') break;
        strrtok(console, "\r\n");
        if(istrcmp(console, "exit")) break;
        else if(istrcmp(console, "rehash")) {
            printf("\nRehashing configuration...\n");
            if(!rehashconfig())
                printf("Failed.");
            else
                printf("Done.");
        }
        else irc_send(console, 1);
    }
    
    irc_quit(NULL);
    
    free_xion_memory();
    
    event_call(EVENT_EXIT, 0);
    
    log_write("*** Program ended successfully. Allocation Calls: %d Free Calls: %d", alloc_calls, free_calls);
    
    return 0;
}

void extern_exit(int info) {
    extern unsigned long alloc_calls;
    extern unsigned long free_calls;
    
    if(info == SIGINT)
        log_write("*** Recieved CTRL-C...");
    
    irc_quit("Program Terminated");
    free_xion_memory();
    event_call(EVENT_EXIT, 0);
    log_write("*** Program ended successfully. Allocation Calls: %d Free Calls: %d", alloc_calls, free_calls);
    exit(EXIT_SUCCESS);
    
    return ;
}

/* Use this instead of exit() to prevent memory leaks */
void clean_exit(unsigned int error) {
    extern unsigned long alloc_calls;
    extern unsigned long free_calls;
    
    irc_disconnect();
    free_xion_memory();
    event_call(EVENT_EXIT, 0);
    if(error) {
        log_write("*** Program was halted due to an error. Allocation Calls: %d Free Calls: %d", alloc_calls, free_calls);
        exit(EXIT_FAILURE);
    }
    else {
        log_write("*** Program was halted. Allocation Calls: %d Free Calls: %d", alloc_calls, free_calls);
        exit(EXIT_SUCCESS);
    }
    
    return ;
}

void dispsplash(void) {
    printf("--------------------------------------------------\n");
    printf("%s\n", XION_VERSION_MSG);
    printf("--------------------------------------------------\n\n\n");
}

unsigned int _init_handle(void **ptr) {
    struct handle_list *check;
    
    for(check = handle_first;check != NULL;check = check->next) {
        if(ptr == check->handle)
            return 1;
    }
    
    *ptr = NULL;
    if(handle_first == NULL) {
        handle_first->next = NULL;
        handle_first->handle = ptr;
        handle_last = handle_first;
    }
    else {
        check = (struct handle_list*)mallocm(sizeof(struct handle_list));
        if(check == NULL) {
            make_error("Failed to allocate memory for internal handle list!");
            return 0;
        }
        handle_last->next = check;
        check->next = NULL;
        check->handle = ptr;
        handle_last = check;
    }
    
    return 0;
}

void free_xion_memory(void) {
    unsigned int i = 0;
    struct send_q *qtmp, *qtmp2;
    struct handle_list *temp_hlist, *temp_hlist2;
    
    if(bot.admin_array != NULL) {
        for(i = 0;i < bot.admin_lines;i++) freem(bot.admin_array[i]);
        freem(bot.admin_array);
    }
    
    if(bot.admin_array_current != NULL) {
        for(i = 0;i < bot.admin_current_count;i++) freem(bot.admin_array_current[i]);
        freem(bot.admin_array_current);
    }
    
    while(q_first != NULL) {
        for(qtmp = q_first;qtmp->next != NULL;qtmp = qtmp->next);
        qtmp2 = qtmp->prev;
        freem(qtmp);
        if(qtmp2 == NULL) break;
        qtmp2->next = NULL;
    }
    
    user_free_data();
    chan_free_data();
    botcmd_free_data();
    relay_free_data();
    
    /* These need to be the last things freed. */
    event_free_data();
    
    temp_hlist = handle_first;
    while(temp_hlist != NULL) {
        temp_hlist2 = temp_hlist->next;
        freem(temp_hlist);
        if(temp_hlist2 == NULL) break;
        temp_hlist = temp_hlist2;
    }
    
    return ;
}

/* This function should not be called more than once */
unsigned int init(void) {
    char **xlines, **clines;
    char *conftok = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    unsigned int i = 0, xcount = 0, ccount = 0,
    acount = 0; /* Don't get this confused with the word "account" */
    extern q_maxbytes, q_maxqueue;
    
    /* For making sure when these variables
        are tested, they are set to NULL first. */
    init_handle(&q_first);
    init_handle(&q_last);
    init_handle(&event_first);
    init_handle(&event_last);
    init_handle(&chan_first);
    init_handle(&chan_last);
    init_handle(&user_first);
    init_handle(&user_last);
    init_handle(&me);
    init_handle(&bc_first);
    init_handle(&bc_last);
    
    bot.connected = 0;
    
    xstrcpy(bot.config, XION_CONFIG, 260);
    
    bot.admin_current_count = 0;
    bot.current_try = 1;
    q_first = NULL;
    
    /*X:Line*/
    xcount = get_confitem(&xlines, 'X');
    if(!xcount) {
        make_error("No X:lines in configuration file.");
        freem(conftok);
        return 0;
    }
    /*nickname*/    xstrcpy(bot.nick, get_itemtok(conftok, xlines[0], 2), 32);
    
    /*altnick*/     xstrcpy(bot.altnick, get_itemtok(conftok, xlines[0], 3), 32);
    
    /*username*/    xstrcpy(bot.username, get_itemtok(conftok, xlines[0], 4), 12);
    
    /*fullname*/    xstrcpy(bot.info, get_itemtok(conftok, xlines[0], 5), 256);
                    xstrcpy(bot.info, conf_replace_alias(conftok), 256);
    
    
    /*C:Line*/
    ccount = get_confitem(&clines, 'C');
    if(!ccount) {
        make_error("No C:lines in configuration file.");
        freem(conftok);
        free_2d_array(xlines, xcount);
        return 0;
    }
    /*servaddr*/    xstrcpy(bot.servaddr, get_itemtok(conftok, clines[0], 2), 256);
    
    /*servport*/    bot.servport = atoi(get_itemtok(conftok, clines[0], 3));
    
    /*servpass*/    xstrcpy(bot.servpass, get_itemtok(conftok, clines[0], 4), 256);
    
    
    /*S:Lines*/
    /*maxretry*/    irc_subconftok(conftok, 'S', "maxretry", 3);
                    bot.maxretry = atoi(conftok);
                    
    /*pingtimeout*/ irc_subconftok(conftok, 'S', "pingtimeout", 3);
                    bot.ping_timeout = atoi(conftok);
    
    /*freshlog*/    irc_subconftok(conftok, 'S', "freshlog", 3);
                    bot.fresh_log = (atoi(conftok) ? 1 : 0);
                    
    /*antiflood*/   irc_subconftok(conftok, 'S', "antiflood", 5);
                    bot.floodcheck = (atoi(conftok) ? 1 : 0);
                    irc_subconftok(conftok, 'S', "antiflood", 3);
                    q_maxbytes = (unsigned int)atoi(conftok);
                    irc_subconftok(conftok, 'S', "antiflood", 4);
                    q_maxqueue = (unsigned int)atoi(conftok);
                    
    /*ctrigger*/    irc_subconftok(conftok, 'S', "ctrigger", 3);
                    conf_replace_alias(conftok);
                    bot.ctrigger = conftok[0];
                    
    /*ptrigger*/    irc_subconftok(conftok, 'S', "ptrigger", 3);
                    conf_replace_alias(conftok);
                    bot.ptrigger = conftok[0];
                    
    /*A:Lines*/
    if(bot.admin_array != NULL)
        free_2d_array(bot.admin_array, bot.admin_lines);
    
    acount = get_confitem(&bot.admin_array, 'A');
    bot.admin_lines = acount;
    if(acount) {
        for(i = 0;i < bot.admin_lines;i++) {
            get_itemtok(conftok, bot.admin_array[i], 3);
            if((istrcmp(conftok, ";")) || (istrcmp(conftok, "*")))
                adm_loginuser(get_itemtok(conftok, bot.admin_array[i], 2),
                                get_itemtok(conftok, bot.admin_array[i], 4),
                                atoi(get_itemtok(conftok, bot.admin_array[i], 5)));
        }
    }
    
    /* URL Module: mod-weburlcache.c */
    urlmod_init();
    
    freem(conftok);
    free_2d_array(xlines, xcount);
    free_2d_array(clines, ccount);
    
    event_call(EVENT_INIT, 0);
    
    return 1;
}

unsigned int rehashconfig(void) {
    char **xlines;
    char *conftok = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    unsigned int i = 0, xcount = 0,
    acount = 0; /* Don't get this confused with the word "account" */
    extern q_maxbytes;
    
    /*X:Line*/
    xcount = get_confitem(&xlines, 'X');
    if(!xcount) {
        make_error("No X:lines in configuration file.");
        freem(conftok);
        return 0;
    }
    /*nickname*/    xstrcpy(bot.nick, get_itemtok(conftok, xlines[0], 2), 32);
    if(!istrcmp(bot.current_nick, bot.nick))
        irc_nick(bot.nick);
    /*altnick*/     xstrcpy(bot.altnick, get_itemtok(conftok, xlines[0], 3), 32);
    
    /*S:Lines*/
    /*maxretry*/    irc_subconftok(conftok, 'S', "maxretry", 3);
                    bot.maxretry = atoi(conftok);
                    
    /*pingtimeout*/ irc_subconftok(conftok, 'S', "pingtimeout", 3);
                    bot.ping_timeout = atoi(conftok);
    
    /*antiflood*/   irc_subconftok(conftok, 'S', "antiflood", 5);
                    bot.floodcheck = (atoi(conftok) ? 1 : 0);
                    irc_subconftok(conftok, 'S', "antiflood", 3);
                    q_maxbytes = (unsigned int)atoi(conftok);
    
    /*ctrigger*/    irc_subconftok(conftok, 'S', "ctrigger", 3);
                    conf_replace_alias(conftok);
                    bot.ctrigger = conftok[0];
    
    /*ptrigger*/    irc_subconftok(conftok, 'S', "ptrigger", 3);
                    conf_replace_alias(conftok);
                    bot.ptrigger = conftok[0];
                    
    /*A:Lines*/
    if(bot.admin_array != NULL)
        free_2d_array(bot.admin_array, bot.admin_lines);
    
    acount = get_confitem(&bot.admin_array, 'A');
    bot.admin_lines = acount;
    if(acount) {
        for(i = 0;i < bot.admin_lines;i++) {
            get_itemtok(conftok, bot.admin_array[i], 3);
            if((istrcmp(conftok, ";")) || (istrcmp(conftok, "*")))
                adm_loginuser(get_itemtok(conftok, bot.admin_array[i], 2),
                                get_itemtok(conftok, bot.admin_array[i], 4),
                                atoi(get_itemtok(conftok, bot.admin_array[i], 5)));
        }
    }
    
    freem(conftok);
    free_2d_array(xlines, xcount);
    
    event_call(EVENT_REHASH, 0);
    
    return 1;
}
