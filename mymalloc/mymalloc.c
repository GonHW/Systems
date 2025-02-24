/**
 * @file mymalloc.c
 * @author: Hench Wu(hhw14)
 * @brief 
 * @version 0.1
 * @date 2022-12-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include "mymalloc.h"

/*******************
* Global Variables *
********************/
int fit; // Fit algorithm: 0: first, 1: next, 2: best
struct header *head = NULL; // head node of the free nodes in LL
struct header *recent_malloc = NULL; // most recent malloc 
void* heap; //initialize the heap variable




// Node in LL for free memory
typedef struct header {
  size_t size;  // free bytes of the block
  int free;     // 0: free | 1: not free
  struct header *next; // next free block node 
  struct header *prev; // previous free block node
}header;

// Testing method
void printFreeList(){
    header* ptr = head;
    while(ptr != NULL){
        printf("%lu\n", ptr -> size);
        ptr = ptr -> next;
    }
    puts("Done");
}

// Set the values of the freeblock node
void set_freeblock_values(header *node, size_t size, int free, header *next, header *prev){
  node->size = size;
  node->free = free;
  node->next = next;
  node->prev = prev;
}


// Finished
void myinit(int allocAlg) {

    heap = (void*) malloc(1024 * 1024); // Allocate 1 MB of memory for the heap
    head = heap; // Initialize the head of the LL to beginning of the heap
    //printf("Address of head: %p\n", head);
    //printf("Address of heap: %p\n", heap);
    int freebytes = 1000000 - sizeof(header); // total free bytes in heap
    //printf("%d\n", freebytes);
    set_freeblock_values(head, freebytes, 0, NULL, NULL);  // Set values of the head of LL 
    
    // Set the fit algorithm 
    if(heap != NULL){
        // 0: first fit 
        if(allocAlg == 0){
          fit = 0;
          //printf("%d\n", fit);
        }
        // 1: next fit
        else if(allocAlg == 1){
          fit = 1;
          recent_malloc=head;
          //printf("%d\n", fit);
        }
        // 2: best fit
        else if(allocAlg == 2){
          fit = 2;
          //printf("%d\n", fit);
        }
        else{
          printf("Not a valid entry\n");
          exit(1);
        }
    }
    //malloc() fails
    else
      printf("system malloc failed\n");
    //free(heap); // need to get rid of because mycleanup() 
}
// First Fit: Finished
// Next Fit: Not started
// Best Fit: Not started
void* mymalloc(size_t size) {
  // Check for a size of 0
  if (size == 0) {
    //printf("NULL\n");
    return NULL;
  }
  if(size>(size_t)1000000){
    printf("Error: Heap doesn't have enough space\n");
    exit(1);
  }

  // Determine the size of the block to be allocated
  //size_t block_size = sizeof(header) + size + (8 - size % 8) % 8;
  size_t block_size = size + (8 - size % 8) % 8;
  //printf("%ld\n", block_size);
  //create a pointer to iterate through the freeblocks from the head of LL
  struct header* current = head;
  // Search for an available block of the desired size with the specified fit(searching) algorithm
  // 0: first fit
  if(fit == 0){
    while(current != NULL && current->size < block_size){
      current = current->next;
    }//printf("%ld\n", current->size);

    // If no blocks fit the data return NULL
    if(current == NULL){
      //printf("Nah\n");
      return NULL;
    }

    // Split the payload(which is 'current->size') when:  
    // sizeof(header) + block_size < current->size meaning there is more freespace than the size of the payload & a new header
    if(sizeof(header) + block_size < current->size){
      // printf("Split\n");
      // shift the ptr to the beginning of the payload
      char* ptr = (char*) current + sizeof(header);
      // save the ptr to the beginning of the payload to return later
      void* data_region = ptr;
      // shift the ptr to the end of the data
      ptr = ptr + block_size;
      // make a new header at ptr (after the payload that was malloced)
      header* split_free_header = (header*) ptr;
      int split_size = current->size - block_size - sizeof(header); // size of free space after split_free_header
      set_freeblock_values(split_free_header, split_size, 0, NULL, NULL); // initialize the split_free_header
      //update the malloc header to correct size and free variable
      current->size = block_size;
      current->free = 1;
      //if current is the head and the last node change the new node to the head / remove current
      if(current->prev == NULL && current->next == NULL){
        split_free_header->next = current->next;
        head = split_free_header; // set head to the next node 
      }
      // if current is the head and it has a following node, change new node to head /remove current
      else if(current->prev == NULL && current->next != NULL){
        split_free_header->next = current->next; // split header next is currents next
        current->next->prev = split_free_header; // node after current points back to split header
        head = split_free_header; // set head to the next node
      }
      //if current is a middle node change the new node to the middle node / remove current
      else if(current->prev != NULL && current->next != NULL){
        current->prev->next = split_free_header; // change next node of the one before current to the new split header
        split_free_header->prev = current->prev; // change the prev node of the split header to the node before current
        current->next->prev = split_free_header; // change the prev node of the node after the current to the split header
        split_free_header->next = current->next; // changing the next node of the split header to the node after the current node
      }
      //if the current is an end node(not the head) change the new node to the end node /remove current
      else if(current->prev != NULL && current->next == NULL){
        current->prev->next = split_free_header;
        split_free_header->prev = current->prev;
      }
      return data_region;
    }
    //Don't Split if  block_size <= current->size
    else{
      // shift the ptr to the beginning of the payload
      char* ptr = (char*) current + sizeof(header);
      // save the ptr to the beginning of the payload to return later
      void* data_region = ptr;
      //update the malloc header to correct size and free variable
      current->size = block_size;
      current->free = 1;
      
      //remove the node from the LL

      // edge case: malloc size fits perfectly into entire heap 
      if(current->prev == NULL && current->next ==NULL){
        //where should the head point to?
        head = NULL;
      }
      // current is head and the next is another node
      if(current->prev == NULL && current->next != NULL){
        head = current->next;
        current->next-> prev = NULL;
      }
      // 
      if(current->prev != NULL){
        current->prev->next = current->next;
      }
      if(current->next != NULL){
        current->next->prev = current->prev;
      }
      //return the pointer
      return data_region;
    }

  }
  // 1: next fit
  else if(fit == 1){
    //Don't know if we need this
    current=recent_malloc;
    //}
    // search until we reach the end of the heap or find a free block
    if(recent_malloc!=NULL){
      recent_malloc=recent_malloc->next;
    }
    else{
      recent_malloc=head;
    }
    
    while(current!=recent_malloc){
      if(recent_malloc != NULL && recent_malloc->size < block_size)
      {  
        recent_malloc = recent_malloc->next;
      }
      else if(recent_malloc == NULL)
      {
        recent_malloc=head;
      }
      else{
        break;
      }
    }

    // If no blocks fit the data return NULL
    if(recent_malloc == NULL){
      //printf("Nah\n");
      return NULL;
    }

    // Split the payload(which is 'current->size') when:  
    // sizeof(header) + block_size < current->size meaning there is more freespace than the size of the payload & a new header
    if(sizeof(header) + block_size < recent_malloc->size){
      // printf("Split\n");
      // shift the ptr to the beginning of the payload
      char* ptr = (char*) recent_malloc + sizeof(header);
      // save the ptr to the beginning of the payload to return later
      void* data_region = ptr;
      // shift the ptr to the end of the data
      ptr = ptr + block_size;
      // make a new header at ptr (after the payload that was malloced)
      header* split_free_header = (header*) ptr;
      int split_size = recent_malloc->size - block_size - sizeof(header); // size of free space after split_free_header
      set_freeblock_values(split_free_header, split_size, 0, NULL, NULL); // initialize the split_free_header
      //update the malloc header to correct size and free variable
      recent_malloc->size = block_size;
      recent_malloc->free = 1;
      //if recent_malloc is the head and the last node change the new node to the head / remove recent_malloc
      if(recent_malloc->prev == NULL && recent_malloc->next == NULL){
        split_free_header->next = recent_malloc->next;
        head = split_free_header; // set head to the next node 
      }
      // if recent_malloc is the head and it has a following node, change new node to head /remove recent_malloc
      else if(recent_malloc->prev == NULL && recent_malloc->next != NULL){
        split_free_header->next = recent_malloc->next; // split header next is currents next
        recent_malloc->next->prev = split_free_header; // node after current points back to split header
        head = split_free_header; // set head to the next node
      }
      //if current is a middle node change the new node to the middle node / remove current
      else if(recent_malloc->prev != NULL && recent_malloc->next != NULL){
        recent_malloc->prev->next = split_free_header; // change next node of the one before current to the new split header
        split_free_header->prev = recent_malloc->prev; // change the prev node of the split header to the node before current
        recent_malloc->next->prev = split_free_header; // change the prev node of the node after the current to the split header
        split_free_header->next = recent_malloc->next; // changing the next node of the split header to the node after the current node
      }
      //if the current is an end node(not the head) change the new node to the end node /remove current
      else if(recent_malloc->prev != NULL && recent_malloc->next == NULL){
        recent_malloc->prev->next = split_free_header;
        split_free_header->prev = recent_malloc->prev;
      }
      return data_region;
    }
    //Don't Split if  block_size <= current->size
    else{
      // shift the ptr to the beginning of the payload
      char* ptr = (char*) recent_malloc + sizeof(header);
      // save the ptr to the beginning of the payload to return later
      void* data_region = ptr;
      //update the malloc header to correct size and free variable
      recent_malloc->size = block_size;
      recent_malloc->free = 1;
      
      //remove the node from the LL

      // edge case: malloc size fits perfectly into entire heap 
      if(recent_malloc->prev == NULL && recent_malloc->next ==NULL){
        //where should the head point to?
        head = NULL;
      }
      // current is head and the next is another node
      if(recent_malloc->prev == NULL && recent_malloc->next != NULL){
        head = recent_malloc->next;
        recent_malloc->next-> prev = NULL;
      }
      // 
      if(recent_malloc->prev != NULL){
        recent_malloc->prev->next = recent_malloc->next;
      }
      if(recent_malloc->next != NULL){
        recent_malloc->next->prev = recent_malloc->prev;
      }
      //return the pointer
      return data_region;
    }
  }
  // 2: best fit
  else if(fit == 2){
    // find the smallest available block of memory that is also large enough
    // to satisfy the request
    header* best_fit = NULL;

    while(current != NULL){
      if(current->size == block_size){
        best_fit = current;
        break;
      }
      if(current->size > block_size && ( best_fit == NULL || current->size < best_fit->size)){
        best_fit = current;
      }
      current = current->next;
    }

    // If no blocks fit the data return NULL
    if(best_fit == NULL){
      //printf("Nah\n");
      return NULL;
    }

    // Split the payload(which is 'best_fit->size') when:  
    // sizeof(header) + block_size < best_fit->size meaning there is more freespace than the size of the payload & a new header
    if(sizeof(header) + block_size < best_fit->size){
      // printf("Split\n");
      // shift the ptr to the beginning of the payload
      char* ptr = (char*) best_fit + sizeof(header);
      // save the ptr to the beginning of the payload to return later
      void* data_region = ptr;
      // shift the ptr to the end of the data
      ptr = ptr + block_size;
      // make a new header at ptr (after the payload that was malloced)
      header* split_free_header = (header*) ptr;
      int split_size = best_fit->size - block_size - sizeof(header); // size of free space after split_free_header
      set_freeblock_values(split_free_header, split_size, 0, NULL, NULL); // initialize the split_free_header
      //update the malloc header to correct size and free variable
      best_fit->size = block_size;
      best_fit->free = 1;
      //if best_fit is the head and the last node change the new node to the head / remove best_fit
      if(best_fit->prev == NULL && best_fit->next == NULL){
        split_free_header->next = best_fit->next;
        head = split_free_header; // set head to the next node 
      }
      // if best_fit is the head and it has a following node, change new node to head /remove best_fit
      else if(best_fit->prev == NULL && best_fit->next != NULL){
        split_free_header->next = best_fit->next; // split header next is best_fits next
        best_fit->next->prev = split_free_header; // node after best_fit points back to split header
        head = split_free_header; // set head to the next node
      }
      //if best_fit is a middle node change the new node to the middle node / remove best_fit
      else if(best_fit->prev != NULL && best_fit->next != NULL){
        best_fit->prev->next = split_free_header; // change next node of the one before best_fit to the new split header
        split_free_header->prev = best_fit->prev; // change the prev node of the split header to the node before best_fit
        best_fit->next->prev = split_free_header; // change the prev node of the node after the best_fit to the split header
        split_free_header->next = best_fit->next; // changing the next node of the split header to the node after the best_fit node
      }
      //if the best_fit is an end node(not the head) change the new node to the end node /remove best_fit
      else if(best_fit->prev != NULL && best_fit->next == NULL){
        best_fit->prev->next = split_free_header;
        split_free_header->prev = best_fit->prev;
      }
      return data_region;
    }
    //Don't Split if  block_size <= best_fit->size
    else{
      // shift the ptr to the beginning of the payload
      char* ptr = (char*) best_fit + sizeof(header);
      // save the ptr to the beginning of the payload to return later
      void* data_region = ptr;
      //update the malloc header to correct size and free variable
      best_fit->size = block_size;
      best_fit->free = 1;
      
      //remove the node from the LL

      // edge case: malloc size fits perfectly into entire heap 
      if(best_fit->prev == NULL && best_fit->next ==NULL){
        //where should the head point to?
        head = NULL;
      }
      // best_fit is head and the next is another node
      if(best_fit->prev == NULL && best_fit->next != NULL){
        head = best_fit->next;
        best_fit->next-> prev = NULL;
      }
      // 
      if(best_fit->prev != NULL){
        best_fit->prev->next = best_fit->next;
      }
      if(best_fit->next != NULL){
        best_fit->next->prev = best_fit->prev;
      }
      //return the pointer
      return data_region;
    }
    
  }

  return NULL;
}

