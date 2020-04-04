import cpu6502
import pathlib
import pytest
import time
import subprocess

SUITES_DIRECTORY = pathlib.Path(__file__).parent / "suites"

def compile(path, config, tmpdir):
    object = tmpdir / "suite.o"
    listing = tmpdir / "suite.lst"
    binary = tmpdir / "suite.bin"
    subprocess.run(["ca65", "-o", object, "-l", listing, path], cwd=tmpdir, check=True)
    subprocess.run(["ld65", "-o", binary, "-C", config, object], cwd=tmpdir, check=True)
    return pathlib.Path(binary).read_bytes()


@pytest.fixture
def compiled_functional_test(tmpdir):
    code = SUITES_DIRECTORY / "functional.ca65"
    cfg = SUITES_DIRECTORY / "functional.cfg"
    return compile(code, cfg, tmpdir)


@pytest.fixture
def compiled_decimal_test(tmpdir):
    code = SUITES_DIRECTORY / "decimal.ca65"
    cfg = SUITES_DIRECTORY / "decimal.cfg"
    return compile(code, cfg, tmpdir)


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
