/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
http://www.gnu.org/licenses/gpl.txt
*/

#include <php_embed.h>
#include <locale.h>

#include "main.h"
#include "conf.h"

#include "php_global.h"
#include "php_funcs.h"
#include "php_event.h"
#include "php_log.h"

zend_function_entry phpmod_functions[] = {
	XION_FUNCS
	EVENT_FUNCS
	{NULL, NULL, NULL}
};

zend_module_entry phpmod_module_entry = {
	STANDARD_MODULE_HEADER,
	"Xion PHP Module",
	phpmod_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	XION_PHPMOD_VERSION,
	STANDARD_MODULE_PROPERTIES,
};

/*static int ub_write(const char *str, unsigned int str_length TSRMLS_DC) {
    return phpm_log_write(str, NULL) ? str_length : 0;
}*/

static void log_message(char *message) {
    phpm_log_write(message, NULL);
    
    return ;
}

static void sapi_error(int type, const char *error_msg, ...) {
	va_list ap;
	
	va_start(ap, error_msg);
	phpm_log_write(error_msg, ap);
	va_end(ap);

	return ;
}

#define GLOBAL_SVAR(VAR, STR) \
    MAKE_STD_ZVAL(VAR); \
    VAR->type = IS_STRING; \
    VAR->value.str.len = strlen(STR); \
    VAR->value.str.val = STR; \
    zend_hash_update( \
        &EG(symbol_table), \
        #VAR, \
        strlen(#VAR)+1, \
        &VAR, \
        sizeof(zval*), \
        NULL \
    );
static void init_global_vars(void) {
    zval *me;
    
    GLOBAL_SVAR(me, bot.current_nick);
    
    return ;
}

unsigned int phpm_init(void) {
    char **llines;
    char *linetok;
    unsigned int i, lcount;
    
    /*php_embed_module.ub_write = ub_write;*/
    php_embed_module.log_message = log_message;
    php_embed_module.sapi_error = sapi_error;
    
	zend_startup_module(&phpmod_module_entry);
	
	init_global_vars();
	
	if(!phpm_eventinit())
	   return 0;
	
	lcount = get_confitem(&llines, 'E');
	if(!lcount)
	   return 0;
	
	linetok = (char*)callocm(CONF_MAX_ITEMLEN, sizeof(char));
    if(linetok == NULL) {
	    free_2d_array(llines, lcount);
        return 0;
    }
	
	for(i = 0;i < lcount;i++) {
        get_itemtok(linetok, llines[i], 2);
        if(!phpm_execfile(linetok)) {
            freem(linetok);
            free_2d_array(llines, lcount);
            return 0;
        }
    }
	
	freem(linetok);
	free_2d_array(llines, lcount);

	return 1;
}

unsigned int phpm_execfile(char *filename) {
	zend_file_handle zfile;
	
	if(blankstr(filename))
	   return 0;
	
	zfile.type = ZEND_HANDLE_FILENAME;
	zfile.filename = filename;
	zfile.free_filename = 0;
	zfile.opened_path = NULL;

	php_execute_script(&zfile TSRMLS_CC);

	return 1;
}
