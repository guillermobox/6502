#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "cpu.h"

byte memory[0x10000];
byte op;
addr address;
struct st_cpustate cpustate;

static inline void memstore(addr address, byte data)
{
	if (address >> 8 == 0xFF) /* page FF is ROM, not writable */
		return;
	*(memory + address) = data;
};

static inline byte memload(addr address)
{
	return *(memory + address);
};

static void stack_push(byte data)
{
	memstore(0x0100 + cpustate.SP--, data);
};

static byte stack_pull()
{
	return memload(0x0100 + ++cpustate.SP);
};

/*
 * Chapter 6 os MOS
 * INDEX REGISTERS AND INDEX ADDRESSING CONCEPTS
 */

static void imp()
{
};

static void dir()
{
	address = cpustate.PC++;
};

static void zer()
{
	address = (addr) memload(cpustate.PC++);
};

static void zex()
{
	address = (addr) 0xFF & (memload(cpustate.PC++) + cpustate.X);
};

static void zey()
{
	address = (addr) 0xFF & (memload(cpustate.PC++) + cpustate.Y);
};

static void aba()
{
	address  =  memload(cpustate.PC++);
	address |= memload(cpustate.PC++) << 8;
};

static void abx()
{
	address  = memload(cpustate.PC++);
	address |= memload(cpustate.PC++) << 8;
	address += cpustate.X;
};

static void aby()
{
	address  = memload(cpustate.PC++);
	address |= memload(cpustate.PC++) << 8;
	address += cpustate.Y;
};

static void ind()
{
	addr off = memload(cpustate.PC++);
	off |= memload(cpustate.PC++) << 8;
	address  = memload(off);
	address |= memload(off + 1) << 8;
};

static void aix()
{
	addr off;
	off = 0xFF & ((memload(cpustate.PC++) + cpustate.X));
	address  = memload(off & 0xff);
	address |= memload((off + 1) & 0xff) << 8;
};

static void aiy()
{
	addr off;
	off = memload(cpustate.PC++);
	address  = memload(off);
	address |= memload((off + 1) & 0xff) << 8;
	address += cpustate.Y;
};

static void rel()
{
	addr original = cpustate.PC + 0x01;
	byte offset = memload(cpustate.PC++);
	if (offset & 0x80)
		address = original + (0xff00 | offset);
	else
		address = original + offset;
};

/*
 * Chapter 2 of MOS
 * THE DATA BUS, ACCUMULATOR AND ARITHMETIC UNIT
 */

static void lda(void) /* page 4 MOS */
{
	cpustate.A = memload(address);
	cpustate.Z = cpustate.A == 0x00;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
};

static void sta(void) /* page 5 MOS */
{
	memstore(address, cpustate.A);
};

static void adc(void) /* page 7 MOS */
{
	byte value = memload(address);
	uint16_t sum = (uint16_t) cpustate.A + (uint16_t) value + cpustate.C;
	if (cpustate.D) {
		if (sum & 0x0100) {
			cpustate.C = 1;
			sum = sum + (6<<4);
		}
		if ((sum & 0x0f) > 0x09)
			sum = sum + 6;
		if ((sum & 0xf0) > 0x90)
			sum = sum + (6<<4);
	}
	cpustate.V = (((cpustate.A ^ value) & 0x80) == 0x00) && ((sum & 0x80) != (value & 0x80));
	cpustate.A = sum;
	cpustate.C = (sum & 0x0100) != 0x0000;
	cpustate.Z = cpustate.A == 0x00;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
};

static void sbc(void) /* page 14 MOS */
{
	byte value = memload(address);
	byte value2 = (value ^ 0xFF) + cpustate.C;
	uint16_t sum = (uint16_t) cpustate.A + (uint16_t) value2;
	if (cpustate.D) {
		if ((sum & 0x0f) > 0x09)
			sum = sum - 6;
		if ((sum & 0xf0) > 0x90)
			sum = sum - (6<<4);
	}
	cpustate.V = (((cpustate.A ^ value) & 0x80) == 0x00) && ((sum & 0x80) != (value & 0x80));
	cpustate.A = sum;
	if (value!=0)
		cpustate.C = (sum & 0x0100) != 0x0000;
	cpustate.Z = cpustate.A == 0x00;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
};

static void and(void) /* page 20 MOS */
{
	byte value = memload(address);
	cpustate.A &= value;
	cpustate.Z = cpustate.A == 0x00;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
};

