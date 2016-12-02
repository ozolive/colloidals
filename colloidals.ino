/*
  Coloidal silver generator
  Schematic:

  +19V ---O  =--
                |
                |
                C
  PWM5-R1-R2--B   2N5088
         |      E
       C1=100n  |- A0 (analog regulating input)
         |      R3
         |      |
  GND -----------

  Ibe(max) = 30µA
  Ice(max) = 10mA
  R3 = 2V/0.01A = 200 ohm

  R1 + R2 = (5V-(2V+0.6V))/0.00003A = 80kohm
  R1 + R2 = (5V-(2V+0.6V))/0.0001A = 24kohm   // gain 100
  R1 + R2 = (3.3V-(2V+0.6V))/0.0001A = 7kohm //
  R1=R2= 40kohm

  C ~= 100n
   
 */

#define PWM_PIN 5
#define BOOST_PIN 3
#define ADC_PIN 0
#define VOLT_PIN 1
#define FAN_PIN 6

#define H1 9
#define H2 10


#define PWM_MAX 252

#define TARGETA 0.00123 //A
#define R3 200 // ohm
#define VOLT_SUPPLY 4.7 // arduino voltage
#define VOLT 1.1 // arduino voltage
#define VOLUME_ML 170 // distilled water volume (mL)
#define SMOOTHNESS 13
#define SMOOTHNESS_SAMPLE 255
#define SMOOTHNESS_VOLT   2
#define BOOST_CAL (4.6/3.37)
#define MA_CAL 6

#define R4  320000 // ohm
#define R5 5600000 // ohm


// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

uint8_t pwm_val = 1;
uint8_t count   = 0;
uint32_t last_sec, last_sec_spin, secs = 0;
uint32_t loops;
uint32_t ohms = 0;

float ppm_target;
float volts_target;
uint8_t boost_val;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;

  volts_target  = 19.1;
  boost_val = 1;
  ppm_target = 12;

  analogReference(INTERNAL);
  analogRead(ADC_PIN);
  Serial.begin(9600);
  delay(10);
  pinMode(PWM_PIN, OUTPUT);
  analogWrite(PWM_PIN, pwm_val);
  pinMode(FAN_PIN, OUTPUT);
  analogWrite(FAN_PIN, 255);
  pinMode(BOOST_PIN, OUTPUT);
  analogWrite(BOOST_PIN,0);
//  digitalWrite(BOOST_PIN, LOW);
  last_sec = micros();
  last_sec_spin = micros();
  //test_boost();
//  reverse_spin();
}


uint16_t sample_max = 0;
uint16_t sample_min = 255;
float sample_avg = 0;
float sample_var_avg = 0;
float pwm_avg = 1;
float pwm_var_avg = 0;
float volts_avg=0;
int8_t spin=0;
float ppm;

float sample_volt() {
    analogRead(VOLT_PIN);
    //delay(10);
    float tmp = (analogRead(VOLT_PIN) * ((R5+R4)/R4)) * BOOST_CAL / (1024*VOLT);
    analogRead(ADC_PIN);
    if(abs(volts_avg) < 0.001){
        volts_avg = tmp;
    }
    volts_avg = ((volts_avg * SMOOTHNESS_VOLT) + tmp) / (SMOOTHNESS_VOLT+1);

    //delay(1);
    return tmp;
}

void reverse_spin() {
    if(spin) {
        digitalWrite(H1,0);
        digitalWrite(H2,1);
    } else {
        digitalWrite(H1,1);
        digitalWrite(H2,0);
    }
    spin = ! spin;
}

void test_boost(){
    float before = sample_volt();
    /*
    Serial.print(before);
    Serial.print("\n");
    */
    analogWrite(BOOST_PIN,boost_val);
//    delay(1);
//    analogWrite(BOOST_PIN,0);
    float after = sample_volt();
    /*
    Serial.print(after);
    Serial.print("\n");
    */


}


