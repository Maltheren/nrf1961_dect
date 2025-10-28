#include <decthandler.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(callback_decthandler);
DECTached DECT;




static uint64_t modem_time;
bool exit;
DECTached* DECTached::instance = &DECT;





/* Callback after init operation. */
void DECTached::on_init(const struct nrf_modem_dect_phy_init_event *evt)
{
	if (evt->err) {
		LOG_ERR("Init failed, err %d", evt->err);
		exit = true;
		return;
	}

	k_sem_give(&operation_sem);
}

/* Callback after deinit operation. */
void DECTached::on_deinit(const struct nrf_modem_dect_phy_deinit_event *evt)
{
	if (evt->err) {
		LOG_ERR("Deinit failed, err %d", evt->err);
		return;
	}

	k_sem_give(&deinit_sem);
}

void DECTached::on_activate(const struct nrf_modem_dect_phy_activate_event *evt)
{
	if (evt->err) {
		LOG_ERR("Activate failed, err %d", evt->err);
		exit = true;
		return;
	}

	k_sem_give(&operation_sem);
}

void DECTached::on_deactivate(const struct nrf_modem_dect_phy_deactivate_event *evt)
{

	if (evt->err) {
		LOG_ERR("Deactivate failed, err %d", evt->err);
		return;
	}

	k_sem_give(&deinit_sem);
}

void DECTached::on_configure(const struct nrf_modem_dect_phy_configure_event *evt)
{
	if (evt->err) {
		LOG_ERR("Configure failed, err %d", evt->err);
		return;
	}

	k_sem_give(&operation_sem);
}

/* Callback after link configuration operation. */
void DECTached::on_link_config(const struct nrf_modem_dect_phy_link_config_event *evt)
{
	LOG_DBG("link_config cb time %"PRIu64" status %d", modem_time, evt->err);
}

void DECTached::on_radio_config(const struct nrf_modem_dect_phy_radio_config_event *evt)
{
	if (evt->err) {
		LOG_ERR("Radio config failed, err %d", evt->err);
		return;
	}

	k_sem_give(&operation_sem);
}

/* Callback after capability get operation. */
void DECTached::on_capability_get(const struct nrf_modem_dect_phy_capability_get_event *evt)
{
	LOG_DBG("capability_get cb time %"PRIu64" status %d", modem_time, evt->err);
}

void DECTached::on_bands_get(const struct nrf_modem_dect_phy_band_get_event *evt)
{
	LOG_DBG("bands_get cb status %d", evt->err);
}

void DECTached::on_latency_info_get(const struct nrf_modem_dect_phy_latency_info_event *evt)
{
	LOG_DBG("latency_info_get cb status %d", evt->err);
}

/* Callback after time query operation. */
void DECTached::on_time_get(const struct nrf_modem_dect_phy_time_get_event *evt)
{
	LOG_DBG("time_get cb time %"PRIu64" status %d", modem_time, evt->err);
}

void DECTached::on_cancel(const struct nrf_modem_dect_phy_cancel_event *evt)
{
	LOG_DBG("on_cancel cb status %d", evt->err);
	k_sem_give(&operation_sem);
}

/* Operation complete notification. */
void DECTached::on_op_complete(const struct nrf_modem_dect_phy_op_complete_event *evt)
{
	LOG_DBG("op_complete cb time %"PRIu64" status %d", modem_time, evt->err);
	k_sem_give(&operation_sem);
}

/* Physical Control Channel reception notification. */
void DECTached::on_pcc(const struct nrf_modem_dect_phy_pcc_event *evt)
{
	LOG_INF("Received header from device ID %d",
		evt->hdr.hdr_type_1.transmitter_id_hi << 8 | evt->hdr.hdr_type_1.transmitter_id_lo);
	
}

/* Physical Control Channel CRC error notification. */
void DECTached::on_pcc_crc_err(const struct nrf_modem_dect_phy_pcc_crc_failure_event *evt)
{
	LOG_DBG("pcc_crc_err cb time %"PRIu64"", modem_time);
}

