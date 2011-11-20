#include <SPI.h>
#include "ps3_usb.h"

/* START bluetooth testing */
#include <avr/pgmspace.h>
/* END bluetooth testing */

#define LATCH_PIN 2
#define CLOCK_PIN 3
#define DATA_PIN 4

/* START bluetooth dongle analysis */
#define LOBYTE(x) ((char*)(&(x)))[0]
#define HIBYTE(x) ((char*)(&(x)))[1]
#define BUFSIZE 256    //buffer size

/* Print strings in Program Memory */
const char Gen_Error_str[] PROGMEM = "\r\nRequest error. Error code:\t"; 
const char Dev_Header_str[] PROGMEM ="Device descriptor: ";
const char Dev_Length_str[] PROGMEM ="\r\nDescriptor Length:\t";
const char Dev_Type_str[] PROGMEM ="\r\nDescriptor type:\t";
const char Dev_Version_str[] PROGMEM ="\r\nUSB version:\t";
const char Dev_Class_str[] PROGMEM ="\r\nDevice class:\t";
const char Dev_Subclass_str[] PROGMEM ="\r\nDevice Subclass:\t";
const char Dev_Protocol_str[] PROGMEM ="\r\nDevice Protocol:\t";
const char Dev_Pktsize_str[] PROGMEM ="\r\nMax.packet size:\t";
const char Dev_Vendor_str[] PROGMEM ="\r\nVendor  ID:\t";
const char Dev_Product_str[] PROGMEM ="\r\nProduct ID:\t";
const char Dev_Revision_str[] PROGMEM ="\r\nRevision ID:\t";
const char Dev_Mfg_str[] PROGMEM ="\r\nMfg.string index:\t";
const char Dev_Prod_str[] PROGMEM ="\r\nProd.string index:\t";
const char Dev_Serial_str[] PROGMEM ="\r\nSerial number index:\t";
const char Dev_Nconf_str[] PROGMEM ="\r\nNumber of conf.:\t";
const char Conf_Trunc_str[] PROGMEM ="Total length truncated to 256 bytes";
const char Conf_Header_str[] PROGMEM ="\r\nConfiguration descriptor:";
const char Conf_Totlen_str[] PROGMEM ="\r\nTotal length:\t";
const char Conf_Nint_str[] PROGMEM ="\r\nNum.intf:\t\t";
const char Conf_Value_str[] PROGMEM ="\r\nConf.value:\t";
const char Conf_String_str[] PROGMEM ="\r\nConf.string:\t";
const char Conf_Attr_str[] PROGMEM ="\r\nAttr.:\t\t";
const char Conf_Pwr_str[] PROGMEM ="\r\nMax.pwr:\t\t";
const char Int_Header_str[] PROGMEM ="\r\nInterface descriptor:";
const char Int_Number_str[] PROGMEM ="\r\nIntf.number:\t";
const char Int_Alt_str[] PROGMEM ="\r\nAlt.:\t\t";
const char Int_Endpoints_str[] PROGMEM ="\r\nEndpoints:\t\t";
const char Int_Class_str[] PROGMEM ="\r\nIntf. Class:\t\t";
const char Int_Subclass_str[] PROGMEM ="\r\nIntf. Subclass:\t";
const char Int_Protocol_str[] PROGMEM ="\r\nIntf. Protocol:\t";
const char Int_String_str[] PROGMEM ="\r\nIntf.string:\t";
const char End_Header_str[] PROGMEM ="\r\nEndpoint descriptor:";
const char End_Address_str[] PROGMEM ="\r\nEndpoint address:\t";
const char End_Attr_str[] PROGMEM ="\r\nAttr.:\t\t";
const char End_Pktsize_str[] PROGMEM ="\r\nMax.pkt size:\t";
const char End_Interval_str[] PROGMEM ="\r\nPolling interval:\t";
const char Unk_Header_str[] PROGMEM = "\r\nUnknown descriptor:";
const char Unk_Length_str[] PROGMEM ="\r\nLength:\t\t";
const char Unk_Type_str[] PROGMEM ="\r\nType:\t\t";
const char Unk_Contents_str[] PROGMEM ="\r\nContents:\t";

USB Usb2;
/* END bluetooth dongle analysis */

/** START Data for bluetooth connection **/
/* CSR Bluetooth data taken from descriptors */
/* All of this is bluetooth-dongle specific */
#define BT_ADDR        1
#define CSR_VID_LO      0x12  // CSR VID
#define CSR_VID_HI      0x0a
#define CSR_PID_LO      0x01  // Bluetooth HCI Device
#define CSR_PID_HI      0x00
#define BT_CONFIGURATION 1
#define BT_INTERFACE    0 // Only use interface 0
#define BT_NUM_EP      4
#define INT_MAXPKTSIZE   16
#define BULK_MAXPKTSIZE  64
#define EP_INTERRUPT    0x03 // endpoint types
#define EP_BULK         0x02
#define EP_POLL         0x01 // interrupt poll interval

#define CONTROL_PIPE      0 // names we give to the 4 pipes
#define EVENT_PIPE        1
#define DATAIN_PIPE       2
#define DATAOUT_PIPE      3

/* Size of data buffer: keep small to save RAM */

#define MAX_BUFFER_SIZE   64 // size of general purpose data buffer

/* Bluetooth HCI states for hci_task() */
#define HCI_INIT_STATE    0
#define HCI_RESET_STATE   1
#define HCI_BUFFER_SIZE_STATE   2
#define HCI_LOCAL_VERSION_STATE 3
#define HCI_BDADDR_STATE  4
#define HCI_LOCAL_NAME_STATE 5
#define HCI_INQUIRY_STATE 6
#define HCI_REMOTE_NAME_STATE 7
#define HCI_DISC_DELAY_STATE 8
#define HCI_CONNECT_IN_STATE 9
#define HCI_CONNECT_OUT_STATE 10
#define HCI_DONE_STATE    11



