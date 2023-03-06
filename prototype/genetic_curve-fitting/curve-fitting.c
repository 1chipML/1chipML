#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serial_port.h"
#include <stdint.h>



const char* getfield(char* line, unsigned int num)
{
    const char* tok;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

int main()
{
    FILE* stream = fopen("eng-daily-01012015-12312015.csv", "r");
    
    char line[369];

    uint16_t coordinatesSize = 365;

    float coordinates [coordinatesSize] ;
    unsigned int index = 0 ;
    float epsilon = 0.0001;
    float mutationRate = 0.1;
    uint16_t populationSize =  25;
    uint16_t tourneySize = 5;
    uint16_t maxIterations = 2000;


    for (unsigned i = 0 ;fgets(line, 369, stream);i++)
    {
        const char * temperature = getfield(line, 8);

        if (temperature != NULL && i > 2){
            coordinates[index] = atof(temperature);
            index++;
        }
        
    }

    for (unsigned int i = 0 ; i < 365 ; i++){
        printf("Temperature %f \n",coordinates[i]);
    }


    const unsigned BAUD_RATE = 115200;
    const unsigned SERIAL_BUFFER_SIZE = 32;
    char* serialPortName = "/dev/ttyACM1";

    int exitCode = 0;
  
    // Open the serial port
    exitCode = openSerialPort(serialPortName, SERIAL_BUFFER_SIZE, BAUD_RATE);
    if (exitCode != 0) {
      return exitCode;
    }

    writeElement(&epsilon, sizeof(epsilon));
    writeElement(&mutationRate, sizeof(mutationRate));
    writeElement(&populationSize, sizeof(populationSize));
    writeElement(&tourneySize, sizeof(tourneySize));
    writeElement(&maxIterations, sizeof(maxIterations));

    uint16_t testSize = 5;
    writeElement(&testSize, sizeof(testSize));

    float testCoords[]  =  {0,1,4,9,16};
    // for (int i =  0 ; i < testSize ; i++){
    //     testCoords[i] = coordinates[i];
    // }

    
    
    writeArray(testSize,testCoords,sizeof(float));
    printf("Here\n ");

    float fitness = 0;
    float bestValues[4];


    readElement(&testSize,sizeof(testSize));

    readElement(&fitness,sizeof(fitness));

    printf("Size %u \n",testSize);


    printf("Fitness %f \n",fitness);

    readArray(4,bestValues,sizeof(float));

    printf("Best Coordinates %f %f %f %f \n",bestValues[0],bestValues[1],bestValues[2],bestValues[3]);



    closeSerialPort();




    

}