/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "main.h"
#include "irc_socket.h"
#include "irc_parse.h"
#include "irc_commands.h"

unsigned int q_bytes = 0;
unsigned int q_maxbytes = 0;
unsigned int q_maxqueue = 0;
unsigned int q_floodcheck_msg = 0;

unsigned int irc_raw(char *raw) {
    if(raw == NULL)
        return 0;
    
    if(bot.floodcheck) {
        if(q_bytes > q_maxbytes) {
            /* Queue the data until we recieve our release message. */
            if(irc_queue(raw, 0)) {
                return 1;
            }
            else {
                make_error("irc_queue failed -- turning off flood checking.");
                bot.floodcheck = 0;
                return 0;
            }
        }
        else {
            /* Haven't hit the maximum bytes yet, just send normal */
            return irc_send(raw, 0);
        }
    }
    
    return irc_send(raw, 1);
}

unsigned int irc_queue(char *raw, unsigned int release) {
    struct send_q *queue = NULL, *queue2 = NULL;
    unsigned int q_count = 1;
    
    if((blankstr(raw)) && (!release))
        return 0;
    
    if(release) {
        /* We recieved our floodcheck ctcp message, let's
        release data from the queue until we hit the max
        bytes again. */
        if(q_first == NULL)
            return 1;
        
        q_bytes = 0;
        
        for(queue = q_first;queue != NULL && q_bytes <= q_maxbytes;queue = queue2) {
            irc_send(queue->raw, 0);
            queue2 = queue->next;
            freem(queue);
        }
        
        q_floodcheck_msg = 1;
        irc_ctcp_user(bot.current_nick, MSG_URGENT, "floodcheck");
        
        if(queue == NULL)
            q_first = NULL;
        else
            q_first = queue;
    }
    else {
        for(queue = q_first;queue != NULL;queue = queue->next)
            q_count++;
            
        if(q_count <= q_maxqueue) {
            /* Put raw data into our queue to be queried when we
            recieve the next release. */
            queue = (struct send_q*)mallocm(sizeof(struct send_q));
            if(queue == NULL)
                return 0;
            
            queue->next = NULL;
            queue->prev = NULL;
            xstrcpy(queue->raw, raw, 511);
            
            if(q_first == NULL) {
                queue->prev = NULL;
                q_first = queue;
            }
            else {
                queue->prev = q_last;
                q_last->next = queue;
            }
            q_last = queue;
            
            if(!q_floodcheck_msg) {
                q_floodcheck_msg = 1;
                irc_ctcp_user(bot.current_nick, MSG_URGENT, "floodcheck");
            }
        }
        else {
            /* Send last queue to bump it off so we don't go over
            our limit. */
            waits(3);
            
            queue = (struct send_q*)mallocm(sizeof(struct send_q));
            if(queue == NULL)
                return 0;
            
            queue->next = NULL;
            queue->prev = NULL;
            xstrcpy(queue->raw, raw, 511);
            
            q_last->next = queue;
            queue->prev = q_last;
            
            queue = q_first;
            q_first = queue->next;
            q_first->prev = NULL;
            
            irc_send(queue->raw, 1);
            freem(queue);
        }
    }
    
    return 1;
}

unsigned int irc_send(char *raw, unsigned int override) {
    char *data;
    int bytes = 0;
    unsigned int total = 0, dsize = 0;
    
    if(raw == NULL)
        return 0;
        
    data = (char*)callocm((strlen(raw)+3), sizeof(char));
    
    xstrcpy(data, raw, (strlen(raw)+3));
    strcat(data, "\r\n");
    dsize = strlen(data);
    
    for(;total < dsize;) {
        if((bytes = send(bot.sock, data+total, dsize, 0)) <= 0) {
            freem(data);
            return 0;
        }
        total += bytes;
        dsize -= bytes;
    }
    
    if((bot.floodcheck) && (!override))
        q_bytes += total;
    
    printf("SEND: %s\n", raw);
    
    freem(data);
    return 1;
}

