/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

struct chanNode;
struct chanlist;

/* Note even though a user is added to the list,
    it may not be a user that we're in a chan with.
    (use the relate variable to test this)
*/
struct userNode {
    struct userNode *next;
    struct userNode *prev;
    
    unsigned int relate; /* If we're in any channels with this user. */
    
    struct chanlist *chanlist_first;
    struct chanlist *chanlist_last;
    unsigned int chan_count;
    
    char nick[MAX_NICKLEN];
    char user[MAX_USERLEN];
    char host[MAX_MASKLEN];
    char hostmask[MAX_HOSTMASKLEN]; /* nick!user@host put together (I'm lazy) */
    char info[MAX_INFOLEN];
    
    unsigned long user_mode;
};

/* User's channel list */
struct chanlist {
    struct chanlist *next;
    struct chanlist *prev;
    
    struct chanNode *chan;
    struct userNode *user;
    
    struct userlist *chanuser_info;
};

unsigned long user_count;
struct userNode *user_first;
struct userNode *user_last;

struct userNode *me;

/* Function Prototypes */
struct userNode* user_get_handle(char *nick);
static void init_user_struct(struct userNode *user);
struct userNode* user_adduser(char *nick);
unsigned int user_deluser(struct userNode *user);
struct chanlist* user_get_chanlist(struct userNode *user, struct chanNode *chan);
struct chanlist* user_add_userchan(struct userNode *user, struct chanNode *chan);
unsigned int user_del_userchan(struct chanlist *hChanList);
unsigned int user_fillmask(struct userNode *user, char *hostmask);
void user_free_chanlist(struct userNode *user);
void user_free_data(void);
