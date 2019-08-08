import cpu6502

with open('fib.bin', 'rb') as f:
	data = f.read()

cpu6502.init()
for (index, byte) in enumerate(data):
	cpu6502.write(0x4000 + index, byte)
cpu6502.set_pc(0x4000)
cpu6502.run_until_break()

print("Contents of 0x1000-0x1020: 0x", end="")
for i in range(32):
	print('{:02x}'.format(cpu6502.read(0x1000 + i)), end='')
print()
print("Contents of 0x1080-0x10A0: 0x", end="")
for i in range(32):
	print('{:02x}'.format(cpu6502.read(0x1080 + i)), end='')
print()
