
all: cpu6502.c setup.py cpu.c cpu.h
	python setup.py build_ext --inplace

clean:
	rm -rf *.o *.so __pycache__ build
