#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

// Declare the functions
int num_args(char* input);
char** parse_command(char* input);
void free_parsed(char** parsed, int num_tokens);

void test_num_args() {
    assert(num_args("ls -la") == 2);
    assert(num_args("   echo hello   world  ") == 3);
    assert(num_args("") == 0);
    assert(num_args("   ") == 0);
    assert(num_args("one") == 1);
    printf("✅ num_args tests passed.\n");
}

void test_parse_command() {
    char input1[] = "gcc -o program main.c";
    int expected_args1 = num_args(input1);
    char** result1 = parse_command(input1);
    assert(strcmp(result1[0], "gcc") == 0);
    assert(strcmp(result1[1], "-o") == 0);
    assert(strcmp(result1[2], "program") == 0);
    assert(strcmp(result1[3], "main.c") == 0);
    free_parsed(result1, expected_args1);

    char input2[] = "echo   hello   world";
    int expected_args2 = num_args(input2);
    char** result2 = parse_command(input2);
    assert(strcmp(result2[0], "echo") == 0);
    assert(strcmp(result2[1], "hello") == 0);
    assert(strcmp(result2[2], "world") == 0);
    free_parsed(result2, expected_args2);

    printf("✅ parse_command tests passed.\n");
}

int main() {
    test_num_args();
    test_parse_command();
    printf("🎉 All tests passed.\n");
    return 0;
}
