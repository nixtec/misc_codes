// Author: Swetha

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#define ADMIN  1
#define USER 4

void view_file();
void view_mail();

//mail operations
void recv_msg();
void view_inbox();
void send_mail();

void delay(int seconds)
{
 clock_t endwait;
 endwait = clock () + seconds * CLOCKS_PER_SEC ;
 while (clock() < endwait) {}
}

int main()
{
    FILE *f;
    char s[1000];
    char user[80];
    char username[20];
    char passwd[20];
    char ch = 0;

    f=fopen("infile.txt","r");
    if(!f)
    {
          printf("Unable to open the file\n");
          return 1;
    }
    //printf("Enter username:\n");
    //gets (user);
   /* while(fgets(s,1000,f))
    {
      //printf("%s",s);
      //printf("Inside the while\n");
      if(s == user)
           printf("Correct user");
      else
        //   printf("User doesnt exist\n");
    }*/

          //printf("%s",s);

    printf("********************************MAIL RUNNERS********************************\n\n");

    printf("Enter your username: \n");
    scanf("%s",&username);

    printf("Enter your password: \n");
    scanf("%s",&passwd);

    printf("Welcome User\n\n");

    printf("You can chose from the following options: \n");
    printf("FILE(F)\t\t");
    printf("MAIL(M)\n\n");

   do
    {
       printf("Enter one of the above options, q to quit(F/M): ");
       //scanf("%c",&ch);
       ch = getche();
       printf("\n");
       switch(ch)
       {
         case 'F':
         case 'f':
	               view_file();
	               break;

         case 'M':
         case 'm':
                   view_mail();
                   break;

         default: printf("Incorrect response....enter File or Manager\n\n");
	              break;
      }
     }while(ch != 'q');

    fclose(f);
    getch();
    return 0;
}

void view_file()
{
      printf("Inside the File\n");
      delay(2);
      printf("Following options in a file:\n\n");
}

void view_mail()
{
      char vm = 0;

      printf("Following options in a mail\n");
      delay(2);
      printf("New Messages (N)\n");
      printf("Inbox (I)\n");
      printf("Send Mail (S)\n\n");

      do
      {
        printf("Enter one of the above options in a MAIL, q to quit(N/I/S): ");
       //scanf("%c",&ch);
       vm = getche();
       printf("\n");
       switch(vm)
       {
         case 'N':
         case 'n':
                  recv_msg();
	              break;

         case 'I':
         case 'i':
                  view_inbox();
                  break;

         case 'S':
         case 's':
                  send_mail();
                  break;

         default: printf("Incorrect\n\n");
	              break;
      }
     }while(vm != 'q');

}

void recv_msg()
{
  int num_msg = 0;

  delay(2);
  printf("Inside the recv msg\n");

  while(num_msg)
  {
    printf("There are no new messages rite now!!!\n");
    num_msg += num_msg;
  }
  printf("There are %d messages in your Inbox\n\n",num_msg);
}

void view_inbox()
{
   delay(2);
   printf("Inside the inbox\n\n");
}

void send_mail()
{
     char to[40];
     char source[40];
     char c = 0;

     delay(2);
     printf("Inside the send mail\n");

     printf("Compose a new message:\n\n");

}
