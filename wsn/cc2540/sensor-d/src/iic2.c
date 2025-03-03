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
#include <ioCC2530.h>
#include <math.h>
#include <stdio.h>

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define    SCL	                P0_3       	                	//IIC时钟引脚定义
#define    SDA	                P0_2       	                	//IIC数据引脚定义

#define    SDA_IN               (P0DIR &= ~(1<<2))
#define    SDA_OUT              (P0DIR |= (1<<2))
/*********************************************************************************************
* 名称：iic_delay_us()
* 功能：延时函数
* 参数：i -- 延时设置
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
#pragma optimize=none
static void  delay(unsigned int i)
{

}

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
  P0SEL &= ~((1<<2)+(1<<3));                                    //设置普通IO模式
  P0DIR |= (1<<2)+(1<<3);                                       //设置为输出模式
  SDA = 1;                                                      //拉高数据线
  SCL = 1;                                                      //拉高时钟线

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
 
  SDA = 1;                                                      //拉高数据
  delay(1); 
  SCL = 1;                                                      //拉高时钟线
  delay(1);                                              //延时
  SDA = 0;                                                      //产生下降沿
  delay(1);                                              //延时
  SCL = 0;                                                      //拉低时钟线
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
  SDA =0;                                                       //拉低数据线
  delay(1); 
  SCL =1;                                                       //拉高时钟线
  delay(1);                                              //延时5us
  SDA=1;                                                        //产生上升沿
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

  for(i = 0;i < 8;i++){                                         
    if(data & 0x80){                                            //判断数据最高位是否为1
      SDA = 1; 
    }
    else{
      SDA = 0;
    }
    delay(1);                                            //延时5us
    SCL = 1;	                                                //输出SDA稳定后，拉高SCL给出上升沿，从机检测到后进行数据采样
    delay(1);                                            //延时5us
    SCL = 0;                                                    //拉低时钟线
    data <<= 1;                                                 //数组左移一位
  } 
  
  SDA_IN;
  delay(1);                                              //延时2us
  SCL = 1;                                                      //拉高时钟线
  if(SDA == 1){			                                //SDA为高，收到NACK
    
    SCL = 0;
    SDA_OUT;
    return 1;	
  }else{ 				                        //SDA为低，收到ACK
    SCL = 0;
    SDA_OUT;
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

  SDA_IN;
  for(i = 0;i < 8;i++){		                                        
    delay(1);	                                        //延时等待信号稳定
    SCL = 1;		                                        //给出上升沿
    data <<= 1;		                                
    data |= SDA;                                //采样获取数据
    delay(1);
    SCL = 0;  
  }
  SDA_OUT;
  SDA = ack;	                                                //应答状态
  delay(1);
  SCL = 1;                         
  delay(1);          
  SCL = 0;
 
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

