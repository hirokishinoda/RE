#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

/* 文字に出会ったらその文字のみ探索？ */
 
/*----------------------------
  
  NFA状態遷移表の状態

　ch    : 遷移するための文字
  next1 : 遷移先１
  next2 : 遷移先２

----------------------------*/
typedef struct nfa_state{
        char ch;
        int next1;
        int next2;
}nfa_state;

/*-----------------------------

  NFAのイプシロン遷移をまとめた
  遷移表の状態

  num  : 状態番号
  ch   : 遷移条件の文字
  next : イプシロン遷移による
         状態をまとめた集合

-----------------------------*/


/*
*/
typedef struct set_cell{
	int num;
	struct set_cell* next;
}set_cell;

void term(int *inlex,int *outlex);
void facter(int *inlex,int *outlex);
void add_set(int n);
/*------------------------------
  木構造作成　変数
------------------------------*/
char regexp_str[100];
int current_point;
char current_char;
 
/*------------------------------
  遷移表　変数
------------------------------*/
nfa_state state_table[100];
int n;
int m;


set_cell *root = NULL;

/*-------------------------------
   次の文字を読み込む
 -------------------------------*/
void to_next(){
        current_point++;
        current_char = regexp_str[current_point];
}

/*--------------------------------
  アルファベットかどうかの判定
--------------------------------*/
int is_alphabet(char ch){
 
        if( 'a' <= ch && ch <= 'z' ){
                return TRUE;
        }
        return FALSE;
}

/*-------------------------------
  新しい状態を作成する
-------------------------------*/
int new_state(){
        static int value = 0;
        value++;
        return value;
}

/*--------------------------------------
  構文木作成用の文法

  <正規表現> := <項> | <項>"|"<正規表現>
  <項>       := <因子> | <因子> <項>
  <因子>     := <文字> | <文字>*

---------------------------------------*/

void regexp(int *inlet,int *outlet){
	int inlet2, outlet2;

	term(inlet,outlet);
	if(current_char != '|' || current_char == '\0'){
		return;
	}
	to_next();
	regexp(&inlet2,&outlet2);
	
	n = new_state();
        state_table[n].ch = 0;
         
        m = new_state();
        state_table[m].ch = 0;

        state_table[n].next1 = *inlet;
        state_table[n].next2 = inlet2;

        state_table[*outlet].next1 = m;
        if(state_table[*outlet].next2 == 0){
		state_table[*outlet].next2 = m;
	}

        state_table[outlet2].next1 = m;
	if(state_table[outlet2].next2 == 0){
        	state_table[outlet2].next2 = m;
	}

        state_table[m].next1 = state_table[m].next2 = 0;

        *inlet = n;
        *outlet = m;

	return;
}

void term(int *inlet,int *outlet){
	int inlet2, outlet2;

	facter(inlet,outlet);
	if( !(is_alphabet(current_char)) ){
		return;	
	}
	term(&inlet2,&outlet2);

	if(state_table[*outlet].next1 == 0){ 
		state_table[*outlet].next1 = inlet2;
	}
	if(state_table[*outlet].next2 == 0){
                state_table[*outlet].next2 = inlet2;
        }

        *outlet = outlet2;

	return;
}

void facter(int *inlet,int *outlet){
	if(is_alphabet(current_char)){
		// 文字を代入・遷移先初期化
		*inlet = *outlet = new_state();
                state_table[*outlet].ch = current_char;
                state_table[*outlet].next1 = 0;
                state_table[*outlet].next2 = 0;

		to_next();
		if(current_char != '*'){
			return;
		}
		to_next();

		// 繰り返しの遷移前状態
                n = new_state();
                state_table[n].ch = 0;
                state_table[n].next1 = 0;
                state_table[n].next2 = *inlet;

                // 繰り返し状態
                state_table[*outlet].next1 = n;
                state_table[*outlet].next2 = n;

                *inlet = *outlet = n;
		return;
	}

	printf("Error!!\n");
	exit(0);
}

/*--------------------------------
  最初の文字を調べるための初期化
--------------------------------*/
void init_first_char(){
        current_point = 0;
        current_char = regexp_str[current_point];
}


/*-------------------------------
  最初の状態を作成
--------------------------------*/
void create_start_state(int *inlet,int *outlet){
        state_table[0].ch = 0;
        state_table[0].next1 = *inlet;
        state_table[0].next2 = *inlet;
}

/*--------------------------------
  終了の状態を作成
--------------------------------*/
void create_end_state(int *inlet,int *outlet){
        n = new_state();
        state_table[n].ch = 0;
        state_table[n].next1 = 0;
        state_table[n].next2 = 0;

        if(state_table[*outlet].next1 == 0){
                state_table[*outlet].next1 = n;
        }
        if(state_table[*outlet].next2 == 0){
                state_table[*outlet].next2 = n;
        }
}

/*--------------------------------
  状態遷移図の表示
---------------------------------*/
void show_state(){
        int i;

	printf("-[state]-[next1]-[next2]-\n");

        for(i = 0;i < 100;i++){

                printf("[%3d]",i);
                if(is_alphabet(state_table[i].ch)){
                        printf("%c ",state_table[i].ch);
                }else{
                        printf("%d ",state_table[i].ch);
                }

                printf("%3d ,%3d \n",state_table[i].next1, state_table[i].next2);

                if(state_table[i].next1 == 0){
                        return ;
                }
        }
}

/*--------------------------------
  正規表現からNFA状態遷移表を作成
--------------------------------*/
void create_NFA(int *inlet,int *outlet){
 	
	init_first_char();

	regexp(inlet,outlet);
	create_start_state(inlet,outlet);
	create_end_state(inlet,outlet);

	show_state();                                         
}

/*--------------------------------
  
--------------------------------*/
void x(int state_num,char state_char){
	
	if(state_table[state_num].next1 == 0 && state_table[state_num].next2 == 0){
		add_set(state_num);
		return ;
	}

	if(state_table[state_num].next1 == state_table[state_num].next2){
		if(state_table[state_num].ch == 0 || state_table[state_num].ch == state_char){
			x(state_table[state_num].next1,state_char);
			add_set(state_num);
		}
	}else{
		if(state_table[state_num].ch == 0 || state_table[state_num].ch == state_char){
			x(state_table[state_num].next1,state_char);
		}

		if(state_table[state_num].next2 != state_num){
			x(state_table[state_num].next2,state_char);
		}
		add_set(state_num);
	}

	return;
}

set_cell *create_set_cell(int state_num){
	set_cell *tmp;
	
	tmp = (set_cell *)malloc(sizeof(set_cell));
	tmp->num = state_num;
	tmp->next = NULL;

	return tmp;
}

void add_set(int state_num){
	set_cell *p;
		
	for(p = root;p->next != NULL;p = p->next){
	}	

	p->next = create_set_cell(state_num);
}
                                         
void disp_list(){
	set_cell *p;

	printf("{");
	for(p = root;p != NULL;p = p->next){
		printf("%d,",p->num);
	}
	puts("}");
}

 
/*--------------------------------
  main関数
--------------------------------*/
int main(void){
        int inlet;
        int outlet;
	//set_cell root;

        //strcpy(regexp_str,"ab|cd*"); //OK
	//strcpy(regexp_str,"a*"); //OK
        //strcpy(regexp_str,"ab*"); //OK

	printf("input regexp -> ");
	scanf("%s",&regexp_str);

        create_NFA(&inlet,&outlet);

	root = create_set_cell(100);
	x(0,'a');

	disp_list();

        return 0;
}


