#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

char    type_list[7][15] = {"Keyword", "Id", "IntLieteral", "FloatLiteral", "Operator", "Separator", "Comment"};

typedef struct  Token{
	int     t_type;
	char    t_value[1000];
}               Token;

void    pass_space(char *str, int *idx)
{
	while (isspace(str[*idx]))
		(*idx)++;
}

bool    is_comment(char *line) { // true, false 리턴
	if (line[0] == '/' && line[1] == '/')
		return (true);
	return (false);
}

bool    is_operator(char *str) {
	if (strncmp("=", str, 1) == 0 || strncmp("<", str, 1) == 0 || strncmp("+", str, 1) == 0
		|| strncmp("-", str, 1) == 0 || strncmp("*", str, 1) == 0
		|| strncmp("!", str, 1) == 0 || strncmp("/", str, 1) == 0) {
		return (true);
	}
	if (strncmp("||", str, 2) == 0 || strncmp("&&", str, 2) == 0 || strncmp("==", str, 2) == 0
		|| strncmp("!=", str, 2) == 0 || strncmp(">=", str, 2) == 0) {
		return (true);
	}
	return (false);
}

bool    is_separator(char c) {
	if (c == ':' || c == '.' || c == '{' || c == '}' || c == '(' || c == ')' || c == ';')
		return (true);
	return (false);
}

bool    is_intlit(char *str) {
	int i = 0;
	
	if (isdigit(str[i]))
	{
		while (isdigit(str[i]))
			i++;
		if (str[i] == '\0' || isspace(str[i]) || is_separator(str[i]) || is_operator(str + i))
			return (true);
	}
	return (false);
}

bool    is_floatlit(char *str) {
	int i = 0;
	int num_flag = 0;
	int point_flag = 0;
	
	while (isdigit(str[i]) && str[i])
	{
		num_flag = 1;
		i++;
	}
	if (str[i] == '.' && num_flag == 1)
		point_flag = 1;
	else
		return (false);
	while (isdigit(str[i]) && str[i])
		i++;
	if (str[i] != '\0' && str[i] != ' ' && !is_separator(str[i]))
		return (false);
	return (true);
}

bool    handle_id(char *line, Token *t, int *index, int *err_flag) {
	int i = 0;
	int flag = 0;

	if (!isalnum(line[i]))
		return (false);
	if (isdigit(line[i]))
		return (false);
	//if (isdigit(line[i])) // 숫자로 시작하는 id에 대해 예외처리 (id는 숫자와 알파벳으로만 구성되있는데, 시작은 반드시 알파벳으로 시작해야함)
	//{
	//	while (line[i])
	//	{
	//		if (line[i] == ' ' || line[i] == ',' || is_separator(line[i]) || is_operator(line + i))
	//			break;
	//		i++;
	//	}
	//	//printf("[%c] %s\n", line[i], line);
	//	*err_flag = 1;
	//	(*index) += i;
	//	return (false);
	//}
	while (line[i])
	{
		if (line[i] == ' ' || line[i] == ',')
			break;
		if (is_separator(line[i + 1]) || is_operator(line + 1 + i))
		{
			i++;
			break;
		}
		if (!isalnum(line[i])) // id에 숫자나 알파벳이 아닌 문자가 있을 경우에 대한 예외처리.
		{
			//printf("[%c] %s\n", line[i], line);
			*err_flag = 1;
			(*index) += i;
			return (false);
		}
		i++;
	}
	(*index) += i;
	t->t_type = 1;
	strncpy(t->t_value, line, i);
	(*index)--;
	return (true);
}

bool    handle_literal(char *line, Token *t, int *index) {
	int i = 0;
	int point_flag = 0;
	
	//pass_space(line, index);
	//printf("[%c] %s\n", line[i], line);
	if (is_intlit(line) && !is_floatlit(line))
	{
		while (isdigit(line[i])) {
			i++;
		}
		t->t_type = 2;
		(*index) += i;
		strncpy(t->t_value, line, i);
		return (true);
	}
	else if (is_floatlit(line)) {
		while (isdigit(line[i]) || line[i] == '.') {
			i++;
		}
		t->t_type = 3;
		(*index) += i;
		strncpy(t->t_value, line, i);
		return (true);
	}
	return (false);
}

bool    handle_seperator(char *str, Token *t, int *index) {
	if (!is_separator(str[0]))
		return (false);
	//(*index)++;
	t->t_type = 5;
	strncpy(t->t_value, str, 1);
	return (true);
}

bool    handle_operator(char *str, Token *t, int *index) {
	bool    ret = false;
	
	if (strncmp("=", str, 1) == 0 || strncmp("<", str, 1) == 0 || strncmp("+", str, 1) == 0
		|| strncmp("-", str, 1) == 0 || strncmp("*", str, 1) == 0
		|| strncmp("!", str, 1) == 0 || strncmp("/", str, 1) == 0) {
		t->t_type = 4;
		strncpy(t->t_value, str, 1);
		//(*index)++;
		ret = true;
	}
	else if (strncmp("||", str, 2) == 0 || strncmp("&&", str, 2) == 0  || strncmp("==", str, 2) == 0 
		|| strncmp("!=", str, 2) == 0 || strncmp(">=", str, 2) == 0) {
		t->t_type = 4;
		strncpy(t->t_value, str, 2);
		//(*index) += 2;
		(*index) ++;
		ret = true;
	}
	return (ret);
}

