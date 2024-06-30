both:
	mkdir -p build && cd build && cmake .. && make

send:
	./build/sender -m 42 < input.txt

receive:
	./build/receiver -m 42 > output.txt