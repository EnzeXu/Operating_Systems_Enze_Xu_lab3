CSC-641 - Lab 2 - EnzeXu
===========================
This document is used to show the detailed description of each part in lab2.

See at `https://github.com/EnzeXu/Operating_Systems_Enze_Xu_lab2` or
```shell
$ git clone https://github.com/EnzeXu/Operating_Systems_Enze_Xu_lab2.git
```

****
 
| Project Name | Operating Systems - Lab2 |
| ---- | ---- |
| Author | Enze Xu |
| Due | 09/30/2021 |

****
# Catalog

* [1 Purpose of Lab](#1-purpose-of-lab)
* [2 Compile & Execution Instructions](#2-compile--execution-instructions)
* [3 Questions](#3-questions)

****

# 1 Purpose of Lab
Simple Shell

The Linux shell is a command interpreter.  It allows users to execute commands without knowledge of the internals of the system. A shell can execute commands directly from the terminal input window or they can read shell commands from a file. These files are known as shell script files.There are many shells available to the user. Some of the most common ones are: sh, bash and csh.  They very much perform the same functions and differ basically in the way the can be programmed through script files. This project concentrates on command line input to the shell.Commands from the terminal window are read by the shell and executed on behalf of the user. To execute a command the shell creates a process to execute the code of the particular command.  For example to execute the ls command, the shell will create a process to execute the code for ls.  The shell will wait for the command to terminate and will prompt the user for another command. Output for the command got to the standard output file. The shell uses three predefined files for input, output and error. The stdin, stdout and stderr, for input, output end error messages. By default, all files are associated with the terminal window.

Description

To execute a command the shell requires three systems calls, fork, exec and wait. The fork system call is used to create a process. exec() is used to overwrite the code of the child process with the code of the command to be executed, and the wait() is used to wait for the child process to terminate. This way the child process does not have to execute the same code as the parent process. The exec() has many variations, execl, execv ,execle, execve, execlp and execvp. The difference is in the way the arguments are sent.When exec() is executed, the program file given by the first argument will be loaded into the caller's address space and over-write the program there. Then, the arguments will be provided to the program and execution starts. Once the specified program file starts its execution, the original program in the caller's address space is gone and is replaced by the new program. exec() returns a negative value if the execution fails. The most common failure is that the file could not be found.

Program Guidlines

The purpose of this programming project is to write a simple shell program. Your shell program should use the same style as the Bourne shell for running programs.  In particular, when the user types a line such as:command  [identifier[identifier]]Your shell should parse the command line to build  the argv array. You should already have this section of the program, from project 1.   Your program will fork a process. The child process will execute the  the execvp() system call to execute the file that contains the command. This way the exec() will search the PATH directory paths automatically.The shell must also interpret the   “&”  operator as a command terminator.  A command terminated with “&” should be executed concurrently with the shell rather than the shell waiting for the command to terminate before it prompts the user for another command (background execution).If a command is given that execvp() cannot execute (such as an erroneous command), an appropriate error message must be outputted and the shell should be re-prompted. 
The shell is in a loop, prompting the user for input and executing the command. The exit()  command is used to exit the shellYour program should not terminate with a  ^C signal.

****

# 2 Compile & Execution Instructions
```shell
$ cd xxx/Operating_Systems_Enze_Xu_lab2
$ cc lab2.c -o lab2
$ ./lab2
$ [Enjoy your time!]
```

****

# 3 Questions
> **1) How can the shell identify if the command was executed successfully or not?**

1) For child process: by whether the code in child process after `execvp()` can be executed. Because if `execvp()` was executed successfully, the code in child process after `execvp()` will just be skipped without execution.
2) For parent process: by the return value of `waitpid(pid, &status, 0);` executed by the parent process. If it returns the pid of child process, the command was executed successfully.

> **2) Under what circumstances will the exec() return to the calling process?**

The calling process of `execvp()` is the child process, and iff errors occur in the `execvp()`(in other word, the command was not executed successfully) it will return to the child process.