//free/add to ordered LL: Finished
//coaleasce: Finished
// Testing: Need to do more cases
void myfree(void* ptr){
  // ptr is NULL
  if(ptr == NULL) return;
  //check the free list to maintain the heap order
  char* ptr_free = (char*)ptr; // use to shift through the heap later
  ptr_free = ptr_free - sizeof(header); // shift to access header data
  header* ptr_free_head = (header*) ptr_free; // convert the char* to header* to access the data
  if(ptr_free_head->free == 0){
    // pointer has been freed
    printf("Error: block has already been freed");
    exit(1);
  }
  else{
    // pointer hasn't been freed
    ptr_free_head->free = 0; // set the free to 0: freed in the ptr node given
  }
  
  //printf("%ld\n", ptr_free_head->size);
  //printf("Address of ptr_free: %p\n", ptr_free);
  //printf("Address of ptr_free_head: %p\n", ptr_free_head);
  header* curr = head; // use curr to iterate through the linked list to maintain the order
  //header* prev = NULL; // use to keep track of the previous node in the LL
  //printf("Address of curr: %p\n", curr);
  char* adjacent_address; // use to shift through the heap later when coalescing
  //printf("Address of curr_address: %p\n", curr_address);

  // Set head to ptr_free_head if the ptr_free_head is to the left of the head
  if(ptr_free_head < curr){
    // ptr is to the left of the head which is curr
    head = ptr_free_head;
    ptr_free_head->prev = NULL;
    ptr_free_head->next = curr;
    curr->prev = ptr_free_head;
  }
    else{
      // find where to place the ptr_free_head after the curr(represents the head) 
    while(curr != NULL && ptr_free_head > curr){
        curr = curr->next; // finds the freeblock to the right of the ptr_free_head
        //printf("Address of prev: %p\n", prev);
        //printf("Address of curr: %p\n", curr);
      }
    
    // insert the node into the LL
    curr->prev->next = ptr_free_head;
    ptr_free_head->prev = curr->prev;
    ptr_free_head->next = curr;
    curr->prev = ptr_free_head;
  }
  header* previous = ptr_free_head->prev; // hold the freeblock before ptr_free_head
  header* nextN = ptr_free_head->next; // hold the freeblock after ptr_free_head
  // bool to check whether I coalesced left, 0: meaning I use previous to coaleasce right, 1: meaning I use ptr_free_head to coalesce right
  int use_previous = 1; 
  // COALESCE LEFT
  if(previous != NULL){
    adjacent_address = (char*) previous; // Beginning of the previous freeblock
    //move to the end of the payload and compare addresses to see if we coalesce
    adjacent_address = adjacent_address + sizeof(header) + previous->size;
    if(adjacent_address == ptr_free){
      //I coalesce left so use previous to coalesce right
      use_previous = 0;
      //printf() check if they are the same
      // address are equal so merge
      previous->size += sizeof(header) + nextN->size;
      previous->next = nextN;
      if(nextN != NULL){
        // there is a freeblock after ptr_free_head
        nextN->prev = previous;
      }
    }
  }
   
  // COALESCE RIGHT
  if(nextN != NULL){
    adjacent_address = (char*) nextN; // Beginning of the next freeblock
    char*coal_right; 
    int payload_size; // size of payload to shift by
    // move previous if I coalesce left and move ptr_free_head if i didn't coalesce left
    if(use_previous == 0){
      // set the coal_right address to move to compare with adjacent_address
      coal_right = (char*) previous;
      payload_size = previous->size; // size of payload to shift by
      //move the coal_right address to the end of the payload to compare
      coal_right = coal_right + sizeof(header) + payload_size;
      if(coal_right == adjacent_address){
        // if the addresses are the same merge/coaleasce the two freeblocks together
        // address are equal so merge
        previous->size += sizeof(header) + nextN->size;
        previous->next = nextN->next;
        if(nextN->next != NULL){
          // there is a freeblock after previous
          nextN->next->prev = previous;
        }
      }
    }
    else{
      // set the coal_right address to move to compare with adjacent_address
      coal_right = (char*) ptr_free_head;
      payload_size = ptr_free_head->size; // size of payload to shift by
      //move the coal_right address to the end of the payload to compare
      coal_right = coal_right + sizeof(header) + payload_size;
      if(coal_right == adjacent_address){
        // if the addresses are the same merge/coaleasce the two freeblocks together
        // address are equal so merge
        ptr_free_head->size += sizeof(header) + nextN->size;
        ptr_free_head->next = nextN->next;
        if(nextN->next != NULL){
          // there is a freeblock after ptr_free_head
          nextN->next->prev = ptr_free_head;
        }
      }
    }
  }
  



}

// Finished: Need to Test
void* myrealloc(void* ptr, size_t size){
  //base cases
  if(ptr == NULL && size > 0){
    return mymalloc(size);
  }
  if(size == 0 && ptr != NULL){
    myfree(ptr);
    return NULL;
  }
  if(ptr == NULL && size == 0){
    return NULL;
  }
  char* resize = (char*) ptr - sizeof(header); // shift to the header of the payload
  header* ptr_resize = (header*) resize; // create the header to access the data
  size_t ptr_size = ptr_resize->size; // store the size
  void* new_resize_ptr = mymalloc(size);
  memcpy(new_resize_ptr, ptr, ptr_size);
  myfree(ptr);
  return new_resize_ptr;
}

// Possibly Finished Need To Check
void mycleanup(){
  free(heap);
}


