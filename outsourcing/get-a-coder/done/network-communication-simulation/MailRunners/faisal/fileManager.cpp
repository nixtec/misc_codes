/******************************************************************************/
/*                 Implementing FileManager for MailRunner                    */
/*                                                                            */
/*Author: Faisal                                                              */
/*                                                                            */
/******************************************************************************/

//Define Libraries

#include <stdio.h>                  /* standard input/output routines.        */
#include <unistd.h>                 /* access(), etc.                         */
#include <sys/stat.h>               /* stat(), etc.                           */
#include <fcntl.h>                  /* open(), close(), etc.                  */
#include <malloc.h>                 /* malloc(), etc.                         */
#include <string.h>                 /* strcpy(), strcat().                    */
#include <dirent.h>                 /* struct DIR, struct dirent, opendir().. */
#include <sys/types.h>
#include <sys/param.h>


#define MAX_DIR_PATH 2048

// This buffer is used to store the full path of the current Working directory.

char cwd[MAX_DIR_PATH+1];

char ch[50];

void move(char argc, char *argv[]);


/*********************** (Print the Path) *************************************/

int curr_dir()
{
	if (!getcwd(cwd, MAX_DIR_PATH+1)) {
		perror("getcwd");
		exit(1);
	}
	printf("\nPresent Working Directory -> %s\n",cwd);

return 0;

}


/************************** (Main Function) ***********************************/

int main()

{
    FILE *createFile;


    char path[MAX_DIR_PATH+1], path1[MAX_DIR_PATH+1], path2[MAX_DIR_PATH+1];

    mkdir("e:/MailRunners/PublicDir/User_1");
    mkdir("e:/MailRunners/PublicDir/User_1/CSI_500");
    createFile = fopen("e:/MailRunners/PublicDir/User_1/CSI_500/Assignment_1.txt", "w");
    mkdir("e:/MailRunners/PublicDir/User_1/CSI_500/HomeWork");
    mkdir("e:/MailRunners/PublicDir/User_2");
    mkdir("e:/MailRunners/PublicDir/User_2/CSI_519");
    mkdir("e:/MailRunners/PublicDir/User_2/CSI_518");
    mkdir("e:/MailRunners/PublicDir/User_3");
    mkdir("e:/MailRunners/PublicDir/User_3/CSI_503");
    mkdir("e:/MailRunners/PublicDir/User_4");
    mkdir("e:/MailRunners/PublicDir/User_5");
     mkdir("e:/MailRunners/PublicDir/User_5/CSI_500");
    createFile = fopen("e:/MailRunners/PublicDir/User_5/CSI_500/Assignment_3.txt", "w");

	curr_dir();


    printf("\n[Mail Runners]$ ");
      scanf("%s",&ch);



/**************** Implement the File Manager's Functions **********************/

while(ch[0]!='q') {

    switch(ch[0])
    {
        case 'l':                       // List the Directory (Folders or Files)
/******************************************************************************/

	          char* dir_path;		                    // Path to the directory

// This structure is used for storing the name of each entry in turn

              struct dirent * entry;

              DIR * dir;
	          dir_path = cwd;
          	  dir = opendir(dir_path);

                 if (!dir) {
		              perror("opendir");
		              exit(1);
                 }

            printf("\nDirectory contents:\n");

            while ( (entry = readdir(dir)) != NULL) {
                  printf("%s\n", entry->d_name);
        }

        break;

       case 'c':     // Move within the Directories (List the Folders or Files)


//******************************************************************************


    FILE* f_from;		/* stream of source file. */
    FILE* f_to;			/* stream of target file. */

    #define MAX_LINE_LEN 1000

    char* file_path_from;	/* path to source file.   */
    char* file_path_to;		/* path to target file.   */

    char buf[MAX_LINE_LEN+1];   /* input buffer.          */

    char argc[80], *argv[80];


    /* Get the source and destination names. */

    printf("\nEnter source file: ");
    scanf("%s", &argv[1]);
    printf("\nEnter destination file: ");
    scanf("%s", &argv[2]);

    file_path_from = argv[1];
    file_path_to = argv[2];

    /* open the source and the target files. */
    f_from = fopen(file_path_from, "r");
    if (!f_from) {
	fprintf(stderr, "Cannot open source file: ");
	perror("");
	//exit(1);
    }
    f_to = fopen(file_path_to, "w+");
    if (!f_from) {
	fprintf(stderr, "Cannot open target file: ");
	perror("");
	//exit(1);
    }

    /* copy source file to target file, line by line. */
    while (fgets(buf, MAX_LINE_LEN+1, f_from)) {
	if (fputs(buf, f_to) == EOF) {  /* error writing data */
	    fprintf(stderr, "Error writing to target file: ");
	    perror("");
	    //exit(1);
	}
    }
    if (!feof(f_from)) { /* fgets failed _not_ due to encountering EOF */
        fprintf(stderr, "Error reading from source file: ");
        perror("");
        //exit(1);
    }

    /* close source and target file streams. */
    if (fclose(f_from) == EOF) {
	fprintf(stderr, "Error when closing source file: ");
	perror("");
    }
    if (fclose(f_to) == EOF) {
	fprintf(stderr, "Error when closing target file: ");
	perror("");
    }

//******************************************************************************

        break;

        default: printf("\nIncorrect response\n\n");

   }

       printf("\n[Mail Runners]$ ");
       scanf("%s", &ch);
 }


   return 0;
}
