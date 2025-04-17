#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  int item;
  printf ("트리를 만드려 하시는군요! 트리에 넣을 값을 알려주시면 도와드릴게요! : ");
  while (scanf("%d", item) > 0){
    rbtree_insert(p,item);
  }

  return p;
}


node_t*makenode(int item){
  node_t *node = malloc(sizeof(node_t));
  node->key = item;
  node->color = RBTREE_RED;
  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;
  return node;
}


void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *checker = t->root;
  node_t *new_node = makenode(key);
  //루트가 존재하지 않을 시 예외처리
  if (checker == NULL){        
    checker = makenode(key);
    return checker;
  }

  //제자리 찾아갈때까지 반복복
  while (1)
  {

    if( key > checker->key ){
      //우측에 아무것도 없을 때때
      if(checker->right ==NULL){
        checker->right= new_node;
        break;
      }
      //뭔가 있어어
      else
        checker = checker-> right;
    }

    else if(key < checker->key ){
      //왼쪽에 아무것도 없을 때때
      if(checker->left ==NULL){
        checker->left= new_node;
        break;
      }
      //뭔가 있어어
      else
        checker = checker-> left;
    }

    // 임시입니다. 임시입니다. 중복값 나오면 일단 레프트에 넣자 
    else{
      new_node->left = checker->left; 
      checker->left = new_node;

      break;
    }
  }
  new_node->parent = checker;
  INSERTChecking(&(new_node));
  return t->root;
}


node_t INSERTChecking(node_t **node){


}




node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}




// void insert_RBnode(RBnode *Pnode, int item){
        
//     if(item < Pnode->item){
//         if(Pnode->left == NULL){
//             RBnode *node = makenode(item);
//             Pnode->left = node;
//             return;
//         }

//         insert_RBnode(Pnode->left,item);
//     } 


//     if(item > Pnode->item){
//         if(Pnode->right == NULL){
//             RBnode *node = makenode(item);
//             Pnode->right = node;
//             return;
//         }
        
//         insert_RBnode(Pnode->right,item);
//     }

//     if(item == Pnode->item){
//         printf("중복입니다 값을 똑바로 넣어주세요 ^^");
//         return;
//     }

//     INSERTcheck(&(Pnode));
// }


// void INSERTcheck(RBnode **Pnode){
//     if((*Pnode)->left->color == 'R')


// }