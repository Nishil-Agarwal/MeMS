For the MeMS we have used a 2D  doubly linked list.
    In the upper list we are storing the main nodes which contain pointers to lower level lists.
The upper list also contains the mmaped physical address.
    The lower linked list nodes contain pointers to specific physical memory locations and their virtual 
addresses, size to simulate MeMS.

1)To create the 2D lists, we have allocated a Page worth memory initially, then we maintain a pointer
    to current location within this page. We use it to create nodes of either upper/lower level list.
    
2)We change value of current pointer by incrementing it by sizeof upper/lower nodes respectively.

3)Moreover, all the nodes also store previous and next node pointers. This aids traversal.
    To achieve this we have used global node variables to store previous node pointer temporarily for us.

Throughout the code we have ensured that as soon as some Page runs out of memory, a new one is allocated.
This is true even for the 2D list structure page.



General Working of code:
1)Memory demand is made.

2)We check through traversing all lower level nodes if one large enough to satisfy requirement exists.

3)If yes then we simulate allocation of that memory for required size and return the virtual address.

4)If not then we create a new node. While doing this we ensure that new node gets enough Pages worth of memory 
    to be able to store whole data completely. Eg: If Page size =100 kb and requirement is of 200, we will 
    allocate 2 pages worth memory and return pointer.
    
5)Whether some memory begining from some pointer is hole pr process is decided by a status variable.

6)In case someone uses free functionality, we just change this status variable to that of hole. At the same time
    we check for adjacent holes in vicinity and merge accordingly by deleting required nodes from lower level list.
    
7)print_mems is easily executed through traversing the lists.

8)get_mems works by first traversing to find the upper list node in between which provided virtual address is located.
    Accordingly we fetch physical address from the upper node and do necessary calculations to return real physical address.
    
9)mems_finish traverses through all upper list nodes unmaping all the memory pages allocated at stored physical address.
    Finally it also removes the pages where the data for 2D lists was stored.

Throughout the code we have also ensured error handling of mmap and munmap in case they fail and have printed required error message.
