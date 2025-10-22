#include <decthandler.h>

LOG_MODULE_REGISTER(decthandler);




k_msgq* DECTached::get_rx_avalible(){
	//Just a return method for the RX queue
	return &this->rx_msq;
}



int DECTached::init(uint16_t device_id, nrf_modem_dect_phy_radio_mode phy_mode){


	LOG_INF("initializing dect, Pray!!!");

	//Initializes the library to interface with the modem
	int err = nrf_modem_lib_init();
	if (err) {
		LOG_ERR("modem init failed, err %d", err);
		return err;
	}

	//Sets the handler for asynctronous callbacks
	err = nrf_modem_dect_phy_event_handler_set(DECTached::dect_phy_event_handler);
	if (err) {
		LOG_ERR("nrf_modem_dect_phy_event_handler_set failed, err %d", err);
		return err;
	}

	//Asks for the modem to wakeup and initialize
	err = nrf_modem_dect_phy_init();
	if (err) {
		LOG_ERR("nrf_modem_dect_phy_init failed, err %d", err);
		return err;
	}

	//See https://www.etsi.org/deliver/etsi_ts/103600_103699/10363604/01.03.01_60/ts_10363604v010301p.pdfm, section 6.2 for information
	header.packet_length = 0x01;
	header.packet_length_type = 0x0;
	header.header_format = 0x0;
	header.short_network_id = (CONFIG_NETWORK_ID & 0xff);
	header.transmitter_id_hi = (device_id >> 8);
	header.transmitter_id_lo = (device_id & 0xff);
	header.df_mcs = CONFIG_MCS;
	header.reserved = 0;
	header.transmit_power = CONFIG_TX_POWER;
	header.pad = 0; // 




	//Waits for the modem to be ready
	k_sem_take(&operation_sem, K_FOREVER);
	if (exit) {
		return -EIO;
	}


	//The parameters for the connection
	dect_phy_config_params = {
	.band_group_index = ((CONFIG_CARRIER >= 525 && CONFIG_CARRIER <= 551)) ? 1 : 0,
	.harq_rx_process_count = 4,
	.harq_rx_expiry_time_us = 5000000,
	};


	//Configuring parameters for the carrier and timeout
	err = nrf_modem_dect_phy_configure(&dect_phy_config_params);
	if (err) {
		LOG_ERR("nrf_modem_dect_phy_configure failed, err %d", err);
		return err;
	}


	//Waits for the modem to become avalible again
	k_sem_take(&operation_sem, K_FOREVER);
	if (exit) {
		return -EIO;
	}


	//Sets the operational mode for latency and listen before talk	
	err = nrf_modem_dect_phy_activate(phy_mode);
	if (err) {
		LOG_ERR("nrf_modem_dect_phy_activate failed, err %d", err);
		return err;
	}

	k_sem_take(&operation_sem, K_FOREVER);
 

	//Fetches ID for use when transmitting
	device_id = get_id();



	//Saves what instance this is for later use...
    instance = this;
    k_msgq_init(&rx_msq, rx_msgq_buffer, sizeof(struct nrf_modem_dect_phy_pdc_event), sizeof(rx_msgq_buffer)/sizeof(struct nrf_modem_dect_phy_pdc_event));    
	

	this->ready = true; 
	return 0;
}

DECTached::DECTached(){
	k_sem_init(&operation_sem, 0, 1);
	k_sem_init(&deinit_sem, 0, 1);
}




uint16_t DECTached::get_id(){
	uint16_t id = 0;
	hwinfo_get_device_id((uint8_t *)&id, sizeof(id));
	return id;
}




void DECTached::set_tx_power(TxPower power){
	header.transmit_power = power;
}



int DECTached::get_capability(){
	int err = nrf_modem_dect_phy_capability_get();
	if (err) {
		LOG_ERR("nrf_modem_dect_phy_capability_get failed, err %d", err);
	}
}

int DECTached::transmit(uint8_t* data, size_t data_len){
	
	int handle = 0;
	int err;
	struct nrf_modem_dect_phy_tx_params tx_op_params = {
		.start_time = 0,
		.handle = handle,
		.network_id = CONFIG_NETWORK_ID,
		.phy_type = 0,
		.lbt_rssi_threshold_max = 0,
		.carrier = CONFIG_CARRIER,
		.lbt_period = NRF_MODEM_DECT_LBT_PERIOD_MAX,
		.phy_header = (union nrf_modem_dect_phy_hdr *)&header,
		.data = data,
		.data_size = data_len,
	};
	
	err = nrf_modem_dect_phy_tx(&tx_op_params);
	if (err != 0) {
		return err;
	}
	k_sem_take(&operation_sem, K_FOREVER);

	return 0;
}



/* Receive operation. */
static int receive(uint32_t handle)
{
	int err;

struct nrf_modem_dect_phy_rx_params rx_op_params{};

	rx_op_params.start_time = 0;
	rx_op_params.handle = handle;
	rx_op_params.network_id = CONFIG_NETWORK_ID;
	rx_op_params.mode = NRF_MODEM_DECT_PHY_RX_MODE_CONTINUOUS;
	rx_op_params.rssi_interval = NRF_MODEM_DECT_PHY_RSSI_INTERVAL_OFF;
	rx_op_params.link_id = NRF_MODEM_DECT_PHY_LINK_UNSPECIFIED;
	rx_op_params.rssi_level = -100;
	rx_op_params.carrier = CONFIG_CARRIER;
	rx_op_params.duration = CONFIG_RX_PERIOD_S * MSEC_PER_SEC *
							NRF_MODEM_DECT_MODEM_TIME_TICK_RATE_KHZ;
	rx_op_params.filter.short_network_id = CONFIG_NETWORK_ID & 0xff;
	rx_op_params.filter.is_short_network_id_used = 1;
	rx_op_params.filter.receiver_identity = 0;

	err = nrf_modem_dect_phy_rx(&rx_op_params);
	if (err != 0) {
		return err;
	}
	
	return 0;
}
