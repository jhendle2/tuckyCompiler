
struct A {
    struct {
        int c;
    } b;
};

int main() {
    char c = 'x';
    const char* s = "Hello, World!\n";
    int x = 15;
    int y = 0xDEADBEEF;
    float d = 3.14159265;
    struct A* a;
    int z = a->b.c;
    return 0;
}