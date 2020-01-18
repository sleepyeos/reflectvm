CC = gcc
CFLAGS = -std=gnu99

reflect: src/reflect.* src/rvm_launcher.c
	rm -f bin/*
	$(CC) -c -o bin/reflect.o src/reflect.c
	$(CC) -c -o bin/disasm_backend.o src/disasm_backend.c
	$(CC) -o bin/reflectvm $(CFLAGS) src/rvm_launcher.c bin/reflect.o
	$(CC) -o bin/rdbg $(CFLAGS) src/rdbg.c bin/disasm_backend.o bin/reflect.o
	$(CC) -o bin/rdsm $(CFLAGS) src/disasm.c src/queue.c bin/disasm_backend.o 
	rm -f bin/*.o

