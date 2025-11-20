#include "includes.h"

/*
**
*/
static bool radioIsActive = false;



/*!
 * \brief Gets the board PA selection configuration
 *
 * \param [IN] channel Channel frequency in Hz
 * \retval PaSelect RegPaConfig PaSelect value
 */
static u08 __Sx1278GetPaSelect(u32 channel);




/*
**
*/
void Sx1278SetRfTxPower(s08 power)
{
    u08 __paConfig = 0;
    u08 __paDac = 0;

    __paConfig = Sx1278Read( REG_PACONFIG );
    __paDac = Sx1278Read( REG_PADAC );

    __paConfig = ( __paConfig & RF_PACONFIG_PASELECT_MASK ) | __Sx1278GetPaSelect( sx1278S.Settings.freq );
    __paConfig = ( __paConfig & RF_PACONFIG_MAX_POWER_MASK ) | 0x70;

    if( ( __paConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if( power > 17 )
        {
            __paDac = ( __paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_ON;
        }
        else
        {
            __paDac = ( __paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_OFF;
        }
        if( ( __paDac & RF_PADAC_20DBM_ON ) == RF_PADAC_20DBM_ON )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            __paConfig = ( __paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( u08 )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )
            {
                power = 2;
            }
            if( power > 17 )
            {
                power = 17;
            }
            __paConfig = ( __paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( u08 )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if( power < -1 )
        {
            power = -1;
        }
        if( power > 14 )
        {
            power = 14;
        }
        __paConfig = ( __paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( u08 )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    Sx1278Write( REG_PACONFIG, __paConfig );
    Sx1278Write( REG_PADAC, __paDac );

	sx1278S.Settings.LoRa.Power = power;
}

static u08 __Sx1278GetPaSelect(u32 channel)
{
    if( channel < RF_MID_BAND_THRESH )
    {
        return RF_PACONFIG_PASELECT_PABOOST;
    }
    else
    {
        return RF_PACONFIG_PASELECT_RFO;
    }
}

void SX1276SetAntSwLowPower(bool status)
{
    if(radioIsActive != status)
    {
        radioIsActive = status;

        if( status == false )
        {
            SX1276AntSwInit( );
        }
        else
        {
            SX1276AntSwDeInit( );
        }
    }
}

void SX1276AntSwInit( void )
{
    //高收低发
    HalWriteRxTx(1);
}

void SX1276AntSwDeInit( void )
{
    HalWriteRxTx(0);
}

void SX1276SetAntSw( u08 opMode )
{
    switch( opMode )
    {
	    case RFLR_OPMODE_TRANSMITTER:
	        HalWriteRxTx(0);
	        break;
	        
	    case RFLR_OPMODE_RECEIVER:
	    case RFLR_OPMODE_RECEIVER_SINGLE:
	    case RFLR_OPMODE_CAD:
	    default:
	        HalWriteRxTx(1);
	        break;
    }
}

void SX1276SetReset(void)
{
    __LORA_RST_(0);
    delay(1);
    __LORA_RST_(1);
    delay(6);
}


u08 SpiTransfer( u08 outData )
{
    return BspLoraSpITransmit(outData);
}





