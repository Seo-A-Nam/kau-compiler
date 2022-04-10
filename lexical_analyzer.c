#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

char    type_list[7][15] = {"Keyword", "Id", "IntLieteral", "FloatLiteral", "Operator", "Separator", "Comment"};

typedef struct  Token{
	int     t_type;
	char    t_value[1000];
}               Token;

bool    is_comment(char *line) { // true, false 리턴
	if (line[0] == '/' && line[1] == '/')
		return (true);
	return (false);
}

bool    is_separator(char c) {
	if (c == ':' || c == '.' || c == '{' || c == '}' || c == '(' || c == ')' || c == ';')
		return (true);
	return (false);
}

bool    is_intlit(char *str) {
	int i = 0;
	
	while (isdigit(str[i]) && str[i])
		i++;
	if (str[i] != '\0' && str[i] != ' ' && !is_separator(str[i]))
		return (false);
	return (true);
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

	if (isdigit(line[i])) // 숫자로 시작하는 id에 대해 예외처리 (id는 숫자와 알파벳으로만 구성되있는데, 시작은 반드시 알파벳으로 시작해야함)
	{
		while (line[i])
		{
			if (line[i] == ' ' || line[i] == ',' || is_separator(line[i]) || is_operator(line + i))
				break;
			i++;
		}
		*err_flag = 1;
		(*index) += i;
		return (false);
	}
	while (line[i])
	{
		if (line[i] == ' ' || line[i] == ',' || is_separator(line[i]) || is_operator(line + i))
			break;
		if (!isalnum(line[i])) // id에 숫자나 알파벳이 아닌 문자가 있을 경우에 대한 예외처리.
		{
			*err_flag = 1;
			(*index) += i;
			return (false);
		}
		i++;
	}
	(*index) += i;
	t->t_type = 1;
	t->t_value = strndup(line, i);
	return (true);
}

bool    handle_literal(char *line, Token *t, int *index) { // lietral (float, int) 형에 대해 test
	int i = 0;
	int point_flag = 0;
	
	if (is_floatlit(line) && is_intlit(line)) {
		while (isdigit(line[i]) || line[i] == '.') {
			if (line[i] == '.')
				point_flag = 1;
			i++;
		}
		(*index) += i;
		t->t_value = strndup(line, i);
		if (point_flag == 1)
			t->t_type = 3;
		else
			t->t_type = 2;
		return (true);
	}
	return (false);
}

bool    handle_seperator(char *str, Token *t, int *index) {
	if (!is_separator(str[0]))
		return (false);
	(*index)++;
	t->t_type = 5;
	t->t_value = strndup(str, 1);
	return (true);
}

bool    handle_operator(char *str, Token *t, int *index) {
	bool    ret = false;
	
	if (strncmp("=", str, 1) == 0 || strncmp("<", str, 1) == 0 || strncmp("+", str, 1) == 0
		|| strncmp("-", str, 1) == 0 || strncmp("*", str, 1) == 0
		|| strncmp("!", str, 1) == 0 || strncmp("/", str, 1) == 0) {
		t->t_type = 4;
		t->t_value = strndup(str, 1);
		(*index)++;
		ret = true;
	}
	else if (strncmp("||", str, 2) == 0 || strncmp("&&", str, 2) == 0  || strncmp("==", str, 2) == 0 
		|| strncmp("!=", str, 2) == 0 || strncmp(">=", str, 2) == 0) {
		t->t_type = 4;
		t->t_value = strndup(str, 2);
		(*index) += 2;
		ret = true;
	}
	return (ret);
}

