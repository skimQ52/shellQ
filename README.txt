
shellQ -- A functioning shell to mimic bash!

myShell.c contains the main function loop, shellQ.c contains all function definitons
necessary for the shell, and shellQ.h holds the necessary includes along with macro definitons and
function prototypes for the functions in shellQ.c

There is also a makefile which creates the executable myShell
With the profile file .CIS3110_profile in the home directory and all necessary files
present, run 'make' then run './myShell' to start the shell.

NOTE: if makefile gives access denied, please run 'chmod +x myShell' then try again
If the makefile still doesnt work just compile with "gcc -Wpedantic -std=gnu99 myShell.c shellQ.c -o myShell"
then run "./myShell"


Set 1 Functions:

	1. exit
if the user enters the command 'exit', all children processes will be killed and the program will end execution.

	2. command with no arguments
Assuming the necessary path is included into $myPATH by using export or the profile file, a command like ls will execute properly
Test: 'ls' will list contents of current directory

	3. A command with arguments
Same as above.
Test: 'ls -lt' will list contents of current directory in ordered fashion

	4. A command, with or without arguments, executed in the background using &
If a & is present as the last character of the inputted string, the executed command will execute as a
background process, allowing the user to execute other commands like ls and such while the background process is still executing.
AFTER the background process ends, no process will be leftover as can be verified using 'ps'.
Test: first execute command 'sleep 10&' which will run command for 10 seconds in the background which can be seen with 'ps'. During
this time, a user may execute other commands. Try 'ls'. After the 10 seconds is up, run 'ps' again and there will no longer be
the running sleep command.



Set 2 functions:

	5. A command, with or without arguments, whose utput is redirected TO a file.
If > is present in the entered string and is followed by a name of a file, the output of the entered command
will be redirected into this file. (A file that does not exist will be created).
Test: simply run 'ls > file.txt' and then the contents of the current directory will be listed inside of file.txt

	6. A command, with or without arguments, whose input is redircted FROM a file
IF < is present ine the entered string and is followed by a name of a file, the input for the entered command will be
taken from said file.
Test: if /usr/bin is in the path, we can run 'sort < file.txt' and we'll see contents of file.txt have been outputted sorted.

	7. A command, with or without arguments, whose output is piped to the input of another command
If | is present between 2 commands in the entered string, then the output of the first command (the command on the left of the |)
will be used for the input of the second command ( the command on the right of the |).
Test: 'ls -lt | more'

	COMBINATIONS OF SET 2:
Only certain combinations function properly.
Firstly, all set 2 functions work as a background process (that is, with & as the last char)

For input redirection and output redirection in one line, the program assumes that the file to get input from is
listed first and the program to output to is listed afterwards.
Test: "tr 'aieou' 'AIEOU' < file.txt > file2.txt" will take the contents of file.txt and set all of the vowels to be capitilized
and place this output into file2.txt.

As for combinations with pipes, the program is extremely inconsistent.



Set 3 functions:

	8. Limited shell environment variables: myPATH, myHISTFILE, myHOME
All of these variables are defined and stored and work with this shell. They all are defaulted to the proper names/contents.
They can be seen by executing "export" with no arguments similar to bash but much with much simpler output.
Commands ran need to have the proper paths. If user wants to execute ./time 7, an executable for a program that waits 7 seconds
then they must export the proper path to the list of paths.

	9. Reading in the profile file on initialization of the shell and executing any commands inside
The profile file is read upon initialization of the shell. It should be located in the home directory and named .CIS3110_profile
Each and every line will be executed from that profile file, which should usually contain additions to myPATH
It is assumed that the user will not run any piping, I/O redirection or background processes with these commands.
NOTE the given .CIS3110_profile file contains ls, ls -lt and myPATH=$myPATH:/usr/bin:/usr/games/bin to demonstrate/setup for commands like sort.

	10. Builtin functions export, history and cd.
export:
Export can be used to change the contents of any of the environment variables myHOME, myHISTFILE and MYPATH.
example: "export myHISTFILE=/usr/local/sbin/histfile.txt" will change the histfile to be that specific test file.
Same idea for myHOME

Exporting with myPATH can either replace or add on to the current list of paths, and multiple can be added in one line:
"export myPATH=/usr/bin" will set myPATH to ONLY be /usr/bin
"export myPATH=/bin:/usr/bin" will set myPATH to be both /usr/bin and /bin
"export myPATH=$myPATH:/usr/games/bin" would add /usr/games/bin to myPATH and keep everything else.

history:
running "history" will open myHISTFILE and print its contents as (space) number (2 spaces) command line
running "history n" where n is a number will print the last n lines in myHISTFILE
running "history -c" will clear myHISTFILE

cd:
running "cd .." will go back one directory
running "cd bin" will go into the directory bin if it exists


