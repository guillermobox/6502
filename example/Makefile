it: cpu6502.so multiply.bin
	python multiply.py

cpu6502.so:
	ln -s ../cpu6502.*.so cpu6502.so

fib.bin: fib.s
	cl65 --config memory.cfg fib.s -o fib.bin

multiply.bin: multiply.s
	cl65 --config memory.cfg multiply.s -o multiply.bin

clean:
	rm -f *.o *.bin *.so
