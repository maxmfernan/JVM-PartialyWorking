//
//  instructions.c
//
//
//  Created by Luiz Henrique Campos Barboza on 13/12/15.
//
//

#include <stdio.h>
#include <inttypes.h>
#include "Heap.h"
#include "Exceptions.h"
#include "Execution_Core.h"
#include "JVM_core.h"
#include "macros.h"
#include "LoadClass_core.h"

uint8_t wide = 0;
extern ClassFile *classHeap;
extern dataMSize_t dmSize;
extern Frame **stackFrame;
extern Frame *executionFrame;
extern MethodHeap mHeap;
extern char *basePath;
extern Heap objHeap;
extern int saiu;


/*!
	Incrementa PC
 */
static void func_op_nop() {
    executionFrame->pc++;
}

/*!
	Empilha NULL na pilha de operandos
 */
static void func_op_aconst_null() {
    push(0);
    executionFrame->pc++;
}

/*!
	Empilha int -1 na pilha de operandos
 */
static void func_op_iconst_m1() {
    push(-1);
    executionFrame->pc++;
}

/*!
	Empilha int 0 na pilha de operandos
 */
static void func_op_iconst_0() {
    push(0);
    executionFrame->pc++;
}

/*!
	Empilha int 1 na pilha de operandos
 */
static void func_op_iconst_1() {
    push(1);
    executionFrame->pc++;
}

/*!
	Empilha int 2 na pilha de operandos
 */
static void func_op_iconst_2() {
    push(2);
    executionFrame->pc++;
}

/*!
	Empilha int 3 na pilha de operandos
 */
static void func_op_iconst_3() {
    push(3);
    executionFrame->pc++;
}

/*!
	Empilha int 4 na pilha de operandos
 */
static void func_op_iconst_4() {
    push(4);
    executionFrame->pc++;
}

/*!
	Empilha int 5 na pilha de operandos
 */
static void func_op_iconst_5() {
    push(5);
    executionFrame->pc++;
}

/*!
	Empilha long 0 na pilha de operandos
 */
static void func_op_lconst_0() {
    push2(0);
    executionFrame->pc++;
}

/*!
	Empilha long 1 na pilha de operandos
 */
static void func_op_lconst_1() {
    push2(1);
    executionFrame->pc++;
}

/*!
	Empilha constante float 0.0 na pilha de operandos
 */
static void func_op_fconst_0() {
    float float_number = 0.0;
    push(*((u8*)&float_number));
    executionFrame->pc++;
}

/*!
	Empilha constante float 1.0 na pilha de operandos
 */
static void func_op_fconst_1() {
    float float_number = 1.0;
    push(*((u8*)&float_number));
    executionFrame->pc++;
}

/*!
	Empilha constante float 2.0 na pilha de operandos
 */
static void func_op_fconst_2() {
    float float_number = 2.0;
    push(*((u8*)&float_number));
    executionFrame->pc++;
}

/*!
	Empilha constante double 0.0 na pilha de operandos
 */
static void func_op_dconst_0() {
    double double_number = 0.0;
    u8 *aux_64 = malloc(sizeof(u8));

    memcpy(aux_64, &double_number, sizeof(u8));
    push2(*aux_64);
    executionFrame->pc++;
}

/*!
	Empilha constante double 1.0 na pilha de operandos
 */
static void func_op_dconst_1() {
    double double_number = 1.0;
    u8 *aux_64 = malloc(sizeof(u8));

    memcpy(aux_64, &double_number, sizeof(u8));
    push2(*aux_64);
    executionFrame->pc++;
}

/*!
	Empilha um byte.
	O valor imediato do byte tem o sinal extendido para um valor int  (8 bits)
	O valor é colocado na pilha de operandos
 */
static void func_op_bipush() {
    //Pula os 8 bits codigo da instrucao contida no array de codes
    int8_t var = 0;
    i8 aux;

    executionFrame->pc++;
    var = executionFrame->code[executionFrame->pc];
    aux = (i8) var;
    push(aux);

    executionFrame->pc++;
}

static void func_op_sipush() {
    uint8_t high,low;
    int16_t aux_16;
    //i8 t;

    executionFrame->pc++;
    high = executionFrame->code[executionFrame->pc];
    executionFrame->pc++;
    low = executionFrame->code[executionFrame->pc];

    aux_16 = high;
    aux_16 <<= 8;
    aux_16 |= low;

    push(aux_16);
    executionFrame->pc++;
}



static void func_op_ldc() {
    uint8_t index, type;

    executionFrame->pc++;
    index = executionFrame->code[executionFrame->pc];
    type = executionFrame->pClass->constant_pool[index - 1].tag;

    switch (type) {
        case CONSTANT_INTEGER:
            push((i8)executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_IntegerFloat_info.bytes);
            break;
        case CONSTANT_FLOAT:
            push(executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_IntegerFloat_info.bytes);
            break;
        case CONSTANT_STRING:
            push(executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_String_info.string_index);
            break;

        default:
            break;
    }
    executionFrame->pc++;

}

static void func_op_ldc_w() {
    uint64_t index;
    uint8_t type;
    uint64_t high, low;

    executionFrame->pc++;

    high = executionFrame->code[executionFrame->pc];
    executionFrame->pc++;
    low = executionFrame->code[executionFrame->pc];

    index = high;
    index = index << 8;
    index = index | low;

    type = executionFrame->pClass->constant_pool[index - 1].tag;

    switch (type) {
        case CONSTANT_INTEGER:
            push((i8)executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_IntegerFloat_info.bytes);
            break;
        case CONSTANT_FLOAT:
            push((i8)executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_IntegerFloat_info.bytes);
            break;
        case CONSTANT_STRING:
            push(executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_String_info.string_index);
            break;

        default:
            break;
    }
    executionFrame->pc++;
}

static void func_op_ldc2_w() {

    uint64_t index, high, low, completeValue = 0;
    uint8_t type;

    high = executionFrame->code[++executionFrame->pc];
    low = executionFrame->code[++executionFrame->pc];

    index = high;
    index = index << 8;
    index = index | low;

    type = executionFrame->pClass->constant_pool[index - 1].tag;

    switch (type) {
        case CONSTANT_LONG:
            high = executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_LongDouble_info.high_bytes;
            low = executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_LongDouble_info.low_bytes;
            long l = getLong(high, low);
            memcpy(&completeValue, &l, sizeof(u8));

            push2(completeValue);
            break;
        case CONSTANT_DOUBLE:
            high = executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_LongDouble_info.high_bytes;
            low = executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_LongDouble_info.low_bytes;

            double d = getDouble(high, low);

            memcpy(&completeValue, &d, sizeof(u8));

            push2(completeValue);
            break;
        default:
            break;
    }
    executionFrame->pc++;
}


static void func_op_iload() {

    uint16_t index;
    executionFrame->pc++;
    index =  executionFrame->code[executionFrame->pc];

    if(wide == 1){
        index = index << 8;
        executionFrame->pc++;
        index = index | executionFrame->code[executionFrame->pc];
        wide = 0;
    }

    push(executionFrame->local[index]);
    executionFrame->pc++;
}

static void func_op_lload() {
    uint16_t index;
    executionFrame->pc++;
    index = executionFrame->code[executionFrame->pc];

    if(wide == 1){
        index = index << 8;
        executionFrame->pc++;
        index = index | executionFrame->code[executionFrame->pc];
        wide = 0;
    }

    push(executionFrame->local[index]);
    push(executionFrame->local[index + 1]);

    executionFrame->pc++;

}
static void func_op_fload() {
    func_op_iload();
}

static void func_op_dload() {
    func_op_lload();
}

static void func_op_aload() {
    func_op_iload();
}

static void func_op_iload_0() {
    push(executionFrame->local[0]);
    executionFrame->pc++;
}

static void func_op_iload_1() {
    push(executionFrame->local[1]);
    executionFrame->pc++;
}

static void func_op_iload_2() {
    push(executionFrame->local[2]);
    executionFrame->pc++;
}

static void func_op_iload_3() {
    push(executionFrame->local[3]);
    executionFrame->pc++;
}

static void func_op_lload_0() {
    push(executionFrame->local[0]);
    push(executionFrame->local[1]);
    executionFrame->pc++;
}

static void func_op_lload_1() {
    push(executionFrame->local[1]);
    push(executionFrame->local[2]);
    executionFrame->pc++;
}

static void func_op_lload_2() {
    push(executionFrame->local[2]);
    push(executionFrame->local[3]);
    executionFrame->pc++;
}

static void func_op_lload_3() {
    push(executionFrame->local[3]);
    push(executionFrame->local[4]);
    executionFrame->pc++;
}

static void func_op_fload_0() {
    func_op_iload_0();
}

static void func_op_fload_1() {
    func_op_iload_1();
}

static void func_op_fload_2() {
    func_op_iload_2();
}

static void func_op_fload_3() {
    func_op_iload_3();
}

static void func_op_dload_0() {
    func_op_lload_0();
}

static void func_op_dload_1() {
    func_op_lload_1();
}

static void func_op_dload_2() {
    func_op_lload_2();
}

static void func_op_dload_3() {
    func_op_lload_3();
}

static void func_op_aload_0() {
    func_op_iload_0();
}

static void func_op_aload_1() {
    func_op_iload_1();
}

static void func_op_aload_2() {
    func_op_iload_2();
}

static void func_op_aload_3() {
    func_op_iload_3();
}

static void func_op_iaload() {
    u8 index, aux;
    struct _array *arrayRef;

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    push(((uint32_t*)arrayRef->values)[index]);

    executionFrame->pc++;
}

static void func_op_laload() {
    u8 index, aux;
    struct _array *arrayRef;

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8)); // convert to pointer

    //printf("\nlaload: index: %llx, aux: %llx, arrayRef: %p, values: %p\n", index, aux, arrayRef, arrayRef->values);
    push2(((u8*)arrayRef->values)[index]);
    executionFrame->pc++;
}

static void func_op_faload() {
    u8 index, aux;
    struct _array* arrayRef;

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8)); // convert to pointer

    push(((uint32_t*)arrayRef->values)[index]);
    executionFrame->pc++;
}

