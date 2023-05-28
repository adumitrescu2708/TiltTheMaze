 #include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#define UP 1
#define LOWER 2

#define HOLE_1 2
#define HOLE_2 3
#define HOLE_3 4
#define HOLE_4 5
#define HOLE_5 13

#define HOLE_4_SENSOR PD6 // pt ledul 4
#define HOLE_1_SENSOR PD7 // pt ledul 1
#define HOLE_3_SENSOR PB0 // pt ledul 3 - 8
#define HOLE_2_SENSOR PB1 // pt ledul 2 - 9
#define HOLE_5_SENSOR PB4 // pt ledul 5 - 12

bool is_set_hole1 = false;
bool is_set_hole2 = false;
bool is_set_hole3 = false;
bool is_set_hole4 = false;
bool is_set_hole5 = false; 

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(10, 11);

bool start_game, set_game_mode, finished;
byte previous_cursor_state = 0;
int last_game_picked, elapsedTime = 0;
int chosen_hole, chosen_game_mode;
int counter = 0;
volatile unsigned long seconds = 0;

void set_LCD_start_game() {
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print(">> Send the ball");
  lcd.setCursor(3,1);
  lcd.print("Lose the ball");

}

void set_start_screen()
{
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Start");
}

void parse_cursor(byte cursor_LCD) {
  lcd.clear(); 
  if(cursor_LCD == 1) {
    lcd.setCursor(3,0);
    lcd.print("Send the ball");
    lcd.setCursor(0,1);
    lcd.print(">> Lose the ball");
  } else {
    lcd.setCursor(0,0);
    lcd.print(">> Send the ball");
    lcd.setCursor(3,1);
    lcd.print("Lose the ball");   
  }
}

bool wait_finish_1st_game(int hole) 
{

      bool win = false;
      bool wrong_hole = false;
      int sensorValue1 = PIND & (1 << HOLE_1_SENSOR);
      int sensorValue2 = PINB & (1 << HOLE_2_SENSOR);
      int sensorValue3 = PINB & (1 << HOLE_3_SENSOR);
      int sensorValue4 = PIND & (1 << HOLE_4_SENSOR);
      int sensorValue5 = PINB & (1 << HOLE_5_SENSOR);
     if(hole == HOLE_1) {
        win = sensorValue1 == 0 ? true : false;
        if(sensorValue2 == 0 || sensorValue3 == 0 || sensorValue4 == 0 || sensorValue5 == 0) {
          wrong_hole = true;
        }
     }
     if(hole == HOLE_2) {
        win = sensorValue2 == 0 ? true : false;
        if(sensorValue1 == 0 || sensorValue3 == 0 || sensorValue4 == 0 || sensorValue5 == 0) {
          wrong_hole = true;
        }
     }
      if(hole == HOLE_3) {
        win = sensorValue3 == 0 ? true : false;
        if(sensorValue1 == 0 || sensorValue2 == 0 || sensorValue4 == 0 || sensorValue5 == 0) {
          wrong_hole = true;
        }
     }
      if(hole == HOLE_4) {
        win = sensorValue4 == 0 ? true : false;
        if(sensorValue2 == 0 || sensorValue3 == 0 || sensorValue1 == 0 || sensorValue5 == 0) {
          wrong_hole = true;
        }
     }
      if(hole == HOLE_5) {
        win = sensorValue5 == 0 ? true : false;
        if(sensorValue2 == 0 || sensorValue3 == 0 || sensorValue4 == 0 || sensorValue1 == 0) {
          wrong_hole = true;
        }
     }
     if(win) {
      lcd.clear();
      lcd.setCursor(0, 1);
      counter = seconds;
      lcd.print("SUCCESS!   ");
      lcd.print(counter);
      lcd.print("sec");
      return true;
     }
     if(wrong_hole) {
      lcd.clear();
      lcd.setCursor(0, 1);
      counter = seconds;
      lcd.print("FAIL!   ");
      lcd.print(counter);
      lcd.print("sec");
      return true;       
     }
  return false;
}

