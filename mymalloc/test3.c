#include "mymalloc.h"

int main(int argc, char** argv){
    myinit(1);
    printFreeList();
    char* a;
    a = mymalloc(4);
    printFreeList();
    
    printf("\n\n");

    for(int i=0; i<4; i++){
        a[i]='a'+i*3;
        printf("%c", a[i]);

    }    
    printf("\n\n");
    char* b;
    b = mymalloc(17);
    printf("%p", b);
    for(int i=0; i<17; i++){
        b[i]='a'+i;

        printf("%c", b[i]);

    }
    printf("\n\n");
    a = myrealloc(a, 24);
    char* c;
    c = mymalloc(31);
    for(int i=0; i<31; i++){
        c[i]='a'+i;
        printf("%c", c[i]);
    }
    
    printf("\n\n");
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
    return 0;
}