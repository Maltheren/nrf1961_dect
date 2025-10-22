
#include <modem/nrf_modem_lib.h>
#include <zephyr/drivers/hwinfo.h>


/* Header type 1, due to endianness the order is different than in the specification. */

struct phy_ctrl_field_common {
	uint32_t packet_length : 4;
	uint32_t packet_length_type : 1;
	uint32_t header_format : 3;
	uint32_t short_network_id : 8;
	uint32_t transmitter_id_hi : 8;
	uint32_t transmitter_id_lo : 8;
	uint32_t df_mcs : 3;
	uint32_t reserved : 1;
	uint32_t transmit_power : 4;
	uint32_t pad : 24;
};


enum TxPower : uint8_t {
    dBm_NEG_40 = 0b0000,
    dBm_NEG_30 = 0b0001,
    dBm_NEG_20 = 0b0010,
    dBm_NEG_13 = 0b0011,
    dBm_NEG_6  = 0b0100,
    dBm_NEG_3  = 0b0101,
    dBm_0      = 0b0110,
    dBm_3      = 0b0111,
    dBm_6      = 0b1000,
    dBm_10     = 0b1001,
    dBm_14     = 0b1010,
    dBm_19     = 0b1011,
    dBm_23     = 0b1100,
    dBm_26     = 0b1101,
    dBm_29     = 0b1110,
    dBm_GT_32  = 0b1111
};