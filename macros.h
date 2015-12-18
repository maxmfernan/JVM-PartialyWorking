/**
 *       @file  macros.h
 *      @brief  coleção de definições
 *
 * Detailed description starts here.
 *
 *     @author  Maxwell M. Fernandes (mmf), maxmfernan@gmail.com
 *
 *   @internal
 *     Created  07/11/2015
 *    Revision  $Id: doxygen.templates,v 1.3 2010/07/06 09:20:12 mehner Exp $
 *    Compiler  gcc/g++
 *     Company  none
 *   Copyright  Copyright (c) 2015, Maxwell M. Fernandes
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * =====================================================================================
 */

#ifndef MACROS_H
#define MACROS_H
#include <stdint.h>
#include "Exceptions.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/** Definição dos tipo com formato u#, onde # é um número */
typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;
typedef int32_t i4;
typedef int64_t i8;

typedef float f4;
typedef short i2;

#define LOINT64(I8) (u4)(I8 &0xFFFFFFFF)
#define HIINT64(I8) (u4)(I8 >> 32)
#define getu4(p) (u4)( (u4)((p)[0])<<24 & 0xFF000000 | (u4)((p)[1])<<16 & 0x00FF0000 | (u4)((p)[2])<<8 & 0x0000FF00| (u4)((p)[3]) & 0x000000FF)
#define getu2(p) (u2)((p)[0]<< 8 & 0x0000FF00 |(p)[1])

#define MAKEI8(i4high, i4low) (i8) (((i8)i4high) << 32 | (i8)i4low)

//getf4 depents on geti4 : carefull when changing...
#define geti4(p) (i4)( (u4)((p)[0])<<24 | (u4)((p)[1])<<16 | (u4)((p)[2])<<8 | (u4)((p)[3]) )
#define geti2(p) (i2)(((p)[0]<<8)|(p)[1])

f4 getf4(char *p);

#define castu4(p) *((u4 *)p)
#define castu2(p) *((u2 *)p)

#define casti4(p) *((i4 *)p)
#define casti2(p) *((i2 *)p)

/** Definição valores máximos */
#define CLSHEAP_MAX 50
#define OBJHEAP_MAX 50
#define STKFRAME_MAX 50


/** Definição de códigos de retorno */
#define SEEK_NOTFOUND 65535 // -1 se usar inteiro sinalizado, mas estamos usando u2

/** Definição dos nomes dos atributos */
#define CONSTANTVALUE_STR "ConstantValue"
#define SYNTHETIC_STR "Synthetic"
#define DEPRECATED_STR "Deprecates"

/** Definição das versões suportadas */
#define SUPPORTED_VERSION12 46


/** Definição de flags de acesso da estrutura .class */
#define ACC_PUBLIC 0x0001
#define ACC_FINAL 0x0010
#define ACC_SUPER 0x0020
#define ACC_INTERFACE 0x0200
#define ACC_ABSTRACT 0x0400
/** Definição de flags de cacesso da estrutura field */
#define ACC_PRIVATE 0x0002
#define ACC_PROTECTED 0x0004
#define ACC_STATIC 0x0008
#define ACC_VOLATILE 0x0040
#define ACC_TRANSIENT 0x0080
/** Definição de flags de acesso da estrutura method */
#define ACC_SYNCHRONIZED 0x0020
#define ACC_NATIVE 0x0100
#define ACC_STRICT 0x0800
/** Definição de flags de acesso da estrutura inner class */


