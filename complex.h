#ifndef COMPLEX_H_INCLUDED
#define COMPLEX_H_INCLUDED

struct cNum{
    double real;
    double imag;
};

struct cNum cAdd(struct cNum a, struct cNum b);

struct cNum cSqr(struct cNum n);

double cMod(struct cNum n);

void cPrint(struct cNum n);

#endif // COMPLEX_H_INCLUDED
