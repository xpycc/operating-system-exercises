/*
  Copyright (c) 2009-2010 David Anderson.  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the example nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY David Anderson ''AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL David Anderson BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
*/
/*  simplereader.c
    This is an example of code reading dwarf .debug_info.
    It is kept as simple as possible to expose essential features.
    It does not do all possible error reporting or error handling.

    The --names 
    option adds some extra printing.

    To use, try
        make
        ./simplereader simplereader
*/

/*Modified by Tim Merrifield for use in CS 385 at the University of Illinois at Chicago*/

#include <sys/types.h> /* For open() */
#include <sys/stat.h>  /* For open() */
#include <fcntl.h>     /* For open() */
#include <stdlib.h>     /* For exit() */
#include <unistd.h>     /* For close() */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "dwarf.h"
#include "libdwarf.h"
#include "dwarf_symbol_reader.h"

struct srcfilesdata {
  char ** srcfiles;
  Dwarf_Signed srcfilescount;
  int srcfilesres;
};

static struct dwarf_compilation_unit * read_cu_list(Dwarf_Debug dbg);

static void print_die_data(Dwarf_Debug dbg, Dwarf_Die print_me,int level,
   struct srcfilesdata *sf);

static void get_die_and_siblings(Dwarf_Debug dbg, Dwarf_Die in_die,int in_level,
   struct srcfilesdata *sf);

static void resetsrcfiles(Dwarf_Debug dbg,struct srcfilesdata *sf);

//Dwarf_Die get_next_function(Dwarf_Die previous_die, struct dwarf_compilation_unit * unit);

//struct dwarf_lines * get_line_info_from_cu(struct dwarf_compilation_unit * cu);

static int namesoptionon = 1;

static void
get_addr(Dwarf_Attribute attr,Dwarf_Addr *val);

static void
get_number(Dwarf_Attribute attr,Dwarf_Unsigned *val);

//struct dwarf_compilation_unit * get_compilation_unit(char * program);

unsigned long dwarf_get_variable_loc_addr(Dwarf_Loc loc){
  return loc.lr_number;
}

int dwarf_get_variable_loc_offset(Dwarf_Loc loc){
  return (int)loc.lr_number;
}

int dwarf_get_variable_loc_type(Dwarf_Loc loc){
  if (loc.lr_atom == DW_OP_addr){
    return DWARF_LOCATION_ADDRESS;
  }
  else if (loc.lr_atom == DW_OP_fbreg){
    return DWARF_LOCATION_BP_OFFSET;
  }
  else{
    return DWARF_LOCATION_SP_OFFSET;
  }
}

Dwarf_Loc dwarf_get_variable_location(Dwarf_Die die, struct dwarf_compilation_unit * unit){
  
  Dwarf_Error error = 0;
  Dwarf_Attribute *attrbuf = 0;
  Dwarf_Signed attrcount = 0;
  Dwarf_Unsigned i;
  Dwarf_Locdesc *llbuf = 0;
  Dwarf_Locdesc **llbufarray = 0;
  Dwarf_Signed no_of_elements;
  int lres = 0;
  Dwarf_Loc first_loc;

  int res = dwarf_attrlist(die,&attrbuf,&attrcount,&error);
  if(res != DW_DLV_OK) {
    return first_loc;
  }
  for(i = 0; i < attrcount ; ++i) {
    Dwarf_Half aform;
    res = dwarf_whatattr(attrbuf[i],&aform,&error);
    if(aform == DW_AT_location) {
      lres = dwarf_loclist_n(attrbuf[i], &llbufarray, &no_of_elements, &error);
      if (llbufarray[0]->ld_s){
        first_loc = (llbufarray[0]->ld_s)[0];
	return first_loc;
      }
    }
  }
  return first_loc;
}

static Dwarf_Die _get_next_variable(Dwarf_Die die, Dwarf_Die original_die, struct dwarf_compilation_unit * unit){
  Dwarf_Die sib_die = 0;
  Dwarf_Die child = 0;
  Dwarf_Half tag = 0;
  Dwarf_Error error;
  int res;
  //first, see what type this is
  dwarf_tag(die,&tag,&error);
  if ((tag == DW_TAG_variable || tag == DW_TAG_formal_parameter) && die != original_die){
    return die;
  }

  //go through each child
  res = dwarf_child(die,&child,&error);
  if(res == DW_DLV_OK) {
    Dwarf_Die result = _get_next_variable(child,original_die,unit);
    if (result > 0){
      return result;
    }
  }
  
  //now do the siblings
  res = dwarf_siblingof(unit->dbg,die,&sib_die,&error);
  Dwarf_Die old_die = 0;
  while(res != DW_DLV_ERROR && (old_die != sib_die)){
    old_die=sib_die;
    if(res ==  DW_DLV_OK) {
      dwarf_tag(sib_die,&tag,&error);
      if (tag == DW_TAG_variable || tag == DW_TAG_formal_parameter){
	return sib_die;
      }
      res = dwarf_siblingof(unit->dbg,sib_die,&sib_die,&error);
    }
  }
  return NULL;
}

