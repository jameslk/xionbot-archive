/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

/* Max Lengths*/
/* This includes terminating nulls */
#define MAX_LEN         513 /* Line length */
#define MAX_MSGLEN      401
#define MAX_CHANLEN     401
#define MAX_PASSLEN     89
#define MAX_NICKLEN     33
#define MAX_USERLEN     13
#define MAX_INFOLEN     257
#define MAX_TARGETLEN   101
#define MAX_MODELEN     501
#define MAX_MASKLEN     65
#define MAX_HOSTMASKLEN 89
#define MAX_TOPICLEN    401

/* IRC Characters */
#define C_EVENT     0x01 /* Event */
#define C_BOLD      0x02 /* Bold */
#define C_ULINE     0x1F /* Underline */
#define C_COLOR     0x03 /* Color */
#define C_REVERSE   0x16 /* Reverse */
#define ALPHA_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define NUMERIC_CHARS "0123456789"
#define ACCESS_CHARS "@%+&!~*"
#define SPECIAL_CHARS "[]\\`_^{|}"

/* Modes */
#define C_OP        '@'
#define C_HALFOP    '%'
#define C_VOICE     '+'

/* Channel Mode List */
#define MODE_OP_TOPIC   0x0001 /* Ops can only set the topic. (t) */
#define MODE_NO_EXT_MSG 0x0002 /* No external messages. (n) */
#define MODE_INVITE     0x0004 /* Invite only. (i) */
#define MODE_MODERATED  0x0008 /* Channel is moderated. (m) */
#define MODE_KEY        0x0010 /* Channel has a key. (k) */
#define MODE_LIMIT      0x0020 /* Channel limits users. (l) */
#define MODE_PRIVATE    0x0040 /* Channel is set to private. (p) */
#define MODE_SECRET     0x0080 /* Channel is set to secret. (s) */
#define MODE_NOCOLORS   0x0100 /* Disallowed colors (c)*/
#define MODE_NOCTCPS    0x0200 /* No channel CTCPs allowed (C)*/
#define MODE_DELAYED_J  0x0400 /* Delayed join. (D)*/
#define MODE_DELAYED_U  0x0800 /* Delayed users. (d) */

/* Channel User Mode List */
#define MODE_OP     0x01
#define MODE_HALFOP 0x02
#define MODE_VOICE  0x04
#define MODE_BAN    0x08

/* User Mode List */
#define MODE_OPER       0x0001 /* User is an operator. (O) */
#define MODE_LOCOP      0x0002 /* User is a local operator. (o) */
#define MODE_SERVICE    0x0004 /* User has 1337 service powers. (k) */
#define MODE_INVISIBLE  0x0008 /* User is invisible. (i) */
#define MODE_WALLOP     0x0010 /* User can recieve wallops. (w) */
#define MODE_SNOTICE    0x0020 /* User can recieve server notices. (s) */
#define MODE_FAKEHOST   0x0040 /* User has a spoofed host. (x) */
#define MODE_AWAY       0x0080 /* User set as away. */
#define MODE_DEAF       0x0100 /* User is deaf. (d) */
#define MODE_DWALLOPS   0x0200 /* User can recieve desynch wallops */

/* All modes that can be used in a channel. */
#define CHAN_MODES "ohvbtnimklpscCDd"
/* All modes that can be applied to users in a channel. */
#define CHAN_USER_MODES "ohvb"
/* All modes that have paramaters. */
#define CHAN_PARAM_MODES "ohvbkl"
/* All user modes. */
#define USER_MODES "iowsgxdk"
/* Modes that halfops cannot set/unset */
#define NON_HALFOP_ACCESS (MODE_OP|MODE_HALFOP)
