/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

/* Command Types to Allow */
#define BC_PRIVMSG   0x01
#define BC_CHANMSG   0x02
#define BC_PRIVCTCP  0x04
#define BC_CHANCTCP  0x08

/* Command Flags */
#define BC_NOT_UNBINDABLE   0x000001
#define BC_NOT_BINDABLE     0x000002

/*
    alias: Alias used to execute this command.
    cmd_type: Type of command this was (T_CHANMSG, T_CTCP, etc.)
    user: User this command was run from.
    chan: Channel this command was run on (if any) or the channel it was for.
    target: Where this command was run from.
    argc: Argument count.
    argv: Argument list (the parameters after the alias).
*/
#define BOT_CMD(NAME) unsigned int bot_cmd_##NAME(char *alias, unsigned int cmd_type, struct userNode *user, struct chanNode *chan, char *target, unsigned int argc, char **argv)

typedef unsigned int(*botcmd_func)();

struct botcmd_list {
    struct botcmd_list *next;
    struct botcmd_list *prev;
    
    char alias[MAX_MSGLEN]; /* Alias of this command */
    botcmd_func func; /* Command Function */
    unsigned int minarg; /* Minimum Arguments */
    unsigned int cmd_type; /* Types to allow */
    unsigned long flags; /* Command Flags */
};

unsigned long bc_count;
struct botcmd_list *bc_first;
struct botcmd_list *bc_last;

/* Function Prototypes */
static struct botcmd_list* get_botcmd_handle(char *alias);
static struct botcmd_list* get_bch_by_func(botcmd_func func);
unsigned int register_bot_cmd(char *alias, botcmd_func func, unsigned int minarg, unsigned int cmd_type, unsigned long flags);
unsigned int unregister_bot_cmd(struct botcmd_list *bc);
unsigned int bot_cmd_init(void);
unsigned int bot_cmd(char *alias, unsigned int cmd_type, char *nick, char *target, char *param);
void botcmd_free_data(void);

EVENT_FUNC(cmd_ban_user);

