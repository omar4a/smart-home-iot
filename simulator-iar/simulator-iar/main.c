#include "tm4c.h"
#include "project.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
int main(void) {
  
  
  UART0_Init();   // Initialize UART0
  char SystemState = UART0_Handler();
  
    while (1) {
        switch (SystemState) {
            case 'T':

              ADC0_Init();
              Buzzer_Init();
              int temperature;
              int adc_value;
                
              while (SystemState == 'T'){
                
                adc_value = ADC0_Read();
                temperature = (ConvertToTemperature(adc_value));
                char buffer[50];
                sprintf(buffer, "TEMP: %i", temperature);
                UART0_WriteString(buffer);
                UART0_WriteChar('\n');  // Send the temperature value via UART

                if (temperature > TEMPERATURE_THRESHOLD) {
                    Buzzer_On();
                } else {
                    Buzzer_Off();
                }
                if (!(UART0_FR_R & UART_FR_RXFE))
                { SystemState = UART0_DR_R;} // Read new character from UART0            
                }
              
                SysTick_Wait(500);
                
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
                
                switch_state = (GPIO_PORTA_DATA_R & 0x10 ) != 0;
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
                
                SysTick_Wait(500);
               
               }
              
                break;

        case 'P':
        case 'A':
        case 'B':
          
              while (SystemState == 'P' || SystemState == 'A' || SystemState == 'B')
              {
                
              Init_PC5();
              
              if(SystemState=='A')
                  Enable_PC5();
              else if(SystemState=='B')
                  Disable_PC5();
              
              if (!(UART0_FR_R & UART_FR_RXFE))
                { SystemState = UART0_DR_R;}
              
              SysTick_Wait(500);
              }
                break;
                 

            case 'D':
              Init_PB3();
              volatile bool door_status;
              
              while(SystemState == 'D'){
                
                door_status = read_magnetic_switch();
                
                if (!door_status){ // door closed
                  
                  UART0_WriteString("DOOR: CLOSED\n");
                  
                }
                
                else{ // door open
                  
                  UART0_WriteString("DOOR: OPEN\n");
                  
                }
                
                
               if (!(UART0_FR_R & UART_FR_RXFE))
                { SystemState = UART0_DR_R;}
                
                SysTick_Wait(500);
              
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