/*********************************************************************************************
* 文件：oled.c
* 作者：zonesion
* 说明：
* 修改：Chenkm 2017.01.05 增加了注释
* 注释：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "iic.h"
#include "oled.h"
#include "oledfont.h"
#include <string.h>

#define ADDR_W  0X78                                            //主机写地址
#define ADDR_R  0X79                                            //主机读地址
#define Max_Column	128
unsigned char OLED_GRAM[128][8];

/*********************************************************************************************
* 名称: OLED_Init()
* 功能: OLED初始化
* 参数: 无
* 返回: 无
* 注释：
* 修改:
*********************************************************************************************/
void  OLED_Init(void){
  iic_init();
  OLED_Write_command(0xAE);//--display off
	OLED_Write_command(0x00);//---set low column address
	OLED_Write_command(0x10);//---set high column address
	OLED_Write_command(0x40);//--set start line address  
	OLED_Write_command(0xB0);//--set page address
	OLED_Write_command(0x81); // contract control
	OLED_Write_command(0xFF);//--128   
	OLED_Write_command(0xA1);//set segment remap 
	OLED_Write_command(0xA6);//--normal / reverse
	OLED_Write_command(0xA8);//--set multiplex ratio(1 to 64)
	OLED_Write_command(0x3F);//--1/32 duty
	OLED_Write_command(0xC8);//Com scan direction
	OLED_Write_command(0xD3);//-set display offset
	OLED_Write_command(0x00);//
	
	OLED_Write_command(0xD5);//set osc division
	OLED_Write_command(0x80);//
	
	OLED_Write_command(0xD8);//set area color mode off
	OLED_Write_command(0x05);//
	
	OLED_Write_command(0xD9);//Set Pre-Charge Period
	OLED_Write_command(0xF1);//
	
	OLED_Write_command(0xDA);//set com pin configuartion
	OLED_Write_command(0x12);//
	
	OLED_Write_command(0xDB);//set Vcomh
	OLED_Write_command(0x30);//
	
	OLED_Write_command(0x8D);//set charge pump enable
	OLED_Write_command(0x14);//
	
	OLED_Write_command(0xAF);//--turn on oled panel
}

/*********************************************************************************************
* 名称: OLED_Write_command()
* 功能: IIC Write Command
* 参数: 命令
* 返回: 无
* 修改:
* 注释: 
*********************************************************************************************/
void OLED_Write_command(unsigned char IIC_Command)
{
  iic_start();						        //启动总线
  if(iic_write_byte(ADDR_W) == 0){		        //地址设置
    if(iic_write_byte(0x00) == 0){			        //命令输入
      iic_write_byte(IIC_Command);	                                //等待数据传输完成
    }
  }
  iic_stop();
}

/*********************************************************************************************
* 名称: OLED_IIC_write()
* 功能: IIC Write Data
* 参数: 数据
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
void OLED_IIC_write(unsigned char IIC_Data)
{
  iic_start();						        //启动总线
  if(iic_write_byte(ADDR_W) == 0){		        //地址设置
    if(iic_write_byte(0x40) == 0){			        //命令输入
      iic_write_byte(IIC_Data);	                                //等待数据传输完成
    }
  }
  iic_stop();
}

/*********************************************************************************************
* 名称: OLED_fillpicture()
* 功能: OLED_fillpicture
* 参数: 数据
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
void OLED_fillpicture(unsigned char fill_Data){
  unsigned char m,n;
  for(m=0;m<8;m++){
    OLED_Write_command(0xb0+m);		                        //page0-page1
    OLED_Write_command(0x00);		                        //low column start address
    OLED_Write_command(0x10);		                        //high column start address
    for(n=0;n<128;n++){
      OLED_IIC_write(fill_Data);
    }
  }
}

/*********************************************************************************************
* 名称: OLED_Set_Pos()
* 功能: 坐标设置
* 参数: 坐标
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) { 	
  OLED_Write_command(0xb0+y);
  OLED_Write_command(((x&0xf0)>>4)|0x10);
  OLED_Write_command((x&0x0f)); 
} 

/*********************************************************************************************
* 名称: OLED_Display_On()
* 功能: 开启OLED显示
* 参数: 
* 返回: 
* 修改:
* 注释:
*********************************************************************************************/
void OLED_Display_On(void){
  OLED_Write_command(0X8D);                                     //SET DCDC命令
  OLED_Write_command(0X14);                                     //DCDC ON
  OLED_Write_command(0XAF);                                     //DISPLAY ON
}

