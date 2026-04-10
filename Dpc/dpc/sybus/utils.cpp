#include "utils.h"

// Рассчитать CRC
#define CRC16CCITT  0x1021   /*0x1021 0x8408  0x8810  */


namespace {
const uint8_t Day[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const uint16_t Days[12] = { 0,  31, 59, 90,120,151,181,212,243,273,304,334 };
#define SECONDS_IN_HOUR  (60L*60L)
#define SECONDS_IN_DAY   (24L*SECONDS_IN_HOUR)
#define SECOND_DELTA_1970_1980 (SECONDS_IN_DAY*((10*365)+2)) // Високосные года (72,76)

typedef struct {
    uint16_t    mSecond;
    uint32_t    Second;
} TZTime;

typedef struct {
    uint16_t    msec;
    uint8_t     sec, min, hour, day, month;
    uint16_t    year;
} TBinTime;

void ZTimeToBin(TZTime* ZilogTime, TBinTime* t)
{
    uint32_t time;

    time = ZilogTime->Second;
    t->msec = ZilogTime->mSecond;

    t->sec = time % 60;
    time /= 60;                             /* Time in minutes */
    t->min = time % 60;
    time /= 60;                             /* Time in hours */

    t->year = 1980 + (int)((time / (1461L * 24L)) << 2); // 1461 = 365*3+366
    time %= 1461L * 24L;

    if (time >= 366 * 24) {
        time -= 366 * 24;
        t->year += 1 + (int)(time / (365 * 24));
        time %= 365 * 24; }

    t->hour = time % 24;
    time /= 24;                             /* Time in days */
    ++time;

    if ((t->year & 3) == 0) {
        if (time > 60) --time;
        else if (time == 60) {
            t->month = 2;
            t->day = 29;
            return; }
    }

    for (t->month = 0; Day[t->month] < time; ++t->month) time -= Day[t->month];

    t->day = time;
    ++t->month;
}

void BinToZTime(TBinTime* t, TZTime* ZilogTime)
{
    uint32_t  x = 0;
    uint16_t   days = 0;
    uint16_t   i = t->year - 1980;       // Количество полных прошедших лет 0..137

    if (i) x = (i >> 2) * 1461L * SECONDS_IN_DAY; // Секунд в полных четырехлетиях
    i = i & 3;
    if (i) x += (i * 365L + 1) * SECONDS_IN_DAY;   // Секунд в неполном четырехлетии
    days = Days[t->month - 1] + t->day - 1;
    if ((t->month > 2) && (i == 0)) ++days;   // В високосном еще один день

    x += (days * 24 + t->hour) * SECONDS_IN_HOUR;
    x += 60L * t->min + t->sec;
    ZilogTime->Second = x;
    ZilogTime->mSecond = t->msec;
}
}
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

    QDateTime fromStrictTime(uint64_t value)
    {
        TBinTime* bin_time = new TBinTime;
        TZTime* z_time = new TZTime;

        //  Формирование структур
        z_time->mSecond = value & 0xFFFF;
        z_time->Second = value >> 16;
        ZTimeToBin(z_time, bin_time);

        //  Перенос данных
        int msec = bin_time->msec % 1000;
        QTime xtime(bin_time->hour, bin_time->min, bin_time->sec, msec);
        QDate xdate(bin_time->year, bin_time->month, bin_time->day);
        delete bin_time;
        delete z_time;

        return QDateTime(xdate, xtime);
    }

    QDateTime fromTime61850(uint64_t value)
    {
        TBinTime* bin_time = new TBinTime;
        TZTime* z_time = new TZTime;

        //  Формирование структур
        z_time->Second = (value >> 32) - SECOND_DELTA_1970_1980;
        z_time->mSecond = ((value & 0xFFFFFFFF) * 1000) >> 32;
        ZTimeToBin(z_time, bin_time);

        //  Перенос данных
        int msec = bin_time->msec % 1000;
        QTime xtime(bin_time->hour, bin_time->min, bin_time->sec, msec);
        QDate xdate(bin_time->year, bin_time->month, bin_time->day);
        delete bin_time;
        delete z_time;

        return QDateTime(xdate, xtime);
    }

    QDateTime fromUtcTime(uint64_t value)
    {
        TBinTime* bin_time = new TBinTime;
        TZTime* z_time = new TZTime;

        //  Формирование структур
        z_time->Second = (value >> 32) - SECOND_DELTA_1970_1980;
        z_time->mSecond = (value & 0xFFFFFFFF) / 1000;
        ZTimeToBin(z_time, bin_time);

        //  Перенос данных
        int msec = bin_time->msec % 1000;
        QTime xtime(bin_time->hour, bin_time->min, bin_time->sec, msec);
        QDate xdate(bin_time->year, bin_time->month, bin_time->day);
        delete bin_time;
        delete z_time;

        return QDateTime(xdate, xtime);
    }

    QDateTime fromSecondsTime(uint32_t value)
    {
        TBinTime* bin_time = new TBinTime;
        TZTime* z_time = new TZTime;

        //  Формирование структур
        z_time->Second = value - SECOND_DELTA_1970_1980;
        z_time->mSecond = 0;
        ZTimeToBin(z_time, bin_time);

        //  Перенос данных
        int msec = bin_time->msec % 1000;
        QTime xtime(bin_time->hour, bin_time->min, bin_time->sec, msec);
        QDate xdate(bin_time->year, bin_time->month, bin_time->day);
        delete bin_time;
        delete z_time;

        return QDateTime(xdate, xtime);
    }


} // namespace
