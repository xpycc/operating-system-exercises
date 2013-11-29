#include <stdio.h>
#include "hw1.h"
#include "parser.h"

int main(){
	//getting array of students
	Student students[100];
	int count = getArrayOfStudents("data", students);
	//print all of the students with the abbreviated format
	int i = 0;
	for(i=0;i<count;++i){
		printStudentShort(students[i]);
	}
}