#include "macros.h"
#include "JVM_core.h"
#include "Heap.h"
#include "Frame.h"
#include "LoadClass_core.h"
#include "Execution_Core.h"
#include "LoadClass_ui.h"
#include "ClassCheck.h"

/**
 * @brief Inicializa a JVM; Aloca memória para as estruturas principais;
 Chama o carregador para a primeira classe; inicializa a primeira classe
 *
 * @param classHeap
 * @param objectHeap
 * @param stackFrame
 * @param classPathF_ptr
 */

ClassFile *classHeap;
dataMSize_t dmSize;
Frame **stackFrame;
Frame *executionFrame;
MethodHeap mHeap;
char *basePath;
Heap objHeap;
int saiu;

void jvmStartup(char *classPathStr, int flag){
    
    classHeap = malloc( CLSHEAP_MAX*sizeof( ClassFile ) );
    stackFrame = malloc( STKFRAME_MAX*sizeof( Frame ) );

    dmSize.clsHeap_size = 0;
    dmSize.objHeap_size = 0;
    dmSize.stkHeap_size = 0;
    
    basePath = (u1*)calloc(1024,sizeof(char));
    basePath[0] = '\0';
    
    objHeap = initHeap();
    mHeap = initMethodHeap();
    executionFrame = NULL;

    //Carrega a classe inicial
    //OK!
    //classLoader(classPathStr);
    
	for(int i = 0; i < classHeap->static_values_size; i++)
		printf("\nname %s", classHeap->static_values[i].field_name);
    ////Checa a consistência da classe
    if( flag ){
        classLoader(classPathStr);
        classCheck(*classHeap);
        printf("\n\nConteudo do .class\n");
        printf("------------------------------------------------\n");
        print_ClassFile(classHeap);
        exit(1);
    }
    printf("\n---%s---\n", classPathStr);
    
    loadClass(classPathStr);

    method_info* _main = getMainMethod();
    if (_main == NULL) {
        printf("nao tem main\n");
    }
     
    //error(E_NO_MAIN_FOUND);
    
    createFrame(_main, mHeap.classes[0]);
    Execute();
    //Fecha o arquivo do primeiro class file aberto
    //fclose(classPathF_ptr);
    //Inicializa a classe inicial, roda clinit

	//OCUPADO
    //initializeClass(classHeap_ptr, stackFrame_ptr, dmSize_ptr, classHeap_ptr);
    //initializeClass(classHeap_ptr, stackFrame_ptr, &dmSize_ptr->stkHeap_size); //Sei que o primeiro elemento da classHeap é a classe inicial

    //OCUPADO
	//Chamo o método main
	//callMethod(classHeap_ptr, stackFrame_ptr, dmSize_ptr, classHeap_ptr, "main", "([Ljava/lang/String;)V");


}

/**
 * @brief
 *
 * @param class_ptr
 * @param stkFrame_ptr
 * @param stkFrameTop_ptr
 */
void initializeClass(ClassFile *class_ptr){

    u2 method_idx = seekMethodInClass( class_ptr, "<clinit>", "()V" );
    method_info *method_ptr = &class_ptr->methods[method_idx];
    //Quem cria deleta.

    createFrame(method_ptr, class_ptr);//Cria o frame para o método <clinit> da classe.

    //u2 aux_idx = dmSize.stkHeap_size - 1; // o stkFrameTop_ptr na verdade é o stack frame size, que indica a qtd de frames na stkframe.

    //Teste
    Execute();
    

    //Deleta o frame.
    deleteFrame();
}

/**
* @brief
*
* @param class_ptr
* @param stkFrame_ptr
* @param dmSize_ptr
* @param classHeap_ptr
* @param mth_name
* @param mth_descriptor
*/
void callMethod(ClassFile *class_ptr, char *mth_name, char *mth_descriptor){
    u2 method_idx = seekMethodInClass( class_ptr, mth_name, mth_descriptor );

	if( method_idx  == SEEK_NOTFOUND){
		exit(1);
	}
    method_info *method_ptr = &class_ptr->methods[method_idx];
    //Quem cria deleta.

    createFrame(method_ptr, class_ptr);//Cria o frame para o método <clinit> da classe.

    //Teste
    Execute();

    //Deleta o frame.
    deleteFrame();
}

/** OK!
 * @brief Busca no classFile por um métodos específico e retorna o indice para a tabela de métodos
 *
 * @param class_ptr ponteiro para um classFile
 * @param methName_str nome do método que se deseja buscar
 * @param methDescriptor_str descritor do método, ()V por exemplo
 *
 * @return retorna o índice na tabela de métodos do classFile
 */
