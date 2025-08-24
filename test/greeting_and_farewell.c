#include <stdio.h>

void print_greeting(const char* name) {
    printf("Hello, %s!\n", name);
}

void print_farewell(const char* name) {
    printf("Goodbye, %s!\n", name);
    // Recursive call example
    if (name[0] == 'W') {
        print_farewell("World");
    }
}

int main() {
    print_greeting("World");
    print_farewell("World");
    return 0;
}