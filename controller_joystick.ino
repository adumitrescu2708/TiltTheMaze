#include <SoftwareSerial.h>
#include <Servo.h>


#define JOYSTICK_DEADZONE_INF  (510)
#define JOYSTICK_DEADZONE_SUP  (526)
#define JOYSTICK_DEADZONE_MID  (522)
#define JOYSTICK_BUTTON        (PD2)
#define DEFAULT_SERVO_POS       35

#define UP 1
#define DOWN 2

SoftwareSerial mySerial(10, 11);
Servo servo_ox;
Servo servo_oy;

int joystick_deadzone = 200;
bool is_start_game;
volatile byte old_value;
volatile bool is_button_pressed = false;
volatile bool is_button_pressed_second = false;
int previous_ox, previous_oy;
bool input_delayed = false;
bool is_servo_set = false;
int chosen_game_mode = 0;
volatile unsigned long seconds = 0;
unsigned long startTime;
bool changed = false;

void set_random_position()
{
  unsigned long currentTime = millis();
  seconds = (currentTime - startTime) / 1000;
  if(seconds % 10 == 0) {
    int random_position_ox = random(45);
    if(random_position_ox <= 25) {
      random_position_ox = 27;
    }

    int random_position_oy = random(45);
    if(random_position_oy <= 25) {
      random_position_oy = 27;
    }

    if(previous_ox < random_position_ox) {
      for(int grade = previous_ox; grade <= random_position_ox; grade++) {
        servo_ox.write(grade);
        delay(35);
      }        
    } else {
       for(int grade = previous_ox; grade >= random_position_ox; grade--) {
        servo_ox.write(grade);
        delay(35);
      }         
    }

    previous_ox = random_position_ox;

    if(previous_oy <= random_position_oy) {
      for(int grade = previous_oy; grade <= random_position_oy; grade++) {
        servo_oy.write(grade);
        delay(25);
      }          
    } else {
      for(int grade = previous_oy; grade >= random_position_oy; grade--) {
        servo_oy.write(grade);
        delay(25);
      }         
    }
  
    previous_oy = random_position_oy;    
    seconds = 0;
  }
}

void setup() {
  mySerial.begin(9600);

  DDRC &= ~(1 << PC2);
  PORTC |= (1 << PC2);

  DDRC &= ~(1 << PC3);
  PORTC |= (1 << PC3);
 
  DDRD &= ~(1 << JOYSTICK_BUTTON);
  PORTD |= (1 << JOYSTICK_BUTTON);

  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0);
  
  sei();

  is_start_game = false;
  previous_ox = previous_oy = DEFAULT_SERVO_POS;
  unsigned long seed = analogRead(A0);
  randomSeed(seed);
  startTime = millis();
}
void init_servo() {
  seconds = 0;
   servo_ox.attach(8);
   servo_oy.attach(9);
   servo_ox.write(DEFAULT_SERVO_POS);
   servo_oy.write(DEFAULT_SERVO_POS);   
}

void loop() {
  int ox = analogRead(A3);
  int oy = analogRead(A2);
   
  if (abs(ox - 512) < joystick_deadzone) {
    ox = 512;
  } else {
    ox = map(ox, 0, 1023, 0, 1023);
  }

  if (abs(oy - 512) < joystick_deadzone) {
    oy = 512;
  } else {
    oy = map(oy, 0, 1023, 0, 1023);
  }

  if(mySerial.available() >= 1) {
      byte finished = mySerial.read();
      is_start_game = true; 
      chosen_game_mode = finished;
  }

  int servo_ox_pos = map(ox, 0, 1023, 25, 45);
  int servo_oy_pos = map(oy, 0, 1023, 25, 45);

  old_value = PIND & (1 << JOYSTICK_BUTTON);

  if(!is_start_game) {

      byte button = 0;
      if(is_button_pressed)
       button |= 0x01;
      
      byte choose_game_cursor = 0;
      if(servo_oy_pos > 40) {
        choose_game_cursor = 1;
      } else if (servo_oy_pos < 30){
         choose_game_cursor = 2;
      } else {
        choose_game_cursor = 0;
      }
    
       if(choose_game_cursor == 1) {
         button |= (1 << 1);
       }
       if(choose_game_cursor == 2) {
          button |= (1 << 2);
       }
       if(is_button_pressed_second) {
          button |= (1 << 3); 
       }   
       mySerial.write(button);    
  } else {
      if(!is_servo_set) {
        mySerial.end();
        init_servo();
        is_servo_set = true;
        seconds = 0;
      }
      if(previous_ox < servo_ox_pos) {
        for(int grade = previous_ox; grade <= servo_ox_pos; grade++) {
          servo_ox.write(grade);
          delay(35);
        }        
      } else {
         for(int grade = previous_ox; grade >= servo_ox_pos; grade--) {
          servo_ox.write(grade);
          delay(35);
        }         
      }

      previous_ox = servo_ox_pos;

      if(previous_oy <= servo_oy_pos) {
        for(int grade = previous_oy; grade <= servo_oy_pos; grade++) {
          servo_oy.write(grade);
          delay(25);
        }          
      } else {
        for(int grade = previous_oy; grade >= servo_oy_pos; grade--) {
          servo_oy.write(grade);
          delay(25);
        }         
      }
    
      previous_oy = servo_oy_pos;
  }
  if(chosen_game_mode == 2) {
    set_random_position();
   }
}
ISR(INT0_vect) {
  int value = PIND & (1 << JOYSTICK_BUTTON);
  if(is_button_pressed == true && old_value != 0) {
    is_button_pressed_second = true;
  }
  if(value == 0) {
    is_button_pressed = true;  
  }
  old_value = value;
  delay(90);
}
