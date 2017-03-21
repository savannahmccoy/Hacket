#include <Spectrum.h>
#include <Matrix.h>
#include <TimedAction.h>


int rowPins[5] = {2, 3, 6, 7, 8};
int colPins[5] = {9, 10, 11, 12, 13};

int specA[7] = {0};
int specB[7] = {0};
int blocked[4][4] = {{0, 0 ,0 ,0}, {0, 0 ,0 ,0}, {0, 0 ,0 ,0}, {0, 0 ,0 ,0}};
int max = 0;
int min = 0;
int reset = 5;
int strobe = 4;
int chA = A0;
int chB = A1;
int scaleFactor = 256;
float avg = 0;
char cmd;
bool doBlink = false;
int blinker = 0;
int idx = 0;

Matrix mat(rowPins, colPins, 5, 5);
Spectrum spc(reset, strobe, chA, chB);

void setup()
{
  Serial.begin(9600);
  mat.setup();
  spc.setup();
}

TimedAction updateThread = TimedAction(0.001, updateDisplay);
TimedAction setThread = TimedAction(50, set);
TimedAction resetThread = TimedAction(20000, doReset);
TimedAction musicThread = TimedAction(25, music);
TimedAction cmdThread = TimedAction(500, getCmd);
TimedAction blinkThread = TimedAction(500, blink);

void loop()
{
   updateThread.check();
   resetThread.check();
   musicThread.check();
   // setThread.check();
   cmdThread.check();
    //Serial.print("print");
   // blinkThread.check();
}

void updateDisplay()
{
  mat.update();
}

void set()
{
  Serial.println(idx);
  
  if (idx < 25)
  {
    mat.set(idx, 1);
  } else {
    mat.set(idx-25, 0);
  }
  idx = (idx + 1)%25;
}
void music()
{
  scaleFactor = max/5;
  spc.readData(specA, specB);
  
  for(int i = 0; i < 5; i++)
  {
    for(int j = 0; j < 5; j++)
    {
      int val = spc.getAmp(i);
      if((j <= val/scaleFactor) && (!blocked[j][i]))
      {
        mat.set(j, i, 1);
      } else
      {
        mat.set(j, i, 0);
      }
      if(max < val)
      {
        max = val;
      }
    }
  }
}

void doReset()
{
  max = 0;
  min = 1024;

}

void getCmd()
{
  if(Serial.available() > 0)
  {
    cmd = Serial.read();
    Serial.print(cmd);
    if(strcmp (cmd,'b') == 0)
    {
      block(0, 3, 1, 2);
      doBlink = true;
    } else if(strcmp (cmd,'u') == 0)
    {
      unblock(0, 3, 1, 2);
      doBlink = false;
    }
  }
}

void blink()
{
    if(doBlink)
    {
      for(int i = 0; i < 5; i++)
      {
        for(int j =  0; j < 5; j++)
        {
          if(blocked[i][j])
          {
            mat.set(i, j, blinker);
          }
        }
      }
      blinker= (blinker+1)%2;
    }

}

void block(int minRow, int maxRow, int minCol, int maxCol)
{
  for(int i = minRow; i <= maxRow; i++)
  {
    for(int j = minCol; j <= maxCol; j++)
    {
      blocked[i][j] = 1;
    }
  }
}

void unblock(int minRow, int maxRow, int minCol, int maxCol)
{
  for(int i = minRow; i <= maxRow; i++)
  {
    for(int j = minCol; j <= maxCol; j++)
    {
      blocked[i][j] = 0;
    }
  }
}