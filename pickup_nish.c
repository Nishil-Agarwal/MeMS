#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

void* userrequest(int requested_size){
    int pagesreqd = requested_size/PAGE_SIZE+1;
    size_t reqdsize = pagesreqd*PAGE_SIZE;
    void* heap_starts_at_this_physical_address=mmap(NULL, reqdsize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    return heap_starts_at_this_physical_address;
}


int main(){
    int* memory=(int*)userrequest(500);
    memory[0]=3;
    printf("%d\n",memory[0]);
    size_t reqdsize = 1*PAGE_SIZE;
    munmap(memory, reqdsize);
}
