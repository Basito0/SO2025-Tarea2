#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    int page;      
    bool ocupado;  
    bool useBit;   
} Frame;

int main(int argc, char* argv[]){

    int nmarcos;
    int tmarcos;
    char file[256];
    bool verbose = false;

    if(argc == 5){
        verbose = true;
        strcpy(file, argv[4]);
    }
    else if(argc == 4){
        strcpy(file, argv[3]);
    }
    else{
        printf("El uso correcto del comando es ./a.out Nmarcos tamanomarco [--verbose] traza.txt  o ./sim Nmarcos tamanomarco traza.txt\n");
        return 1;
    }

    nmarcos = atoi(argv[1]);
    tmarcos = atoi(argv[2]);

    FILE* f = fopen(file, "r");
    if(f == NULL){
        printf("No se pudo abrir el archivo\n");
        return 1;
    }

    Frame* frameTable = calloc(nmarcos, sizeof(Frame));
    int clockHand = 0;

    char buffer[64];
    int references = 0;
    int faults = 0;

    size_t bytesRead;
    while(fgets(buffer, sizeof(buffer), f)){
        int num = (int)strtol(buffer, NULL, 16);
        int MASK = tmarcos - 1;
        int offset = num & MASK;
        int b = (int)log2(tmarcos);
        int npv = num >> b;

        references++;

        bool hit = false;

        
        int marco = -1;
        //ve si el npv ya esta en frames
        //UseBit se va a usar para el algoritmo de reloj
        for (int i = 0; i < nmarcos; i++) {
            if (frameTable[i].ocupado && frameTable[i].page == npv) {
                marco = i; 
                frameTable[i].useBit = true;
                hit = true;
                break;
            }
        }

        if (marco == -1) {
            faults++;

            
            int freeFrame = -1;
            //buscar frame libre
            for (int i = 0; i < nmarcos; i++) {
                if (!frameTable[i].ocupado) {
                    freeFrame = i;
                    break;
                }
            }

            if (freeFrame != -1) {
                marco = freeFrame;
            } else {
                while (true) {
                    //algoritmo reloj
                    if (!frameTable[clockHand].useBit) {
                        marco = clockHand;
                        break;
                    } else {
                        frameTable[clockHand].useBit = false;
                        clockHand = (clockHand + 1) % nmarcos;
                    }
                }
            }

            frameTable[marco].page = npv;
            frameTable[marco].ocupado = true;
            frameTable[marco].useBit = true;
            clockHand = (marco + 1) % nmarcos;
        }

        int DF = (marco << b) | offset;

        if (verbose) {
            printf("DV=%X npv=%d offset=%d %s marco=%d DF=%X\n",
                   num, npv, offset,
                   (hit == false ? "FALLO" : "HIT"), marco, DF);
        }
    }

    fclose(f);

    double faultRate = (double)faults / references;
    printf("Totales: Referencias=%d Fallos=%d Tasa=%.2f\n",
           references, faults, faultRate);

    free(frameTable);

    return 0;
}