/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

struct chanNode;

struct modeparam {
    unsigned int set;
    char mode;
    char param[MAX_LEN];
};

struct banlist {
    struct banlist *next;
    struct banlist *prev;
    
    char ban[MAX_MASKLEN];
    char setby[MAX_MSGLEN];
    unsigned long date;
};

unsigned int mode_ischusrmode(char mode);
unsigned int mode_ischparamode(char mode);
struct modeparam* mode_getparam(struct modeparam *mp, char *modestr, unsigned int tok);
unsigned int mode_modecmp(struct modeparam match, char *modestr);
struct banlist* mode_bantoptr(struct chanNode *chan, char *ban);
struct banlist* mode_addban(struct chanNode *chan, char *ban);
unsigned int mode_delban(struct chanNode *chan, char *ban);
unsigned int mode_chanuser_mode(struct userNode *user, struct chanNode *chan, unsigned int mode);
unsigned int mode_remove_user_bans(struct userNode *user, struct chanNode *chan, unsigned int override);
unsigned int mode_chan(struct chanNode *chan, char *modestr);
unsigned int mode_chanparam(struct chanNode *chan, char *setter, char *modestr);
void mode_free_banlist(struct chanNode *chan);

#define IS_OP(USER, CHAN) mode_chanuser_mode(USER, CHAN, MODE_OP)
#define IS_HALFOP(USER, CHAN) mode_chanuser_mode(USER, CHAN, MODE_HALFOP)
#define IS_VOICE(USER, CHAN) mode_chanuser_mode(USER, CHAN, MODE_VOICE)