u2 seekMethodInClass(ClassFile *class_ptr, char *methName_str, char *methDescriptor_str){
    char *methodN;
    char *methodD;
    //char *bytes;
    //u2 str_size;
    for(int i = 0; i < class_ptr->methods_count; i++){
        //str_size = class_ptr->constant_pool[class_ptr->methods[i].name_inex - 1].info.CONSTANT_Utf8_info.length;
        //methodN = malloc( (str_size + 1)*sizeof(char) );
        //bytes = class_ptr->constant_pool[class_ptr->methods[i].name_index - 1].info.CONSTANT_Utf8_info_bytes;

        methodN = (char*)class_ptr->constant_pool[class_ptr->methods[i].name_index - 1].info.CONSTANT_Utf8_info.bytes;
        methodD = (char*)class_ptr->constant_pool[class_ptr->methods[i].descriptor_index - 1].info.CONSTANT_Utf8_info.bytes;
        if( !strcmp(methodN, methName_str) && !strcmp(methodD, methDescriptor_str) ){
            return i;

        }

        /*for(int j = 0; j < str_size; j++){
         methodN[j] = bytes[j];
         }
         methodN[str_size
         */
    }
    return SEEK_NOTFOUND;
}

/**
 *  @brief Métdodo que acha uma classe dado o classHeap e o nome da classe
 *
 *  @param classHeap_ptr <#classHeap_ptr description#>
 *  @param dmSize        <#dmSize description#>
 *  @param ClassName     <#ClassName description#>
 *
 *  @return <#return value description#>
 */
int findClass(char* ClassName){

    u2 clsHeapSize = dmSize.stkHeap_size;
    u2 index;

    for(int i=0; i < clsHeapSize; i++) {
        index = classHeap[i].this_class;
        if(strcmp((char*)classHeap[i].constant_pool[index-1].info.CONSTANT_Utf8_info.bytes, ClassName) == 0) {
            return i;
        }
    }
    return SEEK_NOTFOUND;
}

u2 findCode(method_info *method) {

    u2 i = 0;
    while(method->attribute[i].tag != 1){
        i++;

    }
    return i;
}


/**
 *  @brief Cria o Frame do método a ser executado
 *
 *  @param method    <#method description#>
 *  @param Class     <#Class description#>
 *  @param frame_ptr <#frame_ptr description#>
 *  @param numFrames <#numFrames description#>
 *
 *  @return <#return value description#>
 */
//Lembrar de enviar o dataMSize->stkHeap_size para o numFrames
void createFrame(method_info *method, ClassFile *Class) {

    u2 i = dmSize.stkHeap_size;
    u2 codeIndex = 0;
    codeIndex = findCode(method);

    if (i < STKFRAME_MAX - 1) {
        //OCUPADO
        stackFrame[i] = initFrame(Class, method, codeIndex);
        executionFrame = stackFrame[i];

        dmSize.stkHeap_size++;
    } else {
        printf("Frame não pode ser alocado, tamanho máximo atingido");
        exit(1);
    }
}

//Lembrar de enviar o dataMSize->stkHeap_size para o numFrames
void deleteFrame(Frame *frame_ptr, u2 *numFrames) {

    dmSize.stkHeap_size--;
    executionFrame = stackFrame[dmSize.stkHeap_size - 1];

}

/*SAMUEL - FIM*/

int searchFile (char* name)
{
	DIR *dp;
	struct dirent *ep;     
	int i, length, j;
	char aux[strlen(name)+1], path[strlen(name)+1], completePath[strlen(name)+1];

	length = strlen(name)+1;
	strcpy(aux, name);
	strcpy(path, name);
	strcpy(completePath, "./");
	for(i = length; (aux[i] != '/' && i != 0); i--) {
		path[i] = '\0';
	}
	if (i == 0) {
		dp = opendir("./");
	}
	else {
		strcat(completePath, path);
		dp = opendir (completePath);
		j = 0;
		while (++i < length) {
			aux[j] = aux[i];
			j++;
		}
		aux[j] = '\0';
	}
	if (dp != NULL)
	{
		while ((ep = readdir (dp)) && ep != NULL) {
			if (!strcmp(ep->d_name, aux)) {
				puts(ep->d_name);
				return 1;
			}
		}
    (void) closedir (dp);
	}
	return 0;
}
