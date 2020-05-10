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

#define ENTRADA8_FILENAME "/Users/juliarocha/Desktop/PUC/INF1018/SoftwareBasico/T1_INF1018/arq_entrada8.txt"

#define ENTRADA32_FILENAME "/Users/juliarocha/Desktop/PUC/INF1018/SoftwareBasico/T1_INF1018/arq_entrada32.txt"

#define SAIDA_FILENAME "/Users/juliarocha/Desktop/PUC/INF1018/SoftwareBasico/T1_INF1018/arq_saida.txt"

int main (void) {
    FILE *entrada,*saida;
    errno = 0;
    
    entrada = fopen(ENTRADA32_FILENAME,"rb");
    saida = fopen(SAIDA_FILENAME, "wb");
    
    if (entrada == NULL) {
        printf("Error %d \n", errno);
        printf("Erro no arquivo de entrada\n");
        return 1;
    }
    
    utf32_8(entrada,saida);
    return 0;
}
