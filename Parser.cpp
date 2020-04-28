#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
//#include "Token_Tree.h"

#define MAX_CHAR 1000

using namespace std;

int counter;
int r_w[6];
char name[99];
int index;

struct tree_Node{
	char name[10];            //current state's name
	int state;                //current state's number
	int terminator;           //a int/boolean (0 or 1) to determine if it is a terminator character to know if it can allow a space/tab/newline as the next character
	tree_Node *child;         //a single child for Node x
	tree_Node *sibling;       //another child for Node x
};

struct saved_token{
	char token[10];           //name of the token
	char id[99];
	saved_token *next;        //next token for the file
};

struct queue{
	char *print;
	queue *next;
};


struct tree_Node *start_tree, *current_tree, *tmp;
struct saved_token *start_saved, *current_saved;
struct queue *front, *back, *tempQueue;

void parseError();
int recursionXML(int state);
void Scan(char *file);
void Interpreter(char character, char next);
void Tree_Create(int state, char character[10], int terminator);
void Tree_Process();
void Create_token(char token_type[10]);
void push(char id[99]);
void pop();

int main(int argc, char *argv[]){
	index = 0;
	FILE *fp;
	fp = fopen(argv[argc - 1], "r");
	char filename[MAX_CHAR];
	int state = 0;

	if(fp == NULL){
		cout << "Unable to open the file designated: " << argv[argc-1] << endl;
		cout << "Ending Program" << endl;
		exit(1);
	}
	else{
		back = front;
		Tree_Process();
		current_tree = start_tree;
		Scan(argv[argc-1]);
		current_saved = start_saved;
		recursionXML(0);
	}
	
	//for later use when printing out the tokens
	
	tempQueue = front;
	int tabs = 0;
	while(tempQueue != NULL){
		if(tempQueue -> print[0] == '<' && tempQueue -> print[1] == '/'){
			tabs -= 1;
			for(int i = tabs; i > 0; i--){
				cout << " ";
			};
			cout << tempQueue -> print << endl;
		}
		else if(tempQueue -> print[0] == '<'){
			for(int i = tabs; i > 0; i--){
				cout << " ";
			}
			tabs++;
			cout << tempQueue -> print << endl;
		}
		else{
			for(int i = tabs; i > 0; i--){
				cout << " ";
			}
			cout << tempQueue -> print << endl;
		}
		tempQueue = tempQueue -> next;
	}
	cout << endl;
	
	
	current_saved = start_saved;
	cout << "(";
	while(current_saved != NULL){
		cout << current_saved -> token;
		if(current_saved -> next != NULL){
			cout << ", ";
		}
		current_saved = current_saved -> next;
	}
	
	cout << ")" << endl;
	
	return 0;
}

void parseError()
{
	cout << "ERROR" << endl;
	exit(2);
}
	
