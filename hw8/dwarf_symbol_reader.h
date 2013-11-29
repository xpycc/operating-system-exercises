

#include "dwarf.h"
#include "libdwarf.h"

/*This figures out if the location is an offset from the stack pointer or a register*/
#define DWARF_LOCATION_SP_OFFSET 0x1
#define DWARF_LOCATION_BP_OFFSET 0x2
#define DWARF_LOCATION_ADDRESS 0x3

/*a structure that represents a line of code*/
struct dwarf_line_of_code{
  unsigned long addr;  /*this instruction's memory address*/
  unsigned int line_number; /*what line is it in in the program?*/
};

/*a collection of instructions*/
struct dwarf_lines{
  struct dwarf_line_of_code * lines; /* an array of instructions*/
  unsigned int number_of_lines; /*how many instructions do we have?*/
};

/*each binary is treated as a seperate compilation unit.*/
struct dwarf_compilation_unit{
  Dwarf_Die root_die;  /*the root debugging entry for the compilation unit*/
  Dwarf_Debug dbg;  /*an object that represents the compilation unit*/
};

/***********functions for working with variables****************************/

/*given a variable (var_die), get the object that represents its location*/
Dwarf_Loc dwarf_get_variable_location(Dwarf_Die var_die, struct dwarf_compilation_unit * unit);

/*if its a DWARF_LOCATION_ADDRESS, get its address*/
unsigned long dwarf_get_variable_loc_addr(Dwarf_Loc loc);

/*if its an DWARF_LOCATION_BP_OFFSET or DWARF_LOCATION_SP_OFFSET, get the offset.*/
int dwarf_get_variable_loc_offset(Dwarf_Loc loc);

/*get the location type...DWARF_LOCATION_* */
int dwarf_get_variable_loc_type(Dwarf_Loc loc);

/*a function that helps you iterate through all the variables for a given function*/
Dwarf_Die dwarf_get_next_variable(Dwarf_Die function_die, struct dwarf_compilation_unit * unit);

/***************************************************************************/

/************Functions for working with function types **********************/

/*a function that helps you iterate through all the functions in a compilation unit*/
Dwarf_Die dwarf_get_next_function(Dwarf_Die previous_die, struct dwarf_compilation_unit * unit);

unsigned long dwarf_function_starting_address(Dwarf_Die function_die,  struct dwarf_compilation_unit * unit);

unsigned long dwarf_function_ending_address(Dwarf_Die function_die,  struct dwarf_compilation_unit * unit);


/***************************************************************************/

/*this one gets all the lines (instructions) associated with the program*/
struct dwarf_lines * dwarf_get_line_info_from_cu(struct dwarf_compilation_unit * cu);

/*this function gets the compilation unit, this should be called at the beginning*/
struct dwarf_compilation_unit * dwarf_get_compilation_unit(char * program);

/*get the name of a function or a variable*/
char * dwarf_get_name(Dwarf_Die function_of_var_die,  struct dwarf_compilation_unit * unit);
