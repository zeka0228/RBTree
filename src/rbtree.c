#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>


////시간에 쫓기며 하느라, 정말 주석은 생각도 못하고 있었습니다... 일단 코드가 좌우대칭 같은 형태로 짜놔서... 
/////좌측 구성이 우측 구성과 같아, 좌측만 우선 보셔도 무방합니다.. ㅜㅠㅠㅜㅜㅠㅜㅠㅠ



node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);
void INSERTChecking(rbtree *, node_t *node);
int rbtree_to_array(const rbtree *, key_t *, const size_t);
void R_rotate(rbtree *t, node_t *node);
void L_rotate(rbtree *t, node_t *node);
void REMOVEchecking(rbtree *t, node_t *node);
void delete_node(rbtree *t, node_t *node);
int checking(const rbtree *t, key_t *arr, node_t *node, const size_t n, int wak);



//*** 너 초기화 되지 않은 값 여기 있다니까 내일 꼭 확인해라 제발발


//트리 여러개 생성 케이스도 만들어야함!!!!! 
rbtree *new_rbtree(void) {
  //센티널, 빈 트리 만들기기 
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *SENTI = (node_t *)malloc(sizeof(node_t));
  SENTI->color = RBTREE_BLACK;  //센티널은 블랙 고정정

  //nil은 센티널, 센티널의 래프트도 센티널 센티널의 라이트도 센티널 센티널의 부모도 센티널 루트도 센티널
  p->nil = SENTI;
  p->nil->left   = p->nil;
  p->nil->right  = p->nil;
  p->nil->parent = p->nil;
  p->root = p->nil;

  return p;
}

//노드 만들기
node_t*makenode(rbtree *t,int item){
  //빈 노드 만들기
  node_t *node = malloc(sizeof(node_t));

  //기본 세팅, 아이템 넣어주고, 색깔은 기본 레드, 레프트 라이트 부모 모드 일단은 센티널널
  node->key = item;
  node->color = RBTREE_RED;
  node->left = t->nil;
  node->right = t->nil;
  node->parent = t->nil;
  return node;
}

//트리 지우기
void delete_rbtree(rbtree *t) {

  //없으면 그냥 리턴
  if (t == NULL) return;

  //빈 트리면 센티널이랑 트리 삭제
  if(t->root==t->nil){
    t->root = NULL;
    free(t->nil);
    free(t);
    return;
  }
  
  //딜리트 노드로 하나하나 차례 차례 밀어주기
  delete_node(t,t->root);
  free(t->nil);
  free(t);
}


void delete_node(rbtree *t, node_t *node){
  //왼쪽이 빌때가지 내려가기기
  if(node->left != t->nil)
    delete_node(t, node->left);

  //오른쪽이 빌 때까지 내려가기
  if(node->right != t->nil)
    delete_node(t, node->right);
  
  //최하층 비우기
  free(node);
  return;  
}


//RB트리 삽입하기
node_t *rbtree_insert(rbtree *t, const key_t key) {
  
  //시작은 루트로 설정
  node_t *checker = t->root;
  
  //루트 노트가 별개로 존재하지 않을 시 예외처리
  if (checker == t->nil){
    
    //새로 만들기
    checker = makenode(t,key);

    //루트 기본 세팅 : 검정, 부모는 센티널널
    checker->color = RBTREE_BLACK;
    checker->parent = t->nil;
    t->root = checker;
    
    return t->root;
  }

  //빈 트리가 아닐 떄떄
  node_t *new_node = makenode(t, key);

  //key 값에 맞춰 알맞는 자리 찾아갈때까지 반복복
  while (1)
  {
    //키가 지금 노드 키 이상일때(중복은 우측으로)
    if( key >= checker->key ){

      //우측에 아무것도 없을 때때
      if(checker->right == t->nil){
        checker->right= new_node;
        break;
      }
      //우측에 뭔가 있으면 최신화
      else
        checker = checker-> right;
    }

    else{
      //왼쪽에 아무것도 없을 때때
      if(checker->left == t->nil){
        checker->left= new_node;
        break;
      }
      //좌측에 뭔ㄱ ㅏ있으면  최신화화
      else
        checker = checker-> left;
    }
    

  }
  //새 노드의 부모는 최신화된 checker
  new_node->parent = checker;


  //부모가 레드일때만 체크, 블랙은 패스
  //지금 노드가 루트인건 위에서 예외처리 했으니 경우의 수 없음 = 지금 노드는 모두 레드
  if (new_node->parent->color == RBTREE_RED){ 
    INSERTChecking(t, new_node);

    //회전으로 루트였던 노드가 내려왔을 때 대비해서 루트 최신화
    while (t->root->parent != t->nil){
        t->root = t->root->parent;
      }
    }

  return t->root;
}