/** Definição das tags para os elementos na constant poll */
#define CONSTANT_CLASS 7
#define CONSTANT_FIELDREF 9
#define CONSTANT_METHODREF 10
#define CONSTANT_INTERFACEMETHODREF 11
#define CONSTANT_STRING 8
#define CONSTANT_INTEGER 3
#define CONSTANT_FLOAT 4
#define CONSTANT_LONG 5
#define CONSTANT_DOUBLE 6
#define CONSTANT_NAMEANDTYPE 12
#define CONSTANT_UTF8 1
/** Definição das tags para os tipo de atributos. Não esixte na especificação */
#define SOURCEFILE 1
#define CONSTANTVALUE 2
#define CODE 3
#define EXCEPTIONS 4
#define INNERCLASSES 5
#define SYNTHETIC 6
#define LINENUMBERTABLE 7
#define LOCALVARIABLETABLE 8
#define DEPRECATED 9
/** Definição dos OPCODES */
#define op_nop  0
#define op_aconst_null 1
#define op_iconst_m1  2
#define op_iconst_0  3
#define op_iconst_1  4
#define op_iconst_2  5
#define op_iconst_3  6
#define op_iconst_4  7
#define op_iconst_5  8
#define op_lconst_0  9
#define op_lconst_1  10
#define op_fconst_0  11
#define op_fconst_1  12
#define op_fconst_2  13
#define op_dconst_0  14
#define op_dconst_1  15
#define op_bipush  16
#define op_sipush  17
#define op_ldc  18
#define op_ldc_w  19
#define op_ldc2_w  20
#define op_iload  21
#define op_lload  22
#define op_fload  23
#define op_dload  24
#define op_aload  25
#define op_iload_0  26
#define op_iload_1  27
#define op_iload_2  28
#define op_iload_3  29
#define op_lload_0  30
#define op_lload_1  31
#define op_lload_2  32
#define op_lload_3  33
#define op_fload_0  34
#define op_fload_1  35
#define op_fload_2  36
#define op_fload_3  37
#define op_dload_0  38
#define op_dload_1  39
#define op_dload_2  40
#define op_dload_3  41
#define op_aload_0  42
#define op_aload_1  43
#define op_aload_2  44
#define op_aload_3  45
#define op_iaload  46
#define op_laload  47
#define op_faload  48
#define op_daload  49
#define op_aaload  50
#define op_baload  51
#define op_caload  52
#define op_saload  53
#define op_istore  54
#define op_lstore  55
#define op_fstore  56
#define op_dstore  57
#define op_astore  58
#define op_istore_0  59
#define op_istore_1  60
#define op_istore_2  61
#define op_istore_3  62
#define op_lstore_0  63
#define op_lstore_1  64
#define op_lstore_2  65
#define op_lstore_3  66
#define op_fstore_0  67
#define op_fstore_1  68
#define op_fstore_2  69
#define op_fstore_3  70
#define op_dstore_0  71
#define op_dstore_1  72
#define op_dstore_2  73
#define op_dstore_3  74
#define op_astore_0  75
#define op_astore_1  76
#define op_astore_2  77
#define op_astore_3  78
#define op_iastore  79
#define op_lastore  80
#define op_fastore  81
#define op_dastore  82
#define op_aastore  83
#define op_bastore  84
#define op_castore  85
#define op_sastore  86
#define op_pop  87
#define op_pop2  88
#define op_dup  89
#define op_dup_x1  90
#define op_dup_x2  91
#define op_dup2  92
#define op_dup2_x1  93
#define op_dup2_x2  94
#define op_swap  95
#define op_iadd  96
#define op_ladd  97
#define op_fadd  98
#define op_dadd  99
#define op_isub  100
#define op_lsub  101
#define op_fsub  102
#define op_dsub  103
#define op_imul  104
#define op_lmul  105
#define op_fmul  106
#define op_dmul  107
#define op_idiv  108
#define op_ldiv  109
#define op_fdiv  110
#define op_ddiv  111
#define op_irem  112
#define op_lrem  113
#define op_frem  114
#define op_drem  115
#define op_ineg  116
#define op_lneg  117
#define op_fneg  118
#define op_dneg  119
#define op_ishl  120
#define op_lshl  121
#define op_ishr  122
#define op_lshr  123
#define op_iushr  124
#define op_lushr  125
#define op_iand  126
#define op_land  127
#define op_ior  128
#define op_lor  129
#define op_ixor  130
#define op_lxor  131
#define op_iinc  132
#define op_i2l  133
#define op_i2f  134
#define op_i2d  135
#define op_l2i  136
#define op_l2f  137
#define op_l2d  138
#define op_f2i  139
#define op_f2l  140
#define op_f2d  141
#define op_d2i  142
#define op_d2l  143
#define op_d2f  144
#define op_i2b  145
#define op_i2c  146
#define op_i2s  147
#define op_lcmp  148
#define op_fcmpl  149
#define op_fcmpg  150
#define op_dcmpl  151
#define op_dcmpg  152
#define op_ifeq  153
#define op_ifne  154
#define op_iflt  155
#define op_ifge  156
#define op_ifgt  157
#define op_ifle  158
#define op_if_icmpeq  159
#define op_if_icmpne  160
#define op_if_icmplt  161
#define op_if_icmpge  162
#define op_if_icmpgt  163
#define op_if_icmple  164
#define op_if_acmpeq  165
#define op_if_acmpne  166
#define op_goto  167
#define op_jsr  168
#define op_ret  169
#define op_tableswitch  170
#define op_lookupswitch  171
#define op_ireturn  172
#define op_lreturn  173
#define op_freturn  174
#define op_dreturn  175
#define op_areturn  176
#define op_return  177
#define op_getstatic  178
#define op_putstatic  179
#define op_getfield  180
#define op_putfield  181
#define op_invokevirtual  182
#define op_invokespecial  183
#define op_invokestatic  184
#define op_invokeinterface  185
#define op_xyzNotUsedxyz 186
#define op_new  187
#define op_newarray  188
#define op_anewarray  189
#define op_arraylength  190
#define op_athrow  191
#define op_checkcast  192
#define op_instanceof  193
#define op_monitorenter  194
#define op_monitorexit  195
#define op_wide  196
#define op_multianewarray  197
#define op_ifnull  198
#define op_ifnonnull  199
#define op_goto_w  200
#define op_jsr_w  201
#define op_breakpoint  202

