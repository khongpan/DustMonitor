#include <Arduino.h>
#include <Wire.h>
#include "CCS811.h"

HardwareSerial _serial(0);

#define SCL_PIN (22)
#define SDA_PIN (21)
#define WAK_PIN (34)
#define INT_PIN (03)
#define RST_PIN (35)
#define ADD_PIN (23)

#define PRINTLN(s)    Serial.println(s)
#define PRINT(s)      Serial.print(s)
#define PRINTLN2(s,m) Serial.println(s,m)
#define PRINT2(s,m)   Serial.print(s,m)
//#define PRINTLN(s)
//#define PRINT(s)
//#define PRINTLN2(s,m)
//#define PRINT2(s,m)


#define CCS811_WAIT_AFTER_RESET_US     2000 
#define CCS811_WAIT_AFTER_APPSTART_US  1000 
#define CCS811_WAIT_AFTER_WAKE_US        50 
#define CCS811_WAIT_AFTER_APPERASE_MS   500 
#define CCS811_WAIT_AFTER_APPVERIFY_MS   70 
#define CCS811_WAIT_AFTER_APPDATA_MS     50 


// Main interface =====================================================================================================
#define CCS811_STATUS           0x00
#define CCS811_MEAS_MODE        0x01
#define CCS811_ALG_RESULT_DATA  0x02 // up to 8 bytes
#define CCS811_RAW_DATA         0x03 // 2 bytes
#define CCS811_ENV_DATA         0x05 // 4 bytes
#define CCS811_THRESHOLDS       0x10 // 5 bytes
#define CCS811_BASELINE         0x11 // 2 bytes
#define CCS811_HW_ID            0x20
#define CCS811_HW_VERSION       0x21
#define CCS811_FW_BOOT_VERSION  0x23 // 2 bytes
#define CCS811_FW_APP_VERSION   0x24 // 2 bytes
#define CCS811_ERROR_ID         0xE0
#define CCS811_APP_ERASE        0xF1 // 4 bytes
#define CCS811_APP_DATA         0xF2 // 9 bytes
#define CCS811_APP_VERIFY       0xF3 // 0 bytes
#define CCS811_APP_START        0xF4 // 0 bytes
#define CCS811_SW_RESET         0xFF // 4 bytes


CCS811::CCS811(int nwake, int slaveaddr) {
  _nwake= nwake;
  _slaveaddr= slaveaddr;
  _i2cdelay_us= 0;

  pinMode(INT_PIN, INPUT);
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN,HIGH);
  pinMode(ADD_PIN, OUTPUT);
  digitalWrite(ADD_PIN, HIGH);
  wake_init();
}

bool CCS811::begin( void ) {
  uint8_t sw_reset[]= {0x11,0xE5,0x72,0x8A};
  uint8_t app_start[]= {};
  uint8_t hw_id;
  uint8_t hw_version;
  uint8_t app_version[2];
  uint8_t status;
  bool ok;


  wake_up();


    ok= i2cwrite(0,0,0);
    if( !ok ) ok= i2cwrite(0,0,0); 
    if( !ok ) {

      _slaveaddr= CCS811_SLAVEADDR_0 + CCS811_SLAVEADDR_1 - _slaveaddr; 
      ok= i2cwrite(0,0,0);
      _slaveaddr= CCS811_SLAVEADDR_0 + CCS811_SLAVEADDR_1 - _slaveaddr; 
      if( ok ) {
        PRINTLN(F("ccs811: wrong slave address, ping successful on other address"));
      } else {
        PRINTLN(F("ccs811: ping failed (VDD/GND connected? SDA/SCL connected?)"));
      }
      goto abort_begin;
    }

  
    ok= i2cwrite(CCS811_SW_RESET,4,sw_reset);
    if( !ok ) {
      PRINTLN(F("ccs811: reset failed"));
      goto abort_begin;
    }
    delayMicroseconds(CCS811_WAIT_AFTER_RESET_US);

 
    ok= i2cread(CCS811_HW_ID,1,&hw_id);
    if( !ok ) {
      PRINTLN(F("ccs811: HW_ID read failed"));
      goto abort_begin;
    }
    if( hw_id!=0x81 ) {
      PRINT(F("ccs811: Wrong HW_ID: "));
      PRINTLN2(hw_id,HEX);
      goto abort_begin;
    }


    ok= i2cread(CCS811_HW_VERSION,1,&hw_version);
    if( !ok ) {
      PRINTLN(F("ccs811: HW_VERSION read failed"));
      goto abort_begin;
    }
    if( (hw_version&0xF0)!=0x10 ) {
      PRINT(F("ccs811: Wrong HW_VERSION: "));
      PRINTLN2(hw_version,HEX);
      goto abort_begin;
    }


    ok= i2cread(CCS811_STATUS,1,&status);
    if( !ok ) {
      PRINTLN(F("ccs811: STATUS read (boot mode) failed"));
      goto abort_begin;
    }
    if( status!=0x10 ) {
      PRINT(F("ccs811: Not in boot mode, or no valid app: "));
      PRINTLN2(status,HEX);
      goto abort_begin;
    }


    ok= i2cread(CCS811_FW_APP_VERSION,2,app_version);
    if( !ok ) {
      PRINTLN(F("ccs811: APP_VERSION read failed"));
      goto abort_begin;
    }
    _appversion= app_version[0]*256+app_version[1];


    ok= i2cwrite(CCS811_APP_START,0,app_start);
    if( !ok ) {
      PRINTLN(F("ccs811: Goto app mode failed"));
      goto abort_begin;
    }
    delayMicroseconds(CCS811_WAIT_AFTER_APPSTART_US);

   
    ok= i2cread(CCS811_STATUS,1,&status);
    if( !ok ) {
      PRINTLN(F("ccs811: STATUS read (app mode) failed"));
      goto abort_begin;
    }
    if( status!=0x90 ) {
      PRINT(F("ccs811: Not in app mode, or no valid app: "));
      PRINTLN2(status,HEX);
      goto abort_begin;
    }


  wake_down();
  // Return success
  return true;

abort_begin:

  wake_down();
 
  return false;
}

