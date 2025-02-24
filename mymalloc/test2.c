#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>


int main(int argc, char** argv){
    myinit(2);
    printFreeList();
    char* a;
    a = mymalloc(4);
    printFreeList();
    char* b;
    b = mymalloc(17);
    printFreeList();
    printf("Address of a: %p\n", a);
    printf("Address of b: %p\n", b);
    a = myrealloc(a, 24);
    char* c;
    c = mymalloc(31);
    printFreeList();
    printf("Address of c: %p\n", c);
    myfree(c);
    printFreeList();
    myfree(a);
    printFreeList();
    char* d = mymalloc(33);
    printFreeList();
    printf("Address of d: %p\n", d);
    mycleanup();
}
