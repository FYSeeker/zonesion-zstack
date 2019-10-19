/*********************************************************************************************
* �ļ���oled.c
* ���ߣ�zonesion
* ˵����
* �޸ģ�Chenkm 2017.01.05 ������ע��
* ע�ͣ�
*********************************************************************************************/
/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "iic.h"
#include "oled.h"
#include "oledfont.h"

unsigned char OLED_GRAM[96][4];

unsigned char OLED_PEN_COLOR=WHITE;


/*********************************************************************************************
* ����: OLED_Init()
* ����: OLED��ʼ��
* ����: ��
* ����: ��
* ע�ͣ�
* �޸�:
*********************************************************************************************/
void  OLED_Init(void)
{
    iic_init();
    OLED_Write_command(0xAE); /*display off*/
    OLED_Write_command(0x00); /*set lower column address*/
    OLED_Write_command(0x10); /*set higher column address*/
    OLED_Write_command(0x40); /*set display start line*/
    OLED_Write_command(0xb0); /*set page address*/
    OLED_Write_command(0x81); /*contract control*/
    OLED_Write_command(0x45); /*128*/
    OLED_Write_command(0xA1); /*set segment remap*/
    OLED_Write_command(0xC0);/*Com scan direction 0XC0 */
    OLED_Write_command(0xA6); /*normal / reverse*/
    OLED_Write_command(0xA8); /*multiplex ratio*/
    OLED_Write_command(0x1F); /*duty = 1/32*/
    OLED_Write_command(0xD3); /*set display offset*/
    OLED_Write_command(0x00);
    OLED_Write_command(0xD5); /*set osc division*/
    OLED_Write_command(0x80);
    OLED_Write_command(0xD9); /*set pre-charge period*/
    OLED_Write_command(0x22);
    OLED_Write_command(0xDA); /*set COM pins*/
    OLED_Write_command(0x12);
    OLED_Write_command(0xdb); /*set vcomh*/
    OLED_Write_command(0x20);
    OLED_Write_command(0x8d); /*set charge pump enable*/
    OLED_Write_command(0x14);
    OLED_Write_command(0xAF); /*display ON*/

    OLED_Clear();
}


/*********************************************************************************************
* ����: OLED_Write_command()
* ����: IIC Write Command
* ����: ����
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_Write_command(unsigned char IIC_Command)
{
    iic_start();						        //��������
    iic_write_byte(ADDR_W);		                        //��ַ����
    iic_write_byte(0x00);			                        //��������
    iic_write_byte(IIC_Command);	                                //�ȴ����ݴ������
    iic_stop();
}

/*********************************************************************************************
* ����: OLED_IIC_write()
* ����: IIC Write Data
* ����: ����
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_IIC_write(unsigned char IIC_Data)
{
    if(OLED_PEN_COLOR==BLACK)
    {
        IIC_Data = ~IIC_Data;
    }
    iic_start();                                                //��������
    iic_write_byte(ADDR_W);                                     //��ַ����
    iic_write_byte(0x40);                                       //��������
    iic_write_byte(IIC_Data);	                                //�ȴ����ݴ������
    iic_stop();
}

/*********************************************************************************************
* ����: OLED_Display_On()
* ����: ����OLED��ʾ
* ����:
* ����:
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_Display_On(void)
{
    OLED_Write_command(0X8D);                                     //SET DCDC����
    OLED_Write_command(0X14);                                     //DCDC ON
    OLED_Write_command(0XAF);                                     //DISPLAY ON
}

/*********************************************************************************************
* ����: OLED_Display_Off()
* ����: �ر�OLED��ʾ
* ����:
* ����:
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_Display_Off(void)
{
    OLED_Write_command(0X8D);                                     //SET DCDC����
    OLED_Write_command(0X10);                                     //DCDC OFF
    OLED_Write_command(0XAE);                                     //DISPLAY OFF
}


/*********************************************************************************************
* ����: OLED_Set_Pos()
* ����: ��������
* ����: ����
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_Write_command(0xb0+y);
    OLED_Write_command(((x&0xf0)>>4)|0x10);
    OLED_Write_command((x&0x0f));
}


/*********************************************************************************************
* ����: OLED_Clear()
* ����: ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
* ����:
* ����:
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_Clear(void)
{
    unsigned char i,n;

    for(i=0; i<OLED_HEIGHT; i++)
    {
        OLED_Write_command (0xb0+i);                                //����ҳ��ַ��0~7��
        OLED_Write_command (0x00);                                  //������ʾλ�á��е͵�ַ
        OLED_Write_command (0x10);                                  //������ʾλ�á��иߵ�ַ
        for(n=0; n<OLED_WIDTH; n++)
            OLED_IIC_write(0);
    }
}


/*********************************************************************************************
* ����: oled_areaClear()
* ����: �������
* ����: ����
* ����:
* �޸�:
* ע��:
*********************************************************************************************/
void oled_areaClear(int Hstart,int Hend,int Lstart,int Lend)
{
    unsigned int x,y;

    for(y=Hstart; y<=Hend; y++)
    {
        OLED_Set_Pos(Lstart,y);
        for(x=Lstart; x<=Lend; x++)
        {
            OLED_IIC_write(0);
        }
    }
}