bool CCS811::start( int mode ) {
  uint8_t meas_mode[]= {(uint8_t)(mode<<4)};
  wake_up();
  bool ok = i2cwrite(CCS811_MEAS_MODE,1,meas_mode);
  wake_down();
  return ok;
}
void CCS811::read( uint16_t*eco2, uint16_t*etvoc, uint16_t*errstat,uint16_t*raw) {
  bool    ok;
  uint8_t buf[8];
  uint8_t stat;
  wake_up();
    if( _appversion<0x2000 ) {
      ok= i2cread(CCS811_STATUS,1,&stat); 
      if( ok && stat==CCS811_ERRSTAT_OK ) ok= i2cread(CCS811_ALG_RESULT_DATA,8,buf); else buf[5]=0;
      buf[4]= stat; 
    } else {
      ok = i2cread(CCS811_ALG_RESULT_DATA,8,buf);
    }
  wake_down();
 
  uint16_t combined = buf[5]*256+buf[4];
  if( combined & ~(CCS811_ERRSTAT_HWERRORS|CCS811_ERRSTAT_OK) ) ok= false; 
  combined &= CCS811_ERRSTAT_HWERRORS|CCS811_ERRSTAT_OK; 
  if( !ok ) combined |= CCS811_ERRSTAT_I2CFAIL;

  if( combined & CCS811_ERRSTAT_HWERRORS ) {
      int err = get_errorid();
      if( err==-1 ) combined |= CCS811_ERRSTAT_I2CFAIL; 
  }
 
  if( eco2   ) *eco2   = buf[0]*256+buf[1];
  if( etvoc  ) *etvoc  = buf[2]*256+buf[3];
  if( errstat) *errstat= combined;
  if( raw    ) *raw    = buf[6]*256+buf[7];;
}
const char * CCS811::errstat_str(uint16_t errstat) {
  static char s[17]; 
                                                  s[ 0]='-';
                                                  s[ 1]='-';
  if( errstat & CCS811_ERRSTAT_HEATER_SUPPLY    ) s[ 2]='V'; else s[2]='v';
  if( errstat & CCS811_ERRSTAT_HEATER_FAULT     ) s[ 3]='H'; else s[3]='h';
  if( errstat & CCS811_ERRSTAT_MAX_RESISTANCE   ) s[ 4]='X'; else s[4]='x';
  if( errstat & CCS811_ERRSTAT_MEASMODE_INVALID ) s[ 5]='M'; else s[5]='m';
  if( errstat & CCS811_ERRSTAT_READ_REG_INVALID ) s[ 6]='R'; else s[6]='r';
  if( errstat & CCS811_ERRSTAT_WRITE_REG_INVALID) s[ 7]='W'; else s[7]='w';
 
  if( errstat & CCS811_ERRSTAT_FW_MODE          ) s[ 8]='F'; else s[8]='f';
                                                  s[ 9]='-';
                                                  s[10]='-';
  if( errstat & CCS811_ERRSTAT_APP_VALID        ) s[11]='A'; else s[11]='a';
  if( errstat & CCS811_ERRSTAT_DATA_READY       ) s[12]='D'; else s[12]='d';
                                                  s[13]='-';
 
  if( errstat & CCS811_ERRSTAT_I2CFAIL          ) s[14]='I'; else s[14]='i';
  if( errstat & CCS811_ERRSTAT_ERROR            ) s[15]='E'; else s[15]='e';
                                                  s[16]='\0';
  return s;
}

int CCS811::hardware_version(void) {
  uint8_t buf[1];
  wake_up();
  bool ok = i2cread(CCS811_HW_VERSION,1,buf);
  wake_down();
  int version= -1;
  if( ok ) version= buf[0];
  return version;
}
int CCS811::bootloader_version(void) {
  uint8_t buf[2];
  wake_up();
  bool ok = i2cread(CCS811_FW_BOOT_VERSION,2,buf);
  wake_down();
  int version= -1;
  if( ok ) version= buf[0]*256+buf[1];
  return version;
}
int CCS811::application_version(void) {
  uint8_t buf[2];
  wake_up();
  bool ok = i2cread(CCS811_FW_APP_VERSION,2,buf);
  wake_down();
  int version= -1;
  if( ok ) version= buf[0]*256+buf[1];
  return version;
}
int CCS811::get_errorid(void) {
  uint8_t buf[1];
  wake_up();
  bool ok = i2cread(CCS811_ERROR_ID,1,buf);
  wake_down();
  int version= -1;
  if( ok ) version= buf[0];
  return version;
}


#define HI(u16) ( (uint8_t)( ((u16)>>8)&0xFF ) )
#define LO(u16) ( (uint8_t)( ((u16)>>0)&0xFF ) )


bool CCS811::set_envdata(uint16_t t, uint16_t h) {
  uint8_t envdata[]= { HI(h), LO(h), HI(t), LO(t) };
  wake_up();
  // Serial.print(" [T="); Serial.print(t); Serial.print(" H="); Serial.print(h); Serial.println("] ");
  bool ok = i2cwrite(CCS811_ENV_DATA,4,envdata);
  wake_down();
  return ok;
}

bool CCS811::set_envdata210(uint16_t t, uint16_t h) {
  
  uint16_t lo= 15882; // (273.15-25)*64 = 15881.6 (float to int error is 0.4)
  uint16_t hi= 24073; // 65535/8+lo = 24073.875 (24074 would map to 65539, so overflow)
 
  bool ok;
  if( t<lo )      ok= set_envdata(0,h);
  else if( t>hi ) ok= set_envdata(65535,h);
  else            ok= set_envdata( (t-lo)*8+3 , h); 
 
  return ok;
}



bool CCS811::get_baseline(uint16_t *baseline) {
  uint8_t buf[2];
  wake_up();
  bool ok = i2cread(CCS811_BASELINE,2,buf);
  wake_down();
  *baseline= (buf[0]<<8) + buf[1];
  return ok;
}
bool CCS811::set_baseline(uint16_t baseline) {
  uint8_t buf[]= { HI(baseline), LO(baseline) };
  wake_up();
  bool ok = i2cwrite(CCS811_BASELINE,2,buf);
  wake_down();
  return ok;
}

