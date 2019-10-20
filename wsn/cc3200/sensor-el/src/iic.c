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
static void  iic_delay_us(unsigned int i)
{
  while(i--){
     asm("nop"); asm("nop"); asm("nop");asm("nop"); asm("nop"); asm("nop");asm("nop"); asm("nop"); asm("nop");asm("nop");
    asm("nop"); asm("nop"); asm("nop");asm("nop"); asm("nop"); asm("nop");asm("nop"); asm("nop"); asm("nop");asm("nop");
    
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
  //iic_delay_us(5);                                             //��ʱ10us
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                                                      //����ʱ����
  //iic_delay_us(5);                                             //��ʱ10us
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
  SDA_OUT;
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xFF);                                                      //����������
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                                                      //����ʱ����
  iic_delay_us(1);                                              //��ʱ
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0);                                                      //�����½���
  iic_delay_us(1);                                              //��ʱ
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);                                                      //����ʱ����
  iic_delay_us(1); 
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
  SDA_OUT;
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0);                                                       //����������
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                                                       //����ʱ����
  iic_delay_us(1);                                              //��ʱ5us
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xFF);                                                        //����������
  iic_delay_us(1);                                              //��ʱ5us
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
  SDA_OUT;
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);                                                      //����ʱ����
  for(i = 0;i < 8;i++){                                         
    if(data & 0x80){                                            //�ж��������λ�Ƿ�Ϊ1
      GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xFF);	                                                
    }
    else
      GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0);
    iic_delay_us(1);                                            //��ʱ5us
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);	                                                //���SDA�ȶ�������SCL���������أ��ӻ���⵽��������ݲ���
    iic_delay_us(1);                                            //��ʱ5us
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);                                                    //����ʱ����
    //iic_delay_us(1);                                            //��ʱ5us
    data <<= 1;                                                 //��������һλ
  } 
  //iic_delay_us(2);                                              //��ʱ2us
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xFF);                                                      //����������
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                                                      //����ʱ����
  SDA_IN;
  iic_delay_us(1);                                              //��ʱ2us���ȴ��ӻ�Ӧ��
  if(GPIOPinRead(GPIOA0_BASE, G3_UCPINS) != 0){			                                //SDAΪ�ߣ��յ�NACK
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);
    iic_delay_us(1);     
    return 1;	
  }else{ 				                        //SDAΪ�ͣ��յ�ACK
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);
    iic_delay_us(1);
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
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, 0xFF);			                                //�ͷ�����	
  SDA_IN;
  for(i = 0;i < 8;i++){
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);		                                        //����������
    iic_delay_us(1);	                                        //��ʱ�ȴ��ź��ȶ�
    data <<= 1;	
    if(GPIOPinRead(GPIOA0_BASE, G3_UCPINS) != 0){ 		                                //������ȡ����
      data |= 0x01;
    }else{
      data &= 0xfe;
    }
   
    GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);   		                                        //�½��أ��ӻ�������һλֵ
    iic_delay_us(1);
  }
  SDA_OUT;
  GPIOPinWrite(GPIOA0_BASE, G3_UCPINS, ack);	                                                //Ӧ��״̬
 
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0XFF);                         
  iic_delay_us(1);          
  GPIOPinWrite(GPIOA1_BASE, G13_UCPINS, 0);
  iic_delay_us(1);
  return data;
}