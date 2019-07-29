
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
word v_changes = 0;
word v_flight_id;

//**********************************

// Modbus Registers Offsets (0-9999)
//variables 
int start = 1;
// values to ask from the case
const int case_id = 100;
const int flow_thro = 101;
const int num_cans = 102;
const int volume = 103;
const int flight_id = 104;
const int changes = 105;

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
uint32_t hreg203;
uint32_t hreg204;
uint32_t hreg205;
uint32_t hreg206;
uint32_t hreg207;
uint32_t hreg208;
uint32_t hreg209;
uint32_t hreg210;
uint32_t hreg211;
uint32_t hreg212;
uint32_t hreg213;
uint32_t hreg214;
uint32_t hreg215;
uint32_t hreg216;
uint32_t hreg217;
uint32_t hreg218;
uint32_t hreg219;
uint32_t hreg220;
uint32_t hreg221;
uint32_t hreg222;
uint32_t hreg223;
uint32_t hreg224;
uint32_t hreg225;
uint32_t hreg226;
uint32_t hreg227;
uint32_t hreg228;
uint32_t hreg229;
uint32_t hreg230;
uint32_t hreg231;
uint32_t hreg232;

uint32_t concat;





// ModbusSerial object
ModbusSerial mb;


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

   getLineCount("plswork1.txt");
    
    
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
    mb.addHreg(flight_id);
    mb.addHreg(changes);
   
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
    mb.Hreg(changes, v_changes);

    
    //dataWriteln("LOGTEST6.txt", "timestamp1 | timestamp2 | vstat 1 to 16 | fill_evac | psi | fill_dur | torr | vac_dur | lat | lon | alt | CO | CO2 | O3 | CH4 | trig_type | leak_rate");

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
  
   hreg203 = mb.Hreg(203);
   hreg204 = mb.Hreg(204);
   hreg205 = mb.Hreg(205);
   hreg206 = mb.Hreg(206);
   hreg207 = mb.Hreg(207);
   hreg208 = mb.Hreg(208);
   hreg209 = mb.Hreg(209);
   hreg210 = mb.Hreg(210);
   hreg211 = mb.Hreg(211);
   hreg212 = mb.Hreg(212);
   hreg213 = mb.Hreg(213);
   hreg214 = mb.Hreg(214);
   hreg215 = mb.Hreg(215);
   hreg216 = mb.Hreg(216);
   hreg217 = mb.Hreg(217);
   hreg218 = mb.Hreg(218);
   
   hreg219 = mb.Hreg(219);
   hreg220 = mb.Hreg(220);
   hreg221 = mb.Hreg(221);
   hreg222 = mb.Hreg(222);
   hreg223 = mb.Hreg(223);
   hreg224 = mb.Hreg(224);
   hreg225 = mb.Hreg(225);
   hreg226 = mb.Hreg(226);
   hreg227 = mb.Hreg(227);
   hreg228 = mb.Hreg(228);
   hreg229 = mb.Hreg(229);
   hreg230 = mb.Hreg(230);
   hreg231 = mb.Hreg(231);
   hreg232 = mb.Hreg(232);
   
  concat = hreg202 << 16 | hreg201;
  
  String datastring = "";
  
  if(hreg201 != 0){
    
    if(prev < concat){
      
      prev = concat;
      datastring += String(hreg201);
      datastring += "\t";
      datastring += String(hreg202);
      datastring += "\t";
      datastring += String(hreg203);
      datastring += "\t";
      datastring += String(hreg204);
      datastring += "\t";
      datastring += String(hreg205);
      datastring += "\t";
      datastring += String(hreg206);
      datastring += "\t";
      datastring += String(hreg207);
      datastring += "\t";
      datastring += String(hreg208);
      datastring += "\t";
      datastring += String(hreg209);
      datastring += "\t";
      datastring += String(hreg210);
      datastring += "\t";
      datastring += String(hreg211);
      datastring += "\t";
      datastring += String(hreg212);
      datastring += "\t";
      datastring += String(hreg213);
      datastring += "\t";
      datastring += String(hreg214);
      datastring += "\t";
      datastring += String(hreg215);
      datastring += "\t";
      datastring += String(hreg216);
      datastring += "\t";
      datastring += String(hreg217);
      datastring += "\t";
      datastring += String(hreg218);
      datastring += "\t";
      datastring += String(hreg219);
      datastring += "\t";
      datastring += String(hreg220);
      datastring += "\t";
      datastring += String(hreg221);
      datastring += "\t";
      datastring += String(hreg222);
      datastring += "\t";
      datastring += String(hreg223);
      datastring += "\t";
      datastring += String(hreg224);
      datastring += "\t";
      datastring += String(hreg225);
      datastring += "\t";
      datastring += String(hreg226);
      datastring += "\t";
      datastring += String(hreg227);
      datastring += "\t";
      datastring += String(hreg228);
      datastring += "\t";
      datastring += String(hreg229);
      datastring += "\t";
      datastring += String(hreg230);
      datastring += "\t";
      datastring += String(hreg231);
      datastring += "\t";
      datastring += String(hreg232);
      
      dataWriteln("plswork1.txt", datastring);
      //dataWriteln("LOGTEST6.txt", String(concat));

    }
  }

 
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

int getLineCount(String filename){
  File dataFile = SD.open(filename);
  String buffer;
  while (dataFile.available()) {
    buffer = dataFile.readStringUntil('\n');
    v_changes++;         
  }
      
  dataFile.close();
}
