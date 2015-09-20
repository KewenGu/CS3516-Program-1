/* Kewen Gu, 09-11-2014 */
/* This file is MEServer program for issuing the command send by the client, and maintaining a database consisting of body entries */

#include <stdio.h>        /* for printf() and fprintf() */
#include <sys/socket.h>   /* for socket(), bind(), and connect() */
#include <arpa/inet.h>    /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>       /* for atoi() */
#include <string.h>       /* for memset() */
#include <unistd.h>       /* for close() */

#include "header.h"

int main(int argc, char *argv[])
{
	int servSock, clntSock, portNum, recvMsgSize, argNum, entNum;
	unsigned int clntLen;
	struct sockaddr_in servAddr, clntAddr;
	char *recvBuf, *sendBuf, *nameRemoved;
	int cmdCount = 0, packNum = 0;
	char *args[MAXARGS];
	char *mode = NULL;
	Database *db = NULL;
	BodyRecord *newBody;
	Database *bodiesFound, *bodyRemoved;
	FILE *fp;
	int i, j, flag = 0;

	/* Memory allocation */
	recvBuf = (char *)calloc(1, MAXBUFSIZE);
	sendBuf = (char *)calloc(1, MAXBUFSIZE);
	/* Server port is the well known port */
	portNum = atoi(WELLKNOWNPORT);
	/* Create the socket */
	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");
	/* Configure server address */
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family      = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port        = htons(portNum);
	/* Bind to the address */
	if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)))
		DieWithError("bind() failed");
	/* Listen to the socket */
	if (listen(servSock, MAXPENDING) < 0)
		DieWithError("listen() failed");

	while(1)
	{
		clntLen = sizeof(clntAddr);
		/* Accept the client */
		if ((clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen)) < 0)
			DieWithError("accept() failed");
		printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));
		
		/* Receive message from client */
		recvMsgSize = ClntServRecv(recvBuf, MAXBUFSIZE, clntSock);
		/* Send received string and receive again until end of transmission */
		while (recvMsgSize > 0)
		{
			/* Slice the received string into tokens */
			argNum = StrSlicing(recvBuf, args, " ");

			/* Switch state according to the command received */
			switch (CmdNumbering(args[0]))
			{	
				case LOGIN:
					cmdCount++;
					mode = strdup(args[1]);
					/* Construct the hello message */
					strcpy(sendBuf, "Hello ");
					strcat(sendBuf, args[1]);
					strcat(sendBuf, "!");
					/* Send hello message to client */
					ClntServSend(sendBuf, MAXBUFSIZE, clntSock);
					packNum++;
				  break;

				case ADD:
					cmdCount++;
					if (NumOfBodies(db) < MAXBODYENT)
					{
						/* Check whether the entry is already exists */
						if (CheckBodyExist(args[1], db) == NULL)
						{
							/* Make a new body */
							newBody = MakeBody(args);
							/* Add the new body to the database */
							db = AddBody(newBody, db);
							/* Send succeed message to the client */
							strcpy(sendBuf, "Adding successful");
							ClntServSend(sendBuf, MAXBUFSIZE, clntSock);
							packNum++;
//							printf("Adding successful\n");
						}
						else
						{
							/* Send error message to client */
							strcpy(sendBuf, "Entry already exists");
							ClntServSend(sendBuf, MAXBUFSIZE, clntSock);
							packNum++;
//							printf("Entry already exists\n");
						}
					}
					else
					{
						/* Send error message to client */
						strcpy(sendBuf, "Cannot add any more entries");
						ClntServSend(sendBuf, MAXBUFSIZE, clntSock);
						packNum++;
//						printf("Cannot add any more entries (100 entries allowed)\n");
					}
				  break;

				case UPDATE:
					cmdCount++;
					/* Check whether the entry is already exists */
					if (CheckBodyExist(args[1], db)!= NULL)
					{
						/* Update the specified bidy */
						db = UpdateBody(args, db);
						/* Send succeed message to client */
						strcpy(sendBuf, "Updating successful");
						ClntServSend(sendBuf, MAXBUFSIZE, clntSock);
						packNum++;
//						printf("Updating successful\n");
					}
					else
					{
						/* Send error message to client */
						strcpy(sendBuf, "Entry does not exist");
						ClntServSend(sendBuf, MAXBUFSIZE, clntSock);
						packNum++;
//						printf("Entry does not exist\n");
					}
				  break;

				case REMOVE:
					cmdCount++;
					if ((bodyRemoved = CheckBodyExist(args[1], db)) != NULL)
					{
						/* Check whether the entry is already exists */
						db = RemoveBody(args[1], db);
						/* Send the name removed to client */
						strcat(sendBuf, "Removed: ");
						strcat(sendBuf, bodyRemoved->body->first_name);
						strcat(sendBuf, " ");
						strcat(sendBuf, bodyRemoved->body->last_name);
						ClntServSend(sendBuf, MAXBUFSIZE, clntSock);
//						printf("%s\n", sendBuf);
					}
					else
					{
						/* Send error message to client */
						strcpy(sendBuf, "Entry does not exist");
						ClntServSend(sendBuf, MAXBUFSIZE, clntSock);
						packNum++;
//						printf("Entry does not exist\n");
					}
				  break;

				case FIND:
					cmdCount++;
					/* Find the entries with specified name */
					bodiesFound = FindBody(args[1], args[2], db);
					/* Calculate the number of bodies found */
					entNum = NumOfBodies(bodiesFound);
					/* Send the number of bodies to client */
					ClntServSend(&entNum, sizeof(int), clntSock);
					packNum++;

					if (entNum > 0)
					{
						/* Send the bodies one by one to client */
						for (i = 0; i < entNum; i++)
						{
							SendBody(bodiesFound->body, sendBuf, clntSock);
							packNum++;
							bzero(sendBuf, MAXBUFSIZE);
							bodiesFound = RemoveBody(bodiesFound->body->id_number, bodiesFound);
						}
//						printf("Found %d entry(s)\n", entNum);
					}
//					else
//						fprintf(stderr, "No entry found\n");
				  break;

				case LIST:
					cmdCount++;
					/* If no additional arguments given, list all the entries in database */
					if (argNum == 1)
						bodiesFound = db;
					else
						bodiesFound = ListBodies(args[1], args[2], db);
					entNum = NumOfBodies(bodiesFound);
					/* Send the number of bodies to client */
					ClntServSend(&entNum, sizeof(int), clntSock);
					packNum++;

					if (entNum > 0)
					{
						for (i = 0; i < entNum; i++)
						{
							/* Send the bodies one by one to client */	
							SendBody(bodiesFound->body, sendBuf, clntSock);
							packNum++;
							bzero(sendBuf, MAXBUFSIZE);
							bodiesFound = RemoveBody(bodiesFound->body->id_number, bodiesFound);
						}
//						printf("Listed %d entry(s)\n", entNum);
					}
//					else
//						fprintf(stderr, "No entry found\n");
				  break;

				case LOCATE:
					cmdCount++;
					bodiesFound = LocateBodies(args[1], db);
					entNum = NumOfBodies(bodiesFound);
					/* Send the number of bodies to client */
					ClntServSend(&entNum, sizeof(int), clntSock);
					packNum++;

					if (entNum > 0)
					{
						for (i = 0; i < entNum; i++)
						{
							/* Send the bodies one by one to client */	
							SendBody(bodiesFound->body, sendBuf, clntSock);
							packNum++;
							bzero(sendBuf, MAXBUFSIZE);
							bodiesFound = RemoveBody(bodiesFound->body->id_number, bodiesFound);
						}
//						printf("Located %d entry(s)\n", entNum);
					}
//					else
//						fprintf(stderr, "No entry found\n");
				  break;

				case QUIT:
					cmdCount++;
					/* Send a quiting indication to client */
					strcpy(sendBuf, "Closing...");
					ClntServSend(sendBuf, MAXBUFSIZE, clntSock);
					/* If EOF is not specified */
					if (args[1] == NULL)
					{
						/* Send the number of command issued by the server to client */
						ClntServSend(&cmdCount, sizeof(int), clntSock);
						packNum++;
						printf("Total number of commands issued by %s: %d\n", mode, cmdCount);
						cmdCount = 0;
						mode = NULL;
					}
					/* If EOF is specified */
					else if (!strcasecmp(args[1], "EOF"))
					{
						/* Write all the entries in database to the file MEDatabase.txt */
						if ((fp = fopen("MEDatabase.txt", "w")) != NULL)
						{
							entNum = NumOfBodies(db);
							bodiesFound = db;
							for (i = 0; i < entNum; i++)
							{
								fprintf(fp, "\n------------\n");
								fprintf(fp, "Body No.%d:\n", i+1);
								fprintf(fp, "------------\n");
								PrintBodyRecord(bodiesFound->body, fp);
								bodiesFound = RemoveBody(bodiesFound->body->id_number, bodiesFound);
							}
							/* Close the output file */
							fclose(fp);
//							printf("Done writing to file MEDatabase.txt\n");
						}
						/* Send the number of command issued by the server to client */
						ClntServSend(&cmdCount, sizeof(int), clntSock);
						printf("Total number of commands issued by %s: %d\n", mode, cmdCount);
						cmdCount = 0;

						packNum++;
						packNum++;
						/* Send the number of packets sent by the server to client */
						ClntServSend(&packNum, sizeof(int), clntSock);
						printf("Total number of packets sent to the client: %d\n", packNum);
						packNum = 0;

						mode = NULL;
						/* set a flag to indicate end of file */
						flag = 1;
					}
				  break;
			}
			/* Zero the recvBuf and sendBuf */
			bzero(recvBuf, MAXBUFSIZE);
			bzero(sendBuf, MAXBUFSIZE);
			/* If flag is set, break from the loop */
			if (flag)
				break;
			/* Receive the next command from the client */
			ClntServRecv(recvBuf, MAXBUFSIZE, clntSock);
		}
		/* Set flag back to 0 */
		flag = 0;
		/* close the sock */
		close(clntSock);
		/* free the database */
		FreeDatabase(db);
	}

	free(recvBuf);
	free(sendBuf);
}