static void ora(void) /* page 21 MOS */
{
	byte value = memload(address);
	cpustate.A |= value;
	cpustate.Z = cpustate.A == 0x00;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
};

static void eor(void) /* page 21 MOS */
{
	byte value = memload(address);
	cpustate.A ^= value;
	cpustate.Z = cpustate.A == 0x00;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
};


/*
 * Chapter 3 of MOS
 * CONCEPTS OF FLAGS AND STATUS REGISTER
 */
static void sec(void) /* page 24 MOS */
{
	cpustate.C = 1;
};

static void clc(void) /* page 25 MOS */
{
	cpustate.C = 0;
};

static void sei(void) /* page 26 MOS */
{
	cpustate.I = 1;
};

static void cli(void) /* page 26 MOS */
{
	cpustate.I = 0;
};

static void sed(void) /* page 26 MOS */
{
	cpustate.D = 1;
};

static void cld(void) /* page 27 MOS */
{
	cpustate.D = 0;
};

static void clv(void) /* page 28 MOS */
{
	cpustate.V = 0;
};

/*
 * Chapter 4 of MOS
 * TEST BRANCH AND JUMP INSTRUCTIONS
 */
static void jmp(void) /* page 36 MOS */
{
	cpustate.PC = address;
};

static void bmi(void) /* page 40 MOS */
{
	if (cpustate.N) {
		cpustate.PC = address;
	}
};

static void bpl(void) /* page 40 MOS */
{
	if (cpustate.N == 0) {
		cpustate.PC = address;
	}
};

static void bcc(void) /* page 40 MOS */
{
	if (cpustate.C == 0) {
		cpustate.PC = address;
	}
};

static void bcs(void) /* page 40 MOS */
{
	if (cpustate.C) {
		cpustate.PC = address;
	}
};

static void beq(void) /* page 41 MOS */
{
	if (cpustate.Z) {
		cpustate.PC = address;
	}
};

static void bne(void) /* page 41 MOS */
{
	if (cpustate.Z == 0) {
		cpustate.PC = address;
	}
};

static void bvs(void) /* page 41 MOS */
{
	if (cpustate.V) {
		cpustate.PC = address;
	}
};

static void bvc(void) /* page 41 MOS */
{
	if (cpustate.V == 0) {
		cpustate.PC = address;
	}
};

static void cmp(void) /* page 45 MOS */
{
	byte mem = memload(address);
	cpustate.C = (mem > cpustate.A)? 0 : 1;
	cpustate.N = ((cpustate.A - mem) & 0x80) != 0x00;
	cpustate.Z = cpustate.A == mem;
};

static void bit(void) /* page 47 MOS */
{
	byte value = memload(address);
	byte and = cpustate.A & value;
	cpustate.Z = and == 0x00;
	cpustate.N = (value & 0x80) != 0x00;
	cpustate.V = (value & 0x40) != 0x00;
};

/*
 * Chapter 7 of MOS
 * INDEX REGISTER INSTRUCTIONS
 */
static void ldx(void) /* page 96 MOS */
{
	cpustate.X = memload(address);
	cpustate.N = (cpustate.X & 0x80) != 0;
	cpustate.Z = (cpustate.X == 0x00);
};

static void ldy(void) /* page 96 MOS */
{
	cpustate.Y = memload(address);
	cpustate.N = (cpustate.Y & 0x80) != 0;
	cpustate.Z = (cpustate.Y == 0x00);
};

static void stx(void) /* page 97 MOS */
{
	memstore(address, cpustate.X);
};

static void sty(void) /* page 97 MOS */
{
	memstore(address, cpustate.Y);
};

static void inx(void) /* page 97 MOS */
{
	cpustate.X += 0x01;
	cpustate.N = (cpustate.X & 0x80) != 0;
	cpustate.Z = (cpustate.X == 0x00);
};

static void iny(void) /* page 97 MOS */
{
	cpustate.Y += 0x01;
	cpustate.N = (cpustate.Y & 0x80) != 0;
	cpustate.Z = (cpustate.Y == 0x00);
};

static void dex(void) /* page 98 MOS */
{
	cpustate.X -= 0x01;
	cpustate.N = (cpustate.X & 0x80) != 0;
	cpustate.Z = (cpustate.X == 0x00);
};

static void dey(void) /* page 98 MOS */
{
	cpustate.Y -= 0x01;
	cpustate.N = (cpustate.Y & 0x80) != 0;
	cpustate.Z = (cpustate.Y == 0x00);
};

