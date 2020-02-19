import cpu6502
import time

with open('multiply.bin', 'rb') as f:
	data = f.read()

cpu6502.init()
for (index, byte) in enumerate(data):
	cpu6502.write(0x4000 + index, byte)
cpu6502.set_pc(0x4000)
cpu6502.run_until_break()

time.sleep(1)
while cpu6502.is_running():
    time.sleep(1)

print("Result: 0x{:02X}{:02X}".format( cpu6502.read(0x0002), cpu6502.read(0x0003)))
