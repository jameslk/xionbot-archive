/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "main.h"

unsigned long alloc_calls;
unsigned long free_calls;

void _make_error(char *errmsg, char *file, const int line) {
    fprintf(stderr, "\n**********\n%s (%s:%d) %s\n**********\n", XION_ERROR_MSG, file, line, errmsg);
    log_write("%s (%s:%d) %s", XION_ERROR_MSG, file, line, errmsg);
    return ;
}

void _make_warning(char *warnmsg, char *file, const int line) {
    fprintf(stderr, "\n**********\n%s (%s:%d) %s\n**********\n", XION_WARN_MSG, file, line, warnmsg);
    log_write("%s (%s:%d) %s", XION_WARN_MSG, file, line, warnmsg);
    return ;
}

char* mkupper(char *buf, const char *str) {
    char *tupr;
    unsigned int i = 0;
    
    if((buf == NULL) || (str == NULL))
        return buf;
    
    if((tupr = (char*)callocm(strlen(str)+1, sizeof(char))) == NULL)
        return NULL;
    
    strncpy(tupr, str, strlen(str)+1);
    
    for(i = 0;i < (strlen(tupr)+1);i++)
        tupr[i] = toupper(str[i]);
    
    strcpy(buf, tupr);
    
    freem(tupr);
    return buf;
}

char* mklower(char *buf, const char *str) {
    char *tlwr;
    unsigned int i = 0;
    
    if((buf == NULL) || (str == NULL))
        return buf;
    
    if((tlwr = (char*)callocm(strlen(str)+1, sizeof(char))) == NULL)
        return NULL;
    
    strncpy(tlwr, str, strlen(str)+1);
    
    for(i = 0;i < (strlen(tlwr)+1);i++)
        tlwr[i] = tolower(str[i]);
    
    strcpy(buf, tlwr);
    
    freem(tlwr);
    return buf;
}

unsigned int istrcmp(char *string1, char *string2) {
    unsigned int i, len1, len2;
    
    if((blankstr(string1)) || (blankstr(string2)))
        return 0;
    
    len1 = strlen(string1);
    len2 = strlen(string2);
    if((len1 < 1) || (len2 < 1))
        return 0;
    
    if(len1 != len2)
        return 0;
        
    for(i = 0;;i++) {
        if(!string1[i]) {
            if(!string2[i])
                break;
            else
                return 0;
        }
        if(!string2[i])
            return 0;
        
        if(tolower(string1[i]) != tolower(string2[i]))
            return 0;
    }
    
    return 1;
}

/* Pause for (seconds) seconds. */
void waits(unsigned int seconds) {
    #if defined(PLATFORM_POSIX)
    sleep(seconds);
    #elif defined(PLATFORM_WINDOWS)
    Sleep(seconds*1000);
    #else
    clock_t finisht = clock() + seconds * CLK_TCK;
    while(clock() < finisht);
    #endif
}

void* mallocm(size_t size) {
    void *ptr;
    
    ptr = malloc(size);
    if(ptr == NULL) {
        make_error("Failed to allocate space with malloc.");
        return NULL;
    }
    
    alloc_calls++;
    
    return ptr;
}

void* callocm(size_t size, size_t typelen) {
    void *ptr;
    
    ptr = calloc(size, typelen);
    
    if(ptr == NULL) {
        make_error("Failed to allocate space with calloc.");
        return NULL;
    }
    
    alloc_calls++;
    
    return ptr;
}

void _freem(void **memblock) {
    if(*memblock == NULL) {
        make_error("Attempt to free a NULL pointer -- halted.");
        return ;
    }
    
    free(*memblock);
    *memblock = NULL;
    
    if(++free_calls > alloc_calls)
        make_warning("The number of free calls is greater than allocated calls.");
    
    return ;
}

void _free_2d_array(void ***array, unsigned long elements) {
    unsigned long i;
    
    for(i = 0;i < elements;i++)
        freem(array[0][i]);
    
    freem(array[0]);
    return ;
}

