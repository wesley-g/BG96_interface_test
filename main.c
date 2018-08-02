
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/////////////* function prototypes *//////////////////
void UART_A2_Init(void);

/* UART communication function */
uint8_t uart_putc(unsigned char c);
uint8_t uart_puts(char *str);

///////////////////////////////////////////////////////
#define BUFF_SIZE 100

volatile uint8_t TXData;
volatile uint8_t RXBuffer[BUFF_SIZE];
volatile uint8_t OKBuffer[4] = {79,75,13,10}; // OK,/x0d,/x0a
volatile uint8_t ERRORBuffer[7] = {69,82,82,79,82,13,10}; // ERROR/x0d,/x0a
volatile uint8_t Buffer[26] = {69,78,84,69,82,32,67,79,77,77,65,78,68,32,84,69,83,84,32,77,79,68,69,10,13,10,13}; // ENTER COMMAND TEST MODE,\x0a,\x0d,\x0a,\x0d,#
volatile uint32_t ii;
volatile uint8_t i = 0, x = 0, y = 0;
volatile uint8_t wordcount;
volatile uint8_t len;
volatile uint8_t Receive_done = 0;
volatile uint8_t AT_Error = 0;
char response;

const eUSCI_UART_Config uartConfig =                      // UART calculator: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                   // SMCLK Clock Source
        6,                                                // BRDIV
        8,                                                // UCxBRF
        32,                                               // UCxBRS
        EUSCI_A_UART_NO_PARITY,                           // No Parity
        EUSCI_A_UART_LSB_FIRST,                           // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                        // One stop bit
        EUSCI_A_UART_MODE,                                // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION     // Oversampling
};

int main(void)
{
    /* Halting WDT  */
    MAP_WDT_A_holdTimer();
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    UART_A2_Init();

    uart_puts("AT\r");

    while(1)
    {

    }

} // end of main

void UART_A2_Init(void)
{
    /* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
}

uint8_t uart_puts(char *str)               //Sends a String to the UART.
{
     Receive_done = 0;
     AT_Error = 0;

     while(*str)
     {
         MAP_UART_transmitData(EUSCI_A2_BASE,(char *) *str++);
     }

}

uint8_t uart_putc(unsigned char c)  //Sends a char to the UART.
{
    MAP_UART_transmitData(EUSCI_A2_BASE,c);
}

/* EUSCI A2 UART ISR */
void EUSCIA2_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
              RXBuffer[i] = (char *)MAP_UART_receiveData(EUSCI_A2_BASE);
              i++;
    }
}
