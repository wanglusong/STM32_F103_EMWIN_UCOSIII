#include "max6675.h"
#include "usart.h"
#include "string.h"
		
uint16_t mytemp = 0;

AVERAGE_FILTER_T max6675_result_t;

void MAX6675_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;	
	
	/* 使能 SPI1 时钟 */                         
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	/* ---------通信I/O初始化----------------
	 * PA5-SPI1-SCK :MAX6675_SCK
	 * PA6-SPI1-MISO:MAX6675_SO
	 * PA7-SPI1-MOSI:MAX6675_SI	 
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// 复用输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ---------控制I/O初始化----------------*/
	/* PA4-SPI1-NSS:MAX6675_CS */							// 片选
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// 推免输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);						  
	GPIO_SetBits(GPIOA, GPIO_Pin_4);						// 先把片选拉高，真正用的时候再拉低
 

	/* SPI1 配置 */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	
	/* 使能 SPI1  */
	SPI_Cmd(SPI1, ENABLE); 
	
	memset(&max6675_result_t, 0, sizeof(max6675_result_t));
	
	max6675_result_t.u8CntBase = 2;
	max6675_result_t.u8TotalCnt = 6;
}

unsigned char MAX6675_ReadByte(void)
{
	
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, 0xff);
	
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}

uint16_t MAX6675_Read_data(void)
{
		unsigned int temp = 0,i;
		unsigned char c;
		unsigned char flag;
	
		MAX6675_CSL();
		c = MAX6675_ReadByte();
		i = c;
		i = i<<8;
		c = MAX6675_ReadByte();
		MAX6675_CSH();
		
		i = i|((unsigned int)c);			//i是读出来的原始数据
		flag = i&0x04;						//flag保存了热电偶的连接状态
		temp = i<<1;
		//printf("t<<1 is %d\r\n",temp);
		temp = temp>>4;
		//printf("t>>4 is %d\r\n",temp);
	
		mytemp = temp ;//* 0.25;
	
		//printf("mytemp is =%d\r\n",mytemp);
		if(i!=0)							//max6675有数据返回
		{
			if(flag==0)						//热电偶已连接
			{
				//printf("原始数据是：%04X,当前温度是：%d\r\n",i,mytemp);
			}	
			else							//热电偶掉线
			{
				printf("未检测到热电偶，请检查。\r\n");
			}
		
		}
		else								//max6675没有数据返回
		{
			printf("max6675没有数据返回，请检查max6675连接。\r\n");
		}
		
		return mytemp;
}

float MAX6675_Read_data_filter(AVERAGE_FILTER_T *Input, uint8_t AdChannelValue)
{
  Input->u16AdcResult = AdChannelValue;

  Input->u16AdcSum += Input->u16AdcResult;  

  if (Input->u16AdcResult <= Input->u16AdcMin)
  {
    Input->u16AdcMin = Input->u16AdcResult;
  }
  if (Input->u16AdcResult >= Input->u16AdcMax)
  {
    Input->u16AdcMax = Input->u16AdcResult;
  }

  Input->u8Cnt++;
  if (Input->u8Cnt >= Input->u8TotalCnt)
  {
    Input->u8Cnt = 0;

    Input->u16AdcSum -= Input->u16AdcMax;
    Input->u16AdcSum -= Input->u16AdcMin;
    Input->u16AdcMax = 0;
    Input->u16AdcMin = 1024;  //可根据实际情况定义该值

    Input->u16AdcResultAve = Input->u16AdcSum / (Input->u8CntBase * 2);
    Input->u16AdcSum = 0;

    //查表一次
    return (1);
  }
  return (0);
}
