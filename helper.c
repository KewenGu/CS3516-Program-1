/* Kewen Gu, 09-11-2014 */
/* This file contains all the helper functions for MEClient.c and MEServer.c */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>     /* for sockaddr_in and inet_addr() */
#include <unistd.h>       /* for close() */
#include <ctype.h>

#include "header.h"

/* If error generated, print error message and die */
void DieWithError(char *errorMessage)
{
	perror(errorMessage);
	exit(1);
}

/* Slice the long string into tokens */
int StrSlicing(char *buf, char *args[] , char *s)
{
	int i = 0;
	/* get the first token */
	char *token = strtok(buf, s);
	args[i] = token;

	/* walk through other tokens */
	while(token != NULL) 
	{
	    token = strtok(NULL, s);
	    args[++i] = token;
	}
	return i;
}

/* Numbering the command */
int CmdNumbering(char *arg)
{
	int num;
	if (!strcasecmp(arg, "login"))
		num = LOGIN;
	else if (!strcasecmp(arg, "add"))
		num = ADD;
	else if (!strcasecmp(arg, "update"))
		num = UPDATE;
	else if (!strcasecmp(arg, "remove"))
		num = REMOVE;
	else if (!strcasecmp(arg, "find"))
		num = FIND;
	else if (!strcasecmp(arg, "list"))
		num = LIST;
	else if (!strcasecmp(arg, "locate"))
		num = LOCATE;
	else if (!strcasecmp(arg, "quit"))
		num = QUIT;
	return num;
}

/* Check whether the strings meet the requirements */
int StrCheck(int argNum, char *args[])
{
	switch (CmdNumbering(args[0]))
	{
		case LOGIN:
			if (argNum == 2)
				return 0;
			else
			{
				fprintf(stderr, "Usage: %s <user_name>\n", args[0]);
				return 1;
			}
		  break;

		case ADD:
			if (argNum != 6)
			{
				fprintf(stderr, "Usage: %s <id_number> <first_name> <last_name> <gender> <location>\n", args[0]);
				return 1;
			}
			else if (strlen(args[1]) > 9)
			{
				fprintf(stderr, "id_umber is too long (9 bytes allowed)\n");
				return 1;
			}
			else if (strlen(args[2]) > 20)
			{
				fprintf(stderr, "first_name is too long (20 bytes allowed)\n");
				return 1;
			}
			else if (strlen(args[3]) > 25)
			{
				fprintf(stderr, "last_name is too long (25 bytes allowed)\n");
				return 1;
			}
			else if (strlen(args[4]) > 1)
			{
				fprintf(stderr, "gender is too long (1 bytes allowed)\n");
				return 1;
			}
			else if (strlen(args[5]) > 30)
			{
				fprintf(stderr, "location is too long (30 bytes allowed)\n");
				return 1;
			}
			else
				return 0;
		  break;

		case UPDATE:
			if (argNum != 6)
			{
				fprintf(stderr, "Usage: %s <id_number> <first_name> <last_name> <gender> <location>\n", args[0]);
				return 1;
			}
			else if (strlen(args[1]) > 9)
			{
				fprintf(stderr, "id_number is too long (9 bytes allowed)\n");
				return 1;
			}
			else if (strlen(args[2]) > 20)
			{
				fprintf(stderr, "first_name is too long (20 bytes allowed)\n");
				return 1;
			}
			else if (strlen(args[3]) > 25)
			{
				fprintf(stderr, "last_name is too long (25 bytes allowed)\n");
				return 1;
			}
			else if (strlen(args[4]) > 1)
			{
				fprintf(stderr, "gender is too long (1 bytes allowed)\n");
				return 1;
			}
			else if (strlen(args[5]) > 30)
			{
				fprintf(stderr, "location is too long (30 bytes allowed)\n");
				return 1;
			}
			else
				return 0;
		  break;

		case REMOVE:
			if (argNum != 2)
			{
				fprintf(stderr, "Usage: %s <id_number>\n", args[0]);
				return 1;
			}
			else if (strlen(args[1]) > 9)
			{
				fprintf(stderr, "id_number is too long (9 bytes allowed)\n");
				return 1;
			}
			else
				return 0;
		  break;

		case FIND:
			if (argNum != 3)
			{
				fprintf(stderr, "Usage: %s <first_name> <last_name>\n", args[0]);
				return 1;
			}
			else if (strlen(args[1]) > 20)
			{
				fprintf(stderr, "first_name is too long (20 bytes allowed)\n");
				return 1;
			}
			else if (strlen(args[2]) > 25)
			{
				fprintf(stderr, "last_name is too long (25 bytes allowed)\n");
				return 1;
			}
			else
				return 0;
		  break;

		case LIST:
			if (argNum == 1)
				return 0;
			else if (argNum == 2)
				if (strlen(args[1]) != 1)
				{
				fprintf(stderr, "start is too long (1 bytes allowed)\n");
				return 1;
				}
				else 
					return 0;
			else if (argNum == 3)
				if (strlen(args[1]) != 1)
				{
				fprintf(stderr, "start is too long (1 bytes allowed)\n");
				return 1;
				}
				else if (strlen(args[2]) != 1)
				{
				fprintf(stderr, "finish is too long (1 bytes allowed)\n");
				return 1;
				}
				else
					return 0;
			else
			{
				fprintf(stderr, "Usage: %s <start> <finish (optional)>\n", args[0]);
				return 1;
			}
		  break;

		case LOCATE:
			if (argNum != 2)
			{
				fprintf(stderr, "Usage: %s <location>\n", args[0]);
				return 1;
			}
			else if (strlen(args[1]) > 30)
			{
				fprintf(stderr, "location is too long (30 bytes allowed)\n");
				return 1;
			}
			else
				return 0;
		  break;

		case QUIT:
			if (argNum == 1)
				return 0;
			else if (argNum == 2)
				if (!strcasecmp(args[1], "EOF"))
					return 0;
				else
				{
					fprintf(stderr, "Second argument is not end-of-file\n");
					return 1;
				}
			else
			{
				fprintf(stderr, "Usage: %s <end-of-file (optional)>\n", args[0]);
				return 1;
			}
		  break;
	}
	return 1;
}

