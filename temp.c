#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>

#define PAGE_SIZE 4096

struct node {
    struct node* prev;
    struct node* next;
    void* mmaped_physical_address;         //Physical add where mmaped page(s) worth of memory is starting from
    size_t mmaped_page_size;                //Total contiguous memory in bytes allocated from mmap for this main node
    struct lownode* lower_linklist_reference_ptr;         //ptr to 1st node of lower linked list
    void* virtual_add_starting_point_for_this_row;
};

struct lownode {
    void* memory_allocated_ptr;    //ptr to mem location where allocated size
    struct lownode* next;
    struct lownode* prev;
    void* virtual_address;
    int status;       //1 = hole, 0 = occupied
    size_t size;         //Node allocated size
};

struct node* header_list_space;
void* current_structure_page_ptr;
void* current;
struct node* latest_upper_list_node;


void* allocatespace(size_t requested_size){
    int pagesreqd = requested_size/PAGE_SIZE+1;
    size_t reqdsize = pagesreqd*PAGE_SIZE;
    void* heap_starts_at_this_physical_address=mmap(NULL, reqdsize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (heap_starts_at_this_physical_address==MAP_FAILED){
        perror("mmap error");
        exit(2);
    }
    return heap_starts_at_this_physical_address;
}


struct lownode* create_lowernode(void* mem_alloc_ptr,void* vir_add, int stats,size_t sizereq){
    if (current+sizeof(struct lownode)>current_structure_page_ptr+PAGE_SIZE){
        struct node* newpage=mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (newpage==MAP_FAILED){
            perror("mmap error");
            exit(2);
        }
        current=newpage;
        current_structure_page_ptr=newpage;
    }

    ((struct lownode*)current)->memory_allocated_ptr= mem_alloc_ptr;
    ((struct lownode*)current)->prev=NULL;
    ((struct lownode*)current)->next=NULL;
    ((struct lownode*)current)->status=stats;
    ((struct lownode*)current)->size=sizereq;
    ((struct lownode*)current)->virtual_address=vir_add;
    struct lownode* return_value=((struct lownode*)current);
    current+=sizeof(struct lownode);
    return return_value;
}


void* insert_uppernode(size_t sizerequired){
    void* alloted_address;
    int pages;
    if (current+sizeof(struct node)>current_structure_page_ptr+PAGE_SIZE){
        struct node* newpage=mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (newpage==MAP_FAILED){
            perror("mmap error");
            exit(2);
        }
        current=newpage;
        current_structure_page_ptr=newpage;
    }

    ((struct node*)current)->next=NULL;
    if (current == header_list_space){
        ((struct node*)current)->prev=NULL;
        ((struct node*)current)->next=NULL;
        ((struct node*)current)->virtual_add_starting_point_for_this_row=(void*)0;
    }else{
        ((struct node*)current)->prev=latest_upper_list_node;
        latest_upper_list_node->next=(struct node*)current;
        ((struct node*)current)->virtual_add_starting_point_for_this_row=(void*)(((long int)(latest_upper_list_node->virtual_add_starting_point_for_this_row))+((long int)(latest_upper_list_node->mmaped_page_size)));
    }

    alloted_address=allocatespace((sizerequired/PAGE_SIZE+1)*PAGE_SIZE);
    ((struct node*)current)->mmaped_physical_address=alloted_address;
    pages=(sizerequired/PAGE_SIZE+1);
    ((struct node*)current)->mmaped_page_size=pages*PAGE_SIZE;
    ((struct node*)current)->lower_linklist_reference_ptr=current+sizeof(struct node);
    if (latest_upper_list_node!=NULL){
        latest_upper_list_node=latest_upper_list_node->next;
    }else{
        latest_upper_list_node=header_list_space;
    }
    current+=sizeof(struct node);

    if (current+sizeof(struct lownode)>current_structure_page_ptr+PAGE_SIZE){
        struct node* newpage=mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (newpage==MAP_FAILED){
            perror("mmap error");
            exit(2);
        }
        current=newpage;
        current_structure_page_ptr=newpage;
    }
    
    ((struct lownode*)current)->memory_allocated_ptr=alloted_address;
    ((struct lownode*)current)->prev=NULL;
    ((struct lownode*)current)->next=NULL;
    ((struct lownode*)current)->status=0;
    ((struct lownode*)current)->size=sizerequired;
    ((struct lownode*)current)->virtual_address=latest_upper_list_node->virtual_add_starting_point_for_this_row;
    void* prev_node_temp=current;
    current+=sizeof(struct lownode);
    
    if (current+sizeof(struct lownode)>current_structure_page_ptr+PAGE_SIZE){
        struct node* newpage=mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (newpage==MAP_FAILED){
            perror("mmap error");
            exit(2);
        }
        current=newpage;
        current_structure_page_ptr=newpage;
    }

    if (sizerequired!=pages*PAGE_SIZE){
        ((struct lownode*)current)->memory_allocated_ptr=(void*)(((long int)alloted_address)+((long int)sizerequired));
        ((struct lownode*)current)->prev=((struct lownode*)(prev_node_temp));
        ((struct lownode*)current)->next=NULL;
        ((struct lownode*)prev_node_temp)->next=((struct lownode*)current);
        ((struct lownode*)current)->status=1;
        ((struct lownode*)current)->size=pages*PAGE_SIZE-sizerequired;
        ((struct lownode*)current)->virtual_address=(void*)(((long int)(((struct lownode*)prev_node_temp)->virtual_address))+((long int)(((struct lownode*)prev_node_temp)->size)));
        current+=sizeof(struct lownode);
    }
    return ((struct lownode*)prev_node_temp)->virtual_address;
}


void* traversal_allocate_process(size_t sizerequired){
    //complete this to find large enough hole. return 1 on success.
    //if no big enough hole then return -1
    struct lownode* lowtraversal;
    struct node* uppertraversal;
    uppertraversal=header_list_space;
    if (header_list_space!=current){
        do{ 
            lowtraversal=uppertraversal->lower_linklist_reference_ptr;
            do{ 
                if (lowtraversal -> status == 1 && lowtraversal-> size > sizerequired ){
                    size_t totsize = lowtraversal -> size ;
                    void* curmemlocation = lowtraversal -> memory_allocated_ptr;
                    void* curvirtualaddress = lowtraversal -> virtual_address;
                    struct lownode* temp = create_lowernode((void*)(((long int)curmemlocation)+((long int)sizerequired)),(void*)(((long int)curvirtualaddress)+((long int)sizerequired)),1,totsize-sizerequired);

                    struct lownode* nextelement = lowtraversal->next;
                    lowtraversal -> next = temp;
                    temp->next = nextelement;
                    temp->prev = lowtraversal;
                    if (nextelement!=NULL){
                        nextelement->prev=temp;
                    }
                    lowtraversal -> status = 0;
                    lowtraversal->size = sizerequired;
                    return lowtraversal->virtual_address;
                }else if (lowtraversal -> status == 1 && lowtraversal-> size == sizerequired ){
                    lowtraversal->status = 0;
                    return lowtraversal->virtual_address;
                }
                else{
                    lowtraversal = lowtraversal -> next;
                }
            }while(lowtraversal!=NULL);
            uppertraversal=uppertraversal->next;
        }while(uppertraversal!=NULL);
    }
    return (void*)(-1);
}


void* mems_get(void* ptr){
    struct node* uppertraversal;
    uppertraversal=header_list_space;
    void* paddress=NULL;
    while(uppertraversal!=NULL){
        if((uppertraversal->next)->virtual_add_starting_point_for_this_row>ptr){
            long int difference=((long int)ptr)-((long int)(uppertraversal->virtual_add_starting_point_for_this_row));
            paddress=(void*)(((long int)(uppertraversal->mmaped_physical_address))+((long int)difference));
        }
    }
    return paddress;
}

void mems_free(void* ptr){
    struct lownode* lowtraversal;
    struct node* uppertraversal;
    uppertraversal=header_list_space;

    while(uppertraversal!=NULL){
            lowtraversal=uppertraversal->lower_linklist_reference_ptr;
        while(lowtraversal!=NULL){
            if(lowtraversal->virtual_address==ptr){
                lowtraversal->status=1;
                if(lowtraversal->prev!=NULL && lowtraversal->prev->status==1 && lowtraversal->next!=NULL && lowtraversal->next->status==1){
                    struct lownode* b=lowtraversal->prev;
                    struct lownode* c=lowtraversal->next->next;
                    b->size=b->size+lowtraversal->size+lowtraversal->next->size;
                    lowtraversal->next->next=NULL;
                    lowtraversal->next->prev=NULL;
                    lowtraversal->next=NULL;
                    lowtraversal->prev=NULL;
                    b->next=c;
                    if (c!=NULL){
                        c->prev=b;
                    }
                }else if(lowtraversal->prev!=NULL && lowtraversal->prev->status==1){
                    struct lownode* b=lowtraversal->prev;
                    struct lownode* c=lowtraversal->next;
                    b->size=b->size+lowtraversal->size;
                    lowtraversal->next=NULL;
                    lowtraversal->prev=NULL;
                    b->next=c;
                    if (c!=NULL){
                        c->prev=b;
                    }
                }else if(lowtraversal->next!=NULL && lowtraversal->next->status==1){
                    struct lownode* c=lowtraversal->next->next;
                    lowtraversal->size=lowtraversal->next->size+lowtraversal->size;
                    lowtraversal->next->next=NULL;
                    lowtraversal->next->prev=NULL;
                    lowtraversal->next=c;
                    if (c!=NULL){
                        c->prev=lowtraversal;
                    }
                }
            }
            lowtraversal = lowtraversal -> next;
        }
        uppertraversal=uppertraversal->next;
    }
}

void pages_used(){
    struct node* uppertraversal;
    uppertraversal=header_list_space;
    long int count=0;
    while(uppertraversal!=NULL){
        count+=(uppertraversal->mmaped_page_size)/PAGE_SIZE;
        uppertraversal=uppertraversal->next;
    }
    printf("Pages used:   %ld\n",count);
}

void upper_list_nodes(){
    struct node* uppertraversal;
    uppertraversal=header_list_space;
    long int count=0;
    while(uppertraversal!=NULL){
        count+=1;
        uppertraversal=uppertraversal->next;
    }
    printf("Main chain length:   %ld\n",count);
}

void space_unused(){
    struct node* uppertraversal;
    uppertraversal=header_list_space;
    struct lownode* lowertraversal;
    long int count=0;
    while(uppertraversal!=NULL){
        lowertraversal=uppertraversal->lower_linklist_reference_ptr;
        while(lowertraversal!=NULL){
            if(lowertraversal->status==1){
                count+=lowertraversal->size;
            }
            lowertraversal=lowertraversal->next;
        }
        uppertraversal=uppertraversal->next;
    }
    printf("Space unused:   %ld\n",count);
}

void sub_chain_length(){
    struct node* uppertraversal;
    uppertraversal=header_list_space;
    struct lownode* lowertraversal;
    long int count;
    printf("Sub chain length array:   [");
    while(uppertraversal!=NULL){
        count=0;
        lowertraversal=uppertraversal->lower_linklist_reference_ptr;
        while(lowertraversal!=NULL){
            count+=1;
            lowertraversal=lowertraversal->next;
        }
        printf("%ld, ",count);
        uppertraversal=uppertraversal->next;
    }
    printf("]\n");
}


void mems_print_stats(){
    struct lownode* lowtraversal;
    struct node* uppertraversal;
    uppertraversal=header_list_space;
    do{ 
        if (uppertraversal!=NULL){
            lowtraversal=uppertraversal->lower_linklist_reference_ptr;
            printf("MAIN[%ld:%ld]-> ",(long int)(uppertraversal->virtual_add_starting_point_for_this_row),((long int)(uppertraversal->virtual_add_starting_point_for_this_row))+((long int)(uppertraversal->mmaped_page_size))-1);
            do{
                if (lowtraversal->status==0){
                    printf("P[%ld:%ld] <-> ",(long int)(lowtraversal->virtual_address),((long int)(lowtraversal->virtual_address))+((long int)(lowtraversal->size))-1);
                }else{
                    printf("H[%ld:%ld] <-> ",(long int)(lowtraversal->virtual_address),((long int)(lowtraversal->virtual_address))+((long int)(lowtraversal->size))-1);
                }
                lowtraversal = lowtraversal -> next;
            }while(lowtraversal!=NULL);
            printf("NULL\n");
            uppertraversal=uppertraversal->next;
        }
    }while(uppertraversal!=NULL);
    pages_used();
    space_unused();
    upper_list_nodes();
    sub_chain_length();
}


void mems_finish(){
    struct node* uppertraversal;
    uppertraversal=header_list_space;
    int check;
    while(uppertraversal!=NULL){
        check=munmap(uppertraversal->mmaped_physical_address,uppertraversal->mmaped_page_size);
        if (check==-1){
            perror("munmap");
            exit(3);
        }
        uppertraversal=uppertraversal->next;
    }

    check=munmap(header_list_space,PAGE_SIZE);
    if (check==-1){
        perror("munmap");
        exit(3);
    }

}

void mems_init(){
    header_list_space = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (header_list_space==MAP_FAILED){
        perror("mmap error");
        exit(2);
    }
    current = header_list_space;
    current_structure_page_ptr= header_list_space;
    latest_upper_list_node=NULL;
}


void* mems_malloc(size_t size){
    void* result = traversal_allocate_process(size);
    if (result==((void*)(-1))){
        result = insert_uppernode(size);
    }
    return result;
}


int main(){
    mems_init();
    void* a=mems_malloc(50);
    void* b=mems_malloc(3000);
    void* c=mems_malloc(1000);
    mems_free(c);
    void* d=mems_malloc(1040);
    void* e=mems_malloc(6);
    void* f=mems_malloc(200);
    mems_print_stats();
    mems_finish();
}
