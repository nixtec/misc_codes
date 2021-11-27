// Author: Swetha

/*  I used Linux for this program, implemented ls -a system call and cd system call.
Refer the doc as to how to execute the prog on ssh.*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>

int main()
{
  int i,j;
  //int count = 0;
  //int count1 = 0;
  char ch[50];
  char ch1[50];
  //char chs[2];
  char user[50];
  char user_name[5] = {'A','B','C','D','E'};
  char cmd[50],list[50];
  char pathname[100];
  char command[50];
  //char dir_array[6][50];

  system("clear");
  FILE *fp;
  fp = fopen("names.txt","r");
  fgets(user,50,fp);

  for(i = 0 ; i < 5 ; i++)
  {
    for(j=1;j<=5;j++)
     {
     //printf("Inside the mkdir command\n");
     sprintf(cmd,"mkdir %c%d",user_name[i],j);
     system(cmd);
   }
  }
  printf("After creating directories\n");
  do
    {
      printf("[Mail Runners]$ ");
      scanf("%s%s",&ch,&ch1);

      printf("%s",ch1);

     switch(ch[0])
      {
      case 'c':  printf("Inside the cd command\n");
 		         getcwd(pathname, 100);
    		     printf("Starting of in: %s\n", pathname);
    		     if (chdir(ch1) != 0)
       		 	    printf("Can't change dir. Sorry.\n");
		         else
                 {
       			   getcwd(pathname,100);
                   printf("Done it - Now we're in %s\n", pathname);
                 }
                 break;

      case 'l':  printf("Inside the ls command\n");
	             sprintf(list,"ls -a");
                 system(list);
                 break;

      default :  printf("Incorrect response\n\n");
                 break;
      }
    }while(ch[0]!='q');
  fclose(fp);
  exit(0);
}
