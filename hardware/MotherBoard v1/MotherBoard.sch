EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:USB_B_Micro J2
U 1 1 5DA16593
P 8000 2250
F 0 "J2" H 8057 2717 50  0000 C CNN
F 1 "USB_B_Micro" H 8057 2626 50  0000 C CNN
F 2 "USB_Micro:USB_Micro" H 8150 2200 50  0001 C CNN
F 3 "~" H 8150 2200 50  0001 C CNN
	1    8000 2250
	1    0    0    -1  
$EndComp
Text GLabel 8550 2350 2    50   Input ~ 0
TARGET_USB_N
Text GLabel 8550 2250 2    50   Input ~ 0
TARGET_USB_P
Wire Wire Line
	8300 2050 8550 2050
$Comp
L Device:C C13
U 1 1 5DA17D0A
P 9350 2200
F 0 "C13" H 9465 2246 50  0000 L CNN
F 1 "4.7 nF" H 9465 2155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9388 2050 50  0001 C CNN
F 3 "~" H 9350 2200 50  0001 C CNN
	1    9350 2200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C15
U 1 1 5DA182A6
P 10150 2200
F 0 "C15" H 10265 2246 50  0000 L CNN
F 1 "4.7 nF" H 10265 2155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10188 2050 50  0001 C CNN
F 3 "~" H 10150 2200 50  0001 C CNN
	1    10150 2200
	1    0    0    -1  
$EndComp
$Comp
L Device:Ferrite_Bead FB2
U 1 1 5DA19446
P 9800 2050
F 0 "FB2" V 9526 2050 50  0000 C CNN
F 1 "BLM18PG471SN1" V 9617 2050 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9730 2050 50  0001 C CNN
F 3 "~" H 9800 2050 50  0001 C CNN
	1    9800 2050
	0    1    1    0   
$EndComp
Wire Wire Line
	9350 2050 9650 2050
Connection ~ 9350 2050
Wire Wire Line
	9950 2050 10150 2050
Wire Wire Line
	10150 2050 10450 2050
Connection ~ 10150 2050
$Comp
L Device:Fuse F1
U 1 1 5DA1B5C7
P 10600 2050
F 0 "F1" V 10403 2050 50  0000 C CNN
F 1 "MF-MSMF050-2" V 10494 2050 50  0000 C CNN
F 2 "MSMF050:MSMF050" V 10530 2050 50  0001 C CNN
F 3 "~" H 10600 2050 50  0001 C CNN
	1    10600 2050
	0    1    1    0   
$EndComp
Text GLabel 9350 2350 3    50   Input ~ 0
GND
Text GLabel 10150 2350 3    50   Input ~ 0
GND
Wire Wire Line
	10750 2050 11100 2050
Text GLabel 11100 2050 2    50   Input ~ 0
VUSB
Text GLabel 8000 2650 3    50   Input ~ 0
GND
$Comp
L Power_Protection:PRTR5V0U2X D3
U 1 1 5DA59D26
P 6550 2250
F 0 "D3" H 7094 2296 50  0000 L CNN
F 1 "PRTR5V0U2X" H 7094 2205 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-143" H 6610 2250 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PRTR5V0U2X.pdf" H 6610 2250 50  0001 C CNN
	1    6550 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2050 8550 1650
Connection ~ 8550 2050
Wire Wire Line
	8550 2050 9350 2050
Text GLabel 6550 2750 3    50   Input ~ 0
GND
Wire Wire Line
	8300 2250 8550 2250
Wire Wire Line
	8300 2350 8550 2350
Text GLabel 6050 2250 0    50   Input ~ 0
TARGET_USB_P
Text GLabel 7050 2250 2    50   Input ~ 0
TARGET_USB_N
Wire Wire Line
	6550 1750 6550 1650
Wire Wire Line
	6550 1650 8550 1650
Wire Wire Line
	7900 2650 7900 3000
$Comp
L Device:R R10
U 1 1 5DA6AD99
P 8250 3150
F 0 "R10" H 8320 3196 50  0000 L CNN
F 1 "DNP" H 8320 3105 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 8180 3150 50  0001 C CNN
F 3 "~" H 8250 3150 50  0001 C CNN
	1    8250 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:C C11
U 1 1 5DA6B60C
P 7900 3150
F 0 "C11" H 8015 3196 50  0000 L CNN
F 1 "DNP" H 8015 3105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 7938 3000 50  0001 C CNN
F 3 "~" H 7900 3150 50  0001 C CNN
	1    7900 3150
	1    0    0    -1  
$EndComp
Connection ~ 7900 3000
Text GLabel 8250 3300 3    50   Input ~ 0
GND
Text GLabel 7900 3300 3    50   Input ~ 0
GND
Wire Wire Line
	7900 3000 8250 3000
$Comp
L rn2483:RN2483 U3
U 1 1 5DAA8A45
P 9600 6450
F 0 "U3" H 9600 4969 60  0000 C CNN
F 1 "RN2483" H 9600 4863 60  0000 C CNN
F 2 "RN2483:RN2483" H 9700 5750 60  0001 C CNN
F 3 "" H 9700 5750 60  0000 C CNN
	1    9600 6450
	1    0    0    -1  
