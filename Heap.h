//
//  Heap.h
//
//
//  Created by Luiz Henrique Campos Barboza on 15/12/15.
//
//

#ifndef Heap_h
#define Heap_h

#include "macros.h"


// Heap
/// Área de armazenamento de objetos e arrays da JVM
/*!
 Heap define a estutura de objetos e arrays;
*/
typedef struct {
    // estrutura
    struct _object {
        ClassFile *class;
        struct _object *super;
            u8* fields;
        }** objects;

    struct _array {
        u4 quantidade;
        u4 tipo;
        u4 element_size;
        u8* values;
    }** arrays;

    u4 object_count;
    u4 array_count;

    // funções
} Heap;

Heap initHeap();
struct _object *newObject(ClassFile *);
struct _array *newArray(u4, u4);
struct _array *newRefArray(u4, char *);
struct _array *newMultiArray(int, int, int*, u4);


#endif /* Heap_h */
