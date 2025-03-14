#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

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
void parse_input(cmd_buff_t *cmd_buff) {
    char *ptr = cmd_buff->_cmd_buffer;
    cmd_buff->argc = 0;
    int cmd_count = 0;

    while (*ptr) {
        while (*ptr == ' ') ptr++;  
        if (*ptr == '\0') break;

        if (*ptr == '|') {
            cmd_count++;
            ptr++;
            continue;
        }

        if (*ptr == '"') {
            ptr++;
            cmd_buff->argv[cmd_buff->argc++] = ptr;

            while (*ptr && *ptr != '"') ptr++;

            if (*ptr == '"') {
                *ptr = '\0';
                ptr++;
            }
        } else {
            cmd_buff->argv[cmd_buff->argc++] = ptr;

            while (*ptr && *ptr != ' ' && *ptr != '|') ptr++;

            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
        }
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;
    cmd_buff->argc = cmd_count + 1;
}


int exec_local_cmd_loop() {
    command_list_t cmd_list;
    cmd_buff_t cmd_buff;
    int rc = OK;

    cmd_buff._cmd_buffer = (char *)malloc(SH_CMD_MAX);
    if (cmd_buff._cmd_buffer == NULL) {
        perror("Failed to allocate memory for command buffer");
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff._cmd_buffer, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff._cmd_buffer[strcspn(cmd_buff._cmd_buffer, "\n")] = '\0';

        if (strlen(cmd_buff._cmd_buffer) == 0) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }

        if (strcmp(cmd_buff._cmd_buffer, EXIT_CMD) == 0) {
            break;
        }

        if (strncmp(cmd_buff._cmd_buffer, "cd ", 3) == 0) {
            char *dir = cmd_buff._cmd_buffer + 3;

            if (chdir(dir) != 0) {
                perror("cd failed");
            } else {
                printf("cd successful\n");
            }
            continue; 
        }


        if (build_cmd_list(cmd_buff._cmd_buffer, &cmd_list) != OK) {
            fprintf(stderr, "Failed to build command list\n");
            continue;
        }

        if (execute_pipeline(&cmd_list) != OK) {
            continue;
        }
    }

    free(cmd_buff._cmd_buffer);
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (!cmd_line || !clist) return ERR_CMD_OR_ARGS_TOO_BIG;

    char *cmd_copy = strdup(cmd_line);
    if (cmd_copy == NULL) {
        return ERR_MEMORY;
    }

    char *cmd;
    char *temp = NULL;

    cmd = strtok(cmd_copy, " ");
    while (cmd) {
        if (strcmp(cmd, "|") == 0) {
            if (temp) {
                cmd_buff_t *current_cmd_buff = &(clist->commands[clist->num]);
                current_cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);

                if (current_cmd_buff->_cmd_buffer == NULL) {
                    free(cmd_copy);
                    return ERR_MEMORY;
                }

                strcpy(current_cmd_buff->_cmd_buffer, temp);
                parse_input(current_cmd_buff);
                clist->num++;
                free(temp);
                temp = NULL;
            }
        } else {
            if (temp == NULL) {
                temp = strdup(cmd);
            } else {
                char *new_temp = (char *)malloc(strlen(temp) + strlen(cmd) + 2);
                if (new_temp == NULL) {
                    free(cmd_copy);
                    free(temp);
                    return ERR_MEMORY;
                }
                sprintf(new_temp, "%s %s", temp, cmd);
                free(temp);
                temp = new_temp;
            }
        }

        cmd = strtok(NULL, " ");
    }

    if (temp) {
        cmd_buff_t *current_cmd_buff = &(clist->commands[clist->num]);
        current_cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);
        if (current_cmd_buff->_cmd_buffer == NULL) {
            free(cmd_copy);
            free(temp);
            return ERR_MEMORY;
        }
        strcpy(current_cmd_buff->_cmd_buffer, temp);
        parse_input(current_cmd_buff);
        clist->num++;
        free(temp);
    }

    free(cmd_copy);
    return OK;
}


int execute_pipeline(command_list_t *clist) {
    int status;

    if (clist->num == 0) {
        printf("%s\n", CMD_WARN_NO_CMD);
        return WARN_NO_CMDS;
    }

    if (clist->num > CMD_MAX) {
        printf("%s\n", CMD_ERR_PIPE_LIMIT);
        return ERR_TOO_MANY_COMMANDS;
    }

    int pipes[CMD_MAX - 1][2];
    pid_t pids[CMD_MAX];

    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            if (execvp(clist->commands[i].argv[0], clist->commands[i].argv) == -1) {
                perror("execvp");
                exit(ERR_EXEC_CMD);
            }
        }
    }

    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    int exit_status = OK;
    for (int i = 0; i < clist->num; i++) {
        if (waitpid(pids[i], &status, 0) == -1) {
            perror("waitpid");
            return ERR_EXEC_CMD;
        }

        if (WIFEXITED(status)) {
            int child_exit_status = WEXITSTATUS(status);
            if (child_exit_status != 0) {
                exit_status = child_exit_status;
            }
        }
    }

    return exit_status;
}