/*********************************************************************************************
* ����: OLED_Fill
* ����: ��亯��
* ����: data����������
* ����:
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_Fill(unsigned char data)
{
    unsigned char x,y;
    for(y=0; y<OLED_HEIGHT; y++)
    {
        OLED_Set_Pos(x,y+1);
        for(x=0; x<OLED_WIDTH; x++)
            OLED_IIC_write(data);
    }
}


/*********************************************************************************************
* ����: OLED_ShowChar()
* ����: ��ָ��λ����ʾһ���ַ�,���������ַ�
* ����: ���꣨x:0~95��y:0~3����chr�ַ���Char_Size�ַ���С
* ����:
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size)
{
    unsigned char c=0,i=0;

    c=chr-' ';//�õ�ƫ�ƺ��ֵ

    if(Char_Size ==16)
    {
        OLED_Set_Pos(x,y);
        for(i=0; i<8; i++)
            OLED_IIC_write(F8X16[c*16+i]);
        OLED_Set_Pos(x,y+1);
        for(i=0; i<8; i++)
            OLED_IIC_write(F8X16[c*16+i+8]);
    }
    else
    {
        OLED_Set_Pos(x,y);
        for(i=0; i<6; i++)
            OLED_IIC_write(F6x8[c][i]);
    }
}

/*********************************************************************************************
* ����: OLED_ShowString()
* ����: ��ʾһ���ַ���
* ����: ��ʼ���꣨x:0~95��y:0~3����chr�ַ���ָ�룻Char_Size�����С
* ����:
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr,unsigned char Char_Size)
{
    unsigned char i=0;

    while(chr[i]!='\0')
    {
        OLED_ShowChar(x,y,chr[i],Char_Size);
        i++;
        if(Char_Size==16)
        {
            x+=8;
            if(x>OLED_WIDTH-1)
            {
                x=0;
                y+=2;
            }
        }
        else
        {
            x+=6;
            if(x>OLED_WIDTH-1)
            {
                x=0;
                y+=1;
            }
        }
    }
}

/*********************************************************************************************
* ����: OLED_ShowCHinese()
* ����: ��ʾһ������
* ����: ��ʼ���꣨x:0~127��y:0~63����num�������Զ����ֿ��еı�ţ�oledfont.h�����
* ����:
* �޸�:
* ע��:
*********************************************************************************************/
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char num)
{
    unsigned char t,adder=0;
    OLED_Set_Pos(x,y);
    for(t=0; t<12; t++)
    {
        OLED_IIC_write(Hzk[2*num][t]);
        adder+=1;
    }
    OLED_Set_Pos(x,y+1);
    for(t=0; t<12; t++)
    {
        OLED_IIC_write(Hzk[2*num+1][t]);
        adder+=1;
    }
}




//����
//x:0~127
//y:0~63
//t:1 ��� 0,���
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t)
{
    unsigned char pos,bx,temp=0;

    if(x>95||y>31)return;//������Χ��.
    pos=7-y/8;
    bx=y%8;
    temp=1<<(7-bx);
    if(t)
        OLED_GRAM[x][pos]|=temp;
    else
        OLED_GRAM[x][pos]&=~temp;
}


//�����Դ浽LCD
void OLED_Refresh_Gram(void)
{
    unsigned char i,n;
    for(i=0; i<4; i++)
    {
        OLED_Write_command (0xb0+i);                                //����ҳ��ַ��0~3��
        OLED_Write_command (0x00);                                  //������ʾλ�á��е͵�ַ
        OLED_Write_command (0x10);                                  //������ʾλ�á��иߵ�ַ
        for(n=0; n<96; n++)
            OLED_IIC_write(OLED_GRAM[n][i]);
    }
}

