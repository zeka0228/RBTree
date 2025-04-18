#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

//트리 여러개 생성 케이스도 만들어야함!!!!! 
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = malloc(sizeof(node_t)); //센티널만들기
  p->nil->color = RBTREE_BLACK;  //블랙 고정정
  int item;
  printf ("트리를 만드려 하시는군요! 트리에 넣을 값을 알려주시면 도와드릴게요! : ");
  while (scanf("%d", item) > 0){
    rbtree_insert(p,item);
  }

  return p;
}


node_t*makenode(rbtree *t,int item){
  node_t *node = malloc(sizeof(node_t));
  node->key = item;
  node->color = RBTREE_RED;
  node->left = t->nil;
  node->right = t->nil;
  node->parent = t->nil;
  return node;
}


void delete_rbtree(rbtree *t) {
  node_t *pre = t->root;
  node_t *cur = t->root->left;

  while (t->root->right != t->nil || t->root->left != t->nil){
    
    //좌측 밑바닥으로로
    while(cur->left->key != t->nil){
      pre = cur;
      cur = cur->left;
    }

    //우측 밑바닥으로
    if(cur->right != t->nil){
      pre = cur;
      cur = cur->right;


    }

    //밑바닥까지 왔다면

    //해당 노드 방향 센티널로 변경
    if(cur->key >= pre->key){ 
      pre->right = t->nil;}

    else{
      pre->left = t->nil;}
    
    free(cur); //할당 해제
    cur = pre; //cur 한단계 올리기
    pre = cur->parent; //pre 한단계 올리기
  }
  free(t->root);
  free(t->nil);
  free(t);
}






node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *checker = t->root;
  
  //루트가 존재하지 않을 시 예외처리
  if (checker == NULL){        
    checker = makenode(t,key);
    checker->color = RBTREE_BLACK;
    t->root = checker;
    
    return t->root;
  }

  node_t *new_node = makenode(t, key);
  //제자리 찾아갈때까지 반복복
  while (1)
  {

    if( key >= checker->key ){
      //우측에 아무것도 없을 때때
      if(checker->right ==NULL){
        checker->right= new_node;
        break;
      }
      //뭔가 있어어
      else
        checker = checker-> right;
    }

    else{
      //왼쪽에 아무것도 없을 때때
      if(checker->left ==NULL){
        checker->left= new_node;
        break;
      }
      //뭔가 있어어
      else
        checker = checker-> left;
    }
    

  }
  new_node->parent = checker;

  //부모가 레드일때만 체크, 블랙은 패스, 루트인건 위에서 예외처리 했으니 경우의 수 없음
  if (new_node->parent->color == RBTREE_RED){ 
    INSERTChecking(new_node);

    //자리 위치 바뀌었을 때 대비해서 루트 최신화
    while (t->root->parent != t->nil){
        t->root = t->root->parent;
      }
      
    }

  return t->root;
}


//삽입 조건 만족 확인
void INSERTChecking(node_t *node){
  //바텀 업 구상이기에 루트까지 올라가면 블랙처리 후 리턴
  if (node->parent->key == NULL){ //루트의 대가리는 NIL
    node->color = RBTREE_BLACK;
    return;
  }

  //RnR 위반 시
  if (node->color = RBTREE_RED && (node->left->color==RBTREE_RED || node->right->color == RBTREE_RED)){
    //노드가 우측일 때
    if(node->key >= node->parent->key){

      //형제 노드가 레드일 때
      if(node->parent->left->color == RBTREE_RED){
        node->color= RBTREE_BLACK;
        node->parent->left->color = RBTREE_BLACK;
        node->parent->color = RBTREE_RED;
      }

      else{ //자식도 동일 방향일 때
        node->parent->color = RBTREE_RED;
        if(node->right->color == RBTREE_RED){
          node->color = RBTREE_BLACK;
          L_rocate(node); //좌회전
        }
        else{
          node->left->color = RBTREE_BLACK;
          R_rocate(node->left); //혹은 우회전
          L_rocate(node->parent); //다시 좌회전..인데 자식노드랑 바꼈으니 부모의 부모로 회전
        }

      }

    }

    //노드가 좌측일 때때
    else{
      //형제 노드가 레드일 때
      if(node->parent->right->color == RBTREE_RED){
        node->color= RBTREE_BLACK;
        node->parent->right->color = RBTREE_BLACK;
        node->parent->color = RBTREE_RED;
      }

      else{ //자식도 동일 방향일 때
        node->parent->color = RBTREE_RED;
        if(node->left->color == RBTREE_RED){
          node->color = RBTREE_BLACK;
          R_rocate(node); //우회전
        }
        else{
          node->right->color = RBTREE_BLACK;
          L_rocate(node->right); //좌회전
          R_rocate(node->parent); //혹은 우회전..인데 자식노드랑 바꼈으니 부모의 부모로 회전
        } 
      }
    }

  }
  
  INSERTChecking(node->parent);
}


void L_rotate(node_t *node){
  node->left->parent = node->parent;
  node->parent->right = node->left; //노드 부모가 노드 좌측을 데려감
  node->left = node->parent;  //노드 자식으로 노드 부모가 이동
  node->parent = node->parent->parent; //노드의 부모를 원래 부모의 부모로 변경(노드 격상)
  node->left->parent = node;  //양방향 연결 마감감
  return;
} 

void R_rotate(node_t *node){
  node->right->parent = node->parent;
  node->parent->left = node->right; //노드 부모가 노드 좌측을 데려감
  node->right = node->parent;  //노드 자식으로 노드 부모가 이동
  node->parent = node->parent->parent; //노드의 부모를 원래 부모의 부모로 변경(노드 격상)
  node->right->parent = node;  //양방향 연결 마감감
  return;
} 


//중요 : 루트가 바꼈을 때 최신화가 아직 안되어있음 이거 꼭 반영해야됨, 회전 함수에 넣는걸 우선 생각중 -> 이중포인터로 넣자






node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *checker = t->root;

  //비어있거나 센티널만 있을 때 대비 예외 처리
  if(checker == NULL || checker == t->nil)
    return NULL;

  while(checker != t->nil){
    if(checker->key > key){
      checker = checker->right;
    }

    else if(checker->key < key){
      checker = checker->left;
    }

    else //중복이면 가장 먼저 조회된 놈부터
      return checker;
  }

  return NULL;


}



node_t *rbtree_min(const rbtree *t) {
  node_t *checker = t->root;

  //비어있거나 센티널만 있을 때 대비 예외 처리
  if(checker == NULL || checker == t->nil)
    return NULL;

  //래프트가 센티널이어야 최소
  while (checker->left != t->nil){
    checker = checker->left;
  }
  
  return checker;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *checker = t->root;

  //비어있거나 센티널만 있을 때 대비 예외 처리
  if(checker == NULL || checker == t->nil)
    return NULL;

  //라이트가 센티널이어야 최대
  while (checker->right != t->nil){
    checker = checker->right;
  }
  
  return checker;
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