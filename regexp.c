#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

typedef struct state{
	char ch;
	int next1;
	int next2;
}state;

void term(int *inlex,int *outlex);
void facter(int *inlex,int *outlex);

/*------------------------------
  木構造作成　変数
------------------------------*/
char regexp_str[100];
int current_point;
char current_char;

/*------------------------------
  遷移表　変数
------------------------------*/
state state_table[100];
int n;
int m;

/*----------------------------------------------------
  <正規表現> := <項> | <項><正規表現>
  <項>       := <因子> | <因子>or<項>
　<因子>     := (<正規表現>) | c | (<正規表現>)* | c*
----------------------------------------------------*/

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

/*--------------------------------
  [正規表現]
--------------------------------*/
void regexp(int *inlet,int *outlet){
	int inlet2,outlet2;

	term(inlet,outlet);
	if(current_char == '\0'){
		return;
	}
	regexp(&inlet2,&outlet2);

	if(state_table[*outlet].next1 == 0){
		state_table[*outlet].next1 = inlet2;
	}
	if(state_table[*outlet].next2 == 0){
		state_table[*outlet].next2 = inlet2;
	}

	*outlet = outlet2;

	return ;
}

/*-------------------------------
  [項]
-------------------------------*/
void term(int *inlet,int *outlet){
	int inlet2,outlet2;

  	facter(inlet,outlet);
	// or(|)以外なら終了
	if(current_char != '|'){
		return;
	}
	// or(|)なら
	to_next();
	term(&inlet2,&outlet2);

	// 
	n = new_state();
	state_table[n].ch = 0;
	// 
	m = new_state();
	state_table[m].ch = 0;

	//
	state_table[n].next1 = *inlet;
	state_table[n].next2 = inlet2;

	state_table[*outlet].next1 = m;
	state_table[*outlet].next2 = m;

	state_table[outlet2].next1 = m;
	state_table[outlet2].next2 = m;

	state_table[m].next1 = state_table[m].next2 = 0; 

	//
	*inlet = n;
  	*outlet = m;

	return;
}

/*-------------------------------
  [因子]
-------------------------------*/
void facter(int *inlet,int *outlet){
	// 現在の文字がアルファベットのとき
	if(is_alphabet(current_char)){
		
		// 文字を代入・遷移先初期化
		*inlet = *outlet = new_state();
		state_table[*outlet].ch = current_char;
		state_table[*outlet].next1 = 0;
		state_table[*outlet].next2 = 0;
		
		to_next();
		// 繰り返しでないなら終了
		if(current_char != '*'){
			return;
		}
		// 繰り返しなら次の文字へ
		to_next();

		// 繰り返しの状態を決定(next1:初期化　next2:繰り返し対象)
		n = new_state();
		state_table[n].ch = 0;
		state_table[n].next1 = 0;
		state_table[n].next2 = *inlet;

		// 文字の遷移先に繰り返し状態を指定(文字->繰り返し)
		state_table[*outlet].next1 = n;
		state_table[*outlet].next2 = n;

		*inlet = *outlet = n;
		
		return;
	}
	// Error!!
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

/*--------------------------------
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
   main関数
--------------------------------*/

int main(void){
	int inlet;
	int outlet;

	strcpy(regexp_str,"a|b");

	init_first_char();

	regexp(&inlet,&outlet);

	create_start_state(&inlet,&outlet);

	create_end_state(&inlet,&outlet);

	show_state();

	return 0;
}
