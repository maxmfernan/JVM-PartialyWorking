#include "LoadClass_core.h"
#include "LoadClass_ui.h"
#include "macros.h"
#include "Execution_Core.h"
#include "Frame.h"
#include "Heap.h"
/**
* @brief
*
* @param classPathStr
* @param classHeap_ptr
* @param dmSize
*/

extern dataMSize_t dmSize;
extern ClassFile *classHeap;
extern Frame *stackFrame;
extern Heap objHeap;

ClassFile *classLoader(char *classPathStr) {
	ClassFile *classHeap_ptr;

    FILE *classPathF_ptr = fopen(classPathStr, "rb");
    if( classPathF_ptr == NULL){
    	printf("\nArquivo não encontrado: %s\n", classPathStr);
    	exit(1);
    }

	classHeap_ptr = classHeap + dmSize.clsHeap_size * sizeof( ClassFile );// seto a posição correta
	if( classHeap_ptr < ( classHeap + CLSHEAP_MAX * sizeof( ClassFile ) ) ){
		dmSize.clsHeap_size++;// incremento a qtd de elementos na heap
	}
	else{
		printf("\nClassHeap está cheio\n");
		exit(1);
	}

	//FILE *arq;
    //u1 i;
	//u1 counter = 0;
	u4 magic;
	u2 minor;
	u2 major;
	u2 poolLength = 0;
	u2 poolElementsNum = 0;
	cp_info *pool;
    //u2 generic_count = 0;

    unsigned int field_offset = 0;

    //ClassFile classFileVar;

    u2 attr_count = 0;
    attribute_info *attributes;

	u2 inter_length = 0;
	u2* interface;
	u2 interface_offset = 0;

	//TESTE: retirar depois
	//strcpy (classPathStr, "HelloWorld.class");

	//arq = fopen(classPathStr, "rb");
	magic = read_magic(classPathF_ptr);
    if( magic != 0xcafebabe)
        error(NOTACLASS_ERR);//age apropriadamente

	minor = read_minor_version(classPathF_ptr);
	major = read_major_version(classPathF_ptr);
    if( major > 0x2e )
        error(INCORRECTVERSION_ERR);//age apropriadamente

	if (classPathF_ptr != NULL) {
		pool = createConstantPool (&poolElementsNum, &poolLength, classPathF_ptr);
	}
	else {
		printf ("Erro de leitura do arquivo!\n");
	}
    /*De magic até major*/
    classHeap_ptr->magic = magic;
    classHeap_ptr->minor_version = minor;
    classHeap_ptr->major_version = major;

    /*Constant pool*/
    classHeap_ptr->constant_pool_count = poolElementsNum; // número de elementos da pool de constantes
   	classHeap_ptr->constant_pool = pool;


    /*De flag de acesso até super class*/
    /*DUVIDA: esse read_u2 não tinha antes, como que essa parte estava sendo lida?*/
    classHeap_ptr->access_flags = read_u2(classPathF_ptr);
   	classHeap_ptr->this_class = read_u2(classPathF_ptr);
   	classHeap_ptr->super_class = read_u2(classPathF_ptr);
	
    /*Interface*/
	interface = interface_decode(classPathF_ptr, &inter_length, &poolLength, &interface_offset);
   	classHeap_ptr->interfaces_count = inter_length;
   	classHeap_ptr->interfaces = interface;

    /*Fields*/
    field_info* fields_array;
    unsigned int fieldn = 0;
    fields_array = field_func(classPathF_ptr, 0, &field_offset, pool, &fieldn);
    classHeap_ptr->fields_count = fieldn;
   	classHeap_ptr->fields = fields_array;

    /*Methods*/
    read_Methods(classHeap_ptr, classPathF_ptr, pool); //Essa função já preenche a estrutura ClassFile
    //u2 methods_count;
   	//method_info *methods;

    /*Atributes*/
    attributes = createAttributeInfo(classPathF_ptr, pool, &attr_count);
    classHeap_ptr->attributes_count = attr_count;
   	classHeap_ptr->attributes = attributes;

    classHeap_ptr->className = pool[pool[classHeap_ptr->this_class -1].info.CONSTANT_Class_info.name_index - 1].info.CONSTANT_Utf8_info.bytes;

   	//Quem abriu fecha.
	//fclose(classPathF_ptr);

	fillStaticFields(classHeap_ptr);
	classHeap[dmSize.clsHeap_size - 1] = *classHeap_ptr;
    
	return classHeap_ptr;

}
