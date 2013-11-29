#include <stdio.h>
#include "hw1.h"
#include "parser.h"

int main(){
	//getting array of students
	Student students[100];
	int count = getArrayOfStudents("data", students);
	//print all of the students
	int i = 0;
	for(i=0;i<count;++i){
		printStudent(students[i]);
	}
}