/* Júlia Affonso Figueiredo Rocha 1710635 3WA */
/* Ana Carolina da Hora 1521133 3WB */

#include <stdio.h>
#include <string.h>
#include "conv_utf.h"

void to_littleEndian(unsigned int * bytes) {
    unsigned int current;
    unsigned int outBuffer =0x0;
    unsigned int mask=0xFF;
    int counter;
    for (counter = 0; counter < 4; counter++) {
        current = mask & *bytes;
        current <<= (24-8 * counter);
        outBuffer = outBuffer | current;
        (*bytes) >>= 8;
    }
    *bytes = outBuffer;
    return;
}

int get_BomValue (unsigned int bom ) {
    if ( bom == 0xFEFF )
        return 1;
    if ( bom == 0Xfffe0000 )
        return 2;
    return -1;
}

int utf8_32(FILE *arq_entrada, FILE *arq_saida) {
    unsigned int outBuffer;
    unsigned int current;
    unsigned char currentByte;
    int byteCounter = 1;
    int byteQtd = 1;
    int counter;
    
    while (fread(&currentByte, 1, 1, arq_entrada) == 1) {
        while(0x8000 & currentByte) {
            byteQtd++;
            currentByte <<= 1;
        }

        if (byteQtd > 4) {
            fprintf(stderr, "Erro E/S: leitura do arquivo de entrada");
            return -1;
        }

        if (byteQtd == 1)
            outBuffer=(0x0 | currentByte);
        
        else {
            current = currentByte;
            current <<= (4*(byteQtd-2));
            outBuffer = (0x0 | current);

            for(counter = 0; counter < byteQtd; counter++) {
                fread(&currentByte, 1, 1, arq_entrada);
                current = currentByte;
                current = (current & 0x7FFF);
                current <<= (6*(byteQtd-2-counter));
                outBuffer = (outBuffer | current);
            }
        }
        fwrite(&outBuffer, 4, 1, arq_saida);
        byteCounter++;
    }
    printf("Conversao 8 para 32 bem sucedida\n");
    return 0;
}

int utf32_8( FILE *arq_entrada , FILE *arq_saida ){
    unsigned int inBuffer;
    unsigned int current;
    unsigned int bytes = 0;
    unsigned int bom;
    unsigned long currentBytes;
    unsigned long writtenBytes;
    unsigned char outBuffer [ 4 ] = { 0 , 0x80 , 0x80 , 0x80 };
    int bytesCounter;
    int counter;

    if (fread(&inBuffer, 4, 4, arq_entrada) == 1) {
        fprintf(stderr,"\nErro E/S: leitura do arquivo de entrada");
        return -1;
    }
    
    bom = get_BomValue(inBuffer);
    
    if (bom == -1) {
        fprintf(stderr,"\nErro E/S: leitura do caracter BOM");
        return -1;
    }
    
    if (bom == 2) {
        to_littleEndian(&inBuffer);
    }
    
    do {
        currentBytes = fread(&inBuffer , 4 , 4 , arq_entrada);
        bytes += 4;
        
        if (currentBytes == 0 )
            return 0;
        
        if (bom == 2) {
            to_littleEndian ( &inBuffer );
        }
        
        bytesCounter = 1;
        current = 0xFFFFFF80;

        if (current & inBuffer) {
            bytesCounter++;
            current <<= 4 ;
            while (current & inBuffer) {
                bytesCounter++ ;
                current <<= 5 ;
            }
        }
        current = 0x80;
        outBuffer[0] = 0;
        
        for (counter = 1; counter < 4; counter++)
            outBuffer[counter] = current;
        
        for (counter = 0 ; counter < bytesCounter; counter++) {
            if (bytesCounter > 1) {
                outBuffer [0] = outBuffer [0] | ( current >> counter );
                outBuffer[counter] = (outBuffer[counter] | ( ( inBuffer >> ( 6 * (bytesCounter-1-counter) ) ) & 0x3F ) );
            } else {
                outBuffer[0] = inBuffer;
            }
            outBuffer[counter] = (outBuffer[counter] | ( ( inBuffer >> ( 6 * (bytesCounter-1-counter) ) ) & 0x3F ) );
        }
        
        writtenBytes = fwrite(outBuffer , 1 , bytesCounter , arq_saida);
        
        if (writtenBytes != bytesCounter) {
            fprintf(stderr,"Erro E/S: escrita do arquivo de saida");
        }
    }
    while (currentBytes == 1);
    printf("Conversao 32 para 8 bem sucedida\n");
    return 0;
}
