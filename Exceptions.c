#include "macros.h"
//#include <unistd.h>
#include <stdio.h>

void error(int errorCode){
    switch( errorCode ){
        case NOTACLASS_ERR:
            printf("\n------------------------");
            printf("\nErro:");
            printf("\nO arquivo lido não foi reconhecido como um como um .class adequado");
            printf("\nSaindo...");
            printf("\n");
            //sleep(2);
            exit(1);
        case INCORRECTVERSION_ERR:
            printf("\n------------------------");
            printf("\nErro:");
            printf("\nVersão maior que 1.2.");
            printf("\nSaindo...");
            printf("\n");
            //sleep(2);
            exit(1);
        case E_SUCCESS:
        case E_INVALID_NUM_ARGS:
        case E_ARGV1_FILE_NOT_EXISTENT:
        case E_CAFEBABE:
        case E_VERSION:
        case W_NAOLIDOINTEIRO:
        case E_OPCAO_NAO_EXISTENTE:
        case E_DOLAR_NOT_SUPPORTED:
        case E_EMPTY_FRAME_STACK:
        case E_STACK_OVERFLOW:
        case E_NO_MAIN_FOUND:
        case E_EMPTY_STACK:
        case E_NOTSUPPORTED_VERSION:
        case E_NOTVALID_CP_INDEX:
        case E_NEG_ARR_SIZE:
        case E_NULL_POINTER:
        case E_ARR_NOT_FOUND:
        case E_NOT_VALID_MARRAY_INFO            :
        case E_NOT_SUPPORTED_ARRAY_TYPE          :
        case E_DISTINCT_MARRAY_DIMENSIONS        :
        case E_NOT_SUPPORTED_INSTRUCTION         :
        case E_BAD_INPUT                        :
        case E_NEGATIVE_ARRAY_SIZE              :
        default:
            printf("\n------------------------");              
            printf("\nErro:");                                 
            printf("\nUm erro indefinido ocorreu");    
            printf("\nSaindo...");                  
            printf("\n");
            //sleep(2);
            exit(1);
    }
}
