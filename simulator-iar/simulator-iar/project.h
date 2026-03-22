#ifndef project_h
#define project_h
#include <stdint.h>

#include <stdbool.h>

#define GPIO_PORTA 0x01  // Port A
#define TEMPERATURE_THRESHOLD 35

void delay_ms(unsigned int ms);

void Init_PA2();

void Enable_PA2();

void Disable_PA2();

void Init_PA4();

void Init_PC5();

void Enable_PC5();

void Disable_PC5();

void Init_PB3();

bool read_magnetic_switch();

void SysTick_Wait(int ms);

void SysTick_Wait1ms(void);


///////////////////////////////////////Temp SENSOR WITH BUZZER ////////////////////////////


void ADC0_Init(void);
int ADC0_Read(void);
int ConvertToTemperature(uint32_t adc_value);
void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void UART0_Init(void);
void UART0_WriteChar(char c);
void UART0_WriteString(const char* str);



////switch//////





void Switch_Init(void);
bool ReadSwitchState(void);
void WriteRelayState(bool state);

char UART0_WaitForChange(void);

// Logic processing for plug control
void ProcessSwitchControl(void);





// UART1 initialization
void UART1_Init(void);

// UART1 communication
void UART1_WriteChar(char c);
char UART1_ReadChar(void);
char UART0_Handler(void);


#endif