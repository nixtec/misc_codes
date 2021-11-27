// David

#include "fileManager.h"

struct mail_tag {
	char  from[50];
	char  to[50];
	char  date[100];
	char  sub[100];
	char  mailpath[200];
};

// usernames should be in order
struct address_list {
	char *username;
};

#define NO_USERS (sizeof (Address_list) / sizeof (Address_list[0]))

extern void view_mailbox ();
extern void copy_mails_to_inbox ();
extern void send_mail ();
extern void readline (char *line);
extern void change_format ();
extern int  search (char *to);
extern void updateMsgPath(char* old, char* new);
extern struct mail_tag * findMessage_byPath(char* path);

#define LINE_LENGTH 100
#define MAIL_LENGTH 3000
