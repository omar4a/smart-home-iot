#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>

// Function prototypes
void UART1_Init(void);
void UART1_WriteChar(char c);
char UART1_ReadChar(void);
void Switch_Init(void);
bool ReadSwitchState(void);
void WriteRelayState(bool state);
void UART1_ProcessPlugControl(void);

volatile bool gui_signal = false; // Stores the last signal from the GUI

int main(void) {
    UART1_Init();   // Initialize UART1 for plug control
    Switch_Init();  // Initialize PA4 as input (switch)
    
    while (1) {
        UART1_ProcessPlugControl(); // Handle plug control commands
    }
}

// Initialize UART1 (PB0 = RX, PB1 = TX)
void UART1_Init(void) {
    SYSCTL_RCGCUART_R |= 0x02;  // Enable clock for UART1
    SYSCTL_RCGCGPIO_R |= 0x02;  // Enable clock for GPIO Port B

    // Configure PB0 and PB1 for UART1
    GPIO_PORTB_AFSEL_R |= 0x03; // Enable alternate function for PB0, PB1
    GPIO_PORTB_DEN_R |= 0x03;   // Enable digital functionality for PB0, PB1
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000011;

    UART1_CTL_R &= ~0x01;       // Disable UART1 during configuration
    UART1_IBRD_R = 104;         // Integer part for 9600 baud rate (assuming 16 MHz clock)
    UART1_FBRD_R = 11;          // Fractional part for 9600 baud rate
    UART1_LCRH_R = 0x70;        // 8-bit, no parity, 1 stop bit
    UART1_CTL_R |= 0x301;       // Enable UART1
}

// Send a character via UART1
void UART1_WriteChar(char c) {
    while ((UART1_FR_R & 0x20) != 0); // Wait until TX FIFO is not full
    UART1_DR_R = c;
}

// Read a character via UART1
char UART1_ReadChar(void) {
    while ((UART1_FR_R & 0x10) != 0); // Wait until RX FIFO is not empty
    return UART1_DR_R & 0xFF;
}

// Initialize PA4 as input (switch)
void Switch_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x01;  // Enable clock for Port A
    GPIO_PORTA_DIR_R &= ~0x10;  // Set PA4 as input
    GPIO_PORTA_DEN_R |= 0x10;   // Enable digital functionality for PA4
}

// Read the state of the switch (connected to PA4)
bool ReadSwitchState(void) {
    return (GPIO_PORTA_DATA_R & 0x10) != 0; // Return true if PA4 is HIGH
}

// Write the relay state (connected to PA2)
void WriteRelayState(bool state) {
    SYSCTL_RCGCGPIO_R |= 0x01;  // Enable clock for Port A
    GPIO_PORTA_DIR_R |= 0x04;   // Set PA2 as output
    GPIO_PORTA_DEN_R |= 0x04;   // Enable digital functionality for PA2

    if (state) {
        GPIO_PORTA_DATA_R |= 0x04; // Turn relay ON
    } else {
        GPIO_PORTA_DATA_R &= ~0x04; // Turn relay OFF
    }
}

// Process plug control commands and handle XOR logic
void UART1_ProcessPlugControl(void) {
    // Check if a new command is received from the GUI
    if ((UART1_FR_R & 0x10) == 0) { // UART1 RX FIFO not empty
        char command = UART1_ReadChar();
        if (command == '1') {
            gui_signal = true; // GUI command: Turn ON
        } else if (command == '0') {
            gui_signal = false; // GUI command: Turn OFF
        }
    }

    // Read the state of the switch (PA4)
    bool switch_state = ReadSwitchState();

    // XOR the GUI signal and the switch state
    bool relay_state = gui_signal ^ switch_state;

    // Write the resulting state to the relay (PA2)
    WriteRelayState(relay_state);
}