static void func_op_daload() {
    u8 index, aux;
    struct _array *arrayRef;

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8)); // convert to pointer

    push2(((u8*)arrayRef->values)[index]);
    executionFrame->pc++;
}

static void func_op_aaload() {
    u8 index, aux;
    struct _array *arrayRef;

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    push(((u8*)arrayRef->values)[index]);

    executionFrame->pc++;
}

static void func_op_baload() {
    u8 index,aux;
    struct _array* arrayRef;

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    push(((int8_t*)arrayRef->values)[index]);
    executionFrame->pc++;

}

static void func_op_caload() {
    u8 index,aux;
    struct _array* arrayRef;

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    push(((uint16_t*)arrayRef->values)[index]);
    executionFrame->pc++;
}

static void func_op_saload() {
    u8 index,aux;
    i8 t;
    struct _array* arrayRef;

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    t = ((int16_t*)arrayRef->values)[index];
    push(t);
    executionFrame->pc++;
}

static void func_op_istore() {
    uint16_t index;
    i8 value;

    executionFrame->pc++;

    index = executionFrame->code[executionFrame->pc];
    value = (i8)pop();

    executionFrame->local[index] = value;
    executionFrame->pc++;
}

static void func_op_lstore() {
    uint16_t index;
    u8 high, low;

    executionFrame->pc++;

    index = executionFrame->code[executionFrame->pc];
    low = pop();
    high = pop();

    executionFrame->local[index] = high;
    executionFrame->local[index+1] = low;

    executionFrame->pc++;
}

static void func_op_fstore() {
    func_op_istore();
}

static void func_op_dstore() {
    uint16_t index;
    u8 high, low;

    executionFrame->pc++;

    index = executionFrame->code[executionFrame->pc];
    low = pop();
    high = pop();

    executionFrame->local[index] = high;
    executionFrame->local[index+1] = low;

    executionFrame->pc++;
}

static void func_op_astore() {
    func_op_istore();
}

static void func_op_istore_0() {
    u8 value = pop();

    executionFrame->local[0] = value;
    executionFrame->pc++;
}

static void func_op_istore_1() {
    i8 value = pop();

    executionFrame->local[1] = value;
    executionFrame->pc++;
}

static void func_op_istore_2() {
    u8 value = pop();

    executionFrame->local[2] = value;
    executionFrame->pc++;
}

static void func_op_istore_3() {
    u8 value = pop();

    executionFrame->local[3] = value;
    executionFrame->pc++;
}

static void func_op_lstore_0() {
    u8 high, low;

    low = pop();
    high = pop();

    executionFrame->local[0] = high;
    executionFrame->local[1] = low;
    executionFrame->pc++;
}

static void func_op_lstore_1() {
    u8 high, low;

    low = pop();
    high = pop();

    executionFrame->local[1]= high;
    executionFrame->local[2] = low;
    executionFrame->pc++;
}

static void func_op_lstore_2() {
    u8 high, low;

    low = pop();
    high = pop();

    executionFrame->local[2] = high;
    executionFrame->local[3] = low;
    executionFrame->pc++;
}

static void func_op_lstore_3() {
    u8 high, low;

    low = pop();
    high = pop();

    executionFrame->local[3]= high;
    executionFrame->local[4] = low;
    executionFrame->pc++;
}

static void func_op_fstore_0() {
    u8  value;

    value = pop();

    executionFrame->local[0] = value;
    executionFrame->pc++;
}

static void func_op_fstore_1() {
    func_op_istore_1();
}


static void func_op_fstore_2() {
    func_op_istore_2();
}

static void func_op_fstore_3() {
    func_op_istore_3();
}

static void func_op_dstore_0() {
    func_op_lstore_0();
}

static void func_op_dstore_1() {
    func_op_lstore_1();
}

static void func_op_dstore_2() {
    func_op_lstore_2();
}

static void func_op_dstore_3() {
    func_op_lstore_3();
}

static void func_op_astore_0() {
    u8 value = pop();

    executionFrame->local[0] = value;
    executionFrame->pc++;
}

static void func_op_astore_1() {
    u8 value = pop();

    executionFrame->local[1] = value;
    executionFrame->pc++;
}

static void func_op_astore_2() {
    u8 value = pop();

    executionFrame->local[2] = value;
    executionFrame->pc++;
}

static void func_op_astore_3() {
    u8 value = pop();

    executionFrame->local[3] = value;
    executionFrame->pc++;
}

static void func_op_iastore() {
    u8 index, value, aux;
    struct _array* arrayRef;

    value = pop();
    index = pop();
    aux = pop();

    memcpy(&arrayRef, &aux, sizeof(u8));

    ((uint32_t*)arrayRef->values)[index] = value;
    executionFrame->pc++;
}


static void func_op_lastore() {
    u8 index, low, high, aux;
    long value;
    struct _array* arrayRef;


    low = pop();
    high = pop();
    value = getLong(high, low);

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    ((u8*)arrayRef->values)[index] = value;
    executionFrame->pc++;
}

static void func_op_fastore() {
    u8 index, value, aux;
    struct _array* arrayRef;

    value = pop();
    index = pop();
    aux = pop();

    memcpy(&arrayRef, &aux, sizeof(u8));

    ((uint32_t*)arrayRef->values)[index] = value;
    executionFrame->pc++;
}

static void func_op_dastore() {
    u8 index, low, high, aux,value;
    struct _array* arrayRef;


    low = pop();
    high = pop();

    value = high;
    value <<= 32;
    value += low;

    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    ((u8*)arrayRef->values)[index] = value;
    executionFrame->pc++;
}

static void func_op_aastore() {
    u8 index,aux,value;
    struct _array* arrayRef;

    value = pop();
    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    ((u8*)arrayRef->values)[index] = value;
    executionFrame->pc++;
}

static void func_op_bastore() {
    u8 index,aux;
    uint8_t value;
    struct _array* arrayRef;

    value = pop();
    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    ((uint8_t*)arrayRef->values)[index] = (uint8_t)value;

    executionFrame->pc++;
}

static void func_op_castore() {
    u8 index,aux, value;
    struct _array* arrayRef;

    value = pop();
    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    ((uint16_t*)arrayRef->values)[index] = value;

    executionFrame->pc++;
}

static void func_op_sastore() {
    u8 index,aux;
    i8 value;
    struct _array* arrayRef;

    value = pop();
    index = pop();
    aux = pop();
    memcpy(&arrayRef, &aux, sizeof(u8));

    ((int16_t*)arrayRef->values)[index] = (int16_t)value;

    executionFrame->pc++;
}

static void func_op_pop() {
    pop();
    executionFrame->pc++;
}

static void func_op_pop2() {
    pop();
    pop();
    executionFrame->pc++;
}

static void func_op_dup() {
    u8 data;
    data = pop();
    push(data);
    push(data);
    executionFrame->pc++;
}

static void func_op_dup_x1() {
    u8 data,other;
    data = pop();
    other = pop();
    push(data);
    push(other);
    push(data);
    executionFrame->pc++;
}

static void func_op_dup_x2() {
    u8 data,other,last;
    data = pop();
    other = pop();
    last = pop();
    push(data);
    push(last);
    push(other);
    push(data);
    executionFrame->pc++;
}

static void func_op_dup2 () {
    u8 data,other;
    data = pop();
    other = pop();
    push(other);
    push(data);
    push(other);
    push(data);
    executionFrame->pc++;
}

static void func_op_dup2_x1() {
    u8 data,other,last;
    data = pop();
    other = pop();
    last = pop();
    push(other);
    push(data);
    push(last);
    push(other);
    push(data);
    executionFrame->pc++;
}

static void func_op_dup2_x2() {
    u8 data,other,middle,last;
    data = pop();
    other = pop();
    middle = pop();
    last = pop();
    push(other);
    push(data);
    push(last);
    push(middle);
    push(other);
    push(data);
    executionFrame->pc++;
}

static void func_op_swap() {
    u8 data,other;
    data = pop();
    other = pop();
    push(data);
    push(other);
    executionFrame->pc++;
}

static void func_op_iadd() {
    u8 op,opp;
    op  = pop();
    opp = pop();

    push(op+opp);
    executionFrame->pc++;

}

static void func_op_ladd() {

    u8 op,opp;
    u8 high,low;

    low  = pop();
    high = pop();

    op = high;
    op = op << 32;
    op = op + low;

    low  = pop();
    high = pop();

    opp = high;
    opp = opp << 32;
    opp = opp + low;

    push2(op+opp);
    executionFrame->pc++;

}

static void func_op_fadd() {

    float op, opp, sum;
    u8 value, other,result;

    value = pop();
    memcpy(&op, &value, sizeof(uint32_t));

    other = pop();
    memcpy(&opp, &other, sizeof(uint32_t));

    sum = op + opp;

    memcpy(&result, &sum, sizeof(uint32_t));
    push(result);

    executionFrame->pc++;
}

static void func_op_dadd() {

    u8 hiOp,loOp,hiOpp,loOpp;
    double sum;
    u8 completeValue = 0;

    loOp = pop();
    hiOp = pop();
    loOpp = pop();
    hiOpp = pop();

    sum = getDouble(hiOp,loOp) + getDouble(hiOpp,loOpp);
    memcpy(&completeValue, &sum, sizeof(u8));

    push2(completeValue);

    executionFrame->pc++;
}

static void func_op_isub() {
    u8 op,opp;
    op  = pop();
    opp = pop();
    push(opp-op);
    executionFrame->pc++;
}

static void func_op_lsub() {
    u8 op,opp;
    u8 high,low;

    low  = pop();
    high = pop();

    op = high;
    op = op << 32;
    op = op + low;

    low  = pop();
    high = pop();

    opp = high;
    opp = opp << 32;
    opp = opp + low;

    push2(opp-op);
    executionFrame->pc++;
}

static void func_op_fsub() {
    float op, opp, sum;
    u8 value, other,result;

    value = pop();
    memcpy(&op, &value, sizeof(uint32_t));

    other = pop();
    memcpy(&opp, &other, sizeof(uint32_t));

    sum = opp - op;

    memcpy(&result, &sum, sizeof(uint32_t));
    push(result);

    executionFrame->pc++;
}

