# Makefile

# Compile source files into object files

# Run the program
run: 
	g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib main.cpp -o main -lsdsl -ldivsufsort -ldivsufsort64
	./main

# Clean object files and executable
clean:
	rm -f main

val:
	g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib main.cpp -o main -lsdsl -ldivsufsort -ldivsufsort64
	valgrind -s ./main