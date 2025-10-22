/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrf_modem_dect_phy.h>
#include <modem/nrf_modem_lib.h>
#include <zephyr/drivers/hwinfo.h>
#include <decthandler.h>

LOG_MODULE_REGISTER(app);

BUILD_ASSERT(CONFIG_CARRIER, "Carrier must be configured according to local regulations");

#define DATA_LEN_MAX 32


static uint16_t device_id;






/* Dect PHY config parameters. */ 
static struct nrf_modem_dect_phy_config_params dect_phy_config_params = {
	.band_group_index = ((CONFIG_CARRIER >= 525 && CONFIG_CARRIER <= 551)) ? 1 : 0,
	.harq_rx_process_count = 4,
	.harq_rx_expiry_time_us = 5000000,
};

/* Send operation. */
static int transmit(uint32_t handle, uint8_t *data, size_t data_len)
{
	int err;

	struct phy_ctrl_field_common header = {
		.packet_length = 0x01,
		.packet_length_type = 0x0,
		.header_format = 0x0,
		.short_network_id = (CONFIG_NETWORK_ID & 0xff),
		.transmitter_id_hi = (device_id >> 8),
		.transmitter_id_lo = (device_id & 0xff),
		.df_mcs = CONFIG_MCS,
		.reserved = 0,
		.transmit_power = CONFIG_TX_POWER,
	};

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

int main(void)
{
	int err;
	uint32_t tx_handle = 0;
	uint32_t rx_handle = 1;
	uint32_t tx_counter_value = 0;
	uint8_t tx_buf[DATA_LEN_MAX];
	size_t tx_len;

	LOG_INF("Dect NR+ PHY Hello sample started");


	LOG_INF("tksssssssssssaigjneuihgiazjfe");


	device_id = DECT.get_id();
	err = DECT.init(device_id);
	LOG_INF("Dect NR+ PHY initialized, device ID: %d", device_id);

	if(err){
		LOG_ERR("panic inital error");
	}

	while (1) {
		
		/** Transmitting message */
		LOG_INF("Transmitting %d", tx_counter_value);
		tx_len = sprintf((char*)tx_buf, "Hello DECT! %d", tx_counter_value) + 1; /* Include \0 */
		err = DECT.transmit(tx_buf, tx_len);
		//err = transmit(tx_handle, tx_buf, tx_len);
		if (err) {
			LOG_ERR("Transmisstion failed, err %d", err);
			return err;
		}

		
		tx_counter_value++;

		/* Wait for TX operation to complete. */
		//k_sem_take(&DECT.operation_sem, K_FOREVER);

		if ((tx_counter_value >= 3600) && CONFIG_TX_TRANSMISSIONS) {
			LOG_INF("Reached maximum number of transmissions (%d)",
				CONFIG_TX_TRANSMISSIONS);
			break;
		}
		
		/** Receiving messages for CONFIG_RX_PERIOD_S seconds. */
		err = receive(rx_handle);
		if (err) {
			LOG_ERR("Reception failed, err %d", err);
			return err;
		}

		/* Wait for RX operation to complete. */
		k_sem_take(&DECT.operation_sem, K_FOREVER);
	}

	LOG_INF("Shutting down");

	err = nrf_modem_dect_phy_deactivate();
	if (err) {
		LOG_ERR("nrf_modem_dect_phy_deactivate failed, err %d", err);
		return err;
	}

	//k_sem_take(&DECT.deinit_sem, K_FOREVER);

	err = nrf_modem_dect_phy_deinit();
	if (err) {
		LOG_ERR("nrf_modem_dect_phy_deinit() failed, err %d", err);
		return err;
	}

	k_sem_take(&DECT.deinit_sem, K_FOREVER);

	err = nrf_modem_lib_shutdown();
	if (err) {
		LOG_ERR("nrf_modem_lib_shutdown() failed, err %d", err);
		return err;
	}

	LOG_INF("Bye!");

	return 0;
}