unsigned int irc_recv(char *buf) {
    char *data;
    char byte;
    unsigned int i = 0, rn = 0;
    
    if((data = (char*)callocm(513, sizeof(char))) == NULL) return -1;
    
    for(i = 0;i < 513;i++) {
        if(recv(bot.sock, &byte, 1, 0) <= 0) {
            xstrcpy(buf, data, 513);
            freem(data);
            return 0;
        }
        
        if((byte == '\r') || (byte == '\n')) {
            rn++;
            if(rn == 2) break;
        }
        else {
            data[i] = byte;
            rn = 0;
        }
    }
    
    strcat(buf, data);
    freem(data);
    return 1;
}

unsigned int irc_connect(unsigned char *servaddr, int servport) {
    struct sockaddr_in addr;
    struct hostent *hosttoip;
    WSADATA wsa;
    
    if(WSAStartup(0x0202, &wsa)) {
        WSACleanup();
        return CERROR_STARTFAIL;
    }
    
    if(wsa.wVersion != 0x0202) {
        WSACleanup();
        return CERROR_WSOLD;
    }
    
    if(!(hosttoip = gethostbyname(servaddr))) {
        WSACleanup();
        return CERROR_HOSTTOIPFAIL;
    }
    
    if((bot.sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        WSACleanup();
        return CERROR_SOCKFAIL;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(bot.servport);
    addr.sin_addr = *((struct in_addr*) hosttoip->h_addr);
    
    if(connect(bot.sock, (struct sockaddr*) &addr, sizeof(struct sockaddr)) == -1) {
        WSACleanup();
        return CERROR_CONFAIL;
    }
    
    return CERROR_SUCCESS;
}

void irc_disconnect(void) {
    irc_quit("selfkill.techcore.org/xionbot");
    shutdown(bot.sock, SD_BOTH);
    closesocket(bot.sock);
    WSACleanup();
    return ;
}

THREADFUNC(irc_sockeventloop) {
    char *data = (char*)callocm(513, sizeof(char));
    int i = 0;
    WSAEVENT wsevent = WSACreateEvent();
    WSANETWORKEVENTS netevents;
    
    WSAEventSelect(bot.sock, wsevent, (FD_CONNECT | FD_READ | FD_CLOSE));
    
    while(1) {
        if(WSAWaitForMultipleEvents(1, &wsevent, 0, WSA_INFINITE, 0) == WSA_WAIT_EVENT_0) {
            WSAEnumNetworkEvents(bot.sock, wsevent, &netevents);
            if(netevents.lNetworkEvents & FD_CONNECT) {
                /* Connected. */
                irc_start(1);
            }
            else if(netevents.lNetworkEvents & FD_READ) {
                /* We've recieved some data, let's do something with it. */
                i = irc_recv(data);
                if(i == 1) {
                    printf("RECV: %s\n", data);
                    if(!irc_parseraw(data))
                        make_warning("irc_parseraw failed.");
                    memset(data, '\0', 513);
                    bot.current_try = 0;
                }
                else if(i == -1) {
                    make_error("Failed to allocate memory for recieved data.");
                }
            }
            else if(netevents.lNetworkEvents & FD_CLOSE) {
                /* We've been disconnected. */
                if(bot.current_try <= bot.maxretry) {
                    bot.current_try++;
                    i = bot.current_try;
                    irc_disconnect();
                    bot.current_try = i;
                    printf("Disconnected. Reconnecting in...\n");
                    for(i = 60;i > 0;i--) {
                        printf("%d\n", i);
                        waits(1);
                    }
                    memset(data, '\0', 513);
                    printf("Connecting to %s:%d... ", bot.servaddr, bot.servport);
                    if((i = irc_connect(bot.servaddr, bot.servport)) != 0) {
                        printf("Failed to connect. ERROR %d:%d\n\n", WSAGetLastError(), i);
                        log_write("Connecting failed with code: %d:%d", WSAGetLastError(), i);
                        printf("Press Enter key to continue.");
                        getchar();
                        freem(data);
                        exit(1);
                    }
                    printf("Connected.\n");
                    WSAEventSelect(bot.sock, wsevent, (FD_CONNECT | FD_READ | FD_CLOSE));
                }
                else {
                    printf("Disconnected.");
                    break;
                }
            }
        }
    }
    
    freem(data);
    return 0;
}
