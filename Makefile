TubTest: TubTest.cc Tub.h
	g++ -lpthread -lgtest $< -o $@

test: TubTest
	./TubTest
