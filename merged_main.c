#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

struct node {
    struct node* prev;
    struct node* next;
    int mmaped_physical_address;         //Physical add where mmaped page(s) worth of memory is starting from
    int mmaped_page_size;                //Total contiguous memory in bytes allocated from mmap for this main node
    struct node* lower_linklist_reference_ptr;         //ptr to 1st node of lower linked list
    int virtual_add_starting_point_for_this_row;
};

struct lownode {
    void* memory_allocated_ptr;    //ptr to mem location where allocated size
    struct lownode* next;
    struct lownode* prev;
    int virtual_address;
    int status;       //1 = hole, 0 = occupied
    int size;         //Node allocated size
};

struct node* header_list_space;
void* current_structure_page_ptr;
void* current;
struct node* previous_upper_list_node=NULL;

int traversal_allocate_process(int sizerequired){
    //complete this to find large enough hole. return 1 on success.
    //additional task (if enough time left) : when large enough hole is found, edit lower link list to split hole into 2 parts -> now a occupied portion, leftover hole
    //if no big enough hole then return -1
    struct lownode* lowtraversal;
    struct node* uppertraversal;
    uppertraversal=header_list_space;
    do{ 
        size_t struct_sizeupper = sizeof(struct myStructupper);
        uppertraversal -> lower_linklist_reference_ptr =  mmap(NULL,struct_sizeupper , PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        uppertraversal -> virtual_add_starting_point_for_this_row += sizerequired;
        uppertraversal -> mmaped_page_size = allocatespace(sizerequired);
        if (uppertraversal ->  mmaped_physical_address + sizerequired > uppertraversal -> mmaped_page_size) {
            insert_uppernode(int sizerequired);
        } else {
            uppertraversal -> mmaped_physical_address += sizerequired; 
            lowtraversal -> virtual_address = uppertraversal -> lower_linklist_reference_ptr:
        }
       // munmap(ptr, usersize);

        lowtraversal=uppertraversal->lower_linklist_reference_ptr;
        do{ 
           
            size_t struct_sizelower = sizeof(struct myStructlower);
            lowtraversal -> memory_allocated_ptr = mmap(NULL, sizerequired , PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            lowtraversal -> size = usersize;
            lowtraversal -> status = 0;  // it's a process 
            lowtraversal = lowtraversal -> next;
        }while(lowtraversal->next!=NULL);

        previous_upper_list_node = uppertraversal;
        uppertraversal=uppertraversal->next;
        uppertraversal -> prev = previous_upper_list_node;


    }while(uppertraversal->next!=NULL);
}

void insert_uppernode(int sizerequired){
    void* alloted_address;
    int pages;
    if (current+sizeof(struct node*)>current_structure_page_ptr+PAGE_SIZE){
        struct node* newpage=mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        current=newpage;
        current_structure_page_ptr=newpage;
    }

    ((struct node*)current)->next=NULL;
    if (current == header_list_space){
        ((struct node*)current)->prev=NULL;
        ((struct node*)current)->virtual_add_starting_point_for_this_row=0;
    }else{
        ((struct node*)current)->prev=previous_upper_list_node;
        previous_upper_list_node->next=(struct node*)current;
        ((struct node*)current)->virtual_add_starting_point_for_this_row=(previous_upper_list_node->virtual_add_starting_point_for_this_row)+1;
    }
    void* alloted_address=allocatespace(sizerequired);
    ((struct node*)current)->mmaped_physical_address=alloted_address;
    int pages=(sizerequired/PAGE_SIZE+1);
    ((struct node*)current)->mmaped_page_size=pages*PAGE_SIZE;
    if (previous_upper_list_node!=NULL){
        previous_upper_list_node=previous_upper_list_node->next;
    }else{
        previous_upper_list_node=(struct node*)current;
    }
    current+=sizeof(struct node*);

    if (current+sizeof(struct lownode*)>current_structure_page_ptr+PAGE_SIZE){
        struct node* newpage=mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        current=newpage;
        current_structure_page_ptr=newpage;
    }

    ((struct lownode*)current)->memory_allocated_ptr=alloted_address;
    ((struct lownode*)current)->prev=NULL;
    ((struct lownode*)current)->next=NULL;
    ((struct lownode*)current)->status=0;
    ((struct lownode*)current)->size=sizerequired;
    ((struct lownode*)current)->virtual_address=previous_upper_list_node->next->virtual_add_starting_point_for_this_row;
    void* prev_node_temp=current;
    current+=sizeof(struct lownode*);

    if (current+sizeof(struct lownode*)>current_structure_page_ptr+PAGE_SIZE){
        struct node* newpage=mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        current=newpage;
        current_structure_page_ptr=newpage;
    }

    if (sizerequired!=pages*PAGE_SIZE){
        ((struct lownode*)current)->memory_allocated_ptr=alloted_address+sizerequired;
        ((struct lownode*)current)->prev=((struct lownode*)(prev_node_temp));
        ((struct lownode*)current)->next=NULL;
        ((struct lownode*)current)->status=1;
        ((struct lownode*)current)->size=pages*PAGE_SIZE-sizerequired;
        ((struct lownode*)current)->virtual_address=((struct lownode*)prev_node_temp)->virtual_address+((struct lownode*)prev_node_temp)->size;
        current+sizeof(struct lownode*);
    }
}


void* allocatespace(int requested_size){
    int pagesreqd = requested_size/PAGE_SIZE+1;
    size_t reqdsize = pagesreqd*PAGE_SIZE;
    void* heap_starts_at_this_physical_address=mmap(NULL, reqdsize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    return heap_starts_at_this_physical_address;
}


int main(){
    header_list_space = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    current = header_list_space;
    current_structure_page_ptr= header_list_space;
}
