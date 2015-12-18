//
//  Execution_Core.h
//
//
//  Created by Luiz Henrique Campos Barboza on 09/12/15.
//
//

#ifndef Execution_Core_h
#define Execution_Core_h

#include "macros.h"
#include "Frame.h"
#include "Heap.h"
typedef struct _instruction {
    uint16_t qtd_operandos;
    char *nome;
    void (*call)();
} Instruction;

extern const Instruction instructions[];


typedef struct {
    ClassFile **classes;
    ClassFile **interfaces;
    u4 classes_count;
    u4 interfaces_count;
} MethodHeap;

MethodHeap initMethodHeap();

void Execute ();
int getClassIndex(u1 *class_name);
void initialize(int class_index);
void prepare(u4 index);
int loadParentClasses();
int loadInterfaces(ClassFile *class);
int loadClass(char *name);
char* getClassPath(char* base_path, char* class_name);
int getInterfaceIndex(u1* interface_name);
int checkIfFieldIsStatic(uint16_t access_flags);
void expandClassArray();
void expandInterfaceArray();
method_info* getclinit(ClassFile *class);
method_info* getMainMethod();
u1 *getClassNameUtf8(ClassFile *pClass, u2 classIndexTemp);
u1 *getParentName(ClassFile *class);
u1* getInterfaceName(ClassFile *class, int interface_index);
Field_Value *getFieldValue(u1 *name, Field_Value *pField, u2 static_values_size);
u1 *GetStringFromConstPool(u2 nIndex, cp_info *pool);
u1 *getFieldName(u2 index, cp_info *pool);
u1 *getUtf8String(cp_info *pool, int index);
method_info* getclinit(ClassFile *class);
method_info* getMainMethod();
method_info *getMethodByNameDesc(ClassFile *main_class, ClassFile *name_type_class, uint16_t name_type_index);
i4 getNumParameters(ClassFile *class, method_info *method);
u8 getObjectField(struct _object *object, uint32_t field_index);
void setObjectField(struct _object *object, u4 field_index, u8 value);
ClassFile *getClassByName(u1 *classname);
u4 getFieldIndex(u1 *className, u1 *name, u2 nameLen, u1 *desc, u2 descLen);
u8 getStaticFieldVal(u4 class_index, u4 field_index);
void setStaticFieldVal(u4 class_index, u4 field_index, u8 value);
u4 searchStaticFieldVal(u4 class_index, u1* name,u1* desc);

#endif /* Execution_Core_h */
