/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

/* Maximum length of an item (X:line) */
#define CONF_MAX_ITEMLEN 1024

/* Function prototypes */
char* config_line(char *buf, unsigned long line);
unsigned long get_confitemcount(unsigned char citem);
unsigned long get_confitem(char ***buf, unsigned char citem);
char* get_itemtok(char *buf, char* item, unsigned int tok);
char* irc_subconftok(char *buf, unsigned char citem, char *subname, unsigned int tok);
