all : first
first : main.c
	gcc main.c -o main -pthread
clean :
	rm main