/* HCI event flags*/
#define HCI_FLAG_CMD_COMPLETE 0x01
#define HCI_FLAG_CMD_STATUS 0x02
#define HCI_FLAG_CONN_COMPLETE 0x04
#define HCI_FLAG_DISCONN_COMPLETE 0x08
#define HCI_FLAG_CONNECT_OK 0x10
#define HCI_FLAG_INQUIRY_COMPLETE 0x20
#define HCI_FLAG_REMOTE_NAME_COMPLETE 0x40
#define HCI_FLAG_INCOMING_REQUEST 0x80


/*Macros for event flag tests */
#define hci_cmd_complete (hci_event_flag & HCI_FLAG_CMD_COMPLETE)
#define hci_cmd_status (hci_event_flag & HCI_FLAG_CMD_STATUS)
#define hci_connect_complete (hci_event_flag & HCI_FLAG_CONN_COMPLETE)
#define hci_disconnect_complete (hci_event_flag & HCI_FLAG_DISCONN_COMPLETE)
#define hci_connect_ok (hci_event_flag & HCI_FLAG_CONNECT_OK)
#define hci_inquiry_complete (hci_event_flag & HCI_FLAG_INQUIRY_COMPLETE)
#define hci_remote_name_complete (hci_event_flag & HCI_FLAG_REMOTE_NAME_COMPLETE)
#define hci_incoming_connect_request (hci_event_flag & HCI_FLAG_INCOMING_REQUEST)

/* HCI Events managed */
#define EV_COMMAND_COMPLETE  0x0e
#define EV_COMMAND_STATUS    0x0f
#define EV_CONNECT_COMPLETE  0x03
#define EV_DISCONNECT_COMPLETE 0x05
#define EV_NUM_COMPLETE_PKT  0x13
#define EV_INQUIRY_COMPLETE  0x01
#define EV_INQUIRY_RESULT    0x02
#define EV_REMOTE_NAME_COMPLETE  0x07
#define EV_INCOMING_CONNECT  0x04
#define EV_ROLE_CHANGED  0x12

// used in control endpoint header for HCI Commands
#define bmREQ_HCI_OUT  USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_DEVICE
#define HCI_COMMAND_REQ    0  



EP_RECORD ep_record[ BT_NUM_EP ];  //endpoint record structure for the Bluetooth controller


/* Print strings in Program Memory */
const char Free_Memory_str[] PROGMEM = "\r\nfreeMemory() reports:\t"; 
const char Dev_Error_str[] PROGMEM ="\r\nDevice Descriptor Error:\t";
const char Wrong_Device_str[] PROGMEM ="\r\nWrong USB Device ID";
const char Config_Error_str[] PROGMEM ="\r\nError Setting Configuration:\t";
const char Int_Error_str[] PROGMEM ="\r\nError Setting Interface:\t";
const char CSR_Init_str[] PROGMEM ="\r\nCSR Initialized";
const char HCI_Reset_str[] PROGMEM ="\r\nHCI Reset complete";
const char Reset_Error_str[] PROGMEM ="\r\nNo response to HCI Reset ";
const char ACL_Length_str[] PROGMEM ="\r\nACL Data Packet Length:\t";
const char SCO_Length_str[] PROGMEM ="\r\nSCO Data Packet Length:\t";
const char ACL_Number_str[] PROGMEM ="\r\nTotal ACL Data Packets:\t ";
const char SCO_Number_str[] PROGMEM ="\r\nTotal SCO Data Packets:\t ";
const char HCI_Version_str[] PROGMEM ="\r\nHCI Version:\t ";
const char HCI_Revision_str[] PROGMEM ="\r\nHCI Revision:\t ";
const char LMP_Version_str[] PROGMEM ="\r\nLMP Version:\t";
const char Manuf_Id_str[] PROGMEM ="\r\nManufacturer Id:\t";
const char LMP_Subvers_str[] PROGMEM ="\r\nLMP Subversion:\t";
const char Local_Name_str[] PROGMEM ="\r\nLocal Name:\t";
const char Local_BDADDR_str[] PROGMEM ="\r\nLocal Bluetooth Address:\t";
const char Device_Search_str[] PROGMEM ="\r\nSearch for devices";
const char Search_Done_str[] PROGMEM ="\r\nSearch complete";
const char Device_Found_str[] PROGMEM ="\r\nDevices Found : ";
const char Found_BDADDR_str[] PROGMEM ="\r\nFound BDADDR: ";
const char Class_str[] PROGMEM ="  Class: ";
const char Mode_str[] PROGMEM ="  Mode: ";
const char Offset_str[] PROGMEM ="  Offset: ";
const char Remote_Name_str[] PROGMEM ="\r\nRemote Name: ";
const char Connect_In_str[] PROGMEM = "\r\nWait for Incoming Connect Request";
const char Device_Connected_str[] PROGMEM = "\r\nConnected to device";
const char HCI_Command_Failed_str[] PROGMEM ="\r\nHCI_Command_Failed: ";
const char Unmanaged_Event_str[] PROGMEM ="\r\nUnmanaged Event: ";




char buf[ MAX_BUFFER_SIZE ] = { 0 };      //General purpose buffer for usb data
/* variables used by high level HCI task */
unsigned char hci_state;  //current state of bluetooth hci connection
unsigned int  hci_counter; // counter used for bluetooth hci loops
unsigned char remote_name_entry;

