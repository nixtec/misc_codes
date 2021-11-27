// Author: Krupa
// Modified By: David

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>

#include "mailrunner6.h"

char mail[MAIL_LENGTH];

char line[LINE_LENGTH];

char date[LINE_LENGTH + 100];

FILE *fp;

char* Address_list[] = {"user1","user2","user3","user4","user5"};

struct mail_tag messages[100];
int numMessages = 0;

int main (int argc, char * argv[])
{
	//register char *p;

	//while (1) {
	//	printf ("V-----> view mail box\n");
	//	printf ("C-----> copy mails from mailbox to inbox\n");
	//	printf ("S-----> send mail\n");
	//	printf ("E-----> Exit: ");
//
//		if (!fgets (line, LINE_LENGTH, stdin)) {
//			fprintf (stderr, "Error: reading fron standard input\n");
//			exit (1);
//		}
//
//		for (p = line; *p == ' ' || *p == '\t'; p++);	// Skip white space
//
//		switch (*p) {
//			case 'v':
//			case 'V': view_mailbox ();
//				break;
//
//			case 'c':
//			case 'C': copy_mails_to_inbox ();
//				break;
//
//			case 's':
//			case 'S': send_mail ();
//				break;
//
//			case 'e':
//			case 'E': printf ("Thank u for using mail manager\n");
//				exit (0);
//		}
//	}
//

	char* user;

	user = "user1";

	send_mail("user1", "user2", "this is a test", "this should be the contents");

	sleep(2);

	send_mail("user1", "user2", "this is another test", "this should also be the contents");

	copy_mails_to_inbox("user1");

	view_mailbox("user1");

	return 0;
}

void view_mailbox (char* user)
{

	if (search (user)) {
		printf ("User not found\n");
		return;
	}

	struct dirent * entry;

	char* dir_path;
	char* file_path = malloc(1000);

	DIR * dir;

	struct mail_tag * msg;

	dir_path = curr_dir();
	strcat(dir_path, "/MailRunners/Mail/");
	strcat(dir_path, user);
	strcat(dir_path, "/");

	dir = opendir(dir_path);
	if (!dir)
	{
		exit(1);
	}

	int numMessages = 0;

	while ( (entry = readdir(dir)) != NULL)
	{
		strcpy(file_path, dir_path);
		strcat(file_path, entry->d_name);

		msg = findMessage_byPath(file_path);
		if(msg != NULL)
		{
			printf("Message: %d \n", numMessages);
			printf("From: %s\n", msg->from);
			printf("Subject: %s\n", msg->sub);
			printf("Send date and time: %s\n\n",msg->date);

			numMessages++;
		}
	}

	if(numMessages == 0)
	{
		printf("No messages found for user: %s \n", user);
	}

	free(file_path);
}

void send_mail (char* to, char* from, char* subject, char* content)
{
	struct mail_tag newMsg;
	char* path;
	time_t mytime = time(0);

	strcpy(newMsg.to, to);
	strcpy(newMsg.from, from);
	strcpy(newMsg.sub, subject);

	if (search (newMsg.to)) {
		printf ("User not found\n");
		return;
	}

	strcpy(newMsg.date, asctime(localtime(&mytime)));
	strcpy(date, newMsg.date);
	change_format ();

	path = curr_dir();

	strcat(path, "/MailRunners/Mail/");
	strcat(path, to);
	strcat(path, "-");
	strcat(path, date);
	strcat(path, ".eml");

	strcpy (newMsg.mailpath, path);


	fp = fopen (path, "w+");
	fputs(content, fp);

	fclose (fp);

	messages[numMessages] = newMsg;
	numMessages++;
}

void readline (char line[])
{
	int 	n;

	if (!fgets (line, 100, stdin))
		exit (1);
	n = strlen (line);
	if (line [n - 1] == '\n')
		line [n - 1] = '\0';
}

void change_format ()
{
	unsigned int i;

	i = strlen (date);
	if (date[i - 1] == '\n')
		date[i - 1] = '\0';

	for (i = 0; i < strlen (date); i++)
		if (date[i] == ' ' || date[i] == ':')
			date[i] = '_';
}

int search (char *to)
{
	int i;

	for (i = 0; i < NO_USERS; i++)
		if (strcmp (Address_list[i], to) == 0)
			return 0;

	return -1;
}

void copy_mails_to_inbox (char* user)
{

	if (search(user)) {
		printf ("User not found\n");
		return;
	}

	struct dirent * entry;

	char* dir_path;

	char* source_path = malloc(1000);
	char* dest_path = malloc(1000);

	char* result;
	DIR * dir;

	dir_path = curr_dir();
	strcat(dir_path, "/MailRunners/Mail/");

	dir = opendir(dir_path);
	if (!dir)
	{
		exit(1);
	}

	while ( (entry = readdir(dir)) != NULL)
	{
		strcpy(source_path, dir_path);
		strcpy(dest_path, dir_path);

		result = strstr(entry->d_name, user);
		if (result != NULL)
		{
			if(strstr(result, ".eml") != NULL)
			{
				strcat(dest_path, user);
				strcat(dest_path, "/");
				strcat(dest_path, entry->d_name);

				strcat(source_path, entry->d_name);

				move(source_path, dest_path);
				updateMsgPath(source_path, dest_path);
			}
		}
	}

	free(source_path);
	free(dest_path);
}

struct mail_tag * findMessage_byPath(char* path)
{
	int i;
	for(i = 0; i < numMessages; i++)
	{
		if(strcmp(messages[i].mailpath, path) == 0)
		{
			return &messages[i];
		}
	}

	return NULL;
}

void updateMsgPath(char* old, char* new)
{
	struct mail_tag * msg;
	msg = findMessage_byPath(old);

	if(msg == NULL)
	{
		printf("Unable to find the message");
		exit(1);
	}

	strcpy(msg->mailpath, new);
}

