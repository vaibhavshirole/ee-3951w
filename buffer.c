/*
 * File:        buffer.c
 * Author:      Abdullah, Vaibhav
 * Instructors: Bosi Yang, Ted Higman
 * Course:      EE 3951W (004) - Junior Design Project
 */

#include "xc.h"
#include "buffer.h"
#define BUFSIZE 12

volatile int adc_buffer[BUFSIZE];
volatile int bufIndex = 0;

void initBuffer(){
    int i;
    for(i = 0; i < BUFSIZE; i++){
        adc_buffer[i] = 0;
    }
}

void putVal(int newVal){
    adc_buffer[bufIndex++] = newVal;
    if(bufIndex >= BUFSIZE){
        bufIndex = 0;
    }
}

int getAvg(){
    double total, result;
    int i;
    total = 0.0;
    for(i = 0; i < BUFSIZE; i++){
        total += adc_buffer[i];
    }
    result = total/BUFSIZE;
    return result;
}
