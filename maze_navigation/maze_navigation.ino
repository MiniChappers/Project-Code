
#include "E_map.h"


// 创建MPU6050对象
MPU6050 mpu6050(Wire);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(19, 40, 18, 17, 16, 15);

byte pin_rows[ROW_NUM] = {14, 13, 12, 11}; // GIOP18, GIOP5, GIOP17, GIOP16 connect to the row pins
byte pin_column[COLUMN_NUM] = {10, 7, 6};  // GIOP4, GIOP0, GIOP2 connect to the column pins


int left_motorspeed = 100;
int right_motorspeed = 100;
int servoangle = 87;
char key;
int distance = 0;
float cmdDistance = 0;
float distanceCm;
long receivedValue1;
long receivedValue2;
byte receivedByte1;
byte receivedByte2;

int set_distance = 0;
int cmd_num;
float old_yaw;
int yaw_bias;
char old_one = 0;
char old_two = 0;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};


Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );


void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.write("Input string .. ");
  lcd.setCursor(0, 1);//前面那个数是列，后面那个数是行
  Serial.begin(115200);
  Wire.begin();


  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  delay(1000);
  mpu6050.update();
  old_yaw = mpu6050.getAngleZ();  //获取上电后第一次的航向角，作为参考角度
  delay(100);

}

