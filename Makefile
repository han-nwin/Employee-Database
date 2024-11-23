TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

default: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p bin
	gcc -o $@ $^

obj/%.o: src/%.c
	@mkdir -p obj
	gcc -c $< -o $@ -Iinclude

.PHONY: clean
clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

.PHONY: run
run: $(TARGET)
	./$(TARGET) $(ARGS)
