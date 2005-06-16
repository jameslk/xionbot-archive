/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#define PARSE_FUNC(NAME) static unsigned int irc_parse_##NAME(char *raw)

unsigned int irc_parseraw(char *raw);
unsigned int irc_parsenumeric(unsigned int numeric, char *raw);
int irc_start(unsigned int next);

PARSE_FUNC(ping);
PARSE_FUNC(pong);
PARSE_FUNC(privmsg);
PARSE_FUNC(notice);
PARSE_FUNC(invite);
PARSE_FUNC(join);
PARSE_FUNC(part);
PARSE_FUNC(kick);
PARSE_FUNC(mode);
PARSE_FUNC(quit);
PARSE_FUNC(topic);
PARSE_FUNC(nick);
PARSE_FUNC(ison);
PARSE_FUNC(302);
PARSE_FUNC(332);
PARSE_FUNC(324);
PARSE_FUNC(353);
PARSE_FUNC(367);

unsigned int mynickison;
unsigned int recieved_ping;
