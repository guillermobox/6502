
cpu6502.so:
	ln -s ../cpu6502.*.so cpu6502.so

fib.bin:
	cl65 --config memory.cfg fib.s -o fib.bin

it: cpu6502.so fib.bin
	python fib.py

clean:
	rm -f *.o *.bin *.so
