int add(int a, int b) {
    return a + b; 
}

int mult(int a, int b) {
    int prod = 0;
    for(int i = 0; i < b; i++) {
        prod += add(prod, a); 
    }
    return prod; 
}