$EndComp
Text GLabel 8700 6350 0    50   Input ~ 0
GND
Wire Wire Line
	8700 6250 8300 6250
$Comp
L Device:R R11
U 1 1 5DAAC54E
P 8300 6100
F 0 "R11" H 8370 6146 50  0000 L CNN
F 1 "10k" H 8370 6055 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 8230 6100 50  0001 C CNN
F 3 "~" H 8300 6100 50  0001 C CNN
	1    8300 6100
	1    0    0    -1  
$EndComp
Connection ~ 8300 6250
Wire Wire Line
	8300 6250 8150 6250
Text GLabel 8300 5950 1    50   Input ~ 0
3V3
Text GLabel 8150 6250 0    50   Input ~ 0
RESETRN2483
NoConn ~ 10500 5950
NoConn ~ 10500 6050
NoConn ~ 10500 6250
NoConn ~ 10500 6150
NoConn ~ 10500 6350
NoConn ~ 10500 6450
NoConn ~ 10500 6550
NoConn ~ 10500 6850
NoConn ~ 10500 6950
NoConn ~ 10500 7350
NoConn ~ 10500 7450
NoConn ~ 10500 7550
NoConn ~ 10500 7650
NoConn ~ 8700 7650
NoConn ~ 8700 7550
NoConn ~ 8700 7450
NoConn ~ 8700 7350
NoConn ~ 8700 7250
NoConn ~ 8700 6950
NoConn ~ 8700 6850
NoConn ~ 8700 6750
NoConn ~ 8700 6650
NoConn ~ 8700 6550
NoConn ~ 8700 6150
NoConn ~ 8700 6050
NoConn ~ 8700 5950
Text GLabel 9150 5450 1    50   Input ~ 0
GND
Text GLabel 9300 5450 1    50   Input ~ 0
GND
Text GLabel 9600 5450 1    50   Input ~ 0
GND
Text GLabel 9900 5450 1    50   Input ~ 0
GND
Text GLabel 10050 5450 1    50   Input ~ 0
GND
Text GLabel 10500 5850 2    50   Input ~ 0
GND
Text GLabel 10500 6750 2    50   Input ~ 0
GND
Text GLabel 10500 7050 2    50   Input ~ 0
GND
Text GLabel 10500 7750 2    50   Input ~ 0
GND
Text GLabel 8700 7750 0    50   Input ~ 0
GND
Text GLabel 8700 7150 0    50   Input ~ 0
GND
NoConn ~ 8700 7050
Text GLabel 8700 5850 0    50   Input ~ 0
GND
Connection ~ 9800 10350
Wire Wire Line
	9800 10350 10300 10350
Text GLabel 8400 10450 0    50   Input ~ 0
GND
Wire Wire Line
	8150 10350 8000 10350
Connection ~ 8150 10350
Wire Wire Line
	8150 10550 8150 10350
Wire Wire Line
	8400 10550 8350 10550
NoConn ~ 9250 10550
Text GLabel 8000 10750 3    50   Input ~ 0
GND
$Comp
L Device:C C12
U 1 1 5DB161FD
P 8000 10600
F 0 "C12" H 7800 10650 50  0000 L CNN
F 1 "1 µF" H 7700 10550 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8038 10450 50  0001 C CNN
F 3 "~" H 8000 10600 50  0001 C CNN
	1    8000 10600
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 10350 8150 10350
Wire Wire Line
	9250 10350 9800 10350
Text GLabel 9800 10650 3    50   Input ~ 0
GND
$Comp
L Device:C C14
U 1 1 5DB1141B
P 9800 10500
F 0 "C14" H 9915 10546 50  0000 L CNN
F 1 "1 µF" H 9915 10455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9838 10350 50  0001 C CNN
F 3 "~" H 9800 10500 50  0001 C CNN
	1    9800 10500
	1    0    0    -1  
$EndComp
Text GLabel 7900 10350 0    50   Input ~ 0
VUSB
Text GLabel 11150 9750 1    50   Input ~ 0
VBAT
Text GLabel 11450 10350 2    50   Input ~ 0
3V3
$Comp
L Diode:PMEG6010CEJ D4
U 1 1 5DBA3AB3
P 10450 10350
F 0 "D4" H 10450 10134 50  0000 C CNN
F 1 "PMEG2005AEA115" H 10450 10225 50  0000 C CNN
F 2 "PMEG2005SOD323:PMEG2005" H 10450 10175 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMEG6010CEH_PMEG6010CEJ.pdf" H 10450 10350 50  0001 C CNN
	1    10450 10350
	-1   0    0    1   
$EndComp
$Comp
L Diode:PMEG6010CEJ D5
U 1 1 5DBA54FC
P 11150 10000
F 0 "D5" V 11104 10079 50  0000 L CNN
F 1 "PMEG2005AEA115" V 11195 10079 50  0000 L CNN
F 2 "PMEG2005SOD323:PMEG2005" H 11150 9825 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/PMEG6010CEH_PMEG6010CEJ.pdf" H 11150 10000 50  0001 C CNN
	1    11150 10000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8000 10450 8000 10350
