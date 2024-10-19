#include "alloc.h"
#include <iostream>
using namespace std;

int main()
{
    init_kalloc();
    void* ptr = kalloc(1UL<<21);
    kalloc(1UL<<13);
    kfree(ptr);
    void* ptr1 = kalloc(1UL<<13);

    void* ptr2 = kalloc(1UL<<13);
    kalloc(1UL<<15);
    kfree(ptr1);
    kfree(ptr2);
    kalloc(1UL<<13);

    /*void* ptr = kalloc(1UL<<12);
    kfree(ptr);
    kalloc(1UL<<21);*/




}