/*variables filled from HCI event management */
unsigned int  hci_event_flag;  // flags of received bluetooth events
char hci_command_packets; //how many packets can host send to controller
int  hci_handle;
unsigned int acl_outstanding_pkt;
unsigned int acl_data_packet_length;
unsigned char sync_data_packet_length;
unsigned int acl_data_packets;
unsigned int sync_data_packets;
unsigned char hci_version;
unsigned int hci_revision;
unsigned char lmp_version;
unsigned int manufacturer_id;
unsigned int lmp_subversion;
unsigned char my_bdaddr[6]; // bluetooth address stored least significant byte first
unsigned char local_name[20];  // first 20 chars of name
unsigned char disc_bdaddr[3][6]; // maximum of three discovered devices
unsigned char disc_class[3][3];
unsigned char disc_mode[3];
unsigned int disc_offset[3];
unsigned char remote_name[3][20]; // first 20 chars of name
unsigned char discovery_results;
unsigned char entry_number;
unsigned char dev_link_type;
unsigned char dev_role;

/** END Data for bluetooth connection **/

volatile int index;
boolean buttons[16];

PS3_USB PS3Game;

int dongleDiagnosisComplete = 0;

void setup() {
  attachInterrupt(0, resetButtons, RISING);
  attachInterrupt(1, snesKeyDown, RISING);
  
  pinMode(CLOCK_PIN, INPUT);
  pinMode(LATCH_PIN, INPUT);
  pinMode(DATA_PIN, OUTPUT);
  
  digitalWrite(CLOCK_PIN, HIGH);
  
  SPI.begin();
  

  initController();
}

void initController() {
  Serial.begin(9600);
  Serial.println("PS3 controller about to be initialized");
  PS3Game.init();
  Serial.println("PS3 controller initialized");
}

void loop() {
  readControllerState();
  
/* START dongle analysis */
  Usb2.Task();
  byte rcode;
  if( dongleDiagnosisComplete == 0 && Usb2.getUsbTaskState() >= 0x80 ) {  //state configuring or higher
    rcode = getdevdescr( 1 );                    //hardcoded device address
    if( rcode ) {
      printProgStr(Gen_Error_str);
      print_hex( rcode, 8 );
    }
    rcode = getconfdescr( 1, 0 );                 //get configuration descriptor
    if( rcode ) {
      printProgStr(Gen_Error_str);
      print_hex(rcode, 8);
    }
    dongleDiagnosisComplete = 1; // Only want to print out our dongle diagnosis once
  }    
/* END dongle analysis */

/* Initialize bluetooth dongle */
    if( Usb2.getUsbTaskState() == USB_STATE_CONFIGURING ) {  //wait for addressing state
        CSR_init();
        Usb2.setUsbTaskState( USB_STATE_RUNNING );
    }
/* Take data from the bluetooth dongle */
    if( Usb2.getUsbTaskState() == USB_STATE_RUNNING ){
      HCI_task(); //poll the HCI event pipe
      ACL_event_task(); // start polling the ACL input pipe too, though discard data until connected
    }  

  digitalWrite(LATCH_PIN, HIGH);
  delayMicroseconds(12);
  digitalWrite(LATCH_PIN, LOW);
  delayMicroseconds(6);
  
  for (int i=0; i < 16; i++){
    digitalWrite(CLOCK_PIN, LOW);
    buttons[i] = digitalRead(DATA_PIN);
    delayMicroseconds(6);
    
    digitalWrite(CLOCK_PIN, HIGH);
    delayMicroseconds(6);
  }
  
  delay(17);
}

void readControllerState() {
  PS3Game.task();
  if ((PS3Game.statConnected()) && (PS3Game.statReportReceived())){ // report received ?
    if (PS3Game.buttonChanged()){
      buttons[0] = PS3Game.buttonPressed(buCross);
      buttons[1] = PS3Game.buttonPressed(buSquare);
      buttons[2] = PS3Game.buttonPressed(buSelect);
      buttons[3] = PS3Game.buttonPressed(buStart);
      buttons[4] = PS3Game.buttonPressed(buUp);
      buttons[5] = PS3Game.buttonPressed(buDown);
      buttons[6] = PS3Game.buttonPressed(buLeft);
      buttons[7] = PS3Game.buttonPressed(buRight);
      buttons[8] = PS3Game.buttonPressed(buCircle);
      buttons[9] = PS3Game.buttonPressed(buTriangle);
      buttons[10] = PS3Game.buttonPressed(buL1);
      buttons[11] = PS3Game.buttonPressed(buR1);
      buttons[12] = false;
      buttons[13] = false;
      buttons[14] = false;
      buttons[15] = false;
      
      int len = sizeof(buttons)/sizeof(boolean);

      for (int i = 0; i < len; i++){
        Serial.print("Button #");
        Serial.print(i);
        Serial.print(": ");
        Serial.println((int)buttons[i]);
      }
    }
  }
}

void snesKeyDown(){
  if (buttons[index] == 0) {
    PORTD |= B00010000; //turns signal to high
  } else {
    PORTD &= B11101111; //turns signal to low
  }
  index++;
}

void resetButtons(){
  index = 0;
}



/* START bluetooth testing */
byte getdevdescr( byte addr )
{
  USB_DEVICE_DESCRIPTOR buf;
  byte rcode;
  rcode = Usb2.getDevDescr( addr, 0, 0x12, ( char *)&buf );
  if( rcode ) {
    return( rcode );
  }
  printProgStr(Dev_Header_str);
  printProgStr(Dev_Length_str);
  print_hex( buf.bLength, 8 );
  printProgStr(Dev_Type_str);
  print_hex( buf.bDescriptorType, 8 );
  printProgStr(Dev_Version_str);
  print_hex( buf.bcdUSB, 16 );
  printProgStr(Dev_Class_str); 
  print_hex( buf.bDeviceClass, 8 );
  printProgStr(Dev_Subclass_str);
  print_hex( buf.bDeviceSubClass, 8 );
  printProgStr(Dev_Protocol_str);
  print_hex( buf.bDeviceProtocol, 8 );
  printProgStr(Dev_Pktsize_str);
  print_hex( buf.bMaxPacketSize0, 8 );
  printProgStr(Dev_Vendor_str);
  print_hex( buf.idVendor, 16 );
  printProgStr(Dev_Product_str);
  print_hex( buf.idProduct, 16 );
  printProgStr(Dev_Revision_str);
  print_hex( buf.bcdDevice, 16 );
  printProgStr(Dev_Mfg_str);
  print_hex( buf.iManufacturer, 8 );
  printProgStr(Dev_Prod_str);
  print_hex( buf.iProduct, 8 );
  printProgStr(Dev_Serial_str);
  print_hex( buf.iSerialNumber, 8 );
  printProgStr(Dev_Nconf_str);
  print_hex( buf.bNumConfigurations, 8 );
  return( 0 );
}