bool CCS811::flash(const uint8_t * image, int size) {
  uint8_t sw_reset[]=   {0x11,0xE5,0x72,0x8A};
  uint8_t app_erase[]=  {0xE7,0xA7,0xE6,0x09};
  uint8_t app_verify[]= {};
  uint8_t status;
  int count;
  bool ok;
  wake_up();

    PRINT(F("ccs811: ping "));
    ok= i2cwrite(0,0,0);
    if( !ok ) {
      PRINTLN(F("FAILED"));
      goto abort_begin;
    }
    PRINTLN(F("ok"));

    PRINT(F("ccs811: reset "));
    ok= i2cwrite(CCS811_SW_RESET,4,sw_reset);
    if( !ok ) {
      PRINTLN(F("FAILED"));
      goto abort_begin;
    }
    delayMicroseconds(CCS811_WAIT_AFTER_RESET_US);
    PRINTLN(F("ok"));

    PRINT(F("ccs811: status (reset1) "));
    ok= i2cread(CCS811_STATUS,1,&status);
    if( !ok ) {
      PRINTLN(F("FAILED"));
      goto abort_begin;
    }
    PRINT2(status,HEX);
    PRINT(F(" "));
    if( status!=0x00 && status!=0x10 ) {
      PRINTLN(F("ERROR - ignoring")); 
    } else {
      PRINTLN(F("ok"));
    }

    PRINT(F("ccs811: app-erase "));
    ok= i2cwrite(CCS811_APP_ERASE,4,app_erase);
    if( !ok ) {
      PRINTLN(F("FAILED"));
      goto abort_begin;
    }
    delay(CCS811_WAIT_AFTER_APPERASE_MS);
    PRINTLN(F("ok"));

    PRINT(F("ccs811: status (app-erase) "));
    ok= i2cread(CCS811_STATUS,1,&status);
    if( !ok ) {
      PRINTLN(F("FAILED"));
      goto abort_begin;
    }
    PRINT2(status,HEX);
    PRINT(F(" "));
    if( status!=0x40 ) {
      PRINTLN(F("ERROR"));
      goto abort_begin;
    }
    PRINTLN(F("ok"));

    
    count= 0;
    while( size>0 ) {
        if( count%64==0 ) { PRINT(F("ccs811: writing ")); PRINT(size); PRINT(F(" ")); }
        int len= size<8 ? size : 8;
        
        uint8_t ram[8];
        memcpy_P(ram, image, len);
        
        ok= i2cwrite(CCS811_APP_DATA,len, ram);
        if( !ok ) {
          PRINTLN(F("ccs811: app data failed"));
          goto abort_begin;
        }
        PRINT(F("."));
        delay(CCS811_WAIT_AFTER_APPDATA_MS);
        image+= len;
        size-= len;
        count++;
        if( count%64==0 ) { PRINT(F(" ")); PRINTLN(size); }
    }
    if( count%64!=0 ) { PRINT(F(" ")); PRINTLN(size); }

    PRINT(F("ccs811: app-verify "));
    ok= i2cwrite(CCS811_APP_VERIFY,0,app_verify);
    if( !ok ) {
      PRINTLN(F("FAILED"));
      goto abort_begin;
    }
    delay(CCS811_WAIT_AFTER_APPVERIFY_MS);
    PRINTLN(F("ok"));

    
    PRINT(F("ccs811: status (app-verify) "));
    ok= i2cread(CCS811_STATUS,1,&status);
    if( !ok ) {
      PRINTLN(F("FAILED"));
      goto abort_begin;
    }
    PRINT2(status,HEX);
    PRINT(F(" "));
    if( status!=0x30 ) {
      PRINTLN(F("ERROR"));
      goto abort_begin;
    }
    PRINTLN(F("ok"));

    
    PRINT(F("ccs811: reset2 "));
    ok= i2cwrite(CCS811_SW_RESET,4,sw_reset);
    if( !ok ) {
      PRINTLN(F("FAILED"));
      goto abort_begin;
    }
    delayMicroseconds(CCS811_WAIT_AFTER_RESET_US);
    PRINTLN(F("ok"));
    PRINT(F("ccs811: status (reset2) "));
    ok= i2cread(CCS811_STATUS,1,&status);
    if( !ok ) {
      PRINTLN(F("FAILED"));
      goto abort_begin;
    }
    PRINT2(status,HEX);
    PRINT(F(" "));
    if( status!=0x10 ) {
      PRINTLN(F("ERROR"));
      goto abort_begin;
    }
    PRINTLN(F("ok"));

  
  wake_down();
  
  return true;

abort_begin:
 
  wake_down();
  // Return failure
  return false;
}


// Advanced interface: i2cdelay ========================================================================================
void CCS811::set_i2cdelay(int us) {
  if( us<0 ) us= 0;
  _i2cdelay_us= us;
}


// Get current delay
int  CCS811::get_i2cdelay(void) {
  return _i2cdelay_us;
}


// Helper interface: nwake pin ========================================================================================
void CCS811::wake_init( void ) {
  if( _nwake>=0 ) pinMode(_nwake, OUTPUT);
}


void CCS811::wake_up( void) {
  //if( _nwake>=0 ) pinMode(_nwake, OUTPUT);
  if( _nwake>=0 ) { digitalWrite(_nwake, LOW); delayMicroseconds(CCS811_WAIT_AFTER_WAKE_US);  }
}


