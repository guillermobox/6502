#include <Python.h>
#include "cpu.h"

extern byte memory[0x10000];
extern void cpu_init();
extern void cpu_step();
extern void cpu_run_until_brk();
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

static PyObject* run_until_break(PyObject* self, PyObject* args)
{
	cpu_run_until_brk();
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

static PyObject* set_p(PyObject* self, PyObject* args)
{
	int value;

	if (!PyArg_ParseTuple(args, "i", &value))
		return NULL;

	if (value > 0xFF) {
		PyErr_SetString(PyExc_ValueError, "The value is not in range");
		return NULL;
	}

	cpustate.P = value;;
	return Py_None;
};

static PyObject* get_pc(PyObject* self, PyObject* args)
{
	return Py_BuildValue("i", cpustate.PC);
};

static PyObject* set_pc(PyObject* self, PyObject* args)
{
	int addr;

	if (!PyArg_ParseTuple(args, "i", &addr))
		return NULL;

	if (addr >= 0x10000) {
		PyErr_SetString(PyExc_ValueError, "The value is not in range");
		return NULL;
	}

	cpustate.PC = addr;
	return Py_None;
};
static PyObject* get_p(PyObject* self, PyObject* args)
{
	return Py_BuildValue("i", cpustate.P);
};


static PyObject* set_stack(PyObject* self, PyObject* args)
{
	int value;

	if (!PyArg_ParseTuple(args, "i", &value))
		return NULL;

	if (value > 0xFF) {
		PyErr_SetString(PyExc_ValueError, "The value is not in range");
		return NULL;
	}

	cpustate.SP = value;
	return Py_None;
};

static PyObject* get_stack(PyObject* self, PyObject* args)
{
	return Py_BuildValue("i", cpustate.SP);
};

static PyObject* set_a(PyObject* self, PyObject* args)
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

static PyObject* get_a(PyObject* self, PyObject* args)
{
	return Py_BuildValue("i", cpustate.A);
};

static PyObject* set_x(PyObject* self, PyObject* args)
{
	int value;

	if (!PyArg_ParseTuple(args, "i", &value))
		return NULL;

	if (value > 0xFF) {
		PyErr_SetString(PyExc_ValueError, "The value is not in range");
		return NULL;
	}

	cpustate.X = value;
	return Py_None;
};

static PyObject* get_x(PyObject* self, PyObject* args)
{
	return Py_BuildValue("i", cpustate.X);
};

static PyObject* set_y(PyObject* self, PyObject* args)
{
	int value;

	if (!PyArg_ParseTuple(args, "i", &value))
		return NULL;

	if (value > 0xFF) {
		PyErr_SetString(PyExc_ValueError, "The value is not in range");
		return NULL;
	}

	cpustate.Y = value;
	return Py_None;
};

static PyObject* get_y(PyObject* self, PyObject* args)
{
	return Py_BuildValue("i", cpustate.Y);
};



static PyMethodDef Methods[] =
{
	{"init", init, METH_VARARGS, "Initialize the cpu"},
	{"read", read_byte, METH_VARARGS, "Read from memory"},
	{"write", write_byte, METH_VARARGS, "Write at memory"},
	{"step", step, METH_VARARGS, "Read and execute one instruction"},
	{"run_until_break", run_until_break, METH_VARARGS, "Read and execute one instruction"},
	{"set_pc", set_pc, METH_VARARGS, "Set the program counter"},
	{"get_pc", get_pc, METH_VARARGS, "Get the program counter"},
	{"set_sp", set_stack, METH_VARARGS, "Set the stack pointer"},
	{"get_sp", get_stack, METH_VARARGS, "Get the stack pointer"},
	{"set_p", set_p, METH_VARARGS, "Set the state register"},
	{"get_p", get_p, METH_VARARGS, "Get the state register"},
	{"set_a", set_a, METH_VARARGS, "Set the register A"},
	{"get_a", get_a, METH_VARARGS, "Get the register A"},
	{"set_x", set_x, METH_VARARGS, "Set the register X"},
	{"get_x", get_x, METH_VARARGS, "Get the register X"},
	{"set_y", set_y, METH_VARARGS, "Set the register Y"},
	{"get_y", get_y, METH_VARARGS, "Get the register Y"},
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
	PyObject * module = PyModule_Create(&cModPyDem);
	return module;
}
