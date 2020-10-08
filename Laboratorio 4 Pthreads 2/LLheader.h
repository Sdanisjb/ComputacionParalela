#include <iostream>
#include <stdlib.h>

using namespace std;

int total_ops = 100000;
int n_threads;

pthread_mutex_t mutex;
pthread_rwlock_t rw_lock;


struct Node{
    int dato;
    Node* next;
    pthread_mutex_t mutex;
    Node(int _dato, Node*_next): dato(_dato), next(_next){};
};


Node* head = NULL;
pthread_mutex_t head_mutex;
float find_percent = 0.8;
float insert_percent = 0.1;
float erase_percent = 0.1;


int find(int value){
    Node* curr_p = head;
    while(curr_p != NULL && curr_p->dato < value) curr_p = curr_p->next;

    return (curr_p == NULL || curr_p->dato > value)? 0:1;
}

int insert(int value){
    Node* curr_p = head;
    Node* pred_p = NULL;
    Node* temp;

    while(curr_p != NULL && curr_p->dato < value){
        pred_p = curr_p;
        curr_p = curr_p->next;
    }
    if(curr_p == NULL || curr_p->dato > value){
        temp = new Node(value, curr_p);
        if(pred_p == NULL){
            head = temp;
        }
        else{
            pred_p->next = temp;
        }
        return 1;
    }else{
        return 0;
    }
}

