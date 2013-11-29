
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include "debugger_interface.h"

int ui_initialized = 0;

#define ERROR_MESSAGE "input is not valid\n"

user_command * get_cmd_type(char * input){
	
	user_command * cmd = malloc(sizeof(user_command));
	memset(cmd,0,sizeof(user_command));

	char * first_word = strsep(&input, " ");
	char * second_word;
	char * third_word;
	char * buffer;

	switch (first_word[0]){
	case 'w':
	  cmd->command_type = CMD_TYPE_WHERE;
	  break;
	case 'c': 
	  cmd->command_type = CMD_TYPE_CONTINUE;
	  break;
	case 'b':
	  cmd->command_type = CMD_TYPE_BREAKPOINT;
	  second_word = strsep(&input, " ");
	  if (!second_word){
	    printf(ERROR_MESSAGE);
	    return NULL;
	  }
	  cmd->value.line_number = atoi(second_word);	  
	  break;
	case 'p':
	  cmd->command_type = CMD_TYPE_PRINT;
	  second_word = strsep(&input, " ");
	  if (!second_word){
	    printf(ERROR_MESSAGE);
	    return NULL;
	  }
	  cmd->value.var.format = second_word;

	  third_word = strsep(&input, " ");
	  if (!third_word){
	    printf(ERROR_MESSAGE);
	    return NULL;
	  }
	  cmd->value.var.name = third_word;
	  break;
	case 'q':
	  printf("quit\n");
	  exit(0);
	default:
	  printf("an error occurred\n");
	  exit(1);
	}
	return cmd;
}

user_command * debugger_interface_get_cmd(){

	char *line = readline ("385db>> ");	
	if (ui_initialized != 1){
		using_history();
	}
	add_history (line);
	return get_cmd_type(line);		
}