void multi_free(void *first, ...) {
    va_list ap;
    void *ptr;
    
    va_start(ap, first);
    while((ptr = va_arg(ap, void*)) != NULL)
        freem(ptr);
    va_end(ap);
    
    freem(first);
    
    return ;
}

/* Remove tokens from a string. */
char* strrtok(char *str, const char *tok) {
    unsigned int i, ii, x, tlen, slen;
    
    tlen = strlen(tok);
    slen = strlen(str);
    
    for(i = 0;i < tlen;i++) {
        for(ii = 0;ii < slen;ii++) {
            if(str[ii] == tok[i]) {
                for(x = 0;(ii+x) < slen;x++)
                    str[ii+x] = str[ii+x+1];
                slen = strlen(str);
            }
        }
    }
    
    return str;
}

char* xstrtok(char *str, const char *tok, int *pos, int start) {
    int i, x;
    unsigned int match;
    
    if(blankstr(tok))
        return str;
    
    if(start) {
        i = 0;
    }
    else {
        if(pos != NULL)
            i = *pos;
    }
    
    for(x = 0, match = 0;str[i];i++, x++) {
        if(str[i] == tok[match]) {
            if(!tok[++match]) {
                while(match--) str[i - match] = 0;
                i++;
                x++;
                
                if(pos != NULL)
                    *pos = i;
                
                if(start)
                    return str;
                else
                    return str + (i - x);
            }
        }
    }
    if(pos != NULL)
        *pos = i;
    
    if(start) {
        return str;
    }
    else {
        if(x) {
            return str + (i - x);
        }
        else
            return NULL;
    }
}

char* xstrcpy(char *dest, const char *src, size_t num) {
    unsigned int i = 0;
    
    if(dest == NULL)
        return dest;
    
    if(src == NULL)
        clearstr(dest, num);
    
    num--;
    for(i = 0;i < num;i++) {
        if(!src[i])
            break;
        dest[i] = src[i];
    }
    dest[i] = 0;
    
    return dest;
}

/*Is this string blank?*/
unsigned int blankstr(const char *str) {
    if(str == NULL)
        return 1;
    
    if(!*str)
        return 1;
    
    return 0;
}

unsigned int clearstr(char *str, unsigned long max) {
    memset(str, 0, max);
    return 1;
}

/* mmatch() Written by Run (carlo@runaway.xs4all.nl), 25-10-96 */
unsigned int matchstr(unsigned char *wildstr, unsigned char *regstr) {
    unsigned char *w = wildstr, *r = regstr,
                    *wa = wildstr, *ra = regstr;
    unsigned int wild = 0;
                    
    if((wildstr == NULL) || (regstr == NULL))
        return 0;
                    
    while(1) {
        if(*w == '*') {
            for(;*w == '*';) w++;
            
            wild = 1;
            wa = w;
            ra = r;
        }
        
        if(!*w) {
            if(!*r)
                break;
                
            for(w++;(w > wildstr) && (*w == '?');w--);
            
            if((*w == '*') && (w > wildstr))
                return 1;
            
            if(!wild)
                return 0;
            
            w = wa;
            r = ++ra;
        }
        else if(!*r) {
            for(;*w == '*';w++);
            
            return (*w == 0);
        }
        
        if((tolower(*w) != tolower(*r)) && (*w != '?')) {
            if(!wild)
                return 0;
                
            w = wa;
            r = ++ra;
        }
        else {
            if(*w) w++;
            if(*r) r++;
        }
    }
    
    return 1;
}

char* replace_alias(char *str) {
    unsigned int i, x, len;
    char next, replace;
    
    if(blankstr(str))
        return str;
    
    len = strlen(str);
    for(i = 0;i < len;i++) {
        if(str[i] == '$') {
            switch(str[i+1]) {
                case '$':
                    replace = '$';
                    break;
                    
                case 'b':
                    replace = C_BOLD;
                    break;
                
                case 'u':
                    replace = C_ULINE;
                    break;
                
                case 'c':
                    replace = C_COLOR;
                    break;
                
                case 'r':
                    replace = C_REVERSE;
                    break;
                
                default:
                    continue;
            }
            str[i] = replace;
            for(x = i+1;x < len;x++) {
                next = str[x+1];
                str[x] = next;
            }
            str[len] = 0;
            len--;
        }
    }
    
    return str;
}