static void cpx(void) /* page 99 MOS */
{
	byte value = memload(address);
	byte sub = cpustate.X - value;
	cpustate.Z = value == cpustate.X;
	cpustate.N = (sub & 0x80) != 0;
	cpustate.C = (value > cpustate.X)? 0 : 1;
};

static void cpy(void) /* page 99 MOS */
{
	byte value = memload(address);
	byte sub = cpustate.Y - value;
	cpustate.Z = value == cpustate.Y;
	cpustate.N = (sub & 0x80) != 0;
	cpustate.C = (value > cpustate.Y)? 0 : 1;
};

static void tax(void) /* page 100 MOS */
{
	cpustate.X = cpustate.A;
	cpustate.N = (cpustate.X & 0x80) != 0;
	cpustate.Z = (cpustate.X == 0x00);
};

static void tay(void) /* page 101 MOS */
{
	cpustate.Y = cpustate.A;
	cpustate.N = (cpustate.Y & 0x80) != 0;
	cpustate.Z = (cpustate.Y == 0x00);
};

static void txa(void) /* page 100 MOS */
{
	cpustate.A = cpustate.X;
	cpustate.N = (cpustate.X & 0x80) != 0;
	cpustate.Z = (cpustate.X == 0x00);
};

static void tya(void) /* page 101 MOS */
{
	cpustate.A = cpustate.Y;
	cpustate.N = (cpustate.Y & 0x80) != 0;
	cpustate.Z = (cpustate.Y == 0x00);
};

/*
 * Chapter 8 of MOS
 * STACK PROCESSING
 */
static void jsr(void) /* page 106 MOS */
{
	cpustate.PC--;
	stack_push(cpustate.PCL);
	stack_push(cpustate.PCH);
	cpustate.PC = address;
};

static void rts(void) /* page 108 MOS */
{
	cpustate.PCH = stack_pull();
	cpustate.PCL = stack_pull();
	cpustate.PC += 1;
};

static void pha(void) /* page 117 MOS */
{
	stack_push(cpustate.A);
};

static void pla(void) /* page 118 MOS */
{
	cpustate.A = stack_pull();
	cpustate.Z = cpustate.A == 0x00;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
};

static void txs(void) /* page 120 MOS */
{
	cpustate.SP = cpustate.X;
};

static void tsx(void) /* page 122 MOS */
{
	cpustate.X = cpustate.SP;
	cpustate.Z = cpustate.X == 0x00;
	cpustate.N = (cpustate.X & 0x80) != 0x00;
};

static void php(void) /* page 122 MOS */
{
	stack_push(cpustate.P);
};

static void plp(void) /* page 123 MOS */
{
	cpustate.P = stack_pull();
};

static void rti(void) /* page 132 MOS */
{
  cpustate.P = stack_pull();
  cpustate.PC = stack_pull() | stack_pull() << 8;

};

static void brk(void) /* page 144 MOS */
{
  stack_push((cpustate.PC+1) >> 8);
  stack_push((cpustate.PC+1) & 0xFF);
  stack_push(cpustate.P | 16 | 32);
  cpustate.I = 1;
  cpustate.B = 1;
  cpustate._unused_ = 1;
  cpustate.PC = (addr)memload(0xFFFE) | ((addr)memload(0xFFFF) << 8);
};

/*
 * Chapter 10
 * SHIFT AND MEMORY MODIFY INSTRUCTIONS
 */
static void lsra(void) /* page 148 MOS */
{
	cpustate.C = cpustate.A & 0x01;
	cpustate.A >>= 1;
	cpustate.N = 0;
	cpustate.Z = cpustate.A == 0x00;
};

static void lsr(void) /* page 148 MOS */
{
	byte value = memload(address);
	cpustate.C = value & 0x01;
	value >>= 1;
	cpustate.N = 0;
	cpustate.Z = value == 0x00;
	memstore(address, value);
};

static void asla(void) /* page 149 MOS */
{
	cpustate.C = (cpustate.A & 0x80) != 0x00;
	cpustate.A <<= 1;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
	cpustate.Z = cpustate.A == 0x00;
};

static void asl(void) /* page 149 MOS */
{
	byte value = memload(address);
	cpustate.C = (value & 0x80) != 0x00;
	value <<= 1;
	cpustate.N = (value & 0x80) != 0x00;
	cpustate.Z = value == 0x00;
	memstore(address, value);
};

static void rola(void) /* page 149 MOS */
{
	byte oldc = cpustate.C;
	cpustate.C = (cpustate.A & 0x80) != 0x00;
	cpustate.A <<= 1;
	if (oldc) cpustate.A += 1;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
	cpustate.Z = cpustate.A == 0x00;
};