Dwarf_Die dwarf_get_next_variable(Dwarf_Die previous_die, struct dwarf_compilation_unit * unit){
  Dwarf_Die var_die = _get_next_variable(previous_die, previous_die, unit);
  if (var_die > 0){
    return var_die;
  }
  else{
    return 0;
  }
}

struct dwarf_lines * dwarf_get_line_info_from_cu(struct dwarf_compilation_unit * cu){
  Dwarf_Signed linecount = 0;
  Dwarf_Line *linebuf = NULL;
  Dwarf_Unsigned lineno = 0;
  Dwarf_Error err;
  Dwarf_Addr pc;

  int lres = dwarf_srclines(cu->root_die, &linebuf, &linecount, &err);
  struct dwarf_line_of_code * lines = malloc(sizeof(struct dwarf_line_of_code) * linecount);
  int i=0;
  for (; i<linecount; i++) {
    Dwarf_Line line = linebuf[i];
    dwarf_lineaddr(line, &pc, &err);

    lines[i].addr = (unsigned long)pc;
    dwarf_lineno(line, &lineno, &err);
    lines[i].line_number = (unsigned int)lineno;
  }
  struct dwarf_lines * dwarf_lines = malloc(sizeof(struct dwarf_lines));
  dwarf_lines->lines = lines;
  dwarf_lines->number_of_lines = linecount;
  return dwarf_lines;
}


char * dwarf_get_name(Dwarf_Die function_die,  struct dwarf_compilation_unit * unit){
  Dwarf_Error error = 0;
  Dwarf_Attribute *attrbuf = 0;
  Dwarf_Addr highpc = 0;
  Dwarf_Signed attrcount = 0;
  Dwarf_Unsigned i;

  int res = dwarf_attrlist(function_die,&attrbuf,&attrcount,&error);
  if(res != DW_DLV_OK) {
    return 0;
  }
  for(i = 0; i < attrcount ; ++i) {
    Dwarf_Half aform;
    res = dwarf_whatattr(attrbuf[i],&aform,&error);
    if (aform == DW_AT_name) {
      char * tmpName;
      res = dwarf_formstring(attrbuf[i], &tmpName, &error);
      return tmpName;
    }
  }
  return NULL;
}

unsigned long dwarf_function_starting_address(Dwarf_Die function_die,  struct dwarf_compilation_unit * unit){
  Dwarf_Error error = 0;
  Dwarf_Attribute *attrbuf = 0;
  Dwarf_Addr lowpc = 0;
  Dwarf_Signed attrcount = 0;
  Dwarf_Unsigned i;

  int res = dwarf_attrlist(function_die,&attrbuf,&attrcount,&error);
  if(res != DW_DLV_OK) {
    return 0;
  }
  for(i = 0; i < attrcount ; ++i) {
    Dwarf_Half aform;
    res = dwarf_whatattr(attrbuf[i],&aform,&error);
    if(aform == DW_AT_low_pc) {
      get_addr(attrbuf[i],&lowpc);
      return lowpc;
    }
  }
  return 0;
}

unsigned long dwarf_function_ending_address(Dwarf_Die function_die,  struct dwarf_compilation_unit * unit){
  Dwarf_Error error = 0;
  Dwarf_Attribute *attrbuf = 0;
  Dwarf_Addr highpc = 0;
  Dwarf_Signed attrcount = 0;
  Dwarf_Unsigned i;

  int res = dwarf_attrlist(function_die,&attrbuf,&attrcount,&error);
  if(res != DW_DLV_OK) {
    return 0;
  }
  for(i = 0; i < attrcount ; ++i) {
    Dwarf_Half aform;
    res = dwarf_whatattr(attrbuf[i],&aform,&error);
    if(aform == DW_AT_high_pc) {
      get_addr(attrbuf[i],&highpc);
      return highpc;
    }
  }
  return 0;
}