Connection ~ 8000 10350
Wire Wire Line
	8000 10350 7900 10350
Text GLabel 6400 10900 2    50   Input ~ 0
SAMD21_RESET3.6A
$Comp
L SAMD21G18A:SAMD21G18A U1
U 1 1 5DA438CC
P 2850 6000
F 0 "U1" H 3825 6715 50  0000 C CNN
F 1 "SAMD21G18A" H 3825 6624 50  0000 C CNN
F 2 "SAMD21G18AGroot:samd21" H 2850 6000 50  0001 C CNN
F 3 "" H 2850 6000 50  0001 C CNN
	1    2850 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 5550 3100 5550
Wire Wire Line
	3100 5550 3100 5100
Wire Wire Line
	3100 5100 2900 5100
Wire Wire Line
	3200 5650 2900 5650
$Comp
L Device:Crystal Y1
U 1 1 5DA46759
P 2900 5250
F 0 "Y1" V 2500 5350 50  0000 L CNN
F 1 "Crystal 32 kHz - CM8V" V 2600 5050 50  0000 L CNN
F 2 "cyrstalCM8V:crystelCM8V" H 2900 5250 50  0001 C CNN
F 3 "~" H 2900 5250 50  0001 C CNN
	1    2900 5250
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 5400 2900 5650
$Comp
L Device:C C5
U 1 1 5DA46760
P 2500 5100
F 0 "C5" V 2248 5100 50  0000 C CNN
F 1 "12 pF" V 2339 5100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2538 4950 50  0001 C CNN
F 3 "~" H 2500 5100 50  0001 C CNN
	1    2500 5100
	0    1    1    0   
$EndComp
$Comp
L Device:C C6
U 1 1 5DA46766
P 2500 5650
F 0 "C6" V 2248 5650 50  0000 C CNN
F 1 "12 pF" V 2339 5650 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2538 5500 50  0001 C CNN
F 3 "~" H 2500 5650 50  0001 C CNN
	1    2500 5650
	0    1    1    0   
$EndComp
Wire Wire Line
	2650 5100 2900 5100
Connection ~ 2900 5100
Wire Wire Line
	2650 5650 2900 5650
Connection ~ 2900 5650
Wire Wire Line
	2350 5650 2350 5350
Text GLabel 2300 5350 0    50   Input ~ 0
GND
Wire Wire Line
	2350 5350 2300 5350
Connection ~ 2350 5350
Wire Wire Line
	2350 5350 2350 5100
NoConn ~ 3200 5750
NoConn ~ 3200 5850
Text GLabel 3200 5950 0    50   Input ~ 0
GND
$Comp
L Device:C C1
U 1 1 5DA55448
P 1350 6200
F 0 "C1" H 1465 6246 50  0000 L CNN
F 1 "100 nF" H 1465 6155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1388 6050 50  0001 C CNN
F 3 "~" H 1350 6200 50  0001 C CNN
	1    1350 6200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 5DA5544E
P 1850 6200
F 0 "C4" H 1965 6246 50  0000 L CNN
F 1 "10 µF" H 1965 6155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1888 6050 50  0001 C CNN
F 3 "~" H 1850 6200 50  0001 C CNN
	1    1850 6200
	1    0    0    -1  
$EndComp
Text GLabel 1350 6350 3    50   Input ~ 0
GND
Text GLabel 1850 6350 3    50   Input ~ 0
GND
Wire Wire Line
	1350 6050 1550 6050
$Comp
L Device:Ferrite_Bead FB1
U 1 1 5DA55457
P 1550 5900
F 0 "FB1" H 1687 5946 50  0000 L CNN
F 1 "BLM18PG471SN1" H 1687 5855 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 1480 5900 50  0001 C CNN
F 3 "~" H 1550 5900 50  0001 C CNN
	1    1550 5900
	1    0    0    -1  
$EndComp
Text GLabel 1550 5750 1    50   Input ~ 0
3V3
Wire Wire Line
	1850 6050 1550 6050
Connection ~ 1550 6050
Wire Wire Line
	1850 6050 3200 6050
Connection ~ 1850 6050
Text GLabel 1550 7050 1    50   Input ~ 0
3V3
Wire Wire Line
	3200 7150 1650 7150
Wire Wire Line
	1550 7150 1550 7050
$Comp
L Device:C C2
U 1 1 5DA721A9
P 1650 7300
F 0 "C2" H 1765 7346 50  0000 L CNN
F 1 "100nF" H 1765 7255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1688 7150 50  0001 C CNN
F 3 "~" H 1650 7300 50  0001 C CNN
	1    1650 7300
	1    0    0    -1  
$EndComp
Connection ~ 1650 7150
Wire Wire Line
	1650 7150 1550 7150
