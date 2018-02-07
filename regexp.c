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
  優先順位の確認

  1.[*] ... 繰り返し処理
  2.[ab]... 連結処理
  3.[|] ... or処理
--------------------------------------*/

/*--------------------------------------
  構文木作成用の文法

  <正規表現> := <項> | <項>"|"<正規表現>
  <項>       := <因子> | <因子> <項>
  <因子>     := <文字> | <文字>*

---------------------------------------*/

/*--------------------------------------
  <正規表現>に関する処理
--------------------------------------*/
void regexp(int *inlet,int *outlet){
	int inlet2, outlet2;

	term(inlet,outlet);
	if(current_char != '|' || current_char == '\0'){
		return;
	}
	// 現在の文字が | だった場合の処理(orの処理)
	to_next();
	regexp(&inlet2,&outlet2);

	// orによって別れる元になる状態(n)
	n = new_state();
        state_table[n].ch = 0;
        // orによって別れた状態が一つになる状態(m)
        m = new_state();
        state_table[m].ch = 0;

	// 別れる前の状態にそれぞれ次の状態を設定
        state_table[n].next1 = *inlet;
        state_table[n].next2 = inlet2;

	// next2に関しては次の状態が決まっていない場合.
        // [*]繰り返し処理など次への移動が決まっている
        // ものを上書きしないため.
        state_table[*outlet].next1 = m;
        if(state_table[*outlet].next2 == 0){
		state_table[*outlet].next2 = m;
	}
        state_table[outlet2].next1 = m;
	if(state_table[outlet2].next2 == 0){
        	state_table[outlet2].next2 = m;
	}

	// 合流した状態の次の状態を終了状態で初期化
        state_table[m].next1 = state_table[m].next2 = 0;

        *inlet = n;
        *outlet = m;

	return;
}

/*----------------------------------
  <項>に関する処理
----------------------------------*/
void term(int *inlet,int *outlet){
	int inlet2, outlet2;

	facter(inlet,outlet);
	if( !(is_alphabet(current_char)) ){
		return;	
	}
	// 現在の文字がアルファベットだった場合の処理
	term(&inlet2,&outlet2);
	// 連結の処理
	if(state_table[*outlet].next1 == 0){ 
		state_table[*outlet].next1 = inlet2;
	}
	if(state_table[*outlet].next2 == 0){
                state_table[*outlet].next2 = inlet2;
        }

        *outlet = outlet2;

	return;
}

/*-------------------------------
  <因子>に関する処理
--------------------------------*/
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
		// 現在の文字が * の場合の処理(繰り返し)
		to_next();

		// 繰り返しに遷移する前の状態
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

	printf("-[state]-[char]-[next1]-[next2]-\n");

        for(i = 0;i < 100;i++){

                printf("-[%5d]-",i);
                if(is_alphabet(state_table[i].ch)){
                        printf("[%4c]",state_table[i].ch);
                }else{
                        printf("[%4d]",state_table[i].ch);
                }

                printf("-[%5d]-[%5d]-\n",state_table[i].next1, state_table[i].next2);

                if(state_table[i].next1 == 0){
                        return ;
                }
        }
}

/*-------------------------------
  Reguler exoression -> NFA
  正規表現からNFAを作る関数
-------------------------------*/
void convert_regexp_into_NFA(int *inlet,int *outlet){
	init_first_char();
	
	regexp(inlet,outlet);

        create_start_state(inlet,outlet);

        create_end_state(inlet,outlet);
 
        show_state();
}

/*--------------------------------
   main関数
--------------------------------*/
int main(void){
        int inlet;
        int outlet;

	// 手入力によるテストケース
        // strcpy(regexp_str,"ab|cd*"); //OK
	// strcpy(regexp_str,"a*"); //OK
        // strcpy(regexp_str,"ab*"); //OK

	printf("input regexp -> ");
	scanf("%s",&regexp_str);

	convert_regexp_into_NFA(&inlet,&outlet);

        return 0;
}


