
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include "debugger_interface.h"
#include "dwarf_symbol_reader.h"
#include <string.h>

//identifies a compilation unit and its debugging symbols
struct dwarf_compilation_unit * compilation_unit;
//an array of line info
struct dwarf_lines * dwarf_lines;

//the child we are debugging
pid_t child;

void initialize_debugger(char * program_name){
  compilation_unit = dwarf_get_compilation_unit(program_name);
  dwarf_lines =  dwarf_get_line_info_from_cu(compilation_unit);
}

void debugger(){
  int status;
  
  while(1){
    //wait for a signal
    wait(&status);
    //if the program exited, just return
    if (WIFEXITED( status )){
      printf("\nthe program terminated\n");
      return;
    }

    int done=0;
    while(!done){
      //get user input, what should we do?
      user_command * cmd = debugger_interface_get_cmd();
      if (cmd){
	//now handle the input
	switch(cmd->command_type){
	case CMD_TYPE_BREAKPOINT:
	  printf("breakpoint not implemented\n");
	  break;
	case CMD_TYPE_PRINT:
	  printf("printing not implemented\n");
	  break;
	case CMD_TYPE_CONTINUE:
	  //tell the child to keep going
	  done=1;				
	  break;
	case CMD_TYPE_WHERE:
	  printf("where not implemented\n");
	  break;
	}
      }
    }

    ptrace( PTRACE_CONT, child, NULL, NULL );

  }
}

void exec_debuggee(char * debuggee_name){
	
  char* argv[] = { NULL };
  char* envp[] = { NULL };	
  
  if (ptrace( PTRACE_TRACEME, 0, NULL, NULL ))
    {
      perror( "ptrace" );
      return;
    }
  
  execve( debuggee_name, argv, envp );
}

/*fork a child process, and lets get this started!*/
void start(char * debuggee_name){

  pid_t pid = fork();
  switch(pid){
  case -1:
    perror("error\n");
    exit(1);
    break;
  case 0:
    //the forked process will execute the target program
    exec_debuggee(debuggee_name);
    break;	
  default:
    initialize_debugger(debuggee_name);
    child=pid;
    debugger();
  }
}

//The lone argument is the program to debug
int main(int argc, char ** argv){
  if (argc < 2){
    perror("Error: Must provide a program name\n");
  }
  else{
    start(argv[1]);	
  }
}
