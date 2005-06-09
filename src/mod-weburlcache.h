/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#define URLM_FILE "weburlcache.log"

unsigned int urlmod_init(void); /* Called in main.c: init() */
EVENT_FUNC(urlm_scan_text);
unsigned int urlm_get_url(char **bufp, const char *text, unsigned int *position);
unsigned int urlm_write_url(char *url);
