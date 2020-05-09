//
//  main.c
//  T1_INF1018
//
//  Created by Julia Rocha on 5/9/20.
//  Copyright © 2020 Julia Rocha. All rights reserved.
//

#include <stdio.h>
#include <errno.h>
#include "conv_utf.h"

#define ENTRADA_FILENAME "ENTRADA.txt"

#define SAIDA_FILENAME "RESULTADO.txt"

int main (void) {
    FILE *entrada,*saida;
    errno = 0;
    
    saida = fopen(SAIDA_FILENAME,"wb");
    entrada = fopen(ENTRADA_FILENAME, "rb");
    
    if (entrada == NULL) {
        printf("Error %d \n", errno);
        printf("Erro no arquivo de entrada\n");
        return 1;
    }
    
    utf32_8(entrada,saida);
    return 0;
}
