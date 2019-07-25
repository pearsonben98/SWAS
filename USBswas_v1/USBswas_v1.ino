
/*
SWAS case software Stephen J Andrews 2017

Uses modbus over rs485 for comms between arduino controlled SWAS case and 
DAQfactory computer
*/
#include <SPI.h>
#include <SD.h>
#include <Modbus.h>
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
int start = 1;
// values to ask from the case
const int case_id = 100;
const int flow_thro = 101;
const int num_cans = 102;
const int volume = 103;

// values to pass to the case
const int time_stamp1 = 201;
const int time_stamp2 = 202;

const int vstat1 = 203;
const int vstat2 = 204;
const int vstat3 = 205;
const int vstat4 = 206;
const int vstat5 = 207;
const int vstat6 = 208;
const int vstat7 = 209;
const int vstat8 = 210;
const int vstat9 = 211;
const int vstat10 = 212;
const int vstat11 = 213;
const int vstat12 = 214;
const int vstat13 = 215;
const int vstat14 = 216;
const int vstat15 = 217;
const int vstat16 = 218;

const int fill_evac = 219;
const int psi = 220;
const int fill_dur = 221;
const int torr = 222;
const int vac_dur = 223;
const int lat = 224;
const int lon = 225;
const int alt = 226;
const int CO = 227;
const int CO2 = 228;
const int O3 = 229;
const int CH4 = 230;
const int trig_type = 231;
const int leak_rate = 232;


//**********************************

// Used Pins
const int ledPin = 13;
const int chipSelect = 53;

uint32_t prev = 1;
uint32_t hreg201;
uint32_t hreg202;
uint32_t concat;


// ModbusSerial object
ModbusSerial mb;
//Modbus modbus = Modbus();
// multiplex setup
//Mux mux;

void setup() {



  
  
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    
    Serial.println("testing serial");

    Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  
  



    
  
    pinMode(13, OUTPUT);
    //Config Multiplexer
//    mux.setup(8,9,10,11,A0); // initialise Mux
    
    // Config Modbus Serial (port, speed, byte format, readEnable pin) 
    mb.config(&Serial, 115200, SERIAL_8N1);  // &serial (uno) or &serial1 (leonardo)
    //mb.config(&Serial1,115200, SERIAL_8N1, 2); uncomment for rs485 comms
    
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
   
    mb.addHreg(vstat1);
    mb.addHreg(vstat2);
    mb.addHreg(vstat3);
    mb.addHreg(vstat4);
    mb.addHreg(vstat5);
    mb.addHreg(vstat6);
    mb.addHreg(vstat7);
    mb.addHreg(vstat8);
    mb.addHreg(vstat9);
    mb.addHreg(vstat10);
    mb.addHreg(vstat11);
    mb.addHreg(vstat12);
    mb.addHreg(vstat13);
    mb.addHreg(vstat14);
    mb.addHreg(vstat15);
    mb.addHreg(vstat16);

    
    mb.addHreg(time_stamp1);
    mb.addHreg(time_stamp2);
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
    
 


    //put case data into registers  
    mb.Hreg(case_id, v_case_id);
    mb.Hreg(flow_thro, v_flow_thro);
    mb.Hreg(num_cans, v_num_cans);
    mb.Hreg(volume, v_volume);

    //dataWriteln("testval1.txt","Testing register Values.");
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


   hreg201 = mb.Hreg(201);
   hreg202 = mb.Hreg(202);
   concat = hreg202 << 16 | hreg201;

  if(hreg201 != 0){
    if(prev < concat){
    prev = concat;
    
    dataWrite("data3.txt", "Timestamp: ");
    dataWriteln("data3.txt", String(prev));
  }
  }

  
  
  
 /* if(prev == 1 || prev < (((uint32_t)mb.Hreg(216) >> 16) | (mb.Hreg(200) + 65537))){
    prev = (((uint32_t)mb.Hreg(216) >> 16) | (mb.Hreg(200) + 65537));
    dataWrite("250719.txt", "Timestamp: ");
    dataWrite("250719.txt", String(prev));
    dataWriteln("250719.txt", "");
    
  }
    
  if(prev == 1000){
    
    dataWrite("testval5.txt", "Hreg(216): ");
    dataWriteln("testval5.txt", String(hreg216));
    dataWrite("testval5.txt", "Hreg(200): ");
    dataWriteln("testval5.txt", String(hreg200));
    dataWrite("testval5.txt", "concatenated: ");
    dataWriteln("testval15.txt", String(concat));
    prev= 1;
  }
  prev++;
   */
}

void dataWrite(String filename, String dataString){
  
    File dataFile = SD.open(filename, FILE_WRITE);

    if (dataFile) {
    dataFile.print(dataString);
    dataFile.close();
    }
    else {
      Serial.println("error opening datalog.txt");
    }
  
}

void dataWriteln(String filename, String dataString){

    File dataFile = SD.open(filename, FILE_WRITE);

    if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(dataString);
    }
    else {
      Serial.println("error opening datalog.txt");
    }
}

uint32_t getTimestamp(){

  //mb.readRegisters(200,1);
}

void readSDcard(String filename){

    File dataFile = SD.open(filename);

    if (dataFile) {
        Serial.println("datalog4.txt:");
        
        while (dataFile.available()) {
            Serial.write(dataFile.read());
        }
  
        dataFile.close();
  } else {
    Serial.println("error opening datalog2.txt");
  }
  
}
