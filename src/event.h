/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#define EVFLAG_CALL_ONCE            0x01
#define EVFLAG_NEW_EVENT            0x02

#define EVENT_NULL                  0x000000
#define EVENT_IRCRAW                0x000001
#define EVENT_IRCSTART              0x000002
#define EVENT_IRCPING               0x000004
#define EVENT_IRCPONG               0x000008
#define EVENT_IRCPRIVMSG            0x000010
#define EVENT_IRCNOTICE             0x000020
#define EVENT_IRCINVITE             0x000040
#define EVENT_IRCJOIN               0x000080
#define EVENT_IRCPART               0x000100
#define EVENT_IRCKICK               0x000200
#define EVENT_IRCMODE               0x000400
#define EVENT_IRCQUIT               0x000800
#define EVENT_IRCTOPIC              0x001000
#define EVENT_IRCNICK               0x002000
#define EVENT_IRCISON               0x004000
#define EVENT_IRCRPL_USERHOST       0x008000
#define EVENT_IRCRPL_TOPIC          0x010000
#define EVENT_IRCRPL_CHANNELMODEIS  0x020000
#define EVENT_IRCRPL_NAMEREPLY      0x040000
#define EVENT_IRCRPL_BANLIST        0x100000

#define EVENT_INIT                  0x200000
#define EVENT_REHASH                0x400000
#define EVENT_EXIT                  0x800000

#define EVENT_FUNC(NAME) void NAME(struct eventNode *event, unsigned long event_id, datalist hook_data, datalist event_data)

typedef void(*call_func)();

struct eventNode {
    struct eventNode *next;
    struct eventNode *prev;
    
    call_func func;
    datalist data;
    unsigned long event_id;
    
    unsigned int call_once;
};

unsigned long event_count;
unsigned long event_types; /* Event types that have been registered. */
struct eventNode *event_first;
struct eventNode *event_last;

struct eventNode* event_get_handle(call_func func);
struct eventNode* event_hook(call_func func, unsigned int call_once, unsigned long event_id, unsigned int arg_count, ...);
unsigned int event_unhook(struct eventNode *event);
unsigned int event_call(unsigned long event_id, unsigned int arg_count, ...);
void event_free_data(void);
