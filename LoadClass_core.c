#include "LoadClass_core.h"
#include "macros.h"
#include "Heap.h"
#include "Frame.h"


extern dataMSize_t dmSize;
extern ClassFile *classHeap;
extern Frame *stackFrame;
extern Heap objHeap;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Constant Pool    -   inicio
////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Código para leitura e exibição da Pool de Constantes de um arquivo
 * do tipo .class.
 * 
 * @author Rafael Penna
 * 		rafaelgpenna@gmail.com
 * 
 */
ClassFile *fetchClass(u1 *strClassName) {
    u2 i = 0;
    u2 found = 0;
    
    while (i < dmSize.clsHeap_size && found == 0) {
        if (strcmp((char*)strClassName, (char*)classHeap[i].className) == 0) {
            found = 1;
        }
        else {
        ++i;
        }
    }
    if (found == 0) {
        return NULL;
    }
    else {
        return &classHeap[i];
    }
    
}

attribute_info *readAttributeInfo (u2 attr_count, FILE *arq, cp_info *pool, int method);
void print_attributes (attribute_info *attributes, u2 attr_count, cp_info *pool);

// Função para converter hex de 4 dígitos de big
// endian para little endian
u2 convertEndianU2 (u2 num) {
	u2 aux1, aux2;
	
	aux1 = (num & 0x00ff) << 8;
	aux2 = (num & 0xff00) >> 8;
	
	return aux1 | aux2;
}

// Função para converter hex de 8 dígitos de big
// endian para little endian
u4 convertEndianU4 (u4 num) {
	u4 aux1, aux2, aux3, aux4;
	
	aux1 = (num & 0x000000ff) << 24;
	aux2 = (num & 0x0000ff00) << 8;
	aux3 = (num & 0x00ff0000) >> 8;
	aux4 = (num & 0xff000000) >> 24;
	
	return aux1 | aux2 | aux3 | aux4;
}
// le o magic
u4 read_magic (FILE *arq) {
	u4 magic;
	fread(&magic, sizeof(u4), 1, arq);
	magic = convertEndianU4(magic);
	return magic;
}

//le minor_version
u2 read_minor_version(FILE *arq) {
	u4 minor;
	fread(&minor, sizeof(u2), 1, arq);
	minor = convertEndianU2(minor);
	return minor;
}

//le major_version
u2 read_major_version(FILE *arq) {
	u2 major;
	fread(&major, sizeof(u2), 1, arq);
	major = convertEndianU2(major);
	return major;
}

// Le 2 bytes de arquivo
u2 read_u2 (FILE *arq) {
    u2 value;
    fread(&value, sizeof(u2), 1, arq);
    value = convertEndianU2(value);
    return value;
}

// Le 4 bytes de arquivo
u4 read_u4 (FILE *arq) {
    u4 value;
    fread(&value, 4, 1, arq);
    value = convertEndianU4(value);
    return value;
}

long getLong(u4 highBytes, u4 lowBytes) {
    return ((u8)highBytes) << 32 | lowBytes;
}

/*!
	pega e trata um double que esta na class, principalmente para exibir e tratar operacoes
 */
double getDouble(u4 highBytes, u4 lowBytes) {
    
    u8 var = 0, auxVar;
    double toReturn;
    
    var = var | (u8)highBytes;
    var = var << 32;
    var = var | (u8)lowBytes;
    
    
    double *double_value = (double*)malloc(sizeof(double));
    
    
    auxVar = (u8)highBytes;
    auxVar = auxVar << 32;
    auxVar |= (u8)lowBytes;
    
    memcpy(double_value, &auxVar, sizeof(double));
    
    toReturn = *double_value;
    
    return toReturn;
}

// Função para ler Utf8
void readUtf8 (u2 *poolLength, FILE *arq, cp_info* aux) {
	u2 length;
	u2 counter;
	u1 byte;
	char *string = "";
	char *auxString = "";
	size_t size;
	
	fread(&length, sizeof(u2), 1, arq);
	length = convertEndianU2(length);
	*poolLength += length + 1 + 2; // 1 tag + 2 length + length bytes de texto
	counter = 0;
	size = 0;
	while (counter++ < length) {
		auxString = string;
		string = (char*) malloc ((size + 2) * sizeof(char));
		strcpy(string, auxString);
		fread(&byte, sizeof(u1), 1, arq);
		string[size] = (u1) byte;
		string[size+1] = '\0';
		size = strlen(string);
	}
	aux->info.CONSTANT_Utf8_info.length = length;
	aux->info.CONSTANT_Utf8_info.bytes = (u1*)string;
    
}