/*********************************************************************************************
* 名称: OLED_Display_Off()
* 功能: 关闭OLED显示
* 参数: 
* 返回: 
* 修改:
* 注释:
*********************************************************************************************/  
void OLED_Display_Off(void){
  OLED_Write_command(0X8D);                                     //SET DCDC命令
  OLED_Write_command(0X10);                                     //DCDC OFF
  OLED_Write_command(0XAE);                                     //DISPLAY OFF
}

/*********************************************************************************************
* 名称: OLED_Clear()
* 功能: 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
* 参数: 
* 返回: 
* 修改:
* 注释:
*********************************************************************************************/  
void OLED_Clear(void)  {  
  unsigned char i,n;		    
  for(i=0;i<8;i++)  {  
    OLED_Write_command (0xb0+i);                                //设置页地址（0~7）
    OLED_Write_command (0x00);                                  //设置显示位置—列低地址
    OLED_Write_command (0x10);                                  //设置显示位置—列高地址   
    for(n=0;n<128;n++)
      OLED_IIC_write(0); 
  } 
}

//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
  unsigned char i,n;		    
  for(i=0;i<8;i++)  
  {  
    OLED_Write_command (0xb0+i);                                //设置页地址（0~7）
    OLED_Write_command (0x00);                                  //设置显示位置—列低地址
    OLED_Write_command (0x10);                                  //设置显示位置—列高地址   
    for(n=0;n<128;n++)OLED_IIC_write(OLED_GRAM[n][i]); 
  }   
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t)
{
  unsigned char pos,bx,temp=0;
  if(x>127||y>63)return;//超出范围了.
  pos=7-y/8;
  bx=y%8;
  temp=1<<(7-bx);
  if(t)OLED_GRAM[x][pos]|=temp;
  else OLED_GRAM[x][pos]&=~temp;	    
}
void OLED_DisFill(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char dot)  
{  
  unsigned char x,y;  
  for(x=x1;x<=x2;x++)
  {
    for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
  }													    
  //OLED_Refresh_Gram();//更新显示
}
/*********************************************************************************************
* 名称: OLED_DisClear()
* 功能: 区域清空
* 参数: 坐标
* 返回: 
* 修改:
* 注释:
*********************************************************************************************/  
void OLED_DisClear(int hstart,int hend,int lstart,int lend){
  unsigned char i,n;
  for(i=hstart;i<=hend;i++)  {  
    OLED_Write_command (0xb0+i);                                //设置页地址（0~7）
    OLED_Write_command (0x00);                                  //设置显示位置—列低地址
    OLED_Write_command (0x10);                                  //设置显示位置—列高地址   
    for(n=lstart;n<=lend;n++)
      OLED_IIC_write(0); 
  } 
}

/*********************************************************************************************
* 名称: OLED_ShowChar()
* 功能: 在指定位置显示一个字符,包括部分字符
* 参数: 坐标（x:0~127；y:0~63），chr字符；Char_Size字符长度
* 返回: 
* 修改:
* 注释:
*********************************************************************************************/  
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size){      	
  unsigned char c=0,i=0;	
  c=chr-' ';//得到偏移后的值			
  if(x>Max_Column-1){x=0;y=y+2;}
  if(Char_Size ==16){
    OLED_Set_Pos(x,y);	
    for(i=0;i<8;i++)
      OLED_IIC_write(F8X16[c*16+i]);
    OLED_Set_Pos(x,y+1);
    for(i=0;i<8;i++)
      OLED_IIC_write(F8X16[c*16+i+8]);
  }
  else if(Char_Size ==12){
    OLED_Set_Pos(x,y);	
    for(i=0;i<6;i++)
      OLED_IIC_write(F6X12[c*12+i]);
    OLED_Set_Pos(x,y+1);
    for(i=0;i<6;i++)
      OLED_IIC_write(F6X12[c*12+i+6]);
  }
  else {	
    OLED_Set_Pos(x,y);
    for(i=0;i<6;i++)
      OLED_IIC_write(F6x8[c][i]);
  }
}

