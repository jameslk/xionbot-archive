#include <stdio.h>
#include <stdlib.h>

#include "sm_syntax.h"
#include "sm_action.h"

int main() {
    act_tb act;
    
    act.ch.abc = 'b';
    
    printf("%c", act.ch.abc);
    getch();
    
    return 0;
}