byte getconfdescr( byte addr, byte conf )
{
  char buf[ BUFSIZE ];
  char* buf_ptr = buf;
  byte rcode;
  byte descr_length;
  byte descr_type;
  unsigned int total_length;
  rcode = Usb2.getConfDescr( addr, 0, 4, conf, buf );  //get total length
  LOBYTE( total_length ) = buf[ 2 ];
  HIBYTE( total_length ) = buf[ 3 ];
  if( total_length > 256 ) {    //check if total length is larger than buffer
    printProgStr(Conf_Trunc_str);
    total_length = 256;
  }
  rcode = Usb2.getConfDescr( addr, 0, total_length, conf, buf ); //get the whole descriptor
  while( buf_ptr < buf + total_length ) {  //parsing descriptors
    descr_length = *( buf_ptr );
    descr_type = *( buf_ptr + 1 );
    switch( descr_type ) {
      case( USB_DESCRIPTOR_CONFIGURATION ):
        printconfdescr( buf_ptr );
        break;
      case( USB_DESCRIPTOR_INTERFACE ):
        printintfdescr( buf_ptr );
        break;
      case( USB_DESCRIPTOR_ENDPOINT ):
        printepdescr( buf_ptr );
        break;
      default:
        printunkdescr( buf_ptr );
        break;
        }//switch( descr_type  
    buf_ptr = ( buf_ptr + descr_length );    //advance buffer pointer
  }//while( buf_ptr <=...
  return( 0 );
}
/* prints hex numbers with leading zeroes */
// copyright, Peter H Anderson, Baltimore, MD, Nov, '07
// source: http://www.phanderson.com/arduino/arduino_display.html
void print_hex(int v, int num_places)
{
  int mask=0, n, num_nibbles, digit;
 
  for (n=1; n<=num_places; n++) {
    mask = (mask << 1) | 0x0001;
  }
  v = v & mask; // truncate v to specified number of places
 
  num_nibbles = num_places / 4;
  if ((num_places % 4) != 0) {
    ++num_nibbles;
  }
  do {
    digit = ((v >> (num_nibbles-1) * 4)) & 0x0f;
    Serial.print(digit, HEX);
  } 
  while(--num_nibbles);
}
/* function to print configuration descriptor */
void printconfdescr( char* descr_ptr )
{
 USB_CONFIGURATION_DESCRIPTOR* conf_ptr = ( USB_CONFIGURATION_DESCRIPTOR* )descr_ptr;
  printProgStr(Conf_Header_str);
  printProgStr(Conf_Totlen_str);
  print_hex( conf_ptr->wTotalLength, 16 );
  printProgStr(Conf_Nint_str);
  print_hex( conf_ptr->bNumInterfaces, 8 );
  printProgStr(Conf_Value_str);
  print_hex( conf_ptr->bConfigurationValue, 8 );
  printProgStr(Conf_String_str);
  print_hex( conf_ptr->iConfiguration, 8 );
  printProgStr(Conf_Attr_str);
  print_hex( conf_ptr->bmAttributes, 8 );
  printProgStr(Conf_Pwr_str);
  print_hex( conf_ptr->bMaxPower, 8 );
  return;
}
/* function to print interface descriptor */
void printintfdescr( char* descr_ptr )
{
 USB_INTERFACE_DESCRIPTOR* intf_ptr = ( USB_INTERFACE_DESCRIPTOR* )descr_ptr;
  printProgStr(Int_Header_str);
  printProgStr(Int_Number_str);
  print_hex( intf_ptr->bInterfaceNumber, 8 );
  printProgStr(Int_Alt_str);
  print_hex( intf_ptr->bAlternateSetting, 8 );
  printProgStr(Int_Endpoints_str);
  print_hex( intf_ptr->bNumEndpoints, 8 );
  printProgStr(Int_Class_str);
  print_hex( intf_ptr->bInterfaceClass, 8 );
  printProgStr(Int_Subclass_str);
  print_hex( intf_ptr->bInterfaceSubClass, 8 );
  printProgStr(Int_Protocol_str);
  print_hex( intf_ptr->bInterfaceProtocol, 8 );
  printProgStr(Int_String_str);
  print_hex( intf_ptr->iInterface, 8 );
  return;
}
/* function to print endpoint descriptor */
void printepdescr( char* descr_ptr )
{
 USB_ENDPOINT_DESCRIPTOR* ep_ptr = ( USB_ENDPOINT_DESCRIPTOR* )descr_ptr;
  printProgStr(End_Header_str);
  printProgStr(End_Address_str);
  print_hex( ep_ptr->bEndpointAddress, 8 );
  printProgStr(End_Attr_str);
  print_hex( ep_ptr->bmAttributes, 8 );
  printProgStr(End_Pktsize_str);
  print_hex( ep_ptr->wMaxPacketSize, 16 );
  printProgStr(End_Interval_str);
  print_hex( ep_ptr->bInterval, 8 );
  
  return;
}
/*function to print unknown descriptor */
void printunkdescr( char* descr_ptr )
{
  byte length = *descr_ptr;
  byte i;
  printProgStr(Unk_Header_str);
  printProgStr(Unk_Length_str);
  print_hex( *descr_ptr, 8 );
  printProgStr(Unk_Type_str);
  print_hex( *(descr_ptr + 1 ), 8 );
  printProgStr(Unk_Contents_str);
  descr_ptr += 2;
  for( i = 0; i < length; i++ ) {
    print_hex( *descr_ptr, 8 );
    descr_ptr++;
  }
}
 

