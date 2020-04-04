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

	BCC continue
	LDA #1
	STA Fib1

	JMP exit
continue:
	LDX INTSIZE
	CLC
partialsum2:
	LDA Fib1,x
	ADC Fib2,x
	STA Fib2,x
	DEX
	BNE partialsum2

	BCC nextfib
	LDA #1
	STA Fib2

	JMP exit

exit:

