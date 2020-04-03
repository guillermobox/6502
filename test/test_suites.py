import cpu6502
import pathlib
import pytest
import time
import subprocess


def compile(path, cfg, tmpdir):
    subprocess.run(['ca65', '-o', tmpdir / 'out.o', '-l', tmpdir / 'out.lst', path])
    subprocess.run(['ld65', tmpdir / 'out.o', '-o', tmpdir / 'out.bin', '-C', cfg])
    return pathlib.Path(tmpdir / 'out.bin').read_bytes()


@pytest.fixture
def compiled_functional_test(tmpdir):
    path = pathlib.Path.cwd() / '6502_functional_test.ca65'
    cfg = pathlib.Path.cwd() / 'functional.cfg'
    return compile(path, cfg, tmpdir)


@pytest.fixture
def compiled_decimal_test(tmpdir):
    path = pathlib.Path.cwd() / '6502_decimal_test.ca65'
    cfg = pathlib.Path.cwd() / 'decimal.cfg'
    return compile(path, cfg, tmpdir)


def test_functional_suite(compiled_functional_test):
    cpu6502.init()
    for (index, byte) in enumerate(compiled_functional_test):
            cpu6502.write(index, byte)
    cpu6502.set_pc(0x0400)

    cpu6502.run_until_break()
    time.sleep(1)
    while cpu6502.is_running():
        time.sleep(1)

    assert 13417 == cpu6502.get_pc()


def test_decimal_suite(compiled_decimal_test):
    cpu6502.init()
    for (index, byte) in enumerate(compiled_decimal_test):
            cpu6502.write(index, byte)
    cpu6502.set_pc(0x0200)

    cpu6502.run_until_break()
    time.sleep(1)
    while cpu6502.is_running():
        time.sleep(1)

    assert 0x00 == cpu6502.read(0x000B)
