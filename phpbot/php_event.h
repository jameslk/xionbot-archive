/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#define ON_RAW          0
#define ON_CONNECT      1
#define ON_PING         2
#define ON_PONG         3
#define ON_NICK         4
#define ON_ISON         5
#define ON_JOIN         6
#define ON_PART         7
#define ON_MESSAGE      8
#define ON_NOTICE       9
#define ON_ACTION       10
#define ON_CTCP         11
#define ON_CTCP_REPLY   12
#define ON_MODE         13
#define ON_KICK         14
#define ON_BAN          15
#define ON_UNBAN        16
#define ON_OP           17
#define ON_DEOP         18
#define ON_HALFOP       19
#define ON_DEHALFOP     20
#define ON_VOICE        21
#define ON_DEVOICE      22
#define ON_INVITE       23
#define ON_TOPIC        24
#define ON_DISCONNECT   25
#define ON_EXIT         26

/* Count of different types of events. */
#define EVENT_TYPE_COUNT 26

/* Function Prototypes */
unsigned int phpm_eventinit(void);
EVENT_FUNC(phpm_parse_event);
ZEND_FUNCTION(bind_event);
ZEND_FUNCTION(unbind_event);

/* Used in our function array for our functions
    to be registered with php */
#define EVENT_FUNCS ZEND_FE(bind_event, NULL) \
                    ZEND_FE(unbind_event, NULL)
