
#include <stdio.h>
#include "hw1.h"

void printStudent(Student student){
  char *p = (char*)&student;
  for (int i = 0; i < sizeof student; ++i)
    printf("%02x%c", p[i], i != sizeof student - 1 ? ' ' : '\n');
  printf("ID: %d\n", student.identifier);
  printf("Name: %s\n", student.name);
  printf("Grade: %c\n", student.grade);
}

void printStudentShort(Student student){
  static int cc = 0;
  char *p = student.name;
  while (*p++ != ' ')
    ;
  printf("%d. %c. %s: %c\n", ++cc, student.name[0], p, student.grade);
}

Student renameStudent(Student s, char* newName){
  int i;
  for (i = 0; i < 29 && newName[i]; ++i)
    s.name[i] = newName[i];
  s.name[i] = 0;
  return s;
}

void sortStudentRecords(Student *s, int count){
  for (int i = 0; i < count - 1; ++i) {
    int k = i;
    for (int j = i + 1; j < count; ++j)
      if (s[j].grade < s[k].grade)
        k = j;
    if (k != i) {
      Student t = s[i];
      s[i] = s[k];
      s[k] = t;
    }
  }
} 