Text GLabel 1650 7450 3    50   Input ~ 0
GND
Text GLabel 3200 7250 0    50   Input ~ 0
GND
NoConn ~ 4450 7750
NoConn ~ 4450 7650
NoConn ~ 4450 7550
NoConn ~ 4450 7450
NoConn ~ 4450 7350
$Comp
L Device:R R7
U 1 1 5DA8AE01
P 6050 7150
F 0 "R7" V 5850 7100 50  0000 L CNN
F 1 "10k" V 5950 7050 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 5980 7150 50  0001 C CNN
F 3 "~" H 6050 7150 50  0001 C CNN
	1    6050 7150
	0    1    1    0   
$EndComp
Text GLabel 6200 7200 2    50   Input ~ 0
3V3
Wire Wire Line
	6200 7150 6200 7250
$Comp
L Device:R R8
U 1 1 5DA8AE09
P 6050 7250
F 0 "R8" V 6200 7200 50  0000 L CNN
F 1 "10k" V 6120 7205 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 5980 7250 50  0001 C CNN
F 3 "~" H 6050 7250 50  0001 C CNN
	1    6050 7250
	0    1    1    0   
$EndComp
Text GLabel 5650 7250 3    50   Input ~ 0
SDA
Text GLabel 5650 7150 1    50   Input ~ 0
SCL
Wire Wire Line
	4450 7150 5900 7150
Wire Wire Line
	4450 7250 5900 7250
Text GLabel 4450 7050 2    50   Input ~ 0
TARGET_USB_N
Text GLabel 4450 6950 2    50   Input ~ 0
TARGET_USB_P
Text GLabel 4450 6850 2    50   Input ~ 0
GND
$Comp
L Device:C C7
U 1 1 5DA9171C
P 5450 6750
F 0 "C7" V 5198 6750 50  0000 C CNN
F 1 "100 nF" V 5289 6750 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5488 6600 50  0001 C CNN
F 3 "~" H 5450 6750 50  0001 C CNN
	1    5450 6750
	0    1    1    0   
$EndComp
Wire Wire Line
	4450 6750 5300 6750
Text GLabel 5600 6750 2    50   Input ~ 0
GND
Text GLabel 5250 6750 1    50   Input ~ 0
3V3
Text GLabel 3200 6950 0    50   Input ~ 0
TXDofSAMD21
Text GLabel 3200 7050 0    50   Input ~ 0
RXDofSAMD21
Text GLabel 4450 6450 2    50   Input ~ 0
TXLED
Text GLabel 4450 6350 2    50   Input ~ 0
SAMD21_RESET3.6A
Text GLabel 4450 6150 2    50   Input ~ 0
GND
$Comp
L Device:C C10
U 1 1 5DA9E059
P 5850 6050
F 0 "C10" V 5598 6050 50  0000 C CNN
F 1 "100 nF" V 5689 6050 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5888 5900 50  0001 C CNN
F 3 "~" H 5850 6050 50  0001 C CNN
	1    5850 6050
	0    1    1    0   
$EndComp
Text GLabel 6000 6050 2    50   Input ~ 0
GND
Wire Wire Line
	4450 6050 5700 6050
Wire Wire Line
	4450 5950 5100 5950
Wire Wire Line
	5100 5950 5100 5600
Wire Wire Line
	5100 5600 5500 5600
$Comp
L Device:C C9
U 1 1 5DAA884A
P 5800 5600
F 0 "C9" V 5548 5600 50  0000 C CNN
F 1 "100 nF" V 5639 5600 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5838 5450 50  0001 C CNN
F 3 "~" H 5800 5600 50  0001 C CNN
	1    5800 5600
	0    1    1    0   
$EndComp
Text GLabel 5950 5600 2    50   Input ~ 0
GND
Text GLabel 4450 5850 2    50   Input ~ 0
SWCLK
Text GLabel 4450 5750 2    50   Input ~ 0
SWDIO
Text GLabel 4450 5550 2    50   Input ~ 0
RXLED
NoConn ~ 4450 5650
$Comp
L Device:LED D1
U 1 1 5DAB00FB
P 1700 2500
F 0 "D1" V 1739 2383 50  0000 R CNN
F 1 "LED" V 1648 2383 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1700 2500 50  0001 C CNN
F 3 "~" H 1700 2500 50  0001 C CNN
	1    1700 2500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 5DAB0101
P 1700 2100
F 0 "R2" H 1770 2146 50  0000 L CNN
F 1 "470" H 1770 2055 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1630 2100 50  0001 C CNN
F 3 "~" H 1700 2100 50  0001 C CNN
	1    1700 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 2250 1700 2350
Text GLabel 1700 1700 1    50   Input ~ 0
3V3
Wire Wire Line
	1700 1700 1700 1950
Text GLabel 1700 2900 3    50   Input ~ 0
RXLED
Wire Wire Line
	1700 2650 1700 2900
$Comp
L Device:LED D2
U 1 1 5DAB2D0D
P 2100 2500
F 0 "D2" V 2139 2383 50  0000 R CNN
F 1 "LED" V 2048 2383 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2100 2500 50  0001 C CNN
F 3 "~" H 2100 2500 50  0001 C CNN
	1    2100 2500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R3
