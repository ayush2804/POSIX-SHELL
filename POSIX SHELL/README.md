# Assignment 2

## Overview

* Created the shell display, with some basic commands implementation.

***

## Commands

### Display

* Displays Username and Systemname in the form:
    ```
    username@systemname:~>
    ```
* And waits for user input, upon which the commands given by the user will be executed.


### echo, pwd, cd

* echo displays the string given by user followed by "echo" command, as it is.
* pwd displays the current directory.
* cd changes the current directory to path given by user.
    * "cd ~" will change the directory to root(~) directory.

### ls

* shows directory and files in the directory.
* flags:
    * -a : displays all the files and directories including hidden files / directories
    * -l : displays information of the files and directories, such as file permissions, username, groupname, filesize, etc.

### search

* returns true, if file / directory given by user exists & false, if the file / directory doesn't exist.

***
