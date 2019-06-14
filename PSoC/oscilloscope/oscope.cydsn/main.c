// #------------------------------------------------------------------------------
 // # Aaron Wood
 // # aamwood
 // # CMPE 121 
 // # 6/6/2019
 // # PSOC final project
 // #------------------------------------------------------------------------------

#include <project.h>

//usb defines
#define USBFS_DEVICE  (0u)
#define CH1_EP_NUM    (1u)
#define CH2_EP_NUM    (2u)
#define BUFFER_SIZE   (64u)
#define RD_BUFFER_SIZE        (2u)
#define WR_BUFFER_SIZE        (1u)

//IC2 clock selection defines
#define C1K 0
#define C10K 1
#define C20K 2
#define C50K 3
#define C100K 4 

//dma flags
#define KEY 0
#define PEELE 1
#define BUSTER 0
#define KEATON 1

/* Defines for DMA_2 */
#define DMA_2_BYTES_PER_BURST 1
#define DMA_2_REQUEST_PER_BURST 1
#define DMA_2_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_2_DST_BASE (CYDEV_SRAM_BASE)


//defines DMA 1
#define DMA_1_BYTES_PER_BURST 1
#define DMA_1_REQUEST_PER_BURST 1
#define DMA_1_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_1_DST_BASE (CYDEV_SRAM_BASE)

//ADC linear shift
#define SCALAR 150
#define BIAS 50

//dma ch1 arrays
uint8 key[BUFFER_SIZE];
uint8 peele[BUFFER_SIZE];

//dma ch2 arrays
uint8 buster[BUFFER_SIZE];
uint8 keaton[BUFFER_SIZE];

//initialize dma flags
int CH1_DMAstate=KEY;
int CH2_DMAstate=BUSTER;

//open USB flags
int ch1flag;
int ch2flag;

char pot1;
char pot2;

//I2C variable initialization
uint8 readbyteCount = 0u;
uint8 read_buffer[RD_BUFFER_SIZE] = {0};
uint8 write_buffer[WR_BUFFER_SIZE] = {0};
uint8 command_reg = 0; // Command register to capture writes from I2C

//swap tx dma flags
CY_ISR(CH1_DMA_SWAP){
    if(CH1_DMAstate==KEY){CH1_DMAstate=PEELE;}
    else if(CH1_DMAstate==PEELE){CH1_DMAstate=KEY;}
    ch1flag=1;
}

//swap rx dma flags
CY_ISR(CH2_DMA_SWAP){
    if(CH2_DMAstate==BUSTER){CH2_DMAstate=KEATON;}
    else if(CH2_DMAstate==KEATON){CH2_DMAstate=BUSTER;}
    ch2flag=1;
}

//check Potentiometer values, send over I2C
CY_ISR(measure){
    static int chan =0; //chan will swap between 0 and 1 on every ISR
    //read pot1
    if(chan==0){
        AMux_FastSelect(1);
        pot1=(ADC_Read32()/SCALAR)-BIAS;   //i found that reading the ADC in 8 bit mode was unstable so this was my solution
        measure_ClearPending();
        chan =1;
    }
    //read pot2
    else if(chan==1){
        AMux_FastSelect(0);
        pot2=(ADC_Read32()/SCALAR)-BIAS;
        measure_ClearPending();
        chan =0;
    }
    //send value to Pi via I2C
    if(I2CS_SlaveStatus() & I2CS_SSTAT_RD_CMPLT){
		readbyteCount = I2CS_SlaveGetReadBufSize();
		if(readbyteCount == RD_BUFFER_SIZE){
			I2CS_SlaveClearReadStatus();
			I2CS_SlaveClearReadBuf();
			CyGlobalIntDisable;
			read_buffer[0] = pot1;
			read_buffer[1] = pot2;
			CyGlobalIntEnable;
		}
	}
	//see if new sample rate has been sent by rPi
    if(I2CS_SlaveStatus() & I2CS_SSTAT_WR_CMPLT)
       {
        // Copy command into command register
        command_reg = write_buffer[0];
        // Clear status
        I2CS_SlaveClearWriteStatus();
        I2CS_SlaveClearWriteBuf();
        //select dma clock via mux
        //control reg 1 selects mux input
        switch(command_reg){
            case 1:
                Control_Reg_1_Write(C1K);
                break;
            case 10:
                Control_Reg_1_Write(C10K);
                break;
            case 20:
                Control_Reg_1_Write(C20K);
                break;
            case 50:
                Control_Reg_1_Write(C50K);
                break;
            case 100:
                Control_Reg_1_Write(C100K);
            default:
                Control_Reg_1_Write(C10K);
                break;
        }
    }
         
}

