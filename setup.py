from distutils.core import setup, Extension

# define the extension module
cpu6502 = Extension("cpu6502", sources=["cpu.c", "cpu6502_module.c"])

# run the setup
setup(ext_modules=[cpu6502])
