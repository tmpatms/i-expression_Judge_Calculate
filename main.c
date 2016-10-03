
//
//  main.c
//  Homework1
//
//  Created by tmpatms on 28/9/16.
//  Copyright © 2016年 tmpatms. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 A struct that can store tokens.
 */
typedef struct {
    char tokens[1000][1000];
    int count;
    int iexpression;
    int value;
}Token;

Token* initToken(){
    Token* token;
    token = (Token*)malloc(sizeof(Token));
    
    if ( !token ){
        printf("Not enough space.");
        return NULL;
    }
    token->count = 0;
    token->iexpression = 1;
    return token;
}

/*
 A stack struct.
 */
typedef struct{
    char data[1000][1000];
    int top;
}Stack;

/*
 A queue struct.
 */
typedef struct{
    char data[1000][1000];
    int size;
}Queue;

Queue* initQueue(){
    Queue* queue;
    queue = (Queue*)malloc(sizeof(Queue));
    if ( !queue ){
        printf("Not enough space.");
        return NULL;
    }
    queue->size = -1;
    return queue;
}

int qIsFull(Queue* queue){
    if ( queue->size == 999 ){
        return 1;
    }
    return 0;
}

int qIsEmpty(Queue* queue){
    if ( queue->size == -1 ){
        return 1;
    }
    return 0;
}

void qPush(Queue* queue, char str[1000]){
    if ( qIsFull(queue) ){
        return;
    }
    queue->size++;
    for (int i = 0; i < strlen(str); i++ ) {
        queue->data[queue->size][i] = str[i];
    }
}

char* qOut(Queue* queue) {
    if ( qIsEmpty(queue) ){
        return "\0";
    }
    char str[1000];
    strcpy(str, queue->data[0]);
    char *temp = str;
    for ( int i = 0; i < queue->size; i++ ){
        strcpy(queue->data[i], queue->data[i + 1]);
    }
    return temp;
}

Stack* initStack(){
    Stack* stack;
    stack = (Stack*)malloc(sizeof(Stack));
    if ( !stack ){
        printf("Not enough space.");
        return NULL;
    }
    stack->top = -1;
    return stack;
}

int sIsFull(Stack* stack){
    if ( stack-> top == 999 ){
        //printf("Stack is full");
        return 1;
    }
    return 0;
}

int sIsEmpty(Stack* stack){
    if ( stack->top == -1 ){
        return 1;
    }
    return 0;
}


void sPush(Stack* stack, char str[1000]){
    if ( sIsFull(stack) ){
        return;
    }
    stack->top++;
    for ( int i = 0; i < strlen(str); i++ ){
        stack->data[stack->top][i] = str[i];
    }
}

char* sPop(Stack* stack){
    if ( sIsEmpty(stack) ){
        return "\0";
    }
    return stack->data[stack->top--];
}

int precedence(char c){
    if ( c == '+' || c == '-' ) return 1;
    return 2;
}

void getTokens(char str[1000], Token *token);

void lexical(Token *token);

void syntactic(Token *token);

void evaluateIExpression(Token *token);

int main(int argc, const char * argv[]) {
    // insert code here...
    char s[1000];
    
    printf("Please input a string:");
    
    while( gets(s) ){
        
        if ( strcmp(s, "bye") == 0 ) break;//If enter "bye", then quit the program.
        
        Token* token;
        token = initToken();
        
        //判断字符串中是否出现特殊空白，如果有，则替换成普通空格。
        for ( int i = 0; i < strlen(s); i++ ){
            if ( isspace(s[i]) ){
                s[i] = ' ';
            }
        }
        
        getTokens(s, token);//Get tokens from the input string.
        
        for ( int i = 0; i < token->count; i++ ){
            printf("%d: %s\n", i + 1, token->tokens[i]);//print out tokens from the string.
        }
        
        lexical(token);//An examination for lexical error.
        syntactic(token);//An examination for syntactic error.
        
        //If the string is an i-expression, then evalue it and print the output of the i-expression.
        if ( token->iexpression == 1 ){
            printf("The result of the i-expression should be: ");
            evaluateIExpression(token);
            printf("%d\n", token->value);
        }else {
            printf("%s is not an i-expression, I can not evaluate it.\n", s);
        }
        
        strcpy(s, "\0");
        
        printf("Please input next string, or enter bye for quit this program:");
        
        //memset(&token,0,sizeof(token));
    }
    return 0;
}

/*
 A function that can get tokens from the string entered by the user.
 str: char[1000], the string input by the user.
 *token: Token, the struct that can store information.
 */