bool wait_finish_2st_game(int hole) 
{

      bool win = false;
      bool wrong_hole = false;
      int sensorValue1 = PIND & (1 << HOLE_1_SENSOR);
      int sensorValue2 = PINB & (1 << HOLE_2_SENSOR);
      int sensorValue3 = PINB & (1 << HOLE_3_SENSOR);
      int sensorValue4 = PIND & (1 << HOLE_4_SENSOR);
      int sensorValue5 = PINB & (1 << HOLE_5_SENSOR);
     wrong_hole = sensorValue1 == 0 || sensorValue2 == 0 || sensorValue3 == 0 || sensorValue4 == 0 || sensorValue5 == 0 ? true : false;

     if(wrong_hole) {
      lcd.clear();
      lcd.setCursor(0, 1);
      counter = seconds;
      lcd.print("FAIL!   ");
      lcd.print(counter);
      lcd.print("sec");
      return true;       
     }
  return false;
}


void set_lose_ball() {
  int random_hole = random(5);
  if(random_hole == 0 || random_hole == 1 || random_hole == 2) {
    random_hole = 3;
  }
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("GO!");
  
  digitalWrite(random_hole, HIGH);
  chosen_hole = random_hole;
  seconds = 0;

}
void set_save_ball() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("SAVE THE BALL!");
  seconds = 0;
}

void setup() {
  mySerial.begin(9600);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(4,0);
  lcd.print("Welcome!");
  lcd.setCursor(0, 1);
  lcd.print("Push to start...");

  start_game = false;
  set_game_mode = false;
  finished = false;

  DDRD |= (1 << PD2);
  DDRD |= (1 << PD3);
  DDRD |= (1 << PD4);
  DDRD |= (1 << PD5);
  
  randomSeed(analogRead(0));

  DDRD &= ~(1 << HOLE_4_SENSOR);
  PORTD |= (1 << HOLE_4_SENSOR);
  DDRD &= ~(1 << HOLE_1_SENSOR);
  PORTD |= (1 << HOLE_1_SENSOR);
  DDRB &= ~(1 << HOLE_3_SENSOR);
  PORTB |= (1 << HOLE_3_SENSOR);
  DDRB &= ~(1 << HOLE_2_SENSOR);
  PORTB |= (1 << HOLE_2_SENSOR);
  DDRB &= ~(1 << HOLE_5_SENSOR);
  PORTB |= (1 << HOLE_5_SENSOR);

  chosen_game_mode = 0;
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 62499;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12);
  TIMSK1  |= (1 << OCIE1A);

  sei();
}


void loop() {
  if(mySerial.available() >= 2) {
    byte received = mySerial.read();
    byte button_pressed = (received & 0x01);
    byte upper_cursor = (received >> 1) & 0x01;
    byte lower_cursor = (received >> 2) & 0x01;
    byte second_button_pressed = (received >> 3) & 0x01;
 
    if(button_pressed && !start_game) {
      start_game = true;
      set_LCD_start_game();
    }

    if(start_game && !set_game_mode && (upper_cursor != 0 || lower_cursor != 0)) {
      last_game_picked = upper_cursor != 0 ? 1 : 2; 
      byte cursor_LCD = upper_cursor != 0 ? 1 : 0;
      if(cursor_LCD != previous_cursor_state) {
        previous_cursor_state = cursor_LCD;
        parse_cursor(cursor_LCD);
      }
    }

    if(start_game && !set_game_mode && second_button_pressed) {
      set_game_mode = true;
      set_start_screen();
      byte sync_game_mode;
      if(last_game_picked == 1) {
        set_lose_ball();
        chosen_game_mode = 1;
        sync_game_mode = 1;
      } else {
        set_save_ball();
        chosen_game_mode = 2;
        sync_game_mode = 2;
      }
      byte sync = 1;
      mySerial.write(sync_game_mode);
      mySerial.end();
    }
  }

  if(set_game_mode && chosen_game_mode == 1 && !finished) {
      finished = wait_finish_1st_game(chosen_hole);
  }
  if(set_game_mode && chosen_game_mode == 2 && !finished) {
    finished = wait_finish_2st_game(chosen_hole);
  }

}
ISR(TIMER1_COMPA_vect) {
  seconds++;
}
