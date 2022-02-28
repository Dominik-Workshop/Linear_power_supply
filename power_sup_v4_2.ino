/**
 * @author Dominik Workshop
 * @date 15.02.2022
 * @version 4.2 (minor changes, seperate function for moving average)
 */
  
  #define FAN 3                           //pin 3 controlls the fans
  #define OUTPUT_PS 4                     //pin 4 can switch off the output of the power supply
  #define RELAY 5                         //pin 5 controlls the relay that switches the transformer tap
  #define THERMISTOR_PIN  A0              //voltage divider with a thermistor measuring the heatsink's temperature, connected to pin A0
  #define OVER_TEMPERATURE_PROTECTION 55  //power supply turns off the output above this temperature

  float VoltageThreshold = 13.5; //voltage threshold to switch the transformer tap
  int TempThreshold = 30;       //tempearature threshold to turn on the cooling fans 
  int Fanspeed;

  int Vo;
  float R1 = 10000;
  float logR2, R2;
  int T;                   // T = temperature
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

  const int numReadings = 6; //number of readings for moving average filter

  float I_readings[numReadings];      // the readings from the analog input
  int I_readIndex = 0;              // the index of the current reading
  float I_total = 0;                  // the running total
  float I_average = 0;                // the average

  float U_readings[numReadings];      // the readings from the analog input
  int U_readIndex = 0;              // the index of the current reading
  float U_total = 0;                  // the running total
  float U_average = 0;                // the average

  float I_set_readings[numReadings];      // the readings from the analog input
  int I_set_readIndex = 0;              // the index of the current reading
  float I_set_total = 0;                  // the running total
  float I_set_average = 0;                // the average
  
  #include <Adafruit_ADS1X15.h>
  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>
  #include <stdio.h>

  Adafruit_ADS1115 ads;   //Use this for the 16-bit version 
  //Adafruit_ADS1015 ads;   //Use this for the 12-bit version 

  LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

void setup(void){

  pinMode(FAN, OUTPUT); 
  pinMode(OUTPUT_PS, OUTPUT); 
  pinMode(RELAY, OUTPUT); 
  
  TCCR2B = TCCR2B & B11111000 | B00000001; //increase the pwm speed
  
  lcd.begin();
  ads.begin();
   
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  //show a greeting screen
  lcd.setCursor(0, 0);         
  lcd.print("Dominik Workshop");
  lcd.setCursor(6, 1);  
  lcd.print(2021);
  delay(1500);
  lcd.clear();

  //set all elements of arrays for the moving average filter to zero
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    I_readings[thisReading] = 0;
    U_readings[thisReading] = 0;
    I_set_readings[thisReading] = 0;
  }
  
}

void MovingAverageFilter(float amps, float volts, float setcurrent){
    // subtract the last reading:
  I_total = I_total - I_readings[I_readIndex];
  // read from the sensor:
  I_readings[I_readIndex] = amps;
  // add the reading to the total:
  I_total = I_total + I_readings[I_readIndex];
  // advance to the next position in the array:
  I_readIndex = I_readIndex + 1;
  // if we're at the end of the array...
  if (I_readIndex >= numReadings) {
    // ...wrap around to the beginning:
    I_readIndex = 0;
  }
  // calculate the average:
  I_average = I_total / numReadings;


    // subtract the last reading:
  U_total = U_total - U_readings[U_readIndex];
  // read from the sensor:
  U_readings[U_readIndex] = volts;
  // add the reading to the total:
  U_total = U_total + U_readings[U_readIndex];
  // advance to the next position in the array:
  U_readIndex = U_readIndex + 1;
  // if we're at the end of the array...
  if (U_readIndex >= numReadings) {
    // ...wrap around to the beginning:
    U_readIndex = 0;
  }
  // calculate the average:
  U_average = U_total / numReadings;


  // subtract the last reading:
  I_set_total = I_set_total - I_set_readings[I_set_readIndex];
  // read from the sensor:
  I_set_readings[I_set_readIndex] = setcurrent;
  // add the reading to the total:
  I_set_total = I_set_total + I_set_readings[I_set_readIndex];
  // advance to the next position in the array:
  I_set_readIndex = I_set_readIndex + 1;
  // if we're at the end of the array...
  if (I_set_readIndex >= numReadings) {
    // ...wrap around to the beginning:
    I_set_readIndex = 0;
  }
  // calculate the average:
  I_set_average = I_set_total / numReadings;
}

void loop(void){
  int16_t adc0, adc1, adc2;
  float amps, volts, setcurrent;
  
  adc0 = ads.readADC_SingleEnded(1);      //measure voltage drop on a shunt resistor referenced to ground
  adc1 = ads.readADC_Differential_0_1();  //measure voltage from a voltage divider from output of the power supply
  adc2 = ads.readADC_Differential_2_3();  //measure voltage on 'current set' poteniometer

  amps = ((adc0)/2876.0);         //you can calibrate measured current here
  volts = ((adc1)/1104.6);        //you can calibrate measured voltage here
  setcurrent = ((adc2)/2613.0);   //you can calibrate set current here

  MovingAverageFilter(amps, volts, setcurrent);          //apply a moving average filter to current, voltage and setcurrent

  //ignore negative values
  if(I_average<0){
  I_average = 0;    
  }
  if(U_average<0){
  U_average = 0;    
  }
  if(I_set_average<0){
  I_set_average = 0;    
  }
  if(volts<0){
  volts = 0;    
  }

  //calculate temperature
  Vo = analogRead(THERMISTOR_PIN);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  //T = (T * 9.0)/ 5.0 + 32.0; //converting the tempereture reading to Fahrenheit

  Fanspeed = map(T , TempThreshold, 50, 100, 255); //calculate current fan speed
  
  //hysteresis, so that fans don't turn on/off repeatedly when crossing the threshold
  if (T < TempThreshold){
    Fanspeed = 0;
    TempThreshold = 31;
  }
  if (T >= TempThreshold){
    TempThreshold=30;
  }
  //if it's hot drive fans at full speed
  if (T > 50){
    Fanspeed = 255;
  }
  //disable the output when it overheats
  if(T > OVER_TEMPERATURE_PROTECTION){
    digitalWrite(OUTPUT_PS, LOW);
  }
  else{
    digitalWrite(OUTPUT_PS, HIGH);
  }

  lcd.setCursor(0, 0);
  lcd.print(U_average);  //display measured output voltage
  lcd.print("V ");

  lcd.setCursor(7, 0);
  if (I_set_average >= 10){
    lcd.print(I_set_average, 1);    //display set output current with 1 decimal place
  } else lcd.print(I_set_average);  //display set output current with 2 decimal places
  lcd.print("A ");

  lcd.setCursor(13, 0);  
  lcd.print(T);  //display measured teperature
  //lcd.write(0);
  lcd.print("C");
  
  lcd.setCursor(0, 1); 
  if (I_average >= 10){
     lcd.print(I_average);        //display measured output current with 2 decimal places
  }else lcd.print(I_average,3);   //display measured output current with 3 decimal places
  lcd.print("A");
  
  lcd.setCursor(7, 1);
  lcd.print(I_average * U_average, 1);  //display output power with 1 decimal place
  lcd.print("W ");

  //hysteresis, so that the relay doesn't turn on/off repeatedly when crossing the threshold
  if(volts > VoltageThreshold){
    digitalWrite(RELAY, HIGH);
    VoltageThreshold = 13;
  }
  if(volts<VoltageThreshold){
    digitalWrite(RELAY, LOW);
    VoltageThreshold = 13.5;
  }
  
  analogWrite(FAN, Fanspeed);
  
  delayMicroseconds(20);
}
