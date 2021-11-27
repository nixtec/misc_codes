// Author: Krupa

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

// mail tag: contains mail information
struct mail_tag {
	char  from[50];
	char  to[50];
	char  sub[100];
	char  date[100];
	char  mailpath[200];
};

// usernames should be in order
struct address_list {
	char *username;
} Address_list[] = {
	"user1",
	"user2",
	"user3",
	"user4",
	"user5"
};

// Number of users
#define NO_USERS (sizeof (Address_list) / sizeof (Address_list[0]))

// function declarations
void view_mailbox ();
void copy_mails_to_inbox ();
void send_mail ();
void readline (char *line);
void change_format ();
int  search (char *to);
void create_mail (char *path);


#define LINE_LENGTH 100
#define MAIL_LENGTH 3000

char mail[MAIL_LENGTH];

char line[LINE_LENGTH];

char date[LINE_LENGTH];

FILE *fp;

int main (int argc, char * argv[])
{
	register char *p;

	while (1) {
		printf ("V-----> view mail box\n");
		printf ("C-----> copy mails from mailbox to inbox\n");
		printf ("S-----> send mail\n");
		printf ("E-----> Exit: ");

		if (!fgets (line, LINE_LENGTH, stdin)) {
			fprintf (stderr, "Error: reading fron standard input\n");
			exit (1);
		}

		for (p = line; *p == ' ' || *p == '\t'; p++);	// Skip white space

		switch (*p) {
			case 'v':
			case 'V': view_mailbox ();
				break;

			case 'c':
			case 'C': copy_mails_to_inbox ();
				break;

			case 's':
			case 'S': send_mail ();
				break;

			case 'e':
			case 'E': printf ("Thank u for using mail manager\n");
				exit (0);
		}
	}

	return 0;
}

/*	View mailbox: to view the mails for perticular user
				input, username
				displays mails
				returns void
*/
void view_mailbox ()
{
	struct mail_tag address;
	char username[100];

	setbuf (stdout, NULL); // clear the stdout buffer
	printf ("Enter user: ");
	readline (username);	// read from a standard input

	// check validity of username
	if (search (username)) {
		printf ("User not found\n");
		return;
	}

	strcat (username, ".txt");
	fp = fopen (username, "r");

	if (!fp) {
		fprintf (stderr, "Error: opening file\n");
		exit (2);
	}

	printf ("List of mails:\n");
	while (fread ((void*)&address, 1, sizeof (address), fp) == sizeof (address)) {
		printf ("From: %s\n",address.from);
		printf ("Subject: %s\n",address.sub);
		printf ("Send date and time: %s\n",address.date);
	}

	fclose (fp);
}

/* Send mail: send a mail to perticular user
	input,  from aadress
			to address
			subject
			mail text
	return void
*/
void send_mail ()
{
	struct mail_tag address;
	char to[300];
	time_t mytime = time(0);	// returns number of seconds

	printf ("Enter FROM address: ");
	readline (address.from);
	setbuf (stdout, NULL);			// clear the stdout buffer
	printf ("Enter TO address: ");
	readline(address.to);			// read from a standard input

	// check validity of destination address
	if (search (address.to)) {
		printf ("User not found\n");
		return;
	}

	setbuf (stdout, NULL);			// clear the stdout buffer
	printf ("Enter subject: ");
	readline(address.sub);			// read from a standard input

	strcpy (address.date, asctime(localtime(&mytime)));	// convert seconds to human readable format
	strcpy (date, address.date);
	change_format ();
	strcat (date, ".txt");		// create mail file in the name (date).txt

	strcpy (address.mailpath, ".");		// Current directory

	strcat (address.mailpath, "/CommonMailBox/");		// add common mail box to path
	strcat (address.mailpath, date);		// create mail file in common mail box

	strcpy (to, address.to);
	strcat (to, ".txt");

	fp = fopen (to, "a");		// create a file in append mode, mails information stored here
	fwrite ((void*)&address, 1, sizeof (address), fp);

	fclose (fp);

	// write a mail text in the file
	create_mail (address.mailpath);
}

/*	Readline: read a line from standard input up to '\n' or up to LINE_LENGTH
		input, characters
		returns void
*/
void readline (char line[])
{
	int 	n;

	if (!fgets (line, LINE_LENGTH, stdin))
		exit (1);
	n = strlen (line);
	if (line [n - 1] == '\n')
		line [n - 1] = '\0';
}

/*	change format: to replace space and colon with underscore from string
	input, null
	return void
*/
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

/*	Search: search username found or not
	input, userbane
	retuns 0 if found else -1
*/
int search (char *to)
{
	int i;

	for (i = 0; i < NO_USERS; i++)
		if (strcmp (Address_list[i].username, to) == 0)
			return 0;

	return -1;
}

/*	write mail: write a mail contents to a userdefined buffer
	input, characters up tp end of file (EOF)
	returns void
*/
void write_mail ()
{
	int i, c;

	for (i = 0; ((c = getchar()) != EOF) && (i < MAIL_LENGTH); i++)
		mail[i] = c;

	mail[i] = '\0';
}

/*  create mail: write mail contents to mail file
	input, enter mail contents
	return void
*/
void create_mail (char *path)
{
	printf ("Please enter mail contents\n");
	write_mail (); // read from stdin to mail buffer
	fp = fopen (path, "w");
	fwrite ((void*)mail, 1, strlen (mail) + 1, fp); // write mail buffer to mail file
	fclose (fp);
}

/*	copy_mails to Inbox: copy mails from CommonMailBox to user Inbox
	input, username
	return void
*/
void copy_mails_to_inbox ()
{
	char username[100];
	struct mail_tag address;

	printf ("Enter user name: ");
	readline (username);	// read from a standard input

	strcpy (line, "./");
	strcat (line, username);	// User Inbox

	// check validity of username
	if (search(username)) {
		printf ("User not found\n");
		return;
	}

	strcat (username, ".txt");
	fp = fopen (username, "r");

	if (!fp) {
		fprintf (stderr, "Error: opening file\n");
		exit (3);
	}

	printf ("Mails copying to user inbox\n");
	while (fread ((void*)&address, 1, sizeof (address), fp) == sizeof (address)) {
		strcpy (mail, "mv ");
		strcat (mail, address.mailpath);
		strcat (mail, " ");
		strcat (mail, line);
		// system: execute shell comands
		system (mail);		// run a move command: move source to destination
	}

	fclose (fp);
	fp = fopen (username, "w"); // empty the mail box
	fclose (fp);
}
