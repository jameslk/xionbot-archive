/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

ZEND_FUNCTION(xirc_send);
ZEND_FUNCTION(xwrite_log);
ZEND_FUNCTION(xconfig_item_count);
ZEND_FUNCTION(xconfig_item);
ZEND_FUNCTION(xconfig_itemtok);
ZEND_FUNCTION(xsubconfig_tok);

#define XION_FUNCS ZEND_FE(xirc_send, NULL) \
                    ZEND_FE(xwrite_log, NULL) \
                    ZEND_FE(xconfig_item_count, NULL) \
                    ZEND_FE(xconfig_item, NULL) \
                    ZEND_FE(xconfig_itemtok, NULL) \
                    ZEND_FE(xsubconfig_tok, NULL)