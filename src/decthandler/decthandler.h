
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrf_modem_dect_phy.h>
#include <modem/nrf_modem_lib.h>
#include <zephyr/drivers/hwinfo.h>
#include <decthandler_types.h>

//#ifdef __cplusplus
//extern "C" {
//#endif


//For all the nesseary info look at https://docs.nordicsemi.com/bundle/ncs-latest/page/nrfxlib/nrf_modem/doc/dectphy.html#nrf-modem-dect-phy





extern bool exit;


//No wires dectached
class DECTached{



public:
    DECTached();

    /// @brief Complete initialization of the radio with the physical mode selected.
    /// @param device_id the ID the device will show up as
    /// @param phy_mode 3 optional physical modes see \ref nrf_modem_dect_phy_radio_mode "the three physical modes" for details
    /// @return 0 for sucess. 
    int init(uint16_t device_id=0x0042, nrf_modem_dect_phy_radio_mode phy_mode = NRF_MODEM_DECT_PHY_RADIO_MODE_LOW_LATENCY);
    
    /// @brief Device ID, maybe its 64 bits, but sample code and package structure only allows for 16 bits, thats what we assume
    /// @return the device address
    uint16_t get_id();

    /// @brief Prints information about the modem to the terminal.
    /// @return 0 on successfull request
    int get_capability();



    /// @brief Transmits a set of
    /// @param data 
    /// @param data_len 
    /// @return 
    int transmit(uint8_t* data, size_t data_len);

    /// @brief Returns true if the system is set up
    /// @return True if the message queue and driver is ready.
    bool is_ready() {return this->ready;}

    /// @brief Sets the transmission power according to table 6.2.1-3a in ETSI TS 103 636-4
    /// @param power a allowed power setting see TxPower enum for more info.
    void set_tx_power(TxPower power);


    /// @brief Gets the pointer for the RX queue of messages.
    k_msgq* get_rx_avalible();

    // Static event handler for C callback
    static void dect_phy_event_handler(const struct nrf_modem_dect_phy_event *evt);

    // Static pointer to current instance
    static DECTached *instance;

    
    /* Semaphore to synchronize modem calls. and busy handling */
    k_sem operation_sem;
    k_sem deinit_sem;


private:


    //Information about the header used for transmission
    phy_ctrl_field_common header; 

    //Value to hold if the interface is ready for use
    bool ready = false;



    //The queue for incoming messages
    struct k_msgq rx_msq;
    //Function to be called at reception. //! Not used
    void (*rx_handle_function)(const nrf_modem_dect_phy_pdc_event*) = nullptr;
    char rx_msgq_buffer[10 * sizeof(struct nrf_modem_dect_phy_pdc_event)];


    struct nrf_modem_dect_phy_config_params dect_phy_config_params;

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