// Função para ler Integer ou Float
void readIntegerFloat (FILE *arq, cp_info* aux) {
	u4 bytes;
	
	fread(&bytes, sizeof(u4), 1, arq);
	bytes = convertEndianU4(bytes);
	aux->info.CONSTANT_IntegerFloat_info.bytes = bytes;
}

// Função para ler Long ou Double
void readLongDouble (FILE *arq, cp_info* aux) {
	u4 bytes;
	
	fread(&bytes, sizeof(u4), 1, arq);
	bytes = convertEndianU4(bytes);
	aux->info.CONSTANT_LongDouble_info.high_bytes = bytes;
	fread(&bytes, sizeof(u4), 1, arq);
	bytes = convertEndianU4(bytes);
	aux->info.CONSTANT_LongDouble_info.low_bytes = bytes;
}

// Função para ler Class
void readClass (FILE *arq, cp_info* aux) {
	u2 name;
	
	fread(&name, sizeof(u2), 1, arq);
	name = convertEndianU2(name);
	aux->info.CONSTANT_Class_info.name_index = name;
}

// Função para ler String
void readString (FILE *arq, cp_info* aux) {
    u2 string_index;
    
    fread(&string_index, sizeof(u2), 1, arq);
    string_index = convertEndianU2(string_index);
    aux->info.CONSTANT_String_info.string_index = string_index;
    
}

// Função para ler Fieldref, Mehtodref ou Interfaceref
void readRefs (FILE *arq, cp_info* aux) {
	u2 next;
	
	fread(&next, sizeof(u2), 1, arq);
	next = convertEndianU2(next);
	aux->info.CONSTANT_FieldMethodIMethod_info.class_index = next;
	fread(&next, sizeof(u2), 1, arq);
	next = convertEndianU2(next);
	aux->info.CONSTANT_FieldMethodIMethod_info.name_and_type_index\
	 = next;
}

// Função para ler Name and Type
void readNameAndType (FILE *arq, cp_info* aux) {
	u2 next;
	
	fread(&next, sizeof(u2), 1, arq);
	next = convertEndianU2(next);
	aux->info.CONSTANT_NameAndType_info.name_index = next;
	fread(&next, sizeof(u2), 1, arq);
	next = convertEndianU2(next);
	aux->info.CONSTANT_NameAndType_info.descriptor_index = next;
}

// Função para ler a Constant Pool
cp_info* readConstantPool (u2 length, u2 *poolLength, FILE *arq) {
	u1 nextConstant; // recebe sempre uma tag
	u2 counter = 0;
	cp_info *pool;
	
	pool = (cp_info*) malloc (length * sizeof(cp_info));
	//*poolLength = 0;
	while (counter < length - 1) {
		fread(&nextConstant, sizeof(u1), 1, arq);
		pool[counter].tag = nextConstant;
		switch ((int)nextConstant) {
			case 1: //Utf8
				readUtf8(poolLength, arq, pool+counter);
				break;
			case 3: // Integer
			case 4: // Float
				readIntegerFloat(arq, pool+counter);
				*poolLength += 5; // 1 tag + 4 bytes
				break;
			case 5: // Long
			case 6: // Double
				readLongDouble(arq, pool+counter);
				*poolLength += 9; // 1 tag + 4 high_bytes + 4 low_bytes
                counter++;
				break;
			case 7: // Class
				readClass(arq, pool+counter);
				*poolLength += 3; // 1 tag + 2 name_index
				break;
            case 8: // String
                readString(arq, pool+counter);
                *poolLength += 3; // 1 tag + 2 string_index
                break;
			case 9: // Fieldref
			case 10: // Methodref
			case 11: // Interfaceref
				readRefs(arq, pool+counter);
				*poolLength += 5; // 1 tag + 2 class_index + 2 name_and_type_index 
				break;
			case 12: // Name And Type
				readNameAndType(arq, pool+counter);
				*poolLength += 5; // 1 tag + 2 name_index + 2 descriptor_index
				break;
			default:
				printf ("Erro de identificação tag!\n");
				break;
		}
		counter++;
	}
	return pool;
}

// Função para retornar o tipo de constante
char* searchConstantType (u1 tag) {
	switch (tag) {
		case 1:
			return "Utf8";
		case 3:
			return "Integer";
		case 4:
			return "Float";
		case 5:
			return "Long";
		case 6:
			return "Double";
		case 7:
			return "Class";
		case 8:
			return "String";
		case 9:
			return "Fieldref";
		case 10:
			return "Methodref";
		case 11:
			return "Interfaceref";
		case 12:
			return "NameAndType";
		case 15:
			return "MethodHandle";
		case 16:
			return "MethodType";
		case 18:
			return "InvokeDynamic";
		default:
			return "Unknown Constant Type";
	}
}	

