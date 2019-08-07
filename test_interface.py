import pytest
import cpu6502

def test_can_initialize():
	cpu6502.init()

def test_can_read_and_write():
	cpu6502.write(0x0013, 0x99)
	assert 0x99 == cpu6502.read(0x0013)

def test_raises_on_read_range_error():
	with pytest.raises(ValueError):
		cpu6502.read(0x10000)

def test_raises_on_write_range_error():
	with pytest.raises(ValueError):
		cpu6502.write(0x10000, 0x00)

	with pytest.raises(ValueError):
		cpu6502.write(0x0010, 0xFF + 1)

def test_can_step():
        value_to_store = 0xFF
        
        cpu6502.init()
        cpu6502.set_accumulator(value_to_store)
        cpu6502.write(0x0000, 0x8D)
        cpu6502.write(0x0001, 0xCD)
        cpu6502.write(0x0002, 0xAB)
        cpu6502.step()

        assert value_to_store == cpu6502.read(0xABCD)
