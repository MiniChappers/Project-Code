#ifndef U_MAP
#define U_MAP


#include <MPU6050_tockn.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Keypad.h>


#define ROW_NUM     4 // four rows
#define COLUMN_NUM  3 // three columns
#define I2C_SLAVE_ADDR 0x04    //  iic从机地址

#define key_0 0x30  
#define key_1 0x31
#define key_2 0x32
#define key_3 0x33
#define key_4 0x34
#define key_5 0x35
#define key_6 0x36
#define key_7 0x37
#define key_8 0x38
#define key_9 0x39
#define key_x 0x2a
#define key_h 0x23


bool FORWARD = false;    //向前指令标志
bool BACKWARD = false;   //向后指令标志
bool ALEFT = false;      //向左指令标志
bool RIGHT = false;    //向右指令标志
bool STOP = false;   //停止指令标志
bool flag_send = false;   // iic发送数据指令标志

bool old_control = false;

bool CW_2R_5 = false;
bool CCW_2R_0 = false;

bool left_U = false;
bool left_D = false;
bool right_U = false;
bool right_D = false;

bool flag_D = false;
bool flag_U = false;






#endif
