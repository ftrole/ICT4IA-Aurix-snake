/**********************************************************************************************************************
 * \file ADC_Queued_Scan.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 *
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are solely in the form of
 * machine-executable object code generated by a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *********************************************************************************************************************/

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "ADC_Queued_Scan.h"
#include "IfxAsclin_Asc.h"
/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define GROUPID_8           IfxEvadc_GroupId_8                  /* EVADC group                                      */

#define CHANNELS_NUM        3                                   /* Number of used channels                          */
#define AN39_CHID           7                                   /* Channel ID for pin AN39                          */
#define AN38_CHID           6                                   /* Channel ID for pin AN38                          */
#define AN37_CHID           5                                   /* Channel ID for pin AN37                          */

#define ISR_PRIORITY_ASCLIN_TX          1                               /* Priority of the interrupt ISR Transmit   */
#define ISR_PRIORITY_ASCLIN_RX          2                               /* Priority of the interrupt ISR Receive    */
#define ISR_PRIORITY_ASCLIN_ER          3                               /* Priority of the interrupt ISR Errors     */
#define ASC_TX_BUFFER_SIZE              64
#define ASC_RX_BUFFER_SIZE              64
#define ASC_PRESCALER                   1
#define ASC_BAUDRATE                    115200

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
 /* EVADC handle */
IfxEvadc_Adc         g_evadc;                                   /* EVADC module handle variable                     */
IfxEvadc_Adc_Group   g_adcGroup;                                /* EVADC group handle variable                      */
IfxEvadc_Adc_Channel g_adcChannel[CHANNELS_NUM];                /* EVADC channels handle array                      */

uint8 g_grp8channels[CHANNELS_NUM] = {AN39_CHID, AN38_CHID, AN37_CHID}; /* AN39, AN38, AN37 channel IDs array       */

Ifx_EVADC_G_RES g_results[CHANNELS_NUM];                        /* Array of results                                 */

/* Variable for Standard interface */
IfxAsclin_Asc g_asc;
IfxStdIf_DPipe g_stdInterface;

/* The transfer buffers allocate memory for the data itself and for the FIFO runtime variables.
 * 8 more bytes have to be added to ensure a proper circular buffer handling independent from
 * the address to which the buffers have been located.
 */
