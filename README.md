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

<img style ="opacity: 0.2" width="202" height="50" alt="image" src="https://github.com/user-attachments/assets/d2dbb1b9-1c76-41cd-8c17-612204b65e6c" />

## <img width="24" height="24" alt="6201651" src="https://github.com/user-attachments/assets/9c21d04f-181f-4771-bf23-1f5e38372625" /> FROM STM32 to PC

- Pushbutton pressed<br/>
  The stm sends a message to the pc
  
      BTN<SW0 number>
  e.g if SW3 is pressed the stm will send *BTN3* to the uart com
- Potentiometer turned
   The stm sends a message to the pc
  
      POT<Pot number i.e 0 or 1> <percentage turned>
  e.g if POT0 is turned halfway the stm will send *POT0 50* to the uart com




  






