#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

struct node {
    int begin;
    int size;
    struct node* lower_linklist_reference_ptr;
    struct node* next;
    struct node* prev;
};


void* userrequest(int requested_size){
    int pagesreqd = requested_size/PAGE_SIZE+1;
    size_t reqdsize = pagesreqd*PAGE_SIZE;
    void* heap_starts_at_this_physical_address=mmap(NULL, reqdsize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    return heap_starts_at_this_physical_address;
}

void insertnewnode(struct node** head, int data, int usersize){
    struct node* current = (struct node*)userrequest(usersize);
    current->next = NULL;

    if (*head == NULL) {
        current->prev = NULL;
        *head = current;
        return;
    }
    
    struct node* temp=*head;

    while (temp->next != NULL)
        temp=temp->next;
   
    temp->next = current;
    current->prev = temp;
}

void printnodes(struct node* head){
    // List is empty
    if (head == NULL) {
        printf("\nList is empty\n");
        return;
    }
    // Else print the Data
    struct node* temp;
    temp = head;
    while (temp != NULL) {
        //printf("Data = %d\n", temp->data);
        temp = temp->next;
    }
}


int main(){
    struct node* head = NULL;
}
