/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "main.h"

struct eventNode* event_get_handle(call_func func) {
    struct eventNode *event;
    
    if(func == NULL)
        return NULL;
    
    for(event = event_first;event != NULL;event = event->next) {
        if(event->func == func) {
            return event;
        }
    }
    
    return NULL;
}

struct eventNode* event_hook(call_func func, unsigned int flags, unsigned long event_id, unsigned int arg_count, ...) {
    struct eventNode *event;
    va_list ap;
    unsigned int i, newnode;
    datalist data = NULL;
    
    if(flags & EVFLAG_NEW_EVENT)
        event = NULL;
    else
        event = event_get_handle(func);
    
    newnode = 0;
    if(event == NULL) {
        event = (struct eventNode*)mallocm(sizeof(struct eventNode));
        if(event == NULL) {
            make_error("Allocating memeory for event failed.");
            return NULL;
        }
        newnode = 1;
        event->event_id = 0;
    }
    
    if(arg_count) {
        datalist_start(data, arg_count);
    
        va_start(ap, arg_count);
        for(i = 0;i < arg_count;i++)
            dlist_v(data, i, void*) = va_arg(ap, void*);
        va_end(ap);
    }
    
    event->func = func;
    event->data = data;
    event->event_id |= event_id;
    event->call_once = flags & EVFLAG_CALL_ONCE ? 1 : 0;
    
    if(newnode) {
        LL_ADDNODE(event, event_first, event_last)
        event_count++;
    }
    
    if((event_types & event_id) == 0)
        event_types |= event_id;
    
    return event;
}

unsigned int event_unhook(struct eventNode *event) {
    struct eventNode *temp_event;
    
    if(event == NULL)
        return 0;
        
    LL_DELNODE(event, event_first, event_last)
    
    /* Remove the flag indicating that this event is
    registered if it's the last one. */
    for(temp_event = event_first;temp_event != NULL;temp_event = temp_event->next) {
        if(temp_event->event_id == event->event_id)
            break;
    }
    if(!temp_event)
        event_types &= ~event->event_id;
    /* /// */
        
    datalist_end(event->data);
    freem(event);
    
    event_count--;
    return 1;
}

unsigned int event_call(unsigned long event_id, unsigned int arg_count, ...) {
    struct eventNode *event, *next;
    va_list ap;
    unsigned int i;
    datalist data;
    
    if((event_types & event_id) < 1)
        return 1;
    
    if(arg_count) {
        datalist_start(data, arg_count);
    
        va_start(ap, arg_count);
        for(i = 0;i < arg_count;i++)
            dlist_v(data, i, void*) = va_arg(ap, void*);
        va_end(ap);
    }
    else {
        datalist_start(data, 1);
        data[0] = NULL;
    }
    
    for(event = event_first;event != NULL;event = next) {
        next = event->next;
        if(event->event_id & event_id) {
            event->func(event, event_id, event->data, data);
            if(event->call_once)
                event_unhook(event);
        }
    }
    
    datalist_end(data);
    
    return 1;
}

void event_free_data(void) {
    struct eventNode *temp_event, *temp_event2;
    
    temp_event = event_last;
    while(temp_event != NULL) {
        temp_event2 = temp_event->prev;
        if(temp_event->data != NULL)
            datalist_end(temp_event->data);
        freem(temp_event);
        if(temp_event2 == NULL) break;
        temp_event2->next = NULL;
        temp_event = temp_event2;
    }
    
    event_first = NULL;
    event_last = NULL;
    event_types = EVENT_NULL;
    
    event_count = 0;
    return ;
}
