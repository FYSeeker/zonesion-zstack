/*********************************************************************************************
* 文件：iic.c
* 作者：zonesion
* 说明：iic驱动程序
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stm32f10x.h"
#include "delay.h"
/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define I2C_GPIO                GPIOA
#define I2C_CLK                 RCC_APB2Periph_GPIOA
#define PIN_SCL                 GPIO_Pin_2
#define PIN_SDA                 GPIO_Pin_3

#define SDA_R                   GPIO_ReadInputDataBit(I2C_GPIO,PIN_SDA)
                 

/*********************************************************************************************
* 名称：iic_init()
* 功能：I2C初始化函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic2_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(I2C_CLK, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = PIN_SCL | PIN_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);  
}

/*********************************************************************************************
* 名称：sda_out()
* 功能：设置SDA为输出
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void sda_out(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PIN_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);  
}

/*********************************************************************************************
* 名称：sda_in()
* 功能：设置SDA为输入
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void sda_in(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PIN_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);  
}

/*********************************************************************************************
* 名称：iic_start()
* 功能：I2C起始信号
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic2_start(void)
{

  GPIO_SetBits(I2C_GPIO,PIN_SDA);                               //拉高数据线
  delay_us(5); 
  GPIO_SetBits(I2C_GPIO,PIN_SCL);                               //拉高时钟线
  delay_us(5);                                                  //延时
  GPIO_ResetBits(I2C_GPIO,PIN_SDA);                             //产生下降沿
  delay_us(5);                                                  //延时
  GPIO_ResetBits(I2C_GPIO,PIN_SCL);                             //拉低时钟线
}

/*********************************************************************************************
* 名称：iic_stop()
* 功能：I2C停止信号
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void iic2_stop(void)
{
  
  GPIO_ResetBits(I2C_GPIO,PIN_SDA);                             //拉低数据线
  delay_us(5); 
  GPIO_SetBits(I2C_GPIO,PIN_SCL);                               //拉高时钟线
  delay_us(5);                                                  //延时5us
  GPIO_SetBits(I2C_GPIO,PIN_SDA);                               //产生上升沿
}


/*********************************************************************************************
* 名称：iic_write_byte()
* 功能：I2C写一个字节数据，返回ACK或者NACK，从高到低，依次发送
* 参数：data -- 要写的数据
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char iic2_write_byte(unsigned char data)
{
  unsigned char i;
 
  GPIO_ResetBits(I2C_GPIO,PIN_SCL);                             //拉低时钟线
  for(i = 0;i < 8;i++){                                         
    if(data & 0x80){                                            //判断数据最高位是否为1
      GPIO_SetBits(I2C_GPIO,PIN_SDA);  	                                                
    }
    else
      GPIO_ResetBits(I2C_GPIO,PIN_SDA); 
    delay_us(5);                                                //延时5us
    GPIO_SetBits(I2C_GPIO,PIN_SCL); 	                        //输出SDA稳定后，拉高SCL给出上升沿，从机检测到后进行数据采样
    delay_us(5);                                                //延时5us
    GPIO_ResetBits(I2C_GPIO,PIN_SCL);                           //拉低时钟线
    
    data <<= 1;                                                 //数组左移一位
  } 
  sda_in();
  delay_us(5);                                                  //延时2us
 
  GPIO_SetBits(I2C_GPIO,PIN_SCL);                               //拉高时钟线
  if(SDA_R){			                                //SDA为高，收到NACK
     GPIO_ResetBits(I2C_GPIO,PIN_SCL);                           //释放总线 
    sda_out();
    return 1;	
  }else{ 				                        //SDA为低，收到ACK
    GPIO_ResetBits(I2C_GPIO,PIN_SCL);                           //释放总线                  
    sda_out();
    return 0;
  }
}

/*********************************************************************************************
* 名称：iic_read_byte()
* 功能：I2C写一个字节数据，返回ACK或者NACK，从高到低，依次发送
* 参数：data -- 要写的数据
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char iic2_read_byte(unsigned char ack)
{
  unsigned char i,data = 0;
  
  GPIO_ResetBits(I2C_GPIO,PIN_SCL); 
  sda_in();
  for(i = 0;i < 8;i++){
    delay_us(5);
    GPIO_SetBits(I2C_GPIO,PIN_SCL);  		                //给出上升沿
    	                                        
    data <<= 1;	
    if(SDA_R){ 		                                        //采样获取数据
      data |= 0x01;
    }else{
      data &= 0xfe;
    }
    delay_us(5);
    GPIO_ResetBits(I2C_GPIO,PIN_SCL);    		        //下降沿，从机给出下一位值
  }
  sda_out();
  if(ack)
    GPIO_SetBits(I2C_GPIO,PIN_SDA);	                        //应答状态
  else
    GPIO_ResetBits(I2C_GPIO,PIN_SDA);  
  delay_us(5);
  GPIO_SetBits(I2C_GPIO,PIN_SCL);                        
  delay_us(5);          
  GPIO_ResetBits(I2C_GPIO,PIN_SCL); 

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