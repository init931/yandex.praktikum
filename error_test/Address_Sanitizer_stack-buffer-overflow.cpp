int main(int argc, char **argv) {
    //https://github.com/google/sanitizers/wiki/AddressSanitizerExampleStackOutOfBounds
    int stack_array[100];
    stack_array[1] = 0;
    //stack-buffer-overflow
    return stack_array[argc + 100];  // BOOM
}