static void func_op_dsub() {
    double op, opp, sum;
    u8 result;
    u8 high,low;

    low  = pop();
    high = pop();
    op = getDouble(high,low);

    low  = pop();
    high = pop();
    opp = getDouble(high,low);

    sum = opp - op;

    memcpy(&result, &sum, sizeof(u8));
    push2(result);

    executionFrame->pc++;
}

static void func_op_imul() {
    u8 op,opp;
    op  = (i8)pop();
    opp = (i8)pop();
    push(op*opp);
    executionFrame->pc++;
}

static void func_op_lmul() {

    i8 hop, lop, hopp, lopp;
    i8 op, opp;

    lop  = pop();
    hop  = pop();
    lopp = pop();
    hopp = pop();

    op  = hop;
    op  = op << 32;
    op  = op + lop;

    opp  = hopp;
    opp  = opp << 32;
    opp  = opp + lopp;

    push2((i8)(op*opp));
    executionFrame->pc++;

}

static void func_op_fmul() {
    float op, opp, mult;
    u8 value, other,result;

    value = pop();
    memcpy(&op, &value, sizeof(uint32_t));

    other = pop();
    memcpy(&opp, &other, sizeof(uint32_t));

    mult = op * opp;

    memcpy(&result, &mult, sizeof(uint32_t));
    push(result);

    executionFrame->pc++;
}

static void func_op_dmul() {
    i8 hop, lop, hopp, lopp;
    double op, opp, mult;
    u8 final;

    lop  = pop();
    hop  = pop();
    lopp = pop();
    hopp = pop();

    op  = getDouble(hop,lop);
    opp  = getDouble(hopp,lopp);
    mult = op*opp;

    memcpy(&final, &mult, sizeof(i8));
    push2((final));

    executionFrame->pc++;
}

static void func_op_idiv() {
    u8 op,opp;
    op  = (i8)pop();
    opp = (i8)pop();
    push(opp/op);
    executionFrame->pc++;
}

static void func_op_ldiv() {

    i8 hop, lop, hopp, lopp;
    i8 op, opp;

    lop  = pop();
    hop  = pop();
    lopp = pop();
    hopp = pop();

    op  = hop;
    op  = op << 32;
    op  = op + lop;

    opp  = hopp;
    opp  = opp << 32;
    opp  = opp + lopp;

    push2((i8)(opp/op));

    executionFrame->pc++;
}

static void func_op_fdiv() {
    float op, opp, mult;
    u8 value, other,result;

    value = pop();
    memcpy(&op, &value, sizeof(uint32_t));

    other = pop();
    memcpy(&opp, &other, sizeof(uint32_t));

    mult = opp/op;

    memcpy(&result, &mult, sizeof(uint32_t));
    push((u8)(result));

    executionFrame->pc++;
}

static void func_op_ddiv() {
    i8 hop, lop, hopp, lopp, final;
    double op, opp, div;

    lop  = pop();
    hop  = pop();
    lopp = pop();
    hopp = pop();

    op  = getDouble(hop,lop);
    opp  = getDouble(hopp,lopp);
    div = opp/op;

    memcpy(&final, &div, sizeof(i8));
    push2(final);

    executionFrame->pc++;
}

static void func_op_irem() {
    u8 op,opp;
    op  = pop();
    opp = pop();
    push(opp%op);
    executionFrame->pc++;
}

static void func_op_lrem() {
    u8 op,opp;
    u8 high,low;

    low  = pop();
    high = pop();

    op = high;
    op = op << 32;
    op = op + low;

    low  = pop();
    high = pop();

    opp = high;
    opp = opp << 32;
    opp = opp + low;

    push2(opp%op);
    executionFrame->pc++;
}

static void func_op_frem() {
    float op, opp, mod;
    u8 value2, value1, result;

    value2 = pop();
    memcpy(&op, &value2, sizeof(uint32_t));

    value1 = pop();
    memcpy(&opp, &value1, sizeof(uint32_t));

    mod = fmodf(opp , op);
    memcpy(&result, &mod, sizeof(uint32_t));

    push(result);
    executionFrame->pc++;

}

static void func_op_drem() {
    u8 hop, lop, hopp, lopp;
    double op, opp, mod;
    u8 final;

    lop  = pop();
    hop  = pop();
    lopp = pop();
    hopp = pop();

    op  = getDouble(hop,lop);
    opp  = getDouble(hopp,lopp);
    mod = fmod(opp,op);

    memcpy(&final, &mod, sizeof(u8));

    push2(final);
    executionFrame->pc++;
}

static void func_op_ineg() {
    i8 op;
    u8 aux = 0;

    op  = -pop();
    memcpy(&aux, &op, sizeof(u8));
    push(aux);

    executionFrame->pc++;
}

static void func_op_lneg() {
    u8 op;
    u8 high,low;

    low  = pop();
    high = pop();

    op = high;
    op = op << 32;
    op = op + low;

    op = -(op);

    push2(op);
    executionFrame->pc++;
}

static void func_op_fneg() {
    float op, negative;
    u8 value, result;

    value = pop();
    memcpy(&op, &value, sizeof(uint32_t));

    negative = -op;

    memcpy(&result, &negative, sizeof(uint32_t));
    push(result);

    executionFrame->pc++;
}

static void func_op_dneg() {
    double op, negative;
    u8 result;
    u8 high,low;

    low  = pop();
    high = pop();
    op = getDouble(high,low);

    negative = -op;

    memcpy(&result, &negative, sizeof(u8));
    push2(result);

    executionFrame->pc++;
}

static void func_op_ishl() {
    u8 lowsFive = 0x1f;
    u8 value1, value2;

    value2 = pop();
    value2 = value2 & lowsFive;

    value1 = pop();
    value1 = value1 << value2;

    push(value1);

    executionFrame->pc++;
}

static void func_op_lshl() {
    i8 bigBits;
    u8 lowSix = 0x3f;
    u8 low, high, result;

    result = pop();
    result = result & lowSix;


    low = pop();
    high = pop();



    bigBits = high;
    bigBits = bigBits << 32;
    bigBits = (signed)(bigBits + low);
    bigBits = bigBits << result;

    push2(bigBits);

    executionFrame->pc++;
}

static void func_op_ishr() {

    int32_t value1,lowFive = 0x1f;
    int32_t value2;

    value1 = pop();
    value1 = value1 & lowFive;
    value2 = (signed)(int32_t)pop();

    for(int j = 0; j < value1; j++) {
        value2 = value2 / 2;
    }

    push((i8)value2);
    executionFrame->pc++;
}

static void func_op_lshr() {

    u8 allOne = 0xffffffffffffffff, firstOne = 0x8000000000000000, var;
    u8 low, high, vartwo, lowSeven = 0x3f;;

    vartwo = pop();
    vartwo = vartwo & lowSeven;

    allOne = allOne << (64-vartwo);
    low = pop();
    high = pop();


    var = high;
    var = var << 32;
    var = (signed)(var + low);

    firstOne = var & firstOne;
    var = var >> vartwo;

    if(firstOne == 1) {
        var = var | allOne;
    }

    push2((u8)var);

    executionFrame->pc++;
}

static void func_op_iushr() {
    u8 lowFive = 0x1f;
    u8 value, other;

    other = pop();
    other = other & lowFive;
    value = pop();
    value = value >> other;
    push(value);
    executionFrame->pc++;
}

static void func_op_lushr() {
    i8 bigBits;
    u8 var, low, high, lowFive = 0x3f;

    var = pop();
    var = var & lowFive;

    low = pop();
    high = pop();

    bigBits = high;
    bigBits = bigBits << 32;
    bigBits = (signed)(bigBits + low);

    bigBits = bigBits >> var;
    push2((u8)bigBits);

    executionFrame->pc++;
}

static void func_op_iand() {
    u8 op,opp;
    op  = pop();
    opp = pop();
    push(op&opp);
    executionFrame->pc++;
}

static void func_op_land() {

    u8 op,opp;
    u8 high,low;

    low  = pop();
    high = pop();

    op = high;
    op = op << 32;
    op = op + low;

    low  = pop();
    high = pop();

    opp = high;
    opp = opp << 32;
    opp = opp + low;

    push2(op&opp);
    executionFrame->pc++;
}

static void func_op_ior() {
    u8 op,opp;
    op  = pop();
    opp = pop();
    push(op|opp);
    executionFrame->pc++;
}

static void func_op_lor() {
    u8 op,opp;
    u8 high,low;

    low  = pop();
    high = pop();

    op = high;
    op = op << 32;
    op = op + low;

    low  = pop();
    high = pop();

    opp = high;
    opp = opp << 32;
    opp = opp + low;

    push2(op|opp);
    executionFrame->pc++;
}

static void func_op_ixor() {
    u8 op,opp;
    op  = pop();
    opp = pop();
    push(op^opp);
    executionFrame->pc++;
}

static void func_op_lxor() {
    u8 op,opp;
    u8 high,low;

    low  = pop();
    high = pop();

    op = high;
    op = op << 32;
    op = op + low;

    low  = pop();
    high = pop();

    opp = high;
    opp = opp << 32;
    opp = opp + low;

    push2(op^opp);
    executionFrame->pc++;
}

static void func_op_iinc() {
    uint8_t field_index = executionFrame->code[++(executionFrame->pc)];
    u8 auxiliar = executionFrame->local[field_index];
    uint8_t auxiliar2 = executionFrame->code[++(executionFrame->pc)];
    int8_t index = (int8_t) auxiliar;
    int8_t constant = (int8_t) auxiliar2;


    index += constant;
    executionFrame->local[field_index] = (u8) index;
    executionFrame->pc++;
}

static void func_op_i2l() {
    u8 value1, value3, oneOne = 0x80000000;
    u8 bigBits,getHigh = 0xffffffff00000000;

    value1 = pop();
    value3 = value1 & oneOne;
    bigBits = (i8) value1;

    if(value3 == 1) {
        bigBits = bigBits | getHigh;
    }

    push2(bigBits);
    executionFrame->pc++;
}

