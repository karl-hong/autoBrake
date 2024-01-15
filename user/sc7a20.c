#include "sc7a20.h"
#include "HC89S003AF4.h"
#include "my_printf.h"
#include "common.h"


uint16_t sc7a20_odr_value[]={
1,3,5,10,20,40,100,1000,
};

uint8_t sc7a20_odr_mask[]={
	ODR1250,
	ODR400,
	ODR200,
	ODR100,
	ODR50,
	ODR25,
	ODR10,
	ODR1,
};

// struct sc7a20_data sc7a20_misc_data;
static uint16_t xyzBuf[3] = {0};
static bool_en gSc7a20Init;
static uchar gPollInterval;
static uchar gMinInterval;
static uchar gSensitivity;
static uchar gShift;
static uchar gCtrlReg1;
static uchar gRange;
static uchar gBdu;
static uchar gBle;
static uchar gHr;
static uint8_t  gResumeState[RESUME_ENTRIES];
static uint16_t gSensorData[3] = {0};
static uint8_t  acc_data[6];


void sc7a20_gpio_init(void)
{
    P0M2 = P0M2&0x0F|0x80;				//P05设置为推挽输出
    P0M3 = P0M3&0x0F|0x40;         //P07带SMT输入
    P2M0 = P2M0&0xF0|0x08;         //P20推挽输出
    P0M3 = P0M3&0xF0|0x08;		//P06设置为推挽输出
    SS_MAP = 0x05;
    MISO_MAP = 0x07;	             //SPI_MISO 映射到P07口
    MOSI_MAP = 0x20;	             //SPI_MOSI 映射到P20口
    SCK_MAP = 0x06;		             //SPI_SCK  映射到P06口
    SPDAT = 0x00;                  //数据寄存器写0
    SPSTAT = 0x00;                 //状态寄存器清0           
    SPCTL = 0xDF;//0xD7;                  //主机模式，时钟128分频
		//SPCTL = 0xD7;                  //主机模式，时钟128分频
    P0_5 = 0x01;
}

/**
  * @说明  	SPI写读函数
  *	@参数	  fuc_Data ：SPI读写数据
  * @返回值 SPDAT : SPI读取数据
  * @注		  无
  */
unsigned char SPI_WriteReadData(unsigned char fuc_Data)
{
	SPSTAT = 0xC0;
	SPDAT = fuc_Data;
	while(!(SPSTAT&0x80));
	SPSTAT = 0xC0;
	return SPDAT;
}

void delayUs(unsigned int fus)
{
	unsigned int fui_j;
	for (; fus > 0; fus--);
		for (fui_j = 2; fui_j > 0; fui_j--);
}

int sc7a20_spi_write(uint8_t regAddr, uint8_t *buf, uint16_t len)
{
  uint16_t length = 0;
  uint8_t addr = 0;
	P0_5 = 0;
  /* write addr */
  addr = regAddr & 0x3F;
  if(len > 1) addr |= SPI_AUTO_INCREMENT;
  SPI_WriteReadData(addr);
	
  /* write data */
  while(length < len && len > 0){
		delayUs(10);
    SPI_WriteReadData(buf[length]);
    length ++;
  } 
	
	P0_5 = 1;

  return 0;
}

int sc7a20_spi_read(uint8_t regAddr, uint8_t *buf, uint16_t len)
{
  uint16_t length = 0;
  uint8_t addr = 0;
	P0_5 = 0;
  /* write addr */
  addr = regAddr | SPI_READ_FLAG;
  if(len > 1) addr |= SPI_AUTO_INCREMENT;
  SPI_WriteReadData(addr);
 
  /* read data */
  while(length < len && len > 0){
		delayUs(10);
    buf[length] = SPI_WriteReadData(0xff);
    length ++;
  } 
  
	P0_5 = 1;

  return 0;
}

static int sc7a20_write_reg(uint8_t addr, uint8_t value)
{
		uint8_t writeValue = value;
    return sc7a20_spi_write(addr, &writeValue, 1);
}

static int sc7a20_read_reg(uint8_t addr, uint8_t *value)
{	
	uint8_t readValue = 0;
  sc7a20_spi_read(addr, &readValue, 1);
	*value = readValue;
	
	return 0;
}

