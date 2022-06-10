/*
This C code is run in Code Composer Studio for the TI-MSP430 MCU
It is designed to collect analog data from three sensors hooked up by wires to the pins of the MSP430
The MCU converts this data to digital version through its ADC module
The program then serially outputs the data either to:
-console
-GUI application made with Windows Forms which then transmits data to ThingSpeak channel.
*/

#include <msp430g2553.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//variable declarations
char v3[5];     //resistance of LDR, in string form
char v4[5];     //thermistor temperature, in string form
char v5[5];     //motion detector echo input, in string form
char a3[] = " Photocell:   ";
char a4[] = " Thermistor:   ";
char a5[] = " Distance:    ";
char space[] = " ";
char newline[] = " \r\n";
char endOfMessage[] = ";";
unsigned int adc[6];

//hc-sr04 variables
volatile int temp[2];
volatile float diff;
volatile unsigned int i=0;
int dst_int;
char dst_char[5];
volatile float distance;

int therm = 0;

//function declarations
void ser_output(char *str);
void ConfigureAdc();
void uart_init(void);
void ConfigClocks(void);
void strreverse(char* begin, char* end);
void itoa(int value, char* str, int base);
void port_init();
void hcsr04_init();

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    port_init();              //Good to go. P1.1 and P1.2 are used for uart TX and RX
    ConfigClocks();
    uart_init();              //19200 baud rate
    ConfigureAdc();             //analog-to-digital converter is configured
    hcsr04_init();              //motion detection sensor is initialized
    _enable_interrupts();
    while(1){
        distance = diff/58;
        dst_int = floor(distance);
        dst_int = abs(dst_int);
        ltoa(dst_int, dst_char, 10);                        //LTOA converts long type to string type
        if (dst_int >= 200) {                           //green LED on
               P2OUT &= ~BIT2 + ~BIT4;
               P2OUT |= BIT3;
        }
        else if (dst_int < 200 && dst_int > 50) {       //yellow LED on
               P2OUT &= ~BIT3 + ~BIT2;
               P2OUT |= BIT4;
        }
        else if (dst_int <= 50) {                        //red LED on
               P2OUT &= ~BIT3 + ~BIT4;
               P2OUT |= BIT2;
        }
        ADC10CTL0 &= ~ENC;
        while(ADC10CTL1 & BUSY);

        ADC10CTL0 |= ENC + ADC10SC;       //Sampling/conversion start
        ADC10SA = (unsigned int)adc;        //assign analog conversion results to the array adc
        //adc[3]=photocell    adc[4]=thermistor

        therm = (adc[4]* 48724 - 30634388) >> 16;
        therm = therm+135;
        if (therm > 99 || therm < 10)
        {
            therm = 75;
        }
        ltoa(adc[3],v4,10);         //convert photocell digital data to a string, v4
        ltoa(therm,v3,10);          //convert thermistor digital data to a string, v3
        ltoa(dst_int,v5,10);        //convert motion digital data to a string, v5
        //ser_output(a3);             //transmit string " Photocell:   "
        ser_output(v4);             //transmit LDR value
        //ser_output(space);
        //ser_output(a4);             //transmit string " Thermistor:   "
        ser_output(v3);             //transmit temp value
        //ser_output(space);
        //ser_output(a5);             //transmit string " Distance:   "
        ser_output(v5);             //transmit distance
        ser_output(endOfMessage);
        //ser_output(newline);
        __delay_cycles(14000000);
    }
}
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_A(void){
        temp[i] = TA1CCR1;          //echo input
        i += 1;
        TA1CCTL1 &= ~CCIFG ;
        if (i==2) {
            diff=temp[i-1]-temp[i-2];
            i=0;
        }

}
void hcsr04_init() {
    TA0CTL = TASSEL_2|MC_1 ;        //SMCLK selected, up_mode selected for timer, compare mode (used for pwm)
    TA0CCR0 = 0xFFFF;               //counts up to 65535 (period for PWM), one cycle one microsecond (clock is at 1 MHZ)
    TA0CCR1 = 0x000A;              //duty cycle = 10 microseconds
    TA0CCTL1 = OUTMOD_7;            //when TimerA0 is counting between 0 and 10, output will be 1 (for trigger)
    TA1CTL = TASSEL_2|MC_2 ;
    TA1CCTL1 = CAP | CCIE | CCIS_0 | CM_3 | SCS ;   //capture mode for getting the time values, interrupts enabled, capture compare input selected
}
// Configure ADC
void ConfigureAdc(){
     ADC10CTL1 = INCH_7 + ADC10DIV_0 + CONSEQ_3 + SHS_0;
     ADC10CTL0 = SREF_0 | ADC10SHT_2 | MSC | ADC10ON ;//| ADC10IE; //Vref+, Vss, 64 ATD clocks per sample, internal references, turn ADCON
     ADC10AE0 = BIT4 + BIT3;           //only these pins will read analog input
     ADC10DTC1 = 8;           //auto transfer ACD10MEM results to adc10sa
}
void uart_init(void){
    UCA0CTL1 |= UCSWRST+UCSSEL_2;            //Disable the UART state machine
    UCA0BR1 = 0;
    UCA0BR0 = 52;                           //Produce a 19,200 Baud UART rate
    UCA0MCTL = UCBRS_0;                     //Choose apropriately from Table 15-4 in User Guide
    UCA0CTL1 &= ~UCSWRST;                   //Enable the UART state machine
}
void ConfigClocks(void) {
  BCSCTL1 = CALBC1_1MHZ;                     // Set range
  DCOCTL = CALDCO_1MHZ;                      // Set DCO step + modulation
 }
void strreverse(char* begin, char* end)      // Function to reverse the order of the ASCII char array elements
{
    char aux;
    while(end>begin)
        aux=*end, *end--=*begin, *begin++=aux;
}

void itoa(int value, char* str, int base) {         //Function to convert the signed int to an ASCII char array

    static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* wstr=str;
    int sign;
                                                    // Validate that base is between 2 and 35 (inlcusive)
    if (base<2 || base>35){
        *wstr='\0';
        return;
    }
                                                    // Get magnitude and th value
    sign=value;
    if (sign < 0)
        value = -value;

    do                                              // Perform interger-to-string conversion.
        *wstr++ = num[value%base];                  //create the next number in converse by taking the modolus
    while(value/=base);                             // stop when you get  a 0 for the quotient

    if(sign<0)                                      //attach sign character, if needed
        *wstr++='-';
    *wstr='\0';                                     //Attach a null character at end of char array. The string is in reverse order at this point
    strreverse(str,wstr-1);                         // Reverse string

}
void port_init(){
    P1SEL = BIT1 | BIT2 | BIT6;            // select non-GPIO  usage for Pins 1,2
    P1SEL2 = BIT1 | BIT2;           // Select UART usage of Pins 1 and 2
    P2SEL = BIT1;           //set p2.1 (echo) to timer/clock usage
    P1DIR |= BIT6;                   //p1.6 as output
    P2DIR |= BIT2 | BIT3 | BIT4;     //p2.2 (red), p2.3 (green), p2.4 (yellow) LEDs as outputs
    P2OUT = 0x00;
}
void ser_output(char *str){
    while(*str != 0){
        while ((IFG2 & UCA0TXIFG) == 0);
        UCA0TXBUF = *str++;
    }
}