int recursionXML(int state){
	int ret = 0;
	char *temp;
	
	switch(state){
		case 0:
			cout << "state " << state;
			temp = "<program>";
			push(temp);
			state = 1;
			ret = recursionXML(state);
			if(ret == 0 && current_saved == NULL){
				temp = "</program>";
				push(temp);
				cout << " -> $$)" << endl;
			}
			else{
				parseError();
			}
			break;
		case 1:
			cout << " -> (state " << state;
			temp = "<stmt_list>";
			push(temp);
			state = 2;
			ret = recursionXML(state);
			if(ret == 0){
				state = 1;
				ret = recursionXML(state);
				temp = "</stmt_list>";
				push(temp);
				cout << ")";
				return ret;
			}
			else{
				pop();
				temp = "</stmt_list>";
				push(temp);
				cout << " -> E)";
				return 0;
			}
			break;
		case 2:
			cout << " -> (state " << state;
			temp = "<stmt>";
			push(temp);
			if(current_saved != NULL && current_saved -> token[0] == 'i'){
				temp = "<id>";
				push(temp);
				push(current_saved -> id);
				temp = "</id>";
				push(temp);
				current_saved = current_saved -> next;
				if(current_saved != NULL && current_saved -> token[0] == 'a'){
					temp = "<assign>";
					push(temp);
					push(current_saved -> id);
					temp = "</assign>";
					push(temp);
					current_saved = current_saved -> next;
					state = 3;
					ret = recursionXML(state);
					cout << ")";
					return ret;
				}
				else{
					parseError();
				}
			}
			else if(current_saved != NULL && current_saved -> token[1] == 'e'){
				temp = "<read>";
				push(temp);
				push(current_saved -> id);
				temp = "</read>";
				push(temp);
				current_saved = current_saved -> next;
				if(current_saved != NULL && current_saved -> token[0] == 'i'){
					temp = "<id>";
					push(temp);
					push(current_saved -> id);
					temp = "</id>";
					push(temp);
					current_saved = current_saved -> next;
					temp = "</stmt>";
					push(temp);
					return 0;
				}
				else{
					parseError();
				}
			}
			else if(current_saved != NULL && current_saved -> token[0] == 'w'){
				temp = "<write>";
				push(temp);
				push(current_saved -> id);
				temp = "<write>";
				push(temp);
				current_saved = current_saved -> next;
				state = 3;
				ret = recursionXML(state);
				cout << ")";
				return ret;
			}
			else{
				pop();
				cout << " -> BACKWARDS)";
				return 1;
			}
			break;
		case 3:
			cout << " -> (state " << state;
			temp = "<expr>";
			push(temp);
			state = 5;
			ret = recursionXML(state);
			if(ret == 0){
				state = 4;
				ret = recursionXML(state);
				cout << ")";
				return ret;
			}
			else{
				pop();
				cout << " -> BACKWARDS)";
				return 1;
			}
			break;
		case 4:
			cout << " -> (state " << state;
			temp = "<term_tail>";
			push(temp);
			state = 8;
			ret = recursionXML(state);
			if(ret == 0){
				state = 5;
				ret = recursionXML(state);
				if(ret == 0){
					state = 4;
					ret = recursionXML(state);
					cout << ")";
					return ret;
				}
				else{
					pop();
					cout << " -> BACKWARDS)";
					return 1;
				}
			}
			else{
				pop();
				cout << " -> E)";
				temp = "</term_tail>";
				push(temp);
				return 0;
			}
			break;
		case 5:
			cout << " -> (state " << state;
			temp = "<term>";
			push(temp);
			state = 7;
			ret = recursionXML(state);
			if(ret == 0){
				state = 6;
				ret = recursionXML(state);
				cout << ")";
				return ret;
			}
			else{
				pop();
				cout << " -> BACKWARDS)";
				return 1;
			}
			break;
		case 6:
			cout << " -> (state " << state;
			temp = "<fact_tail>";
			push(temp);
			state = 9;
			ret = recursionXML(state);
			if(ret == 0){
				state = 7;
				ret = recursionXML(state);
				if(ret == 0){
					state = 6;
					ret = recursionXML(state);
					cout << ")";
					return ret;
				}
				else{
					pop();
					cout << " -> BACKWARDS)";
					return 1;
				}
			}
			else{
				pop();
				cout << " -> E)";
				temp = "</fact_tail>";
				push(temp);
				return 0;
			}
			break;
		case 7:
			cout << " -> (state " << state;
			temp = "<factor>";
			push(temp);
			if(current_saved != NULL && current_saved -> token[0] == 'l'){
				temp = "<lparen>";
				push(temp);
				push(current_saved -> id);
				temp = "</lparen>";
				push(temp);
				current_saved = current_saved -> next;
				state = 3;
				ret = recursionXML(state);
				if(ret == 0 && current_saved != NULL && current_saved -> token[0] == 'r' && current_saved -> token[1] == 'p'){
					temp = "<rparen>";
					push(temp);
					push(current_saved -> id);
					temp = "</rparen>";
					push(temp);
					current_saved = current_saved -> next;
					temp = "</factor>";
					push(temp);
					return 0;
				}
				else{
					pop();
					cout << " -> BACKWARDS)";
					return 1;
				}
			}
			else if(current_saved != NULL && current_saved -> token[0] == 'i'){
				temp = "<id>";
				push(temp);
				push(current_saved -> id);
				temp = "</id>";
				push(temp);
				current_saved = current_saved -> next;
				temp = "</factor>";
				push(temp);
				return 0;
			}
			else if(current_saved != NULL && current_saved -> token[0] == 'n'){
				temp = "<number>";
				push(temp);
				push(current_saved -> id);
				temp = "</number>";
				push(temp);
				current_saved = current_saved -> next;
				temp = "</factor>";
				push(temp);
				return 0;
			}
			else{
				pop();
				cout << " -> BACKWARDS)";
				return 1;
			}
			break;
		case 8:
			cout << " -> (state " << state;
			temp = "<add_op>";
			push(temp);
			if(current_saved != NULL && current_saved -> token[0] == 'p'){
				temp = "<plus>";
				push(temp);
				push(current_saved -> id);
				temp = "</plus>";
				push(temp);
				current_saved = current_saved -> next;
				temp = "</add_op>";
				push(temp);
				return 0;
			}
			else if(current_saved != NULL && current_saved -> token[0] == 'm'){
				temp = "<minus>";
				push(temp);
				push(current_saved -> id);
				temp = "</minus>";
				push(temp);
				current_saved = current_saved -> next;
				temp = "</add_op>";
				push(temp);
				return 0;
			}
			else{
				pop();
				cout << " -> BACKWARDS)";
				return 1;
			}
			break;
		case 9:
			cout << " -> (state " << state;
			temp = "<mult_op>";
			push(temp);
			if(current_saved != NULL && current_saved -> token[0] == 't'){
				temp = "<times>";
				push(temp);
				push(current_saved -> id);
				temp = "</times>";
				push(temp);
				current_saved = current_saved -> next;
				temp = "</mult_op>";
				push(temp);
				return 0;
			}
			else if(current_saved != NULL && current_saved -> token[0] == 'd'){
				temp = "<div>";
				push(temp);
				push(current_saved -> id);
				temp = "</div>";
				push(temp);
				current_saved = current_saved -> next;
				temp = "</mult_op>";
				push(temp);
				return 0;
			}
			else{
				pop();
				cout << " -> BACKWARDS)";
				return 1;
			}
			break;
	}
}