/*static struct dwarf_function * setup_function(Dwarf_Debug dbg,Dwarf_Die die)
{
    int res;
    Dwarf_Error error = 0;
    Dwarf_Attribute *attrbuf = 0;
    Dwarf_Addr lowpc = 0;
    Dwarf_Addr highpc = 0;
    Dwarf_Signed attrcount = 0;
    Dwarf_Unsigned i;
    Dwarf_Unsigned filenum = 0;
    Dwarf_Unsigned linenum = 0;
    char *filename = 0;
    struct dwarf_function * df = malloc(sizeof(struct dwarf_function));
    df->function_die = die;
    res = dwarf_attrlist(die,&attrbuf,&attrcount,&error);
    if(res != DW_DLV_OK) {
        return NULL;
    }
    for(i = 0; i < attrcount ; ++i) {
        Dwarf_Half aform;
        res = dwarf_whatattr(attrbuf[i],&aform,&error);
        if(res == DW_DLV_OK) {
            if(aform == DW_AT_low_pc) {
                get_addr(attrbuf[i],&lowpc);
		df->low_addr=(unsigned long)lowpc;
            }
            if(aform == DW_AT_high_pc) {
                get_addr(attrbuf[i],&highpc);
		df->high_addr=(unsigned long)highpc;
            }
	    if (aform == DW_AT_name) {
	      char * tmpName;
	      res = dwarf_formstring(attrbuf[i], &tmpName, &error);
	      df->function_name = malloc(sizeof(strlen(tmpName)));
	      strcpy(df->function_name,tmpName);
	    }
        }
        dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
    }
    dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
    return df;
    }*/

static Dwarf_Die _get_next_function_helper(Dwarf_Die compilation_die, Dwarf_Debug dbg){
  
  Dwarf_Die cur_die=compilation_die;
  Dwarf_Die sib_die = 0;
  Dwarf_Error error;
  Dwarf_Half tag = 0;
  int res = DW_DLV_ERROR;

  res = dwarf_siblingof(dbg,cur_die,&sib_die,&error);
  cur_die=sib_die;
  while(res==DW_DLV_OK){
    dwarf_tag(cur_die,&tag,&error);
    if (tag == DW_TAG_subprogram){
      //found the first function
      return cur_die;
    }

    res = dwarf_siblingof(dbg,cur_die,&sib_die,&error);
    cur_die=sib_die;
  }
}

Dwarf_Die dwarf_get_next_function(Dwarf_Die previous_die, struct dwarf_compilation_unit * unit){

  Dwarf_Half tag = 0;
  Dwarf_Error error;
  Dwarf_Die func_die=0;

  //first, see what type this is
  dwarf_tag(previous_die,&tag,&error);
  if (tag == DW_TAG_compile_unit){
    //if its the first child, make sure its not a subprogram
    int res = dwarf_child(unit->root_die,&previous_die,&error);
    dwarf_tag(previous_die,&tag,&error);
    //is it a subprogram, if so we are done
    if (tag == DW_TAG_subprogram){
      return previous_die;
    }
  }
  func_die = _get_next_function_helper(previous_die, unit->dbg);
  if (func_die > 0){
    return func_die;
  }

  return NULL;
}


struct dwarf_compilation_unit * dwarf_get_compilation_unit(char * program)
{

    Dwarf_Debug dbg = 0;
    int fd = -1;
    const char *filepath = "<stdin>";
    int res = DW_DLV_ERROR;
    Dwarf_Error error;
    Dwarf_Handler errhand = 0;
    Dwarf_Ptr errarg = 0;

    fd = open(program,O_RDONLY);

    if(fd < 0) {
        printf("Failure attempting to open \"%s\"\n",filepath);
    }
    res = dwarf_init(fd,DW_DLC_READ,errhand,errarg, &dbg,&error);
    if(res != DW_DLV_OK) {
        printf("Giving up, cannot do DWARF processing\n");
        exit(1);
    }

    struct dwarf_compilation_unit * cu = read_cu_list(dbg);
    cu->dbg = dbg;
    
    return cu;

}

