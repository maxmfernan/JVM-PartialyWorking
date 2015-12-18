//
//  Frame.h
//
//
//  Created by Luiz Henrique Campos Barboza on 13/12/15.
//
//

// Frame
/// Contexto de operação da JVM: capítulo 3, seção 3.6 da especificação
/*!
 FRAME define a estrutura de um frame e provê funções para manuseio de um frame da JVM
 */
#ifndef FRAME_H
#define FRAME_H

#include "macros.h"


typedef struct {
    // campos

    ClassFile *pClass;
    method_info *pMethod;

    u4 code_length; //Número de bytes do array abaixo
    u1 *code; //Vetor com a qtd de byte indicada acima

    u2 max_stack;
    struct _operand_stack {
        u4 allocated;
        struct _u4pilha {
            u8 value;
            struct _u4pilha *next;
        } *topo;
    } operand_stack;
    u4 pc;

    u2 max_locals;
    u8* local;
    u2 retornou;

    // funcoes
} Frame;

void push(u8);
u8 pop ();
void push2(u8);
u8 pop2 ();

Frame *initFrame(ClassFile *Class, method_info *method, u2 codeIndex);

#endif
