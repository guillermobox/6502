import cpu6502
import pytest
import sys

@pytest.fixture
def clean_cpu_with_dcb():
    cpu6502.init()
    p = cpu6502.get_p()
    p = p | 0x08
    cpu6502.set_p(p)

def bcd(n):
    return int(str(n), 16)

def split(result):
    if len(str(result)) > 2:
        carry = 1
    else:
        carry = 0

    acc = int(str(result)[-2:], 16)
    return acc, carry

@pytest.mark.parametrize('n, m, result', [
    (bcd(i), bcd(j), split(i + j))
    for i in range(100)
    for j in range(100)
])
def test_adc(clean_cpu_with_dcb, n, m, result):
    # LDA #n 
    # ADC #m
    program = [0xA9, n, 0x69, m] 

    for addr, value in enumerate(program):
        cpu6502.write(0x0000 + addr, value)

    cpu6502.set_pc(0x0000)
    cpu6502.step()
    cpu6502.step()

    assert result[0] == cpu6502.get_a()
    assert result[1] == cpu6502.get_p() & 0x01
