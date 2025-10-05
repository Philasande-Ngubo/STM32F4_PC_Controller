import serial.tools.list_ports
import serial
import time

BAUD_RATE = 115200

def controllers():
    ports = serial.tools.list_ports.comports()
    controllers = []
    for port in ports:
        
        ser = serial.Serial(port.device, BAUD_RATE, timeout=1)
        if (send_and_receive(ser,"UP\r\n") == "YES"):
            temp = []
            temp.append(port.device)
            temp.append(port.description)
            controllers.append(temp)
    return controllers
        
def send_and_receive(ser,message):
    # Send message (encode string to bytes)
    ser.write(message.encode('utf-8'))
    
    # Optional: flush output
    ser.flush()
    
    # Wait a short time for response
    time.sleep(0.1)
    
    # Read response
    response = ser.readline().decode('utf-8').strip()  # strip removes \r\n
    return response    



