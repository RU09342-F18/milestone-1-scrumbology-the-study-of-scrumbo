#include <msp430.h> 

// Authors: Brian Harlow & David Gaffney
// Class: Introduction to Embedded Systems
// Section: 3
// Date: 10 October 2018
// Last Updated: 10 October 2018
// Microcontroller: MSP430G2553

// Program Description:

// Execution Plan:
//      1) Learn to use output modes for controlling pins with timer
//      2) Confirm off-board works with program
//      3) Learn to use microcontroller UART
//      4) Allow program to take input from RXD and use as data for LED
//      5) Allow program to send altered packets through TXD to other nodes


/**
 * main.c
 */

void cfgRGB(void){
    P1SEL |= BIT6;              // Sets P1.6 to output SET/RESET signal from Output Mode 3
    P1SEL2 &= ~BIT6;            // Sets P1.6 to output SET/RESET signal from Output Mode 3

    P1DIR |= BIT6;              // Sets P1.6 to output SET/RESET signal from Output Mode 3

    P2SEL |= (BIT1 + BIT4);     //
    P2SEL2 &= ~(BIT1 + BIT4);   //

    P2DIR |= (BIT1 + BIT4);     //
}

void cfgButton(void){
    P1SEL &= ~BIT3;             // Sets P1.3 to GPIO
    P1SEL2 &= ~BIT3;            // Sets P1.3 to GPIO

    P1DIR &= ~BIT3;             // Sets P1.3 to input

    // Pull-up Resistor
    P1REN |= BIT3;              // Enable pull-up/pull-down resistor
    P1OUT |= BIT3;              // Pull-Up/Pull-Down resistor set to pull-up

    // Interrupt Setup
    P1IE |= BIT3;               // Enable interrupt from P1.3
    P1IES |= BIT3;              // Sets interrupt to trigger on falling edge
    P1IFG &= ~BIT3;             // Clears the interrupt flag for P1.3
}

void cfgTimerA0(void){
    TA0CTL = TASSEL_2 + MC_1 + ID_2 + TACLR + TAIE; // SMCLK selected in Up Mode, divided by 4, timer cleared, and timer interrupts enabled

    TAOCCR0 = 255;              // Sets CCR0 to 255
    TA0CCR1 = 0;                // Sets CCR1 to 0, controls RED in RGB

    TA0CCTL1 = OUTMOD_3;
}

void cfgTimerA1(void){
    TA1CTL = TASSEL_2 + MC_1 + ID_2 + TACLR + TAIE;

    TA1CCR0 = 255;              // Sets CCR0 to 255
    TA1CCR1 = 0;                // Sets CCR1 to 0, controls GREEN in RGB
    TA1CCR2 = 0;                // Sets CCR2 to 0, controls BLUE in RGB

    TA1CCTL1 = OUTMOD_3;
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	cfgRGB();       // Configure pins used for RGB
	cfgButton();    // Configure Button input
	cfgTimerA0();   // Configure Timer A0
	cfgTimerA1();   // Configure Timer A1

	_BIS_SR(LPM0_bits + GIE);   // Enter LPM0 and enable global interrupts
	return 0;
}

// Button Interrupt Service Routine
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void) {

}


