#include "complex.h"
#include <stdio.h>
#include <math.h>

//this file just contains functions to deal with complex numbers

struct cNum cAdd(struct cNum a, struct cNum b){ //Addition
    struct cNum cRes;
    cRes.real = a.real + b.real;
    cRes.imag = a.imag + b.imag;
    return cRes;
}

struct cNum cSqr(struct cNum n){ //Squaring
    struct cNum cRes;
    cRes.real = n.real * n.real - n.imag * n.imag;
    cRes.imag = n.real * n.imag * 2;
    return cRes;
}

double cMod(struct cNum n){ //Modulus
    return sqrt(n.real*n.real + n.imag*n.imag);
}

void cPrint(struct cNum n){
    printf("%f + %fi\n", n.real, n.imag);
}


