#include "tm4c.h"
#include <stdbool.h>
#include <stdint.h>

#define GPIO_PORTA 0x01  // Port A
  volatile bool gui_signal = false; // Stores the last signal from the GUI

void SysTick_Wait1ms(void) {

    NVIC_ST_RELOAD_R = 16000 - 1;
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_CTRL_R = 0x5;
  
    while((NVIC_ST_CTRL_R & 0x00010000) == 0) {
        
    }
}

void SysTick_Wait(int ms) {
    for (int i = 0; i < ms; i++) {
        SysTick_Wait1ms();
    }
}


////////////switch/////////

void Init_PA2(){
  
    // Enable clock for Port A
    SYSCTL_RCGCGPIO_R |= GPIO_PORTA;
    
    // Allow time for clock to stabilize
    while ((SYSCTL_RCGCGPIO_R & GPIO_PORTA) == 0) {}
    
    GPIO_PORTA_LOCK_R = 0x4C4F434B; // Write unlock key to GPIOLOCK
  
    GPIO_PORTA_CR_R |= 0x04; // Allow changes to PA2 (0x04 = 00000100)
    
    // Set PA2 as output (Relay Control)
    GPIO_PORTA_DIR_R |= 0x04;    // Set PA2 as output (0x04 = 00000100)
    GPIO_PORTA_DEN_R |= 0x04;    // Enable digital function for PA2
}

void Enable_PA2(){
  GPIO_PORTA_DATA_R |= 0x04;
}

void Disable_PA2(){
  
  GPIO_PORTA_DATA_R &= ~0x04;
  
}

void Init_PA4(){
  
    // Enable clock for Port A
    SYSCTL_RCGCGPIO_R |= GPIO_PORTA;
    
    // Allow time for clock to stabilize
    while ((SYSCTL_RCGCGPIO_R & GPIO_PORTA) == 0) {}
    
    GPIO_PORTA_LOCK_R = 0x4C4F434B; // Write unlock key to GPIOLOCK
    
    // Set PA4 as input (DIP Switch) with pull-up
    GPIO_PORTA_DIR_R &= ~0x10;   // Set PA4 as input (0x10 = 00010000)
    GPIO_PORTA_DEN_R |= 0x10;    // Enable digital function for PA4
    GPIO_PORTA_PUR_R |= 0x10;    // Enable pull-up resistor for PA4
}

void Init_PC5() {
    // Enable clock for Port C
    SYSCTL_RCGCGPIO_R |= 0x04; // 0x04 corresponds to Port C

    // Allow time for clock to stabilize
    while ((SYSCTL_RCGCGPIO_R & 0x04) == 0) {}

    // Unlock Port C
    GPIO_PORTC_LOCK_R = 0x4C4F434B; // Write unlock key to GPIOLOCK
    GPIO_PORTC_CR_R |= 0x20;        // Allow changes to PC5 (0x20 = 00100000)

    // Set PC5 as output
    GPIO_PORTC_DIR_R |= 0x20;   // Set PC5 as output (0x20 = 00100000)
    GPIO_PORTC_DEN_R |= 0x20;   // Enable digital function for PC5

}

void Enable_PC5(){
  GPIO_PORTC_DATA_R |= 0x20;  // Turn PC5 on
}

void Disable_PC5() {
    GPIO_PORTC_DATA_R &= ~0x20; // Turn off PC5 (0x20 = 00100000)
}


/////////////////////////////


///////////magnetic switch/////////


void Init_PB3() {
    // Enable clock for Port B
    SYSCTL_RCGCGPIO_R |= 0x02;  // Port B

    // Allow time for clock to stabilize
    while ((SYSCTL_RCGCGPIO_R & 0x02) == 0) {}

    // Unlock Port B
    GPIO_PORTB_LOCK_R = 0x4C4F434B;  // Write unlock key to GPIOLOCK
    GPIO_PORTB_CR_R |= 0x08;         // Allow changes to PB3 (0x08 = 00001000)

    // Set PB3 as input
    GPIO_PORTB_DIR_R &= ~0x08;  // Set PB3 as input (0x08 = 00001000)
    GPIO_PORTB_DEN_R |= 0x08;   // Enable digital function for PB3
    GPIO_PORTB_PUR_R |= 0x08;   // Enable pull-up resistor for PB3 (Optional for stable input)
}