U 1 1 5DAB2D13
P 2100 2100
F 0 "R3" H 2170 2146 50  0000 L CNN
F 1 "470" H 2170 2055 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 2030 2100 50  0001 C CNN
F 3 "~" H 2100 2100 50  0001 C CNN
	1    2100 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 2250 2100 2350
Text GLabel 2100 1700 1    50   Input ~ 0
3V3
Wire Wire Line
	2100 1700 2100 1950
Text GLabel 2100 2900 3    50   Input ~ 0
TXLED
Wire Wire Line
	2100 2650 2100 2900
Text GLabel 3200 6850 0    50   Input ~ 0
EXTINT9
Text GLabel 3200 6350 0    50   Input ~ 0
EXTINT4
Text GLabel 3200 6450 0    50   Input ~ 0
EXTINT5
Text GLabel 3200 6550 0    50   Input ~ 0
EXTINT6
Text GLabel 3200 6650 0    50   Input ~ 0
EXTINT7
Text GLabel 4450 7850 2    50   Input ~ 0
DEBUGLED
Text GLabel 3200 7850 0    50   Input ~ 0
RESETRN2483
Text GLabel 12000 6650 2    50   Input ~ 0
3V3
Text GLabel 10500 7150 2    50   Input ~ 0
TXDofSAMD21
Text GLabel 10500 7250 2    50   Input ~ 0
RXDofSAMD21
Wire Wire Line
	9750 5450 9750 5000
NoConn ~ 9450 5450
Text GLabel 3200 7650 0    50   Input ~ 0
EXTINT8
Wire Wire Line
	10600 10350 11150 10350
NoConn ~ 3200 7350
NoConn ~ 3200 7450
NoConn ~ 3200 7550
NoConn ~ 4450 6550
NoConn ~ 4450 6650
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J1
U 1 1 5DD10A67
P 5250 10650
F 0 "J1" H 5300 10967 50  0000 C CNN
F 1 "Conn_02x03_Odd_Even" H 5300 10876 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 5250 10650 50  0001 C CNN
F 3 "~" H 5250 10650 50  0001 C CNN
	1    5250 10650
	1    0    0    -1  
$EndComp
Text GLabel 5050 10550 0    50   Input ~ 0
3V3
Text GLabel 6350 10550 2    50   Input ~ 0
SWDIO
Text GLabel 6400 10650 2    50   Input ~ 0
SWCLK
Text GLabel 5050 10750 0    50   Input ~ 0
GND
Text GLabel 5050 10650 0    50   Input ~ 0
GND
NoConn ~ 8300 2450
NoConn ~ 4450 6250
$Comp
L Device:R R9
U 1 1 5DD9F598
P 6150 10900
F 0 "R9" V 6350 10900 50  0000 C CNN
F 1 "330" V 6250 10900 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 6080 10900 50  0001 C CNN
F 3 "~" H 6150 10900 50  0001 C CNN
	1    6150 10900
	0    1    1    0   
$EndComp
Wire Wire Line
	6300 10900 6400 10900
Wire Wire Line
	6000 10900 5550 10900
Wire Wire Line
	8350 10550 8350 10650
Connection ~ 8350 10550
Wire Wire Line
	8350 10550 8150 10550
$Comp
L Device:R R12
U 1 1 5DDA8B5C
P 8350 10800
F 0 "R12" H 8420 10846 50  0000 L CNN
F 1 "10k" H 8420 10755 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 8280 10800 50  0001 C CNN
F 3 "~" H 8350 10800 50  0001 C CNN
	1    8350 10800
	1    0    0    -1  
$EndComp
Text GLabel 8350 10950 3    50   Input ~ 0
GND
Text GLabel 5550 5200 1    50   Input ~ 0
3V3
$Comp
L tps7a05:TPS705 U2
U 1 1 5DB096CF
P 8600 10550
F 0 "U2" H 8825 11015 50  0000 C CNN
F 1 "TPS7A0533" H 8825 10924 50  0000 C CNN
F 2 "TPS7A05:TPS7A05" H 8600 10750 50  0001 C CNN
F 3 "" H 8600 10750 50  0001 C CNN
	1    8600 10550
	1    0    0    -1  
$EndComp
Text GLabel 8700 6450 0    50   Input ~ 0
3V3
Wire Wire Line
	10500 6650 11150 6650
$Comp
L Device:C C16
U 1 1 5DB1D2DA
P 11150 6800
F 0 "C16" H 11265 6846 50  0000 L CNN
F 1 "100 nF" H 11265 6755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 11188 6650 50  0001 C CNN
F 3 "~" H 11150 6800 50  0001 C CNN
	1    11150 6800
	1    0    0    -1  