/* Send operation for both client and server */
void ClntServSend(void *sendBuf, int len, int sock)
{
	if (send(sock, sendBuf, len, 0) != len)
		DieWithError("send() failed");
}

/* Receive operation for both client and server */
int ClntServRecv(void *recvBuf, int len, int sock)
{	
	int n, bytesRcvd = 0;

	while (bytesRcvd < len)
	{
		if ((n = recv(sock, recvBuf + bytesRcvd, len - bytesRcvd, 0)) <= 0)
			DieWithError("recv() failed");
		bytesRcvd += n;
	}

	return bytesRcvd;
}

/* Make a new body structure */
BodyRecord *MakeBody(char *args[])
{
	BodyRecord *newBody = (BodyRecord *)malloc(sizeof(BodyRecord));

	strncpy(newBody->id_number, args[1], 10);
	strncpy(newBody->first_name, args[2], 21);
	strncpy(newBody->last_name, args[3], 26);
	strncpy(newBody->gender, args[4], 2);
	strncpy(newBody->location, args[5], 31);

	return newBody;
}

/* Make a new database structure */
Database *MakeDatabase(BodyRecord *newBody)
{
	Database *newDB = (Database *)malloc(sizeof(Database));
	newDB->body = (BodyRecord *)malloc(sizeof(BodyRecord));
	newDB->next_body = (Database *)malloc(sizeof(Database));

	newDB->body = newBody;
	newDB->next_body = NULL;

	return newDB;
}

/* Count the number of entries in the database */
int NumOfBodies(Database *db)
{
	if (db == NULL)
		return 0;

	int count = 0;
	Database *head;

	for (head = db; head != NULL; head = head->next_body)
		count++;
	
	return count;
}

/* Check whether the body exists */
Database *CheckBodyExist(char *idNumber, Database *db)
{
	Database *head;

	if (db == NULL)
		return NULL;

	for (head = db; head != NULL; head = head->next_body)
	{	
		if (!strncmp(head->body->id_number, idNumber, 10))
			return head;
	}
	return NULL;
}

/* Add an entry to the database */
Database *AddBody(BodyRecord *newBody, Database *db)
{
	Database *newDB = MakeDatabase(newBody);

	if (db == NULL)
	{
		db = newDB;
		return db;
	}
	else 
	{
		newDB->next_body = db;
		SortDatabase(newDB);
		return newDB;
	}
}

/* update an entry in the database */
Database *UpdateBody(char *args[], Database *db)
{
	Database *needUpdate = CheckBodyExist(args[1], db);
	if (needUpdate != NULL)
	{
		strncpy(needUpdate->body->first_name, args[2], 20);
		strncpy(needUpdate->body->last_name, args[3], 25);
		strncpy(needUpdate->body->gender, args[4], 1);
		strncpy(needUpdate->body->location, args[5], 30);
		SortDatabase(db);
		return db;
	}
	else
		return db;
}

/* Remove an entry from the database */
Database *RemoveBody(char *idNumber, Database *db)
{
	Database *needRemove = CheckBodyExist(idNumber, db);
	Database *head;
	BodyRecord *bodyRemoved;

	if (needRemove != NULL)
	{
		if (db == needRemove)
		{
			bodyRemoved = db->body;
			db = needRemove->next_body;
		}
		else
		{
			for (head = db; head != NULL; head = head->next_body)
			{	
				if (head->next_body == needRemove)
				{
					bodyRemoved = needRemove->body;
					head->next_body = needRemove->next_body;
					needRemove = NULL;
					break;
				}
			}
		}
	}

	return db;
}

