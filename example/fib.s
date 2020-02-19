.define INTSIZE #255

Fib1 = $1000
Fib2 = $2000

.segment "CODE"
	SED
	LDA #1
	LDX INTSIZE

	STA Fib1,x
	STA Fib2,x

nextfib:
	LDX INTSIZE
	CLC
partialsum1:
	LDA Fib1,x
	ADC Fib2,x
	STA Fib1,x
	DEX
	BNE partialsum1

	BCS exit

	LDX INTSIZE
	CLC
partialsum2:
	LDA Fib1,x
	ADC Fib2,x
	STA Fib2,x
	DEX
	BNE partialsum2

	BCS exit
	JMP nextfib

exit:
