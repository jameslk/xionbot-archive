/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

char* irc_get_host(char *buf, char *raw);
char* irc_get_mask(char *buf, char *raw);
char* irc_get_nick(char *buf, char *raw);
char* irc_get_target(char *buf, char *raw);
char* irc_get_msg(char *buf, char *raw);
char* irc_get_cmdtype(char *buf, char *raw);
char* irc_get_cmdtype2(char *buf, char *raw);
char* irc_get_netname(char *buf, char *raw);
char* irc_get_servname(char *buf, char *raw);
THREADFUNC(irc_fightfornick);

#define IS_TYPE_ALPHA 0
#define IS_TYPE_NUMERIC 1
#define IS_TYPE_ACCESS 2
#define IS_TYPE_SPECIAL 3
unsigned int irc_char_type(char c, unsigned int type);

unsigned int irc_strip_ctrlcodes(char **bufp, const char *text);
