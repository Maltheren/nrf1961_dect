#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrf_modem_dect_phy.h>
#include <modem/nrf_modem_lib.h>
#include <zephyr/drivers/hwinfo.h>

//#ifdef __cplusplus
//extern "C" {
//#endif


extern struct k_sem operation_sem;
extern struct k_sem deinit_sem;
extern bool exit;


//No wires dectached
class DECTached{

public:
    int init();
    int attach_rx_handle();
    int transmit();



    // Static event handler for C callback
    static void dect_phy_event_handler(const struct nrf_modem_dect_phy_event *evt);

    // Static pointer to current instance
    static DECTached *instance;

private:




    //Currently stored params for the dect modem
    struct nrf_modem_dect_phy_rx_params rx_op_params;

    // Event handler callbacks
    void on_init(const struct nrf_modem_dect_phy_init_event *evt);
    void on_deinit(const struct nrf_modem_dect_phy_deinit_event *evt);
    void on_activate(const struct nrf_modem_dect_phy_activate_event *evt);
    void on_deactivate(const struct nrf_modem_dect_phy_deactivate_event *evt);
    void on_configure(const struct nrf_modem_dect_phy_configure_event *evt);
    void on_link_config(const struct nrf_modem_dect_phy_link_config_event *evt);
    void on_radio_config(const struct nrf_modem_dect_phy_radio_config_event *evt);
    void on_capability_get(const struct nrf_modem_dect_phy_capability_get_event *evt);
    void on_bands_get(const struct nrf_modem_dect_phy_band_get_event *evt);
    void on_latency_info_get(const struct nrf_modem_dect_phy_latency_info_event *evt);
    void on_time_get(const struct nrf_modem_dect_phy_time_get_event *evt);
    void on_cancel(const struct nrf_modem_dect_phy_cancel_event *evt);
    void on_op_complete(const struct nrf_modem_dect_phy_op_complete_event *evt);
    void on_pcc(const struct nrf_modem_dect_phy_pcc_event *evt);
    void on_pcc_crc_err(const struct nrf_modem_dect_phy_pcc_crc_failure_event *evt);
    void on_pdc(const struct nrf_modem_dect_phy_pdc_event *evt);
    void on_pdc_crc_err(const struct nrf_modem_dect_phy_pdc_crc_failure_event *evt);
    void on_rssi(const struct nrf_modem_dect_phy_rssi_event *evt);
    void on_stf_cover_seq_control(const struct nrf_modem_dect_phy_stf_control_event *evt);


};
extern DECTached DECT;

//#ifdef __cplusplus
//}
//#endif