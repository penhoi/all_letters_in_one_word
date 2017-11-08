all:
	#g++ -g3 -o allINone allINone.cpp
	g++ -o allINone allINone.cpp

test: all
	./test/test.py
