/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

unsigned int act_start(void) {
    /* Met Letter / Met Number / Met Symbol / Met Whitespace / Met EOS */
    actev.start = {ACT_PUT, ACT_ERROR, ACT_PUT, ACT_IGNORE, ACT_IGNORE};