void Scan(char *file){
	char str[MAX_CHAR];
	FILE *fp;
	fp = fopen(file, "r");
	
	while(fgets(str, MAX_CHAR, fp) != NULL){
		for(int i = 0; i < strlen(str); i++){
			//cout << str[i] << ": " << (int)str[i] << endl;
			if(str[i+1] != '\0')
				Interpreter(str[i], str[i+1]);
			else
				Interpreter(str[i], -1);
		}
	}
}

void Interpreter(char character, char next){
	/*
	cout << current_tree -> state << ": " << endl;
	cout << "Current: " << character << ": " << (int)character << endl;
	cout << "Next: " << next << ": " << (int)next << endl;\
	cout << endl;
	*/
	
	char read[10] = {"read"};
	char write[10] = {"write"};
	
	
	int current_ascii = (int)character;
	int next_ascii = (int)next;
	if(current_tree -> state == 1){
		counter = 0;
		for(int j = 0; j < 6; j++){
			r_w[j] = -1;
		}
		if(current_ascii == 9 || current_ascii == 10 || current_ascii == 32)
			current_tree = start_tree;
		else if(current_ascii == 47){
			current_tree = current_tree -> child;
			while(current_tree -> state != 2){
				current_tree = current_tree -> sibling;
			}
			if(next_ascii != 47 && next_ascii != 42){
				Create_token(current_tree -> name);
				//cout << "Created a 'div' Token" <<endl;
				current_tree = start_tree;
			}
		}
		else if(current_ascii == 40){
			current_tree = current_tree -> child;
			while(current_tree -> state != 6){
				current_tree = current_tree -> sibling;
			}
			Create_token(current_tree -> name);
			//cout << "Created a 'lparen' Token" <<endl;
			current_tree = start_tree;
		}
		else if(current_ascii == 41){
			current_tree = current_tree -> child;
			while(current_tree -> state != 7){
				current_tree = current_tree -> sibling;
			}
			Create_token(current_tree -> name);
			//cout << "Created a 'rparen' Token" <<endl;
			current_tree = start_tree;
		}
		else if(current_ascii == 43){
			current_tree = current_tree -> child;
			while(current_tree -> state != 8){
				current_tree = current_tree -> sibling;
			}
			Create_token(current_tree -> name);
			//cout << "Created a 'plus' Token" <<endl;
			current_tree = start_tree;
		}
		else if(current_ascii == 45){
			current_tree = current_tree -> child;
			while(current_tree -> state != 9){
				current_tree = current_tree -> sibling;
			}
			Create_token(current_tree -> name);
			//cout << "Created a 'minus' Token" <<endl;
			current_tree = start_tree;
		}
		else if(current_ascii == 42){
			current_tree = current_tree -> child;
			while(current_tree -> state != 10){
				current_tree = current_tree -> sibling;
			}
			Create_token(current_tree -> name);
			//cout << "Created a 'times' Token" <<endl;
			current_tree = start_tree;
		}
		else if(current_ascii == 58){
			current_tree = current_tree -> child;
			while(current_tree -> state != 11){
				current_tree = current_tree -> sibling;
			}
		}
		else if(current_ascii == 46){
			current_tree = current_tree -> child;
			while(current_tree -> state != 13){
				current_tree = current_tree -> sibling;
			}
		}
		else if(current_ascii >= 48 && current_ascii <= 57){
			name[index] = current_ascii;
			index++;
			current_tree = current_tree -> child;
			while(current_tree -> state != 14){
				current_tree = current_tree -> sibling;
			}
			if(next_ascii != 46 && (next_ascii > 57 || next_ascii < 48)){
				Create_token(current_tree -> name);
				//cout << "Created a 'number' Token" <<endl;
				current_tree = start_tree;
			}
		}
		else if((current_ascii >= 65 && current_ascii <= 90) || (current_ascii >= 97 && current_ascii <= 122)){
			name[index] = current_ascii;
			index++;
			current_tree = current_tree -> child;
			while(current_tree -> state != 16){
				current_tree = current_tree -> sibling;
			}
			r_w[counter] = current_ascii;
			if((next_ascii < 65 || next_ascii > 90) && (next_ascii < 97 || next_ascii > 122) && (next_ascii < 48 || next_ascii > 57)){
				Create_token(current_tree -> name);
				//cout << "Created a 'id' Token" <<endl;
				current_tree = start_tree;
			}
		}
		else{
			cout << "ERROR" << endl;
			exit(2);
		}
	}
	else if(current_tree -> state == 2){
		if(current_ascii == 47)
			current_tree = current_tree -> child;
		else
			current_tree = current_tree -> child -> sibling;
	}
	else if(current_tree -> state == 3){
		if(current_ascii == 10)
			current_tree = start_tree;
	}
	else if(current_tree -> state == 4){
		if(current_ascii == 42)
			current_tree = current_tree -> child;
	}
	else if(current_tree -> state == 5){
		if(current_ascii == 47)
			current_tree = start_tree;
		else if(current_ascii != 42)
			current_tree = current_tree -> child -> sibling;
	}
	else if(current_tree -> state == 11){
		if(current_ascii == 61){
			current_tree = current_tree -> child;
			Create_token(current_tree -> name);
			//cout << "Created a 'assign' Token" <<endl;
			current_tree = start_tree;
		}
		else{
			cout << "PARSE ERROR" << endl;
			exit(2);
		}	
	}
	else if(current_tree -> state == 13){
		if(current_ascii >= 48 && current_ascii <= 57)
			current_tree = current_tree -> child;
		else{
			cout << "PARSE ERROR" << endl;
			exit(2);
		}
	}
	else if(current_tree -> state == 14){
		if((current_ascii >= 48 && current_ascii <= 57) && ((next_ascii < 48 || next_ascii > 57) && next_ascii != 46)){
			Create_token(current_tree -> name);
			//cout << "Created a 'number' Token" <<endl;
			current_tree = start_tree;
		}
		else if(current_ascii == 46){
			name[index] = current_ascii;
			index++;
			current_tree = current_tree -> child;
		}
		
	}
	else if(current_tree -> state == 15){
		if(next_ascii < 57 || next_ascii > 48){
			Create_token(current_tree -> name);
			//cout << "Created a 'number' Token" <<endl;
		}
		else{
			name[index] = current_ascii;
			index++;
		}
	}
	else if(current_tree -> state == 16){	
		if((next_ascii < 65 || next_ascii > 90) && (next_ascii < 97 || next_ascii > 122) && (next_ascii < 48 || next_ascii > 57)){
			
			if(counter < 6)
				r_w[++counter] = current_ascii;
			if(r_w[0] ==  114 && r_w[1] == 101 && r_w[2] == 97 && r_w[3] == 100 && r_w[4] == -1){
				name[index] = current_ascii;
				index++;
				Create_token(read);
				//cout << "Created a 'read' Token" <<endl;
				current_tree = start_tree;
			}
			else if(r_w[0] ==  119 && r_w[1] == 114 && r_w[2] == 105 && r_w[3] == 116 && r_w[4] == 101 && r_w[5] == -1){
				name[index] = current_ascii;
				index++;
				Create_token(write);
				//cout << "Created a 'write' Token" <<endl;
				current_tree = start_tree;
			}
			else{
				Create_token(current_tree -> name);
				//cout << "Created a 'id' Token" <<endl;
				current_tree = start_tree;
			}
		}
		else{
			name[index] = current_ascii;
			index++;
			if(counter < 6){
				r_w[++counter] = current_ascii;
			}
		}
	}
}