/** Definição do array com os nomes dos opcodes */
extern char *opcodes_str_names[];

/** Definição dos códigos numéricos para os atributos */
//#define SOURCEFILE 0x436
//#define CONSTANTVALUE
//#define CODE
//#define EXCEPTIONS
//#define INNERCLASSES
//#define SYNTHETIC
//#define LINENUMBERTABLE
//#define LOCALVARIABLETABLE
//#define DEPRECATED


/**Definição dos códigos para o módulo de mensagens de erro  */
#define NOTACLASS_ERR 2
#define INCORRECTVERSION_ERR 1
#define E_SUCCESS  0
#define E_INVALID_NUM_ARGS  -1
#define E_ARGV1_FILE_NOT_EXISTENT  -2
#define E_CAFEBABE  -3
#define E_VERSION  -4
#define W_NAOLIDOINTEIRO  -5
#define E_OPCAO_NAO_EXISTENTE  -6
#define E_DOLAR_NOT_SUPPORTED  -7
#define E_EMPTY_FRAME_STACK  -8
#define E_STACK_OVERFLOW  -9
#define E_NO_MAIN_FOUND  -10
#define E_EMPTY_STACK  -11
#define E_NOTSUPPORTED_VERSION  -12
#define E_NOTVALID_CP_INDEX  -13
#define E_NEG_ARR_SIZE  -14
#define E_NULL_POINTER  -15
#define E_ARR_NOT_FOUND  -16
#define E_NOT_VALID_MARRAY_INFO  -17
#define E_NOT_SUPPORTED_ARRAY_TYPE  -18
#define E_DISTINCT_MARRAY_DIMENSIONS  -19
#define E_NOT_SUPPORTED_INSTRUCTION  -20
#define E_BAD_INPUT  -21
#define E_NEGATIVE_ARRAY_SIZE  -22




//typedef struct attribute_info_e attribute_info; //foward declaration

