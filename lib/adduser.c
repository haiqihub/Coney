#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PRIVATE void updateUsers();
PRIVATE void updateUsersPassword();

/*****************************************************************************
 *                               addUser
 *****************************************************************************/
/**
 * to add a new user
 * 
 * @param fmt The format string.
 * 
 * @return How many chars have been printed.
 *****************************************************************************/

/* Add User */
PUBLIC void addUser(char *username, char *password)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        if (strcmp(users[i], username) == 0)
        {
            printf("User exists!\n");
            return;
        }
    }
    if (usercount == 2)
    {
        printf("No more users\n");
        return;
    }
    if (strcmp(users[0], "empty") == 0)
    {
        strcpy(users[0], username);
        strcpy(passwords[0], password);
        usercount++;
        updateUsers();
        updateUsersPassword();
	printf("%s is created.\n",users[0]);
        return;
    }
    if (strcmp(users[1], "empty") == 0)
    {
        strcpy(users[1], username);
        strcpy(passwords[1], password);
        usercount++;
        updateUsers();
        updateUsersPassword();
	printf("%s is created.\n",users[1]);
        return;
    }
}
/* Update Users */
PRIVATE void updateUsers()
{
    int i = 0, count = 0;
    editCover("Users", "");
    if (strcmp(users[0], "empty") != 0)
    {
        editAppand("Users", users[0]);
        editAppand("Users", " ");
    }
    else
    {
        editAppand("Users", "empty");
    }
    if (strcmp(users[1], "empty") != 0)
    {
        editAppand("Users", users[1]);
        editAppand("Users", " ");
    }
    else
    {
        editAppand("Users", "empty");
    }
}

/* Update UsersPassword */
PRIVATE void updateUsersPassword()
{
    int i = 0, count = 0;
    editCover("UsersPWD", "");
    if (strcmp(passwords[0], "") != 0)
    {
        editAppand("UsersPWD", passwords[0]);
        editAppand("UsersPWD", " ");
    }
    else
    {
        editAppand("UsersPWD", "empty");
    }
    if (strcmp(passwords[1], "") != 0)
    {
        editAppand("UsersPWD", passwords[1]);
        editAppand("UsersPWD", " ");
    }
    else
    {
        editAppand("UsersPWD", "empty");
    }
}