static struct dwarf_compilation_unit * 
read_cu_list(Dwarf_Debug dbg)
{
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Half version_stamp = 0;
    Dwarf_Unsigned abbrev_offset = 0;
    Dwarf_Half address_size = 0;
    Dwarf_Unsigned next_cu_header = 0;
    Dwarf_Error error;
    int cu_number = 0;
    struct dwarf_compilation_unit * cu = NULL;

    //for(;;++cu_number) {
        struct srcfilesdata sf;
        sf.srcfilesres = DW_DLV_ERROR;
        sf.srcfiles = 0;
        sf.srcfilescount = 0;
        Dwarf_Die no_die = 0;
        Dwarf_Die cu_die = 0;
        int res = DW_DLV_ERROR;
        res = dwarf_next_cu_header(dbg,&cu_header_length,
            &version_stamp, &abbrev_offset, &address_size,
            &next_cu_header, &error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_next_cu_header\n");
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Done. */
            return NULL;
        }
        /* The CU will have a single sibling, a cu_die. */
        res = dwarf_siblingof(dbg,no_die,&cu_die,&error);

	if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_siblingof on CU die \n");
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            // Impossible case.
            printf("no entry! in dwarf_siblingof on CU die \n");
            exit(1);
        }

	cu = malloc(sizeof(struct dwarf_compilation_unit));
	cu->root_die = cu_die;
	return cu;

        /*get_die_and_siblings(dbg,cu_die,0,&sf);
        dwarf_dealloc(dbg,cu_die,DW_DLA_DIE);
        resetsrcfiles(dbg,&sf);*/
	//}
}

static void
get_die_and_siblings(Dwarf_Debug dbg, Dwarf_Die in_die,int in_level,
   struct srcfilesdata *sf)
{
    int res = DW_DLV_ERROR;
    Dwarf_Die cur_die=in_die;
    Dwarf_Die child = 0;
    Dwarf_Error error;
   
    print_die_data(dbg,in_die,in_level,sf);

    for(;;) {
        Dwarf_Die sib_die = 0;
        res = dwarf_child(cur_die,&child,&error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_child , level %d \n",in_level);
            exit(1);
        }
        if(res == DW_DLV_OK) {
            get_die_and_siblings(dbg,child,in_level+1,sf);
        }
        /* res == DW_DLV_NO_ENTRY */
        res = dwarf_siblingof(dbg,cur_die,&sib_die,&error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_siblingof , level %d \n",in_level);
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Done at this level. */
            break;
        }
        /* res == DW_DLV_OK */
        if(cur_die != in_die) {
            dwarf_dealloc(dbg,cur_die,DW_DLA_DIE);
        }
        cur_die = sib_die;
        print_die_data(dbg,cur_die,in_level,sf);
    }
    return;
}
static void
get_addr(Dwarf_Attribute attr,Dwarf_Addr *val)
{
    Dwarf_Error error = 0;
    int res;
    Dwarf_Addr uval = 0;
    res = dwarf_formaddr(attr,&uval,&error);
    if(res == DW_DLV_OK) {
        *val = uval;
        return;
    }
    return;
}
static void
get_number(Dwarf_Attribute attr,Dwarf_Unsigned *val)
{
    Dwarf_Error error = 0;
    int res;
    Dwarf_Signed sval = 0;
    Dwarf_Unsigned uval = 0;
    res = dwarf_formudata(attr,&uval,&error);
    if(res == DW_DLV_OK) {
        *val = uval;
        return;
    }
    res = dwarf_formsdata(attr,&sval,&error);
    if(res == DW_DLV_OK) {
        *val = sval;
        return;
    }
    return;
}
static void
print_subprog(Dwarf_Debug dbg,Dwarf_Die die, int level,
    struct srcfilesdata *sf)
{
    int res;
    Dwarf_Error error = 0;
    Dwarf_Attribute *attrbuf = 0;
    Dwarf_Addr lowpc = 0;
    Dwarf_Addr highpc = 0;
    Dwarf_Signed attrcount = 0;
    Dwarf_Unsigned i;
    Dwarf_Unsigned filenum = 0;
    Dwarf_Unsigned linenum = 0;
    char *filename = 0;
    res = dwarf_attrlist(die,&attrbuf,&attrcount,&error);
    if(res != DW_DLV_OK) {
        return;
    }
    for(i = 0; i < attrcount ; ++i) {
        Dwarf_Half aform;
        res = dwarf_whatattr(attrbuf[i],&aform,&error);
        if(res == DW_DLV_OK) {
            if(aform == DW_AT_decl_file) {
                get_number(attrbuf[i],&filenum);
                if((filenum > 0) && (sf->srcfilescount > (filenum-1))) {
                    filename = sf->srcfiles[filenum-1];
                }
            }
            if(aform == DW_AT_decl_line) {
                get_number(attrbuf[i],&linenum);
            }
            if(aform == DW_AT_low_pc) {
                get_addr(attrbuf[i],&lowpc);
            }
            if(aform == DW_AT_high_pc) {
                get_addr(attrbuf[i],&highpc);
            }
        }
        dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
    }
    if(filenum || linenum) {
        printf("<%3d> file: %" DW_PR_DUu " %s  line %"
            DW_PR_DUu "\n",level,filenum,filename?filename:"",linenum);
    }
    if(lowpc) {
        printf("<%3d> low_pc : 0x%" DW_PR_DUx  "\n",
            level, (Dwarf_Unsigned)lowpc);
    }
    if(highpc) {
        printf("<%3d> high_pc: 0x%" DW_PR_DUx  "\n",
            level, (Dwarf_Unsigned)highpc);
    }
    dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
}