//삽입 조건 만족 확인하기(RnR 체크하기)
//해당 함수는 RnR 시 부모 노드를 기준으로 작동합니다. 즉 자식 중에 레드가 있는지 체크함
void INSERTChecking(rbtree *t, node_t *node){
  
  //바텀 업 구상이기에 루트까지 올라가면 2번규칙으로 루트 블랙처리 후 리턴
  if (node->parent == t->nil){ //루트의 부모는 NIL
    node->color = RBTREE_BLACK;
    return;
  }

  //RnR 위반 시
  if (node->color == RBTREE_RED && (node->left->color==RBTREE_RED || node->right->color == RBTREE_RED)){
    
    //지금 노드가 우측일 때
    if(node->key >= node->parent->key){

      //형제 노드가 레드일 때(케이스 1번)
      if(node->parent->left->color == RBTREE_RED){
        //지금 노드와 형제 노드 모두 블랙
        node->color= RBTREE_BLACK;
        node->parent->left->color = RBTREE_BLACK;
        //부모 노드 빨강강
        node->parent->color = RBTREE_RED;
      }

      //형제 노드가 검정일 때
      else{ 
        
        //부모 노드 빨강강
        node->parent->color = RBTREE_RED;

        //자식도 동일 방향일 때(케이스 3번)
        if(node->right->color == RBTREE_RED){
          node->color = RBTREE_BLACK;
          L_rotate(t, node); //좌회전
        }

        //자식이 반대 방향일 때때 (케이스2번)
        else{

          //자식 노드 검정
          node->left->color = RBTREE_BLACK;
          R_rotate(t, node->left); //자식 기준 우회전 -> 줄 맞춤
          t->nil->left = t->nil->right = t->nil;
          L_rotate(t, node->parent); //다시 좌회전..인데 지금 노드가 자식이고, 자식노드가 부모가 됐으니 node->parent를 기준으로 돌리기기
          t->nil->left = t->nil->right = t->nil;
        }

      }

    }

    //노드가 좌측일 때때(위 코드와 동일, 방향만 다름름)
    else{

      //형제 노드가 레드일 때(케이스 1)
      if(node->parent->right->color == RBTREE_RED){
        node->color= RBTREE_BLACK;
        node->parent->right->color = RBTREE_BLACK;
        node->parent->color = RBTREE_RED;
      }

      //형제 노드가 검정일 떄
      else{ 
        //부모 노드 빨강강
        node->parent->color = RBTREE_RED;

        //같은 방향의 자식이 빨강일때(케이스3) 
        if(node->left->color == RBTREE_RED){
          node->color = RBTREE_BLACK;
          R_rotate(t, node); //우회전
        }

        //반대 방향 자식이 빨강일때(케이스 2)
        else{
          node->right->color = RBTREE_BLACK;
          L_rotate(t, node->right); //좌회전으로 줄맞춤
          t->nil->left = t->nil->right = t->nil;
          R_rotate(t, node->parent); //혹은 우회전..인데 자식노드랑 바꼈으니 부모의 부모로 회전
          t->nil->left = t->nil->right = t->nil;
        } 
      }
    }

  }
  
  //재귀로 바텀업으로 체크
  return INSERTChecking(t, node->parent);
}

//좌회전 함수수
void L_rotate(rbtree *t, node_t *node){
  
  if(node->left != t->nil) node->left->parent = node->parent;

  node->parent->right = node->left; //노드 부모가 노드 좌측을 데려감
  node->left = node->parent;  //노드 자식으로 노드 부모가 이동
  node->parent = node->parent->parent; //노드의 부모를 원래 부모의 부모로 변경(노드 격상)
  if(node->parent->right == node->left)
    node->parent->right = node;
  else
    node->parent->left = node;
  node->left->parent = node;  //양방향 연결 마감감

  t->nil->left = t->nil->right =t->nil;
  return;
} 

//우회전 함수
void R_rotate(rbtree *t, node_t *node){
  if(node->right != t->nil) node->right->parent = node->parent;

  node->parent->left = node->right; //노드 부모가 노드 좌측을 데려감
  node->right = node->parent;  //노드 자식으로 노드 부모가 이동
  node->parent = node->parent->parent; //노드의 부모를 원래 부모의 부모로 변경(노드 격상)
  if(node->parent->right == node->right)
    node->parent->right = node;
  else
    node->parent->left = node;
  node->right->parent = node;  //양방향 연결 마감감

  t->nil->left = t->nil->right =t->nil;
  return;
} 


