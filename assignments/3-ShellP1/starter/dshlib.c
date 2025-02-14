#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (!cmd_line || !clist) return ERR_CMD_OR_ARGS_TOO_BIG;
    char *cmd_copy = strdup(cmd_line); 
    int cmd_count = 0;
    int arg_count = 0;
    int counter = 0;
    int totalCounter = 0;
    char *cmd;

    // FIRST LOOP: Count executable commands (ignore "|")
    cmd = strtok(cmd_copy, " ");
    while (cmd) {
        if (strcmp(cmd, "|") != 0) {  // If it's not a pipe, count it
            totalCounter++;
            while ((cmd = strtok(NULL, " ")) && strcmp(cmd, "|") != 0);  // Skip args until next "|"
        } else {
            cmd = strtok(NULL, " ");  // Skip "|"
        }
    }
    free(cmd_copy);  // Free duplicated string

    if (totalCounter > CMD_MAX) {
        printf("error:pipinglimitedto8commands");
    } else if (totalCounter == 0) {
        printf("warning:nocommandsprovided");
    } else {
 // SECOND LOOP: Print executable commands
    cmd = strtok(cmd_line, " ");
    while (cmd) {
        if (strcmp(cmd, "|") == 0) {  // If we encounter "|", move to next command
            if (arg_count > 1) {
                strcat(clist->commands[cmd_count].args, "]");
            }
            if (cmd_count == 0) {
                printf("PARSEDCOMMANDLINE-TOTALCOMMANDS%d", totalCounter);
            }
            printf("<%d>%s%s", clist->num, clist->commands[cmd_count].exe, clist->commands[cmd_count].args);
            cmd_count++;
            arg_count = 0;
        } else {
            if (arg_count == 0) {  // First argument is the executable
                strcpy(clist->commands[cmd_count].exe, cmd);
                clist->commands[cmd_count].args[0] = '\0';  // Properly reset args
                counter++;
                clist->num = counter;
            } else {
                if (strlen(clist->commands[cmd_count].args) + strlen(cmd) + 2 < ARG_MAX) {
                    if (arg_count == 1) {
                        strcat(clist->commands[cmd_count].args, "[");
                    }
                    strcat(clist->commands[cmd_count].args, cmd);  // Append argument
                } else {
                    printf("Error: Argument buffer overflow\n");
                }   
            }
            arg_count++; 
        }
        cmd = strtok(NULL, " ");
    }
    if (cmd_count == 0) {
        printf("PARSEDCOMMANDLINE-TOTALCOMMANDS%d", counter);
    }
    if (arg_count > 1) {
        strcat(clist->commands[cmd_count].args, "]");
    }
    printf("<%d>%s%s", clist->num, clist->commands[cmd_count].exe, clist->commands[cmd_count].args);

    }

    clist->commands[cmd_count].args[arg_count] = NULL;  // Null-terminate last command
    clist->num = cmd_count + 1;  // Final command count

    return OK;
}

