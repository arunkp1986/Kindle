#include<ulib.h>


struct Node {
    int data;
    struct Node* next;
};
 
/* Given a reference (pointer to pointer) to the head of a
   list and an int, inserts a new node on the front of the
   list. */
void push(struct Node** head_ref, int new_data)
{
    struct Node* new_node
        = (struct Node*) mmap(NULL, sizeof(struct Node), PROT_READ | PROT_WRITE, 0);
    new_node->data = new_data;
    new_node->next = (*head_ref);
    (*head_ref) = new_node;
}
 
/* Given a reference (pointer to pointer) to the head of a
   list and a key, deletes the first occurrence of key in
   linked list */
void deleteNode(struct Node** head_ref, int key)
{
    // Store head node
    struct Node *temp = *head_ref, *prev;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->data == key) {
        *head_ref = temp->next; // Changed head
        munmap(temp, sizeof(struct Node));
        // free(temp); // free old head
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->data != key) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL)
        return;
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    munmap(temp, sizeof(struct Node));
    // free(temp); // Free memory
}
 
// This function prints contents of linked list starting
// from the given node
void printList(struct Node* node)
{
    while (node != NULL) {
        printf(" %d ", node->data);
        node = node->next;
    }
}
 
// Driver code
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

    checkpoint_init(CP_INTERVAL);
    printf("Linked list\n");
    printf("CP_INTERVAL = %d\n", CP_INTERVAL);
    /* Start with the empty list */
    struct Node* head = NULL;

    int size = 100;
    int times = 150;

    unsigned seed = 0xACE1u;
    unsigned bit = 0;
    push(&head, 8);

    while (times--) {
        int length = rand(&seed, &bit) % size;

        for (int i=0; i<length; i++) {
            push(&head, 7);
        }

        
        // printList(head);

        for (int i=0; i<length; i++) {
            deleteNode(&head, 7);
        }

        // printf("Finished Linked List %d\n", times);

        // printf("\nLinked List after Deletion of 1: ");
        // printList(head);

    }

    return 0;
}