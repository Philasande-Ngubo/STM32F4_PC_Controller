import serial
import time
import threading
import random

class Controller:
    def __init__(self, port: str):
        try:
            self.ser = serial.Serial(port, 115200, timeout=1)
            self.send_and_receive("HI")
        except serial.SerialException as e:
            print(f"Error opening serial port: {e}")
            self.ser = None
            self.CONNECTED = False
        
        self._listen = False
        self._listener_thread = None
        self.CONNECTED = True

    def send_and_receive(self, message: str) -> str:
        self._listen = False
        if not self.ser:
            return ""
        
        # Ensure command ends with newline
        if not message.endswith('\r\n'):
            message += '\r\n'
        
        # Send message
        self.ser.write(message.encode('utf-8'))
        self.ser.flush()
        
        # Wait for MCU response
        time.sleep(0.1)
        
        # Read response
        response = self.ser.readline().decode('utf-8').strip()
        
        self._listen =True
        return response

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
    
    
    def setLEDS(self, num :int) -> bool:
        return (self.send_and_receive("LI "+str(num)) == "LIT")
    
    def writeToLCD(self, line1 :str, line2 :str) -> bool:
        return (self.send_and_receive("WR "+line1+";"+line2) == "DONE")
    
    def waitForMessages(self):
        try:
            while self._listen:
                response = self.ser.readline().decode('utf-8').strip()
                if (response):
                    print(response)
        except():
            self.CONNECTED = False
                
    def listening_Mode(self):
        self._listening = True
        self._listener_thread = threading.Thread(target=self.waitForMessages, daemon=True)
        self._listener_thread.start()
    
    def sendingMode(self):
        self._listening = False
        self._listening = False
        self._listening = False
        self._listener_thread = None
        self._listener_thread = None
        self._listener_thread = None
             

# Example usage

con = Controller("COM4")
con.setLEDS(255)
con.writeToLCD("Philasande","Ngubo")

while (True):
    time.sleep(random.random())
    con.setLEDS(random.randrange(0, 256))

con.close()