$EndComp
Connection ~ 11150 6650
$Comp
L Device:C C17
U 1 1 5DB200E6
P 11750 6800
F 0 "C17" H 11865 6846 50  0000 L CNN
F 1 "10 uF" H 11865 6755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 11788 6650 50  0001 C CNN
F 3 "~" H 11750 6800 50  0001 C CNN
	1    11750 6800
	1    0    0    -1  
$EndComp
Text GLabel 11150 6950 3    50   Input ~ 0
GND
Text GLabel 11750 6950 3    50   Input ~ 0
GND
Connection ~ 11750 6650
Wire Wire Line
	11750 6650 12000 6650
Wire Wire Line
	11150 6650 11750 6650
Wire Wire Line
	5550 10550 5800 10550
$Comp
L Device:R R6
U 1 1 5DB355D7
P 5800 10400
F 0 "R6" H 5870 10446 50  0000 L CNN
F 1 "100k" H 5870 10355 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 5730 10400 50  0001 C CNN
F 3 "~" H 5800 10400 50  0001 C CNN
	1    5800 10400
	1    0    0    -1  
$EndComp
Connection ~ 5800 10550
Text GLabel 5800 10250 1    50   Input ~ 0
3V3
Wire Wire Line
	5800 10550 6350 10550
$Comp
L Device:R R5
U 1 1 5DC2BD3C
P 5700 10650
F 0 "R5" V 5900 10650 50  0000 C CNN
F 1 "0 ohm" V 5800 10650 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 5630 10650 50  0001 C CNN
F 3 "~" H 5700 10650 50  0001 C CNN
	1    5700 10650
	0    1    1    0   
$EndComp
Wire Wire Line
	5850 10650 6400 10650
Wire Wire Line
	5550 10750 5550 10900
Text GLabel 3200 6250 0    50   Input ~ 0
DEBUGRX
Text GLabel 3200 6150 0    50   Input ~ 0
DEBUGTX
$Comp
L Connector_Generic:Conn_01x03 J3
U 1 1 5DC02CF7
P 14100 9550
F 0 "J3" H 14180 9592 50  0000 L CNN
F 1 "Conn_01x03" H 14180 9501 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 14100 9550 50  0001 C CNN
F 3 "~" H 14100 9550 50  0001 C CNN
	1    14100 9550
	1    0    0    -1  
$EndComp
Text GLabel 13900 9450 0    50   Input ~ 0
DEBUGTX
Text GLabel 13900 9550 0    50   Input ~ 0
DEBUGRX
Text GLabel 13900 9650 0    50   Input ~ 0
GND
Text Notes 3400 8750 0    50   ~ 0
SERCOM0: UART RN\nSERCOM1: SPI\nSERCOM3: I2C\nSERCOM4: UART DEBUG
$Comp
L 3006.2114:3006.2114 S1
U 1 1 5DC1A9F7
P 2100 10550
F 0 "S1" H 2600 10815 50  0000 C CNN
F 1 "3006.2114" H 2600 10724 50  0000 C CNN
F 2 "TactileSwitchLed_30062114_footprint:30062114" H 2950 10650 50  0001 L CNN
F 3 "https://www.mouser.de/datasheet/2/467/Marquardt_007102018_Specifcation3006_Series-1379934.pdf" H 2950 10550 50  0001 L CNN
F 4 "Tactile Switches TACTILE" H 2950 10450 50  0001 L CNN "Description"
F 5 "4.15" H 2950 10350 50  0001 L CNN "Height"
F 6 "MARQUARDT" H 2950 10250 50  0001 L CNN "Manufacturer_Name"
F 7 "3006.2114" H 2950 10150 50  0001 L CNN "Manufacturer_Part_Number"
F 8 "979-3006.2114" H 2950 10050 50  0001 L CNN "Mouser Part Number"
F 9 "" H 2950 9950 50  0001 L CNN "Mouser Price/Stock"
F 10 "" H 2950 9850 50  0001 L CNN "RS Part Number"
F 11 "" H 2950 9750 50  0001 L CNN "RS Price/Stock"
	1    2100 10550
	1    0    0    -1  
$EndComp
Text GLabel 3100 10550 2    50   Input ~ 0
3V3
$Comp
L Device:R R4
U 1 1 5DC1D9BA
P 3450 10650
F 0 "R4" V 3657 10650 50  0000 C CNN
F 1 "1k" V 3566 10650 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 3380 10650 50  0001 C CNN
F 3 "~" H 3450 10650 50  0001 C CNN
	1    3450 10650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3100 10650 3300 10650
Text GLabel 3600 10650 2    50   Input ~ 0
DEBUGLED
$Comp
L Device:R R1
U 1 1 5DC28B93
P 1650 10400
F 0 "R1" H 1720 10446 50  0000 L CNN
F 1 "100k" H 1720 10355 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1580 10400 50  0001 C CNN
F 3 "~" H 1650 10400 50  0001 C CNN
	1    1650 10400
	1    0    0    -1  
$EndComp
Text GLabel 2100 10650 0    50   Input ~ 0
GND
Wire Wire Line
	1650 10550 2100 10550
Wire Wire Line
	1650 10550 1450 10550