/**
 * Função que cria o vetor de Pool de Constantes, retornando o endereço
 * do primeiro elemento do vetor e o tamanho da pool em bytes(por 
 * referência).
 * 
 * @param *poolLength
 * 		Ponteiro que aramazenará o endereço da variável que indica o 
 * 		tamanho da pool de constantes em bytes, de modo que a 
 * 		passagem do valor seja por referência.
 * @param *arq
 * 		Ponteiro do tipo FILE.
 * @param *poolElementsNum
 * 		Ponteiro que aramazenará o endereço da variável que indica o
 * 		tamanho da pool de constantes em quantidade de elementos, de 
 * 		modo que a passagem do valor seja por referência.
 * @return
 * 		Endereço do primeiro elemento do vetor da Pool de Constantes
 * 
 * @author Rafael Penna
 * 		rafaelgpenna@gmail.com
 * 
 */		 
cp_info* createConstantPool (u2 *poolElementsNum, u2 *poolLength, FILE *arq) {
	
	fseek(arq, 9, SEEK_SET);
	fread(poolElementsNum, sizeof(u1), 1, arq);
	return readConstantPool(*poolElementsNum, poolLength, arq);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//Fields    -   inicio
////////////////////////////////////////////////////////////////////////////////////////////////////////
u2 arrayToint(u1 array[], unsigned int len){
    u2 number = 0;
    for(int i = 0; i < len; i++){
        //printf("\n%d", array[i]);
        //number = number | array[i];
        //number = number << 8;
        //Lembre-se que o arquivo está em big-endian, o array está em big-endian, mas o processador é little-endian.
        //Para corrigir o problema de o inteiro(u2) ser lido em ordem inversa fazemos a operação abaixo em vez de
        // um biwise or
        number = number + array[i]*pow(16, (len - 1) - i);
    }
    return number;
}

/**
 * @brief troca a ordenação dos bytes em uma variável inteira
 *
 * @param number ponteiro para a variável que será reordenada
 * @param bytesn número de bytes alocados para essa variável
 *
 * @return ponteiro para uma variável nova com ordenção de bytes invertida
 */
void* swapEndianess(void* number, unsigned int bytesn){
    switch(bytesn){
        case 2:{
            u2* swapedn = malloc( sizeof( u2 ) );
            *swapedn = *( (u2*)number ) << 8 | *( (u2*)number ) >> 8;
            return  swapedn;
        }
        case 4:{
            u4* swapedn = malloc( sizeof( u4 ) );
            *swapedn = (*( (u4*)number ) << 24) |
            (( *( (u4*)number ) & 0x0000FF00 ) << 8) |
            (( *( (u4*)number ) & 0x00FF0000 ) >> 8) |
            (( *( (u4*)number ) >> 24 ) & 0x000000FF);
            return swapedn;
        }
        default:{
            printf("\nswapEndianess: numero de bytes nao reconhecido\n");
            exit(1);
        }
    }
}
/**
 * @brief lê toda a tabela field_info contida no arquivo .class
 *
 * @param class_f arquivo .class (class_file) aberto no modo binário. Os dados estão em big-endian
 * @param offset quantidade de bytes do início do arquivo .class até o primeiro elemento da tabela analisada
 * @param bytes quantidade de bytes da tabela analisada. Esse valor será retornado e não recebido
 *
 * @return um vetor contendo todas as estruturas fild_info contidas no arquivo .class
 */
field_info *field_func(FILE *class_f, unsigned int offset, unsigned int *bytes_ptr, cp_info* pool_array, unsigned int *fieldn){
    /*
    if( fseek(class_f, offset, SEEK_SET) != 0 ){
        printf("\nErro no deslocamento no stream\n");
        exit(1);
    }
    */
    //*bytes_ptr = 0; //Me certificando que o valor inicial é 0
    
    
    /** Preparação para a leitura no arquivo em busca das informações
     A ordem das informações é a seguinte:
     Contador de fields e depois a tabela de fields.
     O contador de fields especificará a quantidade de elementos no vetor de retorno. */
    field_info* fields_array = NULL; // valor de retorno
    //u1 buffer_u1 = 0;
    u2 buffer_u2;
    u4 buffer_u4;
    unsigned int buffer_size;
    
    fread(&buffer_u2, sizeof(buffer_u2), 1, class_f); //Li o contador de fields
    //printf("\n%x\n", buffer_u2); //TESTE
    u2* field_count_ptr = 0;
    buffer_size = sizeof(buffer_u2);
    field_count_ptr = (u2*) swapEndianess(&buffer_u2, buffer_size);
    //printf("\n%x\n", *field_count_ptr); //TESTE
    *fieldn = *field_count_ptr;
    //printf("\n\nTESTE:%d", *fieldn);
    
    *bytes_ptr += 2; //Li 2 bytes
    
    if( *field_count_ptr != 0 ){
        //Já tenho informação suficiente para estabelecer a qtd de elementos do array de retorno
        fields_array = malloc( (*field_count_ptr)*sizeof(field_info) );
        
        //Vetor já criado. Começo a ler os dados
        u2* acc_flag_ptr;
        u2* name_idx_ptr;
        u2* dcrpt_idx_ptr;
        u2* att_count_ptr;
        //Para atributos internos ao field
        u2* att_nameidx_ptr;
        u4* att_len_ptr; //Indica a qtd de byte, excluindo os 6 primeiros, de um atributo.
        //elemento da pool de constantes apontado pelo att_nameidx
        u2 str_len;
        u1* utf8_str;
        //parte de um atributo específico
        //ConstantValue
        u2* c_valueidx_ptr;
        //Synthetic, não tem campos extras
        //Deprecated, não tem campos extras
        
        for(int i = 0; i < *field_count_ptr; i++){
            
            buffer_size = sizeof(buffer_u2); // posso retirar porque não foi alterado desde a última vez
            
            fread(&buffer_u2, sizeof(buffer_u2), 1, class_f);
            acc_flag_ptr = (u2*) swapEndianess(&buffer_u2, buffer_size);
            fields_array[i].access_flag = *acc_flag_ptr;
            *bytes_ptr += 2;
            //printf("\nAccess flag %x\nContador de bytes %d\n",fields_array[i].access_flag, *bytes_ptr ); //TESTE
            
            fread(&buffer_u2, sizeof(buffer_u2), 1, class_f);
            name_idx_ptr = (u2*) swapEndianess(&buffer_u2, buffer_size);
            fields_array[i].name_index = *name_idx_ptr;
            *bytes_ptr += 2;
            //printf("\nName index %x\nContador de bytes %d\n",fields_array[i].name_index, *bytes_ptr ); //TESTE
            
            fread(&buffer_u2, sizeof(buffer_u2), 1, class_f);
            dcrpt_idx_ptr = (u2*) swapEndianess(&buffer_u2, buffer_size);
            fields_array[i].descriptor_index = *dcrpt_idx_ptr;
            *bytes_ptr += 2;
            //printf("\nDescriptor index %x\nContador de bytes %d\n", fields_array[i].descriptor_index, *bytes_ptr ); //TESTE
            
            
            fread(&buffer_u2, sizeof(buffer_u2), 1, class_f);
            att_count_ptr = (u2*) swapEndianess(&buffer_u2, buffer_size);
            fields_array[i].attributes_count = *att_count_ptr;
            *bytes_ptr += 2;
            //printf("\nAttributes count %x\nContador de bytes %d\n", fields_array[i].attributes_count, *bytes_ptr ); //TESTE
            
            if( *att_count_ptr != 0 ){ //TESTE mudar para !=
                //Crio o array de atributos para o campo específico
                fields_array[i].attributes = (attribute_info*) malloc( (*att_count_ptr)*sizeof(attribute_info) );
                
                for(int idx, j = 0; j < *att_count_ptr; j++){ // TESTE retirar o + 1
                    //Indice com o nome do atributo
                    fread(&buffer_u2, sizeof(buffer_u2), 1, class_f);
                    att_nameidx_ptr = (u2*) swapEndianess(&buffer_u2, buffer_size);
                    idx = fields_array[i].attributes[j].attribute_name_index = *att_nameidx_ptr;
                    *bytes_ptr += 2;
                    //printf("\nAttributes name_index %x\nContador de bytes %d\n",\
                           fields_array[i].attributes[j].attribute_name_index, *bytes_ptr ); //TESTE
                    
                    //Quantidade de bytes do atributo, excluindo estes 4 e os 2 anteriores.
                    fread(&buffer_u4, sizeof(buffer_u4), 1, class_f);
                    //Preciso mudar o tamanho do buffer
                    buffer_size = sizeof(buffer_u4);
                    att_len_ptr = (u4*) swapEndianess(&buffer_u4, buffer_size);
                    fields_array[i].attributes[j].attribute_length = *att_len_ptr;
                    *bytes_ptr += 4;
                    //printf("\nAttributes length %x\nContador de bytes %d\n",\
                           fields_array[i].attributes[j].attribute_length, *bytes_ptr ); //TESTE
                    
                    //	break; // TESTE
                    
                    //Essa parte é dependente do attributo em questão.
                    //Vou conhecer o atributo buscando a "string" na pool de constantes
                    //idx é um índice na pool de constantes
                    //Os únicos permitidos são:
                    //ConstantValues, Synthetic e Deprecated
                    //A tag obrigatoriamente precisa ser para o elemento utf8
                    switch ( pool_array[idx - 1].tag ){
                        case CONSTANT_UTF8:
                            str_len = pool_array[idx - 1].info.CONSTANT_Utf8_info.length;
                            utf8_str = (u1*) malloc( (str_len + 1)*sizeof(u1) );
                            utf8_str[str_len] = '\0';
                            for(int k = 0; k < str_len; k++)
                                utf8_str[k] = pool_array[idx - 1].info.CONSTANT_Utf8_info.bytes[k];
                            printf("\n%s\n", utf8_str);
                            
                            //A partir desse momento já se conhece o atributo
                            if( !strcmp((char*)utf8_str, CONSTANTVALUE_STR) ){
                                if(*att_len_ptr != 2){
                                    printf("\nfield.c: ConstantValue length != 2");
                                    exit(1);
                                }
                                fread(&buffer_u2, sizeof(buffer_u2), 1, class_f);
                                //Preciso mudar o tamanho do buffer
                                buffer_size = sizeof(buffer_u2);
                                c_valueidx_ptr = (u2*) swapEndianess(&buffer_u2, buffer_size); 
                                fields_array[i].attributes[j].info.ConstantValue_attribute.constantvalue_index =\
                                *c_valueidx_ptr;
                                *bytes_ptr += 2;
                                
                            }
                            else if( !strcmp((char*)utf8_str, SYNTHETIC_STR ) ){
                                if(*att_len_ptr != 0){
                                    printf("\nfield.c: deprecated length != 0");
                                    exit(1);
                                }	
                            }
                            else if( !strcmp((char*)utf8_str, DEPRECATED_STR) ){
                                if(*att_len_ptr != 0){
                                    printf("\nfield.c: deprecated length != 0");
                                    exit(1);
                                }
                            }
                            else
                                printf("\nfield.c: atributo invalido.\n");					
                            
                            break;
                        default: //restringir depois	
                            printf("\nfield.c:tag errada para atributo.\n");					
                            printf("\nO valor deve ser uft8\n");
                            
                    }//switch com a tag na pool de constantes	
                    
                }//for contador de atributos dentro do field
                
                
            }//if contador de atributos dentro do field
            //break;
            
        }//for contador de fields	
    }//if de teste do contador de fields.	
    
    return fields_array;
    
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods
////////////////////////////////////////////////////////////////////////////////////////////////////////

