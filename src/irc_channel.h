/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

/*
Files to be included:
(1) irc_user.h
*/

struct userNode;
struct chanlist;

/* Note even though this channel is added to the list,
    it may not be an actual channel that we're in.
    (use the me variable to test this)
*/
struct chanNode {
    struct chanNode *next; /* Pointer to the next element in our list. */
    struct chanNode *prev; /* Pointer to the previous element in our list. */
    
    char name[MAX_CHANLEN]; /* Channel name (include prefix). */
    char chantype; /* What kind of channel is this? (&#+) */
    
    unsigned long mode; /* Channel modes. */
    char topic[MAX_TOPICLEN]; /* Current channel topic. */
    unsigned int userlimit; /* Channel user limit (if any). */
    char key[MAX_PASSLEN]; /* Channel key (if any). */
    
    unsigned int ban_count; /*Number of elements in banlist*/
    struct banlist *banlist_first;
    struct banlist *banlist_last;
    
    unsigned long user_count; /*Number of users*/
    struct userlist *userlist_first;
    struct userlist *userlist_last;
    
    struct userlist *me; /* Pointer to my channel user data (if any). */
};

/* Channel's userlist */
struct userlist {
    struct userlist *next;
    struct userlist *prev;
    
    struct chanNode *chan;
    struct userNode *user;
    
    unsigned int chan_mode; /* User's channel access (if any). */
};

unsigned int chan_count;

struct chanNode *chan_first;
struct chanNode *chan_last;

/* Prototypes */
struct chanNode* chan_get_handle(char *name);
unsigned int chan_prefix(char type);
static void init_chan_struct(struct chanNode *chan);
struct chanNode* chan_addchan(char *chan);
unsigned int chan_delchan(struct chanNode *chan);
struct userlist* chan_get_userlist(struct userNode *user, struct chanNode *chan);
struct userlist* chan_add_chanuser(struct userNode *user, struct chanNode *chan);
unsigned int chan_del_chanuser(struct userlist *hUserList);
void chan_free_userlist(struct chanNode *chan);
void chan_free_data(void);
