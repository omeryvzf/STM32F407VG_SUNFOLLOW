#include "stm32f4xx.h"   // DMA
#include "stm32f4xx_dma.h"
#include "stm32f4xx_adc.h"
void gpioConfig(void);
void dmaConfig(void);
void adcConfig(void);

uint16_t adcValue[2];	// Analog verilerinin tutalaca�� dizi
void gpioConfig(){		// GPIO ayarlar�
	GPIO_InitTypeDef	GPIOInitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	// GPIOA clock hatt� aktif edildi
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIOInitStructure.GPIO_Mode=GPIO_Mode_AN;	// GPIO Modu analog olarak se�ildi
	GPIOInitStructure.GPIO_OType=GPIO_OType_PP;	// otpye push pull se�ildi
	GPIOInitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3; // kullanaca��m�z pinleri belirledik
	GPIOInitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;	// devre kurumunda pull down diren� kulland�k
	GPIO_Init(GPIOA,&GPIOInitStructure);

	GPIOInitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_OType=GPIO_OType_PP;
	GPIOInitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIOInitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIOInitStructure);
}

void adcConfig(){
	ADC_InitTypeDef	ADCInitStructure;
	ADC_CommonInitTypeDef	ADCCommonInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);		// ADC2 clock hatt� aktif edildi

	ADCCommonInitStructure.ADC_Mode=ADC_Mode_Independent;   // ADC mod independent se�ildi
	ADCCommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div2;	// Div 2 se�ildi
	ADCCommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; // multi adc i�in
	ADCCommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

	ADC_CommonInit(&ADCCommonInitStructure);

	ADC_DeInit();

	ADCInitStructure.ADC_ContinuousConvMode=ENABLE;	// ENABLE ederek s�rekli veri okumay� sa�lad�k
	ADCInitStructure.ADC_DataAlign=ADC_DataAlign_Right;	// veriler sa�a yasl�
	ADCInitStructure.ADC_ExternalTrigConv = 0;
	ADCInitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;
	ADCInitStructure.ADC_NbrOfConversion=2; // DMA ile 2 veri tutacag�m�z i�in 2 se�tik
	ADCInitStructure.ADC_Resolution=ADC_Resolution_12b; // 12b lik maksimum okuyabilece�imiz de�er boyutu
	ADCInitStructure.ADC_ScanConvMode=ENABLE;
	ADC_Init(ADC2,&ADCInitStructure);

	ADC_RegularChannelConfig(ADC2,ADC_Channel_2,1,ADC_SampleTime_144Cycles);// pa2
	ADC_RegularChannelConfig(ADC2,ADC_Channel_3,2,ADC_SampleTime_144Cycles);//pa3
	ADC_DMARequestAfterLastTransferCmd(ADC2, ENABLE); // dma ile adc aras� al�sveris
	ADC_DMACmd(ADC2,ENABLE);
	ADC_Cmd(ADC2,ENABLE);
	ADC_SoftwareStartConv(ADC2);

}
void dmaConfig(){
	DMA_InitTypeDef	DMAInitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); // DMA clock hatt� aktif



	DMAInitStructure.DMA_BufferSize=2;  // 2 de�er oldugu i�in 2 se�tik
	DMAInitStructure.DMA_Channel=DMA_Channel_1;
	DMAInitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory;
	DMAInitStructure.DMA_FIFOMode=DMA_FIFOMode_Disable;
	DMAInitStructure.DMA_FIFOThreshold=DMA_FIFOThreshold_HalfFull;
	DMAInitStructure.DMA_Memory0BaseAddr=(uint32_t)&adcValue;
	DMAInitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMAInitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMAInitStructure.DMA_Mode=DMA_Mode_Circular;
	DMAInitStructure.DMA_PeripheralBaseAddr=(uint32_t) & ADC2 ->DR; // dr registerini adres olarak g�sterdik
	DMAInitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMAInitStructure.DMA_PeripheralInc=DMA_PeripheralDataSize_HalfWord;
	DMAInitStructure.DMA_Priority=DMA_Priority_High;
	DMAInitStructure.DMA_MemoryBurst=DMA_MemoryBurst_Single;
	DMAInitStructure.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;

	 DMA_Init(DMA2_Stream2, &DMAInitStructure);
	 DMA_Cmd(DMA2_Stream2, ENABLE);





}


int main(void){
	gpioConfig();
	adcConfig();
	dmaConfig();

  while (1){
	  if(adcValue[0]>4000 && adcValue[1]<4000){
		  GPIO_SetBits(GPIOB,GPIO_Pin_0);
		  GPIO_ResetBits(GPIOB,GPIO_Pin_1);

	  }
	  else if (adcValue[1]>4000 && adcValue[0]<4000 ){
		  GPIO_SetBits(GPIOB,GPIO_Pin_1);
		  GPIO_ResetBits(GPIOB,GPIO_Pin_0);


	  }
	  else{
		  GPIO_ResetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);

	  }


  }
}
