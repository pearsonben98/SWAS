
/*
SWAS case software Stephen J Andrews 2017

Uses modbus over rs485 for comms between arduino controlled SWAS case and 
DAQfactory computer
*/

//#include <Modbus.h>
#include <ModbusSerial.h>  //for modbus chip MAX485
//#include <Mux.h>  //multiplex for CD74HC4067

const int modbus_ID = 5;  //this is unique for each case (1-247)

//case details, 'v' prefix indicates 'value of' num_cans for example
const word v_case_id = modbus_ID;     //should match the modbus ID
const word v_flow_thro = 0;
const word v_num_cans = 16;
const word v_volume = 1400;

//**********************************
// Modbus Registers Offsets (0-9999)
//variables 

// values to ask from the case
const int case_id = 100;
const int flow_thro = 101;
const int num_cans = 102;
const int volume = 103;

// values to pass to the case
const int time_stamp = 200;
const int fill_evac = 201;
const int psi = 202;
const int fill_dur = 203;
const int torr = 205;
const int vac_dur = 206;
const int lat = 207;
const int lon = 208;
const int alt = 209;
const int CO = 210;
const int CO2 = 211;
const int O3 = 212;
const int CH4 = 213;
const int trig_type = 214;
const int leak_rate = 215;
const int time_stamp_seconds = 216;
const int time_stamp_closed = 217;
const int time_stamp_closed_seconds = 218;

//**********************************

// Used Pins
const int ledPin = 13;

// ModbusSerial object
ModbusSerial mb;
// multiplex setup
//Mux mux;

void setup() {
    pinMode(13, OUTPUT);
    //Config Multiplexer
//    mux.setup(8,9,10,11,A0); // initialise Mux
    
    // Config Modbus Serial (port, speed, byte format, readEnable pin) 
    mb.config(&Serial, 115200, SERIAL_8N1, 2);  // &serial (uno) or &serial1 (leonardo)
    
    
    // Set the Slave ID (1-247)
    mb.setSlaveId(modbus_ID);  
    
    // Set ledPin modes
    for(int i=22; i<=37; i++)
    {
    pinMode(i, OUTPUT);
    }
    
    // Add registers - Use addCoil() for digital outputs 1/0
    for(int i=0; i<=15; i++)
    {
    mb.addCoil(i);
    }

    // addHreg() to store 'word' (unsigned 16 bit int) values on the slave 
    mb.addHreg(case_id);
    mb.addHreg(flow_thro);
    mb.addHreg(num_cans);
    mb.addHreg(volume);
   
    
    
    mb.addHreg(fill_evac);
    mb.addHreg(psi);
    mb.addHreg(fill_dur);
    mb.addHreg(torr);
    mb.addHreg(vac_dur);
    mb.addHreg(lat);
    mb.addHreg(lon);
    mb.addHreg(alt);
    mb.addHreg(CO);
    mb.addHreg(CO2);
    mb.addHreg(O3);
    mb.addHreg(CH4);
    mb.addHreg(trig_type);
    mb.addHreg(leak_rate);
    mb.addHreg(time_stamp);
    mb.addHreg(time_stamp_seconds);
    mb.addHreg(time_stamp_closed);  
    mb.addHreg(time_stamp_closed_seconds);  


    //put case data into registers  
    mb.Hreg(case_id, v_case_id);
    mb.Hreg(flow_thro, v_flow_thro);
    mb.Hreg(num_cans, v_num_cans);
    mb.Hreg(volume, v_volume);
}

void loop() {
   // Call once inside loop() 
   mb.task();
   digitalWrite(13, mb.Coil(0));

   // Attach solenoid pins to coil registers     
   digitalWrite(22, mb.Coil(0));
   digitalWrite(23, mb.Coil(1));
   digitalWrite(24, mb.Coil(2));
   digitalWrite(25, mb.Coil(3));
   digitalWrite(26, mb.Coil(4));
   digitalWrite(27, mb.Coil(5));
   digitalWrite(28, mb.Coil(6));
   digitalWrite(29, mb.Coil(7)); 
   digitalWrite(30, mb.Coil(8)); 
   digitalWrite(31, mb.Coil(9)); 
   digitalWrite(32, mb.Coil(10)); 
   digitalWrite(33, mb.Coil(11)); 
   digitalWrite(34, mb.Coil(12)); 
   digitalWrite(35, mb.Coil(13)); 
   digitalWrite(36, mb.Coil(14)); 
   digitalWrite(37, mb.Coil(15)); 
   
  int j = 0;
  
  /*for(int i=22, i < 38, i++){
    digitalWrite(i, mb.Coil(j);
    j++;
  }*/
  

   
  
}