void getTokens(char str[1000], Token *token){
    
    token->iexpression = 1;//Initial
    token->count = 0;//Initial
    
    int target = 0, lastTok = 0;
    
    /*
     处理这个字符串。 我们对其一个字符一个字符地检索，如果遇到除了'(', ')', '+', '-', '*', '/', ',', '.'， 字符，则分割字符串。
     分割通过计数器来控制分割的长度以及分割的起始角标，长度参数为target， 起始角标参数为lastTok。
     关于lastTok: 每次分割后把target + 1 + lastTok的值传递给lastTok, 这样保证lastTok能确保在每次进行分割的角标的下一个字符角标上。
     关于target: 每次不遇到'(', ')', '+', '-', '*', '/', ',', '.'， 字符，target++。每次分割成功后，target初始化为0。
     每次分割出来的字字符串把它传递到token的tokens数组中， 并且是一个字符一个字符地传递。
     */
    for ( int i = 0; i < strlen(str); i++ ){
        if ( (str[i] > 39 && str[i] < 48 ) || str[i] == 33 || str[i] == ' ' ){
            if ( str[i] == ' ' ){
                if ( i == 0 ){
                    str++;
                    i--;
                }else {
                    for ( int j = 0; j < target; j++ ){
                        token->tokens[token->count][j] = str[lastTok + j];
                    }
                    if ( target != 0 ) token->count++;
                    lastTok += (target + 1);
                    target = 0;
                }
            }else {
                if ( i == 0 ){
                    token->tokens[token->count++][0] = str[i];
                    str++;
                    i--;
                }else {
                    for ( int j = 0; j < target; j++ ){
                        token->tokens[token->count][j] = str[lastTok + j];
                    }
                    if ( target != 0 ) token->count++;
                    lastTok += (target + 1);
                    target = 0;
                    token->tokens[token->count++][0] = str[i];
                }
            }
        }else {
            target++;
        }
    }
    if ( !((str[strlen(str)] > 39 && str[strlen(str)] < 48 ) || str[strlen(str)] == 33 || str[strlen(str)] == ' ') ){
        for ( int i = 0; i < target; i++ ){
            token->tokens[token->count][i] = str[lastTok + i];
        }
        if ( target != 0 ) token->count++;
    }
}
/*
 A function that can test if the string has lexical error.
 If there are any character besides +, -, *, /, 0~9, then the string has lexical error.
 */
void lexical(Token *token){
    
    for ( int i = 0; i < token->count; i++ ){
        for ( int j = 0; j < strlen(token->tokens[i]); j++ ){
            if ( (token->tokens[i][0] > 39 && token->tokens[i][0] < 44) || (token->tokens[i][0] == 45) || (token->tokens[i][0] == 47) ) break;
            if ( (token->tokens[i][j] >= '0' && token->tokens[i][j] <= '9') || token->tokens[i][0] == '~' ){
                continue;
            }else {
                printf("%c should not appear in '%s', Lexical error.\n", token->tokens[i][j], token->tokens[i]);
                token->iexpression = 0;
            }
        }
    }
}
/*
 A function that can test if the string has syntactic error.
 */
void syntactic(Token *token){
    
    int numOfLeftParethesis = 0;
    int numOfRightParethesis = 0;
    
    for ( int i = 0; i < token->count; i++ ){
        if ( token->tokens[i][0] == '(' ) numOfLeftParethesis++;
        if ( token->tokens[i][0] == ')' ) numOfRightParethesis++;
    }
    
    if ( numOfLeftParethesis > numOfRightParethesis ){
        printf("Lack of ) in the expression, big mistake. Syntactic error.\n");
        token->iexpression = 0;
    }else if ( numOfLeftParethesis < numOfRightParethesis ){
        printf("Lack of ( in the expression, big mistake. Syntactic error.\n");
        token->iexpression = 0;
    }else {
        int indexOfRightParethesis = token->count - 1;
        for ( int i = 0; i < token->count; i++ ){
            if ( token->tokens[i][0] == '(' ){
                for ( int j = indexOfRightParethesis; j > i; j-- ){
                    if ( token->tokens[j][0] == ')' ){
                        indexOfRightParethesis = j;
                        if ( j - i == 1 ){
                            printf("Nothing between ( ). Syntactic error\n");
                            token->iexpression = 0;
                        }
                    }
                }
            }
        }
    }
    
    for ( int i = 0; i < token->count; i++ ){
        if ( token->tokens[i][0] == '+' || token->tokens[i][0] == '-' || token->tokens[i][0] == '*' || token->tokens[i][0] == '/' ){
            if ( token->tokens[i - 1][0] == '(' ){
                printf("The left parethesis should not appear in the left of the %c/ Syntactic error.\n", token->tokens[i][0]);
                token->iexpression = 0;
            }else if ( token->tokens[i + 1][0] == ')' ){
                printf("The right parethesis should not appear in the right of the %c/ Syntactic error.\n", token->tokens[i][0]);
                token->iexpression = 0;
            }else if ( token->tokens[i - 1][0] == '+' || token->tokens[i - 1][0] == '-' || token->tokens[i - 1][0] == '*' || token->tokens[i - 1][0] == '/'){
                printf("The %c should not appear in the left of the %c. Syntactic error.\n", token->tokens[i - 1][0], token->tokens[i][0]);
                token->iexpression = 0;
            }else if ( token->tokens[i + 1][0] == '+' || token->tokens[i + 1][0] == '-' || token->tokens[i + 1][0] == '*' || token->tokens[i + 1][0] == '/'){
                printf("The %c should not appear in the right of the %c. Syntactic error.\n", token->tokens[i + 1][0], token->tokens[i][0]);
                token->iexpression = 0;
            }else if ( token->tokens[i - 1][0] == '\0' ){
                printf("The left operand of %c is missing. Syntactic error.\n", token->tokens[i][0]);
                token->iexpression = 0;
            }else if ( token->tokens[i + 1][0] == '\0' ){
                printf("The right operand of %c is missing. Syntactic error.\n", token->tokens[i][0]);
                token->iexpression = 0;
            }
        }
    }
}
/*
 A function that can evaluate the i-expression.
 
 */