void loop() {

  mpu6050.update();
  float yaw = mpu6050.getAngleZ(); //获取航向角


  //float yaw =  180;


  key = keypad.getKey();


  Serial.println(yaw); Serial.println("   ");
  Serial.println(distanceCm);
  delay(10);

  Wire.requestFrom(I2C_SLAVE_ADDR, sizeof(byte) * 2);
  if (Wire.available() >= sizeof(byte) * 2) {

    // reading the data
    byte receivedByte1 = Wire.read();
    byte receivedByte2 = Wire.read();
    if (receivedValue1 < receivedByte2) {
      distance++;
    }
    if (receivedValue1 > receivedByte2) {
      distance--;
    }
    receivedValue1 = receivedByte2;
  }
  distanceCm = distance * 0.2;



  if (key == key_h) {
    key = 0;
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("STOP");
    FORWARD = false;
    BACKWARD = false;
    ALEFT = false;
    RIGHT = false;
    stopmotor();
  } else if (key == key_x) {
    key = 0;
    old_control = true;
    if (old_one == key_2) {
      FORWARD = true;
      key = old_two;
    } else if (old_one == key_8) {
      BACKWARD = true;
      key = old_two;
    } else if (old_one == key_4) {
      RIGHT = true;
      key = old_two;
    } else if (old_one == key_6) {
      RIGHT = true;
      key = old_two;
    } else if (old_one == key_5) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("CW 360");
      right_U = true;
      servoangle = 56;
      left_motorspeed = 130;
      right_motorspeed = 100;
      yaw_bias = 350;
      Wire.beginTransmission(4);
      Wire.write((byte)((left_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(left_motorspeed & 0x000000FF));
      Wire.write((byte)((right_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(right_motorspeed & 0x000000FF));
      Wire.write((byte)((servoangle & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(servoangle & 0x000000FF));
      Wire.endTransmission(); // stop transmitting
      delay(10);
      key = 0;
    } else if (old_one == key_0) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("CCW 360");
      right_U = true;
      servoangle = 56;
      left_motorspeed = -130;
      right_motorspeed = -100;
      yaw_bias = 350;
      Wire.beginTransmission(4);
      Wire.write((byte)((left_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(left_motorspeed & 0x000000FF));
      Wire.write((byte)((right_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(right_motorspeed & 0x000000FF));
      Wire.write((byte)((servoangle & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(servoangle & 0x000000FF));
      Wire.endTransmission(); // stop transmitting
      delay(10);
      key = 0;
    }
  }

  if (!FORWARD && !BACKWARD && !ALEFT && !RIGHT && !CW_2R_5 && !CCW_2R_0) {


    if (key == key_2)
    {
      old_one = key;
      FORWARD = true;
      key = 0;

    } else if (key == key_8) {
      BACKWARD = true;
      old_one = key;
      key = 0;
    } else if (key == key_4) {
      ALEFT = true;
      old_one = key;
      key = 0;
    }
    else if (key == key_6) {
      RIGHT = true;
      old_one = key;
      key = 0;
    } else if (key == key_5) {
      old_one = key;
      CW_2R_5 = true;
      key = 0;
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("CW 360");
      right_U = true;
      servoangle = 56;
      left_motorspeed = 130;
      right_motorspeed = 100;
      yaw_bias = yaw + 359;
      if (yaw_bias > 360) yaw_bias = yaw_bias - 360;
      if (yaw_bias < -360) yaw_bias = yaw_bias + 360;
      Wire.beginTransmission(4);
      Wire.write((byte)((left_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(left_motorspeed & 0x000000FF));
      Wire.write((byte)((right_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(right_motorspeed & 0x000000FF));
      Wire.write((byte)((servoangle & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(servoangle & 0x000000FF));
      Wire.endTransmission(); // stop transmitting
      delay(10);

    } else if (key == key_0) {
      old_one = key;
      key = 0;
      CCW_2R_0 = true;
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("CCW 360");
      right_D = true;
      servoangle = 56;
      left_motorspeed = -130;
      right_motorspeed = -100;
      yaw_bias = yaw - 359;
      if (yaw_bias > 360) yaw_bias = yaw_bias - 360;
      if (yaw_bias < -360) yaw_bias = yaw_bias + 360;
      Wire.beginTransmission(4);
      Wire.write((byte)((left_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(left_motorspeed & 0x000000FF));
      Wire.write((byte)((right_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(right_motorspeed & 0x000000FF));
      Wire.write((byte)((servoangle & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(servoangle & 0x000000FF));
      Wire.endTransmission(); // stop transmitting
      delay(10);
    }

    CW_2R_5 = false;
    CCW_2R_0 = false;

  } else {
    old_two = key;
    switch (key) {

      case key_1:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = true;
          flag_D = false;
          lcd.print("Forward 10cm");
          servoangle = 87;
          left_motorspeed = 200;
          right_motorspeed = 200;
          set_distance = abs(distanceCm) + 10;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = false;
          flag_D = true;
          lcd.print("Backward 10cm");
          servoangle = 87;
          left_motorspeed = -200;
          right_motorspeed = -200;
          set_distance = abs(distanceCm) - 10;

        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          left_U = true;
          left_D = false;
          lcd.print("Aleft CW 90°");
          servoangle = 120;
          left_motorspeed = 100;
          right_motorspeed = 130;
          yaw_bias = yaw + 90;
          if (yaw_bias > 360) yaw_bias = yaw_bias - 360;
          if (yaw_bias < -360) yaw_bias = yaw_bias + 360;
        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;
        }

        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");


        flag_send = true;
        break;

      case key_2:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = true;
          flag_D = false;
          lcd.print("Forward 20cm");
          servoangle = 87;
          left_motorspeed = 200;
          right_motorspeed = 200;
          set_distance = abs(distanceCm) + 20;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = false;
          flag_D = true;
          lcd.print("Backward 20cm");
          servoangle = 87;
          left_motorspeed = -200;
          right_motorspeed = -200;
          set_distance = abs(distanceCm) - 20;

        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;

        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;
        }
        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");

        flag_send = true;

        break;

      case key_3:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = true;
          flag_D = false;
          lcd.print("Forward 30cm");
          servoangle = 87;
          left_motorspeed = 200;
          right_motorspeed = 200;
          set_distance = abs(distanceCm) + 30;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = false;
          flag_D = true;
          lcd.print("Backward 30cm");
          servoangle = 87;
          left_motorspeed = -200;
          right_motorspeed = -200;
          set_distance = abs(distanceCm) - 30;
        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;

        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          right_U = true;
          right_D = false;
          lcd.print("Right CW 90°");

          yaw_bias = yaw + 90;
          if (yaw_bias > 360) yaw_bias = yaw_bias - 360;
          if (yaw_bias < -360) yaw_bias = yaw_bias + 360;
          servoangle = 56;
          left_motorspeed = 130;
          right_motorspeed = 100;
        }
        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");
        flag_send = true;

        break;

      case key_4:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("Forward 40cm");
          servoangle = 87;
          left_motorspeed = 200;
          right_motorspeed = 200;
          flag_U = true;
          flag_D = false;
          set_distance = abs(distanceCm) + 40;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = false;
          flag_D = true;
          lcd.print("Backward 40cm");
          servoangle = 87;
          left_motorspeed = -200;
          right_motorspeed = -200;
          set_distance = abs(distanceCm) - 40;

        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          left_U = true;
          left_D = false;
          lcd.print("Aleft CW 180°");
          yaw_bias = yaw + 180;
          if (yaw_bias > 360) yaw_bias = yaw_bias - 360;
          if (yaw_bias < -360) yaw_bias = yaw_bias + 360;
          servoangle = 120;
          left_motorspeed = 100;
          right_motorspeed = 130;

        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;
        }
        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");


        flag_send = true;

        break;

      case key_5:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = true;
          flag_D = false;
          lcd.print("Forward 50cm");
          servoangle = 87;
          left_motorspeed = 200;
          right_motorspeed = 200;
          set_distance = abs(distanceCm) + 50;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = false;
          flag_D = true;
          lcd.print("Backward 50cm");
          servoangle = 87;
          left_motorspeed = -200;
          right_motorspeed = -200;
          set_distance = abs(distanceCm) - 50;

        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;

        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;
        }
        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");


        flag_send = true;

        break;

      case key_6:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = true;
          flag_D = false;
          lcd.print("Forward 60cm");
          servoangle = 87;
          left_motorspeed = 200;
          right_motorspeed = 200;
          set_distance = abs(distanceCm) + 60;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = false;
          flag_D = true;
          lcd.print("Backward 60cm");
          servoangle = 87;
          left_motorspeed = -200;
          right_motorspeed = -200;
          set_distance = abs(distanceCm) - 60;
        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;

        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          right_U = true;
          right_D = false;
          lcd.print("Right CW 180°");

          yaw_bias = yaw + 180;
          if (yaw_bias > 360) yaw_bias = yaw_bias - 360;
          if (yaw_bias < -360) yaw_bias = yaw_bias + 360;
          servoangle = 56;
          left_motorspeed = 130;
          right_motorspeed = 100;
        }
        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");

        flag_send = true;

        break;

      case key_7:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("Forward 70cm");
          servoangle = 87;
          left_motorspeed = 200;
          right_motorspeed = 200;
          flag_U = true;
          flag_D = false;
          set_distance = abs(distanceCm) + 70;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = false;
          flag_D = true;
          lcd.print("Backward 70cm");
          servoangle = 87;
          left_motorspeed = -200;
          right_motorspeed = -200;
          set_distance = abs(distanceCm) - 70;

        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          left_U = false;
          left_D = true;
          lcd.print("Aleft CCW 90°");
          yaw_bias = yaw - 90;
          if (yaw_bias > 360) yaw_bias = yaw_bias - 360;
          if (yaw_bias < -360) yaw_bias = yaw_bias + 360;
          servoangle = 120;
          left_motorspeed = -100;
          right_motorspeed = -130;

        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;
        }
        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");
        flag_send = true;

        break;

      case key_8:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("Forward 80cm");
          servoangle = 87;
          left_motorspeed = 200;
          right_motorspeed = 200;
          flag_U = true;
          flag_D = false;
          set_distance = abs(distanceCm) + 80;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = false;
          flag_D = true;
          lcd.print("Backward 80cm");
          servoangle = 87;
          left_motorspeed = -200;
          right_motorspeed = -200;
          set_distance = abs(distanceCm) - 80;
        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;

        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;
        }
        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");
        flag_send = true;

        break;

      case key_9:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("Forward 90cm");
          servoangle = 87;
          left_motorspeed = 200;
          right_motorspeed = 200;
          flag_U = true;
          flag_D = false;
          set_distance = abs(distanceCm) + 90;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          flag_U = false;
          flag_D = true;
          lcd.print("Backward 90cm");
          servoangle = 87;
          left_motorspeed = -200;
          right_motorspeed = -200;
          set_distance = abs(distanceCm) - 90;
        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;

        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          right_U = true;
          right_D = false;
          lcd.print("RIGHT CCW 90°");
          yaw_bias = yaw - 90;
          if (yaw_bias > 360) yaw_bias = yaw_bias - 360;
          if (yaw_bias < -360) yaw_bias = yaw_bias + 360;
          servoangle = 87;
          left_motorspeed = 0;
          right_motorspeed = 0;
        }
        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");
        flag_send = true;
        break;

      case key_0:
        lcd.clear();
        lcd.setCursor(4, 0);
        if (FORWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          //  servoangle = 87;
          //   left_motorspeed = 200;
          //   right_motorspeed = 200;
        } else if (BACKWARD) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          //  servoangle = 87;
          //   left_motorspeed = 200;
          //   right_motorspeed = 200;

        } else if (ALEFT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          //  servoangle = 87;
          //   left_motorspeed = 200;
          //   right_motorspeed = 200;

        } else if (RIGHT) {
          FORWARD = false;
          BACKWARD = false;
          ALEFT = false;
          RIGHT = false;
          lcd.print("error!,retype");
          //  servoangle = 87;
          //   left_motorspeed = 200;
          //   right_motorspeed = 200;
        }
        lcd.setCursor(0, 0) ;
        Serial.println("Increasespeed");

        // set_distance = 0;
        servoangle = 87;
        left_motorspeed = 0;
        right_motorspeed = 0;
        flag_send = true;

        break;

    }

    if (flag_send) {
      flag_send = false;
      Wire.beginTransmission(4);
      Wire.write((byte)((left_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(left_motorspeed & 0x000000FF));
      Wire.write((byte)((right_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(right_motorspeed & 0x000000FF));
      Wire.write((byte)((servoangle & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
      Wire.write((byte)(servoangle & 0x000000FF));
      Wire.endTransmission(); // stop transmitting
      delay(10);

    }


  }

  if (left_U) {
    if (yaw_bias - yaw >= 0) {
      if (yaw > yaw_bias ) {
        left_U = false;
        stopmotor();
      }
    } else {
      if (yaw > yaw_bias + 360 ) {
        left_U = false;
        stopmotor();
      }
    }
  }
  if (left_D) {
    if (yaw - yaw_bias >= 0) {
      if (yaw < yaw_bias ) {
        left_D = false;
        stopmotor();
      }
    } else {
      if (yaw + 360 < yaw_bias  ) {
        left_D = false;
        stopmotor();
      }
    }
  }

  if (right_U) {
    if (yaw_bias - yaw >= 0) {
      if (yaw > yaw_bias ) {
        right_U = false;
        stopmotor();
      }
    } else {
      if (yaw > yaw_bias + 360 ) {
        right_U = false;
        stopmotor();
      }
    }
  }
  if (right_D) {
    if (yaw - yaw_bias >= 0) {
      if (yaw < yaw_bias ) {
        right_D = false;
        stopmotor();
      }
    } else {
      if (yaw + 360 < yaw_bias  ) {
        right_D = false;
        stopmotor();
      }
    }
  }


  if (flag_U) {
    if (abs(distanceCm) > set_distance)  //如果nano传输过来的数据大于  set_distance 小车停止
    {
      stopmotor();
    }
  }
  if (flag_D) {
    if (abs(distanceCm) < set_distance)  //如果nano传输过来的数据大于  set_distance 小车停止
    {
      stopmotor();
    }
  }




}




/*
  函数功能：停止小车，通过iic发送pwm值和舵机值控制小车停止
  参数：无
  返回：无
*/
void stopmotor()
{
  servoangle = 87;
  left_motorspeed = 0;
  right_motorspeed = 0;
  Wire.beginTransmission(4);
  Wire.write((byte)((left_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
  Wire.write((byte)(left_motorspeed & 0x000000FF));
  Wire.write((byte)((right_motorspeed & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
  Wire.write((byte)(right_motorspeed & 0x000000FF));
  Wire.write((byte)((servoangle & 0x0000FF00) >> 8)); // first byte of x, containing bits 16 to 9
  Wire.write((byte)(servoangle & 0x000000FF));
  Wire.endTransmission(); // stop transmitting
}
