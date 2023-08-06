
#include "shellQ.h"

char **parseBySpaces(char *str, char **cmmd) {

    char **parsed = malloc(sizeof(char *)*MAX_ARGS);// malloc enough space for all parameters
    char *token;//for strtok
    int i = 1;

    token = strtok(str, " ");//first token (BY SPACES)
    *cmmd = strdup(token); //first token is the command
    parsed[0] = strdup(token);
    token = strtok(NULL, " ");//tokenize again
    while(token != NULL) {//for all tokens, until they run out
        parsed[i] = strdup(token);//duplicates and mallocs enough space
        i++;
        token = strtok(NULL, " ");//tokenize again
    }

    parsed[i] = NULL;//set last as NULL POINTER
    
    return parsed;//return the parsed args!
}

int checkForChar(char *str, char c) {

    int i;
    for (i = 0; i < strlen(str); i++) {//for all chars
        if (str[i] == c) {//character found
            return i; //return the index its found at
        }
    }
    return -1;//return -1 if not present
}

void execStandard(char **myPATH, int numPaths, char **parameters, char *inputFile, char *outputFile, int back) {
    
    FILE* fp;
    FILE* fp2;
    char str[MAX_CHARS];
    
    pid_t pid = fork();
    int status; //for child process
    char *cmmd = strdup(parameters[0]);
    int i = 0;

    if (pid >= 0) { //SUCCESSFUL FORK

        if (pid == 0) {//CHILD PROCESS

            if (inputFile != NULL) {
                fp = freopen(inputFile, "r+", stdin);
            }
            if (outputFile != NULL) {
                fp2 = freopen(outputFile, "w+", stdout);
            }

            //running command
  
            //RUN EXECVP PASSING IN parameters (where parameters[0] is the command)
            for (i = 0; i < numPaths; i++) {
                parameters[0] = formPath(myPATH[i], cmmd);//set parameters[0] to be path with command
                status = execv(parameters[0], parameters);
                if (status > 0) {
                    break;
                }
            } 
            if (status == -1) {//if status bad after trying all
                printf("-shellQ: command not found\n");
                exit(status);
            }
            

            if (inputFile != NULL) {
                fgets(str, MAX_CHARS, stdin);
                fprintf(stdout, str);
                fclose(fp);
            }
            if (outputFile != NULL) {
                fclose(fp2);
            }
            exit(0);
        } 

        else {//Parent Process
            if (!back) {
                zombieCheck();
                waitpid(pid, &status, 0);//wait for child to exit
            }
            else {//dont wait if its a background process
                zombieCheck();
            }
        }
    }
    else {//fork failed!
        perror("fork failed");
        return;
    }
}

char *isolateFilename(char *str, int index) {

    if (index < 0) {
        return NULL;
    }

    int i;
    char *filename;

    index++;//start right after the char

    while (str[index] == ' ') {//while spaces are present after the char, delay start until not spaces
        index++;
    }

    filename = malloc(sizeof(char)*MAX_ARGS);//(strlen(str)-index + 1));//malloc enough space for filename

    for (i = index; i < strlen(str); i++) {
        if (str[i] != ' ' && str[i] != '\0' && str[i] != '\n') {
            filename[i-index] = str[i]; 
        }
        else {//str[i] is a space
            break;//stop after first space.
        }
    }
    filename[i-index] = '\0';//set last char to be null terminator!
    return filename;
}

void zombieCheck() {
    int status;
    waitpid(-1, &status, WNOHANG);
}

//returns second str after pipe, and str becomes first half
char *splitForPipe(char *str) {
    char *str2;
    char *token;

    token = strtok(str, "|");//split by pipe
    token = strtok(NULL, "|");//get second one
    if (token == NULL || isEmpty(token)) {//if no second parameter
        printf(">");//just like bash does
        str2 = malloc(sizeof(char)*MAX_CHARS);
        fgets(str2, MAX_CHARS, stdin);
        strtok(str2, "\n");
    }
    else {
        str2 = strdup(token);
    }

    return str2;
}