int erase(int value){
    Node* curr_p = head;
    Node* pred_p = NULL;

    while(curr_p != NULL && curr_p->dato < value){
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if(curr_p != NULL && curr_p->dato == value){
        if(pred_p == NULL){
            head = curr_p->next;
            delete curr_p;
        }
        else{
            pred_p->next = curr_p->next;
            delete curr_p;
        }
        return 1;
    }
    else{
        return 0;
    }
}

/*Funciones con un Mutex para las operaciones*/

int find_mutex(int value){
    pthread_mutex_lock(&mutex);
    int res = find(value);
    pthread_mutex_unlock(&mutex);
    return res;
}

int insert_mutex(int value){
    pthread_mutex_lock(&mutex);
    int res = insert(value);
    pthread_mutex_unlock(&mutex);
    return res;
}

int erase_mutex(int value){
    pthread_mutex_lock(&mutex);
    int res = erase(value);
    pthread_mutex_unlock(&mutex);
    return res;
}


/*Lista enlazada utilizando mutex en cada nodo*/

int find_mutex_node(int value){
    Node* temp;
    
    pthread_mutex_lock(&head_mutex);
    temp = head;
    while(temp != NULL && temp->dato < value){
        if(temp->next != NULL){
            pthread_mutex_lock(&(temp->next->mutex));
        }
        if(temp == head){
            pthread_mutex_unlock(&head_mutex);
        }
        pthread_mutex_unlock(&(temp->mutex));
        temp = temp->next;
    }

    if(temp == NULL || temp->dato > value){
        if(temp == head){
            pthread_mutex_unlock(&head_mutex);
        }
        if(temp != NULL){
            pthread_mutex_unlock(&(temp->mutex));
        }
        return 0;
    }
    else{
        if(temp == head){
            pthread_mutex_unlock(&head_mutex);
        }
        pthread_mutex_unlock(&(temp->mutex));
        return 1;
    }

}

void init_ptrs(Node** curr_p, Node** pred_p){
    *pred_p = NULL;
    pthread_mutex_lock(&head_mutex);
    *curr_p = head;
    if(head != NULL){
        pthread_mutex_lock(&(head->mutex));
    }
}

int advance_ptrs(Node** curr_p, Node** pred_p){
    int rv = 1;
    Node* curr = *curr_p;
    Node* pred = *pred_p;

    if(curr == NULL){
        if(pred == NULL){
            return -1;
        }
        else{
            return 0;
        }
    }
    else{
        if(curr->next != NULL){
            pthread_mutex_lock(&(curr->next->mutex));
        }
        else{
            rv = 0;
        }
        if(pred != NULL){
            pthread_mutex_unlock(&(pred->mutex));
        }
        else{
            pthread_mutex_unlock(&head_mutex);
        }
        *pred_p = curr;
        *curr_p = curr->next;
        return rv;
    }

}

int insert_mutex_node(int value) {
   Node* curr;
   Node* pred;
   Node* temp;
   int rv = 1;

    init_ptrs(&curr, &pred);  
   
   while (curr != NULL && curr->dato < value) {
      advance_ptrs(&curr, &pred);
   }

   if (curr == NULL || curr->dato > value) {     
      pthread_mutex_init(&(temp->mutex), NULL);
      temp = new Node(value, curr);
      if (curr != NULL) 
         pthread_mutex_unlock(&(curr->mutex));
      if (pred == NULL) {
         // Inserting in head of list
         head = temp;
         pthread_mutex_unlock(&head_mutex);
      } else {
         pred->next = temp;
         pthread_mutex_unlock(&(pred->mutex));
      }
   } else { /* value in list */
      if (curr != NULL) 
         pthread_mutex_unlock(&(curr->mutex));
      if (pred != NULL)
         pthread_mutex_unlock(&(pred->mutex));
      else
         pthread_mutex_unlock(&head_mutex);
      rv = 0;
   }

   return rv;
}  

int erase_mutex_node(int value) {
   Node* curr;
   Node* pred;
   int rv = 1;

   init_ptrs(&curr, &pred);

   /* Find value */
   while (curr != NULL && curr->dato < value) {
      advance_ptrs(&curr, &pred);
   }
   
   if (curr != NULL && curr->dato == value) {
      if (pred == NULL) { /* first element in list */
         head = curr->next;
         pthread_mutex_unlock(&head_mutex);
         pthread_mutex_unlock(&(curr->mutex));
         pthread_mutex_destroy(&(curr->mutex));
         delete curr;
      } else { 
         pred->next = curr->next;
         pthread_mutex_unlock(&(pred->mutex));
         pthread_mutex_unlock(&(curr->mutex));
         pthread_mutex_destroy(&(curr->mutex));
         delete curr;
      }
   } else { /* Not in list */
      if (pred != NULL)
         pthread_mutex_unlock(&(pred->mutex));
      if (curr != NULL)
         pthread_mutex_unlock(&(curr->mutex));
      if (curr == head)
         pthread_mutex_unlock(&head_mutex);
      rv = 0;
   }

   return rv;
} 


/*Funciones con un rw_lock para las operaciones*/

int find_rw(int value){
    pthread_rwlock_rdlock(&rw_lock);
    int res = find(value);
    pthread_rwlock_unlock(&rw_lock);
    return res;
}

int insert_rw(int value){
    pthread_rwlock_wrlock(&rw_lock);
    int res = insert(value);
    pthread_rwlock_unlock(&rw_lock);
    return res;
}

int erase_rw(int value){
    pthread_rwlock_wrlock(&rw_lock);
    int res = erase(value);
    pthread_rwlock_unlock(&rw_lock);
    return res;
}

void* thread_one_mutex(void* rank){
    unsigned int my_rank = (long)rank;
    int i, val;
    double op;
    unsigned seed= my_rank +1;
    
    int my_ops = total_ops/n_threads;

    for(i=0; i< my_ops; i++){
        op = rand_r(&my_rank)/RAND_MAX;
        val = rand_r(&my_rank);
        if(op < find_percent){
            find_mutex(val);
        }
        else if(op < find_percent + insert_percent){
            insert_mutex(val);
        }
        else{
            erase_mutex(val);
        }
    }

    return NULL;
}

void* thread_mutex_node(void* rank){
    unsigned int my_rank = (long)rank;
    int i, val;
    double op;
    unsigned seed= my_rank +1;
    
    int my_ops = total_ops/n_threads;

    for(i=0; i< my_ops; i++){
        op = rand_r(&my_rank)/RAND_MAX;
        val = rand_r(&my_rank);
        if(op < find_percent){
           find_mutex_node(val);
        }
        else if(op < find_percent + insert_percent){
            insert_mutex_node(val);
        }
        else{
            erase_mutex_node(val);
        }
    }

    return NULL;
}



void* thread_rw(void* rank){
    unsigned int my_rank = (long)rank;
    int i, val;
    double op;
    unsigned seed= my_rank +1;
    
    int my_ops = total_ops/n_threads;

    for(i=0; i< my_ops; i++){
        op = rand_r(&my_rank)/RAND_MAX;
        val = rand_r(&my_rank);
        if(op < find_percent){
            find_rw(val);
        }
        else if(op < find_percent + insert_percent){
            insert_rw(val);
        }
        else{
            erase_rw(val);
        }
    }

    return NULL;
}