static void func_op_i2f() {

    u8 value;
    u8 value2 = 0;

    float number;

    value = pop();

    number = (float)value;

    memcpy(&value2, &number, sizeof(float));
    push(value2);

    executionFrame->pc++;
}

static void func_op_i2d() {

    u8 value;
    u8 value2 = 0;

    double number;

    value = pop();

    number = (double)value;

    memcpy(&value2, &number, sizeof(double));
    push2(value2);

    executionFrame->pc++;
}

static void func_op_l2i() {
    u8 low;
    low = pop();
    pop();
    push(low);

    executionFrame->pc++;
}

static void func_op_l2f() {
    u8 low, high, toPush;
    long val;
    float number;

    low = pop();
    high = pop();

    val = getLong(high, low);
    number = (float)val;
    memcpy(&toPush, &number, sizeof(uint32_t));

    push(toPush);
    executionFrame->pc++;
}


static void func_op_l2d() {
    u8 low, high, toPush = 0;
    long val;
    double number;

    low = pop();
    high = pop();

    val = getLong(high, low);

    number = (double)val;
    memcpy(&toPush, &number, sizeof(u8));

    push2(toPush);
    executionFrame->pc++;
}

static void func_op_f2i() {
    u8 value;
    float number;

    value = pop();
    memcpy(&number, &value, sizeof(uint32_t));
    value = (u8) number;

    push(value);

    executionFrame->pc++;
}

static void func_op_f2l() {

    u8 value;
    u8 other;
    float number;

    value = pop();
    memcpy(&number, &value, sizeof(uint32_t));

    other = (u8) number;
    push2(other);

    executionFrame->pc++;
}

static void func_op_f2d() {

    u8 value;
    u8 other;

    double double_number;
    float float_number;

    value = pop();
    memcpy(&float_number, &value, sizeof(uint32_t));

    double_number = (double) float_number;
    memcpy(&other, &double_number, sizeof(u8));

    push2(other);

    executionFrame->pc++;
}

static void func_op_d2i() {

    i8 hop, lop;
    double op;
    u8 final;
    i8 int_number;

    lop  = pop();
    hop  = pop();

    op  = getDouble(hop,lop);

    int_number = (i8)op;

    memcpy(&final, &int_number, sizeof(i8));

    push((final));

    executionFrame->pc++;
}

static void func_op_d2l() {

    i8 hop, lop;
    double op;
    u8 final;
    long long_number;

    lop  = pop();
    hop  = pop();

    op  = getDouble(hop,lop);

    long_number = (long)op;

    memcpy(&final, &long_number, sizeof(i8));

    push2((final));

    executionFrame->pc++;

}

static void func_op_d2f() {

    i8 hop, lop;
    double op;
    uint32_t final;
    float float_number;

    lop  = pop();
    hop  = pop();

    op  = getDouble(hop,lop);

    float_number = (float) op;

    memcpy(&final, &float_number, sizeof(int32_t));
    push((final));

    executionFrame->pc++;
}

static void func_op_i2b() {

    int8_t value;
    i8 other;

    value = (int8_t) pop();
    other = (i8)value;
    push((i8)other);

    executionFrame->pc++;
}

static void func_op_i2c() {

    int16_t value;
    i8 other;

    value = (int16_t) pop();
    other = (i8)value;
    push((i8)other);

    executionFrame->pc++;
}

static void func_op_i2s() {

    int16_t value;
    i8 other;

    value = (int16_t) pop();
    other = (i8)value;

    push((u8)other);

    executionFrame->pc++;
}

static void func_op_lcmp() {

    i8 result;
    i8 low, high;
    i8 value, other;

    low = pop();
    high = pop();
    other = getLong(high, low);

    low = pop();
    high = pop();

    value = getLong(high, low);


    if(value == other) {
        result = 0;
    } else{
        if(value > other) {
            result = 1;
        } else {
            result = -1;
        }
    }

    push(result);
    executionFrame->pc++;
}

static void func_op_fcmpl() {

    i8 empilha;
    u8 value;
    float op, opp;

    value = pop();

    memcpy(&opp, &value, sizeof(uint32_t));
    value = pop();

    memcpy(&op, &value, sizeof(uint32_t));

    if(op == opp) {
        empilha = 0;
    }else{

        if(op > opp) {
            empilha = 1;
        }else {
            empilha = -1;
        }
    }

    push((u8) empilha);
    executionFrame->pc++;
}

static void func_op_fcmpg() {

    float value, other;
    u8 auxiliar;

    i8 resultado;

    auxiliar = pop();
    memcpy(&other, &auxiliar, sizeof(uint32_t));

    auxiliar = pop();
    memcpy(&value, &auxiliar, sizeof(uint32_t));

    if(value == other) {
        resultado = 0;
    } else{
        if(value > other) {
            resultado = 1;
        }else {
            resultado = -1;
        }
    }

    push((u8) resultado);
    executionFrame->pc++;
}

static void func_op_dcmpl() {
    double double_number, double_number2;
    u8 low, high;
    i8 empilha;

    low = pop();
    high = pop();

    double_number2 = getDouble(high, low);

    low = pop();
    high = pop();

    double_number = getDouble(high, low);

    if(double_number == double_number2) {
        empilha = 0;
    } else{
        if(double_number > double_number2) {
            empilha = 1;
        }else {
            empilha = -1;
        }
    }

    push((u8) empilha);
    executionFrame->pc++;
}

static void func_op_dcmpg() {
    double double_number, double_number2;
    u8 low, high;
    i8 empilha;

    low = pop();
    high = pop();

    double_number2 = getDouble(high, low);

    low = pop();
    high = pop();

    double_number = getDouble(high, low);

    if(double_number == double_number2) {
        empilha = 0;
    } else{
        if(double_number > double_number2) {
            empilha = 1;
        }else {
            empilha = -1;
        }
    }

    push((u8) empilha);
    executionFrame->pc++;

}

static void func_op_ifeq() {

    uint8_t pathOne, pathTwo;
    int16_t desloc;
    i8 value;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value = (signed) pop();
    if(value == 0) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }
}


static void func_op_ifne() {

    uint8_t pathOne, pathTwo;
    int16_t desloc;
    i8 value;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value = (signed) pop();
    if(value != 0) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }
}

static void func_op_iflt() {

    uint8_t pathOne, pathTwo;
    int16_t desloc;
    i8 value;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value = (signed) pop();
    if(value < 0) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }
}

