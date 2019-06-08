#include <SPI.h>  
#include <Pixy.h>
#include <math.h> 
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Pixy pixy;


#define X_CENTER        ((PIXY_MAX_X-PIXY_MIN_X)/2)       
#define Y_CENTER        ((PIXY_MAX_Y-PIXY_MIN_Y)/2)
int32_t heightmiddle=70;   

 
int Filter_Value_X;
int Value_X;
int Filter_Value_Y;
int Value_Y;
int Filter_Value_Z;
int Value_Z;

class ServoLoop
{
public:
  ServoLoop(int32_t pgain, int32_t dgain);

  void update(int32_t error);
   
  int32_t m_pos;
  int32_t m_prevError;
  int32_t m_pgain;
  int32_t m_dgain;
};


ServoLoop panLoop(500, 600);
ServoLoop tiltLoop(300, 275);
ServoLoop areaLoop(250, 210);

ServoLoop::ServoLoop(int32_t pgain, int32_t dgain)
{
  m_pos = PIXY_RCS_CENTER_POS;
  m_pgain = pgain;
  m_dgain = dgain;
  m_prevError = 0x80000000L;
}

void ServoLoop::update(int32_t error)
{
  long int vel;
  char buf[32];
  if (m_prevError!=0x80000000)
  {	
    vel = (error*m_pgain + (error - m_prevError)*m_dgain)>>10;
    m_pos += vel;
    if (m_pos>PIXY_RCS_MAX_POS) 
      m_pos = PIXY_RCS_MAX_POS; 
    else if (m_pos<PIXY_RCS_MIN_POS) 
      m_pos = PIXY_RCS_MIN_POS;
  }
  m_prevError = error;
}



void setup()
{
  Serial.begin(9600);
  Serial.print("Starting...\n");
   pixy.init();
   pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  Value_X=500;
  Value_Y=800;
  Value_Z=1000;
}



void loop()
{ 
  static int i = 0;
  int j,p,t,z, pTEST;
  uint16_t blocks;
  char buf[32]; 
  int32_t panError, tiltError, areaError;
  
  blocks = pixy.getBlocks();
  
  if (blocks)
  {
     /* a =pixy.blocks[0].width * pixy.blocks[0].height;
      a=sqrt(a);
       if(a>60)  l = -9.5339e-06*pow(a,3) + 0.0040711*pow(a,2)- 0.62826*a + 41.3494;
           else if (a<60 && a>30)
                l=-0.00024666*pow(a,3) + 0.040264*pow(a,2) - 2.5736*a + 78.1414;
           else 
                l=-0.0032999 *pow(a,3) + 0.26089*pow(a,2) - 7.8645*a + 120.0977;    
    // areaError =pixy.blocks[0].height - heightmiddle;    // heightmiddle不能直接写数字*/ 
  
    
    areaError=Filter()-heightmiddle;
    panError = X_CENTER-pixy.blocks[0].x;
    tiltError =pixy.blocks[0].y-Y_CENTER;
   
    
    panLoop.update(panError);
    tiltLoop.update(tiltError);
    areaLoop.update(areaError);
     
    Filter_Value_X = Filter_X();       // 获得滤波器输出值
    Value_X = Filter_Value_X;  
    Filter_Value_Y = Filter_Y();       // 获得滤波器输出值
    Value_Y = Filter_Value_Y;  

    
      p = map(panLoop.m_pos,0,1000,150,500); 
       pTEST = map(panLoop.m_pos,0,1000,0,320); 
      t = map(Filter_Y(),0,1000,350,520); 
      z = map(areaLoop.m_pos,0,1000,350,135); 
      //z = map(Filter_Value_Z,0,1000,135,350); 
    
    pwm.setPWM(0, 0, t);     
    pwm.setPWM(8, 0, p);      

    pwm.setPWM(15, 0, z);     //减小向后仰，增大前伸

    /*  
   Serial.print(pixy.blocks[0].x);
     Serial.print("\t");
     Serial.print(pixy.blocks[0].y);
      Serial.print("\t");*/
    Serial.print( sqrt(pixy.blocks[0].width * pixy.blocks[0].height));
     Serial.print("\t");
     Serial.print("\t");
       Serial.print(pixy.blocks[0].height);
        Serial.print("\n");
   delay (1000);
  
  }  
}


#define FILTER_N 20
int Filter() 
{
   int i, j;
   int filter_temp, filter_sum = 0;
   int filter_buf[FILTER_N];
  for(i = 0; i < FILTER_N; i++) 
  {
    filter_buf[i] = sqrt(pixy.blocks[0].width * pixy.blocks[0].height);
    delay(1);
   }
   // 采样值从小到大排列（冒泡法）
   for(j = 0; j < FILTER_N - 1; j++) {
     for(i = 0; i < FILTER_N - 1 - j; i++) {
     if(filter_buf[i] > filter_buf[i + 1]) {
         filter_temp = filter_buf[i];
         filter_buf[i] = filter_buf[i + 1];
         filter_buf[i + 1] = filter_temp;
       }
     }
 }
     // 去除最大最小极值后求平均
  for(i = 1; i < FILTER_N - 1; i++) filter_sum += filter_buf[i];
   return filter_sum / (FILTER_N - 2);
}




#define FILTER_X 40
int Filter_X() 
{
  int NewValue_X;
  NewValue_X = panLoop.m_pos;
  if(((NewValue_X - Value_X) > FILTER_X) || ((Value_X - NewValue_X) > FILTER_X))
    return Value_X;
  else
    return NewValue_X;
}


/*#define FILTER_Y 60
int Filter_Y()
{
  int NewValue_Y;
  NewValue_Y = tiltLoop.m_pos;
  if(((NewValue_Y - Value_Y) > FILTER_Y) || ((Value_Y - NewValue_Y) > FILTER_Y))
    return Value_Y;
  else
    return NewValue_Y;
}*/

#define FILTER_N 20
int Filter_Y() 
{
   int i, j;
   int filter_temp, filter_sum = 0;
   int filter_buf[FILTER_N];
  for(i = 0; i < FILTER_N; i++) 
  {
    filter_buf[i] = tiltLoop.m_pos;
    delay(1);
   }
   // 采样值从小到大排列（冒泡法）
   for(j = 0; j < FILTER_N - 1; j++) {
     for(i = 0; i < FILTER_N - 1 - j; i++) {
     if(filter_buf[i] > filter_buf[i + 1]) {
         filter_temp = filter_buf[i];
         filter_buf[i] = filter_buf[i + 1];
         filter_buf[i + 1] = filter_temp;
       }
     }
 }
     // 去除最大最小极值后求平均
  for(i = 1; i < FILTER_N - 1; i++) filter_sum += filter_buf[i];
   return filter_sum / (FILTER_N - 2);
}
