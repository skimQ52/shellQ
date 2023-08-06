#include "shellQ.h"

int main() {
    
    size_t length = 0;
    ssize_t nread = 0;
    char *str = malloc(sizeof(char)*MAX_CHARS);
    char *str2;
    char *str3 = malloc(sizeof(char)*MAX_CHARS);
    char **parsed;
    char **parsed2;
    char *path;
    char *cmmd;
    char *cmmd2;
    char *outputFile;
    char *inputFile;
    char cwd[MAX_CHARS];
    int back;
    int pipe;
    int status;
    FILE* fp;
    FILE *fpw;
    FILE *fpr;
    pid_t kidpid[200];
    int kids = 0;
    int i;
    int numPaths = 0;
    char *stat; //for status of fgets during profile read

    printf("\n[Now Running shellQ]\n\n");

    //INITIALIZE ENVIRONMENT VARIABLES!

    char **myPATH = malloc(sizeof(char *)*MAX_ARGS);// malloc enough space for all parameters
    char *myHISTFILE = malloc(sizeof(char)*MAX_CHARS);
    char *myHOME = malloc(sizeof(char)*MAX_CHARS);

    char *profile = malloc(sizeof(char)*MAX_CHARS);

    //myPATH = getenv("PATH");
    myPATH[0] = malloc(sizeof(char)*5);//malloc enough for /bin
    strcpy(myPATH[0], "/bin");
    numPaths++;

    strcpy(myHISTFILE, ".shellQ_history");
    myHOME = getenv("HOME");//get home directory

    strcpy(profile, myHOME);
    strcat(profile, "/.shellQ_profile");//get path to profile

    fp = fopen(profile, "r");
    do {
        stat = fgets(str, MAX_CHARS, fp);//get line
        strtok(str, "\n");//remove newline

        parsed = parseBySpaces(str, &cmmd);//parse String by spaces to get the parameters for executing and the command in cmmd

        if (strcmp(cmmd, "export") == 0) {
            execExport(parsed, myPATH, &numPaths, myHISTFILE, myHOME);
        }
        else {
           execStandard(myPATH, numPaths, parsed, inputFile, outputFile, back);
        }
        zombieCheck();
    } while (stat);

    fclose(fp);

    getcwd(cwd, sizeof(cwd));//get first line
    printf("%s> ", cwd);//prompt
    nread = getline(&str, &length, stdin);//length is the size of the array

    while (1) {
        back = 0;//defaults to Not being a background process
        pipe = 0;//defaults to not being a piping command
        if (strlen(str) > 1) {//not empty

            strcpy(str3, str);
            strtok(str,"\n");//remove newline if there is
            
            if (strcmp(str, "exit") == 0) {
                zombieCheck();
                kill(0, SIGKILL);//kill all children
                exit(0);
            }

            if (checkForChar(str, '&') >= 0) {//check for background process
                str[strlen(str)-1] = '\0'; //remove & from end of char
                back = 1;//is a background process
            }
        
            if (checkForChar(str, '|') >= 0) {
                str2 = splitForPipe(str);
                pipe = 1;//is a piping command
            }

            //both piped and non piped must execute:
            outputFile = isolateFilename(str, checkForChar(str, '>'));//Is there an output redirection
            if (outputFile != NULL) {//if there is an output redirection
                str[checkForChar(str, '>')] = '\0';//trim the string
            }

            inputFile = isolateFilename(str, checkForChar(str, '<'));
            if (inputFile != NULL) {//if there is an input redirection
                str[checkForChar(str, '<')] = '\0';//trim the string
            }

            zombieCheck();
            parsed = parseBySpaces(str, &cmmd);//parse String by spaces to get the parameters for executing and the command in cmmd
            //path = formPath(myPATH, cmmd);
            if (pipe) {//only pipe needs to execute

                //can use the same variables since there can only be one of each
                outputFile = isolateFilename(str2, checkForChar(str2, '>'));//Is the an output redirection
                if (outputFile != NULL) {//if there is an output redirection
                    str2[checkForChar(str2, '>')] = '\0';//trim the string
                }

                inputFile = isolateFilename(str2, checkForChar(str2, '<'));
                if (inputFile != NULL) {//if there is an input redirection
                    str2[checkForChar(str2, '<')] = '\0';//trim the string
                }
                
                parsed2 = parseBySpaces(str2, &cmmd2);// parse other string by spaces 

                execPiped(myPATH, numPaths, parsed, parsed2, inputFile, outputFile, back);
            }

            else {
                if (strcmp(cmmd, "cd") == 0) {
                    execCD(parsed);
                }
                else if (strcmp(cmmd, "history") == 0) {
                    execHistory(parsed, myHISTFILE);
                }
                else if (strcmp(cmmd, "export") == 0) {
                    execExport(parsed, myPATH, &numPaths, myHISTFILE, myHOME);
                }
                else {
                    execStandard(myPATH, numPaths, parsed, inputFile, outputFile, back);
                }
            }

            zombieCheck();

            writeToHistory(str3, myHISTFILE);
        }
        
        //continue loop
        getcwd(cwd, sizeof(cwd));
        printf("%s> ", cwd);//print prompt with current working directory
        nread = getline(&str, &length, stdin);//get next line

        zombieCheck();
    }

    exit(0);

    return 0;
}