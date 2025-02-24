#include <stdio.h>
#include "mymalloc.h"



int main(){
    
    
    int* test = MyMalloc(2*sizeof(int));
    test[0] = 5;
    test[1] = 10;
    for(int i=0; i<2; i++){
        printf("%d\n", (int)sizeof(test)); 
    }
    int* test2 = MyMalloc(10*sizeof(int));
    test2[0] = 5;
    test2[1] = 1;
    test2[2] = 75;
    test2[3] = 120;
    test2[4] = 5;
    
    test2[6] = 51;
    test2[7] = 130;
    test2[8] = 25;
    test2[9] = 1;
    for(int i=0; i<10; i++)
        printf("%d\n", test2[i]);     

    int* test3 = MyMalloc(10*sizeof(int));
    test3[0] = 15;
    test3[1] = 12;
    test3[2] = 53;
    test3[3] = 12;
    test3[4] = 25;
    
    test3[6] = 41;
    test3[7] = 13;
    test3[8] = 2;
    test3[9] = 247;
    for(int i=0; i<10; i++)
        printf("%d\n", test3[i]);   

    
    int* test4 = MyMalloc(5*sizeof(int));
    test4[0] = 15;
    test4[1] = 12;
    test4[2] = 53;
    test4[3] = 12;
    test4[4] = 25;
    
    for(int i=0; i<10; i++)
        printf("%d\n", test4[i]);   
//MyFree(test2);
//    MyFree(test);
//    MyFree(test3);
//    MyFree(test4);
    return 0;
}