/** Definição das estruturas relacionadas as áreas de memória da JVM (Heap, stack) */
typedef struct{
	u2 clsHeap_size;
	u2 objHeap_size;
	u2 stkHeap_size;
}dataMSize_t;

/** Definição das estruturas de todos os componentes da constant poll */
/** Nessas estruturas não foram inseridos os campos de tag. Eles já existem no cp_info */

//
/** Definição da estrutura que representa elementos na constant poll */
typedef struct{
	u1 tag;

	union{
		struct{
			u2 name_index;
		}CONSTANT_Class_info;
		struct{ //cobre 3 tipos de estruturas
			u2 class_index;
			u2 name_and_type_index;
		}CONSTANT_FieldMethodIMethod_info; //serve para field, method ou interface method
        struct{
            u2 string_index;
        }CONSTANT_String_info;
		struct{ //cobre 2 tipos de estrutura
			u4 bytes;
		}CONSTANT_IntegerFloat_info; //serve para integer e float
		struct{ //cobre 2 tipoe de estutura
			u4 high_bytes;
			u4 low_bytes;
		}CONSTANT_LongDouble_info; //serve para long e double
		struct{
			u2 name_index;
			u2 descriptor_index;
		}CONSTANT_NameAndType_info;
		struct{
			u2 length;
			u1 *bytes; //Tamanho especificado por length
		}CONSTANT_Utf8_info;
	}info; //Anonymous unions isn't part of c99
}cp_info;

/**Definição das estrutura das interfaces na tabela de interface*/
typedef struct {
	u2 name_index;
} CONSTANT_Class_info;


/** Definição das estruturas de todos os possíveis atributos */
/** Estrutura para as execeções dentro do atributo code */
typedef struct{
		u2 start_pc;
		u2 end_pc;
		u2 handler_pc;
		u2 catch_type;
	}ExceptionForCode;
//
/** Estrutura para classes dentro do atributo InnerClasses */
typedef struct{
	u2 inner_class_info_index;
	u2 outer_class_info_index;
	u2 inner_name_index;
	u2 inner_class_access_flags;
}ClassesForIClasses;
//
/** Estrutura para número de linha dentro do atributo line number */
typedef struct{
	u2 start_pc;
	u2 line_number; //Número de linha no arquivo fonte
}LineNumberForLNumber;
//
/** Estrutura para variáveis locais dentro do atributo Local Variable */
typedef struct{
	u2 start_pc;
	u2 length;
	u2 name_index;
	u2 descriptor_index;
	u2 index;
}LocalVariableForVariable;
//
/** Definição da estrutura attribute */
typedef struct attribute_info_e{
	u2 attribute_name_index;
	u4 attribute_length; //Número em bytes da informação seguinte
	u1 tag; //Pra saber o tipo de atributo aqui
	union{
		struct{
		//	u2 attribute_name_index;
		//	u4 attribute_length;
			u2 constantvalue_index;
		}ConstantValue_attribute;
		struct{ //Possivelmente contem componentes flexíveis
		//	u2 attribute_name_index;
		//	u4 attribute_length;
			u2 max_stack;
			u2 max_locals;
			u4 code_length; //Número de bytes do array abaixo
			u1 *code; //Vetor com a qtd de byte indicada acima
			u2 exception_table_length; //Número de entradas e não de bytes da exception_table
			ExceptionForCode *exception_table; //A qtd de elementos é indicada pela variável acima.
			u2 attribute_count; //Número de atributos dentro desse atributo
			struct attribute_info_e* attribute; //Vetor de atributos
		}Code_attribute;
		struct{
		//	u2 attribute_name_index;
		//	u4 attribute_lenght; //Indica a qtd de byte do atributo excluindo os 6bytes inici.
			u2 number_of_exceptions; //Número de entradas para o vetor abaixo.
			u2 *exception_index_table; //Vetor imcompleto
		}Exception_attribute;
		struct{
		//	u2 attribute_name_index;
		//	u4 attribute_length; //Tamanho em byte do atributo excluindo os 6 bytes iniciais
			u2 number_of_classes; //Número das estruturas abaixo.
			ClassesForIClasses *classes;
		}InnerClasses_attribute;
		struct{
		//	u2 attribute_name_index;
		//	u4 attribute_length; //Segundo as especificações da JVM esse valor é 0
		}Synthetic_attribute;
		struct{
		//	u2 attribute_name_index;
		//	u4 attribute_length; //Segundo as especificações da JVM esse valor deve ser 2
			u2 sourcefile_index;
		}SourceFile_attribute;
		struct{
		//	u2 attribute_name_index;
		//	u4 attribute_length; //Valor em bytes do atributo excluindo os 6bytes inic.
			u2 line_number_table_length;
			LineNumberForLNumber *line_number_table;
		}LineNumberTable_attribute;
		struct{
		//	u2 attribute_name_index;
		//	u4 attribute_length;
			u2 local_variable_length;
			LocalVariableForVariable *local_variable_table; //Tabela de variáveis locais
		}LocalVariableTable_attribute;
		struct{
		//	u2 attribute_name_index;
		//	u4 attribute_length;
		}Deprecated_attribute;
	}info; //Um tipo específico de atributo
}attribute_info;

