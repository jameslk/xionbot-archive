/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "conf.h"
#include "irc_admin.h"
#include "irc_tools.h"

/*
After coding the majority of this, I realized I could have
just used a structure and saved a lot of time and effort.
Oh well, maybe I will do that in a later version.
*/

int adm_admtoint(char *adm) {
    char *admin;
    unsigned int i = 0;
    
    if(adm == NULL)
        return -1;
        
    if(bot.admin_lines < 1)
        return -1;
        
    admin = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    if(admin == NULL)
        return -1;
    
    for(i = 0;i < bot.admin_lines;i++) {
        get_itemtok(admin, bot.admin_array[i], 4);
        if(istrcmp(admin, adm)) {
            freem(admin);
            return i;
        }
    }
    
    freem(admin);
    return -1;
}

unsigned int adm_isadminmask(unsigned char *mask, char *adm) {
    unsigned char *w, *wildmask, *regmask;
    int admin = adm_admtoint(adm);
    
    if((blankstr(mask)) || (admin == -1))
        return 0;
        
    w = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    get_itemtok(w, bot.admin_array[admin], 2);
    
    wildmask = (char*)callocm(513, sizeof(char));
    regmask = (char*)callocm(513, sizeof(char));
    
    xstrcpy(wildmask, w, 513);
    xstrcpy(regmask, mask, 513);
    
    if(istrcmp(wildmask, "*")) {
        freem(w);
        freem(wildmask);
        freem(regmask);
        return 1;
    }
    
    if(matchstr(strtok(wildmask, "@"), strtok(regmask, "@"))) {
        xstrcpy(wildmask, w, 513);
        xstrcpy(regmask, mask, 513);
        
        strtok(wildmask, "@");
        strtok(NULL, "@");
        
        strtok(regmask, "@");
        strtok(NULL, "@");
        
        if(matchstr(wildmask, regmask)) {
            return 1;
        }
    }
    
    freem(w);
    freem(wildmask);
    freem(regmask);
    return 0;
}

char* adm_userisadmin(char *user) {
    char *admin;
    unsigned int i = 0;
    
    if(user == NULL)
        return NULL;
        
    admin = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    
    for(i = 0;i < bot.admin_lines;i++) {
        get_itemtok(admin, bot.admin_array[i], 4);
        if(istrcmp(user, admin)) {
            freem(admin);
            return user;
        }
    }
    
    freem(admin);
    return NULL;
}

char* adm_isadminpass(char *adm, char *pass) {
    char *confpass;
    int admin = adm_admtoint(adm);
    
    if((admin == -1) || (pass == NULL))
        return NULL;
    
    confpass = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    
    get_itemtok(confpass, bot.admin_array[admin], 3);
    
    if(istrcmp(confpass, pass)) {
        freem(confpass);
        return pass;
    }
    
    freem(confpass);
    return NULL;
}

unsigned int adm_login(unsigned char *mask, char *user, char *pass) {
    char *access;
    
    if((mask == NULL) || (user == NULL) || (pass == NULL))
        return 0;
    
    if((char*)adm_userisadmin(user) == NULL)
        return 0;
    
    if((char*)adm_isadminpass(user, pass) == NULL)
        return 0;
    
    if(!adm_isadminmask(mask, user))
        return 0;
        
    if(adm_maskislogdin(mask))
        return 2;
        
    access = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    
    get_itemtok(access, bot.admin_array[adm_admtoint(user)], 5);
    
    if(!adm_loginuser(mask, user, atoi(access))) {
        freem(access);
        return 0;
    }
    
    freem(access);
    return 1;
}

unsigned int adm_loginuser(unsigned char *mask, char *user, int access) {
    char **tmp;
    if((mask == NULL) || (user == NULL) || (access == 0))
        return 0;
        
    if((strlen(mask) > 512) || (strlen(user) > 96))
        return 0;
        
    if(bot.admin_array_current == NULL)
        bot.admin_current_count = 0;
    
    if((tmp = (char**)realloc(bot.admin_array_current, ++bot.admin_current_count*sizeof(char*))) == NULL)
        return 0;
    bot.admin_array_current = tmp;
    
    if(bot.admin_array_current == NULL)
        return 0;
    
    bot.admin_array_current[(bot.admin_current_count-1)] = (char*)callocm(617, sizeof(char));
    
    if(bot.admin_array_current[(bot.admin_current_count-1)] == NULL)
        return 0;
    
    if(sprintf(bot.admin_array_current[(bot.admin_current_count-1)], "%s:%s:%d", mask, user, access) < 0)
        return 0;
    
    return 1;
}

unsigned int adm_logoutuser(char *mask) {
    int m = adm_cmasktoint(mask);
    unsigned int i = 0;
    char *temp;
    
    if(m == -1)
        return 0;
        
    if((temp = (char*)callocm(617, sizeof(char))) == NULL)
        return 0;
    
    bot.admin_current_count--;
    for(i = m;i < bot.admin_current_count;i++) {
        xstrcpy(temp, bot.admin_array_current[(i+1)], 617);
        xstrcpy(bot.admin_array_current[i], temp, 617);
    }
    
    bot.admin_array_current = (char**)realloc(bot.admin_array_current, bot.admin_current_count*sizeof(char*));
    
    freem(temp);
    return 1;
}

int adm_cmasktoint(unsigned char *mask) {
    char *m;
    unsigned int i = 0;
    
    if(mask == NULL)
        return -1;
        
    if(bot.admin_current_count < 1)
        return -1;
        
    if((m = (char*)callocm(617, sizeof(char))) == NULL)
        return -1;
    
    for(i = 0;i < bot.admin_current_count;i++) {
        xstrcpy(m, bot.admin_array_current[i], 617);
        strtok(m, ":");
        if(istrcmp(m, mask)) return i;
        if(!matchstr((strchr(m, '@')+1), mask)) continue;
        strtok(m, "@");
        if(matchstr(m, mask)) return i;
    }
    
    /*Old way of doing things (no wildcards)
    for(i = 0;i < bot.admin_current_count;i++) {
        xstrcpy(m, bot.admarrcur[i], 617);
        strtok(m, ":");
        if(istrcmp(m, mask)) {
            return i;
        }
    }*/
    
    return -1;
}

unsigned int adm_maskislogdin(unsigned char *mask) {
    if(mask == NULL)
        return 0;
        
    if(bot.admin_current_count < 1)
        return 0;
        
    if(adm_cmasktoint(mask) != -1)
        return 1;
    
    return 0;
}

unsigned int adm_userislogdin(char *user) {
    char *u;
    unsigned int i = 0;
    
    if(user == NULL)
        return 0;
        
    if(bot.admin_current_count < 1)
        return 0;
        
    if((u = (char*)callocm(617, sizeof(char))) == NULL)
        return 0;
    
    for(i = 0;i < bot.admin_current_count;i++) {
        xstrcpy(u, bot.admin_array_current[i], 617);
        strtok(u, ":");
        strtok(NULL, ":");
        if(istrcmp(u, user)) {
            return 1;
        }
    }
    
    return 0;
}

int adm_getcuraccess(unsigned char *mask) {
    char *access;
    int admin = adm_cmasktoint(mask);
    
    if(admin == -1)
        return 0;
    
    access = strchr(bot.admin_array_current[admin], ':');
    access = strchr(access+1, ':');
    access++;
    
    return atoi(access);
}
