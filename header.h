/* Kewen Gu, 09-11-2014 */
/* This file contains all definition and function prototypes for helper.c, MEClient.c and MEServer.c */

#define WELLKNOWNPORT "4074"
#define MAXBODYENT 100
#define MAXBUFSIZE 96
#define MAXSTRLEN 26
#define MAXARGS 7
#define MAXPENDING 1
#define RCVBUFSIZE 86

#define LOGIN 1
#define ADD 2
#define UPDATE 3
#define REMOVE 4
#define FIND 5
#define LIST 6
#define LOCATE 7
#define QUIT 8

typedef struct body_record
{
	char id_number[10];
	char first_name[21];
	char last_name[26];
	char gender[2];
	char location[31];
} BodyRecord;

typedef struct body_node
{
	BodyRecord *body;
	struct body_node *next_body;
} Database;

void DieWithError(char *errorMessage);  /* Error handling function */
int StrSlicing(char *buf, char *args[], char *s);
int CmdNumbering(char *arg);
int StrCheck(int argNum, char *args[]);
void ClntServSend(void *sendBuf, int len, int sock);
int ClntServRecv(void *recvBuf, int len, int sock);

BodyRecord *MakeBody(char *args[]);
Database *MakeDatabase(BodyRecord *newBody);
int NumOfBodies(Database *db);
Database *CheckBodyExist(char *idNumber, Database *db);
void SortDatabase(Database* db);
void SendBody(BodyRecord *body, char *sendBuf, int sock);
char *Encap(char *dest, char *src, int n);
void PrintBodyRecord(BodyRecord *body, FILE *fp);
void FreeDatabase(Database *db);

Database *AddBody(BodyRecord *newBody, Database *db);
Database *UpdateBody(char *args[], Database *db);
Database *RemoveBody(char *idNumber, Database *db);
Database *FindBody(char *firstName, char *lastName, Database *db);
Database *ListBodies(char *start, char *finish, Database *db);
Database *LocateBodies(char *location, Database *db);


