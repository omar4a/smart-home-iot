#include "tm4c.h"
#include <stdbool.h>

#define GPIO_PORTA 0x01  // Port A

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


////////////plug/////////

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