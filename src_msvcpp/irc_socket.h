/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

unsigned int irc_raw(char *raw);
unsigned int irc_queue(char *raw, unsigned int release);
unsigned int irc_send(char *raw, unsigned int override);
unsigned int flckmsg;

/* irc_connect return value:
    0 = Connected Successfully
    1 = Winsock Startup Failed
    2 = Winsock Old Version
    3 = gethostbyname() Failed
    4 = Error Defining Socket
    5 = Connecting Failed
*/
#define CERROR_SUCCESS 0
#define CERROR_STARTFAIL 1
#define CERROR_WSOLD 2
#define CERROR_HOSTTOIPFAIL 3
#define CERROR_SOCKFAIL 4
#define CERROR_CONFAIL 5
#define CERROR_EVENTLOOPFAIL 6
unsigned int irc_connect(unsigned char *servaddr, int servport);

void irc_disconnect(void);
THREADFUNC(irc_sockeventloop);

struct send_q {
    struct send_q *next;
    struct send_q *prev;
    
    char raw[511];
};

struct send_q *q_first;
struct send_q *q_last;
