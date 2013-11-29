#include <stdio.h>
#include <stdlib.h>
#include "hw1.h"

int getArrayOfStudents(char * fileName, Student * students){
	char lastname[30],firstname[30];

	FILE * file = fopen(fileName, "r");	//open the file
	int idCounter=0;
	if (file){			
			 while(fscanf(file,"%c,%30s %30s\n",&students[idCounter].grade,firstname,lastname)==3) {
						students[idCounter].identifier=idCounter+1;						
						snprintf(students[idCounter].name,30,"%s %s",firstname,lastname);
						idCounter++;
			 }
	}
	return idCounter;
}
