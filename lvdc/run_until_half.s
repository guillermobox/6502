FuelActivation = $E010
FuelInput1 = $E002

.segment "CODE"
	LDA #1
	STA FuelActivation

busywait:
	LDA FuelInput1
	CMP #128
	BCS busywait

	LDA #0
	STA FuelActivation
	

	