static int sc7a20_update_g_range(uint8_t new_g_range)
{
	uint8_t sensitivity;
	uint8_t updated_val;
	uint8_t init_val;
	uint8_t new_val;
	uint8_t readValue;
	uint8_t mask = SC7A20_ACC_FS_MASK;

	switch (new_g_range) {
		case SC7A20_ACC_G_2G:
			sensitivity = 4;
			break;
		case SC7A20_ACC_G_4G:
			sensitivity = 3;
			break;
		case SC7A20_ACC_G_8G:
			sensitivity = 2;
			break;
		case SC7A20_ACC_G_16G:
			sensitivity = 1;
			break;		
		default:
			my_printf("invalid g range requested: %d\r\n", new_g_range);
			return -1;
	}
	/* read init value */
	sc7a20_read_reg(CTRL_REG4, &init_val);

	/* calc new value */
	new_val = new_g_range;
	updated_val = ((mask & new_val) | ((~mask) & init_val));

	/* write new value */
	sc7a20_write_reg(CTRL_REG4, updated_val);
	
	/* read out again */
	sc7a20_read_reg(CTRL_REG4, &readValue);
	
	/* check update result */
	if(updated_val != readValue){
		my_printf("update gsensor range fail\r\n");
		return -1;
	}
	   
	gSensitivity = sensitivity;

	my_printf("sensitivity %d g-range %d\r\n", sensitivity,new_g_range);

	return 0;
}

static int sc7a20_update_bdu(uint8_t new_bdu)
{
	uint8_t updated_val;
	uint8_t init_val;
	uint8_t new_val;
	uint8_t readValue;
	uint8_t mask = SC7A20_ACC_BDU_MASK;

	/* read init value */
	sc7a20_read_reg(CTRL_REG4, &init_val);
	
	/* calc new value */
	new_val = new_bdu;	
	updated_val = ((mask & new_val) | ((~mask) & init_val));
	
	/* write new value */
	sc7a20_write_reg(CTRL_REG4, updated_val);
	
	/* read out and check again */
	sc7a20_read_reg(CTRL_REG4, &readValue);
	
	if(updated_val != readValue){
		my_printf("update gsensor bdu fail!\r\n");
		return -1;
	}

	return 0;	
}

static int sc7a20_update_ble(uint8_t new_ble)
{
	uint8_t updated_val;
	uint8_t init_val;
	uint8_t new_val;
	uint8_t mask = SC7A20_ACC_BLE_MASK;

	/* read init value */
	sc7a20_read_reg(CTRL_REG4, &init_val);
	
	/* calc new value */
	new_val = new_ble;	
	updated_val = ((mask & new_val) | ((~mask) & init_val));
	
	/* write new value */
	sc7a20_write_reg(CTRL_REG4, updated_val);
	
	/* read out and check again */
	sc7a20_read_reg(CTRL_REG4, &gResumeState[RES_CTRL_REG4]);

	if(updated_val != gResumeState[RES_CTRL_REG4]){
		my_printf("update gsensor ble fail!\r\n");
		return -1;
	}

	return 0;
}

static int sc7a20_update_hr(uint8_t new_hr)
{
	uint8_t updated_val;
	uint8_t init_val;
	uint8_t new_val;
	uint8_t readValue;
	uint8_t mask = SC7A20_ACC_HR_MASK;

	/* read init value */
	sc7a20_read_reg(CTRL_REG4, &init_val);
	
	/* calc new value */
	new_val = new_hr;	
	updated_val = ((mask & new_val) | ((~mask) & init_val));
	
	/* write new value */
	sc7a20_write_reg(CTRL_REG4, updated_val);
	
	/* read out and check again */
	sc7a20_read_reg(CTRL_REG4, &readValue);
	
	if(updated_val != readValue){
		my_printf("update gsensor hr fail!\r\n");
		return -1;
	}
	
	return 0;	
}

static int sc7a20_update_odr(int poll_interval_ms)
{
	int err = -1;
	int i;
	uint8_t new_value;
	uint8_t readValue;
	uint16_t table_size =  sizeof(sc7a20_odr_value) / sizeof(uint16_t);

	for (i = table_size - 1; i >= 0; i--) {
		if (sc7a20_odr_value[i] <= poll_interval_ms)
			break;
	}

	new_value = sc7a20_odr_mask[i];

	new_value |= SC7A20_ENABLE_ALL_AXES;
	
	/* write new value */
	sc7a20_write_reg(CTRL_REG1, new_value);

	/* read out and check again */
	sc7a20_read_reg(CTRL_REG1, &readValue);
	
	if(new_value != readValue){
		my_printf("update gsensor odr fail!\r\n");
		return -1;
	}
	
	gCtrlReg1 = new_value;
	return 0;
}