static void func_op_ifge() {

    uint8_t pathOne, pathTwo;
    int16_t desloc;
    i8 value;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value = (signed) pop();
    if(value >= 0) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_ifgt() {

    uint8_t pathOne, pathTwo;
    int16_t desloc;
    i8 value;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value = (signed) pop();
    if(value > 0) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_ifle() {

    uint8_t pathOne, pathTwo;
    uint16_t desloc;
    i8 value;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value = (signed) pop();

    if(value <= 0) {
        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }
}

static void func_op_if_icmpeq() {

    int8_t pathOne, pathTwo;
    i8 value1, value2;
    int16_t desloc;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value2 = (signed) pop();
    value1 = (signed) pop();

    if(value1 == value2) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_if_icmpne() {

    int8_t pathOne, pathTwo;
    i8 value1, value2;
    int16_t desloc;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value2 = (signed) pop();
    value1 = (signed) pop();

    if(value1 != value2) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_if_icmplt() {

    int8_t pathOne, pathTwo;
    i8 value1, value2;
    int16_t desloc;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value2 = (signed) pop();
    value1 = (signed) pop();

    if(value1 < value2) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_if_icmpge() {

    int8_t pathOne, pathTwo;
    i8 value1, value2;
    int16_t desloc;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value2 = (signed) pop();
    value1 = (signed) pop();

    if(value1 >= value2) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_if_icmpgt() {

    int8_t pathOne, pathTwo;
    i8 value1, value2;
    int16_t desloc;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value2 = (signed) pop();
    value1 = (signed) pop();

    if(value1 > value2) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_if_icmple() {

    int8_t pathOne, pathTwo;
    i8 value1, value2;
    int16_t desloc;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value2 = (signed) pop();
    value1 = (signed) pop();

    if(value1 <= value2) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_if_acmpeg() {

    int8_t pathOne, pathTwo;
    i8 value1, value2;
    int16_t desloc;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value2 = (signed) pop();
    value1 = (signed) pop();

    if(value1 == value2) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_if_acmpne() {

    int8_t pathOne, pathTwo;
    i8 value1, value2;
    int16_t desloc;

    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    value2 = (signed) pop();
    value1 = (signed) pop();

    if(value1 != value2) {

        desloc = pathOne;
        desloc = desloc << 8;
        desloc = desloc | pathTwo;

        executionFrame->pc = executionFrame->pc + desloc;

    } else {
        executionFrame->pc = executionFrame->pc + 3;
    }

}

static void func_op_goto() {
    uint8_t pathOne, pathTwo;
    int16_t desloc;
    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    desloc = pathOne;
    desloc = desloc << 8;
    desloc = desloc | pathTwo;

    executionFrame->pc += desloc;
}

static void func_op_jsr() {

    uint8_t pathOne, pathTwo;
    int16_t desloc;

    push((executionFrame->pc) + 3);
    pathOne = executionFrame->code[(executionFrame->pc)+1];
    pathTwo = executionFrame->code[(executionFrame->pc)+2];

    desloc = pathOne;
    desloc = desloc << 8;
    desloc = desloc | pathTwo;

    executionFrame->pc = executionFrame->pc + desloc;

}

static void func_op_ret() {

    int16_t index;
    executionFrame->pc++;
    index = executionFrame->code[executionFrame->pc];

    if(wide == 1){
        index = index << 8;
        executionFrame->pc++;
        index = index | executionFrame->code[executionFrame->pc];
        wide = 0;
    }
    executionFrame->pc = executionFrame->local[index];
}

static void func_op_tableswitch() {

    int padrao, hi, lo, index, i ,j;
    int *tabelao;

    u8 locate,saltar, tableSize, desloc, bytes[5];

    index = pop();
    locate = executionFrame->pc;

    while((executionFrame->pc + 1) % 4 != 0)
        executionFrame->pc++;

    executionFrame->pc++;

    for(i = 0; i < 4; i++)
        bytes[i] = (u8)executionFrame->code[executionFrame->pc++];

    padrao = ( (bytes[0] & 0xFF) << 24) |((bytes[1] & 0xFF) << 16) |((bytes[2] & 0xFF) << 8) |(bytes[3] & 0xFF);


    for(i = 0; i < 4; i++)
        bytes[i] = (u8)executionFrame->code[executionFrame->pc++];

    lo = ((bytes[0] & 0xFF) << 24) |((bytes[1] & 0xFF) << 16) |((bytes[2] & 0xFF) << 8) |(bytes[3] & 0xFF);

    for(i = 0; i < 4; i++)
        bytes[i] = (u8)executionFrame->code[executionFrame->pc++];

    hi = ((bytes[0] & 0xFF) << 24) |((bytes[1] & 0xFF) << 16) |((bytes[2] & 0xFF) << 8) |(bytes[3] & 0xFF);

    tableSize = (hi-lo) + 1;
    tabelao = calloc(sizeof(u8), tableSize);


    for(i = 0; i < tableSize; i++) 	{

        for(j = 0; j < 4; j++) 	{
            bytes[j] = (u8)executionFrame->code[executionFrame->pc++];
        }

        tabelao[i] = ((bytes[0] & 0xFF) << 24) |((bytes[1] & 0xFF) << 16) |((bytes[2] & 0xFF) << 8) |(bytes[3] & 0xFF);
    }

    if(index > hi || index < lo ) {

        saltar = locate + padrao;

    } else {

        desloc = tabelao[index - lo];

        saltar = locate + desloc;
    }

    executionFrame->pc = saltar;

}


static void func_op_lookupswitch() {

    int padrao, index, i ,j, pairs,found;
    u8 locate,saltar, bytes[5];
    int32_t *match, *offset;

    index = pop();
    locate = executionFrame->pc;

    while((executionFrame->pc + 1) % 4 != 0)
        executionFrame->pc++;

    executionFrame->pc++;

    for(i = 0; i < 4; i++)
        bytes[i] = (u8)executionFrame->code[executionFrame->pc++];

    padrao = ( (bytes[0] & 0xFF) << 24) |((bytes[1] & 0xFF) << 16) |((bytes[2] & 0xFF) << 8) |(bytes[3] & 0xFF);


    for(i = 0; i < 4; i++)
        bytes[i] = (u8)executionFrame->code[executionFrame->pc++];

    pairs = ((bytes[0] & 0xFF) << 24) |((bytes[1] & 0xFF) << 16) |((bytes[2] & 0xFF) << 8) |(bytes[3] & 0xFF);


    match = calloc(sizeof(int32_t), pairs);
    offset = calloc(sizeof(int32_t), pairs);

    for(i = 0; i < pairs; i++) {

        for(j = 0; j < 4; j++) 	{
            bytes[j] = (u8)executionFrame->code[executionFrame->pc++];
        }

        match[i] = ((bytes[0] & 0xFF) << 24) |((bytes[1] & 0xFF) << 16) |((bytes[2] & 0xFF) << 8) | (bytes[3] & 0xFF);

        for(j = 0; j < 4; j++) 	{
            bytes[j] = (u8)executionFrame->code[executionFrame->pc++];
        }
        offset[i] = ((bytes[0] & 0xFF) << 24) | ((bytes[1] & 0xFF) << 16) |((bytes[2] & 0xFF) << 8) | (bytes[3] & 0xFF);
    }

    i = 0;
    found = 0;
    while((i < pairs) &&(!found)) {
        if(match[i] == index)
            found = 1;
        i++;
    }
    i--;

    if(found) {
        saltar = offset[i] + locate;
    } else {
        saltar = padrao + locate;
    }

    executionFrame->pc = saltar;

}


static void func_op_ireturn() {
    u8 aux = pop();

    deleteFrame();
    saiu = 1;

    if (executionFrame) {
        push(aux);
    }

}

static void func_op_lreturn() {
    u8 low = pop();
    u8 high = pop();

    deleteFrame();
    saiu = 1;

    if (executionFrame) {
        push2(getLong(high,low));
    }
}

static void func_op_freturn() {
    func_op_ireturn();
}

static void func_op_dreturn() {
    u8 low = pop();
    u8 high = pop();

    deleteFrame();
    saiu = 1;

    if (executionFrame) {

        push(high);
        push(low);
    }

}

static void func_op_areturn() {
    func_op_ireturn();
}

static void func_op_return() {
    deleteFrame();
    saiu = 1;
}

static void func_op_getstatic() {
    uint8_t index_1, index_2;
    uint16_t index, nameTypeIndex;
    //u8 classIndexTemp;
    int32_t classIndex, field_index;
    u8 valor;
    u1 *className, *name, *type;

    index_1 = (uint8_t) executionFrame->code[++(executionFrame->pc)];
    index_2 = (uint8_t) executionFrame->code[++(executionFrame->pc)];
    index = ((uint16_t)index_1 << 8) |(uint16_t)index_2;

    classIndex = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.class_index;
    className = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[classIndex-1].info.CONSTANT_Class_info.name_index);

    nameTypeIndex = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.name_and_type_index;
    name = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[nameTypeIndex-1].info.CONSTANT_NameAndType_info.name_index);
    type = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[nameTypeIndex-1].info.CONSTANT_NameAndType_info.descriptor_index);


    while((field_index = getFieldIndex(className, name, strlen((char*)name), type, strlen((char*)type))) == -1) {
        className = getParentName(getClassByName(className));
    }

    classIndex = loadClass(className);

    if(field_index < 0){
        field_index = searchStaticFieldVal(classIndex,name,type);
    }

    valor = getStaticFieldVal(classIndex , field_index);

    if(type[0] == 'J' || type[0] == 'D') {
        push2(valor);
    } else {
        push(valor);
    }

    executionFrame->pc++;
}

static void func_op_putstatic() {
    uint8_t index_1, index_2;
    uint16_t index, nameTypeIndex;
    u8 classIndexTemp;
    int32_t classIndex, field_index;
    u8 valor,valor2;
    u1 *className, *name, *type;

    index_1 = (uint8_t) executionFrame->code[++(executionFrame->pc)];
    index_2 = (uint8_t) executionFrame->code[++(executionFrame->pc)];
    index = ((uint16_t)index_1 << 8) |(uint16_t)index_2;

    if (!index) error(E_NOTVALID_CP_INDEX);

    classIndexTemp = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.class_index;
    className = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[classIndexTemp-1].info.CONSTANT_Class_info.name_index);

    nameTypeIndex = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.name_and_type_index;
    name = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[nameTypeIndex-1].info.CONSTANT_NameAndType_info.name_index);
    type = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[nameTypeIndex-1].info.CONSTANT_NameAndType_info.descriptor_index);

    while((field_index = getFieldIndex(className, name, strlen((char*)name), type, strlen((char*)type))) == -1) {
        className = getParentName(getClassByName(className));
    }

    classIndex = loadClass(className);

    if(type[0] == 'J' || type[0] == 'D') {
        valor  = pop();
        valor2 = pop();
        valor = valor | (valor2 << 32);
    } else {
        valor = pop();
    }

    setStaticFieldVal(classIndex , field_index, valor);
    executionFrame->pc++;
}


static void func_op_getfield() {
    uint8_t low, high;
    u8 index;
    i8 classIndex, field_index, aux;
    uint16_t nameTypeIndex;
    u1 *className, *name, *type;
    struct _object *objeto = NULL;
    u8 aux2;

    high = executionFrame->code[++(executionFrame->pc)];
    low = executionFrame->code[++(executionFrame->pc)];

    index = high;
    index <<= 8;
    index = index | low;

    if (!index) error(E_NOTVALID_CP_INDEX);

    classIndex = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.class_index;
    className = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[classIndex-1].info.CONSTANT_Class_info.name_index);


    nameTypeIndex = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.name_and_type_index;
    name = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[nameTypeIndex-1].info.CONSTANT_NameAndType_info.name_index);
    type = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[nameTypeIndex-1].info.CONSTANT_NameAndType_info.descriptor_index);


    while((field_index = getFieldIndex(className, name, strlen((char*)name), type, strlen((char*)type))) == -1) {
        className = getParentName(getClassByName(className));
    }

    aux = pop();
    memcpy(&objeto, &aux, sizeof(u8));

    aux2 = getObjectField(objeto, field_index);

    if(type[0] == 'J' || type[0] == 'D') {
        push2(aux2);
    } else {
        push(aux2);
    }

    executionFrame->pc++;
}

static void func_op_putfield() {

    uint8_t low, high;
    u8 index,aux;
    i8 classIndex, field_index, val_1;
    uint16_t nameTypeIndex;
    u1 *className, *name, *type;
    struct _object *objeto = NULL;
    u8 valor,valor2;

    high = executionFrame->code[++(executionFrame->pc)];
    low = executionFrame->code[++(executionFrame->pc)];

    index = high;
    index <<= 8;
    index = index | low;

    if (!index) error(E_NOTVALID_CP_INDEX);

    classIndex = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.class_index;
    className = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[classIndex-1].info.CONSTANT_Class_info.name_index);

    nameTypeIndex = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.name_and_type_index;
    name = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[nameTypeIndex-1].info.CONSTANT_NameAndType_info.name_index);
    type = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[nameTypeIndex-1].info.CONSTANT_NameAndType_info.descriptor_index);

    while((field_index = getFieldIndex(className, name, strlen((char*)name), type, strlen((char*)type))) == -1) {
        className = getParentName(getClassByName(className));
    }

    if(type[0] == 'J' || type[0] == 'D') {
        valor  = pop();
        valor2 = pop();
        valor = valor | (valor2 << 32);

        aux = pop();
        memcpy(&objeto, &aux, sizeof(u8));
        setObjectField(objeto, field_index, valor);

    } else {
        val_1 = pop();
        aux = pop();

        memcpy(&objeto, &aux, sizeof(u8));
        setObjectField(objeto, field_index, val_1);
    }

    executionFrame->pc++;
}

