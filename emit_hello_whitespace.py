def emit_stack_push(value):
    print("  ", end="")

    # Assume the number is positive.
    print(" ",end="")
    binary = bin(value)[2:]
    for c in binary:
        if c == '0':
            print(" ",end="")
        else:
            print("\t",end="")
    print("\n",end="")

def emit_putchar():
    print("\t\n  ",end="")

def emit_end():
    print("\n\n\n",end="")

if __name__ == "__main__":
    for c in "Hello, world! from whitespace!\n":
        emit_stack_push(ord(c))
        emit_putchar()
    emit_end()