void execPiped(char **myPATH, int numPaths, char **parameters, char **parameters2, char *inputFile, char *outputFile, int back) {

    int fd[2]; //fd[0] is read, fd[1] is write
    int pid1, pid2;
    int status, status2;
    char str[MAX_CHARS];
    FILE *fp;
    FILE *fp2;
    int i = 0;
    int j = 0;
    char *cmmd = strdup(parameters[0]);
    char *cmmd2 = strdup(parameters2[0]);

    if (pipe(fd) == -1) {
        printf("\nERROR... Could not pipe\n");
        return;
    }
    pid1 = fork();
    if (pid1 >= 0) {//successful fork
        if (pid1 == 0) {//child process!
            dup2(fd[1], STDOUT_FILENO);//duplicate write pipe so we can use it
            close(fd[0]);//close the READING pipe
            close(fd[1]);//dont need original anymore

            if (inputFile != NULL) {
                fp = freopen(inputFile, "r+", stdin);
            }

            //status = execvp(parameters[0], parameters);//status will be -1 if execvp fails otherwise execvp doesnt return.
            for (i = 0; i < numPaths; i++) {
                parameters[0] = formPath(myPATH[i], cmmd);//set parameters[0] to be path with command
                status = execv(parameters[0], parameters);
                if (status >= 0) {
                    break;
                }
            } 
            if (inputFile != NULL) {
                fgets(str, MAX_CHARS, stdin);
                fprintf(stdout, str);
                fclose(fp);
            }
            if (status == 255 || status == -1) {//if status bad
                printf("-shellQ: command not found\n");
                exit(status);
            }
            exit(0);
        }
        else {//parent process 1

            pid2 = fork();//parent then forks for a second child,

            if (pid2 >= 0) {//successful second fork
                if (pid2 == 0) {//child process 2
                    close(fd[1]);//close write pipe
                    dup2(fd[0], STDIN_FILENO);//duplicate read pipe
                    close(fd[0]);//close original

                    if (outputFile != NULL) {
                        fp2 = freopen(outputFile, "w+", stdout);
                    }

                    for (j = 0; j < numPaths; j++) {
                        parameters2[0] = formPath(myPATH[i], cmmd2);//set parameters2[0] to be path with command until one works
                        status2 = execv(parameters2[0], parameters2);
                        if (status2 >= 0) {
                            break;
                        }
                    } 
                    //status2 = execv(parameters2[0], parameters2);
                    if (status2 == -1) {//if status bad
                        kill(pid1, SIGKILL);//kill the first process
                        printf("-shellQ: command not found\n");
                        exit(status2);
                    }

                    if (outputFile != NULL) {
                        fclose(fp2);
                    }
                    exit(0);
                }
                else {//parent process 2
                close(fd[0]);//need to close in orignal process!
                close(fd[1]);//safest to always just close both even though technically only need to close write
                    if (!back) {
                        zombieCheck();
                        waitpid(pid1, &status, 0);//wait for child1 to exit
                        waitpid(pid2, &status2, 0);//wait for child2 to exit
                    }
                    else {//if not background process dont wait
                        zombieCheck();
                    }
                }
            }
            else {//fork 2 no work
                printf("Error forking\n");
                return;
            }
        }
    } 
    else {//fork 1 no work
        printf("Error forking\n");
        return;
    }
    
    close(fd[0]);//need to close in orignal process!
    close(fd[1]);//safest to always just close both even though technically only need to close write
    return;
}

int isEmpty(const char *str) {

    int i = 0;
    while (str[i] != '\0') {
        if (!isspace((unsigned char)str[i])){
            return 0;//not empty
        }
        i++;
    }
    return 1;//if no non whitespaces found
}

void writeToHistory(char *str, char* histFile) {

    FILE *fp;
    char *temp = malloc(sizeof(char)*MAX_CHARS);
    int x;
    char c;

    fp = fopen(histFile, "a+");//open in appending mode with READing also

    if (fp == NULL || isEmpty(str)) {
        perror("Failed to open\n");
        return;
    }

    c = fgetc(fp);//get first char

    if (c == EOF) {//file is empty
        fseek(fp, 0, SEEK_SET);//set back to start
        fprintf(fp, " 1  %s", str);//print the first command entered!
    }

    else {//file is not empty, must append
        fseek(fp, 0, SEEK_SET);//set back to start
        while (!feof(fp)) {//until EOF, after will have the last line
            fgets(temp, MAX_CHARS, fp);//get all lines
        }
        strtok(temp, " ");//get first token (NUMBER)
        x = atoi(temp);
        x++; //increment by 1
        fprintf(fp, " %d  %s", x, str);
    }
    
    fclose(fp);//close
    return;
}

void execHistory(char **parameters, char *histFile) {

    FILE *fp;
    char *temp = malloc(sizeof(char)*MAX_CHARS);
    char *str;
    int total = 0;
    int n;
    char c;
    char *stat;
    
    if (parameters[1] != NULL && !isEmpty(parameters[1])) {//if theres a second param
        if (strcmp(parameters[1], "-c") == 0) { //need to clear history file
            fp = fopen(histFile, "w");//all we have to do is open the file in read mode and it will overwrite it!
            if (fp == NULL) {
                perror("Failed to open\n");
                return;
            }
            fclose(fp);
            return;
        }

        else if (isdigit(parameters[1][0]) > 0) {//if there is an n
            fp = fopen(histFile, "r");
            if (fp == NULL) {
                perror("Failed to open\n");
                return;
            }
            fseek(fp, 0, SEEK_SET);//set back to start
            n = atoi(parameters[1]);//get the int value
            while (!feof(fp)) {//until EOF to figure out total lines
                fgets(temp, MAX_CHARS, fp);//get all lines
                total++;
            }
            fseek(fp, 0, SEEK_SET);//set back to start
            n = total - n;
            total = 0;
            do {//until EOF again
                stat = fgets(temp, MAX_CHARS, fp);//get all lines
                total++;
                if (total >= n) {
                    str = strdup(temp);
                    printf("%s", str);//print if within last n
                    free(str);
                }

                //check if at end
                c = fgetc(fp);
                if (c == EOF) {
                    break;
                }
                fseek(fp, -1, SEEK_CUR);//gotta move it back from the check
            } while (stat);
            fclose(fp);
            return;
        }

        else {
            printf("shellQ: history: %s: numeric argument required\n", parameters[1]);
            return;
        }
    }
    

    fp = fopen(histFile, "r");
    fseek(fp, 0, SEEK_SET);//set back to start
    do {
        stat = fgets(temp, MAX_CHARS, fp);//get all lines
        str = strdup(temp);
        printf("%s", str);//print
        free(str);
        c = fgetc(fp);
        if (c == EOF) {
            break;
        }
        fseek(fp, -1, SEEK_CUR);//gotta move it back from the check
    } while (stat);

    fclose(fp);
    return;
}

