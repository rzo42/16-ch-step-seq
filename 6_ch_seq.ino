const int song_length=16;

/*SEQUENCER                        CH:1  CH:2  CH:3  CH:4  CH:5  CH:6
                                   cv,g  cv,g  cv,g  cv,g  cv,g  cv,g  */
int sequence[song_length][6][2]={{{24,1000},{0,0},{42,750},{12,600},{60,500},{60,250}},
                                 {{0,0},{0,0},{42,750},{0,0},{80,500},{00}},
                                 {{32,1000},{32,500},{42,750},{24,600},{100,600},{0,0}},
                                 {{0,0},{24,1000},{42,750},{24,600},{0,0},{0,0}},
                                 {{24,1000},{32,500},{45,750},{36,600},{80,500},{90,250}},
                                 {{0,0},{28,500},{45,750},{0,0},{0,0},{0,0}},
                                 {{32,1000},{0,0},{45,750},{42,600},{120,600},{0,0}},
                                 {{0,0},{0,0},{45,750},{0,0},{0,0},{0,0}},
                                 {{32,1000},{32,500},{51,750},{54,600},{80,600},{120,600}},
                                 {{0,0},{0,0},{48,500},{51,750},{0,0},{120,250}},
                                 {{44,1000},{32,500},{51,750},{78,600},{60,600},{0,0}},
                                 {{0,0},{38,500},{48,750},{0,0},{0,0},{0,0}},
                                 {{32,1000},{0,0},{48,750},{102,600},{40,600},{30,250}},
                                 {{0,0},{42,500},{48,750},{0,0},{0,0},{0,0}},
                                 {{28,1000},{38,1000},{44,750},{90,600},{60,600},{0,0}},
                                 {{0,0},{32,500},{44,750},{0,0},{0,0},{0,0}}};

 //array pointer for gate
 const int _gate=1;

 //array pointer for cv
 const int _cv=0;
 
/*PIN ASSIGNMENT
 0:unused
 1:unused
 2:ch.1 gate out <dig_num[5]> (digital)
 3:ch.1 cv out <anl_num[5]> (analog)
 4:ch.2 gate out <dig_num[4]> (digital)
 5:ch.2 cv out <anl_num[4]> (analog)
 6:ch.3 cv out <anl_num[3]> (analog)
 7:ch.3 gate out <dig_num[3]> (digital)
 8:ch.4 gate out <dig_num[2]> (digital
 9:ch.4 cv out <anl_num[2]> (analog)
 10:ch.5 cv out <anl_num[1]> (analog)
 11:ch.6 cv out <anl_num[0]> (analog)
 12:ch.5 gate out <dig_num[1]> (digital)
 13:ch.6 gate out <dig_num[0]> (digital)
 A0:pitch control in <pitch_pot>(analog)
 A1:gate control in <gate_pot>(analog)
 A2:clock contol in <clock_pot>(analog)
 A3:!!!(reserved for octave control)!!!
 A4:!!!(reserved for note cv in)!!!
 A5:clock in <clock_pot>(digital)
*/
 
const int clock_in=A5;
const int clock_pot=A2;
const int gate_pot=A1;
const int pitch_pot=A0;

int dig_num[6]={2,4,7,8,12,13};
int anl_num[6]={3,5,6,9,10,11};

//timer
unsigned long timer=0;

//sequence counter
int sq_counter=0;

//gate latch
int latch=0;

//gate switch
int ox_switch=0;

//note value
int _note=0;

//note on function 
void cv_gate(){
  timer=millis();   //sets timer for current step
  latch=sq_counter; //saves sequence counter to the gate latch
//loops for 6 channels
  for(int pin_on=0;pin_on<6;pin_on++){
  if(sequence[sq_counter][pin_on][_cv]>0){ 
  _note=sequence[sq_counter][pin_on][_cv]-((analogRead(pitch_pot)/42)-12);}    //note=current note+pitch cv       
  else{_note=0;}

  
  
  if(_note<0){_note=0;}                                                     //keep note between 0...    
  if(_note>127){_note=127;}                                                 //...127
  analogWrite(anl_num[pin_on],_note);                                       //send note to cv line
  digitalWrite(dig_num[pin_on],sequence[sq_counter][pin_on][_gate]);}}        //set gate (0=LOW(off),1=HIGH(on))

//step count function
void step_seq(){
  if(sq_counter<song_length-1){sq_counter++;}
  else{sq_counter=0;}}
  
//==============================================================================

void setup() {
  pinMode(clock_in,INPUT);
  pinMode(clock_pot,INPUT);
  pinMode(gate_pot,INPUT);
  pinMode(pitch_pot,INPUT);
  for(int pin_init=0;pin_init<6;pin_init++){
  pinMode(dig_num[pin_init],OUTPUT);
  pinMode(anl_num[pin_init],OUTPUT);}
}

//===============================================================================

void loop() {
//loops for 6 pins (used for gate length timer)
  for(int pin_count=0;pin_count<6;pin_count++){

//--------------------------------------------------------------------------------
 
//CLOCK TRIGGER HIGH 
//triggers routine if clock_in goes form LOW to HIGH
if(digitalRead(clock_in)==HIGH&&ox_switch==0){ox_switch=1;cv_gate();}

//TIMER TRIGGER 
// when clock_pot is on
if (analogRead(clock_pot)<1020&&millis()>timer+analogRead(clock_pot)){
  cv_gate();    //notes on
  step_seq();}  //increment step
  
//GATE OFF
//turns gate off for each channel when timer counts to <gate length + gate offset(gate_pot)
if (digitalRead(dig_num[pin_count])==HIGH&&millis()>timer+sequence[latch][pin_count][_gate]-(analogRead(gate_pot)-512)){
    digitalWrite(dig_num[pin_count],LOW);}

//CLOCK TRIGGER LOW 
    if(digitalRead(clock_in)==LOW&&ox_switch==1){
      ox_switch=0; //saves clock status as LOW
      step_seq();} //increment step
}}
