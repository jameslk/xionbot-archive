/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

#include "php_main.h"

void ***tsrm_ls;

#define PHP_RUNTIME_FNAME EG(function_state_ptr)->function->common.function_name

#define PHPE_BEGIN php_embed_init(0, NULL PTSRMLS_CC); \
                    zend_first_try {
#define PHPE_END } zend_catch { \
                /* int exit_status = EG(exit_status); */ \
                } zend_end_try(); \
                php_embed_shutdown(TSRMLS_C);
