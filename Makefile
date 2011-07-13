TubTest: TubTest.cc Tub.h
	g++ -std=gnu++0x -lpthread -lgtest $< -o $@

test: TubTest
	./TubTest
