#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

char regexp[100];
int current_point;
char current_char;

/*
  <正規表現> := <項> | <項><正規表現>
  <項>       := <因子> | <因子>+<項>
　<因子>     := (<正規表現>) | c | (<正規表現>)* | c*
*/

/*
  次の文字を読み込む
*/

void to_next(){
	current_point++;
	current_char = regexp[current_point];
}

/*
  アルファベットかどうかの判定
*/
int is_alphbet(char ch){

	if( 'a' <= ch && ch <= 'z' ){
		return TRUE;
	}
	return FALSE;
}

/*
  [正規表現]
*/
void regexp(){
	term();
	if(current_char == '\0'){
		return;
	}
	regexp();
}

/*
  [項]
*/
void term(){
  	facter();
	if(current_char != '+'){
		return;
	}
	to_next();
	term();
}

/*
  [因子]
*/
void facter(){
	// 現在の文字がアルファベットのとき
	if(is_alphbet(current_char)){
		// 文字を代入・遷移先初期化

		to_next();
		// 繰り返しでないなら終了
		if(cyrrent_char != '*'){
			return;
		}
		// 繰り返しなら次の文字へ
		to_next();

		// 繰り返しの状態を決定

		// 文字の遷移先に繰り返し状態を指定

		return;
	}
	// Error!!
	exit(0);
}


/*
   main関数

*/

int main(void){

	return 0;
}
