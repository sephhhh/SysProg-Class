#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
void parse_command(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    char *ptr = cmd_buff->_cmd_buffer;
    int in_quotes = 0;

    while (*ptr) {
        while (*ptr == ' ' && !in_quotes) ptr++;

        if (*ptr == '\0') break; 

        if (*ptr == '"') {
            in_quotes = !in_quotes;
            ptr++;
        }

        cmd_buff->argv[cmd_buff->argc++] = ptr;

        while (*ptr) {
            if (*ptr == '"' && in_quotes) {
                in_quotes = 0;
                *ptr = '\0'; 
                ptr++;
                break;
            }
            if (!in_quotes && *ptr == ' ') {
                *ptr = '\0';
                ptr++;
                break;
            }
            ptr++;
        }
        
        if (cmd_buff->argc >= ARG_MAX - 1) break;
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;
}

int exec_local_cmd_loop()
{
    cmd_buff_t cmd_buff;
    int rc = OK;

    cmd_buff._cmd_buffer = (char *)malloc(SH_CMD_MAX);
    if (cmd_buff._cmd_buffer == NULL) {
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff._cmd_buffer, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove the trailing \n from cmd_buff
        cmd_buff._cmd_buffer[strcspn(cmd_buff._cmd_buffer, "\n")] = '\0';

        // Check for empty input
        if (strlen(cmd_buff._cmd_buffer) == 0) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }

        // Check for exit command
        if (strcmp(cmd_buff._cmd_buffer, EXIT_CMD) == 0) {
            break;
        }

        parse_command(&cmd_buff);

if (strcmp(cmd_buff.argv[0], "cd") == 0) {
    if (cmd_buff.argc < 2) {
        chdir(getenv("tmp"));
    } else {
        if (chdir(cmd_buff.argv[1]) != 0) {
            perror("chdir failed");
        }
    }
    continue;
}

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            rc = -1;
            break;
        } else if (pid == 0) {
            if (execvp(cmd_buff.argv[0], cmd_buff.argv) == -1) {
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    
    }
    free(cmd_buff._cmd_buffer);

    return rc;
}