/** Definição da estrutura method */
typedef struct{
	u2 access_flags;
	u2 name_index;
	u2 descriptor_index;
	u2 attributes_count;
	attribute_info *attribute; //A quantidade é indicada por attributes_count
}method_info;
/*
struct method_info_ex: method_info
{
    method_info *pMethodInfoBase;
    Code_attribute *pCode_attr;
};
*/
/** Definição da estutura field */
typedef struct{
	u2 access_flag;
	u2 name_index;
	u2 descriptor_index;
	u2 attributes_count; //Indica o número de atributos
    u8 value;
	attribute_info *attributes; //Tabela de attributos
}field_info;

/*
union {
    u4 type;
    u1 charValue;
    u2 shortValue;
    u4 intValue;
    u1 *variable_name;
    //f4 floatValue;

    //LONG_PTR ptrValue;
    Object object;
} Variable;
*/

typedef struct{
    u1 *field_name; //UTF8
    u1 *descriptor;
    union {
        struct {
            u4 value;
        } I4;
        struct {
            u4 high;
            u4 low;
        } U8;
        struct {
            u1 *bytes;
        } UTF8;
    }info;
}Field_Value;


/** Estrutura do arquivo .class */
typedef struct {
	u4 magic;
	u2 minor_version;
	u2 major_version;
	u2 constant_pool_count;
	cp_info *constant_pool; //Tem o tamanho constant_pool_count -1
	u2 access_flags;
	u2 this_class;
	u2 super_class;
	u2 interfaces_count;
	u2 *interfaces;
	u2 fields_count;
	field_info *fields;
	u2 methods_count;
	method_info *methods;
	u2 attributes_count;
	attribute_info *attributes;

    //Variable *variable;
    //field_info *static_fields;

    u2 object_fields_size;
    field_info *object_fields;

    u2 static_values_size;
    Field_Value *static_values;

    u1 *className;


}ClassFile;





/*!
 Enumeração dos possíveis tipos definidos pela JVM para newArray
	*/
enum Array_Tipo {
    tREFERENCIA = 0,
    tBOOLEAN = 4,
    tCHAR = 5,
    tFLOAT = 6,
    tDOUBLE	= 7,
    tBYTE = 8,
    tSHORT = 9,
    tINT = 10,
    tLONG = 11
};

/*!
 Enumeração do tamanho em bytes dos tipos definidos pela JVM
	*/
enum _JVM_TIPO_SIZE {
    tREFERENCIA_SIZE = 4,
    tBOOLEAN_SIZE = 1,
    tCHAR_SIZE = 2,
    tFLOAT_SIZE = 4,
    tDOUBLE_SIZE = 8,
    tBYTE_SIZE = 1,
    tSHORT_SIZE = 2,
    tINT_SIZE = 4,
    tLONG_SIZE = 8
};

#endif