int main(void)
{
    //hardware initialization
    CyGlobalIntEnable;
    USBFS_Start(USBFS_DEVICE, USBFS_5V_OPERATION);
    CH1_DMA_SWAP_StartEx(CH1_DMA_SWAP);
    CH2_DMA_SWAP_StartEx(CH2_DMA_SWAP);
    Control_Reg_1_Write(2);
    ADC_SAR_1_Start();
    ADC_SAR_1_StartConvert();
    ADC_SAR_2_Start();
    ADC_SAR_2_StartConvert();
    I2CS_SlaveInitWriteBuf((uint8 *) write_buffer, WR_BUFFER_SIZE);   
    I2CS_SlaveInitReadBuf((uint8 *) read_buffer, RD_BUFFER_SIZE);
    I2CS_Start();
    ADC_Start();
    measure_StartEx(measure);
    while (0u == USBFS_GetConfiguration())
    {
    }

    //declare DMA variables
    uint8 DMA_1_Chan;
    uint8 DMA_1_TD[2];
    uint8 DMA_2_Chan;
    uint8 DMA_2_TD[2];

    /* DMA Configuration for DMA_1 */
    DMA_1_Chan = DMA_1_DmaInitialize(DMA_1_BYTES_PER_BURST, DMA_1_REQUEST_PER_BURST, HI16(DMA_1_SRC_BASE), HI16(DMA_1_DST_BASE));
    DMA_1_TD[0] = CyDmaTdAllocate();
    DMA_1_TD[1] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_1_TD[0], 64, DMA_1_TD[1],  DMA_1__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_1_TD[1], 64, DMA_1_TD[0],  DMA_1__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA_1_TD[0], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR ), LO16((uint32)&key[0]));
    CyDmaTdSetAddress(DMA_1_TD[1], LO16((uint32)ADC_SAR_1_SAR_WRK0_PTR ), LO16((uint32)&peele[0]));
    CyDmaChSetInitialTd(DMA_1_Chan, DMA_1_TD[0]);
    CyDmaChEnable(DMA_1_Chan, 1);

    /* DMA Configuration for DMA_2 */
    DMA_2_Chan = DMA_2_DmaInitialize(DMA_2_BYTES_PER_BURST, DMA_2_REQUEST_PER_BURST, HI16(DMA_2_SRC_BASE), HI16(DMA_2_DST_BASE));
    DMA_2_TD[0] = CyDmaTdAllocate();
    DMA_2_TD[1] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_2_TD[0], 64, DMA_2_TD[1],  DMA_2__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetConfiguration(DMA_2_TD[1], 64, DMA_2_TD[0],  DMA_2__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA_2_TD[0], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)&buster[0]));
    CyDmaTdSetAddress(DMA_2_TD[1], LO16((uint32)ADC_SAR_2_SAR_WRK0_PTR), LO16((uint32)&keaton[0]));
    CyDmaChSetInitialTd(DMA_2_Chan, DMA_2_TD[0]);
    CyDmaChEnable(DMA_2_Chan, 1);


    //main loop
    for(;;)
    {
        //ch1 usb transfer
        if ((USBFS_IN_BUFFER_EMPTY == USBFS_GetEPState(CH1_EP_NUM))&&ch1flag){
            if (CH1_DMAstate==KEY) {
                USBFS_LoadInEP(CH1_EP_NUM, peele, BUFFER_SIZE);
            } else if(CH1_DMAstate==PEELE){
                USBFS_LoadInEP(CH1_EP_NUM, key, BUFFER_SIZE);
            }
            ch1flag=0;
        }

        //ch2 USB transfer
        if ((USBFS_IN_BUFFER_EMPTY == USBFS_GetEPState(CH2_EP_NUM))&&ch2flag){
            if (CH2_DMAstate==KEATON) {
                USBFS_LoadInEP(CH2_EP_NUM, buster, BUFFER_SIZE);
            } else if(CH2_DMAstate==BUSTER){
                USBFS_LoadInEP(CH2_EP_NUM, keaton, BUFFER_SIZE);
            }
            ch2flag=0;
        }
    }
}