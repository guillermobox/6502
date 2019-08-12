FuelActivation = $E010
FuelInput1 = $E002

BURN_INTENSITY = $FF

.segment "CODE"
	LDA #BURN_INTENSITY
	STA FuelActivation

busywait:
	LDA FuelInput1
	CMP #128
	BCS busywait

	LDA #0
	STA FuelActivation
