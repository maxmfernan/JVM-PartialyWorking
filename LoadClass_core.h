#ifndef HEADER_LOADCLASS_CORE
#define HEADER_LOADCLASS_CORE

#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>


ClassFile *classLoader(char *classPathStr);

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Constant Pool    -   inicio
////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Funções auxiliares de leitura*/
u2 convertEndianU2 (u2 num);

u4 convertEndianU4 (u4 num);

u4 read_magic (FILE *arq);

u2 read_minor_version(FILE *arq);

u2 read_major_version(FILE *arq);

u2 read_u2 (FILE *arq);

u4 read_u4 (FILE *arq);

void readUtf8 (u2 *poolLength, FILE *arq, cp_info* aux);

void readIntegerFloat (FILE *arq, cp_info* aux);

void readLongDouble (FILE *arq, cp_info* aux);

void readClass (FILE *arq, cp_info* aux);

void readString (FILE *arq, cp_info* aux);

void readRefs (FILE *arq, cp_info* aux);

void readNameAndType (FILE *arq, cp_info* aux);

/*Funções de leitura, busca e criação*/
cp_info* readConstantPool (u2 length, u2 *poolLength, FILE *arq);

char* searchConstantType (u1 tag);

cp_info* createConstantPool (u2 *poolElementsNum, u2 *poolLength, FILE *arq);

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Fields    -   inicio
////////////////////////////////////////////////////////////////////////////////////////////////////////

u2 arrayToint(u1 array[], unsigned int len);

void* swapEndianess(void* number, unsigned int bytesn);

field_info *field_func(FILE *class_f, unsigned int offset, unsigned int *bytes_ptr, cp_info* pool_array, unsigned int *fieldn);

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods
////////////////////////////////////////////////////////////////////////////////////////////////////////

void read_Methods(ClassFile *classFileVar, FILE *arq, cp_info *pool);

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Atributos
////////////////////////////////////////////////////////////////////////////////////////////////////////

attribute_info *readAttributeInfo (u2 attr_count, FILE *arq, cp_info *pool, int method);

attribute_info *createAttributeInfo (FILE *arq, cp_info *pool, u2 *attr_count);

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Interface
////////////////////////////////////////////////////////////////////////////////////////////////////////

u2* interface_decode (FILE *arq, u2 *inter_length, u2 *poll_offset, u2 *interface_offset);
void fillStaticFields(ClassFile *clsFile_ptr);


//Extras
u2 buildCPindex(u1 byte1, u2 byte2);


double getDouble(u4 highBytes, u4 lowBytes);
long getLong(u4 highBytes, u4 lowBytes);

#endif
