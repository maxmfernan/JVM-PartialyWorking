//
//  Frame.c
//
//
//  Created by Luiz Henrique Campos Barboza on 09/12/15.
//
//

#include "Frame.h"

extern Frame *executionFrame;

/*!
	Empilha um valor na pilha de operandos
 */

void push(u8 valor) {
    if (executionFrame->operand_stack.allocated >= executionFrame->max_stack) {
        error(E_STACK_OVERFLOW);
    }
    struct _u4pilha* ref = executionFrame->operand_stack.topo; // armazena referencia ao antigo topo

    executionFrame->operand_stack.topo = (struct _u4pilha*)malloc(sizeof(struct _u4pilha));
    executionFrame->operand_stack.topo->next = ref; // guarda referencia para o proximo topo
    executionFrame->operand_stack.topo->value = valor; // guarda o valor do topo
    executionFrame->operand_stack.allocated++;
}

/*!
	desempilha um valor na pilha de operandos
 */
u8 pop() {

    if (executionFrame->operand_stack.topo == NULL || !executionFrame->operand_stack.allocated) {
        error(E_EMPTY_STACK);
    }

    u8 toReturn = executionFrame->operand_stack.topo->value; // guarda valor do topo
    executionFrame->operand_stack.topo = executionFrame->operand_stack.topo->next;
    executionFrame->operand_stack.allocated--;

    return toReturn;
}
/*!
	empilha um valor na pilha de operandos
 */
void push2(u8 valor) {
    push(valor >> 32);
    push(valor & 0xffffffff);
}

/*!
	desempilha um valor na pilha de operandos 64 bits
 */
u8 pop2() {

    u8 var1,var2;
    u8 toReturn = 0;

    var1 = pop();
    var2 = pop();

    toReturn = var2;
    toReturn <<= 32;
    toReturn += var1;

    return toReturn;
}

/*!
	incicia e alococa um frame na memoria
 */
Frame *initFrame(ClassFile *Class, method_info *method, u2 codeIndex) {
    Frame *pFrame = (Frame *) malloc(sizeof(Frame));

    pFrame->code_length = method->attribute[codeIndex].info.Code_attribute.code_length;
    pFrame->code = method->attribute[codeIndex].info.Code_attribute.code;

    pFrame->max_stack = method->attribute[codeIndex].info.Code_attribute.max_stack;

    pFrame->max_locals = method->attribute[codeIndex].info.Code_attribute.max_locals;
    pFrame->local = (u8*) malloc(method->attribute[codeIndex].info.Code_attribute.max_locals*sizeof(u8));

    pFrame->operand_stack.allocated = 0;
    pFrame->operand_stack.topo = NULL;

    pFrame->pClass = Class;
    pFrame->pMethod = method;
    pFrame->pc = 0;
    pFrame->retornou = 0;

    return pFrame;
}
