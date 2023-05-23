#include "virus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


bool create_text_and_write() {
    char *user = getenv("USER"); // obter o nome do usuário
    if (user == NULL) { // verificar se foi possível obter o nome do usuário
        printf("Não foi possível obter o nome do usuário.\n");
        return false;
    }
    
    // montar o caminho do arquivo na área de trabalho do usuário
    char path[2000];

    
    
    snprintf(path, sizeof(path), "/home/%s/Desktop/arquivo.txt", user);
    
    // criar o arquivo e escrever o conteúdo
    FILE *file = fopen(path, "w");
    if (file == NULL) { // verificar se foi possível criar o arquivo
        printf("Não foi possível criar o arquivo.\n");
        return false;
    }
    fputs("Teste, passei por aqui hehehehehe", file);
    fclose(file);
    
    printf("Arquivo criado com sucesso em: %s\n", path);
    
    return true;
}