/* Physical Data Channel reception notification. */
void DECTached::on_pdc(const struct nrf_modem_dect_phy_pdc_event *evt) //THIS is for receiption 
{
	/* Received RSSI value is in fixed precision format Q14.1 */
	LOG_INF("Received data (RSSI: %d.%d): %s",
		(evt->rssi_2 / 2), (evt->rssi_2 & 0b1) * 5, (char *)evt->data);
	

	if (rx_handle_function == nullptr){
		int err = k_msgq_put(&rx_msq, &evt, K_NO_WAIT);
		if(err){
			LOG_ERR("Putting into rxqueue failed %d", err);
		}
	}
	else{
		rx_handle_function(evt);
	}
	
}

/* Physical Data Channel CRC error notification. */
void DECTached::on_pdc_crc_err(const struct nrf_modem_dect_phy_pdc_crc_failure_event *evt)
{
	LOG_DBG("pdc_crc_err cb time %"PRIu64"", modem_time);
}

/* RSSI measurement result notification. */
void DECTached::on_rssi(const struct nrf_modem_dect_phy_rssi_event *evt)
{
	LOG_DBG("rssi cb time %"PRIu64" carrier %d", modem_time, evt->carrier);
}

void DECTached::on_stf_cover_seq_control(const struct nrf_modem_dect_phy_stf_control_event *evt)
{
	LOG_WRN("Unexpectedly in %s\n", (__func__));
}





void DECTached::dect_phy_event_handler(const struct nrf_modem_dect_phy_event *evt)
{
	modem_time = evt->time;

	switch (evt->id) {
	case NRF_MODEM_DECT_PHY_EVT_INIT:
		instance->on_init(&evt->init);
		break;
	case NRF_MODEM_DECT_PHY_EVT_DEINIT:
		instance->on_deinit(&evt->deinit);
		break;
	case NRF_MODEM_DECT_PHY_EVT_ACTIVATE:
		instance->on_activate(&evt->activate);
		break;
	case NRF_MODEM_DECT_PHY_EVT_DEACTIVATE:
		instance->on_deactivate(&evt->deactivate);
		break;
	case NRF_MODEM_DECT_PHY_EVT_CONFIGURE:
		instance->on_configure(&evt->configure);
		break;
	case NRF_MODEM_DECT_PHY_EVT_RADIO_CONFIG:
		instance->on_radio_config(&evt->radio_config);
		break;
	case NRF_MODEM_DECT_PHY_EVT_COMPLETED:
		instance->on_op_complete(&evt->op_complete);
		break;
	case NRF_MODEM_DECT_PHY_EVT_CANCELED:
		instance->on_cancel(&evt->cancel);
		break;
	case NRF_MODEM_DECT_PHY_EVT_RSSI:
		instance->on_rssi(&evt->rssi);
		break;
	case NRF_MODEM_DECT_PHY_EVT_PCC:
		instance->on_pcc(&evt->pcc);
		break;
	case NRF_MODEM_DECT_PHY_EVT_PCC_ERROR:
		instance->on_pcc_crc_err(&evt->pcc_crc_err);
		break;
	case NRF_MODEM_DECT_PHY_EVT_PDC:
		instance->on_pdc(&evt->pdc);
		break;
	case NRF_MODEM_DECT_PHY_EVT_PDC_ERROR:
		instance->on_pdc_crc_err(&evt->pdc_crc_err);
		break;
	case NRF_MODEM_DECT_PHY_EVT_TIME:
		instance->on_time_get(&evt->time_get);
		break;
	case NRF_MODEM_DECT_PHY_EVT_CAPABILITY:
		instance->on_capability_get(&evt->capability_get);
		break;
	case NRF_MODEM_DECT_PHY_EVT_BANDS:
		instance->on_bands_get(&evt->band_get);
		break;
	case NRF_MODEM_DECT_PHY_EVT_LATENCY:
		instance->on_latency_info_get(&evt->latency_get);
		break;
	case NRF_MODEM_DECT_PHY_EVT_LINK_CONFIG:
		instance->on_link_config(&evt->link_config);
		break;
	case NRF_MODEM_DECT_PHY_EVT_STF_CONFIG:
		instance->on_stf_cover_seq_control(&evt->stf_cover_seq_control);
		break;
	}
}