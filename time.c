#include "time.h"

volatile uint32_t msecs = 0;

void initialize_time(){
	cli();

	msecs = 0;

	/* Timer 0 */
	TCCR0A = 0;

	/* CTC, clear on compare match */
	TCCR0A |= (1<<WGM01)|(0<<WGM00);

	/* OCR interrupt enable */
	TIMSK0 |= (1<<OCIE0A);

	/* 8 Mhz prescaled with 1024 =~ 7812 Hz */
	//TCCR0A |= (1<<CS02)|(0<<CS01)|(1<<CS00);

	/* 7812/8 = 976.5 Hz */
	OCR0A = 8;

	sei();
}

static void start_timer(){
	cli();

	msecs = 0;

	/* 32.768 kHz prescaled with 32 = 1024 Hz 
	TCCR2A &= ~(1<<CS22);
	TCCR2A |= (0<<CS22)|(1<<CS21)|(1<<CS20);*/

	TCCR0A &= ~((1<<CS02)|(0<<CS01));
	TCCR0A |= (1<<CS02)|(0<<CS01)|(1<<CS00);

	TCNT0 = 0;

	sei();
}

static void stop_timer(){
	cli();

	TCCR0A &= ~(1<<CS22|1<<CS21|1<<CS20);
	
	sei();
}

void initialize_time_2(){
	cli();

	msecs = 0;

	/* Timer 2 */
	TCCR2A = 0;

	/* CTC, clear on compare match */
	TCCR2A |= (1<<WGM21)|(0<<WGM20);

	/* OCR interrupt enable */
	TIMSK2 |= (1<<OCIE2A);

	/* Asynchronous clock */
	ASSR |= (1<<AS2);

	/* 32.768 kHz prescaled with 32 = 1024 Hz
	TCCR2A &= ~(1<<CS22);
	TCCR2A |= (0<<CS22)|(1<<CS21)|(1<<CS20);
	*/

	/* 1024/1 = 1024 Hz */
	OCR2A = 1;

	TCNT2 = 0;

	sei();
}

ISR(TIMER0_COMP_vect){
	msecs++;
}

static volatile uint32_t get_sys_ms(){
	static volatile uint32_t retval = 0;
	cli();
	retval = msecs;
	sei();
	return retval;
}

void delay(uint32_t ms){
	start_timer();
	static uint32_t last_time = 0;
	last_time = get_sys_ms();
	while ((get_sys_ms()-last_time) < ms) sleep_mode();
	stop_timer();
}
