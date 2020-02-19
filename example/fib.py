import cpu6502
import time

with open('fib.bin', 'rb') as f:
	data = f.read()

cpu6502.init()
for (index, byte) in enumerate(data):
	cpu6502.write(0x4000 + index, byte)
cpu6502.set_pc(0x4000)
cpu6502.run_until_break()

time.sleep(0.1)
while cpu6502.is_running():
    time.sleep(0.1)

print('=' * 80, "\nContents of 0x1000...")
for i in range(0, 256):
	if i % 16 == 0:
		print()
	print('{:02x}'.format(cpu6502.read(0x1000 + i)), end='')
print()
print('=' * 80, "\nContents of 0x2000...")
for i in range(0, 256):
	if i % 16 == 0:
		print()
	print('{:02x}'.format(cpu6502.read(0x2000 + i)), end='')
print()