bool read_magnetic_switch() {
    return (GPIO_PORTB_DATA_R & 0x08) >> 3;  // Read PB3 status
}

/////////////////////////////////////////////





///////////////////////temp////////////////////

// Initialize ADC0
void ADC0_Init(void) {
    SYSCTL_RCGCADC_R |= 1;        // Enable clock to ADC0
    SYSCTL_RCGCGPIO_R |= 0x10;    // Enable clock to PORTE
while ((SYSCTL_PRGPIO_R & 0x10) == 0) {} 
while ((SYSCTL_PRADC_R & 0x01) == 0) {}

    GPIO_PORTE_AFSEL_R |= 0x08;   // Enable alternate function on PE3
    GPIO_PORTE_DEN_R &= ~0x08;    // Disable digital function on PE3
    GPIO_PORTE_AMSEL_R |= 0x08;   // Enable analog function on PE3
    ADC0_ACTSS_R &= ~8;           // Disable sample sequencer 3
    ADC0_EMUX_R &= ~0xF000;       // Software trigger conversion
    ADC0_SSMUX3_R = 0;            // Get input from channel 0 (AIN0/PE3)
    ADC0_SSCTL3_R = 0x06;         // Take one sample at a time, set flag at 1st sample
    ADC0_ACTSS_R |= 8;            // Enable sample sequencer 3
}

// Read ADC value
int ADC0_Read(void) {
    ADC0_PSSI_R = 8;              // Start conversion on sequencer 3
    while ((ADC0_RIS_R & 8) == 0); // Wait for conversion to complete
    int result = ADC0_SSFIFO3_R; // Read the ADC value
    ADC0_ISC_R = 8;               // Clear the completion flag
    return result;
}

// Convert ADC value to temperature
int ConvertToTemperature(int adc_value) {
    int voltage_mV = (adc_value * 5000) / 4096; // Convert to millivolts (scale up by 1000)
    return voltage_mV / 10; // Convert millivolts to temperature in °C
}



// Initialize PA5 as output for buzzer
void Buzzer_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x01; // Enable clock for Port A
    GPIO_PORTA_DIR_R |= 0x20;  // Set PA5 as output
    GPIO_PORTA_DEN_R |= 0x20;  // Enable digital function for PA5
}

// Turn buzzer on
void Buzzer_On(void) {
    GPIO_PORTA_DATA_R |= 0x20; // Set PA5 HIGH
}

// Turn buzzer off
void Buzzer_Off(void) {
    GPIO_PORTA_DATA_R &= ~0x20; // Set PA5 LOW
}

// Initialize UART0 for communication
void UART0_Init(void) {
    SYSCTL_RCGCUART_R |= 0x01;        // Enable clock for UART0
    SYSCTL_RCGCGPIO_R |= 0x01;        // Enable clock for Port A

    // Set PA0 and PA1 for UART0
    GPIO_PORTA_AFSEL_R |= 0x03;       // Enable alternate functions on PA0 and PA1
    GPIO_PORTA_DEN_R |= 0x03;         // Enable digital functions on PA0 and PA1
    GPIO_PORTA_PCTL_R |= 0x11;        // Configure PA0 and PA1 as UART0 RX and TX

    UART0_CTL_R &= ~0x01;              // Disable UART0
    UART0_IBRD_R = 104;                // Set baud rate to 9600 (assuming 16 MHz system clock)
    UART0_FBRD_R = 11;                 // Set fractional baud rate
    UART0_LCRH_R = 0x70;               // 8-bit data, no parity, 1 stop bit
    UART0_CTL_R |= 0x301;              // Enable UART0
}

