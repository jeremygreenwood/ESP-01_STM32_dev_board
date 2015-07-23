#include "USART_Print.h"


/***************************************************************************//**
 * @brief  Print "Hello, World!" via USART1
 ******************************************************************************/
void USART_Print(void)
{
    SetupClock();
    SetupUSART();

    while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET ); // Wait for Empty
    USART_SendData( USART1, 'H' );
    while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET ); // Wait for Empty
    USART_SendData( USART1, 'i' );
    while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET ); // Wait for Empty
    USART_SendData( USART1, '\n' );
    while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET ); // Wait for Empty
    USART_SendData( USART1, '\r' );


    while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET ); // Wait for Empty
    /* Output a message on Hyperterminal using printf function */
    puts( "Hello, World!\n\r" );
}

/***************************************************************************//**
 * @brief Setup clocks
 ******************************************************************************/
void SetupClock()
{
      RCC_DeInit ();                    /* RCC system reset(for debug purpose)*/
      RCC_HSEConfig (RCC_HSE_ON);       /* Enable HSE                         */

      /* Wait till HSE is ready                                               */
      while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

//      RCC_HCLKConfig   (RCC_SYSCLK_Div1);   /* HCLK   = SYSCLK                */
      RCC_PCLK2Config  (RCC_HCLK_Div1);     /* PCLK2  = HCLK                  */
//      RCC_PCLK1Config  (RCC_HCLK_Div2);     /* PCLK1  = HCLK/2                */
//      RCC_ADCCLKConfig (RCC_PCLK2_Div4);    /* ADCCLK = PCLK2/4               */

      /* PLLCLK = 8MHz * 3 = 24 MHz                                           */
      RCC_PLLConfig (0x00010000, RCC_PLLMul_3);

      RCC_PLLCmd (ENABLE);                  /* Enable PLL                     */

      /* Wait till PLL is ready                                               */
      while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

      /* Select PLL as system clock source                                    */
      RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);

      /* Wait till PLL is used as system clock source                         */
      while (RCC_GetSYSCLKSource() != 0x08);

      /* Enable USART1 and GPIOA clock                                        */
      RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
}
/***************************************************************************//**
 * @brief Init USART1
 ******************************************************************************/
void SetupUSART()
{
      GPIO_InitTypeDef  GPIO_InitStructure;
      USART_InitTypeDef USART_InitStructure;

      /* Enable GPIOA clock                                                   */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

      /* Configure USART1 Rx (PA10) as input floating                         */
      GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
      GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
      GPIO_Init(GPIOA, &GPIO_InitStructure);

      /* Configure USART1 Tx (PA9) as alternate function push-pull            */
      GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
      GPIO_Init(GPIOA, &GPIO_InitStructure);

      /* USART1 configured as follow:
            - BaudRate = 115200 baud
            - Word Length = 8 Bits
            - One Stop Bit
            - No parity
            - Hardware flow control disabled (RTS and CTS signals)
            - Receive and transmit enabled
            - USART Clock disabled
            - USART CPOL: Clock is active low
            - USART CPHA: Data is captured on the middle
            - USART LastBit: The clock pulse of the last data bit is not output to
                             the SCLK pin
      */
      USART_InitStructure.USART_BaudRate            = 115200;
      USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
      USART_InitStructure.USART_StopBits            = USART_StopBits_1;
      USART_InitStructure.USART_Parity              = USART_Parity_No ;
      USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
      USART_Init(USART1, &USART_InitStructure);
      USART_Cmd(USART1, ENABLE);
}