void tight_loop(){
    float volt = sample_volt();
    /*
    Serial.print(volt);
    Serial.print(",");
    Serial.print(volts_target);
    Serial.print("\n");
*/
    if (!((volt - volts_target) > 0.001)) {
        test_boost();
    }
    else {
        analogWrite(BOOST_PIN,0);
    }
}

// the loop function runs over and over again forever
void loop() {
  uint32_t right_now = micros();
  uint16_t sample = analogRead(ADC_PIN);
  if(ppm >= ppm_target) {
      analogWrite(PWM_PIN, 0);
      return;
  }

  if(sample <= MA_CAL) {
      sample = 0;
  }
  else {
      sample -= MA_CAL; 
  }

  if(abs(sample_avg) < 0.001){
      sample_avg = sample;
      }
  sample_avg = ((sample_avg * SMOOTHNESS_SAMPLE) + sample)/(SMOOTHNESS_SAMPLE+1);
  sample_var_avg = ((sample_var_avg * SMOOTHNESS_SAMPLE) + (abs(sample_avg - sample)))/(SMOOTHNESS_SAMPLE+1);



  float diff = (R3*TARGETA*1024/VOLT) - sample;
  float next_val = (diff/SMOOTHNESS) + pwm_val;
  if  (PWM_MAX < next_val)
    next_val = PWM_MAX;
  if  (0 > next_val)
    next_val = 0;

  pwm_val = (uint8_t) next_val;
  pwm_avg = ((pwm_avg * 255) + pwm_val)/256;    
  analogWrite(PWM_PIN, pwm_val);
  pwm_var_avg = ((pwm_var_avg * 255) + abs(pwm_val-pwm_avg)  )/256;    


  if (sample_max < sample ) {
      sample_max = sample;
  }

  if (sample_min > sample ) {
      sample_min = sample;
  }


  float mA = ( ((float)sample_avg * VOLT * 1000 ) / 1024.0) / R3 ;

  if(((right_now-last_sec_spin) > 1000000) || ((right_now-last_sec_spin) < 0)){
      last_sec_spin = micros();
      ppm += (107.87*(mA/1000)*(1 /* second */ )/96485.0*1000.0)/VOLUME_ML*1000;
  }


  if((loops % 512)==12) {
     sample_volt();
  }

  if(((right_now-last_sec) > 1000000) || ((right_now-last_sec) < 0)){
     sample_volt();
     float volt_reg = 0.2;
     if ((pwm_val < PWM_MAX) && (volts_target > VOLT_SUPPLY)) {
         volts_target -= 0.1;
     }
     ohms = (volts_avg - (((sample_avg * VOLT)/ 1024.0)) + volt_reg ) / (mA/1000);

      Serial.print(sample_min);
      Serial.print(',');
      Serial.print(sample_avg);
      Serial.print(',');
      Serial.print(sample_max);
      Serial.print(',');
      Serial.print(sample_var_avg);
      Serial.print(',');
      Serial.print(sample);
      Serial.print(',');
      Serial.print(pwm_avg);
      Serial.print(',');
      Serial.print(pwm_var_avg);
      Serial.print(',');
      Serial.print(volts_target);
      Serial.print(',');
      Serial.print(volts_avg);
      Serial.print(',');
      Serial.print(mA);
      Serial.print(',');
      Serial.print(ohms);
      Serial.print(',');
      Serial.print(ppm);
      Serial.print('\n');
      last_sec = micros();
      sample_max = 0;
      sample_min = 255;
      if ((secs%60)==0) {
          analogWrite(FAN_PIN,255);
          }
      if ((secs%60)==27) {
//          analogWrite(FAN_PIN,0);
        }
      //test_boost();
//      boost_val += 8;
      secs++;
  }
  else {
    uint8_t loop_out = 0;
    while(!loop_out) {
        uint32_t wait_time = 1000 -  (micros()-right_now);
        if(wait_time<1000) {
            tight_loop();
            delayMicroseconds(micros()-right_now);
        }
        else {
            loop_out = !0;
            right_now += 1000; // not yet
        }
    }
  }
  loops++;
}
