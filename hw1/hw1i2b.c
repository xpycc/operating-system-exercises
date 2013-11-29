#include <stdio.h>
#include "hw1.h"
#include "parser.h"

int main(){
	//getting array of students
	Student students[100];
	int count = getArrayOfStudents("data", students);
  printf("Original record:\n\n");
	printStudent(students[0]);

	// rename the student and print
  char *newName="Hu Li";
  printf("\nRenaming to \"%s\"\n\n",newName);
	students[0] = renameStudent(students[0], newName);
  printStudent(students[0]);

  // rename and print again
  newName="Balakrishnan Radhakrishnan";
  printf("\nRenaming to \"%s\"\n\n",newName);
  students[0] = renameStudent(students[0], newName);
	printStudent(students[0]);
	
}
