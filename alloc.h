#include "list.h"
#include "helpers.h"
#include <iostream>
#include <cstring>
using namespace std;

#define MAXORDER 21
#define MINORDER 12
#define PAGESIZE (1UL << 12)
#define NPAGES 512 // MEMSIZE_PHYSICAL / PAGESIZE


// Next free physical address for kalloc

void init_kalloc();
void* kalloc(size_t sz);
void kfree(void* ptr); 

struct page {
    list_links p;
    uintptr_t pa;
    int order;
    bool is_freed = 1;
};



uintptr_t getBuddy(uintptr_t pa, int o)
{
    return (pa^(1<<o));
    // get the address of the buddy;
}

// init_kalloc
//    Initialize stuff needed by `kalloc`. Should be called
//    in testpset1.cpp before using 'kalloc' and 'bfree'. 
    list<page,&page::p> free_list[22];
    page page_array[NPAGES+1];

void init_kalloc() {
    // YOUR CODE HER
    page_array[0].order=21;
    page_array[0].pa = 0;
    for(int i=0;i<=511;i++){
        page_array[i].pa=i*4096;
    }
    free_list[21].push_back(&page_array[0]);
    // initialize the code. Originally the free_list would only have a block with order 21;
}


// kalloc(sz)
//    Allocate and return a pointer to at least `sz` contiguous bytes of
//    memory. Returns `nullptr` if `sz == 0` or on failure.
//
//    The caller should initialize the returned memory before using it.
//    The handout allocator sets returned memory to 0xCC 
void* kalloc(size_t sz) 
{
    
    if (sz==0||sz > (1UL << 21)){
        return nullptr;
    } 
    

    int a = msb(sz-1); // a represents the order the data need
    if(a<12){
        a=12;
    }
    int b = a; //  b will be used to find the order that currently have a free block

     /* if next free block on page array have the size we need,
    we allocate it*/

    bool success = 0;
    if(!free_list[a].empty()){
        uintptr_t help = free_list[a].front()->pa; // help is the address of the block I am going to alloate in.
        free_list[a].erase(free_list[a].front());
        page_array[help/4096].is_freed = 0;
        cout<<"data allocated"<<endl;
        return pa2kptr(page_array[help/4096].pa);
    }//if I already have a free block with order I need, I store the data in the block


    for(b=a;b<=21;b++){
        if(!free_list[b].empty()){
            success = 1;
            break;
        }
    }// if not, I find the min order that currently have a free block
    // if success, that means there are still enough room to store the data; otherwise there is not.
    if(success){
        while(b!=a){
            page* block = free_list[b].front();
            free_list[b].erase(free_list[b].front());
            b--;
            uintptr_t ads = block -> pa;
            int index = ads/4096;
            free_list[b].push_back(&page_array[index]);
            page_array[index].order = b;
            int index1 = getBuddy(ads,b)/4096;
            page_array[index1].order = b;
            free_list[b].push_back(&page_array[index1]);
            
        }// deviding the blocks to the order I need

        free_list[b].front()->is_freed=0;
        void* store = pa2kptr(free_list[b].front()->pa);
        free_list[b].erase(free_list[b].front());
        cout<<"data allocated"<<endl;
        return store;// allocate it

    }else{

        cout<<"No free blocks available"<<endl;
        return nullptr;

    }
        
}





// kfree(ptr)
//    Free a pointer previously returned by `kalloc`. Does nothing if
//    `ptr == nullptr`.
void kfree(void* ptr) {
    // YOUR CODE HERE
    
    if(ptr == nullptr){
        return;
    }// return if null ptr
   
    uintptr_t address= kptr2pa(ptr);
    uintptr_t a = address/4096;
    int order = page_array[a].order;
    
    page_array[a].is_freed = 1;
    free_list[order].push_front(&page_array[a]);
    // free the block

    uintptr_t b = getBuddy(address,order);
    b/=4096;

    // if the buddy of the block is also free, combine them together
    while(page_array[b].is_freed&&page_array[a].order<21){
       /*cout<<a<<" "<<b<<endl;
        cout<<page_array[a].order<<endl;
        cout<<page_array[b].order<<endl;*/

        free_list[order].erase(&page_array[a]);
        free_list[order].erase(&page_array[b]);

        //cout<<"fine2"<<endl;
        int c;
        if(a>b) c=b;
        else c=a;
        //find the buddy that is on the left, use its address to store the new combined block info
        page_array[c].order+=1;
        
        a=c;
        address = a*4096;
        //cout<<"fine3"<<endl;

        order = page_array[c].order;
        free_list[order].push_front(&page_array[a]);

        //cout<<"fine4"<<endl;
         
        b = getBuddy(address,order);
        b/= 4096;
        //cout<<"fine5"<<endl;
    }
}
