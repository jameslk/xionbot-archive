/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
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
