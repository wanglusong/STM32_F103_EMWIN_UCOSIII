#include "max6675.h"
#include "usart.h"
#include "string.h"
		
uint16_t mytemp = 0;

AVERAGE_FILTER_T max6675_result_t;

void MAX6675_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;	
	
	/* ʹ�� SPI1 ʱ�� */                         
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	/* ---------ͨ��I/O��ʼ��----------------
	 * PA5-SPI1-SCK :MAX6675_SCK
	 * PA6-SPI1-MISO:MAX6675_SO
	 * PA7-SPI1-MOSI:MAX6675_SI	 
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// �������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ---------����I/O��ʼ��----------------*/
	/* PA4-SPI1-NSS:MAX6675_CS */							// Ƭѡ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// �������
	GPIO_Init(GPIOA, &GPIO_InitStructure);						  
	GPIO_SetBits(GPIOA, GPIO_Pin_4);						// �Ȱ�Ƭѡ���ߣ������õ�ʱ��������
 

	/* SPI1 ���� */ 
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
	
	
	/* ʹ�� SPI1  */
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
		
		i = i|((unsigned int)c);			//i�Ƕ�������ԭʼ����
		flag = i&0x04;						//flag�������ȵ�ż������״̬
		temp = i<<1;
		//printf("t<<1 is %d\r\n",temp);
		temp = temp>>4;
		//printf("t>>4 is %d\r\n",temp);
	
		mytemp = temp ;//* 0.25;
	
		//printf("mytemp is =%d\r\n",mytemp);
		if(i!=0)							//max6675�����ݷ���
		{
			if(flag==0)						//�ȵ�ż������
			{
				//printf("ԭʼ�����ǣ�%04X,��ǰ�¶��ǣ�%d\r\n",i,mytemp);
			}	
			else							//�ȵ�ż����
			{
				printf("δ��⵽�ȵ�ż�����顣\r\n");
			}
		
		}
		else								//max6675û�����ݷ���
		{
			printf("max6675û�����ݷ��أ�����max6675���ӡ�\r\n");
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
    Input->u16AdcMin = 1024;  //�ɸ���ʵ����������ֵ

    Input->u16AdcResultAve = Input->u16AdcSum / (Input->u8CntBase * 2);
    Input->u16AdcSum = 0;

    //���һ��
    return (1);
  }
  return (0);
}
