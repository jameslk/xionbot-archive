/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include "irc_def.h"

/* Xion Main Macros */
#define XION_VERSION "2.3b"
#define XION_VERSION_MSG "XionBot v2.3b by SeLfkiLL (www.jaleko.tk)"

#define XION_ERROR_MSG "XionBot ERROR:"
#define XION_WARN_MSG "XionBot WARNING:"

#define XION_CONFIG "xionbot.conf"
#define XION_LOG "xionbot.log"

/* Misc Macros */
#ifndef NULL
#define NULL ((void*) 0)
#endif

/* There will be a real purpose for these someday... */
/* Compiling under POSIX/UNIX */
#undef PLATFORM_POSIX
/* Compiling under Windows */
#define PLATFORM_WINDOWS

/* Clear the log file on start */
#define XION_FRESH_LOG 1

/*The "rediculously long" IRC/XionBot information structure.*/
struct xionbot {
    char nick[MAX_NICKLEN];
    char altnick[MAX_NICKLEN];
    char username[MAX_USERLEN];
    char info[MAX_INFOLEN];
    char current_nick[MAX_NICKLEN];
    /* // */
    char netname[33];
    char servname[257];
    char servaddr[257];
    unsigned int servport;
    char servpass[MAX_PASSLEN];
    /* // */
    char config[261];
    /* // */
    unsigned int maxretry;
    unsigned int current_try;
    /* // */
    unsigned int floodcheck;
    /* // */
    char ctrigger;  /* Channel trigger (e.g. !command) */
    char ptrigger;  /* Private Message trigger */
    /* // */
    unsigned int sock;
    /* // */
    char **admin_array;
    unsigned int admin_lines;
    char **admin_array_current;
    unsigned int admin_current_count;
} bot;

#include "log.h"
#include "tools.h"

/* Function Prototypes */
void extern_exit(int info);
void dispsplash(void);
unsigned int _init_handle(void **ptr);
#define init_handle(HANDLE) _init_handle((void*)HANDLE)
void free_xion_memory(void);
unsigned int init(void);
unsigned int rehashconfig(void);

#include "event.h"
