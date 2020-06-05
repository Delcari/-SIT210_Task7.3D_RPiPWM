import RPi.GPIO as GPIO
import time
import math
 
from multiprocessing import Process, Value

requestedDC = Value('i', 0)

GPIO.setmode(GPIO.BOARD)

#trig pin 11 
GPIO.setup(11, GPIO.OUT)
#echo pin
GPIO.setup(13, GPIO.IN)

#Approaches the Target PWM
def targetPWM():
    #PWM Setup
    GPIO.setup(7, GPIO.OUT)
    buzzer = GPIO.PWM(7, 50)
    buzzer.start(0)

    currentDC = 0
    while (True):
        currentReq = requestedDC.value
        #Increment or Decrement
        increment = 1
        if (currentDC > currentReq):
            increment = -1
        #Loop through until to Current Req
        for i in range(currentDC, currentReq, increment):
            #Change the duty cycle
            buzzer.ChangeDutyCycle(i)
            currentDC = i 
            time.sleep(0.1)
            #If the target changes, return to the beginning
            if (not requestedDC.value == currentReq):
                break



#Reads distance values from the sensor
def ReadDistance():
    time.sleep(0.05)

    #Trigger the Trig PIN
    GPIO.output(11, True)
    time.sleep(0.001)
    GPIO.output(11, False)

    #Wait until input is LOW
    while GPIO.input(13) == 0:
        pulseStart = time.time()
    
    #Wait until input is High
    while GPIO.input(13) == 1:
        pulseEnd = time.time()

    #FInd the elapsed time
    duration = pulseEnd - pulseStart
    #Signal transmitted then bounces back and returns therefore /2
    #Speed of sound 343m/s
    distance = duration * (34300/2)

    #Convert to value from 0-100
    #Assuming Max range = 4m
    #Min range = 2cm
    return int((math.ceil(distance/4)))




try:
    #Start process for PWM
    p = Process(target=targetPWM, args=())
    p.start()
    while (True):
        time.sleep(0.5)
        distance = ReadDistance()
        #If reading valid
        if (distance <= 100 and distance >= 0):
            #Update the target dutycycle
            requestedDC.value = distance
except KeyboardInterrupt:
    GPIO.cleanup()