bool    handle_keyword(char *line, Token *t, int *index) {
	bool    ret = false;
	
	if (strncmp("bool", line, 4) == 0 && line[4] == ' ') {
		t->t_type = 0;
		strncpy(t->t_value, "Bool", 4);
		(*index) += 4;
		ret = true;
	}
	else if (strncmp("int", line, 3) == 0 && line[3] == ' ') {
		t->t_type = 0;
		strncpy(t->t_value, "Int", 3);
		(*index) += 3;
		ret = true;
	}
	else if (strncmp("char", line, 4) == 0 && line[4] == ' ') {
		t->t_type = 0;
		strncpy(t->t_value, "Char", 4);
		(*index) += 4;
		ret = true;
	}
	else if (strncmp("float", line, 5) == 0 && line[5] == ' ') {
		t->t_type = 0;
		strncpy(t->t_value, "Float", 5);
		(*index) += 5;
		ret = true;
	}
	else if (strncmp("if", line, 2) == 0 && !(isalnum(line[2])) && !(is_separator(line[2]))) {
		t->t_type = 0;
		strncpy(t->t_value, "If", 2);
		(*index) += 2;
		ret = true;
	}
	else if (strncmp("else", line, 4) == 0 && !(isalnum(line[4])) && !(is_separator(line[4]))) {
		t->t_type = 0;
		strncpy(t->t_value, "Else", 4);
		(*index) += 4;
		ret = true;
	}
	else if (strncmp("true", line, 4) == 0 && !(isalnum(line[4])) && !(is_separator(line[4]))) {
		t->t_type = 0;
		strncpy(t->t_value, "True", 4);
		(*index) += 4;
		ret = true;
	}
	else if (strncmp("false", line, 5) == 0 && !(isalnum(line[5])) && !(is_separator(line[5]))) {
		t->t_type = 0;
		strncpy(t->t_value, "False", 5);
		(*index) += 5;
		ret = true;
	}
	else if (strncmp("while", line, 5) == 0 && !(isalnum(line[5])) && !(is_separator(line[5]))) {
		t->t_type = 0;
		strncpy(t->t_value, "While", 5);
		(*index) += 5;
		ret = true;
	}
	return (ret);
}

bool    handle_comment(char *line, Token *t)
{
	if (is_comment(line)) { // 한 줄 주석일 경우
		t->t_type = 6;
		strncpy(t->t_value, line, strlen(line));
		return (true);
	}
	return (false);
}

void    print_Token(Token t)
{
	if (t.t_type == 1 || t.t_type == 2 || t.t_type == 3)
		printf("%s %s\n", type_list[t.t_type], t.t_value);
	else
		printf("%s\n", t.t_value);
}

Token   get_Token(FILE *fp, char **nline, int *nl_flag) { // 1줄씩 읽어서 그 안에 있는 토큰을 한번에 읽어 리턴.
	
	static Token    t[1000]; // 1문장씩 읽어 토큰들을 저장해놓음
	static int      request = 0; // 사용자가 get_Tokend을 호출한 횟수 = 받으려는 토큰의 인덱스 번호
	static int      err_flag = 0; // 에러가 한번이라도 있었으면 다음부터 이 함수 호출하지 않게끔 static으로 남김.
	static int      i = 0;
	static int      done = 0;
	int             idx = 0;
	size_t          len = 0;
	ssize_t         read;
	char            *line = NULL;
	// 에러토큰은 타입 번호를 -1로 구분. 종료 타입은 타입 번호를 -2로 구분.
	
	request++; 
	printf("%d\t%d\n", request, i);
	if (i > request - 1 || done == 1) { // 만약 읽어놓은 것이 이미 있으면.
		*nl_flag = 0;
		return (t[request - 1]);
	}
	else { // 요청한 토큰이 없다면 한 문장 더 읽는다.
		if (done == 1)
		{
			Token t;
			t.t_type = -2;
		}
		read = getline(&line, &len, fp);
		if (read == -1 || read == 0) {
			done = 1;// 더이상 읽을 게 없거나 에러가 일어날 경우
		}
		*nl_flag = 1;
		*nline = line;
		if (handle_comment(line, &t[i])) {
			i++;
			return (t[request - 1]);
		}
		if (err_flag == 0) {
			while (idx != read)
			{
				pass_space(line + idx, &idx); // 공백 문자는 무시하고 지나감.
				if (handle_keyword(line + idx, &t[i], &idx)) {
					idx--;
					i++; // 토큰 인덱스
				}
				else if (handle_seperator(line + idx, &t[i], &idx))
					i++;
				else if (handle_operator(line + idx, &t[i], &idx))
					i++;	
				else if (handle_literal(line + idx, &t[i], &idx))
					i++;
				else if (handle_id(line + idx, &t[i], &idx, &err_flag))
					i++;
				if (err_flag == 1) {
					t[i++].t_type = -1;
					break;
				}
				idx++; // 읽어들인 문장 속 문자열 인덱스
			}
		}
	}
	return (t[request - 1]);
}

int     main(int argc, char **argv) {
	FILE    *fp;
	char    *line = NULL;
	int     i = 0;
	int     line_nb = 1;
	int     nl_flag = 0;
	int     error = 0;
	Token   tk;

	fp = fopen("input.txt", "r");
	if (fp == NULL)
		return (0);
	while (1)
	{
		sleep(1);
		memset(&tk, 0, sizeof(Token));
		tk = get_Token(fp, &line, &nl_flag);
		if (tk.t_type == -1) {
			error = 1;
			break;
		}
		if (tk.t_type == -2)
			break;
		if (nl_flag == 1) {
			printf("Line %d %s", line_nb++, line);
			free(line);
		}
		print_Token(tk);
	}
	if (error == 1)
		printf("Error!\n");
	fclose(fp);
	return (0);
}