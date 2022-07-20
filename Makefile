.PHONY:build
build:
	clang++ main.cpp -o main -DNDEBUG -Ofast -std=c++17
	# clang++ main.cpp -o main -Wall -Wextra -pedantic -Wshadow -Wreturn-type -fsanitize=undefined -std=c++17

.PHONY:run
run: build
	# time ./main
	time ./main > output.txt

.PHONY:clean
clean:
	-rm main output.txt
