#ifndef HEADER_JVM_CORE
#define HEADER_JVM_CORE
#include "macros.h"
#include <sys/types.h>
#include <dirent.h>

void jvmStartup(char *classPathStr, int flag);

void initializeClass(ClassFile *class_ptr);

u2 seekMethodInClass(ClassFile *class_ptr, char *methName_str, char *methDescriptor_str);

int findClass(char* ClassName);

u2 findCode(method_info *method);

void createFrame(method_info *method, ClassFile *Class);

void deleteFrame();

void callMethod(ClassFile *class_ptr, char *mth_name, char *mth_descriptor);

int searchFile (char* name);

#endif
