#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_gpio.h"
#include "nrfx_twim.h"
#include "nrf52810.h"
#include "feng_twi.h"
#include "feng_gpiote.h"
//=======================================================================

static uint8_t m_sample;

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

static volatile bool m_xfer_done = false;  
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
	switch(p_event->type)
	{
	case NRF_DRV_TWI_EVT_DONE:
		switch(p_event->xfer_desc.type)
		{
		case NRF_DRV_TWI_XFER_TX:
			break;
		case NRF_DRV_TWI_XFER_RX:
			break;
		case NRF_DRV_TWI_XFER_TXRX:
			break;
		case NRF_DRV_TWI_XFER_TXTX:
			break;
		}
		break;

	default:
		break;
	}

	m_xfer_done = true;
}
/* ***********************************************************

*/
void twi_init(void)
{
	static ret_code_t err_code;

    const nrf_drv_twi_config_t twi_tp_config = {
       .scl                = TP_SCL_PIN,
       .sda                = TP_SDA_PIN,
       .frequency          = NRF_DRV_TWI_FREQ_250K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false, 
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_tp_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

//wrtie value to reg
void nrf_twi_tx(uint8_t reg_addr, uint8_t reg_value)
{
	ret_code_t err_code;
	uint8_t reg[2] = {reg_addr, reg_value};

	m_xfer_done = false;
	err_code = nrf_drv_twi_tx(&m_twi, TP_I2C_ADDRESS, reg, sizeof(reg), false);
	if(err_code != NRF_SUCCESS)
		return;

	while(!m_xfer_done);
}

void nrf_twi_rx(uint8_t reg_addr, uint8_t *p_data, uint8_t length)
{
	ret_code_t err_code;

	m_xfer_done = false;
	err_code = nrf_drv_twi_tx(&m_twi, TP_I2C_ADDRESS, &reg_addr, sizeof(reg_addr), false);
	if(err_code != NRF_SUCCESS)
		return;

	while(!m_xfer_done);

	m_xfer_done = false;
	err_code = nrf_drv_twi_rx(&m_twi, TP_I2C_ADDRESS, p_data, length);
	if(err_code != NRF_SUCCESS)
		return;

	while(!m_xfer_done);
}


void disable_nrf_twi(void)
{
	nrf_drv_twi_disable(&m_twi);
}

void enable_nrf_twi(void)
{
	nrf_drv_twi_enable(&m_twi);
}

bool is_twi_busy(void)
{
	bool flag;
	
	flag = nrf_drv_twi_is_busy(&m_twi);
	return flag;
}

 
void stop_task(void)
{
	nrf_drv_twi_stopped_event_get(&m_twi);
}
