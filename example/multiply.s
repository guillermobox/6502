.segment "CODE"
	SED
	LDY #$72
	LDX #$23

	STX $01
	STY $04

process_digit:
	LSR $04
	BEQ end
	BCC no_digit
	CLC
	LDA $03
	ADC $01
	STA $03
	LDA $02
	ADC $00
	STA $02
no_digit:
	ASL $01
	ROL $00
	JMP process_digit
end:
	CLC
	LDA $03
	ADC $01
	STA $03
	LDA $02
	ADC $00
	STA $02
