/*********************************************************************************************
* �ļ���iic.c
* ���ߣ�zonesion
* ˵����iic��������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "iic.h"
/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define G13_UCPINS              ( 1 << (13%8))
#define G3_UCPINS               ( 1 << (3%8))
#define SDA_OUT                 GPIODirModeSet(GPIOA0_BASE, G3_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO3Ϊ���ģʽ
#define SDA_IN                  GPIODirModeSet(GPIOA0_BASE, G3_UCPINS, GPIO_DIR_MODE_IN);   //����GPIO3Ϊ���ģʽ

/*********************************************************************************************
* ���ƣ�iic_delay_us()
* ���ܣ���ʱ����
* ������i -- ��ʱ����
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#pragma optimize=none
static void  iic_delay_us(unsigned int i)
{
  while(i--){
    
  }
}

/*********************************************************************************************
* ���ƣ�iic_init()
* ���ܣ�I2C��ʼ������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_init(void)
{
  PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
  PinTypeGPIO(PIN_58,PIN_MODE_0,false);                           //ѡ������ΪGPIOģʽ��gpio4��
  PinConfigSet(PIN_58,PIN_STRENGTH_2MA,PIN_TYPE_STD_PU);
  GPIODirModeSet(GPIOA0_BASE, G3_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO17Ϊ���ģʽ
  
  PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
  PinTypeGPIO(PIN_04,PIN_MODE_0,false);                           //ѡ������ΪGPIOģʽ��gpio5��
  PinConfigSet(PIN_04,PIN_STRENGTH_2MA,PIN_TYPE_STD_PU);
  GPIODirModeSet(GPIOA1_BASE, G13_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO3Ϊ���ģʽ
  
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xFF);                                                      //����������
  iic_delay_us(5);                                             //��ʱ10us
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                                                      //����ʱ����
  iic_delay_us(5);                                             //��ʱ10us
}

/*********************************************************************************************
* ���ƣ�iic_start()
* ���ܣ�I2C��ʼ�ź�
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_start(void)
{
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xFF);                                                      //����������
  iic_delay_us(1);
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                                                      //����ʱ����
  iic_delay_us(1);                                              //��ʱ
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0);                                                      //�����½���
  iic_delay_us(1);                                              //��ʱ
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);                                                      //����ʱ����
}

/*********************************************************************************************
* ���ƣ�iic_stop()
* ���ܣ�I2Cֹͣ�ź�
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_stop(void)
{
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0);                                                       //����������
  iic_delay_us(1);  
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                                                       //����ʱ����
  iic_delay_us(1);                                              //��ʱ5us
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xFF);                                                        //����������
                                         
}

/*********************************************************************************************
* ���ƣ�iic_write_byte()
* ���ܣ�I2Cдһ���ֽ����ݣ�����ACK����NACK���Ӹߵ��ͣ����η���
* ������data -- Ҫд������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char iic_write_byte(unsigned char data)
{
  unsigned char i;
  
  for(i = 0;i < 8;i++){                                         
    if(data & 0x80){                                            //�ж��������λ�Ƿ�Ϊ1
      GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xFF);	                                                
    }
    else
      GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0);
    iic_delay_us(1);                                            //��ʱ5us
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);	        //���SDA�ȶ�������SCL���������أ��ӻ���⵽��������ݲ���
    iic_delay_us(1);                                            //��ʱ5us
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);                                                
    data <<= 1;                                                 //��������һλ
  } 

  SDA_IN;
  iic_delay_us(1);
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                  //����ʱ����
                     
  if(GPIOPinRead(GPIOA0_BASE, G3_UCPINS) != 0){			//SDAΪ�ߣ��յ�NACK
    iic_delay_us(1); 
     GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);
     SDA_OUT;
     return 1;	
  }else{ 				                        //SDAΪ�ͣ��յ�ACK
    iic_delay_us(1);
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);
    SDA_OUT;
    return 0;
  }
}

/*********************************************************************************************
* ���ƣ�iic_read_byte()
* ���ܣ�I2Cдһ���ֽ����ݣ�����ACK����NACK���Ӹߵ��ͣ����η���
* ������data -- Ҫд������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char iic_read_byte(unsigned char ack)
{
  unsigned char i,data = 0;
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);
  SDA_IN;
  for(i = 0;i < 8;i++){
    iic_delay_us(1);
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);		                 //����������
    data <<= 1;	
    if(GPIOPinRead(GPIOA0_BASE, G3_UCPINS) != 0){ 		                 //������ȡ����
      data |= 0x01;
    }else{
      data &= 0xfe;
    }
    iic_delay_us(1);
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);   		                 //�½��أ��ӻ�������һλֵ
  }
  SDA_OUT;
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS,  (ack!=0)?0xff:0x00);	                                 //Ӧ��״̬
  iic_delay_us(1);
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                         
  iic_delay_us(1);          
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);
  return data;
}
int iic_read_buf(char addr, char r, char *buf, int len)
{
    int i;
    
    iic_start();
    if (iic_write_byte(addr<<1)) {
      iic_stop();
      return -1;
    }
    if (iic_write_byte(r)) return -1;
   
    iic_start();
    if (iic_write_byte((addr<<1)|1)){
      iic_stop();
      return -1;
    }
   
    for (i=0; i<len-1; i++) {
      buf[i] = iic_read_byte(0);
    }
    buf[i++] = iic_read_byte(1);
    iic_stop();
    return i;
}
int iic_write_buf(char addr, char r, char *buf, int len)
{
  int i;
  
  iic_start();
  if (iic_write_byte(addr<<1)) {
      iic_stop();
      return -1;
    }

  if (iic_write_byte(r)) {
      iic_stop();
      return -1;
    }
 
  for (i=0; i<len; i++) {
    if (iic_write_byte(buf[i])) {
      iic_stop();
      return -1;
    }
  }
  iic_stop();
  return i;
}