
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
String myFile = "data.txt";
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



// Used Pins
const int ledPin = 13;
const int chipSelect = 53;

uint32_t prev = 1;
uint32_t concat;
ModbusSerial mb;


void setup() {
  
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }

    pinMode(13, OUTPUT);
 
    
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

    /* adding all of the holding registers  
     */
    mb.addHreg(case_id);
    mb.addHreg(flow_thro);
    mb.addHreg(num_cans);
    mb.addHreg(volume);
    mb.addHreg(flight_id);
    mb.addHreg(changes);
    mb.addHreg(readLine);
    mb.addHreg(writeLine);
    mb.addHreg(readFrom);
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
    

    /* These registers are constant and their values only differ between each case.
     *  The unique values of the case are then stored in the registers, for DAQfactory to be able to
     *  tell which case its communicating with.
     */
    mb.Hreg(case_id, v_case_id);
    mb.Hreg(flow_thro, v_flow_thro);
    mb.Hreg(num_cans, v_num_cans);
    mb.Hreg(volume, v_volume);
    mb.Hreg(changes, v_changes);

    
    /* if myFile does not exist, create the file, and write the below to the file
     *  
     *  Putting 3's in all the valve states, to indicate that the state of the valve is currently
     *  unknown. */
    if(!SD.exists(myFile)){
      dataWriteln(myFile, "0\t0\t3\t3\t3\t3\t3\t3\t3\t3\t3\t3\t3\t3\t3\t3\t3\t3\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0");
    }



   // counting the number of lines in the text file, and then placing the number into a variable
   getLineCount(myFile);


   /* reads the most recent line appended to the text file, and then populates the registers with
    *  the contents of the line. This is used so on startup, DAQfactory can immediately tell the most
    *  recent state of the case, basically populates the current_status array in daqfactory
    */
    if(v_changes != 0){
      formatData(getLine(myFile, v_changes));
    }

    
        
}


//------------------------------------------------------LOOP------------------------------------------
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
   int hreg108 = mb.Hreg(108);
   readLineValue = mb.Hreg(106);
  

  // if the file contains data, put the number of lines into register 105
  if(mb.Hreg(105) == 0){
      getLineCount(myFile);
      mb.Hreg(changes, v_changes);
   }

  /* when loading the case history table on daqfactory, the user inputs the number of records they want 
   *  to read. This if statement sets the arduino to start reading the text file from the point
   *  (number of lines in text file minus how many records to read)
  */
  if((mb.Hreg(108) != 0) && (mb.Hreg(110) == 1)){
    startFrom();
  }


  /* In DAQFactory, when a value is passed into register 106, the arduino will read the line in the
   *  text file corresponding to the value places in register 106, and then the data from the line
   *  will be placed in the 201-232 registers, for DAQfactory to read. This is used for populating 
   *  the case records table with one button press.
   */
  if(readLineValue != prevLineValue && v_changes != 0){

      /* getLine1 will get the specified line of the text file, and formatData will split up the String
       *  and put the correct values into each register, for DAQfactory to read and then place the 
       *  values into a column on the table
       */
      formatData(getLine1(myFile, interval));
      prevLineValue = readLineValue;
      interval++;
   }

  
   


   /* If statement to check if the DAQFactory table has finished populating. If it has, set the 
    *  interval back to 0 so you can press the populate table button again if you want
    */
   if(mb.Hreg(110) == 2){
    interval = 0;
   }
   
  /* If needed, to convert from the two timestamps stored in the registers to proper UNIX time,
   * you have to concatenate the two values like this:
   * concat = hreg202 << 16 | hreg201;
   * 
   * where hreg201 and 202 are the timestamp registers
   */


   
  
  /* Hreg(107) is a register solely to tell the arduino to write a line to the SD card.
   *  DAQfactory will populate the registers with values, and then the arduino will write the
   *  value in the registers to the SD card.
   */
  if(mb.Hreg(107) == 1){
    writeToSD();
    
    mb.Hreg(107, 0);
  }

}

//----------------------------------FUNCTIONS---------------------------------------------------------


/* takes a string argument, and then writes the string to a text file on the SD card. */ 
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

/* function which takes arguments of a String, and then writes the string onto a new line on the SD 
 *  card. */
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

/* function to count how many lines of data are stored on the SD card. */
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


/* getLine function isnt used currently, but it takes linenumber as an argument, and the function will
 *  return the data on line lineNumber as a string. */
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


/* FormatData will take arguments of a String, which will be retrieved from the function "getLine1"
 *  the String gets converted into a char array, and then split up into separate variables. These 
 *  variables then get stored in their respective registers, ready for DAQfactory to read. 
 *  This function is used to populate the case history table.  */
void formatData(String data){

    // converting the string to a char array
    char charData[500];
    data.toCharArray(charData,500);
    
    int value;
    int i = 201;

    // reads the char array until there is an instance of tab (\t)
    char *token = strtok(charData, "\t");
    // converts the char array into integer
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


/* Function which reads all the values currently in the registers, appends the data to a string, then
 *  puts the string onto the SD card. */
void writeToSD(){
    
    String datastring = "";

    for(int i = 201; i < 233; i++){
      datastring += String(mb.Hreg(i));
      datastring += "\t";
    }
    
    dataWriteln(myFile, datastring);
  
}


/* Function which is used so the arduino can figure out which line to jump to when reading the SD card
 *  
 *  DAQfactory will send a value to Hreg108, which is the number of records that want to be read from
 *  the text file. Therefore, the position will be set to the beginning of line (total lines on the
 *  SD card - number of records to be read). The function gets called every time you populate the 
 *  table in DAQfactory. */
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
          return;
          
        }
    
    }
    dataFile.close();
 

  
  
  
}

/*
 * This function will take arguments of the filename, and the line of the text file that needs to be
 * read. the 'position' variable will be set by the above function 'startFrom', and when opening the
 * text file, the cursor will jump straight to the line specified by position, which is set from 
 * DAQfactory. 
 * 
 * Depending on the value of interval, this function will read line number "position + interval" */
String getLine1(String filename, int interval2){
  
    File dataFile = SD.open(filename);
    dataFile.seek(position);
    // read the line
    String data = dataFile.readStringUntil('\r');
    
    position = dataFile.position();
    
    return data;
  
}