Connection ~ 1650 10550
$Comp
L Device:C C3
U 1 1 5DC328CE
P 1650 10700
F 0 "C3" H 1765 10746 50  0000 L CNN
F 1 "100 nF" H 1765 10655 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 1688 10550 50  0001 C CNN
F 3 "~" H 1650 10700 50  0001 C CNN
	1    1650 10700
	1    0    0    -1  
$EndComp
Text GLabel 1650 10850 3    50   Input ~ 0
GND
Text GLabel 1650 10250 1    50   Input ~ 0
3V3
Text GLabel 1450 10550 0    50   Input ~ 0
SAMD21_RESET3.6A
NoConn ~ 3200 6750
NoConn ~ 3200 7750
Text GLabel 14200 2750 0    50   Input ~ 0
3V3
Text GLabel 14200 2650 0    50   Input ~ 0
GND
Text GLabel 14200 2550 0    50   Input ~ 0
EXTINT4
Text GLabel 14700 2550 2    50   Input ~ 0
VBAT
Text GLabel 14700 2650 2    50   Input ~ 0
SCL
Text GLabel 14700 2750 2    50   Input ~ 0
SDA
$Comp
L Connector_Generic:Conn_02x03_Top_Bottom J4
U 1 1 5DCD1F6B
P 14400 2650
F 0 "J4" H 14450 2967 50  0000 C CNN
F 1 "Conn_02x03_Top_Bottom" H 14450 2876 50  0000 C CNN
F 2 "ConnectorHorizontalPcb:ConnectorHorizontalPcb" H 14400 2650 50  0001 C CNN
F 3 "~" H 14400 2650 50  0001 C CNN
	1    14400 2650
	1    0    0    -1  
$EndComp
Text GLabel 14200 3750 0    50   Input ~ 0
3V3
Text GLabel 14200 3650 0    50   Input ~ 0
GND
Text GLabel 14200 3550 0    50   Input ~ 0
EXTINT5
Text GLabel 14700 3550 2    50   Input ~ 0
VBAT
Text GLabel 14700 3650 2    50   Input ~ 0
SCL
Text GLabel 14700 3750 2    50   Input ~ 0
SDA
$Comp
L Connector_Generic:Conn_02x03_Top_Bottom J5
U 1 1 5DCD6A0F
P 14400 3650
F 0 "J5" H 14450 3967 50  0000 C CNN
F 1 "Conn_02x03_Top_Bottom" H 14450 3876 50  0000 C CNN
F 2 "ConnectorHorizontalPcb:ConnectorHorizontalPcb" H 14400 3650 50  0001 C CNN
F 3 "~" H 14400 3650 50  0001 C CNN
	1    14400 3650
	1    0    0    -1  
$EndComp
Text GLabel 14200 4800 0    50   Input ~ 0
3V3
Text GLabel 14200 4700 0    50   Input ~ 0
GND
Text GLabel 14200 4600 0    50   Input ~ 0
EXTINT6
Text GLabel 14700 4600 2    50   Input ~ 0
VBAT
Text GLabel 14700 4700 2    50   Input ~ 0
SCL
Text GLabel 14700 4800 2    50   Input ~ 0
SDA
Text GLabel 14200 5800 0    50   Input ~ 0
3V3
Text GLabel 14200 5700 0    50   Input ~ 0
GND
Text GLabel 14700 5600 2    50   Input ~ 0
VBAT
Text GLabel 14700 5700 2    50   Input ~ 0
SCL
Text GLabel 14700 5800 2    50   Input ~ 0
SDA
$Comp
L Connector_Generic:Conn_02x03_Top_Bottom J7
U 1 1 5DCDD804
P 14400 5700
F 0 "J7" H 14450 6017 50  0000 C CNN
F 1 "Conn_02x03_Top_Bottom" H 14450 5926 50  0000 C CNN
F 2 "ConnectorHorizontalPcb:ConnectorHorizontalPcb" H 14400 5700 50  0001 C CNN
F 3 "~" H 14400 5700 50  0001 C CNN
	1    14400 5700
	1    0    0    -1  
$EndComp
Text GLabel 14200 6800 0    50   Input ~ 0
3V3
Text GLabel 14200 6700 0    50   Input ~ 0
GND
Text GLabel 14200 6600 0    50   Input ~ 0
EXTINT8
Text GLabel 14700 6600 2    50   Input ~ 0
VBAT
Text GLabel 14700 6700 2    50   Input ~ 0
SCL
Text GLabel 14700 6800 2    50   Input ~ 0
SDA
$Comp
L Connector_Generic:Conn_02x03_Top_Bottom J8
U 1 1 5DCE201F
P 14400 6700
F 0 "J8" H 14450 7017 50  0000 C CNN
F 1 "Conn_02x03_Top_Bottom" H 14450 6926 50  0000 C CNN
F 2 "ConnectorHorizontalPcb:ConnectorHorizontalPcb" H 14400 6700 50  0001 C CNN
F 3 "~" H 14400 6700 50  0001 C CNN
	1    14400 6700
	1    0    0    -1  