/* Print a string from Program Memory directly to save RAM */
void printProgStr(const prog_char str[])
{
  char c;
  if(!str) return;
  while((c = pgm_read_byte(str++)))
    Serial.print(c,BYTE);
}
/* END bluetooth testing */

/* Initialize CSR Bluetooth Controller */
void CSR_init( void )
{
 byte rcode = 0;  //return code

 
/**/

    /* Initialize data structures for endpoints of device 1*/
    ep_record[ CONTROL_PIPE ] = *( Usb2.getDevTableEntry( 0,0 ));  //copy endpoint 0 parameters
    ep_record[ EVENT_PIPE ].epAddr = 0x01;    // Bluetooth event endpoint
    ep_record[ EVENT_PIPE ].Attr  = EP_INTERRUPT;
    ep_record[ EVENT_PIPE ].MaxPktSize = INT_MAXPKTSIZE;
    ep_record[ EVENT_PIPE ].Interval  = EP_POLL;
    ep_record[ EVENT_PIPE ].sndToggle = bmSNDTOG0;
    ep_record[ EVENT_PIPE ].rcvToggle = bmRCVTOG0;
    ep_record[ DATAIN_PIPE ].epAddr = 0x02;    // Bluetoth data endpoint
    ep_record[ DATAIN_PIPE ].Attr  = EP_BULK;
    ep_record[ DATAIN_PIPE ].MaxPktSize = BULK_MAXPKTSIZE;
    ep_record[ DATAIN_PIPE ].Interval  = 0;
    ep_record[ DATAIN_PIPE ].sndToggle = bmSNDTOG0;
    ep_record[ DATAIN_PIPE ].rcvToggle = bmRCVTOG0;
    ep_record[ DATAOUT_PIPE ].epAddr = 0x02;    // Bluetooth data endpoint
    ep_record[ DATAOUT_PIPE ].Attr  = EP_BULK;
    ep_record[ DATAOUT_PIPE ].MaxPktSize = BULK_MAXPKTSIZE;
    ep_record[ DATAOUT_PIPE ].Interval  = 0;
    ep_record[ DATAOUT_PIPE ].sndToggle = bmSNDTOG0;
    ep_record[ DATAOUT_PIPE ].rcvToggle = bmRCVTOG0;
    Usb2.setDevTableEntry( BT_ADDR, ep_record );              //plug kbd.endpoint parameters to devtable
    
    /* read the device descriptor and check VID and PID*/
    rcode = Usb2.getDevDescr( BT_ADDR, ep_record[ CONTROL_PIPE ].epAddr, DEV_DESCR_LEN , buf );
    if( rcode ) {
        printProgStr(Dev_Error_str);
        Serial.print( rcode, HEX );
        while(1);  //stop
    }
    if((buf[ 8 ] != CSR_VID_LO) || (buf[ 9 ] != CSR_VID_HI) || (buf[ 10 ] != CSR_PID_LO) || (buf[ 11 ] != CSR_PID_HI) ) {
        printProgStr(Wrong_Device_str);
          while(1);  //stop   
    }
    
    /* Configure device */
    rcode = Usb2.setConf( BT_ADDR, ep_record[ CONTROL_PIPE ].epAddr, BT_CONFIGURATION );                    
    if( rcode ) {
        printProgStr(Config_Error_str);
        Serial.print( rcode, HEX );
        while(1);  //stop
    }
  

    hci_state = HCI_INIT_STATE;
    hci_counter = 0;
  //  LCD.clear();
    
    printProgStr(CSR_Init_str);
    delay(200);
    
}

/* Poll Bluetooth and print result */

void HCI_task( void )

