.PHONY:build
build:
	clang++ main.cpp -o main -DNDEBUG -Ofast -std=c++17

.PHONY:run
run: build
	./main > output.txt

.PHONY:clean
clean:
	rm main
