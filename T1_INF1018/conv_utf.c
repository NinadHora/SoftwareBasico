//
//  conv_utf.c
//  T1_INF1018
//
//  Created by Julia Rocha on 5/9/20.
//  Copyright © 2020 Julia Rocha. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "conv_utf.h"

void conv_little_endian(unsigned int * n) {
    unsigned int aux, saida, mask=0xFF;
    int i;

    saida=0x0;

    for( i = 0; i < 4; i++) {
        aux= mask & *n;
        aux<<=(24-8*i);
        saida = saida | aux;
        (*n)>>=8;
    }

    *n=saida;
    return;
}

char GET_BOM (unsigned int byte_bom ) {
    if ( byte_bom == 0Xfffe0000 )
        return 'b';
    if ( byte_bom == 0xFEFF )
        return 'l';
    return 'e';
}

int utf8_32(FILE *arq_entrada, FILE *arq_saida) {
    unsigned int saida, aux;
    int num_byte = 1;
    unsigned char byte_lido;
    int qtd_bytes = 1;
    int i;
    
    while (fread(&byte_lido, 1, 1, arq_entrada) == 1) {
        while(0x8000 & byte_lido) {
            qtd_bytes++;
            byte_lido <<= 1;
        }

        if (qtd_bytes > 4) {
            printf("Erro no byte %d",num_byte);
            return -1;
        }

        if (qtd_bytes == 1)
            saida=(0x0 | byte_lido);
        else {
            aux = byte_lido;

            aux <<= (4*(qtd_bytes-2));

            saida = (0x0 | aux);

            for(i = 0; i < qtd_bytes; i++) {
                
                fread(&byte_lido, 1, 1, arq_entrada);  // le proximo byte


                if(!(0x8000 & byte_lido) || (0x4000 & byte_lido) ) {
                    printf("Erro no byte %d",num_byte);
                    return -1;
                }

                aux = byte_lido;
                aux = (aux & 0x7FFF);  // zera primeiro byte
                aux <<= (6*(qtd_bytes-2-i)); // shifta até a posição correta
                saida = (saida | aux);
            }
        }
        fwrite(&saida, 4, 1, arq_saida); //imprime
        num_byte++;
    }
    return 0;
}

int utf32_8( FILE *arq_entrada , FILE *arq_saida ){

    unsigned int buffer_entrada , aux , bytes=0;

    unsigned char bom, buffer_saida [ 4 ] = { 0 , 0x80 , 0x80 , 0x80 };

    unsigned long elements,escritos;
    int n , i ;

    if (fread(&buffer_entrada, 4, 1, arq_entrada) == 1) {
        fprintf(stderr,"\nErro de entrada: Arquivo termina inesperadamente no byte %u\n",bytes);
        return -1;
    }
    
    bom = GET_BOM(buffer_entrada);
    
    if (bom == 'b') {
        conv_little_endian(&buffer_entrada);
    }
    
    if (bom == 'e') {
        fprintf(stderr,"\nErro de entrada: caracter BOM em formato errado:\n");
        return -1;
    }
    
    do {
        
        elements = fread(&buffer_entrada , 4 , 1 , arq_entrada);

        bytes += 4;

        if (elements == 0 )
            return 0;
        if (bom == 'b') {
            conv_little_endian ( &buffer_entrada );
        }
        if (buffer_entrada > 0x10FFFF) {
            fprintf(stderr,"\nErro de entrada: Algarismo não existe na representação UNICODE. Byte: %u\n",bytes);
            return -1;
        }
        if (elements != 1) {
            fprintf(stderr,"\nErro de entrada: Arquivo corrompido, terminou em um estado instável no byte %u\n",bytes);
            return -1;
        }
        
        n = 1;
        aux = 0xFFFFFF80;

        if (aux & buffer_entrada) {
            n++;
            aux <<= 4 ;
            while (aux & buffer_entrada) {
                n++ ;
                aux <<= 5 ;
            }
        }
        aux = 0x80; // aux agora serve pra colocar os 1's no byte inicial
        buffer_saida[0] = 0;
        
        for (i = 1; i < 4; i++)
            buffer_saida[i]=0x80;
        
        for (i = 0 ; i < n; i++) {
            if (n > 1) {
                buffer_saida [ 0 ] = buffer_saida [ 0 ] | ( aux >> i );
                buffer_saida[i] = (buffer_saida[i] | ( ( buffer_entrada >> ( 6 * (n-1-i) ) ) & 0x3F ) );
            } else {
                buffer_saida[0] = buffer_entrada;
            }
            buffer_saida[i] = (buffer_saida[i] | ( ( buffer_entrada >> ( 6 * (n-1-i) ) ) & 0x3F ) );
        }
        
        escritos = fwrite(buffer_saida , 1 , n , arq_saida);
        
        if (escritos != n) {
            fprintf(stderr,"Erro de saida: Nao foi possivel escrever no arquivo");
        }
    }
    while (elements == 1);
    return 0;
}