$EndComp
Text GLabel 14200 7800 0    50   Input ~ 0
3V3
Text GLabel 14200 7700 0    50   Input ~ 0
GND
Text GLabel 14200 7600 0    50   Input ~ 0
EXTINT9
Text GLabel 14700 7600 2    50   Input ~ 0
VBAT
Text GLabel 14700 7700 2    50   Input ~ 0
SCL
Text GLabel 14700 7800 2    50   Input ~ 0
SDA
$Comp
L Connector_Generic:Conn_02x03_Top_Bottom J9
U 1 1 5DCE64C0
P 14400 7700
F 0 "J9" H 14450 8017 50  0000 C CNN
F 1 "Conn_02x03_Top_Bottom" H 14450 7926 50  0000 C CNN
F 2 "ConnectorHorizontalPcb:ConnectorHorizontalPcb" H 14400 7700 50  0001 C CNN
F 3 "~" H 14400 7700 50  0001 C CNN
	1    14400 7700
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x03_Top_Bottom J6
U 1 1 5DCDAC02
P 14400 4700
F 0 "J6" H 14450 5017 50  0000 C CNN
F 1 "Conn_02x03_Top_Bottom" H 14450 4926 50  0000 C CNN
F 2 "ConnectorHorizontalPcb:ConnectorHorizontalPcb" H 14400 4700 50  0001 C CNN
F 3 "~" H 14400 4700 50  0001 C CNN
	1    14400 4700
	1    0    0    -1  
$EndComp
Text GLabel 14200 5600 0    50   Input ~ 0
EXTINT7
$Comp
L Device:C C8
U 1 1 5DD6DF27
P 5800 5200
F 0 "C8" V 5548 5200 50  0000 C CNN
F 1 "10 µF" V 5639 5200 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5838 5050 50  0001 C CNN
F 3 "~" H 5800 5200 50  0001 C CNN
	1    5800 5200
	0    1    1    0   
$EndComp
Text GLabel 5950 5200 2    50   Input ~ 0
GND
Wire Wire Line
	5650 5200 5500 5200
Wire Wire Line
	5500 5200 5500 5600
Connection ~ 5500 5600
Wire Wire Line
	5500 5600 5650 5600
Wire Notes Line
	500  11200 11800 11200
Wire Notes Line
	11800 9950 16050 9950
Wire Notes Line
	16050 9950 16050 500 
Wire Notes Line
	16050 500  500  500 
Wire Notes Line
	500  500  500  11200
Wire Notes Line
	4400 9450 4400 11200
Text Notes 1300 9800 0    79   ~ 0
Reset button and debug led
Text Notes 5300 9750 0    79   ~ 0
SWD connector
Wire Notes Line
	7500 9450 7500 11200
Wire Wire Line
	11150 10150 11150 10350
Wire Wire Line
	11150 10350 11450 10350
Connection ~ 11150 10350
Wire Wire Line
	11150 9750 11150 9850
Text Notes 9000 9750 0    79   ~ 0
Powermanagement
Text Notes 13500 9150 0    79   ~ 0
UART connector
Text Notes 13950 1600 0    79   ~ 0
Sensor connectors
Wire Notes Line
	500  9450 12950 9450
Wire Notes Line
	11800 11200 11800 9950
Wire Notes Line
	12950 500  12950 9950
Wire Notes Line
	12950 8650 16050 8650
Wire Notes Line
	7000 9450 7000 4000
Text Notes 3450 4350 0    79   ~ 0
ATSAMD21 chip
Wire Notes Line
	500  4000 12950 4000
Wire Notes Line
	3550 500  3550 4000
Text Notes 1800 1050 0    79   ~ 0
LED
Text Notes 7700 1000 0    79   ~ 0
Micro USB connector
Text Notes 9150 4350 0    79   ~ 0
RN2483 Lora chip
$Comp
L Device:Antenna AE1
U 1 1 5DB3227F
P 9300 4750
F 0 "AE1" H 9380 4739 50  0000 L CNN
F 1 "Antenna" H 9380 4648 50  0000 L CNN
F 2 "helixAntenna:helixAntenna" H 9300 4750 50  0001 C CNN
F 3 "~" H 9300 4750 50  0001 C CNN
	1    9300 4750
	1    0    0    -1  
$EndComp
$Comp
L UFL_antenna:UFL_Antenna U4
U 1 1 5E456AD8
P 9950 4650
F 0 "U4" H 10428 4688 50  0000 L CNN
F 1 "UFL_Antenna" H 10428 4597 50  0000 L CNN
F 2 "UFLConn:U.FL_Molex_MCRF_73412-0110_Vertical" H 9950 4650 50  0001 C CNN
F 3 "" H 9950 4650 50  0001 C CNN
	1    9950 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 4950 9300 5000
Wire Wire Line
	9300 5000 9750 5000
Wire Wire Line
	9750 5000 10200 5000
Connection ~ 9750 5000
Text GLabel 10300 5000 3    50   Input ~ 0
GND
$EndSCHEMATC
