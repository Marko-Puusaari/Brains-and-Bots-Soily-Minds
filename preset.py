import serial
import time
arduino = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
def output():
	print(arduino.readline().decode('utf-8').strip())
def pause():
	time.sleep(4) #for easy testing, being able to change all pauses at once

arduino.write(b'ON\n')
pause()
output()

arduino.write(b'ARM_UP\n')
pause()
output()

arduino.write(b'ARM_DOWN\n')
pause()
output()

arduino.write(b'SENSE_WATER\n')
pause()
output()

arduino.write(b'OFF\n')
pause()
output()
pause()
output()
