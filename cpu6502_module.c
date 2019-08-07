#include <Python.h>
#include "cpu.h"

extern byte memory[0x10000];
extern void cpu_init();
extern void cpu_step();
extern struct st_cpustate cpustate;

static PyObject* init(PyObject* self, PyObject* args)
{
	cpu_init();
	return Py_None;
};

static PyObject* step(PyObject* self, PyObject* args)
{
	cpu_step();
	return Py_None;
};

static PyObject* read_byte(PyObject* self, PyObject* args)
{
	int addr;

	if (!PyArg_ParseTuple(args, "i", &addr))
		return NULL;

	if (addr >= 0x10000) {
		PyErr_SetString(PyExc_ValueError, "The address is not in range");
		return NULL;
	}

	return Py_BuildValue("i", memory[addr]);
};

static PyObject* write_byte(PyObject* self, PyObject* args)
{
	int addr, value;

	if (!PyArg_ParseTuple(args, "ii", &addr, &value))
		return NULL;

	if (addr >= 0x10000) {
		PyErr_SetString(PyExc_ValueError, "The address is not in range");
		return NULL;
	}
	if (value > 0xFF) {
		PyErr_SetString(PyExc_ValueError, "The value is not in range");
		return NULL;
	}
      
	memory[addr] = value;
  
	return Py_None;
};

static PyObject* set_accumulator(PyObject* self, PyObject* args)
{
	int value;

	if (!PyArg_ParseTuple(args, "i", &value))
		return NULL;

	if (value > 0xFF) {
		PyErr_SetString(PyExc_ValueError, "The value is not in range");
		return NULL;
	}

	cpustate.A = value;
	return Py_None;
};

static PyMethodDef Methods[] =
{
	{"init", init, METH_VARARGS, "Initialize the cpu"},
	{"read", read_byte, METH_VARARGS, "Read from memory"},
	{"write", write_byte, METH_VARARGS, "Write at memory"},
	{"set_accumulator", set_accumulator, METH_VARARGS, "Set the accumulator to a given byte value"},
	{"step", step, METH_VARARGS, "Read and execute one instruction"},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef cModPyDem =
{
	PyModuleDef_HEAD_INIT,
	"cpu6502_module",
	"Emulate a non-specific 6502 cpu",
	-1,
	Methods
};

PyMODINIT_FUNC
PyInit_cpu6502(void)
{
	return PyModule_Create(&cModPyDem);
}
