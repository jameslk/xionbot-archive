/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#if defined(PLATFORM_POSIX)
#include <unistd.h>
#elif defined(PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

void _make_error(char *errmsg, char *file, const int line);
void _make_warning(char *warnmsg, char *file, const int line);
#define make_error(ERR) _make_error(ERR, __FILE__, __LINE__)
#define make_warning(WARN) _make_warning(WARN, __FILE__, __LINE__)
char* mkupper(char *buf, const char *str);
char* mklower(char *buf, const char *str);
unsigned int istrcmp(char *string1, char *string2);
void waits(unsigned int seconds);
void* callocm(size_t size, size_t typelen);
void* mallocm(size_t size);
void _freem(void** memblock);
#define freem(MEM) _freem((void*)&MEM)

/* The use of free_2d_array is discouraged. */
void _free_2d_array(void ***array, unsigned long elements);
#define free_2d_array(ARRAY, ELEMENTS) _free_2d_array((void*)&ARRAY, ELEMENTS)

void multi_free(void *fist, ...);
char* strrtok(char *str, const char *tok);
char* xstrtok(char *str, const char *tok, int *pos, int start);
char* xstrcpy(char *dest, const char *src, size_t num);
unsigned int blankstr(const char *str);
unsigned int clearstr(char *str, unsigned long max);
unsigned int matchstr(unsigned char *wildstr, unsigned char *regstr);
char* replace_alias(char *str);
char* conf_replace_alias(char *str);
unsigned int make_argument_array(char ***buf, const char *str);
unsigned int bind_argument_array(char **buf, char **argv, unsigned int argc, unsigned int offset);

#define THREADFUNC(a) DWORD WINAPI a(LPVOID lpParam)
unsigned int mkthread(LPTHREAD_START_ROUTINE func, LPVOID param);

/* A list (void array) of any type of data */
typedef void** datalist;
#define datalist_start(NAME, COUNT) NAME = (void**)mallocm(COUNT*sizeof(void**))
#define dlist_v(NAME, OFFSET, TYPE) (TYPE)NAME[OFFSET]
#define datalist_end(NAME) freem(NAME);

/* Linked List Handlers */
/* Format of linked list:
    struct linkedlist {
        struct linkedlist *next;
        struct linkedlist *prev;
        ...
    };
    struct linkedlist *first;
    struct linkedlist *last;
*/
/* Add a node to linked list. */
#define LL_ADDNODE(NODE, FIRST, LAST) \
    NODE->next = NULL; \
    if(FIRST == NULL) { \
        NODE->prev = NULL; \
        FIRST = NODE; \
    } \
    else { \
        NODE->prev = LAST; \
        LAST->next = NODE; \
    } \
    LAST = NODE;

/* Delete a node from linked list. */
#define LL_DELNODE(NODE, FIRST, LAST) \
    if(NODE->next == NULL) { \
        if(NODE->prev != NULL) { \
            /* ...prev <- NODE -> NULL */ \
            NODE->prev->next = NULL; \
            LAST = NODE->prev; \
        } \
        else { \
            /* NULL <- NODE -> NULL */ \
            FIRST = NULL; \
            LAST = NULL; \
        } \
    } \
    else { \
        if(NODE->prev == NULL) { \
            /* NULL <- NODE -> next... */ \
            FIRST = NODE->next; \
            FIRST->prev = NULL; \
        } \
        else { \
            /* ...prev <- NODE -> next... */ \
            NODE->next->prev = NODE->prev; \
            NODE->prev->next = NODE->next; \
        } \
    } \
/* *** */

/* For debugging */
#define print_dvar(d) printf("\n|  " #d " = %d  |\n\n", d);
#define print_cvar(c) printf("\n|  " #c " = %c  |\n\n", c);
#define print_svar(s) printf("\n|  " #s " = %s  |\n\n", s);