void Tree_Create(int state, char character[10], int terminator){ //We had to add this new function from our original report as we needed another function to help construct the DFA Transition Table
	tree_Node *temp1 = (tree_Node*)malloc(sizeof(tree_Node));	
	for(int i = 0; i < 10; i++)
		temp1 -> name[i] = character[i];
	temp1 -> state = state;
	temp1 -> terminator = terminator;
	temp1 -> child = NULL;
	temp1 -> sibling = NULL;
	if(start_tree == NULL){
		start_tree = temp1;
	}
	current_tree = temp1;
}

void Tree_Process(){ //Another file created post submission of the report as we needed to design the operation order of our previously created DFA tree.
	char arr_char[10][10] = {"", "div", "lparen", "rparen", "plus", "minus", "times", "assign", "number", "id"};
	
	Tree_Create(1, arr_char[0], 0);
	tmp = current_tree;
	Tree_Create(2, arr_char[1], 1);
	tmp -> child = current_tree;
	tmp = current_tree;
	Tree_Create(3, arr_char[0], 0);
	tmp -> child = current_tree;
	Tree_Create(4, arr_char[0], 0);
	tmp -> child -> sibling = current_tree;
	tmp = current_tree;
	Tree_Create(5, arr_char[0], 0);
	tmp -> child = current_tree;
	current_tree -> child = start_tree;
	tmp = current_tree;
	Tree_Create(6, arr_char[2], 1);
	start_tree -> child -> sibling = current_tree;
	Tree_Create(7, arr_char[3], 1);
	start_tree -> child -> sibling -> sibling = current_tree;
	Tree_Create(8, arr_char[4], 1);
	start_tree -> child -> sibling -> sibling -> sibling = current_tree;
	Tree_Create(9, arr_char[5], 1);
	start_tree -> child -> sibling -> sibling -> sibling -> sibling = current_tree;
	Tree_Create(10, arr_char[6], 1);
	start_tree -> child -> sibling -> sibling -> sibling -> sibling -> sibling = current_tree;
	Tree_Create(11, arr_char[0], 0);
	start_tree -> child -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling = current_tree;
	tmp = current_tree;
	Tree_Create(12, arr_char[7], 1);
	tmp -> child = current_tree;
	Tree_Create(13, arr_char[0], 0);
	start_tree -> child -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling = current_tree;
	tmp = current_tree;
	Tree_Create(14, arr_char[8], 1);
	start_tree -> child -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling = current_tree;
	Tree_Create(15, arr_char[8], 1);
	tmp -> child = current_tree;
	start_tree -> child -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling -> child = current_tree;
	Tree_Create(16, arr_char[9], 1);
	start_tree -> child -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling -> sibling = current_tree;

	start_tree -> child -> child -> sibling -> child -> child -> sibling = start_tree -> child -> child -> sibling;
	
}

