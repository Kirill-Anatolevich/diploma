CC = gcc
TARGET = prog

PREF_OBJ = ./obj/
PREF_SRC = ./src/

SRC = $(wildcard $(PREF_SRC)*.c)
OBJ = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC))

$(TARGET) : $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) -lm -lGL -lX11
	./$(TARGET)

$(PREF_OBJ)%.o : $(PREF_SRC)%.c
	$(CC) -c $< -o $@ -lm 

clean :
	rm  $(TARGET) $(PREF_OBJ)*.o
