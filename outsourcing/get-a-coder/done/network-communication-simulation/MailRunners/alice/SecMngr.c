// Author: Alice Pannu

//PROGRAM BEGINS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum bool
{
	false = 0,
	true = 1,
};

struct pathListNode
{
	char *path;
	struct pathListNode *next;
};
typedef struct pathListNode pathListNode;

struct  usersListNode
{
	char *username;
	pathListNode *paths;
	struct usersListNode *next;
};
typedef struct usersListNode usersListNode;

enum bool validateCredential(char *username,char *password);
enum bool verifyUserPermission(char *username,char *path);
void addUserPath(char *user,char *path);

//usernames and passwords
char users[5][20]={"user1","user2","user3","user4","user5"};
char passwords[5][20]={"pass1","pass2","pass3","pass4","pass5"};

int nUsers = 5; //the number of users
usersListNode *usersPaths=NULL; //the valid paths for each user


//Main Module Begins
int main()
{
	//adding user directories
	addUserPath("user2","//Mailrunners/Files/User2/csi500/homework");
	addUserPath("user2","//Mailrunners/Files/User2/csi503");
	addUserPath("user2","//Mailrunners/Mails/User2");
	addUserPath("user3","//Mailrunners/Files/User3/csi509");
	addUserPath("user3","//Mailrunners/Mails/User3");
	addUserPath("user4","//Mailrunners/Files/User4/csi518");
	addUserPath("user4","//Mailrunners/Files/User4/csi519");
	addUserPath("user4","//Mailrunners/Mails/User4");
	addUserPath("user5","//Mailrunners/Files/User5/csi530");
	addUserPath("user5","//Mailrunners/Files/User5/csi539");
	addUserPath("user5","//Mailrunners/Mails/User5");

	//Checking if functions work correctly
	validateCredential("user1","pass1");
	validateCredential("user2","pass1");
	validateCredential("user7","pass2");

	verifyUserPermission("user2","/Mailrunners/Files/User2/csi500/homework");
	verifyUserPermission("user2","/Mailrunners/Files/User2/csi503");
	verifyUserPermission("user3","/Mailrunners/Files/User3/csi500");
	verifyUserPermission("user7","/Mailrunners/Files/User2/csi503");

	return 0;
}
//Main Module Ends


//verifying username and password
enum bool validateCredential(char *username,char *password)
{
	int i;

	printf("\nVerifying Username\n");
	for(i=0;i<nUsers;i++)
	{
		if (strcmp(username,users[i])==0)//compare usernames
		{
			printf("Username is correct\n\n");
			printf("Verifiying Password\n");
			if (strcmp(password,passwords[i])==0)//compare passwords
			{
				printf("Password is correct\n\n");
				if (i==0)
				printf("Welcome Superuser\n\n");
				return true;
			}
			else
			{
				printf("Incorrect Password\n\n");
				return false;
			}
		}
	}
	printf("Incorrect Username. Please Try Again\n\n");
	return false;
}


//verifying user's access to a path
enum bool verifyUserPermission(char *username,char *path)
{
	usersListNode *tmp;
	pathListNode *aux;

	tmp=usersPaths;
	while(tmp!=NULL)//searching for the username
	{
		if (strcmp(tmp->username,username)==0)
		{
			aux=tmp->paths;
			//searching for the path
			printf("\nVerifying path\n");
			while(aux!=NULL)
			{
				if (strstr(aux->path,path)!=NULL)//checking path
				{
					printf("Access to Path Allowed\n\n");
					return true;
				}
				aux=aux->next;
			}
			{
				printf("Access to Path Not Allowed\n\n");
				return false;
			}
		}
		tmp=tmp->next;
	}
	printf("Incorrect Username. Please Try Again\n\n");
	return false;
}


//add a new valid path for a user
void addUserPath(char *user,char *path)
{
	usersListNode *tmp;
	pathListNode *aux;

	//if no user path yet assigned
	if (usersPaths==NULL)
	{
		usersPaths=(usersListNode *)malloc(sizeof(usersListNode));
		usersPaths->next=NULL;
		usersPaths->username=(char *)malloc((strlen(user)+1)*sizeof(char));
		strcpy(usersPaths->username,user);
		tmp=usersPaths;
		tmp->paths=NULL;
	}
	else
	{
		//add a path to an already assigned user
		tmp=usersPaths;
		while(tmp!=NULL && strcmp(user,tmp->username)!=0)
			tmp=tmp->next;
		if (tmp==NULL)
		{
			tmp=(usersListNode *)malloc(sizeof(usersListNode));
			tmp->next=usersPaths;
			tmp->username=(char *)malloc((strlen(user)+1)*sizeof(char));
			strcpy(tmp->username,user);
			usersPaths=tmp;
			tmp->paths=NULL;
		}
	}
	aux=(pathListNode *)malloc(sizeof(pathListNode));
	aux->next=tmp->paths;
	aux->path=(char *)malloc((strlen(path)+1)*sizeof(char));
	strcpy(aux->path,path);
	tmp->paths=aux;
}

//PROGRAM ENDS


