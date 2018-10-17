#include <msp430.h> 

// Authors: Brian Harlow & David Gaffney
// Class: Introduction to Embedded Systems
// Section: 3
// Date: 10 October 2018
// Last Updated: 10 October 2018
// Microcontroller: MSP430G2553

// Program Description: This program is designed to use UART to chain different RGB LEDs together. Data can be transferred
//                      between the different LEDs (nodes) so that an input will send data through the entire chain.


/**
 * main.c
 */

char packetSize = 0;            // Global variable definition for packet size
char packetByte = 0;            // Global variable definition for current byte

void cfgRGB(void){
    P1SEL |= BIT6;              // Sets P1.6 to output SET/RESET signal from Output Mode 3
    P1SEL2 &= ~BIT6;            // Sets P1.6 to output SET/RESET signal from Output Mode 3

    P1DIR |= BIT6;              // Sets P1.6 to output SET/RESET signal from Output Mode 3

    P2SEL |= (BIT1 + BIT4);     // Sets P2.1 and P2.4 to output SET/RESET signal from Output Mode 3
    P2SEL2 &= ~(BIT1 + BIT4);   // Sets P2.1 and P2.4 to output SET/RESET signal from Output Mode 3

    P2DIR |= (BIT1 + BIT4);     // Sets P2.1 and P2.4 to output SET/RESET signal from OutPut Mode 3
}

void cfgTimerA0(void){
    TA0CTL = TASSEL_2 + MC_1 + ID_2 + TACLR;    // SMCLK selected in Up Mode, divided by 4, timer cleared

    TA0CCR0 = 255;              // Sets CCR0 to 255
    TA0CCR1 = 0;                // Sets CCR1 to 0, controls RED in RGB

    TA0CCTL1 = OUTMOD_3;        // Enables Output Mode 3 for TA0CCR1
}

void cfgTimerA1(void){
    TA1CTL = TASSEL_2 + MC_1 + ID_2 + TACLR;    // SMCLK selected in Up Mode, divided by 4, timer cleared

    TA1CCR0 = 255;              // Sets CCR0 to 255
    TA1CCR1 = 0;                // Sets CCR1 to 0, controls GREEN in RGB
    TA1CCR2 = 0;                // Sets CCR2 to 0, controls BLUE in RGB

    TA1CCTL1 = OUTMOD_3;        // Enables Output Mode 3 for TA1CCR1
    TA1CCTL2 = OUTMOD_3;        // Enables Output Mode 3 for TA1CCR2
}

void cfgUART(void){
    P1SEL |= BIT1 + BIT2;       // Sets P1.1 and P1.2 to RXD and TXD, respectively
    P1SEL2 |= BIT1 + BIT2;      // Sets P1.1 and P1.2 to RXD and TXD, respectively

    UCA0CTL1 |= UCSSEL_2;       // CLK = SMCLK
    UCA0BR0 = 109;              // Sets baud rate to 9600
    UCA0BR1 = 0;                // Sets baud rate to 9600
    UCA0MCTL = UCBRS_2;         // Sets baud rate to 9600

    UCA0CTL1 &= ~ UCSWRST;      // Disable UART reset
    UC0IE |= UCA0RXIE;          // Enable UART_RXD interrupts
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	cfgRGB();       // Configure pins used for RGB
	cfgTimerA0();   // Configure Timer A0
	cfgTimerA1();   // Configure Timer A1
	cfgUART();      // Configure UART

	_BIS_SR(LPM0_bits + GIE);   // Forces microcontroller into LPM0 and enables global interrupts
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
   char data = UCA0RXBUF;           // Stores first byte of received packet into data char

   if (packetByte == 0){            // Initializes when data is being received
       packetByte = data;           // Stores the value of the current byte in the packet
       packetSize = data;           // Stores the number of bytes in the packet received

       if (data >= 8){              // Check condition for if there is sufficient data to send to next node in chain
           UCA0TXBUF = data - 3;    // Sends expected packet size to next node in chain
       }
   }
   else if (packetSize - packetByte == 1){  // Check condition for if current data is for red
       TA0CCR1 = data;                      // Stores data into TA0CCR1 to control red
   }
   else if (packetSize - packetByte == 2){  // Check condition for if current data is for green
       TA1CCR1 = data;                      // Stores data into TA1CCR1 to control green
   }
   else if (packetSize - packetByte == 3){  // Check condition for if current data is for blue
       TA1CCR2 = data;                      // Stores data into TA1CCR2 to control blue
   }
   else {
       if (packetSize >= 8){
           UCA0TXBUF = data;        // Transmits the byte to the next node in the chain
       }
   }

   packetByte -= 1;     // Decrements counter to represent the next byte in the packet
}



