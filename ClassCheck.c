/**
 * Módulo que checa irregularidades APÓS o carregamento da classe
 * 
 * @author Rafael Penna 
 * 		rafaelgpenna@gmail.com
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "LoadClass_core.h"
#include "LoadClass_ui.h"

/**
 * @brief Verifica se a tag é válida para a pool de constantes.
 * @param u1 tag 	Byte que será comparado com as tags válidas da pool de constantes.
 * @return 1 se for uma tag válida, 0 se for inválida.
 */
int isAValidTag (u1 tag) {
	if (!(tag == 7 || tag == 9 || tag == 10 || tag == 11 || tag == 8 || \
		tag == 3 || tag == 4 || tag == 5 || tag == 6 || tag == 12 || \
		tag == 1)) {
			return 0;
	}
	return 1;
}

/**
 * @brief Verifica se o segundo parâmetro está no intervalo que compreende o tamanho da pool.
 * @param ClassFile class 	Estrutura da classe que utiliza a Pool de Constantes.
 * @param u2 index 	Índice que será testado.
 * @return 1 se index for um índice válido da Pool de Constantes. Se não for, retorna 0.
 */
int isAValidPoolIndex (ClassFile class, u2 index) {
	if ((index >= 0) && (index <= class.constant_pool_count - 2)) {
		return 1;
	}
	return 0;
}

/**
 * @brief Verifica se determinada tag ocorre no índice indicado na pool.
 * @param ClassFile class 	Estrutura da classe que utiliza a Pool de Constantes.
 * @param u1 tag 	tag que será comparada.
 * @param u2 index 	Índice do elemento da Pool de Constantes que será testada.
 * @return 1 se o elemento da pool tiver a mesma tag que o segundo parâmetro, 0 caso contrário.
 */
int equalsTags (ClassFile class, u1 tag, u2 index) {
	if (class.constant_pool[index - 1].tag == tag) {
		return 1;
	}
	return 0;
}

/**
 * @brief Verifica se a classe foi carregada adequadamente. Se algum erro for detectado, encerra a execução do programa.
 * @param ClassFile class 	Classe que será verificada.
 */
