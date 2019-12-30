#pragma once

unsigned char newImage[30][40][3];
unsigned short avgImage[30][40][3];
unsigned char difference[30][40];
unsigned char distance[30][40];
unsigned char flames[32][41];
unsigned char flamesC[255][3];
long long frames[30][40];
int effect = 1;

void calibrateWall()
{
  calibration = 100;
}

void setEffect(int i)
{
  effect = i;
}

void initEffects()
{
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
      frames[y][x] = 0;

  for(int y = 0; y < 32; y++)
    for(int x = 0; x < 41; x++)
      flames[y][x] = 0;
      
  for(int c = 0; c < 255; c++)
  {
    flamesC[c][0] = c;
    flamesC[c][1] = (c * c) / (255);
    flamesC[c][2] = (c * c) / (255 * 8);
  }
  //flamesC[255][0] = 255;
  //flamesC[255][1] = 255;
  //flamesC[255][2] = 255;
}

void recalcAverage()
{
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
    {
      avgImage[y][x][0] = ((avgImage[y][x][0]) * 128 + newImage[y][x][0] * 128) >> 8;
      avgImage[y][x][1] = ((avgImage[y][x][1]) * 128 + newImage[y][x][1] * 128) >> 8;
      avgImage[y][x][2] = ((avgImage[y][x][2]) * 128 + newImage[y][x][2] * 128) >> 8;
    }
}

void calcDifference()
{
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
    {
      difference[y][x] = min(255, 
      abs((avgImage[y][x][0]) - newImage[y][x][0]) + 
      abs((avgImage[y][x][1]) - newImage[y][x][1]) + 
      abs((avgImage[y][x][2]) - newImage[y][x][2]));
    }
}

void downSample(unsigned char *frame)
{
  for(int y = 0; y < 30; y++)
  {
    for(int x = 0; x < 40; x++)
    {
      int r = 0;
      int g = 0;
      int b = 0;
      for(int j = 0; j < 4; j++)
        for(int i = 0; i < 4; i++)
        {
          unsigned char p[3];          
          getPixel(x * 4 + i, y * 4 + j, frame, p);
          r += p[0];
          g += p[1];
          b += p[2];
        }
      newImage[y][x][0] = r >> 4;
      newImage[y][x][1] = g >> 4;
      newImage[y][x][2] = b >> 4;
    }
  }
  /*
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++) 
      getPixel(x * 4 + 2, y * 4 + 2, frame, newImage[y][x]);*/
}

void calcDistance()
{
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
      if(difference[y][x] > 128)
        distance[y][x] = 0;
      else
        distance[y][x] = 255;

  for(int i = 0; i < 40; i++)
  {
    for(int y = 1; y < 29; y++)
      for(int x = 1; x < 39; x++)
        distance[y][x] = min(min(min(distance[y][x - 1], distance[y][x + 1]), min(distance[y - 1][x], distance[y + 1][x])) + 1, distance[y][x] + 0);  
    for(int y = 1; y < 29; y++)
    {
        distance[y][0] = min(min(distance[y][1], min(distance[y - 1][0], distance[y + 1][0])) + 1, distance[y][0] + 0);  
        distance[y][39] = min(min(distance[y][38], min(distance[y - 1][39], distance[y + 1][39])) + 1, distance[y][39] + 0);  
    }
    for(int x = 1; x < 39; x++)
    {
      distance[0][x] = min(min(min(distance[0][x - 1], distance[0][x + 1]), distance[1][x]) + 1, distance[0][x] + 0);
      distance[29][x] = min(min(min(distance[29][x - 1], distance[29][x + 1]), distance[29][x]) + 1, distance[29][x] + 0);
    }
    distance[0][0] = min(min(distance[1][0], distance[0][1]) + 1, distance[0][0] + 0);
    distance[39][0] = min(min(distance[38][0], distance[39][1]) + 1, distance[39][0] + 0);
    distance[0][29] = min(min(distance[1][29], distance[0][28]) + 1, distance[0][29] + 0);
    distance[39][29] = min(min(distance[38][29], distance[39][28]) + 1, distance[39][29] + 0);
  }
}

