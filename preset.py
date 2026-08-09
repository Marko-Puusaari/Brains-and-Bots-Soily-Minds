
import serial
import time
arduino = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)
def output():
	print(arduino.readline().decode('utf-8').strip())
def pause():
	time.sleep(5) #for easy testing, being able to change all pauses at once

arduino.write(b'ARM_UP\n')
pause()
output()

arduino.write(b'ARM_DOWN\n')
pause()
output()

arduino.write(b'STEP_FORWARDS\n')
pause()
output()

arduino.write(b'QUARTER_TURN_LEFT')
pause()
output()

arduino.write(b'QUARTER_TURN_RIGHT')
pause()
output()

arduino.write(b'ARM_DOWN')
pause()
output()

arduino.write(b'SENSE_WATER\n')
pause()
output()

arduino.write(b'ARM_UP\n')
pause()
output()

arduino.write(b'OFF\n')
pause()
output()
output()
