/**
 * @file weight.h
 * 
 * Header file for weight sensor interface
 * 
 */
#ifndef WEIGHT_H
#define WEIGHT_H

#include <stdint.h>

/* Initialize weight sensor */
void weight_init();

/* Read weight value based on offset */
int32_t weight_read();

#endif /* WEIGHT_H */