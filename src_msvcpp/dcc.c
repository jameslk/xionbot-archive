/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>

#include "main.h"

#if defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
#elif defined(PLATFORM_POSIX)
    #include <unistd.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#include "irc_user.h"
#include "dcc.h"

struct dccNode* dcc_connect(struct userNode *user, char *address, unsigned short port) {
    struct sockaddr_in addr;
    struct dccNode *dcc;
    unsigned long longip;
    #ifdef PLATFORM_WINDOWS
    WSADATA wsa;
    #endif
    
    dcc = (struct dccNode*)malloc(sizeof(struct dccNode));
    if(dcc == NULL) {
        make_error("Failed to allocate space for dcc structure.");
        return NULL;
    }
    LL_ADDNODE(dcc, dcc_first, dcc_last);
    stats.dcc_count++;
    
    dcc->user = user;
    
    #ifdef PLATFORM_WINDOWS
    if(WSAStartup(0x0202, &wsa)) {
        WSACleanup();
        make_error("Failed to initiat use of WS2_32.DLL for dcc connection.");
        return NULL;
    }
    
    if(wsa.wVersion != 0x0202) {
        WSACleanup();
        make_error("Old version of winsock detected, update this pos.");
        return NULL;
    }
    #endif
    
    if((dcc->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        #ifdef PLATFORM_WINDOWS
        WSACleanup();
        #endif
        freem(dcc);
        make_error("Failed to create dcc socket.");
        return NULL;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if((longip = atol(address)))
        addr.sin_addr.s_addr = htonl(longip);
    else
        addr.sin_addr.s_addr = inet_addr(address);
    memset(&addr.sin_zero, 0, 8);
    
    xstrcpy(dcc->ip, inet_ntoa(addr.sin_addr), 256);
    
    if(connect(dcc->sock, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1) {
        #ifdef PLATFORM_WINDOWS
        WSACleanup();
        #endif
        freem(dcc);
        make_error("Failed to connect to specified dcc ip or port.");
        return NULL;
    }
    
    return dcc;
}