static void
print_comp_dir(Dwarf_Debug dbg,Dwarf_Die die,int level, struct srcfilesdata *sf)
{
    int res;
    Dwarf_Error error = 0;
    Dwarf_Attribute *attrbuf = 0;
    Dwarf_Signed attrcount = 0;
    Dwarf_Unsigned i;
    res = dwarf_attrlist(die,&attrbuf,&attrcount,&error);
    if(res != DW_DLV_OK) {
        return;
    }
    sf->srcfilesres = dwarf_srcfiles(die,&sf->srcfiles,&sf->srcfilescount, 
        &error);
    for(i = 0; i < attrcount ; ++i) {
        Dwarf_Half aform;
        res = dwarf_whatattr(attrbuf[i],&aform,&error);
        if(res == DW_DLV_OK) {
            if(aform == DW_AT_comp_dir) {
                char *name = 0;
                res = dwarf_formstring(attrbuf[i],&name,&error);
                if(res == DW_DLV_OK) {
                    printf(    "<%3d> compilation directory : \"%s\"\n",
                        level,name);
                }
            }
            if(aform == DW_AT_stmt_list) {
                /* Offset of stmt list for this CU in .debug_line */
            }
        }
        dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
    }
    dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
}

static void
resetsrcfiles(Dwarf_Debug dbg,struct srcfilesdata *sf)
{
    Dwarf_Signed sri = 0;
    for (sri = 0; sri < sf->srcfilescount; ++sri) {
        dwarf_dealloc(dbg, sf->srcfiles[sri], DW_DLA_STRING);
    }
    dwarf_dealloc(dbg, sf->srcfiles, DW_DLA_LIST);
    sf->srcfilesres = DW_DLV_ERROR;
    sf->srcfiles = 0;
    sf->srcfilescount = 0;
}

static void
print_die_data(Dwarf_Debug dbg, Dwarf_Die print_me,int level,
    struct srcfilesdata *sf)
{
    char *name = 0;
    Dwarf_Error error = 0;
    Dwarf_Half tag = 0;
    const char *tagname = 0;
    int localname = 0;

    int res = dwarf_diename(print_me,&name,&error);

    if(res == DW_DLV_ERROR) {
        printf("Error in dwarf_diename , level %d \n",level);
        exit(1);
    }
    if(res == DW_DLV_NO_ENTRY) {
        name = "<no DW_AT_name attr>";
        localname = 1;
    }
    res = dwarf_tag(print_me,&tag,&error);
    if(res != DW_DLV_OK) {
        printf("Error in dwarf_tag , level %d \n",level);
        exit(1);
    }
    res = dwarf_get_TAG_name(tag,&tagname);
    if(res != DW_DLV_OK) {
        printf("Error in dwarf_get_TAG_name , level %d \n",level);
        exit(1);
    }
    if(namesoptionon) {
        if( tag == DW_TAG_subprogram) {
            printf(    "<%3d> subprogram            : \"%s\"\n",level,name);
            print_subprog(dbg,print_me,level,sf);
        } else if (tag == DW_TAG_compile_unit ||
            tag == DW_TAG_partial_unit ||
            tag == DW_TAG_type_unit) {

            resetsrcfiles(dbg,sf);
            printf(    "<%3d> source file           : \"%s\"\n",level,name);
            print_comp_dir(dbg,print_me,level,sf);
        }
	else {
	  printf("<%d> tag: %d %s  name: \"%s\"\n",level,tag,tagname,name);
	}
	}
    if(!localname) {
        dwarf_dealloc(dbg,name,DW_DLA_STRING);
    }
}



