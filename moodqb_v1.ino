#define RBOW_SIZE 50
#define MAX_DELAY_MS 100
#define TOGGLE_DELAY_MS 10

#define R_PORT 0
#define G_PORT 4
#define B_PORT 3

#define ANALOG_IN 2   //PINS SIND IM LAYOUT FALSCH!
#define TOGGLE_IN 1

typedef struct _COLOR{
	byte r;
	byte g;
	byte b;
} COLOR;

//COLOR-CODE: pwm_period_ms --> 255

COLOR pwm_color;

String msg = "SOPHIE<3";

byte mode = 0;
long unsigned int last_toggle = 0;
long unsigned int last_change = 0;

byte colorchange_delay;
byte color_ctr = 0;
byte msg_ctr = 0;

//PWM
int pwm_period_ms = 10;
long unsigned int pwm_timer = 0;
byte d_time;

void changeColor(COLOR* output_color, COLOR input_color){
	output_color->r = input_color.r;
	output_color->g = input_color.g;
	output_color->b = input_color.b; 
}

void setColor(COLOR* output_color, byte red, byte green, byte blue){
	output_color->r = red;
	output_color->g = green;
	output_color->b = blue;
}

void setup(){
	//pinMode...
}

void loop(){
	colorchange_delay = map(analogRead(ANALOG_IN), 0, 1024, 0, MAX_DELAY_MS);
	if(digitalRead((TOGGLE_IN) == HIGH) && ((millis() - last_toggle) > TOGGLE_DELAY_MS)){
		last_toggle = millis();
		mode += 1;
	}
	switch(mode){
		case 0:
      //Switch Color To Black
      setColor(&pwm_color, 0, 0, 0);
      mode += 1;
		break;
    case 1:
      //Lights Out --> Black is static
    break;
		case 2:
      //Rainbow
			if(millis() - last_change > colorchange_delay){
				color_ctr = color_ctr >= RBOW_SIZE ? 0 : color_ctr + 1;
        byte value = (color_ctr / RBOW_SIZE) * pwm_period_ms;
        switch(map(color_ctr, 0, RBOW_SIZE, 0, 3)){
          case 0:
            setColor(&pwm_color, pwm_period_ms - value, value, 0);
            break;
          case 1:
            setColor(&pwm_color, 0, pwm_period_ms - value, value);
            break;
          case 2:
            setColor(&pwm_color, value, 0, pwm_period_ms - value);
            break;
        }
				last_change = millis();
			}
		break;

		case 3:
			//Color from RBOW as static		
		break;

		case 4:
      if(millis() - last_change > (colorchange_delay <= 3*MAX_DELAY_MS/4) ? colorchange_delay : (10/3)){
        msg_ctr = msg_ctr >= msg.length() ? 0 : msg_ctr + 1;
        
        setColor(&pwm_color, 
          msg[floor(msg_ctr / 8)] & (0x1 << (msg_ctr % 8)) ? 255 : 0,
          msg[floor(msg_ctr / 8)] & (0x2 << (msg_ctr % 8)) ? 255 : 0,
          msg[floor(msg_ctr / 8)] & (0x4 << (msg_ctr % 8)) ? 255 : 0);
        last_change = millis();
      }
			//SECRET
		break;
		default:
			mode = 0;
		break; 
	}

  //PWM
  d_time = millis() - pwm_timer;
  if(d_time >= pwm_period_ms){
      pwm_timer = millis();
  }
  digitalWrite(R_PORT, d_time > pwm_color.r ? HIGH : LOW);
  digitalWrite(G_PORT, d_time > pwm_color.g ? HIGH : LOW);
  digitalWrite(B_PORT, d_time > pwm_color.b ? HIGH : LOW);
}
