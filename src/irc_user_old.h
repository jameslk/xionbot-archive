/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

/*
Files to be included:
(1) irc_def.h
*/

struct chanNode;

typedef struct chanuserlist {
    struct chanuserlist *next;
    struct chanuserlist *prev;
    
    struct chanNode *chan;
    
    char nick[MAX_NICKLEN];
    unsigned char mask[MAX_MASKLEN];
    
    unsigned int mode;
} chusr_t;

chusr_t* chusr_getlast(chusr_t *usrfirst);
chusr_t* chusr_nametoptr(chusr_t *usrfirst, char *nick);
chusr_t* chusr_addusr(struct chanNode *chan, char *nick);
unsigned int chusr_usrmodes(struct chanNode *chan, char *modestr, char *nick);
unsigned int chusr_topaccess(chusr_t *usr);