/* Find the entries with the specified name in the database */
Database *FindBody(char *firstName, char *lastName, Database *db)
{
	Database *head;
	Database *bodiesFound = NULL;

	if (db == NULL)
		return NULL;
	else
	{
		for (head = db; head != NULL; head = head->next_body)
		{
			if ((!strcasecmp(head->body->first_name, firstName)) && (!strcasecmp(head->body->last_name, lastName)))
				bodiesFound = AddBody(head->body, bodiesFound);
		}
		return bodiesFound;
	}
}

/* List the entries with last name starting at start characters, and finishing at finish character */
Database *ListBodies(char *start, char *finish, Database *db)
{
	Database *head;
	Database *bodiesFound = NULL;

	if (db == NULL)
		return NULL;


	else
	{
		for (head = db; head != NULL; head = head->next_body)
		{
			if (!!strcasecmp(head->body->last_name, "UNK"))
			{
				if (finish != NULL)
				{
					if ((toupper(head->body->last_name[0]) >= toupper(start[0])) && (toupper(head->body->last_name[0]) <= toupper(finish[0])))
						bodiesFound = AddBody(head->body, bodiesFound);
				}
				else if (finish == NULL)
				{
					if(toupper(head->body->last_name[0]) >= toupper(start[0]))
						bodiesFound = AddBody(head->body, bodiesFound);
				}
			}
		}
		return bodiesFound;
	}
}

/* List the entries with the specifies location */
Database *LocateBodies(char *location, Database *db)
{
	Database *head;
	Database *bodiesFound = NULL;

	if (db == NULL)
		return NULL;
	else
	{
		for (head = db; head != NULL; head = head->next_body)
		{
			if (!strcasecmp(head->body->location, location))
				bodiesFound = AddBody(head->body, bodiesFound);
		}
		return bodiesFound;
	}
}

/* Send a body in one string (stream) */
void SendBody(BodyRecord *body, char *sendBuf, int sock)
{
	char idNumber[11], firstName[22], lastName[27], gender[3], location[32];

	Encap(idNumber, body->id_number, 9);
	Encap(firstName, body->first_name, 20);
	Encap(lastName, body->last_name, 25);
	Encap(gender, body->gender, 1);
	Encap(location, body->location, 30);

	strncat(sendBuf, idNumber, 10);
	strncat(sendBuf, firstName, 21);
	strncat(sendBuf, lastName, 26);
	strncat(sendBuf, gender, 2);
	strncat(sendBuf, location, 31);

	ClntServSend(sendBuf, MAXBUFSIZE, sock);
}

/* Appending a space character at the end of the string */
char *Encap(char *dest, char *src, int n)
{
	int len, i;
	bzero(dest, n+2);
	strncpy(dest, src, n);
	strcat(dest, " ");
	return dest;
}

/* Print the body information to file */
void PrintBodyRecord(BodyRecord *body, FILE *fp)
{
	if (body == NULL)
		exit(1);

	fprintf(fp, "ID Number:  %s\n", body->id_number);
	fprintf(fp, "First Name: %s\n", body->first_name);
	fprintf(fp, "Last Name:  %s\n", body->last_name);
	fprintf(fp, "Gender:     %s\n", body->gender);
	fprintf(fp, "Location:   %s\n", body->location);
}

/* Sort the database according to the last names of entries */
void SortDatabase(Database *db)
{
    int swapped;
    Database *ptr1;
    Database *lptr = NULL;
    BodyRecord *temp;
 
    if (ptr1 == NULL)
        return;
 
    do
    {
        swapped = 0;
        ptr1 = db;
 
        while (ptr1->next_body != lptr)
        {
            if (strcasecmp(ptr1->body->last_name, ptr1->next_body->body->last_name) > 0)
            { 
            	temp = ptr1->body;
                ptr1->body = ptr1->next_body->body;
    			ptr1->next_body->body = temp;
                swapped = 1;
            }
            ptr1 = ptr1->next_body;
        }
        lptr = ptr1;
    }
    while (swapped);
}

/* Free the memory of database */
void FreeDatabase(Database *db)
{
	Database *head, *cur, *next;
	BodyRecord *ent;
	head = db;
	if (db->next_body == NULL)
	{
		db->body = NULL;
		free(db->body);
		db = NULL;
		free(db);
	}
	else 
	{
		for (next = db->next_body; next != NULL; next = next->next_body)
		{
			cur = head;
			ent = head->body;
			head = RemoveBody(head->body->id_number, db);
			ent = NULL;
			free(ent);
			cur = NULL;
			free(cur);
			head = next;
		}
	}
}

