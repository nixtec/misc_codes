// David

enum bool
{
	false = 0,
	true = 1,
};
extern enum bool validateCredential(char *username,char *password);
extern enum bool verifyUserPermission(char *username,char *path);
extern void addUserPath(char *user,char *path);

