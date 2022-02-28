#define TOGGLE_DELAY_MS 300
#define MSG_LENGTH 48
#define CURRENT_BIT (msg[msg_ctr>>3] & (0x1 << (msg_ctr % 8)))

#define R_PORT 0
#define G_PORT 4
#define B_PORT 3

#define ANALOG_IN 2   //PINS SIND IM LAYOUT FALSCH!
#define TOGGLE_IN 1

byte mode = 0;
//long unsigned int runtime = 0;
long unsigned int last_toggle = 0;
long unsigned int last_change = 0;

//unsigned int colorchange_delay_ms = 0;

byte free_run = 0;
byte change = 0;
byte msg_flag = 0;
byte msg_ctr = 0;
byte msg[6] = {0b10111010, 0b10001110, 0b11101110, 0b00101010, 0b00111010, 0b00000000};
//byte red_delay, green_delay, blue_delay;

void setup(){
	DDRB = 0b00011001;
  ADMUX = 0b00100001;
  ADCSRA |= 0b11000011; //Freerunning (BIT 6) Prescalar dicvision by 8 at 0:2
}

void loop(){
  ADCSRA |= (1 << ADSC);
	unsigned int colorchange_delay_ms = ADCH << 1;
  byte input = PINB;
  long unsigned int runtime = millis();
	if(!(input & 2) && ((runtime - last_toggle) > TOGGLE_DELAY_MS)){
		last_toggle = runtime;
    if(free_run == 1){
      free_run = 0;
      mode = 0;
      msg_flag = 1;
      msg_ctr = 0; 
    }
    else if(msg_flag){
      msg_flag = 0;
      mode = 0;
    }
    else if(mode == 7 && !free_run){
      free_run = 1;
      mode = 1;
    }
		else{
		  mode = mode + 1;
		}
	}
	switch (mode) {
		case 0:
      PORTB &= 0b11100110;
      if(free_run && change){
        mode += 1;
        change = 0;
      }
      else if(msg_flag && change && CURRENT_BIT){
        mode = 7;
        change = 0;
      }
		break;
		case 1:
      //ROT
      PORTB &= 0b11100111;
      PORTB |= 0b00000001;
      if(free_run && change){
        mode += 1;
        change = 0;
      }
		break;
    case 2:
      //GELB
      PORTB &= 0b11101111;
      PORTB |= 0b00001001;
      if(free_run && change){
        mode += 1;
        change = 0;
      }
    break;
		case 3:
      //GRÜN
      PORTB &= 0b11101110;
      PORTB |= 0b00001000;  
      if(free_run && change){
        mode += 1;
        change = 0;
      } 
		break;
    case 4:
      //CYAN
      PORTB &= 0b11111110;
      PORTB |= 0b00011000;
      if(free_run && change){
        change = 0;
        mode += 1;
      }
    break;
    case 5:
      //BLAU
      PORTB &= 0b11110110;
      PORTB |= 0b00010000;
      if(free_run && change){
        change = 0;
        mode += 1;
      }
    break;
    case 6:
      //MAGENTA
      PORTB &= 0b11110111;
      PORTB |= 0b00010001;
      if(free_run && change){
        change = 0;
        mode = 1;
      }
    break;
    case 7:
      //WEISS
      PORTB |= 0b00011001;
      if(msg_flag && change && !CURRENT_BIT){
        mode = 0;
        change = 0;
      }
    break;
		default:
			mode = 0;
		break; 
	}
  change = ((runtime - last_change) > colorchange_delay_ms) ? 1 : 0; 
  if (change){
    last_change = runtime;
    msg_ctr = (msg_ctr == MSG_LENGTH - 1) ? 0 : msg_ctr + 1;
  }
}