void calib()
{
  recalcAverage();
  for(int y = 0; y < 30; y++)
  {
    for(int x = 0; x < 40; x++)
    {
      int sample = pixelMap(x, y);
      gfx.setLED(sample, 128, 128, 128);
    }
  }
  calibration--;
}

void processDifference()
{
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
    {
      int sample = pixelMap(x, y);
      int d = min(128, difference[y][x] - 128) * 2 - 1;
      if(d > 0)
      {
        gfx.setLED(sample, d, d, d);
        //gfx.setLED(sample, newImage[y][x][0], newImage[y][x][1], newImage[y][x][2]);
      }
      else
      {
        gfx.setLED(sample, 0, 0, 0);
      }
    }  
}

void processDistance()
{
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
    {
      int sample = pixelMap(x, y);
      int r = min(distance[y][x] * 16, 255);
      gfx.setLED(sample, rainbow[r][0], rainbow[r][1], rainbow[r][2]);
    }  
}

void processFlames()
{
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
    if(difference[y][x] > 128)
      flames[y + 1][x] = (rand() & 127) + 128;
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
    {
      int f = flames[y + 1][x] = (flames[y][x + 1] + flames[y + 1][x + 1] + flames[y + 2][x + 1]) >> 2;
      int sample = pixelMap(x, y);
      gfx.setLED(sample, flamesC[f][0], flamesC[f][1], flamesC[f][2]);
    }  
}

void processShape()
{
  static unsigned long lastBeat = 0;
  unsigned long t = millis();
  unsigned long d = t - lastBeat;
  if(d >= 1236)
  {
    lastBeat = t;
  }
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
    {
      int sample = pixelMap(x, y);
      if(difference[y][x] > 128)
      {
        gfx.setLED(sample, 255, 255, 255);
      }  
      else
      {
        int r = d / 32 - distance[y][x];
        if(r < 0) r = -r;
        r = min(r * 64, 255);
        gfx.setLED(sample, 255 - r, 0, 0);
      }
    } 
}

void showImage()
{
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
    {
      int sample = pixelMap(x, y);
      gfx.setLEDGamma(sample, newImage[y][x][0], newImage[y][x][1], newImage[y][x][2]);
    }
}

void processFrames()
{
  const int col[][3] = {{0, 0, 127}, {127, 0, 0}, {0, 127, 0}, {64, 0, 64}};
  for(int y = 0; y < 30; y++)
    for(int x = 0; x < 40; x++)
    {
      int sample = pixelMap(x, y);
      int bit = difference[y][x] > 128 ? 1 : 0;
      if(bit)
      {
        gfx.setLED(sample, col[255][0], col[255][1], col[255][2]);
        frames[y][x] = (frames[y][x] << 1) | bit;
      }
      else
      {
        int b = 0;
        for(; b < 4; b++)
        if((frames[y][x] >> (b * 16 + 15)) & 1)
        {
          gfx.setLED(sample, col[b][0], col[b][1], col[b][2]);
          break;
        }
        if(b == 4)
          gfx.setLED(sample, 0, 0, 0);
        frames[y][x] = frames[y][x] << 1;        
      }
    }
}

void processImage()
{
  if(calibration)
  {
    calib();
    return;
  }
  switch(effect)
  {
    case 0:
      showImage();
      break;
    case 1:
      calcDifference();
      processDifference();
      break;
    case 2:
      calcDifference();
      calcDistance();
      processDistance();
      break;
    case 3:
      calcDifference();
      calcDistance();
      processShape();    
      break;
    case 4:
      calcDifference();
      processFlames();    
      break;
    case 5:
      calcDifference();
      processFrames();
      break;
  }
}