void evaluateIExpression(Token *token){
    
    Stack* stack;
    stack = initStack();
    
    Queue* queue;
    queue = initQueue();
    
    for ( int i = 0; i < token->count; i++ ){
        char str[1000];
        char temp[1000];
        strcpy(str, token->tokens[i]);
        if ( (str[0] >= '0' && str[0] <= '9') || str[0] == '~' ){
            qPush(queue, str);
            continue;
        }
        if ( str[0] == '(' ) sPush(stack, str);
        else if ( str[0] == ')' ){
            while ( !sIsEmpty(stack) ){
                strcpy(temp, sPop(stack));
                if ( temp[0] != '(' ) qPush(queue, temp);
                else break;
            }
        }else {
            if ( sIsEmpty(stack) || stack->data[stack->top][0] == '(' || precedence(stack->data[stack->top][0]) < precedence(str[0]) ){
                sPush(stack, str);
            }else{
                strcpy(temp, sPop(stack));
                qPush(queue, temp);
                sPush(stack, str);
            }
        }
    }
    
    int stackSize = stack->top;
    
    for ( int i = 0; i <= stackSize; i++ ){
        qPush(queue, sPop(stack));
    }
    
    int queueSize = queue->size;
    
    /*
     以下for循环会计算i-expression的值，每遇到一个计算符号，就会用这个符号前面的两个数字进行这个符号的计算，计算结束后，
     计算结果会代替前面两个被计算过的数字。
     如果数字前面出现了~符号，则这个数字是一个负数。
     */
    for ( int i = 0; i <= queueSize; i++ ){
        if ( (queue->data[i][0] >= '0' && queue->data[i][0] <= '9') || queue->data[i][0] == '~' ) continue;
        else if ( queue->data[i][0] == '*' || queue->data[i][0] == '/' || queue->data[i][0] == '+' || queue->data[i][0] == '-'){
            int index1 = 0, index2 = 0;
            int count = 0;
            for ( int j = i - 1; j >= 0; j-- ){
                if ( (queue->data[j][0] >= '0' && queue->data[j][0] <= '9') || queue->data[j][0] == '~' ){
                    count++;
                    if ( count == 1 ) index2 = j;
                    if ( count == 2 ) index1 = j;
                }
                if ( count == 2 ){
                    int a, b;
                    if ( queue->data[index1][0] == '~' ) {
                        char tempNumber[100];
                        for ( int number = 1; number < strlen(queue->data[index1]); number++ ){
                            tempNumber[number - 1] = queue->data[index1][number];
                        }
                        a = -atoi(tempNumber);
                    }
                    else {
                        a = atoi(queue->data[index1]);
                    }
                    if ( queue->data[index2][0] == '~' ){
                        char tempNumber[100];
                        for ( int number = 1; number < strlen(queue->data[index2]); number++ ){
                            tempNumber[number - 1] = queue->data[index2][number];
                        }
                        b = -atoi(tempNumber);
                    }else {
                        b = atoi(queue->data[index2]);
                    }
                    if ( queue->data[i][0] == '*' ){
                        sprintf(queue->data[index1], "%d", a * b);
                    }else if ( queue->data[i][0] == '/' ){
                        sprintf(queue->data[index1], "%d", a / b);
                    }else if ( queue->data[i][0] == '+' ){
                        sprintf(queue->data[index1], "%d", a + b);
                    }else if ( queue->data[i][0] == '-' ){
                        sprintf(queue->data[index1], "%d", a - b);
                    }
                    queue->data[index2][0] = ' ';
                    break;
                }
            }
        }
    }
    
    int index = 0;
    for ( int i = 0; i < queueSize; i++ ){
        if ( queue->data[i][0] >= '0' && queue->data[i][0] <= '9' ){
            index = i;
            break;
        }
    }
    token->value = atoi(queue->data[index]);
}