//중요 : 루트가 바꼈을 때 최신화가 아직 안되어있음 이거 꼭 반영해야됨, 회전 함수에 넣는걸 우선 생각중 -> 이중포인터로 넣자
//네~ 했어요~





//해당 값의 노트 찾기
node_t *rbtree_find(const rbtree *t, const key_t key) {
  
  //시작은 루트
  node_t *checker = t->root;

  //비어있거나 센티널만 있을 때 대비 예외 처리
  if(checker == NULL || checker == t->nil)
    return NULL;

  //센티널이 나올 때 까지
  while(checker != t->nil){

    //키가 작으면 왼쪽으로 조회
    if(checker->key > key){
      checker = checker->left;
    }

    //키가 더 크면 오른쪽으로 조회
    else if(checker->key < key){
      checker = checker->right;
    }

    else //중복이더라도 가장 먼저 조회된 놈부터 반환
      return checker;
  }
  
  //아무것도 없으면 null 반환
  return NULL;


}


//최소 찾기
node_t *rbtree_min(const rbtree *t) {
  node_t *checker = t->root;

  //비어있거나 센티널만 있을 때 대비 예외 처리
  if(checker == NULL || checker == t->nil)
    return NULL;

  //래프트가 센티널일떄까지 돌려서 최소 찾기
  while (checker->left != t->nil){
    checker = checker->left;
  }
  
  //최소 반환
  return checker;
}


//최대 찾기기
node_t *rbtree_max(const rbtree *t) {
  node_t *checker = t->root;

  //비어있거나 센티널만 있을 때 대비 예외 처리
  if(checker == NULL || checker == t->nil)
    return NULL;

  //라이트가 센티널일때까지 돌려서 최대 찾기
  while (checker->right != t->nil){
    checker = checker->right;
  }
  
  //최대 반환
  return checker;
}



