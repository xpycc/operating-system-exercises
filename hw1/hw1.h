#ifndef HW1_H
#define HW1_H

typedef struct student{
	int identifier;
	char grade;
	char name[30];
} Student;

void printStudent(Student student);

void printStudentShort(Student student);

Student renameStudent(Student s, char* newName);

void sortStudentRecords(Student *students, int count);

#endif