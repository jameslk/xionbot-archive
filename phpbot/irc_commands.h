/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#define MSG_URGENT         0x01
#define MSG_CTCP_REPLY     0x02
#define MSG_ALIAS          0x08 /* Replace message aliases. */

#define MSG_PRIVMSG    0
#define MSG_ACTION     1
#define MSG_NOTICE     2
#define MSG_CTCP       3

int irc_user(char *user, char *servname, char *fullname);
int irc_pass(char *pass);
int irc_ping(char *ping);
int irc_pong(char *pong);
int irc_nick(char *nick);
int irc_mode(char *target, char *format, ...);
int irc_join(char *chan, char *pass);
int irc_part(char *chan, char *reason);
int irc_quit(char *quitmsg);
int irc_msg(char *target, unsigned int msg_type, unsigned int flags, char *msg, va_list ap);
int irc_privmsg_user(char *target, unsigned int flags, char *msg, ...);
int irc_privmsg_chan(char *target, unsigned int flags, char *msg, ...);
int irc_action_user(char *target, unsigned int flags, char *msg, ...);
int irc_action_chan(char *target, unsigned int flags, char *msg, ...);
int irc_notice_user(char *target, unsigned int flags, char *msg, ...);
int irc_notice_chan(char *target, unsigned int flags, char *msg, ...);
int irc_ctcp_user(char *target, unsigned int flags, char *msg, ...);
int irc_ctcp_chan(char *target, unsigned int flags, char *msg, ...);
int irc_ison(char *nicks);
int irc_userhost(char *nick);

char lastison[497];
unsigned int nickchange;