char* conf_replace_alias(char *str) {
    unsigned int i, x, len;
    char next, replace;
    
    if(blankstr(str))
        return str;
    
    len = strlen(str);
    for(i = 0;i < len;i++) {
        if(str[i] == '$') {
            switch(str[i+1]) {
                case 'c':
                    replace = ':';
                    break;
                
                case 's':
                    replace = ';';
                    break;
                
                default:
                    continue;
            }
            str[i] = replace;
            for(x = i+1;x < len;x++) {
                next = str[x+1];
                str[x] = next;
            }
            str[len] = 0;
            len--;
        }
    }
    
    return str;
}

unsigned int make_argument_array(char ***buf, const char *str) {
    unsigned int x = 0, y = 0, z = 0, count = 0, length;
    char *temp, *backptr;
    char **argv;
    
    if(blankstr(str))
        return 0;
    
    length = strlen(str);
    
    while(str[x++] == ' ') z++;
    if(z >= length)
        return 0;
    
    for(;x < length;x++) {
        if((str[x] == ' ') && (str[x+1] != ' ') && (str[x+1] != 0))
            count++;
    }
    count++;
    
    argv = (char**)mallocm(count*sizeof(char*));
    if(argv == NULL) {
        make_warning("Couldn't allocate space needed for make_argument_array()");
        return 0;
    }
    
    temp = (char*)callocm(length+1, sizeof(char));
    if(temp == NULL) {
        freem(argv);
        return 0;
    }
    backptr = temp;
        
    xstrcpy(temp, str, length+1);
    
    temp += z;
    for(x = z, count = 0;str[x];count++) {
        if(x > z)
            x++;
        temp += y;
        for(y = 0;str[x];x++, y++) {
            if(str[x] == ' ') {
                if((str[x+1] == ' ') || (str[x+1] == 0))
                    temp[y] = 0;
                else
                    break;
            }
        }
        temp[y] = 0;
        argv[count] = (char*)callocm(strlen(temp)+1, sizeof(char));
        if(argv[count] == NULL) {
            make_warning("Couldn't allocate space needed for make_argument_array()");
            freem(argv);
            freem(temp);
            return 0;
        }
        xstrcpy(argv[count], temp, y+1);
        y++;
    }
    
    *buf = argv;
    
    freem(backptr);
    return count;
}

unsigned int bind_argument_array(char **buf, char **argv, unsigned int argc, unsigned int offset) {
    unsigned int i, size = 0;
    char *temp;
    
    if((argv == NULL) || (argc < 1))
        return 0;
    
    if(argc == 1) {
        temp = (char*)callocm(strlen(argv[0])+1, sizeof(char));
        if(temp == NULL)
            return 0;
        strcpy(temp, argv[0]);
        *buf = temp;
        return 1;
    }
    
    for(i = offset;i < argc;i++)
        size += strlen(argv[i])+1;
    
    temp = (char*)callocm(size, sizeof(char));
    if(temp == NULL)
        return 0;
    
    for(i = offset;i < argc;i++) {
        strcat(temp, argv[i]);
        if(i+1 != argc)
            strcat(temp, " ");
    }
    
    *buf = temp;
    
    return 1;
}

unsigned int mkthread(LPTHREAD_START_ROUTINE func, LPVOID param) {
    DWORD thdId;
    HANDLE thdHandle;
    
    thdHandle = CreateThread(NULL, 0, func, param, 0, &thdId);
    
    if(thdHandle == NULL) {
        make_error("Failed to create new thread.");
        return 0;
    }
    else {
        CloseHandle(thdHandle);
    }
    
    return 1;
}
