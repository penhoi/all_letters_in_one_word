all:
	g++ -o allINone allINone.cpp

test: all
	./test/test.py
