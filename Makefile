TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

default: $(TARGET)

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude

.PHONY: run
clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

.PHONY: run
run: $(TARGET) 
	./$(TARGET) $(ARGS)
