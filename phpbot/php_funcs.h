/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
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