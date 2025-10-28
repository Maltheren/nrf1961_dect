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

		if ((tx_counter_value >= 3600) && CONFIG_TX_TRANSMISSIONS) {
			LOG_INF("Reached maximum number of transmissions (%d)",
				CONFIG_TX_TRANSMISSIONS);
			break;
		}
		
		/** Receiving messages for CONFIG_RX_PERIOD_S seconds. */
		
		
		//err = receive(rx_handle);
		err = DECT.receive(rx_handle, 1000);
		if (err) {
			LOG_ERR("Reception failed, err %d", err);
			return err;
		}
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
