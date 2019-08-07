#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>

//typedef char bool;
typedef uint8_t byte;
typedef uint16_t addr;
typedef void (*opfunct)(void);

struct st_cpustate {
	union {
		addr PC; /* program counter */
		struct {
			byte PCH;
			byte PCL;
		};
	};
	byte SP; /* stack counter */
	byte A; /* accumulator register */
	byte X; /* x register */
	byte Y; /* y register */
	byte NMI; /* non masked interrupt */
	byte IRQ; /* maskeable interrupt */
	byte RESET; /* reset interrupt */
	union {
		byte P; /* processor state flags */
		struct {
			byte C:1; /* carry */
			byte Z:1; /* zero */
			byte I:1; /* interrupt disable */
			byte D:1; /* decimal mode (not in 2A03) */
			byte B:1; /* break */
			byte _unused_:1; /* unused, always 1 */
			byte V:1; /* overflow */
			byte N:1; /* negative */
		};
	};
};

#endif
