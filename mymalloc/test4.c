#include "mymalloc.h"

int main(int argc, char** argv){
    myinit(1);
    
    char* a;
    a = mymalloc(4);

    printf("\n\n");

    for(int i=0; i<4; i++){
        a[i]='a'+i*3;
        printf("%c", a[i]);

    }    
    printf("\n\n");
    int* b;
    b = mymalloc(1000001);
    for(int i=0; i<800000; i++){
        b[i]=i;
        printf("%d", b[i]);
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
   printf("%c", a[0]);
   printf("%c", a[1]);
   printf("%c", a[2]);
   printf("%c", a[3]);
       for(int i=4; i<31; i++){
        a[i]='A'+i;
        printf("%c", a[i]);

    }    
    printf("\n\n");
    myfree(c);
   myfree(b);
    myfree(a);
    
    char* d = mymalloc(33);
    printf("Address of d: %p\n", d);
    mycleanup();
    return 0;
}