void CCS811::wake_down( void) {
  if( _nwake>=0 ) digitalWrite(_nwake, HIGH);
}


// Helper interface: i2c wrapper ======================================================================================


// Writes `count` from `buf` to register at address `regaddr` in the CCS811. Returns false on I2C problems.
bool CCS811::i2cwrite(int regaddr, int count, const uint8_t * buf) {
  Wire.beginTransmission(_slaveaddr);              // START, SLAVEADDR
  Wire.write(regaddr);                             // Register address
  for( int i=0; i<count; i++) Wire.write(buf[i]);  // Write bytes
  int r= Wire.endTransmission(true);               // STOP
  return r==0;
}

// Reads 'count` bytes from register at address `regaddr`, and stores them in `buf`. Returns false on I2C problems.
bool CCS811::i2cread(int regaddr, int count, uint8_t * buf) {
  Wire.beginTransmission(_slaveaddr);              // START, SLAVEADDR
  Wire.write(regaddr);                             // Register address
  int wres= Wire.endTransmission(false);           // Repeated START
  delayMicroseconds(_i2cdelay_us);                 // Wait
  int rres=Wire.requestFrom(_slaveaddr,count);     // From CCS811, read bytes, STOP
  for( int i=0; i<count; i++ ) buf[i]=Wire.read();
  return (wres==0) && (rres==count);
}


CCS811 ccs811(WAK_PIN);

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskCCS811(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
   uint16_t eco2, etvoc, errstat, raw;
  for (;;) 
  {


      ccs811.read(&eco2,&etvoc,&errstat,&raw); 
    
      if( errstat==CCS811_ERRSTAT_OK ) { 
        Serial.print("CCS811: ");
        Serial.print("eco2=");  Serial.print(eco2);     Serial.print(" ppm  ");
        Serial.print("etvoc="); Serial.print(etvoc);    Serial.print(" ppb  ");
        //Serial.print("raw6=");  Serial.print(raw/1024); Serial.print(" uA  "); 
        //Serial.print("raw10="); Serial.print(raw%1024); Serial.print(" ADC  ");
        //Serial.print("R="); Serial.print((1650*1000L/1023)*(raw%1024)/(raw/1024)); Serial.print(" ohm");
        Serial.println();
      } else if( errstat==CCS811_ERRSTAT_OK_NODATA ) {
        Serial.println("CCS811: waiting for (new) data");
      } else if( errstat & CCS811_ERRSTAT_I2CFAIL ) { 
        Serial.println("CCS811: I2C error");
      } else {
        Serial.print("CCS811: errstat="); Serial.print(errstat,HEX); 
        Serial.print("="); Serial.println( ccs811.errstat_str(errstat) ); 
      }

       vTaskDelay(1000);  
    
  }
}
void CCS811Setup() {
  _serial.begin(115200,SERIAL_8N1,2,15);  
  pinMode(WAK_PIN, OUTPUT);
  digitalWrite(WAK_PIN,HIGH);
  pinMode(INT_PIN, INPUT);
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN,LOW);
  pinMode(ADD_PIN, OUTPUT);
  digitalWrite(ADD_PIN, LOW);

  vTaskDelay(1000);
  digitalWrite(RST_PIN,HIGH);
  vTaskDelay(1000);
  digitalWrite(WAK_PIN,LOW);
  Serial.println("");
  Serial.println("setup: Starting CCS811 basic demo");
  Serial.print("setup: ccs811 lib  version: "); Serial.println(CCS811_VERSION);

 
  Wire.begin();
  //Wire.begin(21,22,0);
  //Wire.begin(5,18,0); 
  
  
  ccs811.set_i2cdelay(50); 
  bool ok= ccs811.begin();
  if( !ok ) Serial.println("setup: CCS811 begin FAILED");

  
  Serial.print("setup: hardware    version: "); Serial.println(ccs811.hardware_version(),HEX);
  Serial.print("setup: bootloader  version: "); Serial.println(ccs811.bootloader_version(),HEX);
  Serial.print("setup: application version: "); Serial.println(ccs811.application_version(),HEX);
  
  
  ok= ccs811.start(CCS811_MODE_1SEC);
  if( !ok ) Serial.println("setup: CCS811 start FAILED");

  
// 
//  xTaskCreate(
//    TaskCCS811
//    ,  "TaskCCS811"   
//    ,  1024  
//    ,  NULL
//    ,  2  
//    ,  NULL );

  
}
