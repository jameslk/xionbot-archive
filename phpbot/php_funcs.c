/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include <php_embed.h>

#include "main.h"
#include "conf.h"
#include "irc_socket.h"
#include "irc_commands.h"
#include "php_main.h"
#include "php_log.h"
#include "php_funcs.h"

/* int xirc_send(string raw [, bool urgent]) */
ZEND_FUNCTION(xirc_send) {
	char *raw;
	int *raw_len;
	zend_bool urgent = FALSE;

	if(ZEND_NUM_ARGS() < 1)
		WRONG_PARAM_COUNT;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &raw, &raw_len, &urgent) != SUCCESS)
		return ;

    RETURN_LONG(irc_send(raw, urgent));
}

/* bool xwrite_log(string string) */
ZEND_FUNCTION(xwrite_log) {
    char *str;
    int *str_len;
    
    if(ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) != SUCCESS)
		RETURN_FALSE;
	
    if(phpm_log_write(str, NULL)) {
        RETURN_TRUE;
    }
    else {
        RETURN_FALSE;
    }
}

/* long xconfig_item_count(char citem) */
ZEND_FUNCTION(xconfig_item_count) {
    char *citem;
    int len;
    
    if(ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &citem, &len) != SUCCESS)
		return ;
	
    RETURN_LONG(get_confitemcount(citem[0]));
}

/* array xconfig_item(char citem) */
ZEND_FUNCTION(xconfig_item) {
    char *str, citem, **list;
    unsigned int count, i;
    
    if(ZEND_NUM_ARGS() != 1)
		WRONG_PARAM_COUNT;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &count) != SUCCESS)
		return ;
	
	citem = str[0];
	count = get_confitem(&list, citem);
	
	if(!count)
	    RETURN_NULL();
	
	array_init(return_value);
	
	for(i = 0;i < count;i++)
        add_index_string(return_value, i, list[i], 1);
    
    free_2d_array(list, count);
    return ;
}

/* string xconfig_itemtok(string item, int tok) */
ZEND_FUNCTION(xconfig_itemtok) {
    char *item, *buf;
    int len;
    long tok;
    
    if(ZEND_NUM_ARGS() != 2)
		WRONG_PARAM_COUNT;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &item, &len, &tok) != SUCCESS)
		return ;
	
	if((!len) || (tok < 0))
	   return ;
	
	buf = (char*)calloc(len+1, sizeof(char));
	if(buf == NULL)
	   return ;
	
	get_itemtok(buf, item, (unsigned int)tok);
	RETVAL_STRING(buf, 1);
	
	freem(buf);
	return ;
}

/* string xsubconfig_tok(char citem, string subname, int tok) */
ZEND_FUNCTION(xsubconfig_tok) {
    char *str, *subname, *buf;
    char citem;
    int len1, len2;
    long tok;
    
    if(ZEND_NUM_ARGS() != 3)
		WRONG_PARAM_COUNT;
	
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &str, &len1, &subname, &len2, &tok) != SUCCESS)
		return ;
	
	citem = str[0];
	
	if((!len2) || (tok < 0))
	   return ;
	
	buf = (char*)calloc(CONF_MAX_ITEMLEN, sizeof(char));
	if(buf == NULL)
	   return ;
	
    irc_subconftok(buf, citem, subname, (unsigned int)tok);
    RETVAL_STRING(buf, 1);
	
	freem(buf);
    return ;
}