static void rol(void) /* page 149 MOS */
{
	byte value = memload(address);
	byte oldc = cpustate.C;
	cpustate.C = (value & 0x80) != 0x00;
	value <<= 1;
	if (oldc) value += 1;
	cpustate.N = (value & 0x80) != 0x00;
	cpustate.Z = value == 0x00;
	memstore(address, value);
};

static void rora(void) /* page 150 MOS */
{
	byte oldc = cpustate.C;
	cpustate.C = cpustate.A & 0x01;
	cpustate.A >>= 1;
	if (oldc)
		cpustate.A += 0x80;
	cpustate.N = (cpustate.A & 0x80) != 0x00;
	cpustate.Z = cpustate.A == 0x00;
};

static void ror(void) /* page 149 MOS */
{
	byte value = memload(address);
	byte oldc = cpustate.C;
	cpustate.C = value & 0x01;
	value >>= 1;
	if (oldc)
		value += 0x80;
	cpustate.N = (value & 0x80) != 0x00;
	cpustate.Z = value == 0x00;
	memstore(address, value);
};

static void inc(void) /* page 155 MOS */
{
	byte value = memload(address) + 1;
	cpustate.Z = value == 0x00;
	cpustate.N = (value & 0x80) != 0x00;
	memstore(address, value);
};

static void dec(void) /* page 155 MOS */
{
	byte value = memload(address) + 0xff;
	cpustate.Z = value == 0x00;
	cpustate.N = (value & 0x80) != 0x00;
	memstore(address, value);
};

static void nop(void)
{
};

#define NUL NULL

opfunct addressing_map[] = {
       /* 0   1    2    3    4    5    6    7    8    9    a    b    c    d    e    f  */
/* 0 */ imp, aix, NUL, NUL, NUL, zer, zer, NUL, imp, dir, imp, NUL, NUL, aba, aba, NUL,
/* 1 */ rel, aiy, NUL, NUL, NUL, zex, zex, NUL, imp, aby, NUL, NUL, NUL, abx, abx, NUL,
/* 2 */ aba, aix, NUL, NUL, zer, zer, zer, NUL, imp, dir, imp, NUL, aba, aba, aba, NUL,
/* 3 */ rel, aiy, NUL, NUL, NUL, zex, zex, NUL, imp, aby, NUL, NUL, NUL, abx, abx, NUL,
/* 4 */ imp, aix, NUL, NUL, NUL, zer, zer, NUL, imp, dir, imp, NUL, aba, aba, aba, NUL,
/* 5 */ rel, aiy, NUL, NUL, NUL, zex, zex, NUL, imp, aby, NUL, NUL, NUL, abx, abx, NUL,
/* 6 */ imp, aix, NUL, NUL, NUL, zer, zer, NUL, imp, dir, imp, NUL, ind, aba, aba, NUL,
/* 7 */ rel, aiy, NUL, NUL, NUL, zex, zex, NUL, imp, aby, NUL, NUL, NUL, abx, abx, NUL,
/* 8 */ NUL, aix, NUL, NUL, zer, zer, zer, NUL, imp, NUL, imp, NUL, aba, aba, aba, NUL,
/* 9 */ rel, aiy, NUL, NUL, zex, zex, zey, NUL, imp, aby, imp, NUL, NUL, abx, NUL, NUL,
/* a */ dir, aix, dir, NUL, zer, zer, zer, NUL, imp, dir, imp, NUL, aba, aba, aba, NUL,
/* b */ rel, aiy, NUL, NUL, zex, zex, zey, NUL, imp, aby, imp, NUL, abx, abx, aby, NUL,
/* c */ dir, aix, NUL, NUL, zer, zer, zer, NUL, imp, dir, imp, NUL, aba, aba, aba, NUL,
/* d */ rel, aiy, NUL, NUL, NUL, zex, zex, NUL, imp, aby, NUL, NUL, NUL, abx, abx, NUL,
/* e */ dir, aix, NUL, NUL, zer, zer, zer, NUL, imp, dir, imp, NUL, aba, aba, aba, NUL,
/* f */ rel, aiy, NUL, NUL, NUL, zex, zex, NUL, imp, aby, NUL, NUL, NUL, abx, abx, NUL,
};

