/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

struct dccNode {
    struct dccNode *next;
    struct dccNode *prev;
    
    unsigned int sock;
    struct userNode *user;
    char ip[256];
};

struct dccNode *dcc_first;
struct dccNode *dcc_last;

struct dccNode* dcc_connect(struct userNode *user, char *address, unsigned short port);
