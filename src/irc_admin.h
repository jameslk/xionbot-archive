/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

int adm_admtoint(char *adm);
unsigned int adm_isadminmask(unsigned char *mask, char *adm);
char* adm_userisadmin(char *user);
char *adm_isadminpass(char *adm, char *pass);
unsigned int adm_login(unsigned char *mask, char *user, char *pass);
unsigned int adm_loginuser(unsigned char *mask, char *user, int access);
unsigned int adm_logoutuser(char *mask);
int adm_cmasktoint(unsigned char *mask);
unsigned int adm_maskislogdin(unsigned char *mask);
unsigned int adm_userislogdin(char *user);
int adm_getcuraccess(unsigned char *mask);
