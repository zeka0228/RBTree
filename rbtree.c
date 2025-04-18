#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>




node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);
void INSERTChecking(node_t *node);
int rbtree_to_array(const rbtree *, key_t *, const size_t);
void R_rotate(node_t *node);
void L_rotate(node_t *node);
void REMOVEchecking(node_t *node);










//트리 여러개 생성 케이스도 만들어야함!!!!! 
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = malloc(sizeof(node_t)); //센티널만들기
  p->nil->key = -1;
  p->nil->color = RBTREE_BLACK;  //블랙 고정정
  int item;
  printf ("트리를 만드려 하시는군요! 트리에 넣을 값을 알려주시면 도와드릴게요! : ");
  rbtree_insert(p,item);

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
    while(cur->left != t->nil){
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
  if (node->parent->key == -1){ //루트의 대가리는 NIL
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
          L_rotate(node); //좌회전
        }
        else{
          node->left->color = RBTREE_BLACK;
          R_rotate(node->left); //혹은 우회전
          L_rotate(node->parent); //다시 좌회전..인데 자식노드랑 바꼈으니 부모의 부모로 회전
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
          R_rotate(node); //우회전
        }
        else{
          node->right->color = RBTREE_BLACK;
          L_rotate(node->right); //좌회전
          R_rotate(node->parent); //혹은 우회전..인데 자식노드랑 바꼈으니 부모의 부모로 회전
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
  color_t remove_color = p->color;
  color_t check_color;
  node_t *RP;
  //루트일 때 예외처리리



  //검색해서 나온게 왼쪽 노드 였을 때
  if(p->key < p->parent->key){
    //자식 노드가 하나일때
    if(p->left == t->nil){
      p->parent->left = p->right;

      p->right->parent = p->parent;

      check_color = p->right->color; 
      p->right->color = RBTREE_BLACK;
      RP = p->right;
    }

    else if(p->right == t->nil){
      p->parent->left = p->left;

      p->left->parent = p->parent;

      check_color = p->left->color;
      p->left->color = RBTREE_BLACK;
      RP = p->left;
    }

    //자식 노드가 두개일 때
    else{
      RP = p->right; //후임자 찾기용용
      while (RP->left = t->nil){
        RP = RP->left;
      }
      RP->parent->left = RP->right;
      RP->right->parent = RP->parent;

      p->parent->left = RP;
      RP->parent = p->parent;
      RP->right = p->right;
      p->right->parent = RP;

      check_color = RP->color;
      RP->color = RBTREE_BLACK;

    }
    
  }  


  //우측 노드일때때
  else{
    //자식 노드가 하나일때
    if(p->left == t->nil){
      p->parent->right = p->right;

      p->right->parent = p->parent;

      check_color = p->right->color; 
      p->right->color = RBTREE_BLACK;
      RP = p->right;
    }

    else if(p->right == t->nil){
      p->parent->right = p->left;

      p->left->parent = p->parent;

      check_color = p->left->color;
      p->left->color = RBTREE_BLACK;
      RP = p->left;
    }

    //자식 노드가 두개일 때
    else{
      RP = p->right; //후임자 찾기용용
      while (RP->left = t->nil){
        RP = RP->left;
      }

      //우측 서브트리/리프노드 부모에 붙이기
      RP->parent->right = RP->right;
      RP->right->parent = RP->parent;


      p->parent->right = RP;
      RP->parent = p->parent;
      RP->right = p->right;
      p->right->parent = RP;

      check_color = RP->color;
      RP->color = RBTREE_BLACK;
    }
    
    
  }
    
  free(p);


  if (remove_color ==RBTREE_BLACK && check_color == RBTREE_BLACK){
        REMOVEchecking(RP);
  }
    


  t->nil->parent = NULL;
  return 0;
}

//이중블랙일때만 실행되도록 설계 **중요:형제가 nil일 수는 없다 내가 일단 블랙인데 옆에 블랙이 없으면 그건 잘못된거겠지???
void REMOVEcheking(node_t *node){
  //루트 예외 설계 필요
  if(node->parent->key == -1){
    return;
  }



  node_t *silbling;

  //방향 판별
  if(node == node->parent->left ){
    silbling = node->parent->right;
    //형제가 빨강
    if(silbling->color == RBTREE_RED){
      silbling->color = RBTREE_BLACK;
      node->parent->color = RBTREE_RED;
      L_rotate(node->parent);
      return REMOVEcheking(node);
      
    }

    //형제가 검정 
    else{

      //형제 방향과 같은 방향 자식이 빨강  ***중요 : 둘다 레드여도 얘가 우선순위라 이게 맞음!!!
      if(silbling->right->color == RBTREE_RED){
        silbling->color = silbling->parent->color;
        silbling->right->color = RBTREE_BLACK;
        silbling->parent->color = RBTREE_BLACK;
        L_rotate(silbling->parent);
      }
      //다른 방향 자식이 빨강
      else if(silbling->left->color == RBTREE_RED){
        silbling->color = RBTREE_RED;
        silbling->left->color = RBTREE_BLACK;
        R_rotate(silbling);
        return REMOVEcheking(node); //다시 실행해서 위에꺼 작동되도록, 그래서 레드 준겨 ㅇㅇㅇ
      }

      //둘다 검정
      else{
        silbling->color = RBTREE_RED;
        if(node->parent->color == RBTREE_BLACK)
          return REMOVEcheking(node->parent);
        node->parent->color = RBTREE_BLACK;
      }
    }
  }



  //오른쪽일 때!!!!!!!!!!!
  else{
    silbling = node->parent->left;
    //형제가 빨강
    if(silbling->color == RBTREE_RED){
      silbling->color = RBTREE_BLACK;
      node->parent->color = RBTREE_RED;
      R_rotate(node->parent);
      return REMOVEcheking(node);
      
    }

    //형제가 검정 
    else{

      //형제 방향과 같은 방향 자식이 빨강  ***중요 : 둘다 레드여도 얘가 우선순위라 이게 맞음!!!
      if(silbling->left->color == RBTREE_RED){
        silbling->color = silbling->parent->color;
        silbling->left->color = RBTREE_BLACK;
        silbling->parent->color = RBTREE_BLACK;
        R_rotate(silbling->parent);
      }
      //다른 방향 자식이 빨강
      else if(silbling->right->color == RBTREE_RED){
        silbling->color = RBTREE_RED;
        silbling->right->color = RBTREE_BLACK;
        L_rotate(silbling);
        return REMOVEcheking(node); //다시 실행해서 위에꺼 작동되도록, 그래서 레드 준겨 ㅇㅇㅇ
      }

      //둘다 검정
      else{
        silbling->color = RBTREE_RED;
        if(node->parent->color == RBTREE_BLACK)
          return REMOVEcheking(node->parent);
        node->parent->color = RBTREE_BLACK;
      }
    }



  }

  return;
}




int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}




