
#include <stdio.h>       /* Input/Output */
#include <string.h>      /* String stuff */
#include <ctype.h>
#include <stdlib.h>      /* General Utilities */
#include <unistd.h>      /* Symbolic Constants */
#include <sys/types.h>   /* Primitive System Data Types */
#include <sys/wait.h>    /* Wait for Process Termination */
#include <errno.h>       /* Errors */


#define MAX_CHARS 500//maximum chars for input command
#define MAX_ARGS 20

char **parseBySpaces(char *str, char **cmmd);

int checkForChar(char *str, char c);

void execStandard(char **myPATH, int numPaths, char **parameters, char *inputFile, char *outputFile, int back);

char *isolateFilename(char *str, int index);

void zombieCheck();

char *splitForPipe(char *str);

void execPiped(char **myPATH, int numPaths, char **parameters, char **parameters2, char *inputFile, char *outputFile, int back);

int isEmpty(const char *str);

void writeToHistory(char *str, char *histFile);

void execHistory(char **parameters, char *histFile);

void execCD(char **parameters);

char *formPath(char *myPATH, char *cmmd);

void execExport(char **parameters, char **myPATH, int *numPaths, char *myHISTFILE, char *myHOME);

int addToPath(char *str, char **myPATH, int numPaths);

int replacePath(char *str, char **myPATH, int numPaths);

char *envToString(char *env, char **myPATH, int numPaths, char *myHISTFILE, char *myHOME);