{
 
    HCI_event_task();
    
    switch (hci_state){
      case HCI_INIT_STATE:
      hci_counter++;
      if (hci_counter > 10){  // wait until we have looped 10 times to clear any old events
      hci_reset();
      hci_state = HCI_RESET_STATE;
      hci_counter = 0;
      }
      break;
      
      case HCI_RESET_STATE:
      hci_counter++;
      if (hci_cmd_complete){
       hci_state = HCI_BUFFER_SIZE_STATE;
       printProgStr(HCI_Reset_str);
       hci_read_buffer_size();
      }
      if (hci_counter > 1000) {
        printProgStr(Reset_Error_str);
        hci_state = HCI_INIT_STATE;
        hci_counter = 0;
      }
      break;
      
      case HCI_BUFFER_SIZE_STATE:
      if (hci_cmd_complete){
        printProgStr(ACL_Length_str);
        Serial.print(acl_data_packet_length, DEC);
        printProgStr(SCO_Length_str);
        Serial.print(sync_data_packet_length, DEC);
        printProgStr(ACL_Number_str);
        Serial.print(acl_data_packets, DEC);
        printProgStr(SCO_Number_str);
        Serial.print(sync_data_packets, DEC);
      hci_state = HCI_LOCAL_VERSION_STATE; 
      hci_read_local_version();
      }
      break;
 
      
 case HCI_LOCAL_VERSION_STATE:
      if (hci_cmd_complete){
        printProgStr(HCI_Version_str);
        Serial.print(hci_version, DEC);
        printProgStr(HCI_Revision_str);
        Serial.print(hci_revision, DEC);
        printProgStr(LMP_Version_str);
        Serial.print(lmp_version, DEC);
        printProgStr(Manuf_Id_str);
        Serial.print(manufacturer_id, DEC);
        printProgStr(LMP_Subvers_str);
        Serial.print(lmp_subversion, DEC);
      hci_state = HCI_LOCAL_NAME_STATE; 
      hci_read_local_name();
      }
      break;
 
      case HCI_LOCAL_NAME_STATE:
      if (hci_cmd_complete){
        printProgStr(Local_Name_str);
        for (char i = 0; i < 20; i++){
          Serial.print(local_name[i]);
          if(local_name[i] == NULL) break;
        }
        hci_state = HCI_BDADDR_STATE;
        hci_read_bdaddr();
      }
      break;
      
      case HCI_BDADDR_STATE:
      if (hci_cmd_complete){
        printProgStr(Local_BDADDR_str);
        for ( char i=5; i >= 0; i--){
          if (my_bdaddr[i] < 16) Serial.print("0");
          Serial.print(my_bdaddr[i], HEX);
        }
        
        printProgStr(Device_Search_str);
      hci_state = HCI_INQUIRY_STATE; 
      hci_inquiry();
      }
      break;
      
      case HCI_INQUIRY_STATE:
      if (hci_inquiry_complete){
        printProgStr(Search_Done_str); 
        printProgStr(Device_Found_str);
        Serial.print(discovery_results, DEC);
        for ( char j=0; j < discovery_results; j++){
          printProgStr(Found_BDADDR_str);
          for ( char i=5; i >= 0; i--){
            if (disc_bdaddr[j][i] < 16) Serial.print("0");
            Serial.print(disc_bdaddr[j][i], HEX);
          }
        printProgStr(Class_str);
          for ( char i=0; i < 3; i++){
            if (disc_class[j][i] < 16) Serial.print("0");
            Serial.print(disc_class[j][i], HEX);
          } 
        printProgStr(Mode_str);
        Serial.print(disc_mode[j],HEX);
        printProgStr(Offset_str);
        Serial.print(disc_offset[j],HEX);
        Serial.println(" ");
        }
        if(discovery_results){
          remote_name_entry = 0;
          hci_remote_name(remote_name_entry);
          hci_state = HCI_REMOTE_NAME_STATE;
          
        }
        else{
          hci_write_scan_enable();
          hci_state = HCI_CONNECT_IN_STATE;
          printProgStr(Connect_In_str); 
        }
          
      }
      break;
      
      case HCI_REMOTE_NAME_STATE:
     
      if(hci_remote_name_complete){
        printProgStr(Remote_Name_str);
        Serial.print(remote_name_entry, DEC);
        Serial.print(" ");
        for (char i = 0; i < 20; i++){
          Serial.print(remote_name[remote_name_entry][i]);          
        }       
        if (++remote_name_entry != discovery_results) hci_remote_name(remote_name_entry);                 
        else {                   
           hci_state = HCI_DISC_DELAY_STATE;  
           hci_counter = 0;
        }
      }
      break;
      
      case HCI_DISC_DELAY_STATE:
      if(hci_counter++ == 100){ // need to allow 100 ms for remote name to clear lmp connection
            hci_connect(0); // connect to first device 
            hci_state = HCI_CONNECT_OUT_STATE;  
      }
      break;
      
      case HCI_CONNECT_OUT_STATE:
      if(hci_connect_complete){
        printProgStr(Device_Connected_str);
        hci_state = HCI_DONE_STATE;  
      
      }      
      break;
     
           
      case HCI_CONNECT_IN_STATE:
      if(hci_incoming_connect_request){
        hci_accept_connection(0);
        printProgStr(Device_Connected_str);
        hci_state = HCI_DONE_STATE; 
      }
        
      break;
      
           
      case HCI_DONE_STATE:
      
      break;
      
      default:
      break;
   
    }

    return;
}