void execCD(char **parameters) {//function to change directory

    int c;
    char cwd[256];

    if (parameters[1] == NULL || isEmpty(parameters[1])) {//cd with no parameters will just do nothing
        return;
    }

    c = chdir(parameters[1]);
    if (c < 0) {//fail
        printf("shellQ: cd: %s: No such file or directory\n", parameters[1]);
    }
    else {
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
    }
    return;
}

char *formPath(char *myPATH, char *cmmd) {//function to form path with cmmd

    char *path = calloc(1, (sizeof(char)*(strlen(myPATH) + strlen(cmmd)+1)));
    strcpy(path, myPATH);
    strcat(path,"/");
    strcat(path, cmmd);
    path[strlen(path)] = '\0';
    return path;
}

void execExport(char **parameters, char **myPATH, int *numPaths, char *myHISTFILE, char *myHOME) {

    if (parameters[1] == NULL || isEmpty(parameters[1])) {//export with empty params prints envs
        printf("myHOME: %s\nmyHISTFILE: %s\nmyPATH: %s\n\n", myHOME, myHISTFILE, envToString("$myPATH", myPATH, *numPaths, myHISTFILE, myHOME));
        return;
    }
    char *env;
    char *rest;
    char *str = strdup(parameters[1]);
    env = strtok(str, "=");//get what type of environment variable it is
    rest = strtok(NULL, "=");

    if (strcmp(env, "myHISTFILE") == 0) {
        strcpy(myHISTFILE, rest);
    }
    else if (strcmp(env, "myHOME") == 0) {
        strcpy(myHOME, rest);
    }
    else if (strcmp(env, "myPATH") == 0) {
        if (strstr(rest, "$myPATH")) {
            *numPaths = addToPath(rest, myPATH, *numPaths);
        }
        else {//replacing
            *numPaths = replacePath(rest, myPATH, *numPaths);
        }
        
    }
    else {
        return;
    }
    return;
}

int addToPath(char *str, char **myPATH, int numPaths) {

    char *token;
    int i = numPaths;
    token = strtok(str, ":");//first token (BY COLON)

    while(token != NULL) {//for all tokens, until they run out
        if (token[0] != '$') {//not an env var
            myPATH[i] = strdup(token);//duplicates and mallocs enough space
            i++;
        }

        token = strtok(NULL, ":");//tokenize again 
    }
    return i;//return new numPaths
}

int replacePath(char *str, char **myPATH, int numPaths) {

    
    char *token;
    int i;

    for (i = 0; i < numPaths; i++) {//remove all old paths
        myPATH[i][0] = '\0';//empty string
        free(myPATH[i]);//free string
    }

    i = 0;
    token = strtok(str, ":");//first token (BY COLON)

    while(token != NULL) {//for all tokens, until they run out

        if (token[0] != '$') {//not an env var
            myPATH[i] = strdup(token);//duplicates and mallocs enough space
            i++;
        }

        token = strtok(NULL, ":");//tokenize again 
    }
    return i;//return numPaths
}

char *envToString(char *env, char **myPATH, int numPaths, char *myHISTFILE, char *myHOME) {
    char *str = malloc(sizeof(char)*MAX_CHARS);
    if (strcmp(env, "$myHISTFILE") == 0) {//simply return whats in histfile
        strcpy(str, myHISTFILE);
    }
    else if (strcmp(env, "$myHOME") == 0) {//simply return home var
        strcpy(str, myHOME);
    }
    else if (strcmp(env, "$myPATH") == 0) {//go through array and concat all together delimited by colons.
        strcpy(str, myPATH[0]);//start with first path there will always be atleast /bin
        for (int i = 1; i < numPaths; i++) {
            strcat(str, ":");
            strcat(str, myPATH[i]);
        }
        str[strlen(str)] = '\0';//set last char to be null term
    }
    else {
        printf("ENVIRONMENT VARIABLE '%s' COULD NOT BE FOUND\n", env);//if not one of the options
        return NULL;
    }
    return str;//return created string.
}

