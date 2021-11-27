// Author: Swetha
// Modified By: David

#include<stdio.h>
#include<stdlib.h>
#include "interface.h"
#define ADMIN  1
#define USER 4

void printHeader()
{
    printf("********************************MAIL RUNNERS********************************\n\n");
}

void printMainScreen()
{
    printf("You can chose from the following options: \n");
    printf("FILE(F)\t\t");
    printf("MAIL(M)\n");
}

void printFooter()
{
    printf("\n");
    printf("Logout(L) \t\t Home Screen(H)");
    printf("Please choose from the options above: ");
}

void printMailOptions()
{
    printf("New Messages (N)\n");
    printf("Inbox (I)\n");
    printf("Send Mail (S)\n");
}

void printFolderList()
{

}

void printFolderOptions()
{
    printf("To view the contents of the given folder type (ls) \n");
    printf("To change the current directory, type (cd) and the directory you would like to change to \n");
}

void clear()
{
    system ("cls");
}

int main()
{
    return 1;
}
