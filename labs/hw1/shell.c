#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "tokenizer.h"


#define CHECK_SYSCALL_RET(ret, exit_code)      \
  if(ret < 0) {                     \
    char *error = strerror(errno);    \
    fprintf(stdout, "%s\n", error); \
    if(exit_code != 0){             \
      _exit(exit_code);             \
    }                               \
  }

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_exit(struct tokens *tokens);
int cmd_help(struct tokens *tokens);
int cmd_cd(struct tokens *tokens);
int cmd_pwd(struct tokens *tokens);

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens *tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_exit, "exit", "exit the command shell"},
  {cmd_cd, "cd", "change current working directory"},
  {cmd_pwd, "pwd", "print current working directory to standard output"},
};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens *tokens) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens *tokens) {
  exit(0);
}

/* Prints the current working directory to standard output */
int cmd_pwd(unused struct tokens *tokens){
  char* pwd = getcwd(NULL, 0);
  if(pwd){
    printf("%s\n", pwd);
    free(pwd);
  }
  return 0;
}

/* Change current working directory to the specified directory */
int cmd_cd(unused struct tokens *tokens){
  int len = tokens_get_length(tokens);
  if(len != 2){
    printf("try '?' for help.\n");
    return 1;
  }
  char* new_wd = tokens_get_token(tokens, 1);
  errno = 0;
  int ret = chdir(new_wd);
  if(ret != 0){
    char *error = strerror(errno);
    printf("%s\n", error);
    return errno;
  }
  return 0;
}

/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
  /* Our shell is connected to standard input. */
  shell_terminal = STDIN_FILENO;

  /* Check if we are running interactively */
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive) {
    /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
     * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
     * foreground, we'll receive a SIGCONT. */
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN);

    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Save the current termios to a variable, so it can be restored later. */
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

int main(unused int argc, unused char *argv[]) {
  init_shell();

  static char line[4096];
  int line_num = 0;
  char* args[256];

  /* Please only print shell prompts when standard input is not a tty */
  if (shell_is_interactive)
    fprintf(stdout, "%d: ", line_num);

  while (fgets(line, 4096, stdin)) {
    /* Split our line into words. */
    struct tokens *tokens = tokenize(line);

    /* Find which built-in function to run. */
    int fundex = lookup(tokens_get_token(tokens, 0));

    if (fundex >= 0) {
      cmd_table[fundex].fun(tokens);
    } else {
      /* REPLACE this to run commands as programs. */
      int ret = 0;
      pid_t pid = fork();
      if(pid == 0){
        int tlen = tokens_get_length(tokens);
        int idx = 0;
        for(idx = 0; idx < tlen; idx++){
          char* s = tokens_get_token(tokens, idx);
          if(strcmp(s, "<") == 0 || strcmp(s, ">") == 0){
            break;
          }
          args[idx] = s;
        }
        args[idx] = NULL;
        char* in_file = NULL;
        char* out_file = NULL;
        for(; idx < tlen; idx++){
          char* s = tokens_get_token(tokens, idx);
          if(strcmp(s, "<") == 0 && in_file == NULL && idx+1 < tlen){
            in_file = tokens_get_token(tokens, idx+1);
          }else if(strcmp(s, ">") == 0 && out_file == NULL && idx+1 < tlen){
            out_file = tokens_get_token(tokens, idx+1);
          }else{
            continue;
          }
        }
        if(in_file){
          int rd_fd = open(in_file, O_CLOEXEC|O_RDONLY);
          CHECK_SYSCALL_RET(rd_fd, 1);
          dup2(rd_fd, 0);
        }
        if(out_file){
          int wr_fd = open(out_file, O_CLOEXEC|O_WRONLY|O_CREAT|O_TRUNC,
              S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR);
          CHECK_SYSCALL_RET(wr_fd, 1);
          dup2(wr_fd, 1);
        }
        execvp(args[0], args);
        return errno;
      }else{
        waitpid(pid, &ret, 0);
      }
    }

    if (shell_is_interactive)
      /* Please only print shell prompts when standard input is not a tty */
      fprintf(stdout, "%d: ", ++line_num);

    /* Clean up memory */
    tokens_destroy(tokens);
  }

  return 0;
}