bool    handle_keyword(char *line, Token *t, int *index) {
	bool    ret = false;
	
	if (strncmp("bool", line, 4) == 0 && line[4] == ' ') {
		t->t_type = 0;
		t->t_value = strndup("Bool", 4);
		(*index) += 4;
		ret = true;
	}
	else if (strncmp("int", line, 3) == 0 && line[3] == ' ') {
		t->t_type = 0;
		t->t_value = strndup("Int", 3);
		(*index) += 3;
		ret = true;
	}
	else if (strncmp("char", line, 4) == 0 && line[4] == ' ') {
		t->t_type = 0;
		t->t_value = strndup("Char", 4);
		(*index) += 4;
		ret = true;
	}
	else if (strncmp("float", line, 5) == 0 && line[5] == ' ') {
		t->t_type = 0;
		t->t_value = strndup("Float", 5);
		(*index) += 5;
		ret = true;
	}
	else if (strncmp("if", line, 2) == 0 && !(isalnum(line[2])) && !(is_separator(line[2]))) {
		t->t_type = 0;
		t->t_value = strndup("If", 2);
		(*index) += 2;
		ret = true;
	}
	else if (strncmp("else", line, 4) == 0 && !(isalnum(line[4])) && !(is_separator(line[4]))) {
		t->t_type = 0;
		t->t_value = strndup("Else", 4);
		(*index) += 4;
		ret = true;
	}
	else if (strncmp("true", line, 4) == 0 && !(isalnum(line[4])) && !(is_separator(line[4]))) {
		t->t_type = 0;
		t->t_value = strndup("True", 4);
		(*index) += 4;
		ret = true;
	}
	else if (strncmp("false", line, 5) == 0 && !(isalnum(line[5])) && !(is_separator(line[5]))) {
		t->t_type = 0;
		t->t_value = strndup("False", 5);
		(*index) += 5;
		ret = true;
	}
	else if (strncmp("while", line, 5) == 0 && !(isalnum(line[5])) && !(is_separator(line[5]))) {
		t->t_type = 0;
		t->t_value = strndup("While", 5);
		(*index) += 5;
		ret = true;
	}
	return (ret);
}

bool    handle_comment(char *line, Token *t)
{
	if (is_comment(line)) { // 한 줄 주석일 경우
		t->t_type = 6;
		t->t_value = line;
		return (true);
	}
	return (false);
}

void    pass_blank(char *str, int *idx)
{
	while (str[*idx] == ' ')
		(*idx)++;
}

void    print_Tokens(Token *t, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (t[i].t_type == 1 || t[i].t_type == 2 || t[i].t_type == 3)
			printf("%s %s\n", type_list[t[i].t_type], t[i].t_value);
		else
			printf("%s\n", t[i].t_value);
	}
}

int     get_Token(FILE *fp) { // 1줄씩 읽어서 그 안에 있는 토큰을 한번에 읽어 리턴.
	size_t          len = 0;
	ssize_t         read;
	static Token    t[100];
//	Token       *t = malloc(sizeof(Token));
	int             idx = 0;
	int             i = 0;
	int             err_flag = 0;
	char            *line;
	
	read = getline(&line, &len, fp);
	if (read == -1)
		return 0;
	printf("%s\n", line);
	if (handle_comment(line, &t[i])) {
		return (1);	
	}
	while (idx != read)
	{
		pass_blank(line + idx, &idx);
		if (handle_keyword(line + idx, &t[i], &idx))
			i++;
		else if (handle_seperator(line + idx, &t[i], &idx))
			i++;
		else if (handle_operator(line + idx, &t[i], &idx))
			i++;	
		else if (handle_literal(line + idx, &t[i], &idx))
			i++;
		else if (handle_id(line + idx, &t[i], &idx, &err_flag))
			i++;
		if (err_flag == 1)
		{
			print_Tokens(t, i);
			printf("Error!\n");
			return 0;
		}
		idx++;
	}
	print_Tokens(t, i);
	return 1;
}

int     main(int argc, char **argv) {
	FILE    *fp;
	Token   *tk;
	int     line_nb = 1;
	int     i = 0;

	fp = fopen("input.txt", "r");
	if (fp == NULL)
		return (0);
	while (1)
	{
		printf("Line %d ", line_nb++);
		int i = 0;
		i = get_Token(fp);
		if (!i)
			break;
		//printf("%s %s\n", type_list[tk->t_type], tk->t_value);
		//free(tk->t_value);
		//free(tk);
	}
	fclose(fp);
	return (0);
}