static void func_op_invokevirtual() {
    u8 index = 0, valorHigh = 0, valorLow = 0, vU8 = 0, array_ref = 0;
    uint8_t low = 0, high = 0;
    i8 numParams = 0, i = 0, j = 0;
    i8 classIndex = 0, classIndexTemp = 0;
    uint16_t nameTypeIndex = 0, methodNameIndex = 0, methodDescriptorIndex = 0;
    u1 *className = NULL, *methodName = NULL, *methodDesc = NULL;
    u8 *fieldsTemp = NULL;
    float vfloat = 0;
    ClassFile *class = NULL;
    method_info  *method = NULL;

    high = executionFrame->code[++(executionFrame->pc)];
    low = executionFrame->code[++(executionFrame->pc)];

    index = high;
    index <<= 8;
    index = index | low;

    if (!index) error(E_NOTVALID_CP_INDEX);

    classIndexTemp = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.class_index;
    className = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[classIndexTemp-1].info.CONSTANT_Class_info.name_index);
    nameTypeIndex = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.name_and_type_index;

    methodNameIndex = executionFrame->pClass->constant_pool[nameTypeIndex - 1].info.CONSTANT_NameAndType_info.name_index;
    methodDescriptorIndex = executionFrame->pClass->constant_pool[nameTypeIndex - 1].info.CONSTANT_NameAndType_info.descriptor_index;
    methodDesc = getUtf8String(executionFrame->pClass->constant_pool, methodDescriptorIndex);
    methodName = getUtf8String(executionFrame->pClass->constant_pool, methodNameIndex);

    if((strcmp((char*)className, "java/io/PrintStream") == 0) && ((strcmp((char*)methodName,"println") == 0) ||(strcmp((char*)methodName,"print") == 0))){

        //Quando tem que imprimir long
        if(strstr((char*)methodDesc, "J") != NULL){
            valorLow = pop();
            valorHigh = pop();

            printf("%ld",(long)getLong(valorHigh,valorLow));

            //Quando tem que imprimir double
        } else if(strstr((char*)methodDesc, "D") != NULL) {
            valorLow = pop();
            valorHigh = pop();

            printf("%f", (double)getDouble(valorHigh,valorLow));

            //Quando tem que imprimir boolean
        } else if(strstr((char*)methodDesc, "Z") != NULL) {

            if(!pop()) {
                printf("false");
            } else {
                printf("true");
            }

            //Quando tem que imprimir char
        } else if(strstr((char*)methodDesc, "C") != NULL) {

            //array
            if(strstr((char*)methodDesc, "[C") != NULL){
                array_ref = pop();
                for(i = 0; i < objHeap.array_count; i++){
                    if(!memcmp(&objHeap.arrays[i], &array_ref, sizeof(u8)))
                        break;
                }
                for(j = 0; j < objHeap.array_count; j++){
                    printf("%"PRIi16,(i2)(array_ref +i));
                }
                //CHAR
            } else {
                printf("%c",(int16_t)pop());
            }

            //Quando tem que imprimir inteiro
        }else if(strstr((char*)methodDesc, "I") != NULL) {
            printf("%d",(int)pop());

            //Quando tem que imprimir float
        }else if(strstr((char*)methodDesc, "F") != NULL) {
            vU8 = pop();
            memcpy(&vfloat, &vU8, sizeof(uint32_t));
            printf("%f", vfloat);

            //Quando tem que imprimir string
        }else if(strstr((char*)methodDesc, "Ljava/lang/String") != NULL) {
            vU8 = pop();
            printf("%s",executionFrame->pClass->constant_pool[vU8-1].info.CONSTANT_Utf8_info.bytes);

            //OBJECT
        }else if(strstr((char*)methodDesc, "Ljava/lang/Object") != NULL) {
            void* aux = NULL;
            vU8 = pop();
            memcpy(&aux, &vU8, sizeof(u8));
            printf("%p",aux);
        }

        if(strcmp((char*)methodName,"println") == 0) {
            printf("\n");
        }

        pop();
    } else {
        classIndex = loadClass(className);
        class = mHeap.classes[classIndex];

        while(class != NULL && (method = getMethodByNameDesc(class, executionFrame->pClass, nameTypeIndex)) == NULL) {
            className = getParentName(class);
            classIndex = loadClass(className);
            class = mHeap.classes[classIndex];
        }

        if(class == NULL) {
            printf("Erro: Metodo nao encontrando.\n");
            exit(1);
        }

        numParams = getNumParameters(class, method);
        fieldsTemp = calloc(sizeof(u8),numParams+1);
        for(i = numParams; i > 0; i--) {
            fieldsTemp[i] = pop();
        }

        if(((method->access_flags) & 0x0100) || strcmp("println", (char*)getUtf8String(class->constant_pool, method->name_index)) == 0) {
            pop();

            // implementar aqui codigo para lidar com metodos nativos

        } else {
            u8 objectref = pop();
            createFrame(method,class);
            for(i = numParams; i > 0; i--) {
                executionFrame->local[i] = fieldsTemp[i];
            }
            executionFrame->local[0] = objectref;
            Execute();
        }
    }

    executionFrame->pc++;
}

static void func_op_invokespecial() {
    u8 index, *fieldsTemp;;
    uint8_t low, high;
    i8 i, classIndex, classIndexTemp;
    uint16_t nameTypeIndex;
    u1 *className;
    ClassFile *class;
    method_info *method;
    int numParams;

    high = executionFrame->code[++(executionFrame->pc)];
    low = executionFrame->code[++(executionFrame->pc)];

    index = high;
    index <<= 8;
    index = index | low;

    if (!index) error(E_NOTVALID_CP_INDEX);

    classIndexTemp = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.class_index;
    className = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[classIndexTemp-1].info.CONSTANT_Class_info.name_index);

    nameTypeIndex = executionFrame->pClass->constant_pool[index-1].info.CONSTANT_FieldMethodIMethod_info.name_and_type_index;

    classIndex = loadClass(className);
    class = mHeap.classes[classIndex];

    while(class != NULL && (method = getMethodByNameDesc(class, executionFrame->pClass, nameTypeIndex)) == NULL) {
        className = getParentName(class);
        classIndex = loadClass(className);
        class = mHeap.classes[classIndex];
    }

    if(class == NULL) {
        printf("Metodo nao foi encontrando, veja.\n");
    }

    numParams = getNumParameters(class , method);


    fieldsTemp = calloc(sizeof(u8),numParams+1);
    for(i = numParams; i > 0; i--) {
        fieldsTemp[i] = pop();
    }

    if((method->access_flags) & 0x0100) {
        pop();

        // implementar aqui codigo para lidar com metodos nativos

    } else {
        u8 objectref = pop();
        createFrame(method,class);

        for(i = numParams; i > 0; i--) {
            executionFrame->local[i] = fieldsTemp[i];
        }

        executionFrame->local[0] = objectref;
        Execute();
    }

    executionFrame->pc++;

}

static void func_op_invokestatic() {
    u8 index, *fieldsTemp;
    uint8_t low, high;
    i8 numParams, i, classIndex, classIndexTemp;
    uint16_t nameTypeIndex;
    u1 *className;
    ClassFile *class;
    method_info *method;

    high = executionFrame->code[++(executionFrame->pc)];
    low = executionFrame->code[++(executionFrame->pc)];

    index = high;
    index <<= 8;
    index = index | low;
    if (!index) {
        error(E_NOTVALID_CP_INDEX);
    }

    classIndexTemp = executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_FieldMethodIMethod_info.class_index;
    className = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[classIndexTemp-1].info.CONSTANT_Class_info.name_index);
    nameTypeIndex = executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_FieldMethodIMethod_info.name_and_type_index;

    classIndex = loadClass(className);
    class = mHeap.classes[classIndex];

    method = getMethodByNameDesc(class, executionFrame->pClass, nameTypeIndex);
    numParams = getNumParameters(class , method);
    fieldsTemp = calloc(sizeof(u8),numParams+1);
    for(i = numParams - 1; i >= 0; i--) {
        fieldsTemp[i] = pop();
    }

    if((method->access_flags) & 0x0100) {

        // Não implementado

    } else {
        createFrame(method,class);
        for(i = numParams - 1; i >= 0; i--) {
            executionFrame->local[i] = fieldsTemp[i];
        }
        Execute();
    }
    executionFrame->pc++;
}

static void func_op_invokeinterface() {
    u8 index, *fieldsTemp;
    uint8_t low, high, args_count;
    i8 classIndex, classIndexTemp, i;
    uint16_t nameTypeIndex;
    u1 *className;
    ClassFile *class;
    method_info *method;

    high = executionFrame->code[++(executionFrame->pc)];
    low = executionFrame->code[++(executionFrame->pc)];

    index = high;
    index <<= 8;
    index = index | low;

    if (!index) {
        error(E_NOTVALID_CP_INDEX);
    }

    args_count = executionFrame->code[++(executionFrame->pc)];
    executionFrame->pc++;

    fieldsTemp = calloc(sizeof(u8),args_count+1);


    for(i = args_count; i > 0; i--) {
        fieldsTemp[i] = pop();
    }

    classIndexTemp = executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_FieldMethodIMethod_info.class_index;
    className = getUtf8String(executionFrame->pClass->constant_pool, executionFrame->pClass->constant_pool[classIndexTemp-1].info.CONSTANT_Class_info.name_index);
    nameTypeIndex = executionFrame->pClass->constant_pool[index - 1].info.CONSTANT_FieldMethodIMethod_info.name_and_type_index;

    classIndex = loadClass(className);
    class = mHeap.classes[classIndex];

    while(class != NULL && (method = getMethodByNameDesc(class, executionFrame->pClass, nameTypeIndex)) == NULL) {
        className = getParentName(class);
        classIndex = loadClass(className);
        class = mHeap.classes[classIndex];
    }

    if(class == NULL) {
        printf("Metodo nao encontrado.\n");
    }

    u8 objectref = pop();
    createFrame(method,class);
    for(i = args_count; i > 0; i--) {
        executionFrame->local[i] = fieldsTemp[i];
    }
    executionFrame->local[0] = objectref;
    Execute();
    executionFrame->pc++;

}