// Function to send a single character over UART
void UART0_WriteChar(char c) {
    while ((UART0_FR_R & 0x20) != 0);  // Wait until UART0 is ready to transmit
    UART0_DR_R = c;                     // Send the character
}

// Function to send a string over UART
void UART0_WriteString(const char* str) {
    while (*str) {
        UART0_WriteChar(*str++);
    }
}

        
        
        /////////////switch UART0 ////////
        
        
        // Initialize PA4 as input (switch) and PA2 as output (relay)
void Switch_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x01;  // Enable clock for Port A
    GPIO_PORTA_DIR_R &= ~0x10;  // Set PA4 as input (switch)
    GPIO_PORTA_DIR_R |= 0x04;   // Set PA2 as output (relay)
    GPIO_PORTA_DEN_R |= 0x14;   // Enable digital functionality for PA4 and PA2
}

// Read the state of the switch (connected to PA4)
bool ReadSwitchState(void) {
    return (GPIO_PORTA_DATA_R & 0x10) != 0; // Return true if PA4 is HIGH
}

// Write the relay state (connected to PA2)
void WriteRelayState(bool state) {
    if (state) {
        GPIO_PORTA_DATA_R |= 0x04; // Turn relay ON
    } else {
        GPIO_PORTA_DATA_R &= ~0x04; // Turn relay OFF
    }
}
        
        char UART0_ReadChar(void) {
    while ((UART0_FR_R & 0x10) != 0); // Wait until RX FIFO is not empty
    return UART0_DR_R & 0xFF;
}

        
        


// Process switch control commands and handle XOR logic

void ProcessSwitchControl(void) {
    // Check if a new command is received from the GUI
    if ((UART0_FR_R & 0x10) == 0) { // UART1 RX FIFO not empty
        char command = UART0_ReadChar();
        if (command == '1') {
            gui_signal = true; // GUI command: Turn ON
        } else if (command == '0') {
            gui_signal = false; // GUI command: Turn OFF
        }
    }

    // Read the state of the physical switch (PA4)
    bool switch_state = ReadSwitchState();

    // XOR the GUI signal and the switch state
    bool relay_state = gui_signal ^ switch_state;

    // Write the resulting state to the relay (PA2)
    WriteRelayState(relay_state);
}
        












        // Initialize UART1 (PB0 = RX, PB1 = TX)
//void UART1_Init(void) {
  //  SYSCTL_RCGCUART_R |= 0x02;  // Enable clock for UART1
    //SYSCTL_RCGCGPIO_R |= 0x02;  // Enable clock for GPIO Port B
//
    // Configure PB0 and PB1 for UART1
  //  GPIO_PORTB_AFSEL_R |= 0x03; // Enable alternate function for PB0, PB1
    //GPIO_PORTB_DEN_R |= 0x03;   // Enable digital functionality for PB0, PB1
    //GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000011;

    //UART1_CTL_R &= ~0x01;       // Disable UART1 during configuration
    //UART1_IBRD_R = 104;         // Integer part for 9600 baud rate (assuming 16 MHz clock)
    //UART1_FBRD_R = 11;          // Fractional part for 9600 baud rate
    //UART1_LCRH_R = 0x70;        // 8-bit, no parity, 1 stop bit
    //UART1_CTL_R |= 0x301;       // Enable UART1
//}





// Send a character via UART1
//void UART1_WriteChar(char c) {
  //  while ((UART1_FR_R & 0x20) != 0); // Wait until TX FIFO is not full
    //UART1_DR_R = c;
//}






// Read a character via UART1


char UART0_WaitForChange(void) {
  // Read the current data in the UART 
  char current_data = UART0_DR_R; 
  // Wait until the data changes 
  while (UART0_DR_R == current_data) {} //  Return the new data 
  return UART0_DR_R; 
}

