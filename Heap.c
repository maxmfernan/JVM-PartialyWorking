//
//  Heap.c
//
//
//  Created by Luiz Henrique Campos Barboza on 15/12/15.
//
//
#include "Execution_Core.h"
#include "Heap.h"

extern MethodHeap mHeap;
extern Heap objHeap;

/*!
	adiciona um objeto no array de objetos para um possível Garbage Collector
 */
struct _object* addObject(struct _object* obj) {
    objHeap.objects = realloc(objHeap.objects, (objHeap.object_count+1)*sizeof(struct _object*));
    objHeap.objects[objHeap.object_count++] = obj;
    return obj;
}

/*!
	adiciona um array na no array de arrays para um possível Garbage Collector
 */
struct _array* addArray(struct _array* arr) {
    objHeap.arrays = realloc(objHeap.arrays, (objHeap.array_count+1)*sizeof(struct _array*));
    objHeap.arrays[objHeap.array_count++] = arr;
    return arr;
}

/*!
	cria um novo objeto, alocando espaco na memoria para ele
 */
struct _object* newObject(ClassFile *class) {
    if (!class) return NULL;

    struct _object* newObj = (struct _object*)malloc(sizeof(struct _object));
    newObj->class = class;
    newObj->fields = (uint64_t*)malloc(class->fields_count*sizeof(uint64_t));

    int index = loadClass(getParentName(class));

    if(index > -1 ){
        ClassFile *parentClass = mHeap.classes[index];
        newObj->super = newObject(parentClass);
    }else{
        return NULL;
    }

    return addObject(newObj);
}

/*!
	devolve uma instancia de um array, do tipo passado por parametro
 */
struct _array* newArray(uint32_t count, uint32_t tipo) {
    struct _array* newArr = (struct _array*)malloc(sizeof(struct _array));

    newArr->quantidade = count;
    newArr->tipo = tipo;
    switch(tipo) {
        case tREFERENCIA:
            newArr->element_size = sizeof(uint64_t);
            break;
        case tBOOLEAN:
            newArr->element_size = sizeof(uint8_t);
            break;
        case tCHAR:
            newArr->element_size = sizeof(uint16_t);
            break;
        case tFLOAT:
            newArr->element_size = sizeof(uint32_t);
            break;
        case tDOUBLE:
            newArr->element_size = sizeof(uint64_t);
            break;
        case tBYTE:
            newArr->element_size = sizeof(uint8_t);
            break;
        case tSHORT:
            newArr->element_size = sizeof(uint16_t);
            break;
        case tINT:
            newArr->element_size = sizeof(uint32_t);
            break;
        case tLONG:
            newArr->element_size = sizeof(uint64_t);
            break;
        default:
            break;
    }
    newArr->values = calloc(count, newArr->element_size);

    return addArray(newArr);
}

/*!
	Cria uma instancia de array de referencia
 */
struct _array* newRefArray(uint32_t count, char* className) {
    struct _array* newArr = newArray(count, tREFERENCIA);

    loadClass((u1*)className);

    return newArr;
}

/*!
	A partir da dimension 0, para cada dimension de um array de multiplas dimensions,
	intancia um novo array para cada campo valor disponível na dimension indicada por $dimension.
	A instância é dada de forma recursiva, e a condição de parada é quando o número da dimensão for igual ao número total de dimensões do array.
	@param dimension: dimensão atual
	@param dimensionCount: número total de dimensions
	@param qtdByDimension: tamanho do array de cada dimensão
	@param tipo: o tipo do array
	@return um array de múltiplas dimensões
 */
struct _array* newMultiArray(int dimension, int dimensionCount, int* qtdByDimension, uint32_t tipo){
    if (dimension == dimensionCount - 1) {
        return newArray(qtdByDimension[dimension], tipo);
    } else if (dimension < dimensionCount) {
        struct _array* toReturn = newArray(qtdByDimension[dimension], tipo);
        for (int i = 0; i < qtdByDimension[dimension]; i++) {
            toReturn->values[i] = (uint64_t)(intptr_t)newMultiArray(dimension+1, dimensionCount, qtdByDimension, tipo);
        }
        return toReturn;
    } else {
        error(E_BAD_INPUT);
        return NULL;
    }
}

/*!
	incicia e aloca o heap inicial na memoria
 */
Heap initHeap() {
    Heap toReturn; // = (Heap*)malloc(sizeof(Heap));

    // campos
    toReturn.object_count = 0;
    toReturn.array_count = 0;
    toReturn.objects = (struct _object**)malloc(sizeof(struct _object*));
    toReturn.arrays = (struct _array**)malloc(sizeof(struct _array*));
    return toReturn;
}
