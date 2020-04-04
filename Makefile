
it: cpu6502.c setup.py
	python setup.py build_ext --inplace

clean:
	rm -rf *.o *.so __pycache__ build