opfunct instruction_map[] = {
       /* 0   1    2    3    4    5    6    7    8    9    a    b    c    d    e    f  */
/* 0 */ brk, ora, NUL, NUL, NUL, ora, asl, NUL, php, ora,asla, NUL, NUL, ora, asl, NUL,
/* 1 */ bpl, ora, NUL, NUL, NUL, ora, asl, NUL, clc, ora, NUL, NUL, NUL, ora, asl, NUL,
/* 2 */ jsr, and, NUL, NUL, bit, and, rol, NUL, plp, and,rola, NUL, bit, and, rol, NUL,
/* 3 */ bmi, and, NUL, NUL, NUL, and, rol, NUL, sec, and, NUL, NUL, NUL, and, rol, NUL,
/* 4 */ rti, eor, NUL, NUL, NUL, eor, lsr, NUL, pha, eor,lsra, NUL, jmp, eor, lsr, NUL,
/* 5 */ bvc, eor, NUL, NUL, NUL, eor, lsr, NUL, cli, eor, NUL, NUL, NUL, eor, lsr, NUL,
/* 6 */ rts, adc, NUL, NUL, NUL, adc, ror, NUL, pla, adc,rora, NUL, jmp, adc, ror, NUL,
/* 7 */ bvs, adc, NUL, NUL, NUL, adc, ror, NUL, sei, adc, NUL, NUL, NUL, adc, ror, NUL,
/* 8 */ NUL, sta, NUL, NUL, sty, sta, stx, NUL, dey, NUL, txa, NUL, sty, sta, stx, NUL,
/* 9 */ bcc, sta, NUL, NUL, sty, sta, stx, NUL, tya, sta, txs, NUL, NUL, sta, NUL, NUL,
/* a */ ldy, lda, ldx, NUL, ldy, lda, ldx, NUL, tay, lda, tax, NUL, ldy, lda, ldx, NUL,
/* b */ bcs, lda, NUL, NUL, ldy, lda, ldx, NUL, clv, lda, tsx, NUL, ldy, lda, ldx, NUL,
/* c */ cpy, cmp, NUL, NUL, cpy, cmp, dec, NUL, iny, cmp, dex, NUL, cpy, cmp, dec, NUL,
/* d */ bne, cmp, NUL, NUL, NUL, cmp, dec, NUL, cld, cmp, NUL, NUL, NUL, cmp, dec, NUL,
/* e */ cpx, sbc, NUL, NUL, cpx, sbc, inc, NUL, inx, sbc, nop, NUL, cpx, sbc, inc, NUL,
/* f */ beq, sbc, NUL, NUL, NUL, sbc, inc, NUL, sed, sbc, NUL, NUL, NUL, sbc, inc, NUL,
};

void cpu_init()
{
	memset(memory, 0, sizeof(memory));
	memset(&cpustate, 0, sizeof(cpustate));
	cpustate.SP = 0xFF;
	cpustate.B = 1;
	cpustate._unused_ = 1;
	cpustate.PC = (addr)memload(0xFFFC) | ((addr)memload(0xFFFD) << 8);
	cpustate.PC = 0x00;
};

void check_interrupts()
{
	if (cpustate.NMI) {
		cpustate.NMI = 0;
		addr newpc = (addr) memload(0xfffa) | ((addr) memload(0xfffb) << 8);
		stack_push(cpustate.P);
		stack_push((byte)cpustate.PC);
		stack_push((byte)(cpustate.PC >> 8));
		cpustate.PC = newpc;

	} else if (cpustate.RESET) {
		memset(&cpustate, 0, sizeof(cpustate));
		cpustate.SP = 0xFF;
		cpustate.PC = (addr)memload(0xFFFC) | ((addr)memload(0xFFFD) << 8);
	}
};

void cpu_step()
{
	opfunct addressing, instruction;
	op = memload(cpustate.PC);
	addressing = addressing_map[op];
	instruction = instruction_map[op];

	if (instruction == brk) {
		return;
	}

	cpustate.PC++;

	addressing();
	instruction();
	check_interrupts();
};

void cpu_run_until_brk()
{
	int counter = 0;
	struct timespec start={0,0}, end={0,0};
	double elapsed;

	clock_gettime(CLOCK_MONOTONIC, &start);
	do {
		cpu_step();
		counter += 1;
	} while (op != 0x0);
	clock_gettime(CLOCK_MONOTONIC, &end);

	elapsed = ((double)end.tv_sec + 1.0e-9*end.tv_nsec) - ((double)start.tv_sec + 1.0e-9*start.tv_nsec);

	printf(" * Ran %d instructions in %lf seconds: %lf Mop/s\n",
		counter,
		elapsed,
		(double) counter / elapsed / 1e6);

};
