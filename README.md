# ![clarify_24dp_E3E3E3_FILL0_wght400_GRAD0_opsz24](https://github.com/user-attachments/assets/c8a00147-c714-4978-8867-879944a7f852) Protocol for STM32F446RTX

Me and my friend <a href="https://github.com/ethane-magubane">Ethane Magubane</a> were tasked with building a simple Dongle lock to share codes between the PC and the STM, after a mistake f not reading the whole the
practical requirements we made a whole protocol that communicates with the PC and send allmost everything to the dongle this can be used to test the STM and UART connection.

![499581631508302795](https://github.com/user-attachments/assets/2c3a14f3-43fb-4a9f-90c9-43612ec406b5)

<br/>

## <img width="24" height="24" alt="16319418" src="https://github.com/user-attachments/assets/e913d363-9611-4bd8-9982-f0ae4fedfefc" /> Loading the program to stm32 

There are couple of options one may choose from but the program source code is in _Controller/_

- Use the <a href= "https://www.st.com/en/development-tools/stm32cubeide.html">STM32CUBIDE</a>
* Use Visual studio code (see <a href= "https://zenembed.com/vscode-cubemx-guide">setting up vscode for STM32</a>)
+ Or your convenient way of running stm32 programs

<br/>

## <img width="24" height="24" alt="18967516" src="https://github.com/user-attachments/assets/d35eabf1-aa29-4f70-b6a1-1bcdfb5e9ac8" /> The Protocol

Perhaps before explaining the protocol it is best I also show the schematic of the STM32F446RTCx

<img width="413" height="308" alt="image" src="https://github.com/user-attachments/assets/22cc996f-1c6a-44c4-a1e8-63e1ff55fca7" />

<img width="669" height="273" alt="image" src="https://github.com/user-attachments/assets/fc79840f-0ce4-4f74-a889-974617537a96" />

 
### <img width="20" height="20" alt="images" src="https://github.com/user-attachments/assets/e045468e-aff0-42ca-b2cf-e7c0949240e4" /> Potentiometers
There are two potentiometers namely 
- POT0
- POT1

### <img width="20" height="20" alt="5735377" src="https://github.com/user-attachments/assets/0bd06e8f-1104-41a3-ae07-579b82dd0476" /> PushButtons
There are four programmable switch buttons
- SW0
- SW1
- SW2
- SW3

### <img width="20" height="20" alt="2231268" src="https://github.com/user-attachments/assets/252a7dab-794b-443a-bdf2-8a055e649e85" /> LED STRIP

The LED strip consists of 8 Leds numbered 0 to 7

<img style ="opacity: 0.2" width="202" height="50" alt="image" src="https://github.com/user-attachments/assets/d2dbb1b9-1c76-41cd-8c17-612204b65e6c" /><br/><br/>

##  <img width="24" height="24" alt="3430375" src="https://github.com/user-attachments/assets/513deb21-64fc-4855-af74-ea0cee898e1f" /> FROM STM32 to PC

- Pushbutton pressed<br/>
  The stm sends a message to the pc
  
      BTN<SW0 number>
  e.g if SW3 is pressed the stm will send *BTN3* to the uart com
- Potentiometer turned
   The stm sends a message to the pc
  
      POT<Pot number i.e 0 or 1> <percentage turned>
  e.g if POT0 is turned halfway the stm will send *POT0 50* to the uart com<br/><br/><br/>

## <img width="24" height="24" alt="4186598" src="https://github.com/user-attachments/assets/c8b86d35-466e-448b-8962-71f170df37cc" /> FROM STM32 to PC

- <img width="20" height="20" alt="18536728" src="https://github.com/user-attachments/assets/c5b1e3c9-4a87-4209-9a0e-d6430bde5d27" /> **Handshake** <br/>
  The PC sends the *HI* message to the  to the STM
  
      HI
  the STM must respond with a HEY
  
      HEY
  To recognise the connection request there and then the handshake is done<br/><br/>

  

- <img width="20" height="20" alt="9968180" src="https://github.com/user-attachments/assets/0d1fafa4-5ca7-4f4a-86dc-b3e64b90aa59" /> **PING** <br/>
  The PC sends the *UP* message to the  to the STM
  
      UP
  the STM must respond with a HEY
  
      YES
  There the PC knows the STM is still available<br/><br/>


- <img width="20" height="20" alt="908792" src="https://github.com/user-attachments/assets/2dd67cf3-a463-4f0a-9469-e0d6c49b3bbd" />   **Write to LCD** <br/>
  The PC sends the *WR <LINE1>;<LINE2>* message to the  to the STM
  
      WR <LINE1>;<LINE2>
  the STM must respond with a *DID* after ensuring that it has written to the LCD
  
      YES
  e.g sending a *WR HELLO;WORLD* will write a *Hello* on the first line of the lcd and a *World* on the Second screen of the lcd<br/><br/>

- <img width="20" height="20" alt="2338802" src="https://github.com/user-attachments/assets/5d13a932-3362-4841-a126-40bc2e971d6d" />  **Write to the LED strip** <br/>
  The PC sends the *LI <NUM>* message to the  to the STM
  
      LI <NUM>
  the STM must respond with a *LIT* after ensuring that it has written to the LED strip
  
      LIT
   i.e sending a LI 0 will turn off all the lights while sending LI 255 will light all the light so the way this works is the they are eight LED represented in binary number 0b00000000 each bit represents a lit LED unit so the number you can send is bounded between 0 and 225 inclusive any number in this range will be valid. e.g to light the first and light LED the targert is 0b10000001 for which the decimal equavalent is 129 so LI 129 will achieve the desired effect.<br/><br/>

- <img width="20" height="20" alt="5334705" src="https://github.com/user-attachments/assets/81806352-c669-493a-a46d-2bb9d9b5df36" /> **Ending the Session** <br/>
  The PC sends the *ES* message to the  to the STM
  
      ES
  the STM must respond with a SHO 
  
      SHO
  There the PC knows the STM was disconnected gracefully

<br/><br/>

## <img width="24" height="24" alt="11152927" src="https://github.com/user-attachments/assets/f97d1614-dc18-4132-b206-0091ea60e0b6" />   COMMUNICATING

I have written a small python program to communicate with the STM which is on *PC/* which is simple but is not exhaustive  you will need to have **pySerial, PyQt5** installed

    pip install PyQt5
    pip install pyserial
 The in order to Run the program just run a 

    python3 Main.py
<img height="250" alt="image" src="https://github.com/user-attachments/assets/fb41768a-79c0-4650-9a91-16f112c9cd95" />
<br/><br/>

So now it will automatically detect if the an stm is connected Howerver I recommend you use a more robust serial tester that will allow flexibity

- Advanced Serial Port Monitor (download <a href = "https://www.aggsoft.com/serial-port-monitor/download.htm">Here</a>)
- COM Port Monitoring (download <a href = "https://www.aggsoft.com/serial-port-monitor/download.htm](https://www.com-port-monitoring.com/serial-port-terminal/">Here</a>)
- COM Port Monitoring (download <a href = "https://advanced-serial-port-monitor.software.informer.com/">Here</a>)
- Or any other COM port monitor

## <img width="24" height="24" alt="12695920" src="https://github.com/user-attachments/assets/7deba80a-c036-4f6b-abee-d99e90235176" /> Tips
<br/><br/>

- If your PC cannot detect the COM download the port driver <a href="https://www.silabs.com/interface/usb-bridges/classic/device.cp2102?tab=specs">download </a> the driver and update it(see more on<a href="https://support.microsoft.com/en-us/windows/update-drivers-through-device-manager-in-windows-ec62f46c-ff14-c91d-eead-d7126dc1f7b6">updating drivers</a>)

- Write your own programs to test and more functionality



> Feel free to contact me on Email to query about the code 