void HCI_event_task( void )
{
byte rcode = 0;  //return code
char char_left;
char result_pointer;
char buf_offset;
  /* check the event pipe*/
  rcode = Usb2.inTransfer(BT_ADDR, ep_record[ EVENT_PIPE ].epAddr, MAX_BUFFER_SIZE, buf, USB_NAK_NOWAIT); // input on endpoint 1
  if ( !rcode){
    switch (buf[0]){            //switch on event type
    
      case EV_COMMAND_COMPLETE:
      hci_command_packets = buf[2]; // update flow control
      hci_event_flag |= HCI_FLAG_CMD_COMPLETE; // set command complete flag
      
      // process parameters if any
      
      if((buf[3] == 0x09) && (buf[4] == 0x10)){  // parameters from read local bluetooth address
        for (char i = 0; i < 6; i++) my_bdaddr[i] = (unsigned char) buf[6 + i];
      }
      
      if((buf[3] == 0x05) && (buf[4] == 0x10)){  //parameters from read buffer size
       acl_data_packet_length = (unsigned char) buf[6] | (unsigned char) buf[7] << 8; 
       sync_data_packet_length =(unsigned char) buf[8];
       acl_data_packets= (unsigned char) buf[9]  | (unsigned char) buf[10] << 8; 
       sync_data_packets =(unsigned char) buf[11] | (unsigned char) buf[12] << 8;  
      }
      if((buf[3] == 0x01) && (buf[4] == 0x10)){  // parameters from read local version
       hci_version = (unsigned char) buf[6]; 
       hci_revision =(unsigned char) buf[7] | (unsigned char) buf[8] << 8; 
       lmp_version= (unsigned char) buf[9]; 
       manufacturer_id = (unsigned char) buf[10] | (unsigned char) buf[11] << 8;  
       lmp_subversion =(unsigned char) buf[12] | (unsigned char) buf[13] << 8;  
      }
      
      if((buf[3] == 0x14) && (buf[4] == 0x0c)){ // parameters from read local name
        for (char i = 0; i <20; i++){ // save first 20 bytes
          local_name[i] = (unsigned char) buf[6 + i];         
        }
        
        for(char i = 0; i < 3; i++){ // ignore rest
          Usb2.inTransfer(BT_ADDR, ep_record[ EVENT_PIPE ].epAddr, MAX_BUFFER_SIZE, buf, USB_NAK_NOWAIT);
        }
        
      }
      break;
      
    case EV_COMMAND_STATUS:
    
      hci_command_packets = buf[3]; // update flow control
      hci_event_flag |= HCI_FLAG_CMD_STATUS; //set status flag
      if(buf[2]){    // show status on serial if not OK  
        printProgStr(HCI_Command_Failed_str);
        Serial.print( buf[2], HEX );
        Serial.print(' ');
        Serial.print( buf[4], HEX );
        Serial.print(' ');
        Serial.print( buf[5], HEX );
        
      }
      break;
    
    case EV_CONNECT_COMPLETE:
    
      hci_event_flag |= HCI_FLAG_CONN_COMPLETE; // set connection complete flag
      if (!buf[2]){ // check if connected OK
      hci_handle = (unsigned char) buf[3] | (unsigned char) buf[4] << 8; //store the handle for the ACL connection
      hci_event_flag |= HCI_FLAG_CONNECT_OK; //set connection OK flag
      }
      break;
    
    
    case EV_DISCONNECT_COMPLETE:
    
      hci_event_flag |= HCI_FLAG_DISCONN_COMPLETE; //set disconnect commend complete flag
      if (!buf[2]){ // check if disconnected OK
        hci_event_flag &= ~(HCI_FLAG_CONNECT_OK); //clear connection OK flag
      }
      break;
    
    case EV_NUM_COMPLETE_PKT:
      acl_outstanding_pkt -= (unsigned char) buf[6] | (unsigned char) buf[7] << 8;;    //
      break;
    
    case EV_INQUIRY_COMPLETE:
      hci_event_flag |= HCI_FLAG_INQUIRY_COMPLETE;
      break;
  
    case EV_INQUIRY_RESULT:
      char_left = 62;
      result_pointer = 0;
      buf_offset = 3;
      discovery_results += buf[2];
      
      for (char buf_count = 0; buf_count < buf[1]; buf_count++){
           if (!char_left){
             Usb2.inTransfer(BT_ADDR, ep_record[ EVENT_PIPE ].epAddr, MAX_BUFFER_SIZE, buf, USB_NAK_NOWAIT);
             char_left = 64; 
             buf_offset = 0;         
           }
           else char_left--;
         
           if(result_pointer < 6) disc_bdaddr[entry_number][ result_pointer] = (unsigned char) buf[(buf_count % 16) + buf_offset];
           else if (result_pointer == 6) disc_mode[entry_number] = (unsigned char) buf[(buf_count % 16) + buf_offset];
           else if ((result_pointer > 8) &&( result_pointer < 12)) disc_class[entry_number][result_pointer - 9] = (unsigned char) buf[(buf_count % 16) + buf_offset];
           else if (result_pointer == 12) disc_offset[entry_number] = (unsigned char) buf[(buf_count % 16) + buf_offset];
           else if (result_pointer == 13){
             disc_offset[entry_number] |= (unsigned char) buf[(buf_count  % 16)+ buf_offset] << 8;
             entry_number++;
           }       
          result_pointer++; // 14 bytes to each result
          if (result_pointer == 14) result_pointer = 0;
     } 
     break;     
       
   
    case EV_REMOTE_NAME_COMPLETE:
    
      for (char i = 0; i < 20; i++){
          remote_name[remote_name_entry][i] = (unsigned char) buf[9 + i];  //store first 20 bytes  
        }
        for(char i = 0; i < 4; i++){ // discard additional bytes
          Usb2.inTransfer(BT_ADDR, ep_record[ EVENT_PIPE ].epAddr, MAX_BUFFER_SIZE, buf, USB_NAK_NOWAIT);
        }
      hci_event_flag |=HCI_FLAG_REMOTE_NAME_COMPLETE;
      break;
  
    case EV_INCOMING_CONNECT:
      disc_bdaddr[0][0] = (unsigned char) buf[2];
      disc_bdaddr[0][1] = (unsigned char) buf[3];
      disc_bdaddr[0][2] = (unsigned char) buf[4];
      disc_bdaddr[0][3] = (unsigned char) buf[5];
      disc_bdaddr[0][4] = (unsigned char) buf[6];
      disc_bdaddr[0][5] = (unsigned char) buf[7];
      disc_class[0][0] = (unsigned char) buf[8];
      disc_class[0][1] = (unsigned char) buf[9];
      disc_class[0][2] = (unsigned char) buf[10];
      dev_link_type = (unsigned char) buf[11];
      hci_event_flag |=HCI_FLAG_INCOMING_REQUEST;
      break;
    
    case EV_ROLE_CHANGED:
      dev_role = (unsigned char)buf[9];
      break;
          
    
    default:
      printProgStr(Unmanaged_Event_str);
      Serial.println( buf[0], HEX );
      break;
    
    }  // switch
  }
  return;
}

void ACL_event_task()
{
    Usb2.inTransfer(BT_ADDR, ep_record[ DATAIN_PIPE ].epAddr, MAX_BUFFER_SIZE, buf, USB_NAK_NOWAIT); // input on endpoint 2

}

void hci_reset(void)
{
   hci_event_flag = 0; // clear all the flags
   buf[0] = 0x03;
   buf[1] = 0x0c;
   buf[2] = 0x00;
   HCI_Command(3 , buf);
  return;
}