/*********************************************************************************************
* 名称: OLED_ShowString()
* 功能: 显示一个字符串
* 参数: 起始坐标（x:0~127；y:0~63），chr字符串指针；Char_Size字符串长度
* 返回: 
* 修改:
* 注释:
*********************************************************************************************/  
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr,unsigned char Char_Size){
  unsigned char j=0;
  while (chr[j]!='\0'){	
    OLED_ShowChar(x,y,chr[j],Char_Size);
    x+=8;
    if(x>120){
      x=0;
      y+=2;
    }
    j++;
  }
}

///*********************************************************************************************
//* 名称: OLED_ShowCHinese()
//* 功能: 显示一个汉字
//* 参数: 起始坐标（x:0~127；y:0~63），num汉字在自定义字库中的编号（oledfont.h）编号
//* 返回: 
//* 修改:
//* 注释:
//*********************************************************************************************/ 
//void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char num){      			    
//  unsigned char t,adder=0;
//  OLED_Set_Pos(x,y);	
//  for(t=0;t<16;t++){
//    OLED_IIC_write(Hzk[2*num][t]);
//    adder+=1;
//  }	
//  OLED_Set_Pos(x,y+1);	
//  for(t=0;t<16;t++){	
//    OLED_IIC_write(Hzk[2*num+1][t]);
//    adder+=1;
//  }					
//}

void OLED_Fill(void)  {  
  unsigned char i,n;		    
  for(i=0;i<8;i++)  {  
    OLED_Write_command (0xb0+i);                                //设置页地址（0~7）
    OLED_Write_command (0x00);                                  //设置显示位置—列低地址
    OLED_Write_command (0x10);                                  //设置显示位置—列高地址   
    for(n=0;n<128;n++)
      OLED_IIC_write(0xff); 
  } 
}

/*********************************************************************************************
* 名称: OLED_Display_On()
* 功能: 开启OLED显示
* 参数: 
* 返回: 
* 修改:
* 注释:
*********************************************************************************************/
void oled_turn_on(void)
{
  char i=0;
  OLED_Clear();
  for(i = 0;i < 4;i++){
  OLED_DisFill(0,63-16*0-7,32*i+23,63-8*0,1);
  OLED_Refresh_Gram();
  }
  for(i = 0;i < 2;i++){
  OLED_DisFill(127-8*0-7,63-32*i-23,127-8*0,63,1);
  OLED_Refresh_Gram();
  }
  
  
  for(i = 0;i < 4;i++){
  OLED_DisFill(127-32*i-23,8*0,127,8*0+7,1);
  OLED_Refresh_Gram();
  }
  for(i = 0;i < 2;i++){
  OLED_DisFill(8*0,0,8*0+7,32*i+23,1);
  OLED_Refresh_Gram();
  }
  
//  for(i = 0;i < 4;i++){
//  OLED_DisFill(0,8*i,127,8*i+7,1);
//  OLED_DisFill(8*i,0,8*i+7,63,1);
//  OLED_DisFill(0,63-8*i-7,127,63-8*i,1);
//  OLED_DisFill(127-8*i-7,0,127-8*i,63,1);
//  OLED_Refresh_Gram();
//  }
}

/*********************************************************************************************
* 名称: oled_turn_off()
* 功能: 关闭OLED显示
* 参数: 
* 返回: 
* 修改:
* 注释:
*********************************************************************************************/  
void oled_turn_off(void)
{
  unsigned char i=0;
  OLED_Fill();
  memset(OLED_GRAM, 0xff, sizeof OLED_GRAM);
  for(i = 0;i < 4;i++){
  OLED_DisFill(0,8*i,127,8*i+7,0);
  OLED_DisFill(8*i,0,8*i+7,63,0);
  OLED_DisFill(0,63-8*i-7,127,63-8*i,0);
  OLED_DisFill(127-8*i-7,0,127-8*i,63,0);
  OLED_Refresh_Gram();
  }
  //OLED_Clear();
}
