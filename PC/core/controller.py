import serial
import time

class Controller:
    def __init__(self, port: str):
        try:
            self.ser = serial.Serial(port, 115200, timeout=1)
            print(f"Connected to {port}")
        except serial.SerialException as e:
            print(f"Error opening serial port: {e}")
            self.ser = None

    def send_and_receive(self, message: str) -> str:
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
        return response
    
    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
    
    def setLEDS(self, num :int) -> bool:
        return (self.send_and_receive("LI "+str(num)) == "LIT")
    
    def writeToLCD(self, line1 :str, line2 :str) -> bool:
        return (self.send_and_receive("WR "+line1+";"+line2) == "DONE")

# Example usage

con = Controller("COM4")
con.setLEDS(255)
con.writeToLCD("Philasande","Ngubo")
con.close()
