#ifndef HEADER_CLASSCHECK
#define HEADER_CLASSCHECK

int isAValidTag (u1 tag);

int isAValidPoolIndex (ClassFile class, u2 index);

int equalsTags (ClassFile class, u1 tag, u2 index);

void classCheck (ClassFile class);

#endif
