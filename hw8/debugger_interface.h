
#ifndef DEBUGGER_INTERFACE
#define DEBUGGER_INTERFACE

/*all of the different command types*/
#define CMD_TYPE_BREAKPOINT 1
#define CMD_TYPE_PRINT 2
#define CMD_TYPE_CONTINUE 3
#define CMD_TYPE_WHERE 4

typedef struct _user_command{
  int command_type;
  union{
    unsigned long address;	
    unsigned int line_number;
    struct print_var{
      char * name;
      char * format;
    }var;
  }value;

} user_command;

user_command * debugger_interface_get_cmd();

#endif
