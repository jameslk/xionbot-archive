/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

struct chanNode;
struct userNode;

struct relayNode {
    struct relayNode *next;
    struct relayNode *prev;
    
    struct userNode *to_user;
    struct chanNode *to_chan;
    struct chanNode *from_chan;
};

unsigned int relay_count;
struct relayNode *relay_first;
struct relayNode *relay_last;

/* Function Prototypes */
struct relayNode* relayed_chan(struct chanNode *to_chan, struct chanNode *from_chan);
struct relayNode* relayed_user(struct userNode *to_user, struct chanNode *from_chan);
static unsigned int is_relay(struct chanNode *chan);
static void relay_all(struct chanNode *chan, char *user, char *message);
void relay_message(char *raw, char *chan_name, char *user); /* Used in irc_parse.c: irc_parse_privmsg */
static unsigned int register_relay(struct relayNode *relay);
static unsigned int unregister_relay(struct relayNode *relay);
void relay_free_data(void); /* Used in main.c: free_xion_memory and in parse.c: irc_start */

BOT_CMD(relay); /* Used in irc_botcmd.c: bot_cmd_init */
BOT_CMD(relays); /* Used in irc_botcmd.c: bot_cmd_init */
BOT_CMD(unrelay); /* Used in irc_botcmd.c: bot_cmd_init */
BOT_CMD(unrelayall); /* Used in irc_botcmd.c: bot_cmd_init */