void classCheck (ClassFile class) {
	u2 major = 0x002e;
	int public, private, protected, final, volat, stat, abstract, sync, native, strict;
	int i, j, k, l, constantV, code, exceptionFlag, source;
	
	printf("Class checking initialized...\n");
	source = 0;
	// --- pool de constantes
	// tamanho da pool = counter - 1, índice começando do zero (-1)
	/*for (i = 0; i < class.constant_pool_count - 1 -1; i++) {
		if (!isAValidTag(class.constant_pool[i].tag)) {
			printf ("A Pool de Constantes não foi completamente lida.\n");
			exit(1);
		}
	}*/
	// --- fields
	// Verifica se os modificadores de acesso são aceitáveis
	for (i = 0; i < class.fields_count; i++) {
		constantV = 0;
		public = class.fields[i].access_flag & ACC_PUBLIC;
		private = class.fields[i].access_flag & ACC_PRIVATE;
		protected = class.fields[i].access_flag & ACC_PROTECTED;
		final = class.fields[i].access_flag & ACC_FINAL;
		volat = class.fields[i].access_flag & ACC_VOLATILE;
		stat = class.fields[i].access_flag & ACC_STATIC;
		if (public && private && protected) {
			printf("Um field não pode assumir os modificadores de acesso PUBLIC, PRIVATE e PROTECTED simultaneamente.\n");
			exit(1);
		}
		if (public && private) {
			printf("Um field não pode assumir os modificadores de acesso PUBLIC e PRIVATE simultaneamente.\n");
			exit(1);
		}
		if (public && protected) {
			printf("Um field não pode assumir os modificadores de acesso PUBLIC e PROTECTED simultaneamente.\n");
			exit(1);
		}
		if (private && protected) {
			printf("Um field não pode assumir os modificadores de acesso PRIVATE e PROTECTED simultaneamente.\n");
			exit(1);
		}
		if (volat && final) {
			printf("Um field não pode assumir os modificadores FINAL e VOLATILE simultaneamente.\n");
			exit(1);
		}
		if (!equalsTags(class, CONSTANT_UTF8, class.fields[i].name_index)) {
			printf("O name index de um field deve representar uma estrutura UTF8 na pool de constantes.\n");
			exit(1);
		}
		if (!equalsTags(class, CONSTANT_UTF8, class.fields[i].descriptor_index)) {
			printf("O descriptor index de um field deve representar uma estrutura UTF8 na pool de constantes.\n");
			exit(1);
		}
		// Verifica se o field é public, static e final, caso seja uma interface
		if (class.access_flags == ACC_INTERFACE) {
			if (!(public && stat && final)) {
				printf("A interface deve ser public, static e final.\n");
				exit(1);
			}
		}

		for(j=0; j < class.fields[i].attributes_count; j++) {
			// Verificação para o caso de atributo ConstantValue
			if(class.fields[i].attributes[j].tag == 0) {
				if (constantV) {
					printf("Field possui mais de um atributo ConstantValue.\n");
					exit(1);
				}
				constantV = 1;
				if (!stat) {
					printf("Field possui atributo ConstantValue sem ser static.\n");
					exit(1);
				}
			}
			// Verificação do Attribute Synthetic
			if (class.fields[i].attributes[j].tag == 4) {
				if (!isAValidPoolIndex(class, class.fields[i].attributes[j].attribute_name_index)) {
					printf("O nome de um Attribute Synthetic não está na pool de constantes.\n");
					exit(1);
				}
				if (!equalsTags( class, CONSTANT_UTF8, class.fields[i].attributes[j].attribute_name_index)) {
					printf("O nome do Attribute Synthetic na Pool de Constantes não é do tipo UTF8.\n");
					exit(1);
				}
				if (class.fields[i].attributes[j].attribute_length != 0) {
					printf("Attribute_Length em Attribute Synthetic deve ser zero.\n");
				}
			}
			if (class.fields[i].attributes[j].tag == 8) {
				if(!isAValidPoolIndex(class, class.fields[i].attributes[j].attribute_name_index)) {
					printf("Índice para o nome do Attribute Deprecated não encontrado na pool de constantes.\n");
					exit(1);
				}
				if(!equalsTags(class, CONSTANT_UTF8, class.fields[i].attributes[j].attribute_name_index)) {
					printf("O nome do Attribute Deprecated não está em UTF8 na pool de constantes.\n");
					exit(1);
				}
				if (class.fields[i].attributes[j].attribute_length != 0) {
					printf("O tamanho do Attribute Deprecated deve ser sempre zero.\n");
				}
			}
		}
	}
	// --- methods
	for (i = 0; i < class.methods_count; i++) {
		code = 0;
		exceptionFlag = 0;
		public = class.methods[i].access_flags & ACC_PUBLIC;
		private = class.methods[i].access_flags & ACC_PRIVATE;
		protected = class.methods[i].access_flags & ACC_PROTECTED;
		final = class.methods[i].access_flags & ACC_FINAL;
		abstract = class.methods[i].access_flags & ACC_ABSTRACT;
		stat = class.methods[i].access_flags & ACC_STATIC;
		sync = class.methods[i].access_flags & ACC_SYNCHRONIZED;
		native = class.methods[i].access_flags & ACC_NATIVE;
		strict = class.methods[i].access_flags & ACC_STRICT;
		if (public && private && protected) {
			printf("Um method não pode assumir os modificadores de acesso PUBLIC, PRIVATE e PROTECTED simultaneamente.\n");
			exit(1);
		}
		if (public && private) {
			printf("Um method não pode assumir os modificadores de acesso PUBLIC e PRIVATE simultaneamente.\n");
			exit(1);
		}
		if (public && protected) {
			printf("Um method não pode assumir os modificadores de acesso PUBLIC e PROTECTED simultaneamente.\n");
			exit(1);
		}
		if (private && protected) {
			printf("Um method não pode assumir os modificadores de acesso PRIVATE e PROTECTED simultaneamente.\n");
			exit(1);
		}
		if (abstract && (strict || sync || final || native || private || stat)) {
			printf ("Se for ABSTRACT, um method só pode ser PUBLIC ou PROTECTED\n");
			exit(1);
		}
		// Verifica se o method é public e abstract, caso seja de uma interface

		if (class.access_flags & ACC_INTERFACE) {
			if (!(public && abstract && !(strict || sync || final || native || private || stat || protected))) {
				printf("O método de uma interface deve ser apenas PUBLIC e ABSTRACT.\n");
				exit(1);
			}
		}
		// Verifica se é um método de inicialização
		if (!strcmp("<init>", class.constant_pool[class.methods[i].name_index].info.CONSTANT_Utf8_info.bytes) ||\
		!strcmp("<clinit>", class.constant_pool[class.methods[i].name_index].info.CONSTANT_Utf8_info.bytes)) {
			if (strict) {
				if (abstract || sync || final || native || stat ) {
					printf("Se for STRICT, um método <init> ou <clinit>, não pode ter nenhum outro modificador que não sejam PUBLIC, PROTECTED OU PRIVATE.\n");
					exit(1);
				}
			}
		}
		if (!equalsTags(class, CONSTANT_UTF8, class.methods[i].descriptor_index)) {
			printf("O descriptor index de um method deve representar uma estrutura UTF8 na pool de constantes.\n");
			exit(1);
		}
		for(j=0; j < class.methods[i].attributes_count; j++) {
			// Verificação do Code
			if (class.methods[i].attribute[j].tag == 1) {
				if (code) {
					printf("Método possui mais de um atributo code.\n");
					exit(1);
				}
				else {
					if (native || abstract) {
						printf ("Métodos NATIVE ou BASTRACT não possuem atributo Code.\n");
						exit(1);
					}
					if (class.methods[i].attribute[j].info.Code_attribute.code_length <= 0) {
						printf("Atributo Code de um método possui tamanho menor ou igual a zero.\n");
						exit(1);
					}
					for (k = 0; k < class.methods[i].attribute[j].info.Code_attribute.exception_table_length; k++) {
						if (class.methods[i].attribute[j].info.Code_attribute.exception_table[k].start_pc < 0 || \
						class.methods[i].attribute[j].info.Code_attribute.exception_table[k].start_pc >= class.methods[i].attribute[j].info.Code_attribute.code_length) {
							printf("StartPC de um a exceção do Code é inválido.\n");
							exit(1);
						}
						if (class.methods[i].attribute[j].info.Code_attribute.exception_table[k].end_pc < 0 || \
						class.methods[i].attribute[j].info.Code_attribute.exception_table[k].end_pc > class.methods[i].attribute[j].info.Code_attribute.code_length) {
							printf("EndPC de um a exceção do Code é inválido.\n");
							exit(1);
						}
						if (class.methods[i].attribute[j].info.Code_attribute.exception_table[k].handler_pc < 0 || \
						class.methods[i].attribute[j].info.Code_attribute.exception_table[k].handler_pc >= class.methods[i].attribute[j].info.Code_attribute.code_length) {
							printf("HandlerPC de um a exceção do Code é inválido.\n");
							exit(1);
						}
						if (class.methods[i].attribute[j].info.Code_attribute.exception_table[k].catch_type != 0) {
							if (class.methods[i].attribute[j].info.Code_attribute.exception_table[k].catch_type < 0 || \
							class.methods[i].attribute[j].info.Code_attribute.exception_table[k].catch_type >= class.constant_pool_count - 2) {
								printf("Catch Type de um a exceção do Code é inválido.\n");
								exit(1);
							}
						}
					}
					for (k = 0; k < class.methods[i].attribute[j].info.Code_attribute.attribute_count; k++) {
						// Verificação do LineNumber
						if (class.methods[i].attribute[j].info.Code_attribute.attribute[k].tag == 6) {
							if (!isAValidPoolIndex(class, class.methods[i].attribute[j].info.Code_attribute.attribute[k].attribute_name_index)) {
								printf ("Índice de nome de attribute da Line Number Table inválido.\n");
								exit(1);
							}
							if (!equalsTags(class, CONSTANT_UTF8, class.methods[i].attribute[j].info.Code_attribute.attribute[k].attribute_name_index)) {
								printf ("Nome do attribute do LineNumber Table não está no formato UTF8 na Pool de Constantes.\n");
								exit(1);
							}
							for (l = 0; l < class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LineNumberTable_attribute.line_number_table_length; l++) {
								if (class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LineNumberTable_attribute.line_number_table[l].start_pc >= class.methods[i].attribute[j].info.Code_attribute.code_length) {
									printf("Índice para o Code Array do Line Number Table inválido.\n");
									exit(1);
								}
							}
						}
						// Verificação do LocalVariableTable
						if (class.methods[i].attribute[j].info.Code_attribute.attribute[k].tag == 7) {
							if (!isAValidPoolIndex(class, class.methods[i].attribute[j].info.Code_attribute.attribute[k].attribute_name_index)) {
								printf ("Índice de nome de attribute da Local Variable Table inválido.\n");
								exit(1);
							}
							if (!equalsTags(class, CONSTANT_UTF8, class.methods[i].attribute[j].info.Code_attribute.attribute[k].attribute_name_index)) {
								printf ("Nome do attribute do Local Variable Table não está no formato UTF8 na Pool de Constantes.\n");
								exit(1);
							}	
							for (l = 0; l < class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LocalVariableTable_attribute.local_variable_length; l++) {
								if (class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LocalVariableTable_attribute.local_variable_table[l].start_pc >= class.methods[i].attribute[j].info.Code_attribute.code_length) {
									printf("Índice para o Code Array do Local Variable Table inválido.\n");
									exit(1);
								}
								if (class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LocalVariableTable_attribute.local_variable_table[l].start_pc + \
								class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LocalVariableTable_attribute.local_variable_table[l].length > class.methods[i].attribute[j].info.Code_attribute.code_length + 1) {
									printf("Índice para o Code Array do Local Variable Table somado ao seu tamanho é inválido.\n");
									exit(1);
								}
								if(!isAValidPoolIndex(class, class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LocalVariableTable_attribute.local_variable_table[l].name_index)) {
									printf("Nome de elemento da Local Variable Table não está na Pool de Constantes.\n");
									exit(1);
								}
								if(!isAValidPoolIndex(class, class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LocalVariableTable_attribute.local_variable_table[l].descriptor_index)) {
									printf("Descritor de elemento da Local Variable Table não está na Pool de Constantes.\n");
									exit(1);
								}
								if(!equalsTags(class, CONSTANT_UTF8, class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LocalVariableTable_attribute.local_variable_table[l].name_index)) {
									printf("Nome de elemento da Local Variable Table não está na Pool de Constantes como um UTF8.\n");
									exit(1);
								}
								if(!equalsTags(class, CONSTANT_UTF8, class.methods[i].attribute[j].info.Code_attribute.attribute[k].info.LocalVariableTable_attribute.local_variable_table[l].descriptor_index)) {
									printf("Nome de elemento da Local Variable Table não está na Pool de Constantes como um UTF8.\n");
									exit(1);
								}
							}
						}				
					}
				}
				code = 1;
			}
			// Verificação dos Exceptions
			if (class.methods[i].attribute[j].tag == 2) {
				if (exceptionFlag) {
					printf("Método com mais de uma Exception.\n");
					exit(1);
				}
				else {
					exceptionFlag = 1;
					if (!isAValidPoolIndex(class, class.methods[i].attribute[j].attribute_name_index)) {
						printf("Nome de um atributo Exception não localizado na Pool de Constantes.\n");
						exit(1);
					}
					for (k = 0; k < class.methods[i].attribute[j].info.Exception_attribute.number_of_exceptions; k++) {
						if (!isAValidPoolIndex(class, class.methods[i].attribute[j].info.Exception_attribute.exception_index_table[k])) {
							printf("Atributo Exception não localizado na Pool de Constantes.\n");
							exit(1);
						}
						if (!equalsTags(class, CONSTANT_CLASS, class.methods[i].attribute[j].info.Exception_attribute.exception_index_table[k])) {
							printf("Atributo Exception na pool de constantes não é do tipo CONSTANT_Class_info.\n");
							exit(1);
						}
					}
				}
			}
			// Attribute Synthetic
			if (class.methods[i].attribute[j].tag == 4) {
				if (!isAValidPoolIndex(class, class.methods[i].attribute[j].attribute_name_index)) {
					printf("O nome de um Attribute Synthetic não está na pool de constantes.\n");
					exit(1);
				}
				if (!equalsTags( class, CONSTANT_UTF8, class.methods[i].attribute[j].attribute_name_index)) {
					printf("O nome do Attribute Synthetic na Pool de Constantes não é do tipo UTF8.\n");
					exit(1);
				}
				if (class.methods[i].attribute[j].attribute_length != 0) {
					printf("Attribute_Length em Attribute Synthetic deve ser zero.\n");
					exit(1);
				}
			}
			// Verificação do Deprecated
			if (class.methods[i].attribute[j].tag == 8) {
				if(!isAValidPoolIndex(class, class.methods[i].attribute[j].attribute_name_index)) {
					printf("Índice para o nome do Attribute Deprecated não encontrado na pool de constantes.\n");
					exit(1);
				}
				if(!equalsTags(class, CONSTANT_UTF8, class.methods[i].attribute[j].attribute_name_index)) {
					printf("O nome do Attribute Deprecated não está em UTF8 na pool de constantes.\n");
					exit(1);
				}
				if (class.methods[i].attribute[j].attribute_length != 0) {
					printf("O tamanho do Attribute Deprecated deve ser sempre zero.\n");
				}
			}
		}
		if (!(native || abstract) && !code) {
			printf ("Método não possui o atributo Code.\n");
			exit(1);
		}
	}
	// --- Attributes
	for (i = 0; i < class.attributes_count; i++) {
		// Verificação de InnerClasses
		if (class.attributes[i].tag == 3) {
			if (!isAValidPoolIndex(class, class.attributes[i].attribute_name_index)) {
				printf("O nome de uma InnerClass não está na pool de constantes.\n");
				exit(1);
			}
			if (!equalsTags( class, CONSTANT_UTF8, class.attributes[i].attribute_name_index)) {
				printf("O nome da InnerClass na Pool de Constantes não é do tipo UTF8.\n");
				exit(1);
			}
			for (j = 0; j < class.attributes[i].info.InnerClasses_attribute.number_of_classes; j++) {
				if (!isAValidPoolIndex(class, class.attributes[i].info.InnerClasses_attribute.classes[j].inner_class_info_index)) {
					printf("A InnerClass não está na pool de constantes.\n");
					exit(1);
				}
				if (class.attributes[i].info.InnerClasses_attribute.classes[j].outer_class_info_index != 0) {
					if (!isAValidPoolIndex(class, class.attributes[i].info.InnerClasses_attribute.classes[j].outer_class_info_index)) {
							printf("A OuterClass não está na pool de constantes.\n");
							exit(1);
					}
					if (!equalsTags(class, CONSTANT_CLASS, class.attributes[i].info.InnerClasses_attribute.classes[j].outer_class_info_index)) {
						printf("OuterClassIndex não tem tipo CONSTANT_Class_info na pool de constantes.\n");
						exit(1);
					}
				}
				if (class.attributes[i].info.InnerClasses_attribute.classes[j].inner_name_index != 0) {
					if(!isAValidPoolIndex(class, class.attributes[i].info.InnerClasses_attribute.classes[j].inner_name_index)) {
						printf("Inner_Name_Index não é um índice válido na Pool de Constantes.\n");
						exit(1);
					}
					if(!equalsTags(class, CONSTANT_UTF8, class.attributes[i].info.InnerClasses_attribute.classes[j].inner_name_index)) {
						printf("Inner_Name_Index deve representar um índice com tag CONSTANT_UTF8_info na Pool de constantes.\n");
						exit(1);
					}
				}
			}
		}
		// Verificação do Attribute Synthetic
		if (class.attributes[i].tag == 4) {
			if (!isAValidPoolIndex(class, class.attributes[i].attribute_name_index)) {
				printf("O nome de um Attribute Synthetic não está na pool de constantes.\n");
				exit(1);
			}
			if (!equalsTags( class, CONSTANT_UTF8, class.attributes[i].attribute_name_index)) {
				printf("O nome do Attribute Synthetic na Pool de Constantes não é do tipo UTF8.\n");
				exit(1);
			}
			if (class.attributes[i].attribute_length != 0) {
				printf("Attribute_Length em Attribute Synthetic deve ser zero.\n");
				exit(1);
			}
		}		
		// Verificação do Attribute SourceFile
		if (class.attributes[i].tag == 5) {
			if (source) {
				printf("Mais de um Attribute SourceFile na mesma classe.\n");
				exit(1);
			}
			else {
				source = 1;
				if(!isAValidPoolIndex(class, class.attributes[i].attribute_name_index)) {
					printf("Índice para o nome do Attribute SourceFile não encontrado na pool de consantes.\n");
					exit(1);
				}
				if (!equalsTags(class, CONSTANT_UTF8, class.attributes[i].attribute_name_index)) {
					printf("O nome do Attribute SourceFile não está em UTF8 na Pool de Constantes.\n");
					exit(1);
				}
				if (class.attributes[i].attribute_length != 2) {
					printf("O attribute_length de um SourceFile deve ser sempre 2.\n");
					exit(1);
				}
				if(!isAValidPoolIndex(class, class.attributes[i].info.SourceFile_attribute.sourcefile_index)) {
					printf("Índice para o nome do Attribute SourceFile não encontrado na pool de consantes.\n");
					exit(1);
				}
				if (!equalsTags(class, CONSTANT_UTF8, class.attributes[i].info.SourceFile_attribute.sourcefile_index)) {
					printf("O nome do Attribute SourceFile não está em UTF8 na Pool de Constantes.\n");
					exit(1);
				}
			}
		}
		// Verificação do Attribute Deprecated
		if (class.attributes[i].tag == 8) {
			if(!isAValidPoolIndex(class, class.attributes[i].attribute_name_index)) {
				printf("Índice para o nome do Attribute Deprecated não encontrado na pool de constantes.\n");
				exit(1);
			}
			if(!equalsTags(class, CONSTANT_UTF8, class.attributes[i].attribute_name_index)) {
				printf("O nome do Attribute Deprecated não está em UTF8 na pool de constantes.\n");
				exit(1);
			}
			if (class.attributes[i].attribute_length != 0) {
				printf("O tamanho do Attribute Deprecated deve ser sempre zero.\n");
			}
		}
	}
	printf("Class checking successfull!\n");
}
