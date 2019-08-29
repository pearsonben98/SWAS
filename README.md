# SWAS
Son of Whole Air Sampler - 8 Week Summer Project at Wolfson Atmospheric Chemistry Laboratories. 

SWAS_WASP_SINGLE_CASE.ctl - DAQFactory file which uses the ModbusRTU protocol along with an RS485 module to communicate to an arduino
slave device for a whole air sampler. 

The program can automate the process of sampling canisters inside the air sampler by connecting to a GC-FID(Gas Chromatography - 
Flame Ionisation Detector), and will also be able to read past data from the slave device to determine the current state of each 
SWAS case.

SWAS_WASP_MULTI_CASE.ctl - Same as the single case file, however enables the functionality to connect to multiple cases at the same time 
instead of just one. This program is a prototype, as I have been unable to test the functionality of connecting two cases.

SWAS_WASP_SINGLE_CASE.ino (inside the folder) - Each SWAS case is represented by a single arduino slave device, and this is the code that
gets flashed onto the slave arduino. Uses the MODBUSRTU protocol over RS485 to send data to and from the Master device (the device that
will be running DAQFactory, which is in this case the control box).

The google_docs_files is autogenerated by DAQFactory, depending on which case is connected. Say you have case5 connected, and there is 
some data on the SD card on case 5, this data will be transferred over to DAQFactory, and then read into a .csv format to be opened in
Microsoft Excel.
