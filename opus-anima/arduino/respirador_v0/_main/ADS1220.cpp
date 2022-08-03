

#include <Arduino.h>
#include "_global.h"

//#define BOARD_SENSYTHING ST_1_3

ADS1220::ADS1220()                 // Constructors
{

}

void ADS1220::writeRegister(uint8_t address, uint8_t value)
{
  digitalWrite(m_cs_pin, LOW);
  delay(5);
  SPI.transfer(WREG | (address << 2));
  SPI.transfer(value);
  delay(5);
  digitalWrite(m_cs_pin, HIGH);
}

uint8_t ADS1220::readRegister(uint8_t address)
{
  uint8_t data;

  digitalWrite(m_cs_pin, LOW);
  delay(5);
  SPI.transfer(RREG | (address << 2));
  data = SPI.transfer(SPI_MASTER_DUMMY);
  delay(5);
  digitalWrite(m_cs_pin, HIGH);

  return data;
}

void ADS1220::begin(uint8_t cs_pin, uint8_t drdy_pin)
{
  m_drdy_pin = drdy_pin;
  m_cs_pin = cs_pin;

  pinMode(m_cs_pin, OUTPUT);
  pinMode(m_drdy_pin, INPUT);

#if defined(BOARD_SENSYTHING)
  SPI.begin(18, 35, 23, 19);
#else
  SPI.begin();
#endif
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  delay(50);
  ads1220_Reset();
  delay(50);

  digitalWrite(m_cs_pin, LOW);

  m_config_reg0 = 0x00;   //Default settings: AINP=AIN0, AINN=AIN1, Gain 1, PGA enabled
  m_config_reg1 = 0x04;   //Default settings: DR=20 SPS, Mode=Normal, Conv mode=continuous, Temp Sensor disabled, Current Source off
  m_config_reg2 = 0x10;   //Default settings: Vref internal, 50/60Hz rejection, power open, IDAC off
  m_config_reg3 = 0x00;   //Default settings: IDAC1 disabled, IDAC2 disabled, DRDY pin only

  writeRegister( CONFIG_REG0_ADDRESS , m_config_reg0);
  writeRegister( CONFIG_REG1_ADDRESS , m_config_reg1);
  writeRegister( CONFIG_REG2_ADDRESS , m_config_reg2);
  writeRegister( CONFIG_REG3_ADDRESS , m_config_reg3);

  delay(50);

  Config_Reg0 = readRegister(CONFIG_REG0_ADDRESS);
  Config_Reg1 = readRegister(CONFIG_REG1_ADDRESS);
  Config_Reg2 = readRegister(CONFIG_REG2_ADDRESS);
  Config_Reg3 = readRegister(CONFIG_REG3_ADDRESS);

  digitalWrite(m_cs_pin, HIGH);
  delay(10);
}

void ADS1220::SPI_Command(unsigned char data_in)
{
  digitalWrite(m_cs_pin, LOW);
  delay(2);
  digitalWrite(m_cs_pin, HIGH);
  delay(2);
  digitalWrite(m_cs_pin, LOW);
  delay(2);
  SPI.transfer(data_in);
  delay(2);
  digitalWrite(m_cs_pin, HIGH);
}

void ADS1220::ads1220_Reset()
{
  SPI_Command(RESET);
}

void ADS1220::Start_Conv()
{
  SPI_Command(START);
}

void ADS1220::PGA_ON(void)
{
  m_config_reg0 &= ~_BV(0);
  writeRegister(CONFIG_REG0_ADDRESS, m_config_reg0);
}

void ADS1220::PGA_OFF(void)
{
  m_config_reg0 |= _BV(0);
  writeRegister(CONFIG_REG0_ADDRESS, m_config_reg0);
}

void ADS1220::set_conv_mode_continuous(void)
{
  m_config_reg1 |= _BV(2);
  writeRegister(CONFIG_REG1_ADDRESS, m_config_reg1);
}

void ADS1220::set_conv_mode_single_shot(void)
{
  m_config_reg1 &= ~_BV(2);
  writeRegister(CONFIG_REG1_ADDRESS, m_config_reg1);
}

void ADS1220::set_data_rate(int datarate)
{
  m_config_reg1 &= ~REG_CONFIG1_DR_MASK;
  m_config_reg1 |= datarate;
  writeRegister(CONFIG_REG1_ADDRESS, m_config_reg1);
}

void ADS1220::select_mux_channels(int channels_conf)
{
  m_config_reg0 &= ~REG_CONFIG0_MUX_MASK;
  m_config_reg0 |= channels_conf;
  writeRegister(CONFIG_REG0_ADDRESS, m_config_reg0);
}

void ADS1220::set_pga_gain(int pgagain)
{
  m_config_reg0 &= ~REG_CONFIG0_PGA_GAIN_MASK;
  m_config_reg0 |= pgagain ;
  writeRegister(CONFIG_REG0_ADDRESS, m_config_reg0);
}

