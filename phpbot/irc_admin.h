/*
COPYRIGHT XionBot Developement Team 2003 - 2005
http://xionbot.sourceforge.net
Licensed under the (Open Source) GNU Public License (GPL).
A copy of the GPL license should have been supplied.
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
