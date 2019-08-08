.define INTSIZE #32
.define N #180

Fib1 = $1000
Fib2 = $1080

.segment "CODE"

	LDA #1
	LDX INTSIZE-1
	LDY N

	STA Fib1,x
	STA Fib2,x

nextfib:
	CLC
	LDX INTSIZE
partialsum1:
	DEX
	LDA Fib1,x
	ADC Fib2,x
	STA Fib1,x
	TXA
	BNE partialsum1

	CLC
	LDX INTSIZE
partialsum2:
	DEX
	LDA Fib1,x
	ADC Fib2,x
	STA Fib2,x
	TXA
	BNE partialsum2

	DEY
	BNE nextfib