void Create_token(char token_type[10]){ //This is the last file that we had to create post report and it was designed to help give the tokens a better flow throughout the DFA tree.
	saved_token *temp1 = (saved_token*)malloc(sizeof(saved_token));
	for(int i = 0; i < 10; i++){
		temp1 -> token[i] = token_type[i];
	}
	for(int j = 0; j < 99; j++){
		temp1 -> id[j] = name[j];
		if(name[j] == 0){
			break;
		}
	}
	if(temp1 -> id[0] == 0){
		for(int k = 0; k < 10; k++){
			temp1 -> id[k] = temp1 -> token[k];
		}
	}
	temp1 -> next = NULL;
	if(start_saved == NULL){
		start_saved = temp1;
		current_saved = temp1;
	}
	else{
		current_saved -> next = temp1;
		current_saved = temp1;
	}
	index = 0;
	cout << temp1 -> id << endl;
	for(int j = 0; j < 99; j++){
		name[j] = 0;
	}
}

void push(char id[99]){
	queue *temp1 = (queue*)malloc(sizeof(queue));
	temp1 -> print = id;
	if(front == NULL){
		front = temp1;
	}
	else{
		back -> next = temp1;
		tempQueue = back;
	}
	back = temp1;
	temp1 -> next = NULL;
	cout << "[Next -> " << back -> print << "]";
}

void pop(){
	back = tempQueue;
	//back = back -> next;
	cout << "[Next -> " << back -> print << "]";
}