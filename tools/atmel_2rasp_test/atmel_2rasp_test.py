#import serial
#import os

#files = []
#path = '/dev/serial/by-id/'
#for (dirpath, dirnames, filenames) in os.walk(path):
#  files.extend(filenames)
#  break
#fileNameAndPath = path + files[0]
#print(fileNameAndPath)

#ser = serial.Serial(fileNameAndPath, 115200, timeout=1)
#ser.flush()
#while True:
#	if ser.in_waiting > 0:
#		line = ser.read()
#		print(line)




while True: #Loop para a conexão com o Arduino
    try:  #Tenta se conectar, se conseguir, o loop se encerra
        arduino = serial.Serial('COM4', 9600)
        print('Arduino conectado')
        break
    except:
        pass
while True: #Loop principal
    msg = str(arduino.readline()) #Lê os dados em formato de string
    msg = msg[2:-5] #Fatia a string
    print(msg) #Imprime a mensagem
    arduino.flush() #Limpa a comunicação