static void func_op_new() {
    // printf("\n\t\t\tentrou _new");
    u1 low, high;
    u8 index;
    u1 *className;
    i8 classIndex;

    ClassFile *pClass;
    struct _object *objeto;

    high = executionFrame->code[++(executionFrame->pc)];
    low = executionFrame->code[++(executionFrame->pc)];

    index = high;
    index <<= 8;
    index |= low;


    if (!index) {
        error(E_NOTVALID_CP_INDEX);
    }
    className = getClassNameUtf8(executionFrame->pClass, index);

    classIndex = loadClass(className);

    pClass = mHeap.classes[classIndex];

    objeto = newObject(pClass);


    push((u8)(intptr_t)objeto);
    executionFrame->pc++;
}

static void func_op_newarray() {
    int count;
    uint8_t type;
    count = pop();
    executionFrame->pc++;

    type = executionFrame->code[(executionFrame->pc)];
    if (count < 0) error(E_NEG_ARR_SIZE);

    push((u8)(intptr_t) newArray(count,type));
    executionFrame->pc++;
}

static void func_op_anewarray() {
    int count = pop();
    if (count < 0) error(E_NEG_ARR_SIZE);

    uint8_t index_byte1 = executionFrame->code[++executionFrame->pc];
    uint8_t index_byte2 = executionFrame->code[++executionFrame->pc];
    uint16_t index = index_byte1; index = index << 8 | index_byte2;

    u1 *className = getClassNameUtf8(executionFrame->pClass, index);

    void *pointer = newRefArray(count,(char*)className);

    push((u8)(intptr_t)pointer);
    executionFrame->pc++;
}

static void func_op_arraylength() {
    u8 reference = pop();
    if (reference == 0) error(E_NULL_POINTER);

    for(int i=0;i < objHeap.array_count; i++){

        // push somente a quantidade do array correto
        if(!memcmp(&objHeap.arrays[i], &reference, sizeof(struct _array*))) {
            push(objHeap.arrays[i]->quantidade);
            executionFrame->pc++;
            return;
        }
    }

    error(E_ARR_NOT_FOUND);
}

static void func_op_athrow() {
    executionFrame->pc++;
}

static void func_op_checkcast() {
    struct _object *reference;
    uint16_t index;

    executionFrame->pc++;
    index = executionFrame->code[(executionFrame->pc)];
    index = index << 8;
    executionFrame->pc++;
    index = index | executionFrame->code[(executionFrame->pc)];

    reference = (struct _object *)(intptr_t)pop();

    u1 *className1 = getClassNameUtf8(executionFrame->pClass, executionFrame->pClass->this_class);
    u1 *className2 = getClassNameUtf8(reference->class, index);

    if(reference == NULL) {
        printf("Erro: Null Reference\n");
    }
    if(strcmp((char*)className1, (char*)className2) == 0) {
        printf(" Erro: Wrong Typt Object\n");
    }
    push((u8)(intptr_t)reference);
    executionFrame->pc++;

}

static void func_op_instanceof() {

    struct _object *reference;
    uint16_t index;

    executionFrame->pc++;
    index = executionFrame->code[(executionFrame->pc)];
    index = index << 8;
    executionFrame->pc++;
    index = index | executionFrame->code[(executionFrame->pc)];
    reference = (struct _object *)(intptr_t)pop();

    if(reference == NULL) {
        printf("Erro: Null Reference\n");
    }

    u1 *className1 = getClassNameUtf8(executionFrame->pClass, executionFrame->pClass->this_class);
    u1 *className2 = getClassNameUtf8(reference->class, index);

    if(strcmp((char*)className1, (char*)className2) == 0) {
        push(1);
        executionFrame->pc++;
        return;
    }
    push(0);
    executionFrame->pc++;
}

static void func_op_monitorenter() {
    printf("monitorenter: Not supported instruction");
    exit(1);
}

static void func_op_monitorexit() {
    printf("monitorexit: Not supported instruction");
    exit(1);
}

static void func_op_wide() {
    wide = 1;
    executionFrame->pc++;
}

static void func_op_multianewarray() {
    // error(E_NOT_SUPPORTED_INSTRUCTION);
    uint16_t index = executionFrame->code[++executionFrame->pc];
    index = index << 8 | executionFrame->code[++executionFrame->pc];
    int dimensionCount = executionFrame->code[++executionFrame->pc];
    u1 *marrayInfo = getClassNameUtf8(executionFrame->pClass, index);

    // multianewarray apenas para arrays de dimensão >= 2
    if (dimensionCount < 1 || marrayInfo == NULL || (marrayInfo[0] != '[' && marrayInfo[1] != '[')) {
        error(E_NOT_VALID_MARRAY_INFO);
    }

    int* qtdByDimension = (int*)malloc(dimensionCount * sizeof(int));

    for (int i = 0; i < dimensionCount; i++) {
        int aux = pop();
        if (aux > 0) {
            qtdByDimension[i] = aux;
        }else {
            error(E_NEGATIVE_ARRAY_SIZE);
        }
    }

    uint32_t tipo = 0;
    switch(marrayInfo[dimensionCount]) {
        case 'L': // seta o tipo e carrega classe para method_area
            tipo = tREFERENCIA;
            u1 *className = (u1*) malloc(strlen((char*)marrayInfo)*sizeof(u1));
            for (int i = dimensionCount + 1; i < strlen((char*)marrayInfo); i++) {
                className[i - dimensionCount - 1] = marrayInfo[i];
                className[i - dimensionCount] = '\0';
            }
            loadClass(className);
            break;
        case 'Z':
            tipo = tBOOLEAN;
            break;
        case 'C':
            tipo = tCHAR;
            break;
        case 'F':
            tipo = tFLOAT;
            break;
        case 'D':
            tipo = tDOUBLE;
            break;
        case 'B':
            tipo = tBYTE;
            break;
        case 'S':
            tipo = tSHORT;
            break;
        case 'I':
            tipo = tINT;
            break;
        case 'J':
            tipo = tLONG;
            break;
        case '[':
            error(E_DISTINCT_MARRAY_DIMENSIONS);
            break;
        default:
            error(E_NOT_SUPPORTED_ARRAY_TYPE);
            break;
    }

    void *pointer = newMultiArray(0, dimensionCount, qtdByDimension, tipo);
    push((u8)(intptr_t)pointer);
    executionFrame->pc++;
}

static void func_op_ifnull() {
    uint8_t bb1, bb2;
    i8 auxiliar_32;
    u8 offset;

    bb1 = executionFrame->code[(executionFrame->pc)+1];
    bb2 = executionFrame->code[(executionFrame->pc)+2];
    auxiliar_32 = (signed) pop();

    if(auxiliar_32 == 0) {
        u8 value;
        value = bb1;
        value <<= 8;
        value |= bb2;
        offset = value;
        executionFrame->pc += offset;
    } else {
        executionFrame->pc += 3;
    }

}

static void func_op_ifnonnull() {
    uint8_t bb1, bb2;
    i8 auxiliar_32;
    int16_t offset;

    bb1 = executionFrame->code[(executionFrame->pc)+1];
    bb2 = executionFrame->code[(executionFrame->pc)+2];
    auxiliar_32 = (signed) pop();

    if(auxiliar_32 != 0){
        u8 value;
        value = bb1;
        value <<= 8;
        value |= bb2;
        offset = value;
        executionFrame->pc += offset;
    }else{
        executionFrame->pc += 3;
    }

}

static void func_op_goto_w() {
    u8 bb1, bb2, bb3, bb4;
    i8 offset;

    bb1 = executionFrame->code[(executionFrame->pc)+1];
    bb2 = executionFrame->code[(executionFrame->pc)+2];
    bb3 = executionFrame->code[(executionFrame->pc)+3];
    bb4 = executionFrame->code[(executionFrame->pc)+4];

    offset = (i8)(((bb1 & 0xFF)<<24) |((bb2 & 0xFF)<<16) |((bb3 & 0xFF)<<8) |(bb4));
    executionFrame->pc += offset;
}

static void func_op_jsr_w() {
    u8 bb1, bb2, bb3, bb4;
    i8 offset;

    push((executionFrame->pc)+5);

    bb1 = executionFrame->code[(executionFrame->pc)+1];
    bb2 = executionFrame->code[(executionFrame->pc)+2];
    bb3 = executionFrame->code[(executionFrame->pc)+3];
    bb4 = executionFrame->code[(executionFrame->pc)+4];

    offset = (i8)(((bb1 & 0xFF)<<24) |((bb2 & 0xFF)<<16) |((bb3 & 0xFF)<<8) |(bb4));
    executionFrame->pc += offset;
}


