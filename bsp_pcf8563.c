#include "bsp_pcf8563.h"

/*
需要移植的代码：
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//PCF8563 IIC从机地址(8位)
#define BSP_PCF8563_ADDR 0xA2

//接口函数-PCF8563 IIC初始化
void BSP_PCF8563_Init_Port(void)
{
	HAL_IIC1_Init();
}

//接口函数-PCF8563 IIC失能
void BSP_PCF8563_Default_Port(void)
{
	HAL_IIC1_Default();
}

//接口函数-PCF8563 IIC单字节写入函数
#define BSP_PCF8563_SingleWrite_Port(REG_Address,SlaveAddress,REG_data)   HAL_IIC1_SingleWrite(REG_Address,SlaveAddress,REG_data)

//接口函数-PCF8563 IIC单字节读取函数
#define BSP_PCF8563_SingleRead_Port(REG_Address,SlaveAddress,recv_buf)    HAL_IIC1_SingleRead(REG_Address,SlaveAddress,recv_buf)

//接口函数-PCF8563 IIC多字节写入函数
#define BSP_PCF8563_MultipleWrite_Port(star_addr,num,SlaveAddress,send_buf)    HAL_IIC1_MultipleWrite(star_addr,num,SlaveAddress,send_buf)

//接口函数-PCF8563 IIC多字节读取函数
#define BSP_PCF8563_MultipleRead_Port(star_addr,num,SlaveAddress,recv_buf)   HAL_IIC1_MultipleRead(star_addr,num,SlaveAddress,recv_buf)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const u8 ToBCD[100] = 
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,
};
#define FromBCD(X) (((X)>>4)*10+((X)&0x0f))


/*
 ************************************************************
 *  名称：	Get_Week()
 *  功能：	通过年月日获取星期
 *	输入：  year-年  month-月  day-日
 *	输出：  星期1-7
 ************************************************************
*/
u8 Get_Week(u16 year,u8 month,u8 day)
{
  u8 week;
  if (month == 1 || month == 2)
  {
    month+=12;
    year--;
  }
  week =(day+2*month+3*(month+1)/5+year+year/4-year/100+year/400)%7;
  week++;
  return week;
}


/*
 ************************************************************
 *  名称：	BSP_PCF8563_Check()
 *  功能：	检测PCF8563是否正常
 *	输入：  无
 *	输出：  true-读取成功  false-读取失败
 ************************************************************
*/
bool BSP_PCF8563_Check(void)
{
  u8 rec_data;
	
  if(false == BSP_PCF8563_SingleWrite_Port(0x00,BSP_PCF8563_ADDR,0x00)) return false;
  if(false == BSP_PCF8563_SingleRead_Port(0x00,BSP_PCF8563_ADDR,&rec_data)) return false;
	
  if(rec_data == 0x00) return true;
  else return false; 
}

/*
 ************************************************************
 *  名称：	BSP_PCF8563_ReadTimes()
 *  功能：	PCF8563 设定时间
 *	输入：  rtc-读取出的时间
						rtc[0]-秒
						rtc[1]-分
						rtc[2]-时
						rtc[3]-日
						rtc[4]-星期
						rtc[5]-月
						rtc[6]-年
 *	输出：  true-读取成功  false-读取失败
 ************************************************************
*/
bool BSP_PCF8563_ReadTimes(u8 *rtc)
{
  u8 read_buf[7];
  
  if(false == BSP_PCF8563_MultipleRead_Port(0x02,7,BSP_PCF8563_ADDR,read_buf)) return false;
	
  rtc[0] = FromBCD(read_buf[0]&0x7F);	//秒
  rtc[1] = FromBCD(read_buf[1]&0x7F);	//分
  rtc[2] = FromBCD(read_buf[2]&0x3F);	//时
  rtc[3] = FromBCD(read_buf[3]&0x3F);	//日
	
	if(read_buf[4] == 0) rtc[4] = 7;	//星期
  else rtc[4] = read_buf[4];
	
  rtc[5] = FromBCD(read_buf[5]&0x1F);	//月
  rtc[6] = FromBCD(read_buf[6]);	//年
  
  return true;
}

/*
 ************************************************************
 *  名称：	BSP_PCF8563_SetTimes()
 *  功能：	PCF8563 设定时间
 *	输入：  year-年  month-月  day-日  hour-时  min-分  sec-秒
 *	输出：  true-设定成功  false-设定失败
 ************************************************************
*/
bool BSP_PCF8563_SetTimes(u8 year,u8 month,u8 day,u8 hour,u8 min,u8 sec)
{
  u8 write_buf[7];
	
  write_buf[0] = ToBCD[sec];
  write_buf[1] = ToBCD[min];
  write_buf[2] = ToBCD[hour];
  write_buf[3] = ToBCD[day];
	
	write_buf[4] = Get_Week(year+2000,month,day);	//星期
  if(write_buf[4] == 7) write_buf[4] = 0;	
	
  write_buf[5] = ToBCD[month];
  write_buf[6] = ToBCD[year];
	
  if(false == BSP_PCF8563_MultipleWrite_Port(0x02,7,BSP_PCF8563_ADDR,write_buf)) return false;
  else return true;
}




