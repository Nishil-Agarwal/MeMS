struct node {
  struct node *start = NULL;
  struct node *next;
  struct node * prev;
};
int main() {
  head = mmap(sizeof(page*numpages(usersize)));
  head->start = &head;

  
 // if(head-> start +usersize > head-> end): insertnewNode(usersize)
  
}

int numpages(size){
    return floor(size/page) + 1;
} 


void insertnewNode(struct Node** head, int usersize) {
   
    struct Node* headnext = mmap(sizeof(page*numpages(usersize)));
    headnext->next = NULL;
    struct Node* current = *headnext;
    if (*head == NULL) {
        headnext->prev = NULL;
        *head = headnext;
        return;
    }
    while (current->next != NULL)
        current = current->next;
   
    current->next = NULL;
    cuurent->prev = head;
}
