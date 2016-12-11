import serial
import matplotlib.pyplot as plt
import numpy
from drawnow import *


x_values1 = []
y_values1 = []
x_values2 = []
y_values2 = []

count1 = 0
count2 = 0

Com3 = serial.Serial('COM6', 115200)
plt.ion()





def makefig1():
	plt.figure(1,dpi=72)
	
	a = plt.subplot(211)
	a.axis([-1,25,-20,20])
	a.set_title('Velocity Waiter 1')
	a.set_xlabel('Plot')
	a.set_ylabel('V')
	#a.set_ylim([-0.2,0.2])
	a.plot(x_values1, 'ro-', y_values1, 'bs-')

	b = plt.subplot(212)
	b.axis([-1,25,-20,20])
	b.set_title('Velocuty Waiter 2')
	b.set_xlabel('Plot')
	b.set_ylabel('V')
	b.plot(x_values2, 'go-', y_values2, 'ys-')
	plt.tight_layout()


while True:
	while (Com3.inWaiting()==0):
		pass
	
	Com3string = Com3.readline()
	Com3splitted = Com3string.split(",")
	print Com3splitted


	if Com3splitted[0] == "\r1":
		#print "1", Com3splitted[3], Com3splitted[4]
		x_values1.append(Com3splitted[3])
		y_values1.append(Com3splitted[4])
		drawnow(makefig1)

	 	count1 +=1
	 	if count1 >= 25:
		 	x_values1.pop(0)
		 	y_values1.pop(0)


	if Com3splitted[0] == "\r2":
		#print "2--" ,Com3splitted[3], Com3splitted[4]
		x_values2.append(Com3splitted[3])
		y_values2.append(Com3splitted[4])
		drawnow(makefig1)
		count2 +=1
	 	if count2 >= 25:
		 	x_values2.pop(0)
		 	y_values2.pop(0)

	plt.pause(.000001)

