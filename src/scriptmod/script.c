/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>

#include "../main.h"
#include "script.h"

int main() {
    char test[] = "alias cypherhack.chr {\nvar %i = 1;\nwhile (%i <= $len($2-)) {\nvar %msg = %msg $+ $replace($chr($calc($asc($mid($2-,%i,1)) - $1)),$chr(32),$str($chr(32),2));\ninc %i;\n}\nreturn %msg;\n}";
    
    sm_setstr(test);
    printf("|%s|", test);
    getchar();
    
    return 0;
}

unsigned int sm_switch(XBOOLEAN enable) {
    #ifdef SCRIPTMOD_OVERRIDE
    if(enable) {
        /*enable stuff*/
        #define SCRIPTMOD
        printf("Scripting Module has been enabled.");
    }
    else {
        /*disable stuff*/
        #undef SCRIPTMOD
        printf("Scripting Module has been disabled.");
    }
    #else
    printf("Scripting Module has been disabled.");
    #endif
    
    return 1;
}

unsigned int sm_printerr(char *alias) {
    printf("Unknown alias: %s", alias);
    
    return 1;
}