uint8_t * ADS1220::get_config_reg()
{
  static uint8_t config_Buff[4];

  m_config_reg0 = readRegister(CONFIG_REG0_ADDRESS);
  m_config_reg1 = readRegister(CONFIG_REG1_ADDRESS);
  m_config_reg2 = readRegister(CONFIG_REG2_ADDRESS);
  m_config_reg3 = readRegister(CONFIG_REG3_ADDRESS);

  config_Buff[0] = m_config_reg0 ;
  config_Buff[1] = m_config_reg1 ;
  config_Buff[2] = m_config_reg2 ;
  config_Buff[3] = m_config_reg3 ;

  return config_Buff;
}


// inclui para o turbo
void ADS1220::set_op_turbo()
{
  Config_Reg1 &= ~REG_CONFIG_OM_MASK;
  Config_Reg1 |= 0x10;
  writeRegister(CONFIG_REG1_ADDRESS, Config_Reg1);
}


int32_t ADS1220::Read_WaitForData()
{
  static byte SPI_Buff[3];
  int32_t mResult32 = 0;
  long int bit24;
  convTimeOut = false;


  if ((digitalRead(m_drdy_pin)) == HIGH)            //        Wait for DRDY to transition low
  {
    // aguardo o tempo de conversao, caso necesssario
    // se demorar mais que 2 milisegundos, retorno zero
    unsigned long start = millis();
    while ( (digitalRead(m_drdy_pin)) == HIGH) {
      if ( passouTempoMillis(start, 2)) {
        convTimeOut = true;
        return mResult32;
      }
    }
  }

  digitalWrite(m_cs_pin, LOW);                        //Take CS low
  // O valor original era 100 us, mas no datasheet pede 50 de minimo, entao dei margem de 20%
  delayMicroseconds(60);
  for (int i = 0; i < 3; i++)
  {
    SPI_Buff[i] = SPI.transfer(SPI_MASTER_DUMMY);
  }
  // O valor original era 100 us, mas no datasheet pede 50 de minimo, entao dei margem de 20%
  delayMicroseconds(30);
  digitalWrite(m_cs_pin, HIGH);                 //  Clear CS to high

  bit24 = SPI_Buff[0];
  bit24 = (bit24 << 8) | SPI_Buff[1];
  bit24 = (bit24 << 8) | SPI_Buff[2];                                 // Converting 3 bytes to a 24 bit int

  bit24 = ( bit24 << 8 );
  mResult32 = ( bit24 >> 8 );                      // Converting 24 bit two's complement to 32 bit two's complement


  return mResult32;
}

int32_t ADS1220::Read_SingleShot_WaitForData(void)
{
  static byte SPI_Buff[3];
  int32_t mResult32 = 0;
  long int bit24;

  Start_Conv();

  if ((digitalRead(m_drdy_pin)) == LOW)            //        Wait for DRDY to transition low
  {
    digitalWrite(m_cs_pin, LOW);                        //Take CS low
    delayMicroseconds(100);
    for (int i = 0; i < 3; i++)
    {
      SPI_Buff[i] = SPI.transfer(SPI_MASTER_DUMMY);
    }
    delayMicroseconds(100);
    digitalWrite(m_cs_pin, HIGH);                 //  Clear CS to high

    bit24 = SPI_Buff[0];
    bit24 = (bit24 << 8) | SPI_Buff[1];
    bit24 = (bit24 << 8) | SPI_Buff[2];                                 // Converting 3 bytes to a 24 bit int

    bit24 = ( bit24 << 8 );
    mResult32 = ( bit24 >> 8 );                      // Converting 24 bit two's complement to 32 bit two's complement
  }
  return mResult32;
}

int32_t ADS1220::Read_SingleShot_SingleEnded_WaitForData(uint8_t channel_no)
{
  static byte SPI_Buff[3];
  int32_t mResult32 = 0;
  long int bit24;

  select_mux_channels(channel_no);
  delay(50);

  Start_Conv();
  delay(50);

  if ((digitalRead(m_drdy_pin)) == LOW) //        Wait for DRDY to transition low
  {
    digitalWrite(m_cs_pin, LOW);//Take CS low
    delayMicroseconds(100);
    for (int i = 0; i < 3; i++)
    {
      SPI_Buff[i] = SPI.transfer(SPI_MASTER_DUMMY);
    }
    delayMicroseconds(100);
    digitalWrite(m_cs_pin, HIGH);//  Clear CS to high

    bit24 = SPI_Buff[0];
    bit24 = (bit24 << 8) | SPI_Buff[1];
    bit24 = (bit24 << 8) | SPI_Buff[2]; // Converting 3 bytes to a 24 bit int

    bit24 = ( bit24 << 8 );
    mResult32 = ( bit24 >> 8 ); // Converting 24 bit two's complement to 32 bit two's complement
  }
  return mResult32;
}

bool ADS1220::getTimeOutFlag(void)
{
  return convTimeOut;
}