static int sc7a20_update_reg4(uint8_t range, uint8_t bdu, uint8_t ble, uint8_t hr)
{
	uint8_t writeValue;
	uint8_t readValue;
	
	writeValue = range | bdu | ble | hr;
	
	/* write new value */
	sc7a20_write_reg(CTRL_REG4, writeValue);
	
	/* read out and check again */
	sc7a20_read_reg(CTRL_REG4, &readValue);
	
	if(writeValue != readValue){
		my_printf("update gsensor hr fail!\r\n");
		return -1;
	}
	
	return 0;	
}

static void sc7a20_set_interrupt(void)
{
	uint8_t writeValue;
	uint8_t readValue = 0;
	
	/* High Interrupt Triggler */
	writeValue = 0x00;
	sc7a20_write_reg(CTRL_REG6, writeValue);
	sc7a20_read_reg(CTRL_REG6, &readValue);
	if(readValue != writeValue){
		my_printf("gsensor set interrupt triggler fail!\r\n");
	}
	
	/* Interrup thresold */
	writeValue = 0x50;//0x40;//0x60;//0x50;
	sc7a20_write_reg(CLICK_THS, writeValue);
	sc7a20_read_reg(CLICK_THS, &readValue);
	if(readValue != writeValue){
		my_printf("gsensor set interrupt triggler fail!\r\n");
	}
	
	/* Time limit */
	writeValue = 0x7f;
	sc7a20_write_reg(TIME_LIMIT, writeValue);
	sc7a20_read_reg(TIME_LIMIT, &readValue);
	if(readValue != writeValue){
		my_printf("gsensor set TIME_LIMIT fail!\r\n");
	}
	
	/* Time latency */
	writeValue = 0x05;
	sc7a20_write_reg(TIME_LATENCY, writeValue);
	sc7a20_read_reg(TIME_LATENCY, &readValue);
	if(readValue != writeValue){
		my_printf("gsensor set TIME_LATENCY fail!\r\n");
	}
	
	/* x/y/z Interrupt Detect Enable */
	writeValue = 0x15;
	sc7a20_write_reg(CLICK_CFG, writeValue);
	sc7a20_read_reg(CLICK_CFG, &readValue);
	if(readValue != writeValue){
		my_printf("gsensor set interrupt detect enable fail!\r\n");
	}
	
	/* enable x/y/z direction interrupt, low event */
	writeValue = 0x15;
	sc7a20_write_reg(INT1_CFG, writeValue); 
	sc7a20_read_reg(INT1_CFG, &readValue);
	if(readValue != writeValue){
		my_printf("gsensor enable direction interrupt event fail!\r\n");
	}
	
	
	/* Enable Interrupt */
	writeValue = 0x80;
	sc7a20_write_reg(CTRL_REG3, writeValue);
	sc7a20_read_reg(CTRL_REG3, &readValue);
	if(readValue != writeValue){
		my_printf("gsensor enable interrupt fail!\r\n");
	}
}

void sc7a20_init(void)
{
	uint8_t chipId = 0;
	int rc;
	
	sc7a20_read_reg(WHO_AM_I, &chipId);
	if(SC7A20_CHIP_ID != chipId){
		my_printf("sc7a20 wrong chip id!\r\n");
		return;
	}
	
	gResumeState[RES_CTRL_REG1] = SC7A20_ENABLE_ALL_AXES;
	gResumeState[RES_CTRL_REG2] = 0x00;
	gResumeState[RES_CTRL_REG3] = 0x00;
	gResumeState[RES_CTRL_REG4] = 0x00;
	gResumeState[RES_CTRL_REG5] = 0x00;
	gResumeState[RES_CTRL_REG6] = 0x00;
	
//	gRange     = SC7A20_ACC_G_2G;
//	gBdu			 = SC7A20_ACC_BDU_MASK;
//	gBle			 = 0x00;//SC7A20_ACC_BLE_MASK;
//	gHr 			 = SC7A20_ACC_HR_MASK;
//	rc = sc7a20_update_reg4(gRange, gBdu, gBle, gHr);
//	if(0 > rc){
//		my_printf("update reg4 fail!\r\n");
//		goto error1;
//	}
	
	gRange     = SC7A20_ACC_G_2G;
	gSensitivity = SENSITIVITY_2G;
	/* set g range */
	rc = sc7a20_update_g_range(gRange);
	if(0 > rc){
		my_printf("update_g_range failed\r\n");
		goto error1;
	}
	
	/* set bdu */
	gBdu = 0x80;
	rc = sc7a20_update_bdu(gBdu);
	if(0 > rc){
		my_printf("update_bdu failed\r\n");
		goto error1;
	}
	
	/* set ble */
	gBle = 0x00;
	rc = sc7a20_update_ble(gBle);
	if(0 > rc){
		my_printf("update_ble failed\r\n");
		goto error1;
	}
	
	/* set hr */
	gHr = 0x08;
	rc = sc7a20_update_hr(gHr);
	if(0 > rc){
		my_printf("update_hr failed\r\n");
		goto error1;
	}
	
	/* set odr */
	gMinInterval  = 20;
	gPollInterval = 20;
	rc = sc7a20_update_odr(gPollInterval);
	if(0 > rc){
		my_printf("update_odr failed\r\n");
		goto error1;
	}
	
	sc7a20_set_interrupt();
	
	gSc7a20Init = 1;
	my_printf("sc7a20 hw init done!\r\n");
	
	return;
error1:
	gSc7a20Init = 0;
	my_printf("sc7a20 init failed\r\n");
	return;
}

