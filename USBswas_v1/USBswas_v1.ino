
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


unsigned long position = 0;
String myFile = "pls9wo.txt";
int interval = 0;


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
const int readLine = 106;
const int writeLine = 107;
const int readFrom = 108;
const int readLine1 = 109;
const int confirmReadFrom = 110;

int prevLineValue1 = 0;
int readLineValue1;
int prevLineValue = 0;
int readLineValue;
int prev105 = 0;

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

const int bottlenum = 219;
const int valve_state = 220;
const int psi = 221;
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

uint32_t concat;

// ModbusSerial object
ModbusSerial mb;


void setup() {
  
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    
   // Serial.println("testing serial");

   // Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }



  
  
  
  

    
    
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
    mb.addHreg(readLine);
    mb.addHreg(writeLine);
    mb.addHreg(readFrom);
    mb.addHreg(readLine1);
    mb.addHreg(confirmReadFrom);

   
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
    mb.addHreg(bottlenum);
    mb.addHreg(valve_state);
    mb.addHreg(psi);
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


   getLineCount(myFile);
  
   formatData(getLine(myFile, v_changes));

    
        
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

   
   int hreg105 = mb.Hreg(105);
   readLineValue = mb.Hreg(106);
   readLineValue1 = mb.Hreg(109);


  int hreg108 = mb.Hreg(108);
  


  if(mb.Hreg(105) == 0){
      getLineCount(myFile);
      mb.Hreg(changes, v_changes);
   }

  
  
  if((mb.Hreg(108) != 0) && (mb.Hreg(110) == 1)){
    startFrom();
    
  }

  /*
  
  if(readLineValue1 != prevLineValue1){
    formatData(getLine1(myFile));
    prevLineValue1 = readLineValue1;
    interval++;
  }
  
 */
  
  if(readLineValue != prevLineValue && v_changes != 0){
      formatData(getLine1(myFile, interval));
      prevLineValue = readLineValue;
      interval++;
   }

   
   
  //concat = hreg202 << 16 | hreg201;
  
  // puts register values into the SD card
  if(mb.Hreg(107) == 1){
    writeToSD();
    
    mb.Hreg(107, 0);
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
  v_changes = 0;
  while (dataFile.available()) {
    buffer = dataFile.readStringUntil('\n');
    v_changes++;         
  }
 
  dataFile.close();
}


// FUNCTION WORKS PROPERLY FINALLY
String getLine(String filename, int lineNumber){

  File dataFile = SD.open(filename);
  int linenum = 0;
  while(dataFile.available()){
    
    String data = dataFile.readStringUntil('\r');
    linenum++;
    
    if(linenum == lineNumber){
      dataFile.close();
      return data;
    }
    
  }
  dataFile.close();
  
}


// FUNCTION WORKS TOO
void formatData(String data){

    // converting the string to a char array
    char charData[500];
    data.toCharArray(charData,500);
    
    int value;
    int i = 201;

    // reads the char array until there is an instance of tab (\t)
    char *token = strtok(charData, "\t");
    // converts the char array into integer
    //sscanf(token, "%d", &value);
    value = atoi(token);

    // places the integer into the register address 201 (timestamp1)
    mb.Hreg(i, value);

    i++;
    // iterates through the rest of the char array
    while(token != NULL){
      
      token = strtok(NULL, "\t");
      value = atoi(token);
      mb.Hreg(i, value);

      i++;

     
    }
   
}

void writeToSD(){

    String datastring = "";

    //datastring = String(concat);
    //datastring += "\t";
    datastring += String(mb.Hreg(201));
    datastring += "\t";
    datastring += String(mb.Hreg(202));
    datastring += "\t";
    datastring += String(mb.Hreg(203));
    datastring += "\t";
    datastring += String(mb.Hreg(204));
    datastring += "\t";
    datastring += String(mb.Hreg(205));
    datastring += "\t";
    datastring += String(mb.Hreg(206));
    datastring += "\t";
    datastring += String(mb.Hreg(207));
    datastring += "\t";
    datastring += String(mb.Hreg(208));
    datastring += "\t";
    datastring += String(mb.Hreg(209));
    datastring += "\t";
    datastring += String(mb.Hreg(210));
    datastring += "\t";
    datastring += String(mb.Hreg(211));
    datastring += "\t";
    datastring += String(mb.Hreg(212));
    datastring += "\t";
    datastring += String(mb.Hreg(213));
    datastring += "\t";
    datastring += String(mb.Hreg(214));
    datastring += "\t";
    datastring += String(mb.Hreg(215));
    datastring += "\t";
    datastring += String(mb.Hreg(216));
    datastring += "\t";
    datastring += String(mb.Hreg(217));
    datastring += "\t";
    datastring += String(mb.Hreg(218));
    datastring += "\t";
    datastring += String(mb.Hreg(219));
    datastring += "\t";
    datastring += String(mb.Hreg(220));
    datastring += "\t";
    datastring += String(mb.Hreg(221));
    datastring += "\t";
    datastring += String(mb.Hreg(222));
    datastring += "\t";
    datastring += String(mb.Hreg(223));
    datastring += "\t";
    datastring += String(mb.Hreg(224));
    datastring += "\t";
    datastring += String(mb.Hreg(225));
    datastring += "\t";
    datastring += String(mb.Hreg(226));
    datastring += "\t";
    datastring += String(mb.Hreg(227));
    datastring += "\t";
    datastring += String(mb.Hreg(228));
    datastring += "\t";
    datastring += String(mb.Hreg(229));
    datastring += "\t";
    datastring += String(mb.Hreg(230));
    datastring += "\t";
    datastring += String(mb.Hreg(231));
    datastring += "\t";
    datastring += String(mb.Hreg(232));
      
    dataWriteln(myFile, datastring);

}

void startFrom(){

  File dataFile = SD.open(myFile);
  int linenum = 0;
  while (dataFile.available()){
    
      dataFile.readStringUntil('\r');
      position = dataFile.position();
      linenum++;

      // if changes - readRecords is equal to linenumber
      if((mb.Hreg(105) - mb.Hreg(108)) == linenum){
        dataFile.close();
        //mb.Hreg(108, 0);
        return;
        
      }
    
  }
  dataFile.close();
  
}

String getLine1(String filename, int interval2){

  File dataFile = SD.open(filename);
  dataFile.seek(position);
  // read the line
  String data = dataFile.readStringUntil('\r');
  
  for(int i = 0; i < interval2; i++){
    data = dataFile.readStringUntil('\r');
  }
  
  return data;

  
}







/*I think you need to explain in detail what you want to achieve. From what I can figure out, you want to jump to a specific line based on a random number. If that is the case, the trick could be to use a form of indexing.

Read your file (e.g. in setup()) once character by character.
If you encounter a '\n', you store that position (actually the next one) in an array. This array should be 600 elements. In your wordList() function, you open the file, use fseek() and read the line.

The positions are unsigned integers (if your file is less than 64kB) and will cost you 1200 bytes. This is quite a lot and including the SD library causes already requires more RAM then available.


*/
