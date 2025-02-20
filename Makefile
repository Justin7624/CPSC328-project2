all: p2

p2: p2.cpp
	g++ -o p2 p2.cpp

clean:
	rm -f p2

submit:
	~schwesin/bin/submit cpsc328 project2

.PHONY: all clean submit
