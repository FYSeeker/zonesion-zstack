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
#include "iic2.h"
#include <math.h>
#include <stdio.h>

/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define G10_UCPINS               ( 1 << (10%8))
#define G11_UCPINS               ( 1 << (11%8))
#define SDA_OUT                 GPIODirModeSet(GPIOA1_BASE, G11_UCPINS, GPIO_DIR_MODE_OUT)   //����GPIO11Ϊ���ģʽ
#define SDA_IN                  GPIODirModeSet(GPIOA1_BASE, G11_UCPINS, GPIO_DIR_MODE_IN)   //����GPIO11Ϊ���ģʽ

/*********************************************************************************************
* ���ƣ�iic_delay_us()
* ���ܣ���ʱ����
* ������i -- ��ʱ����
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#pragma optimize=none
static void  delay(unsigned int i)
{
  i = i*50;
  while(i--){
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
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
void iic2_init(void)
{
  PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);      //ʹ��ʱ��
  PinTypeGPIO(PIN_01,PIN_MODE_0,false);                           //ѡ������ΪGPIOģʽ��gpio10��
  PinConfigSet(PIN_01,PIN_STRENGTH_4MA,PIN_TYPE_STD_PU);
  GPIODirModeSet(GPIOA1_BASE, G10_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO17Ϊ���ģʽ
  
  PinTypeGPIO(PIN_02,PIN_MODE_0,false);                           //ѡ������ΪGPIOģʽ��gpio11��
  PinConfigSet(PIN_02,PIN_STRENGTH_4MA,PIN_TYPE_STD_PU);
  GPIODirModeSet(GPIOA1_BASE, G11_UCPINS, GPIO_DIR_MODE_OUT);   //����GPIO3Ϊ���ģʽ
  
  GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0xFF);                                                      //����������
  GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0xFF);                                                      //����ʱ����
}

/*********************************************************************************************
* ���ƣ�iic_start()
* ���ܣ�I2C��ʼ�ź�
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic2_start(void)
{

  GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0xFF);                                                      //����������
  delay(1);  
  GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0xFF);                                                      //����ʱ����
  delay(1);                                              //��ʱ
  GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0x00);                                                      //�����½���
  delay(1);                                              //��ʱ
  GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0x00);                                                      //����ʱ����
}

/*********************************************************************************************
* ���ƣ�iic_stop()
* ���ܣ�I2Cֹͣ�ź�
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic2_stop(void)
{
  GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0x00);                                                        //����������
  delay(1);  
  GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0xFF);                                                        //����ʱ����
  delay(1);                                              //��ʱ5us
  GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0xFF);                                                         //����������
}


/*********************************************************************************************
* ���ƣ�iic_write_byte()
* ���ܣ�I2Cдһ���ֽ����ݣ�����ACK����NACK���Ӹߵ��ͣ����η���
* ������data -- Ҫд������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char iic2_write_byte(unsigned char data)
{
  unsigned char i;
                                         //��ʱ2us
  for(i = 0;i < 8;i++){   
   
    if(data & 0x80){                                            //�ж��������λ�Ƿ�Ϊ1
      GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0xFF);
    }
    else{
      GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, 0x00);
    }
    delay(1);                                        
    GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0xFF);	                                                
    delay(1);                                          
    GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0x00);                                          
                                                
    data <<= 1;                                                 //��������һλ
  } 

  SDA_IN;
  delay(1);                                              //��ʱ2us
  GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0xFF);                   //����ʱ����
  if(GPIOPinRead(GPIOA1_BASE, G11_UCPINS) != 0){			//SDAΪ�ߣ��յ�NACK
    delay(1); 
    GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0x00);
    SDA_OUT; 
    return 1;	
  }else{ 				                        //SDAΪ�ͣ��յ�ACK
    delay(1); 
    GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0x00);
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
unsigned char iic2_read_byte(unsigned char ack)
{
  unsigned char i,data = 0;		                                //�ͷ�����

  SDA_IN;
  for(i = 0;i < 8;i++){
    delay(1);	                                        //��ʱ�ȴ��ź��ȶ�
    GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0xFF);		         //����������                        
    data <<= 1;	
    if(GPIOPinRead(GPIOA1_BASE, G11_UCPINS) != 0){ 		//������ȡ����
      data |= 0x01;
    }else{
      data &= 0xfe;
    }
    delay(1);
    GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0x00);	              //����������
 
  }   

  SDA_OUT;                                                //����P0_4/P0_5Ϊ���ģʽ
  GPIOPinWrite(GPIOA1_BASE, G11_UCPINS, (ack!=0)?0xff:0x00);	            //Ӧ��״̬
  delay(1);
  GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0xFF);                         
  delay(1);          
  GPIOPinWrite(GPIOA1_BASE, G10_UCPINS, 0x00);
  return data;
}


int iic2_read_buf(char addr, char r, char *buf, int len)
{
    int i;
    
    iic2_start();
    if (iic2_write_byte(addr<<1)) {
      iic2_stop();
      return -1;
    }
    if (iic2_write_byte(r)){
     iic2_stop();
      return -1;
    }
    iic2_start();
    if (iic2_write_byte((addr<<1)|1)) {
      iic2_stop();
      return -1;
    }
    for (i=0; i<len-1; i++) {
      buf[i] = iic2_read_byte(0);
    }
    buf[i++] = iic2_read_byte(1);
    iic2_stop();
    return i;
}
int iic2_write_buf(char addr, char r, char *buf, int len)
{
  int i;
  
  iic2_start();
  if (iic2_write_byte(addr<<1)) {
    iic2_stop();
    return -1;
  }

  if (iic2_write_byte(r)) {
    iic2_stop();
    return -1;
  }
 
  for (i=0; i<len; i++) {
    if (iic2_write_byte(buf[i])){
     iic2_stop();
     return -1;
    }
  }
  iic2_stop();
  return i;
}