//지우기
int rbtree_erase(rbtree *t, node_t *p) {
  color_t remove_color = p->color;
  color_t check_color;
  node_t *RP, *check;
  
  //빈 트리 일때 예외 처리리
  if(t->root == t->nil){
    return 0;
  }

  //지우는게 루트일 때때
  if(p == t->root){
    //RP : p와 자리를 교체할 노드
    RP = p;

    //우측에 노드가 있을 때때
    if(RP->right != t->nil){
      RP = RP->right;
      
      //더 내려갈 수 있을 때(당장의 RP가 후임자가 아니어서 래프트로 후임자 찾기)
      if(RP->left != t->nil){

        while (RP->left != t->nil){
          RP = RP->left;
        }

        //이중 블랙 체크를 돌릴 check 노드를 지정
        check = RP->right;
        //RP를 뺴내 갈거니, 꼬일 수 있는 간선 재배치
        check->parent = RP->parent;
        RP->parent->left = check;

        //RP 옮겨 심기
        t->root = RP;

        //RP 좌우 간선 연결해주기, 좌우 노드의 부모도 기존 p 에서 RP로 최신화 해주기
        RP->left = p->left;
        RP->right = p->right;
        p->left->parent = RP;
        p->right->parent = RP;

        //RP가 루트니, 부모는 센티널
        RP->parent = t->nil;


        //이중 블랙 확인용 컬러 저장
        check_color = RP->color;

        //루트니 블랙으로 변경
        RP->color = RBTREE_BLACK;
      }

      //바로 오른쪽에 있는게 후임자일 때때
      else{
        //p를 RP로 옮겨 심기
        t->root = RP;

        //기존 p->left 간선 연결해주기, 왼쪽 노드 부모도 RP로 최신화
        RP->left = p->left;
        p->left->parent = RP;

        //루트니 부모는 센티널
        RP->parent = t->nil;

        //이중 블랙 확인용 체크 형성
        //차피 좌측 없으니까 RP우측만 넣어주면 됨
        check = RP->right;
        
        //이중블랙 확인용 컬러 저장
        check_color = RP->color;
        
        //루트니까 검정
        RP->color = RBTREE_BLACK;
      }  
    }
    
    //왼쪽에만 존재할 때 (우측이 없다는건 -> 루트, 좌측노드 총 2개 노드만 존재한다는거(근거 : 한쪽 자식만 존재한다면 이 자식 개수는 최대 한개, 두개면 5번 규칙 위반))
    else if (p->left != t->nil)
    {
      //p와 교체할 RP 생성 및 교체
      RP = p->left;
      t->root = RP;
      
      //루트니까 부모는 센티널
      RP->parent = t->nil;

      //이중블랙 확인용 체크 생성, 자식 없으니, t->nil이 체크여도 무방
      check = t->nil;
      check->parent = RP;

      //이중 블랙 확인용 컬러 생성
      check_color = RP->color;

      //루트니까 블랙
      RP->color = RBTREE_BLACK;
    }    
    
    //루트 혼자만 존재할 때
    else{
      //트리 공백으로 만들기
      t->root = t->nil;

      //프리~
      free(p);
      return 0;
    }
    
  }

  //지우는게 루트가 아닐 때
  else{

    //검색해서 나온게 왼쪽 노드 였을 때
    if(p == p->parent->left){
      
      //검색해서 나온게(p) 왼쪽 노드고, 자식 노드가 하나인데, 그게 오른쪽일 때
      if(p->left == t->nil){
        //p가 왼쪽 노드니까, 그 자리를 하나뿐인 p의 자식 오른쪽이 대체
        p->parent->left = p->right;
        p->right->parent = p->parent;

        //이중블랙 확인용 컬러 저장, 대체되는 오른쪽 자식은 블랙으로 전환
        check_color = p->right->color; 
        p->right->color = RBTREE_BLACK;

        //이중블랙 확인용용
        check = p->right;
       
      }

      ////검색해서 나온게(p) 왼쪽 노드고, 자식 노드가 하나인데, 그게 왼쪽쪽일 때
      else if(p->right == t->nil){
        p->parent->left = p->left;

        p->left->parent = p->parent;

        check_color = p->left->color;
        p->left->color = RBTREE_BLACK;

        check = p->left;
    
      }

      //자식 노드가 두개일 때
      else{
        RP = p->right; //후임자 찾기용용
        if(RP->left != t->nil){
          while (RP->left != t->nil){
            RP = RP->left;
          }
          //후임차 찾았을 때 이를 p와와 교체
          RP->parent->left = RP->right;
          RP->right->parent = RP->parent;

          //이중블랙 체크용용
          check = RP->parent->left;

          p->parent->left = RP;
          RP->parent = p->parent;

          RP->right = p->right;
          p->right->parent = RP;
          RP->left = p->left;
          p->left->parent = RP;

          check_color = RP->color;
          RP->color = RBTREE_BLACK;

         
        }

        //바로 오른쪽이 후임자 였을 때때
        else{
          p->parent->left = RP;
          RP->parent = p->parent;
          
          RP->left = p->left;
          p->left->parent = RP;

          check_color = RP->color;
          RP->color = RBTREE_BLACK;

          //이중블랙 체크용
          check = RP;
        }
      }
      
    }  


    //우측 노드일때때 (좌측과 코드 구성 동일일)
    else{
      //자식 노드가 하나일때
      if(p->left == t->nil){
        p->parent->right = p->right;

        p->right->parent = p->parent;
        
        check_color = p->right->color; 
        p->right->color = RBTREE_BLACK;
        check = p->right;
      
      }

      else if(p->right == t->nil){
        p->parent->right = p->left;

        p->left->parent = p->parent;

        check_color = p->left->color;
        p->left->color = RBTREE_BLACK;
        check = p->left;
      }

      //자식 노드가 두개일 때
      else{
        RP = p->right; //후임자 찾기용용
        if(RP->left != t->nil){
          while (RP->left != t->nil){
            RP = RP->left;
          }
          RP->parent->left = RP->right;
          RP->right->parent = RP->parent;
          check = RP->parent->left;

          p->parent->right = RP;
          RP->parent = p->parent;

          RP->right = p->right;
          p->right->parent = RP;
          RP->left = p->left;
          p->left->parent = RP;

          check_color = RP->color;
          RP->color = RBTREE_BLACK;

         
        }
        else{
          p->parent->right = RP;
          RP->parent = p->parent;
          
          RP->left = p->left;
          p->left->parent = RP;

          check_color = RP->color;
          RP->color = RBTREE_BLACK;
          
          check = RP;
        }
      }
      
      
    }
    
  }
  

  free(p); //p 할당 해제제
  if (remove_color ==RBTREE_BLACK && check_color == RBTREE_BLACK){
        REMOVEchecking(t, check); // 체킹~
  }
    

  t->nil->parent = t->nil;
  return 0;
}

