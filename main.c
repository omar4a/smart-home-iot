#include "tm4c.h"
#include "project.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

char received_command;

// Define states for controlling different functionalities
typedef enum {
    IDLE,
    MONITOR_TEMP,
    CONTROL_LAMP,
    CONTROL_PLUG,
    CONTROL_DOOR
} SystemState;

SystemState systemState = IDLE;  // Initial state

 char UART0_Handler(void) {
    // Read the incoming UART command
    while (UART0_FR_R & UART_FR_RXFE){};  // Wait until UART has data
    char x = UART0_DR_R;
    
    return x;
    }

int main(void) {
  
  
  UART0_Init();   // Initialize UART0
  char SystemState = UART0_Handler();
  
    while (1) {
        switch (SystemState) {
            case 'T':
              
              

                ADC0_Init();    // Initialize ADC0 
                Buzzer_Init();  // Initialize Buzzer 
              int temperature;
              int adc_value;
                
              while (SystemState == 'T'){
                
                adc_value = ADC0_Read();  // Read ADC value 
                temperature = (ConvertToTemperature(adc_value));  // Convert to temperature 
                char buffer[50];
                sprintf(buffer, "%.2f", temperature);
                UART0_WriteString(buffer);
                UART0_WriteChar('\n');  // Send the temperature value via UART

                if (temperature > TEMPERATURE_THRESHOLD) {
                    Buzzer_On();  // Turn buzzer on if temperature exceeds threshold
                } else {
                    Buzzer_Off();  // Turn buzzer off if temperature is below threshold
                }
                printf("%i", temperature);
                if (!(UART0_FR_R & UART_FR_RXFE))
                { SystemState = UART0_DR_R;} // Read new character from UART0 }             
                }
                SysTick_Wait(100);
                
                break;

            case 'L':
            case 'C':
            case 'E':
          
          
          
               Init_PA2();
               Init_PA4();
               bool switch_state;
               bool app_signal;
               bool lamp_control;
               while(SystemState == 'L' || SystemState == 'C' || SystemState == 'E'){
                
                switch_state = (GPIO_PORTA_DATA_R & 0x10 ) != 0 ;
                if (SystemState == 'C')
                  app_signal = 1;
                else if (SystemState == 'E')
                  app_signal = 0;
                
                lamp_control = switch_state ^ app_signal;
                if (lamp_control)
                  Enable_PA2();
                else
                  Disable_PA2();
                
                if (!(UART0_FR_R & UART_FR_RXFE))
                { SystemState = UART0_DR_R;}
               
               }            
              
                break;

        case 'P':
        case 'A':
        case 'B':
              while (SystemState == 'P' || SystemState == 'A' || SystemState == 'B')
              {
                
                // Add code to control the plug
              Init_PC5();
              
              if(SystemState=='A')
                  Enable_PC5();
              else if(SystemState=='B')
                  Disable_PC5();
              
              if (!(UART0_FR_R & UART_FR_RXFE))
                { SystemState = UART0_DR_R;}
              }
                break;
                 

            case 'D':
              Init_PB3();
              bool door_status;
              
              while(SystemState == 'D'){
                
                door_status = read_magnetic_switch();
                
                if (!door_status){ // door closed
                  
                }
                
                else{ // door open
                  
                }
                
                
               if (!(UART0_FR_R & UART_FR_RXFE))
                { SystemState = UART0_DR_R;}
                
                SysTick_Wait(100);
              
              }
              
                break;

            default:
                // Stay idle
                break;
        }
        
        if (!(UART0_FR_R & UART_FR_RXFE))
        { SystemState = UART0_DR_R;}
    }
  return 0;
  
    
}