uint8 g_AscTxBuffer[ASC_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
uint8 g_AscRxBuffer[ASC_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void initEVADCModule(void);                                     /* Function to initialize the EVADC module          */
void initEVADCGroup(void);                                      /* Function to initialize the EVADC group           */
void initEVADCChannels(void);                                   /* Function to initialize the EVADC channels        */
void fillAndStartQueue(void);                             /* Function to add each channel to the queue and start it */

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Function to initialize the EVADC with default parameters */
void initEVADC()
{
    initEVADCModule();      /* Initialize the EVADC module  */
    initEVADCGroup();       /* Initialize the EVADC group   */
    initEVADCChannels();    /* Initialize the channels      */
    fillAndStartQueue();    /* Fill the queue and start it  */
}

/* Function to initialize the EVADC module with default parameters */
void initEVADCModule()
{
    /* Create configuration */
    IfxEvadc_Adc_Config adcConfig;
    IfxEvadc_Adc_initModuleConfig(&adcConfig, &MODULE_EVADC);

    /* Initialize module */
    IfxEvadc_Adc_initModule(&g_evadc, &adcConfig);
}

/* Function to initialize the EVADC group with default parameters */
void initEVADCGroup()
{
    /* Create and initialize group configuration with default values */
    IfxEvadc_Adc_GroupConfig adcGroupConfig;
    IfxEvadc_Adc_initGroupConfig(&adcGroupConfig, &g_evadc);

    /* Setting user configuration using group 2 */
    adcGroupConfig.groupId = GROUPID_8;
    adcGroupConfig.master = GROUPID_8;

    /* Enable queued source */
    adcGroupConfig.arbiter.requestSlotQueue0Enabled = TRUE;

    /* Enable all gates in "always" mode (no edge detection) */
    adcGroupConfig.queueRequest[0].triggerConfig.gatingMode = IfxEvadc_GatingMode_always;

    /* Initialize the group */
    IfxEvadc_Adc_initGroup(&g_adcGroup, &adcGroupConfig);
}

void initEVADCChannels()
{
    /* Create channel configuration */
    IfxEvadc_Adc_ChannelConfig adcChannelConfig[CHANNELS_NUM];

    for(uint16 idx = 0; idx < CHANNELS_NUM; idx++)
    {
        /* Initialize the configuration with default values */
        IfxEvadc_Adc_initChannelConfig(&adcChannelConfig[idx], &g_adcGroup);

        /* Select the channel ID and the respective result register */
        adcChannelConfig[idx].channelId = (IfxEvadc_ChannelId)(g_grp8channels[idx]);
        adcChannelConfig[idx].resultRegister = (IfxEvadc_ChannelResult)(g_grp8channels[idx]);

        /* Initialize the channel */
        IfxEvadc_Adc_initChannel(&g_adcChannel[idx], &adcChannelConfig[idx]);
    }
}

void fillAndStartQueue()
{
    for(uint16 idx = 0; idx < CHANNELS_NUM; idx++)
    {
        /* Add channel to queue with refill option enabled */
        IfxEvadc_Adc_addToQueue(&g_adcChannel[idx], IfxEvadc_RequestSource_queue0, IFXEVADC_QUEUE_REFILL);
    }

    /* Start the queue */
    IfxEvadc_Adc_startQueue(&g_adcGroup, IfxEvadc_RequestSource_queue0);
}

/* Function to read the EVADC used channel */
void readEVADC()
{
    for(uint8 i = 0; i < CHANNELS_NUM; i++)
    {
        /* Wait for a valid result */
        Ifx_EVADC_G_RES conversionResult;
        do
        {
            conversionResult = IfxEvadc_Adc_getResult(&g_adcChannel[i]); /* Read the result of the selected channel */
        } while(!conversionResult.B.VF);

        /* Store result */
        g_results[i] = conversionResult;

        IfxStdIf_DPipe_print(&g_stdInterface, "ADC Sample: %d\r\n", conversionResult.B.RESULT);

    }
        IfxStdIf_DPipe_print(&g_stdInterface, "\r\n");
        //wait(IfxStm_getTicksFromMilliseconds(&MODULE_STM0, 1000));
}
/* ASCLIN TX Interrupt Service Routine */
IFX_INTERRUPT(ISR_ASCLIN_Tx, 0, ISR_PRIORITY_ASCLIN_TX);

void ISR_ASCLIN_Tx(void)
{
    IfxStdIf_DPipe_onTransmit(&g_stdInterface);
}

/* ASCLIN RX Interrupt Service Routine */
IFX_INTERRUPT(ISR_ASCLIN_Rx, 0, ISR_PRIORITY_ASCLIN_RX);

void ISR_ASCLIN_Rx(void)
{
    IfxStdIf_DPipe_onReceive(&g_stdInterface);
}

/* ASCLIN Error Interrupt Service Routine */
IFX_INTERRUPT(ISR_ASCLIN_Er, 0, ISR_PRIORITY_ASCLIN_ER);

void ISR_ASCLIN_Er(void)
{
    IfxStdIf_DPipe_onError(&g_stdInterface);
}

/* Function to initialize ASCLIN module */
void init_UART(void)
{
    IfxAsclin_Asc_Config ascConf;

    IfxAsclin_Asc_initModuleConfig(&ascConf, &MODULE_ASCLIN0);

    /* Set the desired baud rate */
    ascConf.baudrate.prescaler = ASC_PRESCALER;
    ascConf.baudrate.baudrate = ASC_BAUDRATE;
    ascConf.baudrate.oversampling = IfxAsclin_OversamplingFactor_16;            /* Set the oversampling factor      */

    /* Configure the sampling mode */
    ascConf.bitTiming.medianFilter = IfxAsclin_SamplesPerBit_three;             /* Set the number of samples per bit*/
    ascConf.bitTiming.samplePointPosition = IfxAsclin_SamplePointPosition_8;    /* Set the first sample position    */

    /* ISR priorities and interrupt target */
    ascConf.interrupt.txPriority = ISR_PRIORITY_ASCLIN_TX;
    ascConf.interrupt.rxPriority = ISR_PRIORITY_ASCLIN_RX;
    ascConf.interrupt.erPriority = ISR_PRIORITY_ASCLIN_ER;
    ascConf.interrupt.typeOfService = IfxSrc_Tos_cpu0;

    /* FIFO configuration */
    ascConf.txBuffer = g_AscTxBuffer;
    ascConf.txBufferSize = ASC_TX_BUFFER_SIZE;
    ascConf.rxBuffer = g_AscRxBuffer;
    ascConf.rxBufferSize = ASC_RX_BUFFER_SIZE;

    /* Pin configuration */
    const IfxAsclin_Asc_Pins pins = {
            NULL, IfxPort_InputMode_pullUp,                                       /* CTS port pin not used            */
            &IfxAsclin0_RXA_P14_1_IN, IfxPort_InputMode_pullUp,                   /* RX port pin                      */
            NULL, IfxPort_OutputMode_pushPull,                                    /* RTS port pin not used            */
            &IfxAsclin0_TX_P14_0_OUT, IfxPort_OutputMode_pushPull,                /* TX port pin                      */
            IfxPort_PadDriver_cmosAutomotiveSpeed1
    };
    ascConf.pins = &pins;

    /* Initialize the module */
    IfxAsclin_Asc_initModule(&g_asc, &ascConf);

    /* Initialize the Standard Interface */
    IfxAsclin_Asc_stdIfDPipeInit(&g_stdInterface, &g_asc);
}