// Le Dados dos Metodos
void read_Methods(ClassFile *classFileVar, FILE *arq, cp_info *pool) {
    attribute_info *attribute;
    
    classFileVar->methods_count = read_u2(arq);
    //printf("%x = Numero de metodos\n", classFileVar->methods_count);// So pra ver se ta pegando certo
    classFileVar->methods = (method_info*) malloc (classFileVar->methods_count * sizeof(method_info));
    for (int i = 0; i < classFileVar->methods_count; i++) {
        classFileVar->methods[i].access_flags = read_u2(arq);
        //printf("%x = access flag\n", classFileVar->methods[i].access_flags);// So pra ver se ta pegando certo
        classFileVar->methods[i].name_index = read_u2(arq);
        classFileVar->methods[i].descriptor_index = read_u2(arq);
        
        classFileVar->methods[i].attributes_count = read_u2(arq);
        attribute = readAttributeInfo(classFileVar->methods[i].attributes_count, arq, pool, 1);
        classFileVar->methods[i].attribute = attribute;
        
        //classFileVar->methods[i].attributes_count = read_u2(arq);
        //fseek(arq, 244, SEEK_SET); //Pulando os atributos do primeiro metodo - indo direto para segundo metodo
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//Atributos
////////////////////////////////////////////////////////////////////////////////////////////////////////

attribute_info *readAttributeInfo (u2 attr_count, FILE *arq, cp_info *pool, int method) {
    attribute_info *attributes, *aux;
    u2 counter = 0;
    u4 attr_length;
    u4 counter_length = 0;
    u2 attr_name_index = 0;
    u1 *byte, attr_info_1;
    u2 attr_info;
    u4 attr_info_4 = 0;
    
    attributes = (attribute_info *) malloc (attr_count * sizeof(attribute_info));
    
    while (counter < attr_count) {
        attr_length = 0;
        
        fread(&attr_name_index, sizeof(u2), 1, arq);
        attr_name_index = convertEndianU2(attr_name_index);
        attributes[counter].attribute_name_index = attr_name_index;
        //printf("%x\n", attributes[counter].attribute_name_index);
        
        fread(&attr_length, sizeof(u4), 1, arq);
        attr_length = convertEndianU4(attr_length);
        attributes[counter].attribute_length = attr_length;
        //printf("attr length = %x\n", attributes[counter].attribute_length);
        
        if(strcmp((char*)pool[attr_name_index-1].info.CONSTANT_Utf8_info.bytes,"ConstantValue") == 0) {
            attributes[counter].tag = 0;
            fread(&attr_info, sizeof(u2), 1, arq);
            attr_info = convertEndianU2(attr_info);
            attributes[counter].info.ConstantValue_attribute.constantvalue_index = attr_info;
            
        }
        else {
            if(strcmp((char*)pool[attr_name_index-1].info.CONSTANT_Utf8_info.bytes,"Code") == 0) {
                attributes[counter].tag = 1;
                fread(&attr_info, sizeof(u2), 1, arq);
                attr_info = convertEndianU2(attr_info);
                attributes[counter].info.Code_attribute.max_stack = attr_info;
                //printf("max stack = %x\n", attributes[counter].info.Code_attribute.max_stack);
                //getchar();
                
                fread(&attr_info, sizeof(u2), 1, arq);
                attr_info = convertEndianU2(attr_info);
                attributes[counter].info.Code_attribute.max_locals = attr_info;
                //printf("max locals = %x\n", attributes[counter].info.Code_attribute.max_locals);
                //getchar();
                
                fread(&attr_info_4, sizeof(u4), 1, arq);
                attr_info_4 = convertEndianU4(attr_info_4);
                attributes[counter].info.Code_attribute.code_length = attr_info_4;
                //printf("code length = %x\n", attributes[counter].info.Code_attribute.code_length);
                //getchar();
                
                //byte = malloc(sizeof(attr_info_4));
                byte = (u1 *) calloc (attr_info_4, sizeof(u1));
                
                while (counter_length < attr_info_4) {
                    fread(&attr_info_1, sizeof(u1), 1, arq);
                    byte[counter_length] = attr_info_1;
                    //printf("code = %x\n", byte[counter_length]);
                    //getchar();
                    ++counter_length;
                }
                attributes[counter].info.Code_attribute.code = byte;
                
                
                //free(byte);
                
                fread(&attr_info, sizeof(u2), 1, arq);
                attr_info = convertEndianU2(attr_info);
                attributes[counter].info.Code_attribute.exception_table_length = attr_info;
                //printf("exception table length = %x\n", attributes[counter].info.Code_attribute.exception_table_length);
                //getchar();
                
                ExceptionForCode *exception = (ExceptionForCode *) calloc (attr_info, sizeof(ExceptionForCode));
                //ExceptionForCode *exception = malloc(sizeof(attr_info));
                
                u2 counter_exception = 0;
                u2 exception_info;
                while (counter_exception < attr_info) {
                    fread(&exception_info, sizeof(u2), 1, arq);
                    exception_info = convertEndianU2(exception_info);
                    exception[counter_exception].start_pc = exception_info;
                    
                    fread(&exception_info, sizeof(u2), 1, arq);
                    exception_info = convertEndianU2(exception_info);
                    exception[counter_exception].end_pc = exception_info;
                    
                    fread(&exception_info, sizeof(u2), 1, arq);
                    exception_info = convertEndianU2(exception_info);
                    exception[counter_exception].handler_pc = exception_info;
                    
                    fread(&exception_info, sizeof(u2), 1, arq);
                    exception_info = convertEndianU2(exception_info);
                    exception[counter_exception].catch_type = exception_info;
                    
                    ++counter_exception;
                }
                attributes[counter].info.Code_attribute.exception_table = exception;
                
                //free(exception);
                
                fread(&attr_info, sizeof(u2), 1, arq);
                attr_info = convertEndianU2(attr_info);
                attributes[counter].info.Code_attribute.attribute_count = attr_info;
                
                //aux = (attribute_info *) calloc (attr_info, sizeof(attribute_info));
                aux = readAttributeInfo(attr_info,arq, pool, method);
                attributes[counter].info.Code_attribute.attribute = aux;
                
                //free(aux);
                
                //attributes[counter].info.Code_attribute.attribute = readAttributeInfo(attr_info,arq, pool);
                
            }
            else {
                if(strcmp((char*)pool[attr_name_index-1].info.CONSTANT_Utf8_info.bytes,"Exceptions") == 0) {
                    attributes[counter].tag = 2;
                    fread(&attr_info, sizeof(u2), 1, arq);
                    attr_info = convertEndianU2(attr_info);
                    attributes[counter].info.Exception_attribute.number_of_exceptions = attr_info;
                    
                    u2 *exception_table_index = (u2 *) calloc (attr_info, sizeof(u2));
                    u2 counter_exception = 0;
                    while (counter_exception < attributes[counter].info.Exception_attribute.number_of_exceptions) {
                        fread(&attr_info, sizeof(u2), 1, arq);
                        attr_info = convertEndianU2(attr_info);
                        exception_table_index[counter_exception] = attr_info;
                        
                        ++counter_exception;
                    }
                    
                    attributes[counter].info.Exception_attribute.exception_index_table = exception_table_index;
                    
                    //free(exception_table_index);
                    
                }
                else {
                    if(strcmp((char*)pool[attr_name_index-1].info.CONSTANT_Utf8_info.bytes,"InnerClasses") == 0) {
                        attributes[counter].tag = 3;
                        fread(&attr_info, sizeof(u2), 1, arq);
                        attr_info = convertEndianU2(attr_info);
                        attributes[counter].info.InnerClasses_attribute.number_of_classes = attr_info;
                        
                        ClassesForIClasses *classes = (ClassesForIClasses *) calloc (attr_info, sizeof(ClassesForIClasses));
                        u2 class_counter = 0;
                        while (class_counter < attributes[counter].info.InnerClasses_attribute.number_of_classes) {
                            fread(&attr_info, sizeof(u2), 1, arq);
                            attr_info = convertEndianU2(attr_info);
                            classes[class_counter].inner_class_info_index = attr_info;
                            
                            fread(&attr_info, sizeof(u2), 1, arq);
                            attr_info = convertEndianU2(attr_info);
                            classes[class_counter].outer_class_info_index = attr_info;
                            
                            fread(&attr_info, sizeof(u2), 1, arq);
                            attr_info = convertEndianU2(attr_info);
                            classes[class_counter].inner_name_index = attr_info;
                            
                            fread(&attr_info, sizeof(u2), 1, arq);
                            attr_info = convertEndianU2(attr_info);
                            classes[class_counter].inner_class_access_flags = attr_info;
                            ++class_counter;
                        }
                        
                        attributes[counter].info.InnerClasses_attribute.classes = classes;
                        
                        //free(classes);
                        
                    }
                    else {
                        if(strcmp((char*)pool[attr_name_index-1].info.CONSTANT_Utf8_info.bytes,"Synthetic") == 0) {
                            attributes[counter].tag = 4;
                            
                        }
                        else {
                            if(strcmp((char*)pool[attr_name_index-1].info.CONSTANT_Utf8_info.bytes,(char*)"SourceFile") == 0) {
                                attributes[counter].tag = 5;
                                fread(&attr_info, sizeof(u2), 1, arq);
                                attr_info = convertEndianU2(attr_info);
                                attributes[counter].info.SourceFile_attribute.sourcefile_index = attr_info;
                            }
                            else {
                                if(strcmp((char*)pool[attr_name_index-1].info.CONSTANT_Utf8_info.bytes,"LineNumberTable") == 0) {
                                    attributes[counter].tag = 6;
                                    fread(&attr_info, sizeof(u2), 1, arq);
                                    attr_info = convertEndianU2(attr_info);
                                    attributes[counter].info.LineNumberTable_attribute.line_number_table_length = attr_info;
                                    
                                    LineNumberForLNumber *lineNumber = (LineNumberForLNumber *) calloc (attr_info, sizeof(LineNumberForLNumber));
                                    u2 line_counter = 0;
                                    while (line_counter < attributes[counter].info.LineNumberTable_attribute.line_number_table_length) {
                                        fread(&attr_info, sizeof(u2), 1, arq);
                                        attr_info = convertEndianU2(attr_info);
                                        lineNumber[line_counter].start_pc = attr_info;
                                        
                                        fread(&attr_info, sizeof(u2), 1, arq);
                                        attr_info = convertEndianU2(attr_info);
                                        lineNumber[line_counter].line_number = attr_info;
                                        
                                        ++line_counter;
                                    }
                                    
                                    attributes[counter].info.LineNumberTable_attribute.line_number_table = lineNumber;
                                    
                                    //free(lineNumber);
                                    
                                }
                                else {
                                    if(strcmp((char*)pool[attr_name_index-1].info.CONSTANT_Utf8_info.bytes,"LocalVariableTable") == 0) {
                                        attributes[counter].tag = 7;
                                        fread(&attr_info, sizeof(u2), 1, arq);
                                        attr_info = convertEndianU2(attr_info);
                                        attributes[counter].info.LocalVariableTable_attribute.local_variable_length = attr_info;
                                        
                                        LocalVariableForVariable *localVariable = (LocalVariableForVariable *) calloc (attr_info, sizeof(LocalVariableForVariable));
                                        u2 lv_counter = 0;
                                        while (lv_counter < attributes[counter].info.LocalVariableTable_attribute.local_variable_length) {
                                            fread(&attr_info, sizeof(u2), 1, arq);
                                            attr_info = convertEndianU2(attr_info);
                                            localVariable[lv_counter].start_pc = attr_info;
                                            
                                            fread(&attr_info, sizeof(u2), 1, arq);
                                            attr_info = convertEndianU2(attr_info);
                                            localVariable[lv_counter].length = attr_info;
                                            
                                            fread(&attr_info, sizeof(u2), 1, arq);
                                            attr_info = convertEndianU2(attr_info);
                                            localVariable[lv_counter].name_index = attr_info;
                                            
                                            fread(&attr_info, sizeof(u2), 1, arq);
                                            attr_info = convertEndianU2(attr_info);
                                            localVariable[lv_counter].descriptor_index = attr_info;
                                            
                                            fread(&attr_info, sizeof(u2), 1, arq);
                                            attr_info = convertEndianU2(attr_info);
                                            localVariable[lv_counter].index = attr_info;
                                            
                                            ++lv_counter;
                                        }
                                        
                                        attributes[counter].info.LocalVariableTable_attribute.local_variable_table = localVariable;
                                        
                                        //free(localVariable);
                                    }
                                    else {
                                        if(strcmp((char*)pool[attr_name_index-1].info.CONSTANT_Utf8_info.bytes,"Deprecated") == 0) {
                                            attributes[counter].tag = 8;
                                            
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ++counter;
    }
    return attributes;
}

attribute_info *createAttributeInfo (FILE *arq, cp_info *pool, u2 *attr_count) {
    
    //Move para attribute_count
    //fseek(arq, 0x00000136, SEEK_SET);
    fread(attr_count, sizeof(u2), 1, arq);
    *attr_count = convertEndianU2(*attr_count);
    
    //printf("%x = Numero de atributos\n", *attr_count);
    return readAttributeInfo(*attr_count, arq, pool, 0);
}





////////////////////////////////////////////////////////////////////////////////////////////////////////
//Interface
////////////////////////////////////////////////////////////////////////////////////////////////////////


u2* interface_decode (FILE *arq, u2 *inter_length, u2 *poll_offset, u2 *interface_offset) {
	//u1 tag;									//1 byte de tag; tem o valor CONSTANT_Class (7)
	u2 name_index;							//2 bytes tem um indice valido para a tabela do pool de constantes
											
	u2 offset = 16 + *poll_offset;			//offset para a interface
	//CONSTANT_Class_info *interface;			//retorno do tipo interface
    u2* interface;
	
		
	fseek(arq, offset, SEEK_SET);					//posicionando o ponteiro do arquivo para ler inteface
	fread(inter_length, sizeof(uint16_t),1 ,arq);	//le o numero de interfaces

	*inter_length = convertEndianU2(*inter_length);	//converte para little endian
	interface = (u2*) malloc ((*inter_length) * sizeof(u2));
	*interface_offset = 2 * (*inter_length);		//calcula o offset da interface, retornado por referencia
	
	//loop que le as interfaces
	for (int i=0; i < *inter_length; i++) {
		fread(&name_index, sizeof(name_index),1 ,arq);
		name_index = convertEndianU2(name_index);
		interface[i] = name_index;
	}
return interface;
}


void fillStaticFields(ClassFile *clsFile_ptr){	
	clsFile_ptr->static_values_size = clsFile_ptr->fields_count;
	clsFile_ptr->static_values = malloc( clsFile_ptr->static_values_size*sizeof(Field_Value) );
	int j = 0;
	for(int i = 0; i < clsFile_ptr->fields_count; i++){
		if( (clsFile_ptr->fields[i].access_flag & 0x0008) ){
			clsFile_ptr->static_values[j].field_name = \
			clsFile_ptr->constant_pool[clsFile_ptr->fields[i].name_index - 1].info.CONSTANT_Utf8_info.bytes;
            clsFile_ptr->static_values[j].descriptor = \
            clsFile_ptr->constant_pool[clsFile_ptr->fields[i].descriptor_index - 1].info.CONSTANT_Utf8_info.bytes;
            j++;
		}
	}
	clsFile_ptr->static_values_size = j;
}

//DOX
u2 buildCPindex(u1 byte1, u2 byte2){
	u2 cpIdx;
	cpIdx = byte2;
	cpIdx = cpIdx << 8;
	cpIdx = cpIdx | byte1;
	return cpIdx;
}