void hci_connect(char disc_device)
{
   hci_event_flag &= ~(HCI_FLAG_CONN_COMPLETE | HCI_FLAG_CONNECT_OK);
   buf[0] = 0x05; // HCI OCF = 5
   buf[1]= 0x04; // HCI OGF = 1
   buf[2] = 0x0d; // parameter length =13
   buf[3] = disc_bdaddr[disc_device][0]; // 6 octet bluetooth address
   buf[4] = disc_bdaddr[disc_device][1];
   buf[5] = disc_bdaddr[disc_device][2];
   buf[6] = disc_bdaddr[disc_device][3];
   buf[7] = disc_bdaddr[disc_device][4];
   buf[8] = disc_bdaddr[disc_device][5];
   buf[9] = 0x18; // DM1 or DH1 may be used
   buf[10] = 0xcc; // DM3, DH3, DM5, DH5 may be used
   buf[11] = 0x01; // page repetition mode R1
   buf[12] = 0x00; // always 0
   buf[13] = 0x00; // clock offset 
   buf[14] = 0x00;  // 
   buf[15] = 0x00; //  do not allow role switch
   HCI_Command(16 , buf);
 
  return;
}
void hci_read_buffer_size(void)
{
   
   buf[0] = 0x05; // HCI OCF = 5
   buf[1] = 0x10; // HCI OGF = 4
   buf[2] = 0x00;
   HCI_Command(3 , buf);
  return;
  
}

void hci_read_local_version(void)
{
   
   buf[0] = 0x01; // HCI OCF = 1
   buf[1] = 0x10; // HCI OGF = 4
   buf[2] = 0x00;
   HCI_Command(3 , buf);
  return;
  
}


void hci_read_local_name(void)
{
   
   buf[0] = 0x14; // HCI OCF = 14
   buf[1] = 0x0c; // HCI OGF = 3
   buf[2] = 0x00;
   HCI_Command(3 , buf);
  return;
  
}

void hci_write_scan_enable(void)
{
   
   buf[0] = 0x1A; // HCI OCF = 1A
   buf[1] = 0x0c; // HCI OGF = 3
   buf[2] = 0x01;
   buf[3] = 0x03;
   HCI_Command(4 , buf);
  return;
  
}

void hci_read_bdaddr(void)
{
   
   buf[0] = 0x09; // HCI OCF = 9
   buf[1] = 0x10; // HCI OGF = 4
   buf[2] = 0x00;
   HCI_Command(3 , buf);
  return;
  
}



void hci_disconnect(void)
{
   hci_event_flag &= ~HCI_FLAG_DISCONN_COMPLETE;
   buf[0] = 0x06; // HCI OCF = 6
   buf[1]= 0x04; // HCI OGF = 1
   buf[2] = 0x03; // parameter length =3
   buf[3] = hci_handle & 0x0f; // handle
   buf[4] = hci_handle >> 8;
   buf[5] = 0x13; // reason
   
   HCI_Command(6 , buf);
 
  return;
}

void hci_inquiry(void)
{
   hci_event_flag &= ~HCI_FLAG_INQUIRY_COMPLETE;
   entry_number = 0;
   discovery_results = 0;
   buf[0] = 0x01; // HCI OCF = 1
   buf[1]= 0x04; // HCI OGF = 1
   buf[2] = 0x05; // parameter length =3
   buf[3] = 0x33; // LAP unlimited inquiry
   buf[4] = 0x8B;
   buf[5] = 0x9E;
   buf[6] = 0x0A; // Inquiry time
   buf[7] = 0x03; // Max 3 results
   
   HCI_Command(8 , buf);
 
  return;
}

void hci_accept_connection(char disc_device)
{
   hci_event_flag |= HCI_FLAG_CONNECT_OK;
   hci_event_flag &= ~(HCI_FLAG_INCOMING_REQUEST);
   
   buf[0] = 0x09; // HCI OCF = 9
   buf[1]= 0x04; // HCI OGF = 1
   buf[2] = 0x07; // parameter length 7
   buf[3] =disc_bdaddr[disc_device][0]; // 6 octet bdaddr
   buf[4] = disc_bdaddr[disc_device][1];
   buf[5] = disc_bdaddr[disc_device][2];
   buf[6] = disc_bdaddr[disc_device][3];
   buf[7] = disc_bdaddr[disc_device][4];
   buf[8] = disc_bdaddr[disc_device][5];
   buf[9] = 0; //switch role to master
   
   HCI_Command(10 , buf);
 
  return;
}


void hci_remote_name(char disc_device)
{
   hci_event_flag &= ~(HCI_FLAG_REMOTE_NAME_COMPLETE);
   buf[0] = 0x19; // HCI OCF = 19
   buf[1]= 0x04; // HCI OGF = 1
   buf[2] = 0x0a; // parameter length =10
   buf[3] = disc_bdaddr[disc_device][0]; // 6 octet bdaddr
   buf[4] = disc_bdaddr[disc_device][1];
   buf[5] = disc_bdaddr[disc_device][2];
   buf[6] = disc_bdaddr[disc_device][3];
   buf[7] = disc_bdaddr[disc_device][4];
   buf[8] = disc_bdaddr[disc_device][5];
   buf[9] = disc_mode[disc_device];
   buf[10] = 0x00; // always 0
   buf[11] = 0x00; // offset
   buf[12] = 0x00; 
   
   HCI_Command(13 , buf);
 
  return;
}





//perform HCI Command
byte HCI_Command( unsigned int nbytes, char* dataptr ) {
    hci_command_packets--; 
    hci_event_flag &= ~HCI_FLAG_CMD_COMPLETE;
    return( Usb2.ctrlReq( BT_ADDR, ep_record[ CONTROL_PIPE ].epAddr, bmREQ_HCI_OUT, HCI_COMMAND_REQ, 0x00, 0x00 ,0, nbytes, dataptr ));
}
