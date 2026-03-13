#include "utils.h"

// Рассчитать CRC
#define CRC16CCITT  0x1021   /*0x1021 0x8408  0x8810  */

namespace Dpc::Sybus {

	uint16_t crc16(uint8_t * ptr, uint32_t size, uint16_t init)
	{
		uint32_t crcb = (init ^ 0xFFFF) << 8;
		while (size--) {
			crcb = (crcb & 0xFFFF00) | *ptr++;
			for (uint32_t i = 8; i > 0; i--) {
				crcb <<= 1;
				if (crcb & (0x8000 << 9))
					crcb ^= (CRC16CCITT << 8);
			}
		}

		return (crcb >> 8) ^ 0xFFFF;
	}
} // namespace