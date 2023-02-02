SamacSys ECAD Model
567207/980801/2.49/22/3/Integrated Circuit

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r92_70"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.700) (shapeHeight 0.920))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(padStyleDef "r92_50"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.500) (shapeHeight 0.920))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Default"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 50 mils)
			(strokeWidth 5 mils)
		)
	)
	(patternDef "RXMGPSRMT" (originalName "RXMGPSRMT")
		(multiLayer
			(pad (padNum 1) (padStyleRef r92_70) (pt -6.350, -6.500) (rotation 0))
			(pad (padNum 2) (padStyleRef r92_70) (pt -5.080, -6.500) (rotation 0))
			(pad (padNum 3) (padStyleRef r92_70) (pt -3.810, -6.500) (rotation 0))
			(pad (padNum 4) (padStyleRef r92_70) (pt -2.540, -6.500) (rotation 0))
			(pad (padNum 5) (padStyleRef r92_70) (pt -1.270, -6.500) (rotation 0))
			(pad (padNum 6) (padStyleRef r92_70) (pt 1.270, -6.500) (rotation 0))
			(pad (padNum 7) (padStyleRef r92_70) (pt 2.540, -6.500) (rotation 0))
			(pad (padNum 8) (padStyleRef r92_70) (pt 3.810, -6.500) (rotation 0))
			(pad (padNum 9) (padStyleRef r92_70) (pt 5.080, -6.500) (rotation 0))
			(pad (padNum 10) (padStyleRef r92_70) (pt 6.350, -6.500) (rotation 0))
			(pad (padNum 11) (padStyleRef r92_70) (pt 6.350, 6.500) (rotation 0))
			(pad (padNum 12) (padStyleRef r92_70) (pt 5.080, 6.500) (rotation 0))
			(pad (padNum 13) (padStyleRef r92_70) (pt 3.810, 6.500) (rotation 0))
			(pad (padNum 14) (padStyleRef r92_70) (pt 2.540, 6.500) (rotation 0))
			(pad (padNum 15) (padStyleRef r92_70) (pt 1.270, 6.500) (rotation 0))
			(pad (padNum 16) (padStyleRef r92_70) (pt -1.270, 6.500) (rotation 0))
			(pad (padNum 17) (padStyleRef r92_70) (pt -2.540, 6.500) (rotation 0))
			(pad (padNum 18) (padStyleRef r92_70) (pt -3.810, 6.500) (rotation 0))
			(pad (padNum 19) (padStyleRef r92_70) (pt -5.080, 6.500) (rotation 0))
			(pad (padNum 20) (padStyleRef r92_70) (pt -6.350, 6.500) (rotation 0))
			(pad (padNum 21) (padStyleRef r92_50) (pt 0.000, -6.290) (rotation 90))
			(pad (padNum 22) (padStyleRef r92_50) (pt 0.000, 6.290) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0.000, 0.000) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 28)
			(line (pt -7.5 6.5) (pt 7.5 6.5) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 7.5 6.5) (pt 7.5 -6.5) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 7.5 -6.5) (pt -7.5 -6.5) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -7.5 -6.5) (pt -7.5 6.5) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt -8.5 7.96) (pt 8.5 7.96) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 8.5 7.96) (pt 8.5 -7.96) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 8.5 -7.96) (pt -8.5 -7.96) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt -8.5 -7.96) (pt -8.5 7.96) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 7.5 6.5) (pt 7.5 -6.5) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -7.5 6.5) (pt -7.5 -6.5) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -6.4 -7.4) (pt -6.4 -7.4) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -6.4, -7.45) (radius 0.05) (startAngle 90.0) (sweepAngle 180.0) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -6.4 -7.5) (pt -6.4 -7.5) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -6.4, -7.45) (radius 0.05) (startAngle 270) (sweepAngle 180.0) (width 0.1))
		)
	)
	(symbolDef "RXM-GPS-RM-T" (originalName "RXM-GPS-RM-T")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 3) (pt 0 mils -200 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -225 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 4) (pt 0 mils -300 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -325 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 5) (pt 0 mils -400 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -425 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 6) (pt 0 mils -500 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -525 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 7) (pt 0 mils -600 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -625 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 8) (pt 0 mils -700 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -725 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 9) (pt 0 mils -800 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -825 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 10) (pt 0 mils -900 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -925 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 11) (pt 0 mils -1000 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -1025 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 12) (pt 1300 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 13) (pt 1300 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 14) (pt 1300 mils -200 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -225 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 15) (pt 1300 mils -300 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -325 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 16) (pt 1300 mils -400 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -425 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 17) (pt 1300 mils -500 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -525 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 18) (pt 1300 mils -600 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -625 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 19) (pt 1300 mils -700 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -725 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 20) (pt 1300 mils -800 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -825 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 21) (pt 1300 mils -900 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -925 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 22) (pt 1300 mils -1000 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1070 mils -1025 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(line (pt 200 mils 100 mils) (pt 1100 mils 100 mils) (width 6 mils))
		(line (pt 1100 mils 100 mils) (pt 1100 mils -1100 mils) (width 6 mils))
		(line (pt 1100 mils -1100 mils) (pt 200 mils -1100 mils) (width 6 mils))
		(line (pt 200 mils -1100 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 1150 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "RXM-GPS-RM-T" (originalName "RXM-GPS-RM-T") (compHeader (numPins 22) (numParts 1) (refDesPrefix IC)
		)
		(compPin "1" (pinName "NC_1") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "NC_2") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "3" (pinName "1PPS") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "4" (pinName "TX") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "5" (pinName "RX") (partNum 1) (symPinNum 5) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "6" (pinName "NC_3") (partNum 1) (symPinNum 6) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "7" (pinName "LCKIND") (partNum 1) (symPinNum 7) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "8" (pinName "__RESET") (partNum 1) (symPinNum 8) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "9" (pinName "NC_4") (partNum 1) (symPinNum 9) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "10" (pinName "NC_5") (partNum 1) (symPinNum 10) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "11" (pinName "VBACKUP") (partNum 1) (symPinNum 11) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "12" (pinName "VCC") (partNum 1) (symPinNum 12) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "13" (pinName "NC_6") (partNum 1) (symPinNum 13) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "14" (pinName "NC_7") (partNum 1) (symPinNum 14) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "15" (pinName "NC_8") (partNum 1) (symPinNum 15) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "16" (pinName "NC_9") (partNum 1) (symPinNum 16) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "17" (pinName "VOUT") (partNum 1) (symPinNum 17) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "18" (pinName "GND_1") (partNum 1) (symPinNum 18) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "19" (pinName "RFIN") (partNum 1) (symPinNum 19) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "20" (pinName "GND_2") (partNum 1) (symPinNum 20) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "21" (pinName "GND_3") (partNum 1) (symPinNum 21) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "22" (pinName "GND_4") (partNum 1) (symPinNum 22) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "RXM-GPS-RM-T"))
		(attachedPattern (patternNum 1) (patternName "RXMGPSRMT")
			(numPads 22)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
				(padNum 3) (compPinRef "3")
				(padNum 4) (compPinRef "4")
				(padNum 5) (compPinRef "5")
				(padNum 6) (compPinRef "6")
				(padNum 7) (compPinRef "7")
				(padNum 8) (compPinRef "8")
				(padNum 9) (compPinRef "9")
				(padNum 10) (compPinRef "10")
				(padNum 11) (compPinRef "11")
				(padNum 12) (compPinRef "12")
				(padNum 13) (compPinRef "13")
				(padNum 14) (compPinRef "14")
				(padNum 15) (compPinRef "15")
				(padNum 16) (compPinRef "16")
				(padNum 17) (compPinRef "17")
				(padNum 18) (compPinRef "18")
				(padNum 19) (compPinRef "19")
				(padNum 20) (compPinRef "20")
				(padNum 21) (compPinRef "21")
				(padNum 22) (compPinRef "22")
			)
		)
		(attr "Mouser Part Number" "712-RXM-GPS-RM-T")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Linx-Technologies/RXM-GPS-RM-T?qs=S4ILP0tmc7SEXHa3NpZsMQ%3D%3D")
		(attr "Manufacturer_Name" "Linx Technologies")
		(attr "Manufacturer_Part_Number" "RXM-GPS-RM-T")
		(attr "Description" "RF RCVR GPS 1575.42MHZ MODULE")
		(attr "Datasheet Link" "https://datasheet.datasheetarchive.com/originals/distributors/Datasheets-DGA1/159687.pdf")
		(attr "Height" "2.2 mm")
	)

)