/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
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
int irc_pong(char *pong);
int irc_nick(char *nick);
int irc_mode(char *target, char *format, ...);
int irc_join(char *chan, char *pass);
int irc_part(char *chan, char *reason);
int irc_quit(char *quitmsg);
int irc_msg(char *target, unsigned int msg_type, unsigned int flags, char *msg, ...);
int irc_ison(char *nicks);
int irc_userhost(char *nick);

#define irc_privmsg_user(TARGET, FLAGS, MSG...) irc_msg(TARGET, MSG_PRIVMSG, FLAGS, ##MSG)
#define irc_privmsg_chan(TARGET, FLAGS, MSG...) irc_msg(TARGET, MSG_PRIVMSG, FLAGS, ##MSG)
#define irc_action_user(TARGET, FLAGS, MSG...) irc_msg(TARGET, MSG_ACTION, FLAGS, ##MSG)
#define irc_action_chan(TARGET, FLAGS, MSG...) irc_msg(TARGET, MSG_ACTION, FLAGS, ##MSG)
#define irc_notice_user(TARGET, FLAGS, MSG...) irc_msg(TARGET, MSG_NOTICE, FLAGS, ##MSG)
#define irc_notice_chan(TARGET, FLAGS, MSG...) irc_msg(TARGET, MSG_NOTICE, FLAGS, ##MSG)
#define irc_ctcp_user(TARGET, FLAGS, MSG...) irc_msg(TARGET, MSG_CTCP, FLAGS, ##MSG)
#define irc_ctcp_chan(TARGET, FLAGS, MSG...) irc_msg(TARGET, MSG_CTCP, FLAGS, ##MSG)

char lastison[497];
unsigned int nickchange;
