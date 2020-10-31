# My Shell
This shell comprises of 8 C files and 9 header files along with 1 text file 
and 1 makefile.
On terminal, it is compiled by the command 'make' and then run with the executable 
'./shell'.
```
$ make
$ ./shell
```

The shell program consists of the following components divided into separate files
associated with their respective headers:

## main
This file contains the primary shell loop which executes on running
the program. It can handle both redirection to and from files, and piping
between different processes. 

## func
This file contains various functionalities that are used in 
other sections of the program.

* numval:   used to convert a numerical string to corresponding integer value.
* splitargs:    used to split a line of characters into words/tokens as arguments.
* max/min:      returns max/min of 2 numbers
* syntaxchk:    checks if the input command by the user is valid or not; 
                only grammar for piping is checked
        
## inbuilt
This file contains functions to perform following shell operations-

* cd:       for changing current working directory.
* echo:     it displays whatever input is provided to it.
* pwd:      to display the present working directory.
* envset:   set value of an environment variable
* envunset: unset value of an environment variable

## prompt
This file handles the part which is displayed at every instance of entering
input into the shell.

* curdir:   it is used to get the current directory to be displayed
            in the prompt.
* prompt:   display prompt and get input from user

## ls
This file contains the functions for 'ls' shell command.

* printlist:        it iterates over the list of directories required to be listed.
* printentity:      it iterates over the contents of each directory to be listed.

## pinfo
This file contains the function for displaying the info about any 
process via pid.

## proc
this file contains a number of functions that deal with processes of the shell.

* procchk: ensures that a message gets displayed whenever a background process
         finishes; executed when parent process recieves SIGCHLD signal.
* jobs: lists the background processes that the shell has spawned;
        each with a different jobid
* kjob: to send a signal to some job in above mentioned list.

* overkill: kills all background processes of shell

* fg: brings some background process to foreground

* bg: changes state of some background process from stopped to running

* procadd: add some process to list of background processes


## history
This file contains functions for handling the history of
the commands entered into the shell.

* addhistory:       to add the entered input into the history log.
* printhistory:     used to print the lines stored in the log via `$ history`

The log is stored in the text file **_history.txt_**.