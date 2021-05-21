#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED_DDR 		DDRC						///< DDR of indicator LED.
#define NUMERAL_DDR 		DDRB						///< DDR for controlling individual segments.
#define CONTROL_DDR 		DDRD						///< DDR for controlling individual digits.
#define LED_PORT 		PORTC						///< Port of indicator LED.
#define NUMERAL_PORT 		PORTB						///< Port for controlling individual segments.
#define CONTROL_PORT 		PORTD						///< Port for controlling individual digits.
#define LED_PIN1		PC0						///< Pin of first indicator LED.
#define LED_PIN2		PC1						///< Pin of second indicator LED.
#define DIGIT1_PIN		PD0						///< Pin controlling first digit of SSD.
#define DIGIT2_PIN		PD1						///< Pin controlling second digit of SSD.
#define DIGIT3_PIN		PD2						///< Pin controlling third digit of SSD.
#define DIGIT4_PIN		PD3						///< Pin controlling fourth digit of SSD.

void PORT_INIT(void);
void TIMER_INIT(void);
void DECODE_SSD(uint16_t value);
void DISPLAY_SSD(void);

volatile uint16_t milliseconds = 0;						///< Variable to hold the milliseconds elapsed.
volatile uint16_t count = 0;							///< Variable to hold the count.
volatile uint8_t working = 1;							///< Variable to determine if the ISR is still working.

uint8_t dig[4];									///< Array to hold individual digits of the value to be displayed.

uint16_t time = 1000;								///< Variable to hold the required time the ISR should run in milliseconds.

/*!
 *	@brief ISR for Timer/Counter0 Compare Match Interrupt; increments count variable every millisecond.
 */

ISR(TIMER0_COMP_vect){
	milliseconds++;
	count++;
	LED_PORT ^= (1<<LED_PIN2);
	if(milliseconds > time){
		working = 0;
		milliseconds = 0;
	}
}

int main(void){
	PORT_INIT();
	TIMER_INIT();
	
	while(1){
		sei();								///< Enable global interrupts.
		
		while(working);
		
		cli();								///< Disable global interrupts.
		
		LED_PORT ^= (1<<LED_PIN1);
		
		DECODE_SSD(count);
		DISPLAY_SSD();
		
		count = 0;
		working = 1;
		
		LED_PORT ^= (1<<LED_PIN1);
	}
}

/*!
 *	@brief Initialize Ports.
 */

void PORT_INIT(void){
	LED_DDR |= (1<<LED_PIN1)|(1<<LED_PIN2);
	NUMERAL_DDR = 0xFF;
	CONTROL_DDR |= (1<<DIGIT1_PIN)|(1<<DIGIT2_PIN)|(1<<DIGIT3_PIN)|(1<<DIGIT4_PIN);
}

/*!
 *	@brief Initialize Timer/Counter0 in CTC mode.
 */

void TIMER_INIT(void){
	TCCR0 = (1<<WGM00)|(1<<WGM01)|(1<<CS01)|(1<<CS00);			///< Initialize timer0 in CTC mode with prescalar 64.
	TIMSK = (1<<OCIE0);							///< Enable compare match interrupt.
	OCR0 = 125;								///< Set OCR0 to 125 to generate an interrupt every 1 ms.
}

/*!
 *	@brief Decode the value into individual digits.
 *	@param Value to be decoded (uint16_t).
 */

void DECODE_SSD(uint16_t value){
	dig[0] = (value/1000);							///< Save first digit to array.
	dig[1] = (value%1000)/100;						///< Save second digit to array.
	dig[2] = (value%100)/10;						///< Save third digit to array.
	dig[3] = (value%10);							///< Save fourth digit to array.
}

/*!
 *	@brief Display each individual digit on the SSD.
 */

void DISPLAY_SSD(void){
	
	uint8_t ssd[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};	///< Array to hold values of segments corresponding to each digit.
	
	for(uint8_t rounds=0;rounds<100;rounds++){
		NUMERAL_PORT = ssd[dig[0]];					///< Display first digit momentarily.
		CONTROL_PORT |= (1<<DIGIT1_PIN);				///< Turn on display of first digit place.
		_delay_ms(1);
		CONTROL_PORT &= ~(1<<DIGIT1_PIN);				///< Turn off display of first digit place.
		
		NUMERAL_PORT = ssd[dig[1]];					///< Display second digit momentarily.
		CONTROL_PORT |= (1<<DIGIT2_PIN);				///< Turn on display of second digit place.
		_delay_ms(1);
		CONTROL_PORT &= ~(1<<DIGIT2_PIN);				///< Turn off display of second digit place.
		
		NUMERAL_PORT = ssd[dig[2]];					///< Display third digit momentarily.
		CONTROL_PORT |= (1<<DIGIT3_PIN);				///< Turn on display of third digit place.
		_delay_ms(1);
		CONTROL_PORT &= ~(1<<DIGIT3_PIN);				///< Turn off display of third digit place.
		
		NUMERAL_PORT = ssd[dig[3]];					///< Display fourth digit momentarily.
		CONTROL_PORT |= (1<<DIGIT4_PIN);				///< Turn on display of fourth digit place.
		_delay_ms(1);
		CONTROL_PORT &= ~(1<<DIGIT4_PIN);				///< Turn off display of fourth digit place.
	}
}
