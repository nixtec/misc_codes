/******************************************************************************/
/*                 Implementing FileManager for MailRunner                    */
/*                                                                            */
/*  Author: Faisal                                                            */
/*  Modified by: David                                                        */
/******************************************************************************/

//Define Libraries

#include <stdio.h>                  /* standard input/output routines.        */
#include <stdlib.h>
#include <unistd.h>                 /* access(), etc.                         */
#include <sys/stat.h>               /* stat(), etc.                           */
#include <fcntl.h>                  /* open(), close(), etc.                  */
#include <malloc.h>                 /* malloc(), etc.                         */
#include <string.h>                 /* strcpy(), strcat().                    */
#include <dirent.h>                 /* struct DIR, struct dirent, opendir().. */
#include <sys/types.h>
#include <sys/param.h>

#include "fileManager.h"
#include "SecMngr.h"

// This buffer is used to store the full path of the current Working directory.

char cwd[MAX_DIR_PATH+1];

char ch[50];

/*********************** FUNCTIONS *************************************/

char* curr_dir()
{
	if (!getcwd(cwd, MAX_DIR_PATH+1)) {
		perror("getcwd");
		exit(1);
	}

	return cwd;
}

void change_dir(char* user, char* new_path)
{
	char* old_path;

	if(strcmp(new_path, "..") != 0)
	{
		if(verifyUserPermission(user, new_path) == false)
		{
			printf("User: %s, does not have access to this path: %s \n", user, new_path);
			exit(1);
		}

		old_path = curr_dir();
		old_path = strcat(old_path, "/");
		new_path = strcat(old_path, new_path);
	}

	chdir(new_path);
}
void list()
{
	struct dirent * entry;
	struct stat buf;
	int exists;
	char* dir_path;
	DIR * dir;

	dir_path = curr_dir();

	dir = opendir(dir_path);
	if (!dir)
	{
		exit(1);
	}

	//printf("\nDirectory contents:\n");

	printf("Name \t\t\t Size \t Time Last Modified \n");

	while ( (entry = readdir(dir)) != NULL)
	{
		exists = stat(entry->d_name, &buf);
		printf("%s \t\t %10ld \t %10ld \n", entry->d_name,buf.st_size,buf.st_atime);
	}
}

void move(char* source_path, char* dest_path)
{
	copy(source_path, dest_path);

	unlink(source_path);
}

void copy(char* source_path, char* dest_path)
{
	FILE* f_from;
	FILE* f_to;
	char buf[MAX_LINE_LEN+1];

	f_from = fopen(source_path, "r");
	if (!f_from)
	{
		printf("Cannot open source file: %s", source_path);
		exit(1);
	}
	f_to = fopen(dest_path, "w+");
	if (!f_to)
	{
		printf("Cannot open destination file: %s", dest_path);
		exit(1);
	}

	/* copy source file to target file, line by line. */
	while (fgets(buf, MAX_LINE_LEN+1, f_from)) {
	if (fputs(buf, f_to) == EOF) {  /* error writing data */
		printf("Error writing to target file");
		exit(1);
	}
	}
	if (!feof(f_from)) { /* fgets failed _not_ due to encountering EOF */
		printf("Error reading from source file");
		exit(1);
	}

	/* close source and target file streams. */
	if (fclose(f_from) == EOF) {
		fprintf(stderr, "Error when closing source file: ");
	}
	if (fclose(f_to) == EOF) {
		fprintf(stderr, "Error when closing target file: ");
		perror("");
	}
}
