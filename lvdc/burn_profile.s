FuelActivation = $E010
FuelInput1 = $E002
TABLE_MASK = $03

.segment "DATA"
BurnIntensityTable:
	.byte $B0, $B0, $FF, $FF

.segment "CODE"
busywait:
	LDA FuelInput1
	ROL
	ROL
	ROL
	AND #TABLE_MASK
	TAX
	LDA BurnIntensityTable,x
	STA FuelActivation
	LDA FuelInput1
	BNE busywait

	LDA #0
	STA FuelActivation
