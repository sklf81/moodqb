#include <avr/io.h>

#define TOGGLE_DELAY_MS 500
#define BAUD_PERIOD_MS 2 //=1/300Baud

#define R_PORT 0
#define G_PORT 4
#define B_PORT 3

#define ANALOG_IN 2   //PINS SIND IM LAYOUT FALSCH!
#define TOGGLE_IN 1

//COLOR-CODE: 0b00bbggrr;

byte pwm_color = 0, pwm_color_tmp;

char msg[9] = {'S', 'O', 'P', 'H', 'I', 'E', '<', '3', (char)255};
byte msg_ctr = 0;
//byte baud_period;
byte color_ctr = 0;

byte mode = 0;
long unsigned int runtime = 0;
long unsigned int last_toggle = 0;
long unsigned int last_change = 0;

byte d_time = 0;

unsigned int colorchange_delay_ms = 0;

//PWM
unsigned short pwm_period_ms = 20;
long unsigned int pwm_timer = 0;
long unsigned int pwm_start_time;
//byte red_delay, green_delay, blue_delay;


void updatePWMColorChannel(byte channel_delay, byte PORT){
  if(d_time < channel_delay){
    PORTB |= PORT;
  }
  else{
    PORTB &= !PORT;
  }
}

void setup(){
	DDRB = 0b00011001;
  ADMUX = 0b00100001;
  ADCSRA |= 0b11000011; //Freerunning (BIT 6) Prescalar dicvision by 8 at 0:2
}

void loop(){
  ADCSRA |= (1 << ADSC);
	colorchange_delay_ms = ADCH; //bei 255 --> 1000 ms
  byte input = PINB;
  runtime = millis();
	if(!(input & 2) && ((runtime - last_toggle) > TOGGLE_DELAY_MS)){
		last_toggle = runtime;
		mode = (mode == 4) ? 0 : mode + 1;
	}

	switch (mode) {
		case 0:
      //Switch Color To Black
      pwm_color = colorchange_delay_ms;
		break;
		case 1:
      //Rainbow
			if((runtime - last_change) > colorchange_delay_ms){
        pwm_color = color_ctr ^(color_ctr >> 1);
        color_ctr = color_ctr >= 63 ? 0 : color_ctr + 1;
				last_change = millis();
			}
		break;

    case 2:
      pwm_color_tmp = pwm_color;
      mode += 1;
    break;
  
		case 3:
			//Color from RBOW as static
		break;

    case 4:
      byte msg_index = msg_ctr % 9;
      byte baud_period = colorchange_delay_ms > 190 ? BAUD_PERIOD_MS : colorchange_delay_ms;
      if((runtime - last_change) > baud_period){
        if(msg[msg_index] & (0x1 << msg_ctr)){ 
          pwm_color &= 0b00000000;
        }
        else{
          pwm_color |= 0b00111111;
        }
        msg_ctr += 1;
        last_change = runtime;
      }
    break;
		default:
			mode = 0;
		break; 
	}
 

  //PWM

  byte red_delay = (pwm_color & 0b00000011) << 1;
  byte green_delay = (pwm_color & 0b00001100) >> 1;
  byte blue_delay = (pwm_color & 0b00110000) >> 3;
  
  d_time = runtime - pwm_timer;
  
  if(d_time >= pwm_period_ms){
    pwm_timer += d_time;
  }
  
  updatePWMColorChannel(red_delay, 0b00000001);
  updatePWMColorChannel(green_delay, 0b00001000);
  updatePWMColorChannel(blue_delay, 0b00010000);
}