const Instruction instructions[] = {
    {0,     "nop", 				func_op_nop		},
    {0, 	"aconst_null", 		func_op_aconst_null},
    {0, 	"iconst_m1", 		func_op_iconst_m1	},
    {0, 	"iconst_0", 		func_op_iconst_0	},
    {0, 	"iconst_1",			func_op_iconst_1	},
    {0, 	"iconst_2",			func_op_iconst_2	},
    {0, 	"iconst_3",			func_op_iconst_3	},
    {0, 	"iconst_4",			func_op_iconst_4	},
    {0, 	"iconst_5",			func_op_iconst_5	},
    {0, 	"lconst_0",			func_op_lconst_0	},
    {0, 	"lconst_1",			func_op_lconst_1	},
    {0, 	"fconst_0",			func_op_fconst_0	},
    {0, 	"fconst_1",			func_op_fconst_1	},
    {0, 	"fconst_2",			func_op_fconst_2	},
    {0, 	"dconst_0",			func_op_dconst_0	},
    {0, 	"dconst_1",			func_op_dconst_1	},
    {1, 	"bipush",			func_op_bipush		},
    {2, 	"sipush",			func_op_sipush		},
    {1, 	"ldc",				func_op_ldc         },
    {2, 	"ldc_w",			func_op_ldc_w		},
    {2, 	"ldc2_w",			func_op_ldc2_w		},
    {1, 	"iload",			func_op_iload		},
    {1, 	"lload",			func_op_lload		},
    {1, 	"fload",			func_op_fload		},
    {1, 	"dload",			func_op_dload		},
    {1, 	"aload",			func_op_aload		},
    {0, 	"iload_0",			func_op_iload_0     },
    {0, 	"iload_1",			func_op_iload_1     },
    {0, 	"iload_2",			func_op_iload_2     },
    {0, 	"iload_3",			func_op_iload_3     },
    {0, 	"lload_0",			func_op_lload_0     },
    {0, 	"lload_1",			func_op_lload_1     },
    {0, 	"lload_2",			func_op_lload_2     },
    {0, 	"lload_3",			func_op_lload_3     },
    {0, 	"fload_0",			func_op_fload_0     },
    {0, 	"fload_1",			func_op_fload_1     },
    {0, 	"fload_2",			func_op_fload_2     },
    {0, 	"fload_3",			func_op_fload_3     },
    {0, 	"dload_0",			func_op_dload_0     },
    {0, 	"dload_1",			func_op_dload_1     },
    {0, 	"dload_2",			func_op_dload_2     },
    {0, 	"dload_3",			func_op_dload_3     },
    {0, 	"aload_0",			func_op_aload_0     },
    {0, 	"aload_1",			func_op_aload_1     },
    {0, 	"aload_2",			func_op_aload_2     },
    {0, 	"aload_3",			func_op_aload_3     },
    {0, 	"iaload",			func_op_iaload		},
    {0, 	"laload",			func_op_laload		},
    {0, 	"faload",			func_op_faload		},
    {0, 	"daload",			func_op_daload		},
    {0, 	"aaload",			func_op_aaload		},
    {0, 	"baload",			func_op_baload		},
    {0, 	"caload",			func_op_caload		},
    {0, 	"saload",			func_op_saload		},
    {1, 	"istore",			func_op_istore		},
    {1, 	"lstore",			func_op_lstore		},
    {1, 	"fstore",			func_op_fstore		},
    {1, 	"dstore",			func_op_dstore		},
    {1, 	"astore",			func_op_astore		},
    {0, 	"istore_0",			func_op_istore_0	},
    {0, 	"istore_1",			func_op_istore_1	},
    {0, 	"istore_2",			func_op_istore_2	},
    {0, 	"istore_3",			func_op_istore_3	},
    {0, 	"lstore_0",			func_op_lstore_0	},
    {0, 	"lstore_1",			func_op_lstore_1	},
    {0, 	"lstore_2",			func_op_lstore_2	},
    {0, 	"lstore_3",			func_op_lstore_3	},
    {0, 	"fstore_0",			func_op_fstore_0	},
    {0, 	"fstore_1",			func_op_fstore_1	},
    {0, 	"fstore_2",			func_op_fstore_2	},
    {0, 	"fstore_3",			func_op_fstore_3	},
    {0, 	"dstore_0",			func_op_dstore_0	},
    {0, 	"dstore_1",			func_op_dstore_1	},
    {0, 	"dstore_2",			func_op_dstore_2	},
    {0, 	"dstore_3",			func_op_dstore_3	},
    {0, 	"astore_0",			func_op_astore_0	},
    {0, 	"astore_1",			func_op_astore_1	},
    {0, 	"astore_2",			func_op_astore_2	},
    {0, 	"astore_3",			func_op_astore_3	},
    {0, 	"iastore",			func_op_iastore     },
    {0, 	"lastore",			func_op_lastore     },
    {0, 	"fastore",			func_op_fastore     },
    {0, 	"dastore",			func_op_dastore     },
    {0, 	"aastore",			func_op_aastore     },
    {0, 	"bastore",			func_op_bastore     },
    {0, 	"castore",			func_op_castore     },
    {0, 	"sastore",			func_op_sastore     },
    {0, 	"pop",				func_op_pop         },
    {0, 	"pop2",				func_op_pop2		},
    {0, 	"dup",				func_op_dup         },
    {0, 	"dup_x1",			func_op_dup_x1		},
    {0, 	"dup_x2",			func_op_dup_x2		},
    {0, 	"dup2",				func_op_dup2		},
    {0, 	"dup2_x1",			func_op_dup2_x1     },
    {0, 	"dup2_x2",			func_op_dup2_x2     },
    {0, 	"swap",				func_op_swap		},
    {0, 	"iadd",				func_op_iadd		},
    {0, 	"ladd",				func_op_ladd		},
    {0, 	"fadd",				func_op_fadd		},
    {0, 	"dadd",				func_op_dadd		},
    {0, 	"isub",				func_op_isub		},
    {0, 	"lsub",				func_op_lsub		},
    {0, 	"fsub",				func_op_fsub		},
    {0, 	"dsub",				func_op_dsub		},
    {0, 	"imul",				func_op_imul		},
    {0, 	"lmul",				func_op_lmul		},
    {0, 	"fmul",				func_op_fmul		},
    {0, 	"dmul",				func_op_dmul		},
    {0, 	"idiv",				func_op_idiv		},
    {0, 	"ldiv",				func_op_ldiv		},
    {0, 	"fdiv",				func_op_fdiv		},
    {0, 	"ddiv",				func_op_ddiv		},
    {0, 	"irem",				func_op_irem		},
    {0, 	"lrem",				func_op_lrem		},
    {0, 	"frem",				func_op_frem		},
    {0, 	"drem",				func_op_drem		},
    {0, 	"ineg",				func_op_ineg		},
    {0, 	"lneg",				func_op_lneg		},
    {0, 	"fneg",				func_op_fneg		},
    {0, 	"dneg",				func_op_dneg		},
    {0, 	"ishl",				func_op_ishl		},
    {0, 	"lshl",				func_op_lshl		},
    {0, 	"ishr",				func_op_ishr		},
    {0, 	"lshr",				func_op_lshr		},
    {0, 	"iushr",			func_op_iushr		},
    {0, 	"lushr",			func_op_lushr		},
    {0, 	"iand",				func_op_iand		},
    {0, 	"land",				func_op_land		},
    {0, 	"ior",				func_op_ior         },
    {0, 	"lor",				func_op_lor         },
    {0, 	"ixor",				func_op_ixor		},
    {0, 	"lxor",				func_op_lxor		},
    {2, 	"iinc",				func_op_iinc		},
    {0, 	"i2l",				func_op_i2l         },
    {0, 	"i2f",				func_op_i2f         },
    {0, 	"i2d",				func_op_i2d         },
    {0, 	"l2i",				func_op_l2i         },
    {0, 	"l2f",				func_op_l2f         },
    {0, 	"l2d",				func_op_l2d         },
    {0, 	"f2i",				func_op_f2i         },
    {0, 	"f2l",				func_op_f2l         },
    {0, 	"f2d",				func_op_f2d         },
    {0, 	"d2i",				func_op_d2i         },
    {0, 	"d2l",				func_op_d2l         },
    {0, 	"d2f",				func_op_d2f         },
    {0, 	"i2b",				func_op_i2b         },
    {0, 	"i2c",				func_op_i2c         },
    {0, 	"i2s",				func_op_i2s         },
    {0, 	"lcmp",				func_op_lcmp		},
    {0, 	"fcmpl",			func_op_fcmpl		},
    {0, 	"fcmpg",			func_op_fcmpg		},
    {0, 	"dcmpl",			func_op_dcmpl		},
    {0, 	"dcmpg",			func_op_dcmpg		},
    {2, 	"ifeq",				func_op_ifeq		},
    {2, 	"ifne",				func_op_ifne		},
    {2, 	"iflt",				func_op_iflt		},
    {2, 	"ifge",				func_op_ifge		},
    {2, 	"ifgt",				func_op_ifgt		},
    {2, 	"ifle",				func_op_ifle		},
    {2, 	"if_icmpeq",		func_op_if_icmpeq	},
    {2, 	"if_icmpne",		func_op_if_icmpne	},
    {2, 	"if_icmplt",		func_op_if_icmplt	},
    {2, 	"if_icmpge",		func_op_if_icmpge	},
    {2, 	"if_icmpgt",		func_op_if_icmpgt	},
    {2, 	"if_icmple",		func_op_if_icmple	},
    {2, 	"if_acmpeg",		func_op_if_acmpeg	},
    {2, 	"if_acmpne",		func_op_if_acmpne	},
    {2, 	"goto",				func_op_goto		},
    {2, 	"jsr",				func_op_jsr         },
    {1, 	"ret",				func_op_ret         },
    {14, 	"tableswitch",		func_op_tableswitch },
    {10,	"lookupswitch",		func_op_lookupswitch},
    {0, 	"ireturn",			func_op_ireturn     },
    {0, 	"lreturn",			func_op_lreturn     },
    {0, 	"freturn",			func_op_freturn     },
    {0, 	"dreturn",			func_op_dreturn     },
    {0, 	"areturn",			func_op_areturn     },
    {0, 	"return",			func_op_return		},
    {2, 	"getstatic",		func_op_getstatic	},
    {2, 	"putstatic",		func_op_putstatic	},
    {2, 	"getfield",			func_op_getfield	},
    {2, 	"putfield",			func_op_putfield	},
    {2, 	"invokevirtual",	func_op_invokevirtual},
    {2, 	"invokespecial",	func_op_invokespecial},
    {2, 	"invokestatic",		func_op_invokestatic},
    {4, 	"invokeinterface",	func_op_invokeinterface},
    {0, 	NULL,				NULL                },
    {2, 	"new",				func_op_new         },
    {1, 	"newarray",			func_op_newarray	},
    {2, 	"anewarray",		func_op_anewarray	},
    {0, 	"arraylength",		func_op_arraylength },
    {0, 	"athrow",			func_op_athrow		},
    {0, 	"checkcast",		func_op_checkcast	},
    {0, 	"instanceof",		func_op_instanceof	},
    {0, 	"monitorenter",		func_op_monitorenter},
    {0, 	"monitorexit",		func_op_monitorexit },
    {7, 	"wide",				func_op_wide		},
    {3, 	"multianewarray",	func_op_multianewarray},
    {2, 	"ifnull",			func_op_ifnull		},
    {2, 	"ifnonnull",		func_op_ifnonnull	},
    {4, 	"goto_w",			func_op_goto_w		},
    {4, 	"jsr_w",			func_op_jsr_w		}
};
