import serial
import matplotlib.pyplot as plt
import numpy
from drawnow import *


Pitch1 = []
Rol1 = []
Pitch2 = []
Rol2 = []

count1 = 0
count2 = 0

Com3 = serial.Serial('COM7', 115200)
plt.ion()




def makefig1():
	
	fig = plt.figure(1)
	a = plt.subplot(111)
	a.axis([-1.2,1.2,-1.2,1.2])
	#plot(Pitch1,Rol1, "bo-", Pitch2, Rol2, "yo-")
	a.set_xlabel('Pitch')
	a.set_ylabel('Roll')
	a.set_title('Stability')
	a.plot(Pitch1,Rol1, "bo-", Pitch2, Rol2, "yo-")

	CircleSmall = fig.add_subplot(1,1,1)
	CircleBig = fig.add_subplot(1,1,1)
	circ1 = plt.Circle((0,0), radius=0.2, color = 'g', fill=False )
	circ2 = plt.Circle((0,0), radius=0.9, color = 'r', fill=False )
	CircleSmall.add_patch(circ1)
	CircleBig.add_patch(circ2)





while True:
	while (Com3.inWaiting()==0):
		pass


	Com3string = Com3.readline()
	Com3splitted = Com3string.split(",")
	print Com3splitted

	if Com3splitted[0] == "\r1":
		Pitch1.append(Com3splitted[1])
		Rol1.append(Com3splitted[2])
		drawnow(makefig1)
		count1 += 1
		if count1 >= 3:
			Pitch1.pop(0)
			Rol1.pop(0)

	if Com3splitted[0] =="\r2":
		Pitch2.append(Com3splitted[1])
		Rol2.append(Com3splitted[2])
		drawnow(makefig1)
		count1 += 1
		if count1 >= 3:
			Pitch2.pop(0)
			Rol2.pop(0)