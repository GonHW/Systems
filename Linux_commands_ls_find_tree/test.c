#include<stdio.h>
#include<stdlib.h>

void f() {
    fork();
    printf("hello\n");
}

int main() {
    fork();
    f();
    fork();
    printf("hello\n");
    return 0;
}