//이중블랙일때만 실행되도록 설계 **중요:형제가 nil일 수는 없다 내가 일단 블랙인데 옆에 블랙이 없으면 그건 잘못된거겠지???
//잘못된 건데 있었다!!!!!! 다 죽 자!!!!
//04.23 고쳤다!!!! 케이스 2번 스타일로 적용 시키면 된다!!!! 밸런스 테스트로도 문제 없었다!!!!
void REMOVEchecking(rbtree *t, node_t *node){
  //혹시 몰라서 nil 좌우 세팅 
  t->nil->left = t->nil;
  t->nil->right = t->nil;

  //루트 예외 설계, 비어도 리턴
  if(node == NULL) return;
  if(node == t->root || node->parent == t->nil){
    return;
  }

  

  node_t *silbling;
 
  //노드가 왼쪽에 속했을 때
  if(node == node->parent->left ){
    silbling = node->parent->right;
    
    //형제가 센티널일 때 예외 처리리
    if(silbling == t->nil)
      return REMOVEchecking(t, node->parent);


    //형제가 빨강(케이스 1)
    if(silbling->color == RBTREE_RED){

      silbling->color = RBTREE_BLACK;
      node->parent->color = RBTREE_RED;

      L_rotate(t, silbling);

      while (t->root->parent != t->nil){
        t->root = t->root->parent;
      }

      return REMOVEchecking(t, node); 
      
    }

    //형제가 검정 
    else{

      //형제 방향과 같은 방향 자식이 빨강 (케이스 3)  ***중요 : 둘다 레드여도 얘가 우선순위라 이게 맞음!!!

      if( silbling->right->color == RBTREE_RED){

        silbling->color = silbling->parent->color;
        silbling->right->color = RBTREE_BLACK;
        silbling->parent->color = RBTREE_BLACK;

        L_rotate(t, silbling);

        t->nil->parent = silbling->parent;

      }
      //다른 방향 자식이 빨강 (케이스 4)
      else if( silbling->left->color == RBTREE_RED ){

        silbling->color = RBTREE_RED;
        silbling->left->color = RBTREE_BLACK;

        R_rotate(t, silbling->left);

        return REMOVEchecking(t, node); //다시 실행해서 위에꺼 작동되도록, 그래서 레드 준겨 ㅇㅇㅇ

      }

      //둘다 검정 (케이스 2)
      else{
        
        silbling->color = RBTREE_RED;

        if(node->parent->color == RBTREE_BLACK)
          return REMOVEchecking(t, node->parent);
         
        //레드면 재귀 없이 블랙 변경 후 끝
        node->parent->color = RBTREE_BLACK;
      }
    }
  }



  //오른쪽일 때!!!!!!!!!!! (좌측이었을 때와 구성 같음)
  else{
    silbling = node->parent->left;
    if(silbling == t->nil)
      return REMOVEchecking(t, node->parent);

    //형제가 빨강
    if(silbling->color == RBTREE_RED){
      silbling->color = RBTREE_BLACK;
      node->parent->color = RBTREE_RED;
      R_rotate(t, silbling);
      while (t->root->parent != t->nil){
        t->root = t->root->parent;
      }

      return REMOVEchecking(t, node);
      
    }

    //형제가 검정 
    else{

      //형제 방향과 같은 방향 자식이 빨강  ***중요 : 둘다 레드여도 얘가 우선순위라 이게 맞음!!!
      if(silbling->left->color == RBTREE_RED ){
        silbling->color = silbling->parent->color;
        silbling->left->color = RBTREE_BLACK;
        silbling->parent->color = RBTREE_BLACK;
        R_rotate(t, silbling);

      }
      //다른 방향 자식이 빨강
      else if(silbling->right->color == RBTREE_RED){
        silbling->color = RBTREE_RED;
        silbling->right->color = RBTREE_BLACK;
        L_rotate(t, silbling->right);
        return REMOVEchecking(t, node); //다시 실행해서 위에꺼 작동되도록, 그래서 레드 준겨 ㅇㅇㅇ
      }

      //둘다 검정
      else{
        
        silbling->color = RBTREE_RED;
        if(node->parent->color == RBTREE_BLACK)
          return REMOVEchecking(t, node->parent);
        node->parent->color = RBTREE_BLACK;
      }
    }



  }

  //각 케이스 끝나고, 루트 혹시 모르니 최신화
  while (t->root->parent != t->nil){
    t->root = t->root->parent;
  }
  return;
}




int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  checking(t, arr, t->root, n, 0);
  return 0;
}

//중위 순회로 구현현
int checking(const rbtree *t, key_t *arr, node_t *node, const size_t n, int wak){
  if(node == t->nil) return wak;
  if(wak == n) return wak;

  wak = checking(t, arr, node->left, n, wak);
  
  if(wak == n) return wak;
  *(arr + wak) = node->key;
  wak++;

  if(wak == n) return wak;
  wak = checking(t, arr, node->right, n, wak);
  return wak;
}
<<<<<<< HEAD
=======

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  return t->root;
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
=======





























>>>>>>> 700609f (주석 최신화 짜잘 버그 최신화 진짜진짜진짜 최종종)