static int sc7a20_get_acceleration_data(uint16_t *xyz)
{
	static uint16_t errCnt = 0;
	
	int err = -1;
	/* Data bytes from hardware xL, xH, yL, yH, zL, zH */
	int16_t hw_d[3] = { 0 };
	uint8_t readValue = 0;
	
	/* test read api */
	sc7a20_read_reg(WHO_AM_I, &readValue);
	if(SC7A20_CHIP_ID != readValue){
		my_printf("sc7a20 wrong chip id!\r\n");
		return -1;
	}
	
	/* check status reg */
	sc7a20_read_reg(STATUS_REG, &readValue);
	if((readValue & 0x0f) != 0x0f){
		goto error;
	}
			
	/* read XOUT_L reg */
	sc7a20_read_reg(XOUT_L, &readValue);
	/* get data */
	acc_data[0] = readValue;
	
	/* read XOUT_H reg */
	sc7a20_read_reg(XOUT_H, &readValue);
	/* get data */
	acc_data[1] = readValue;	

	/* read YOUT_L reg */
	sc7a20_read_reg(YOUT_L, &readValue);
	/* get data */
	acc_data[2] = readValue;

	/* read YOUT_H reg */
	sc7a20_read_reg(YOUT_H, &readValue);
	/* get data */
	acc_data[3] = readValue;

	/* read ZOUT_L reg */
	sc7a20_read_reg(ZOUT_L, &readValue);
	/* get data */
	acc_data[4] = readValue;

	/* read ZOUT_H  reg */
	sc7a20_read_reg(ZOUT_H, &readValue);
	/* get data */
	acc_data[5] = readValue;

	/* get all data */
	hw_d[0] =  ((acc_data[1] << 8) & 0xff00) | acc_data[0];
	hw_d[1] =  ((acc_data[3] << 8) & 0xff00) | acc_data[2];
	hw_d[2] =  ((acc_data[5] << 8) & 0xff00) | acc_data[4];

	hw_d[0] = ((int16_t) hw_d[0] ) >> gSensitivity;
	hw_d[1] = ((int16_t) hw_d[1] ) >> gSensitivity;
	hw_d[2] = ((int16_t) hw_d[2] ) >> gSensitivity;

	xyz[0] = hw_d[0];
	xyz[1] = hw_d[1];
	xyz[2] = hw_d[2];

	xyzBuf[0] = xyz[0];
	xyzBuf[1] = xyz[1];
	xyzBuf[2] = xyz[2];
	
	errCnt = 0;

	return 0;
	
error:
	errCnt ++;
	if(errCnt >= 6000){
		errCnt = 0;
		/* reinit */
		sc7a20_init();
	}
	
	return -1;
}

void sc7a20_interrupt_handle(void)
{
	//Sensor.gSensorState = 1;
	//printf("sc7a20!!!\r\n");
}


void sc7a20_task(void)
{
	static uint8_t stTaskCnt = 0;
	if(false == gMainContext.mSc7a20Delay){
		return;
	}
	gMainContext.mSc7a20Delay = false;
	
	stTaskCnt ++;
	if(stTaskCnt < 10){
		return;
	}
	stTaskCnt = 0;
	
	if(0 == gSc7a20Init){
		sc7a20_init();
		return;
	}
	
	if(-1 == sc7a20_get_acceleration_data(gSensorData)){
		return;
	}

	my_printf("x: %d, y: %d, z: %d\r\n", gSensorData[0], gSensorData[1], gSensorData[2]);
}














