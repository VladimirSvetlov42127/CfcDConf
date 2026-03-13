#ifndef __SMPARLIST_h
#define __SMPARLIST_h


                                                       //      Задачи
#define     TASK_GBS                                  0//      GBS
#define     TASK_SM                                   1//      SYSTEM MANAGEMENT
#define     TASK_SMTCP                                2//      SYSTEM MANAGEMENT with header
#define     TASK_STREAM                            0x08//      потоковые данные и управление
#define     TASK_LINK                              0x09//      интерфейсы связи
#define     TASK_DOU                               0x0A//      Выходные дискреты
#define     TASK_IDIS                              0x0B//      Входные дискреты
#define     TASK_AIN                               0x0C//      Входные аналоги
#define     TASK_CIN                               0x0D//      Счетчики
#define     TASK_ARC                               0x0E//      Архивы
#define     TASK_AOUT                              0x0F//      Выходные аналоги
#define     TASK_STATS                             0x10//      Задача статистики
#define     TASK_FILE                              0x11//      Задача файлы
#define     TASK_CFG                               0x12//      Задача конфигурирования
#define     TASK_PWRSUPP                           0x13//      Задача питания (основного/резервного/аккумуляторного)
#define     TASK_GRAPH                             0x14//      Задача построения векторной диаграммы
#define     TASK_CONVERT                           0x1D//      конвертер протоколов (прозрачный)
#define     TASK_SWITCH                            0x1E//      мост
#define     TASK_CONS                              0x1F//      Консольная задача


                                                       //      Типы параметров
#define     T_NOTYPE                               0x00//      неизвестный тип
#define     T_BYTE                                 0X01//      байт
#define     T_8BIT                                 0x81//      8 бит
#define     T_CHAR                                 0X02//      символ
#define     T_BYTEBOOL                             0x03//      логический байт
#define     T_SHORT                                0x04//      2 байта со знаком
#define     T_WORD                                 0x05//      2 байта без знака
#define     T_16BIT                                0x85//      16 бит
#define     T_BOOL                                 0x06//
#define     T_UNIT                                 0x07//
#define     T_INTEGER                              0x08//      4 байта со знаком
#define     T_DWORD                                0x09//      4 байта без знака
#define     T_32BIT                                0x89//      32 бит
#define     T_LONG                                 0x0A//
#define     T_FLOAT                                0x0B//
#define     T_DATETIME                             0x10//      астрономическое время 4 байта
#define     T_STRING                               0x11//
#define     T_STAMP                                0x12//
#define     T_STRICT                               0x13//      астрономическое время 6 байт
#define     T_USAGE                                0x14//
#define     T_16BYTE                               0x16//      16 байт
#define     T_OUI                                  0x17//      MAC адрес  6 байт big endian
#define     T_IP4ADR                               0x18//      адрес IP4  4 байта big endian
#define     T_UTCTIME                              0x19//      астрономическое время 8 байт
#define     T_SBYTE                                0x1A//      знаковый байт
#define     T_UINT64                               0x1B//      8 байт без знака
#define     T_INT64                                0x1C//      8 байт со знаком
#define     T_TIME61850                            0x1D//      8 байт времени в формате  МЭК 61850

                                                       //      Протоколы
#define     PORT_UNUSE                             0x00//      Порт не используется
#define     PRT_SYBUS                              0x01//      Протокол сети SyBus
#define     PRT_IEC_60870_5_101                    0x02//      Протокол сети МЭК 60870-5-101
#define     PRT_MODBUS                             0x03//      Протокол сети MODBUS

                                                       //      Флаги опций конфигурации плат DSU
#define     DSU_CPU_PRESENT                      0x8000//      Плата с процессором

                                                       //      Коды ПО устройств
#define     FICTIVE_ID                           0x0000//      фиктивный проект
#define     TEMPL_DIN16                          0x0001//      плата DIN16 v1.7 ,PIC18F252,TEMPLATE
#define     TEMPL_DIN16_BOOT                     0x0002//      плата DIN16 v1.7,PIC18F252, загрузчик
#define     EXDI8_EXDI8V32                       0x0003//      плата EXDI8 v3.2 PIC18F452 ПО- EXDI8
#define     EXDI8_EXDI8V32_BOOT                  0x0004//      плата EXDI8 v3.2 PIC18F452 загрузчик
#define     TEMPL_AIN16SV2_3                     0x0005//      плата AIN16S v2.3 PIC18F8520 ПО-Template
#define     TEMPL_AIN16SV2_3_BOOT                0x0006//      плата AIN16S v2.3 PIC18F8520 загрузчик
#define     AIN16R_AIN16SV2_3                    0x0007//      плата AIN16S v2.3 PIC18F8520 ПО-AIN16R-20
#define     EXR3I4_V3_3                          0x0008//      плата EXR3I4 v3.3 ,PIC18F252, EXR3I4
#define     EXR3I4_V3_3_BOOT                     0x0009//      плата EXR3I4 v3.3 ,PIC18F252, загрузчик
#define     RZA33_V3_6                           0x000A//      плата RZA33 v 3.6,TMS320F2812 RZA 33
#define     RZA33_V3_6_BOOT                      0x000B//      плата RZA33 v 3.6,TMS320F2812 RZA 33 загрузчик
#define     RZA44_V4_1                           0x000C//      плата RZA44 v 4.1,TMS320F2812 RZA 44
#define     RZA44_V4_1_BOOT                      0x000D//      плата RZA44 v 4.1,TMS320F2812 RZA 44 загрузчик
#define     RTU2_v2_1                            0x000E//      плата RTU2 v 2.1,,PIC18F252,ПО -RTU2
#define     RTU2_v2_1_BOOT                       0x000F//      плата RTU2 v 2.1,,PIC18F252,загрузчик
#define     EM3Mv21_EM3v32                       0x0010//      платы EM3M v 2.1 и EM3 v 2.2- ПО- EM3
#define     EM3Mv21_EM3v32_BOOT                  0x0011//      платы EM3M v 2.1 и EM3 v 2.2,загрузчик
#define     PLX_v5_2                             0x0012//      плата PLX v 5.2, ПО- PLX
#define     PLX_v5_2_BOOT                        0x0013//      плата PLX v 5.2,загрузчик
#define     EXDP_EXDI8V31                        0x0014//      плата EXDI8 v3.1 PIC18F452 ПО- EXDP
#define     EXDO8_EXDO8V32                       0x0015//      плата EXDO8 v3.2 PIC18F252 ПО- EXDO8
#define     EXDO8_EXDO8V32_BOOT                  0x0016//      плата EXDO8 v3.2 PIC18F252 ПО- EXDO8 -загрузчик
#define     EXLINE_EXLINEV32                     0x0017//      плата EXLINE v3.2 PIC18F252 ПО- EXLINE
#define     EXLINE_EXLINEV32_BOOT                0x0018//      плата EXLINE v3.2 PIC18F252 ПО- EXLINE -загрузчик
#define     EXDZ_EXDZV32                         0x0019//      плата ExDz v3.2 PIC18F252 ПО- EXDZ
#define     DOUT16TR_V2_2                        0x001A//      плата DOUT16-T80 v2.2 PIC18F452 по DOUT16TR
#define     DOUT16TR_V2_2_BOOT                   0x001B//      плата DOUT16-T80 v2.2 PIC18F452 по DOUT16TR - загрузчик
#define     ALINE_ALINE_V2_2                     0x001C//      плата ALINE v2.2 PIC18F252 ПО- ALINE
#define     ALINE_ALINE_V2_3_BOOT                0x001D//      плата ALINE v2.2 PIC18F252 ПО- ALINE  - загрузчик
#define     DZ2_DZ2_V2_2                         0x001E//      плата DZ2 v2.2 PIC18F452 ПО- DZ2
#define     DZ2_DZ2_V2_2_BOOT                    0x001F//      плата DZ2 v2.2 PIC18F452 ПО- DZ2 - загрузчик
#define     DOUT16_v2_1                          0x0020//      плата DOUT16-T05 v2.1 PIC18F252 по DOUT16
#define     DOUT16_v2_1_BOOT                     0x0021//      плата DOUT16-T05 v2.1 PIC18F252 по DOUT16 -загрузчик
#define     DOUT8_V4_3                           0x0022//      плата DOUT8 v4.3 PIC18F252 ПО- DOUT8
#define     DOUT8_V4_3_BOOT                      0x0023//      плата DOUT8 v4.3 PIC18F252 ПО- DOUT8 -загрузчик
#define     R2IN6_V2_11                          0x0024//      плата R2IN6 v2.11 PIC18F252 по R2IN6
#define     R2IN6_V2_11_BOOT                     0x0025//      плата R2IN6 v2.11 PIC18F252 по R2IN6 - загрузчик
#define     AIN8_V3_1                            0x0026//      плата AIN8 v3.1 PIC18F252 ПО AIN8
#define     AIN8_V3_1_BOOT                       0x0027//      плата AIN8 v3.1 PIC18F252 ПО AIN8 - загрузчик
#define     _AIN8_V3_1                           0x0028//      плата AIN8 v3.1 PIC18F2520 ПО AIN8
#define     _AIN8_V3_1_BOOT                      0x0029//      плата AIN8 v3.1 PIC18F2520 ПО AIN8 - загрузчик
#define     _TEMPL_DIN16                         0x002A//      плата DIN16 v1.7 ,PIC18F2520,TEMPLATE
#define     _TEMPL_DIN16_BOOT                    0x002B//      плата DIN16 v1.7,PIC18F2520, загрузчик
#define     DOUT8TR_V2_1                         0x002C//      Плата DOUT8-TN100 v2.1 PIC18F452
#define     DOUT8TR_V2_1_BOOT                    0x002D//      Плата DOUT8-TN100 v2.1 PIC18F452 - загрузчик
#define     R3IN6_V2_3                           0x002E//      плата R3IN6 v 2.3 PIC18F252 ПО R3IN6
#define     _R3IN6_V2_3                          0x002F//      плата R3IN6 v 2.3 PIC18F2520 ПО R3IN6
#define     AOUT4_V2_1                           0x0030//      плата AOUT4 v2.1 PIC18F252 ПО AOUT4
#define     AOUT4_V2_1_BOOT                      0x0031//      плата AOUT4 v2.1 PIC18F252 ПО AOUT4 - загрузчик
#define     AIN8U_V3_1                           0x0032//      плата AIN8U v3.1 PIC18F252 ПО AIN8-U60
#define     _AIN8U_V3_1                          0x0033//      плата AIN8U v3.1 PIC18F2520 ПО AIN8-U60
#define     MEMORY                               0x0034//      плата STEND2 PIC18F252 ПО MEMORY
#define     MEMORY_BOOT                          0x0035//      плата STEND2 PIC18F252 ПО MEMORY - загрузчик
#define     _MEMORY                              0x0036//      плата STEND2 PIC18F2520 ПО MEMORY
#define     _MEMORY_BOOT                         0x0037//      плата STEND2 PIC18F2520 ПО MEMORY - загрузчик
#define     AIN16_I20                            0x0038//      плата AIN16I_20 PIC18F452 по AIN16I_20
#define     AIN16_I20_BOOT                       0x0039//      плата AIN16I_20 PIC18F452 по AIN16I_20 - загрузчик
#define     _AIN16_I20                           0x003A//      плата AIN16I_20 PIC18F4520 по AIN16I_20
#define     _AIN16_I20_BOOT                      0x003B//      плата AIN16I_20 PIC18F4520 по AIN16I_20 - загрузчик
#define     AIN16P                               0x003C//      плата AIN16P PIC18F252 по AIN16P
#define     AIN16P_BOOT                          0x003D//      плата AIN16P PIC18F252 по AIN16P - загрузчик
#define     _AIN16P                              0x003E//      плата AIN16P PIC18F2520 по AIN16P
#define     _AIN16P_BOOT                         0x003F//      плата AIN16P PIC18F2520 по AIN16P - загрузчик
#define     DIN16U                               0x0040//      плата DIN16 v1.7 ,PIC18F252, ПО - DIN16U
#define     _DIN16U                              0x0041//      плата DIN16 v1.7 ,PIC18F2520, ПО - DIN16U
#define     _DOUT16TR_V2_2                       0x0042//      плата DOUT16-T80 v2.2 PIC18F4520 по DOUT16TR
#define     _DOUT16TR_V2_2_BOOT                  0x0043//      плата DOUT16-T80 v2.2 PIC18F4520 по DOUT16TR - загрузчик
#define     _EXDO8_EXDO8V32                      0x0044//      плата EXDO8 v3.2 PIC18F2520 ПО- EXDO8
#define     _EXDO8_EXDO8V32_BOOT                 0x0045//      плата EXDO8 v3.2 PIC18F2520 ПО- EXDO8 -загрузчик
#define     _DOUT16_v2_1                         0x0046//      плата DOUT16-T05 v2.1 PIC18F2520 по DOUT16
#define     _DOUT16_v2_1_BOOT                    0x0047//      плата DOUT16-T05 v2.1 PIC18F2520 по DOUT16 -загрузчик
#define     _DOUT8_V4_3                          0x0048//      плата DOUT8 v4.3 PIC18F2520 ПО- DOUT8
#define     _DOUT8_V4_3_BOOT                     0x0049//      плата DOUT8 v4.3 PIC18F2520 ПО- DOUT8 -загрузчик
#define     _DOUT8TR_V2_1                        0x004A//      Плата DOUT8-TN100 v2.1 PIC18F4520
#define     _DOUT8TR_V2_1_BOOT                   0x004B//      Плата DOUT8-TN100 v2.1 PIC18F4520 - загрузчик
#define     _AOUT4_V2_1                          0x004C//      плата AOUT4 v2.1 PIC18F2520 ПО AOUT4
#define     _AOUT4_V2_1_BOOT                     0x004D//      плата AOUT4 v2.1 PIC18F2520 ПО AOUT4 - загрузчик
#define     _EM3Mv21_EM3v32                      0x004E//      платы EM3M v 2.1 и EM3 v 2.2- ПО- EM3 PIC18F2520
#define     _EM3Mv21_EM3v32_BOOT                 0x004F//      платы EM3M v 2.1 и EM3 v 2.2,загрузчик PIC18F2520
#define     BOXPULT                              0x0050//      плата PULT-5.1 PIC18F452 ПО BOXPULT
#define     BOXPULT_BOOT                         0x0051//      плата PULT-5.1 PIC18F452 ПО BOXPULT - загрузчик
#define     _BOXPULT                             0x0052//      плата PULT-5.1 PIC18F4520 ПО BOXPULT
#define     _BOXPULT_BOOT                        0x0053//      плата PULT-5.1 PIC18F4520 ПО BOXPULT - загрузчик
#define     DOUT64_T80                           0x0054//      Плата DOUT64_T80 pic PIC18F452 ПО DOUT64TR
#define     DOUT64_T80_BOOT                      0x0055//      Плата DOUT64_T80 pic PIC18F452 ПО DOUT64TR - загрузчие
#define     _DOUT64_T80                          0x0056//      Плата DOUT64_T80 pic PIC18F4520 ПО DOUT64TR
#define     _DOUT64_T80_BOOT                     0x0057//      Плата DOUT64_T80 pic PIC18F4520 ПО DOUT64TR - загрузчие
#define     DIN64                                0x0058//      Плата DIN64 (DCPW v3.2) pic PIC18F452 ПО DIN64
#define     DIN64_BOOT                           0x0059//      Плата DIN64 (DCPW v3.2) pic PIC18F452 ПО DIN64 - загрузчие
#define     _DIN64                               0x005A//      Плата DIN64 (DCPW v3.2) pic PIC18F4520 ПО DIN64
#define     _DIN64_BOOT                          0x005B//      Плата DIN64 v3.2 pic PIC18F4520 ПО DIN64 - загрузчие
#define     EM3FMv21_EM3Fv32                     0x005C//      платы EM3M v 2.1 и EM3 v 2.2 pic18F252- ПО- EM3F
#define     _EM3FMv21_EM3Fv32                    0x005D//      платы EM3M v 2.1 и EM3 v 2.2 pic18F2520- ПО- EM3F
#define     _EM3FMv21_EM3Fv32_BOOT               0x005E//      платы EM3M v 2.1 и EM3 v 2.2 pic18F2520-- загрузчиk (НЕ ИСП!)
#define     MDBPLC_AIN16Sv23                     0x005F//      плата MTPLC2 v 1.1  pic18F8520- ПО-MDBPLC
#define     BARRIER                              0x0060//      плата Ex_BarP8 v4.3 pic18F452 по BFRRIER
#define     BARRIER_BOOT                         0x0061//      плата Ex_BarP8 v4.3 pic18F452 по BFRRIER - загрузчик
#define     _BARRIER                             0x0062//      плата Ex_BarP8 v4.3 pic18F4520 по BFRRIER
#define     _BARRIER_BOOT                        0x0063//      плата Ex_BarP8 v4.3 pic18F4520 по BFRRIER - загрузчик
#define     AOUT1_V3_1                           0x0064//      плата AOUT1 v 3.1 pic18F252 ПО AOUT1
#define     AOUT1_V3_1_BOOT                      0x0065//      плата AOUT1 v 3.1 pic18F252 ПО AOUT2-загрузчик
#define     _AOUT1_V3_1                          0x0066//      плата AOUT1 v 3.1 pic18F2520 ПО AOUT1
#define     _AOUT1_V3_1_BOOT                     0x0067//      плата AOUT1 v 3.1 pic18F2520 ПО AOUT2-загрузчик
#define     _A9_SERV_PIC                         0x0068//      плата DECONT A9 V3.3 pic18LF2420 ПО A9_SERV
#define     _A9_SERV_PIC_BOOT                    0x0069//      плата DECONT A9 V3.3 pic18LF2420 ПО A9_SERV - загрузчик
#define     _R2IN6_V2_11                         0x006A//      плата R2IN6 v2.11 PIC18F2520 по R2IN6
#define     _R2IN6_V2_11_BOOT                    0x006B//      плата R2IN6 v2.11 PIC18F2520 по R2IN6 - загрузчик
#define     _A9_V_3_4_SERV_PIC                   0x006C//      плата DECONT A9 V3.4 pic18LF2420 ПО A9_SERV
#define     _A9_V_3_4_SERV_PIC_BOOT              0x006D//      плата DECONT A9 V3.4 pic18LF2420 ПО A9_SERV - загрузчик
#define     RZA44_V4_2                           0x006E//      плата RZA44 v 4.2,TMS320F2812 RZA 44
#define     RZA44_V4_2_BOOT                      0x006F//      плата RZA44 v 4.2,TMS320F2812 RZA 44 загрузчик
#define     ExEM2                                0x0070//      плата Ex_EM2 v1.0 TMS320F2811 ExEM2
#define     ExEM2_BOOT                           0x0071//      плата Ex_EM2 v1.0 TMS320F2811 ExEM2 загрузчик
#define     HUBSYBYS_MTPLC2_V1_1                 0x0072//      плата MTPLC2 V1.1 ПО HUBSYBYS
#define     DIO16_V1_0                           0x0073//      плата DIO16 V1.0 ПО DIO16
#define     DIO16_V1_0_BOOT                      0x0074//      плата DIO16 V1.0 ПО DIO16 загрузчик
#define     ExUPS_POWER                          0x0075//      плата ExUPS блока питания ПО ExUPSP pic18F6520
#define     ExUPS_POWER_BOOT                     0x0076//      плата ExUPS блока питания ПО ExUPSP pic18F6520 загрузчик
#define     _ExUPS_BAT                           0x0077//      плата ExUPS аккумулятора ПО ExUPSA pic18F2520
#define     _ExUPS_BAT_BOOT                      0x0078//      плата ExUPS аккумулятора ПО ExUPSA pic18F2520 загрузчик
#define     MTPLC2_BOOT                          0x0079//      плата MTPLC2 V1.1 - PIC18F8520 загрузчик
#define     _ExUPS_POWER                         0x007A//      плата ExUPS блока питания ПО ExUPSP pic18F6527
#define     _ExUPS_POWER_BOOT                    0x007B//      плата ExUPS блока питания ПО ExUPSP pic18F6527 загрузчик
#define     _EXDI8_EXDI8V32                      0x007C//      плата EXDI8 v3.2 PIC18F4520 ПО- EXDI8
#define     _EXDI8_EXDI8V32_BOOT                 0x007D//      плата EXDI8 v3.2 PIC18F4520 загрузчик
#define     PLM380_MTPLC2_V1_1                   0x007E//      плата MTPLC2 V1.1 PIC18F8520 ПО PLM380
#define     _MTPLC2_BOOT                         0x007F//      плата MTPLC2 V1.1 - PIC18F8527 загрузчик
#define     _EXDZ_EXDZV32                        0x0080//      плата ExDz v3.2 PIC18F2520 ПО- EXDZ
#define     PLM380_PLM380_V1_0                   0x0081//      плата PLM-380 v1.0 PIC18F8520 ПО- PLM380
#define     PLM380_PLM380_V1_0_BOOT              0x0082//      плата PLM-380 v1.0 PIC18F8520 загрузчик
#define     _EXLINE_EXLINEV32                    0x0083//      плата EXLINE v3.2 PIC18F2520 ПО- EXLINE
#define     _EXLINE_EXLINEV32_BOOT               0x0084//      плата EXLINE v3.2 PIC18F2520 ПО- EXLINE -загрузчик
#define     _EXR3I4_V3_3                         0x0085//      плата EXR3I4 v3.3 ,PIC18F2520, EXR3I4
#define     _EXR3I4_V3_3_BOOT                    0x0086//      плата EXR3I4 v3.3 ,PIC18F2520, загрузчик
#define     EXAI8U_EXAI8U60V1_0                  0x0087//      плата EXAI8U60 v1_0,PIC18F252 ПО- EXAI8U
#define     EXAI8U_EXAI8U60V1_0_BOOT             0x0088//      плата EXAI8U60 v1_0,PIC18F252 ПО- EXAI8U-загрузчик
#define     _EXDI26_EXDI2x6V1_0                  0x0089//      плата EXDI2x6 v1_0,PIC18F4520 ПО- EXDI26
#define     _EXDI26_EXDI2x6V1_0_BOOT             0x008A//      плата EXDI2x6 v1_0,PIC18F4520 ПО- EXDI26-загрузчик
#define     _ExIND                               0x008B//      плата EXIND v3.2, PIC18F2420 ПО ExINDIC
#define     _ExIND_BOOT                          0x008C//      плата EXIND v3.2, PIC18F2420 ПО ExINDIC - загручик
#define     _EXAI8U_EXAI8U60V1_0                 0x008D//      плата EXAI8U60 v1_0,PIC18F2520 ПО- EXAI8U
#define     _EXAI8U_EXAI8U60V1_0_BOOT            0x008E//      плата EXAI8U60 v1_0,PIC18F2520 ПО- EXAI8U-загрузчик
#define     EXAI4R_EXAI4RV4_0                    0x008F//      плата EXAI4R v4_0,PIC18F252 ПО- EXAI4R
#define     EXAI4R_EXAI4RV4_0_BOOT               0x0090//      плата EXAI4R v4_0,PIC18F252 ПО- EXAI4R-загрузчик
#define     _EXAI4R_EXAI4RV4_0                   0x0091//      плата EXAI4R v4_0,PIC18F2520 ПО- EXAI4R
#define     _EXAI4R_EXAI4RV4_0_BOOT              0x0092//      плата EXAI4R v4_0,PIC18F2520 ПО- EXAI4R-загрузчик
#define     Z_AIN6                               0x0093//      интерфейсный модуль AIN6, PIC18F2523
#define     Z_AIN6_BOOT                          0x0094//      интерфейсный модуль AIN6, PIC18F2523 ПО - загрузчик
#define     _IND_4                               0x0095//      плата IND v3.3, PIC18F2420 ПО INDIC
#define     _IND_4_BOOT                          0x0096//      плата IND v3.3, PIC18F2420 ПО INDIC - загручик
#define     _RTU2_v2_1                           0x0097//      плата RTU2 v 2.1,,PIC18F2520,ПО -RTU2
#define     _RTU2_v2_1_BOOT                      0x0098//      плата RTU2 v 2.1,,PIC18F2520,загрузчик
#define     ARROW_MONITOR                        0x0099//      Плата MS2 PIC18F252 монитор стрелок
#define     ARROW_MONITOR_BOOT                   0x009A//      Плата MS2 PIC18F252 монитор стрелок загрузчик
#define     _ARROW_MONITOR                       0x009B//      Плата MS2 PIC18F2520 монитор стрелок
#define     _ARROW_MONITOR_BOOT                  0x009C//      Плата MS2 PIC18F2520 монитор стрелок загрузчик
#define     PLX_v5_2_6SYMB                       0x009D//      Плата PLX v 5.2 6 символов PIC18F452 ПО -PLX
#define     PLX_v5_2_6SYMB_BOOT                  0x009E//      Плата PLX v 5.2 6 символов PIC18F452 ПО -PLX-загрузчик
#define     _PLX_v5_2_6SYMB                      0x009F//      Плата PLX v 5.2 6 символов PIC18F4520 ПО -PLX
#define     _PLX_v5_2_6SYMB_BOOT                 0x00A0//      Плата PLX v 5.2 6 символов PIC18F4520 ПО -PLX-загрузчик
#define     _INTERCOM_v2_0                       0x00A1//      Плата ExCom v 2.0 ,PIC18LF2520 ПО INTERCOM
#define     _INTERCOM_v2_0_BOOT                  0x00A2//      Плата ExCom v 2.0 ,PIC18LF2520 ПО INTERCOM -загрузчик
#define     _RTU3_SLAVE                          0x00A3//      Плата DIOMX v1.1 ведомый процессор PIC18F2523
#define     _RTU3_SLAVE_BOOT                     0x00A4//      Плата DIOMX v1.1 ведомый процессор PIC18F2523 загрузчик
#define     _RTU3_MASTER                         0x00A5//      Плата DIOMX v1.1 ведущий процессор PIC18F6527
#define     _RTU3_MASTER_BOOT                    0x00A6//      Плата DIOMX v1.1 ведущий процессор PIC18F6527 загрузчик
#define     _LAB_PW                              0x00A7//      Стендовый блок питания процессор PIC18F6527
#define     _LAB_PW_BOOT                         0x00A8//      Стендовый блок питания процессор PIC18F6527 загрузчик
#define     _TEMPL_RZA44                         0x00A9//      Плата RZA44 v4.2процессор TMS320F2812 ПО- TEMPLATE
#define     _TEMPL_RZA44_BOOT                    0x00AA//      Плата RZA44 v4.2процессор TMS320F2812 -загрузчик
#define     _ExDo3                               0x00AB//      Плата ExDO3-T50 v1.0 процессор PIC18F2420
#define     _ExDo3_BOOT                          0x00AC//      Плата ExDO3-T50 v1.0 процессор PIC18F2420 - загрузчик
#define     _HUBSYBYS_MTPLC2_V1_1                0x00AD//      плата MTPLC2 V1.1 процессор PIC8527 ПО HUBSYBYS
#define     _ExDin8                              0x00AE//      плата ExNMR8 V1.1 процессор PIC18F2520 ПО ExNMR8
#define     _ExDin8_BOOT                         0x00AF//      плата ExNMR8 V1.1 процессор PIC18F2520  - загрузчик
#define     DIN8_GI                              0x00B0//      плата DIN8-220 v1.00 процессор PIC18F452 ПО DIN8
#define     DIN8_GI_BOOT                         0x00B1//      плата DIN8-220 v1.00 процессор PIC18F452 - загрузчик
#define     _DIN8_GI                             0x00B2//      плата DIN8-220 v1.00 процессор PIC18F4520 ПО DIN8
#define     _DIN8_GI_BOOT                        0x00B3//      плата DIN8-220 v1.00 процессор PIC18F4520 - загрузчик
#define     _ExBarCU4                            0x00B4//      плата ExBAR-CU4 v10 процессор PIC18F2520
#define     _ExBarCU4_BOOT                       0x00B5//      плата ExBAR-CU4 v10 процессор PIC18F2520 - загрузчик
#define     _ALINE_ALINE_V2_2                    0x00B6//      плата ALINE v2.2 PIC18F2520 ПО- ALINE
#define     _ALINE_ALINE_V2_3_BOOT               0x00B7//      плата ALINE v2.2 PIC18F2520 ПО- ALINE  - загрузчик
#define     EXLIN2_Exline_V3_4                   0x00B8//      плата Exline v 3.4 PIC18F252 ПО-EXLIN2
#define     EXLIN2_Exline_V3_4_BOOT              0x00B9//      плата Exline v 3.4 PIC18F252 ПО-EXLIN2 - загрузчик
#define     _EXLIN2_Exline_V3_4                  0x00BA//      плата Exline v 3.4 PIC18F2520 ПО-EXLIN2
#define     _EXLIN2_Exline_V3_4_BOOT             0x00BB//      плата Exline v 3.4 PIC18F2520 ПО-EXLIN2 - загрузчик
#define     Ex485fm                              0x00BC//      Плата Ex485fm pic 18f2523
#define     Ex485fm_BOOT                         0x00BD//      Плата Ex485fm pic 18f2523 загрузчик
#define     _RTU3_D0_5A_MASTER                   0x00BE//      Плата RTU3 v2.3 DO-5A ведущий процессор PIC18F6527
#define     _RTU3_D0_5A_MASTER_BOOT              0x00BF//      Плата RTU3 v2.3 DO-5A ведущий процессор PIC18F6527 загрузчик
#define     _ExAI4                               0x00C0//      плата ExAI4 v1.0 активный PIC18F2523
#define     _ExAI4_BOOT                          0x00C1//      плата ExAI4 v1.0 активный PIC18F2523 загрузчик
#define     _RZAPAN                              0x00C2//      плата RZAPAN v4.1 pic18f4520
#define     _RZAPAN_BOOT                         0x00C3//      плата RZAPAN v4.1 pic18f4520 загрузчик
#define     RZAPAN                               0x00C4//      плата RZAPAN v4.1 pic18f452
#define     RZAPAN_BOOT                          0x00C5//      плата RZAPAN v4.1 pic18f452 загрузчик
#define     _EXCOM_V4_1                          0x00C6//      Плата ExCom v 4.1 ,PIC18F45K20 ПО EXCOM
#define     _EXCOM_V4_1_BOOT                     0x00C7//      Плата ExCom v 4.1 ,PIC18F45K20 загрузчик
#define     _A9_Ex_SERV_PIC                      0x00C8//      Плата ExA9cpu V1.2 или новый A9 (конец 2009 года) pic18LF2420 ПО A9_SERV
#define     _EXCOM_V4_2                          0x00C9//      Плата ExCom v 4.2 ,PIC18F45K20 ПО EXCOM
#define     _EXCOM_V4_2_BOOT                     0x00CA//      Плата ExCom v 4.2 ,PIC18F45K20 загрузчик
#define     _ExR2In8                             0x00CB//      Плата ExR2IN8-v1.0 PIC18F2520 ПО ExR2IN8
#define     _ExR2In8_BOOT                        0x00CC//      Плата ExR2IN8-v1.0 PIC18F2520 ПО ExR2IN8 загрузчик
#define     _EXAI4P                              0x00CD//      Плата ExAI4-P20-v1.0 PIC18F2523 ПО ExAI4P
#define     _EXAI4P_BOOT                         0x00CE//      Плата ExAI4-P20-v1.0 PIC18F2523 ПО ExAI4P загрузчик
#define     MTPLC2_PLM380_V2_0                   0x00CF//      плата MTPLC2 V2.0 PIC18F8520 ПО MTPPLC2 и PLM380
#define     Ex_A9_SERV_PIC                       0x00D0//      Плата ExA9cpu V2.0 pic18LF2420 ПО A9_SERV
#define     Ex_A9_SERV_PIC_BOOT                  0x00D1//      Плата ExA9cpu V2.0 pic18LF2420 ПО A9_SERV загрузчик
#define     ExHB5                                0x00D2//      Плата ExHB5-0_v10 pic18LF2420 ПО ExHB5
#define     ExHB5_BOOT                           0x00D3//      Плата ExHB5-0_v10 pic18LF2420 ПО ExHB5 загрузчик
#define     _EXCOM_V4_2_46K20                    0x00D4//      Плата ExCom v 4.2 ,PIC18F46K20 ПО EXCOM
#define     _EXCOM_V4_2_46K20_BOOT               0x00D5//      Плата ExCom v 4.2 ,PIC18F46K20 загрузчик
#define     EXLINED4_V11                         0x00D6//      Плата ExLineD4 v11 PIC18F252 ПО ExLineD4
#define     EXLINED4_V11_BOOT                    0x00D7//      Плата ExLineD4 v11 PIC18F252 загрузчик
#define     _EXLINED4_V11                        0x00D8//      Плата ExLineD4 v11 PIC18F2520 ПО ExLineD4
#define     _EXLINED4_V11_BOOT                   0x00D9//      Плата ExLineD4 v11 PIC18F2520 загрузчик
#define     _EXCOM_V4_3_45K20                    0x00DA//      Плата ExCom v 4.3 ,PIC18F45K20 ПО EXCOM
#define     _EXCOM_V4_3_46K20                    0x00DB//      Плата ExCom v 4.3 ,PIC18F46K20 ПО EXCOM
#define     Ex485FMI                             0x00DC//      Плата Ex485FMI v 4.5 ,PIC18F2523 ПО Ex485FMI
#define     Ex485FMI_BOOT                        0x00DD//      Плата Ex485FMI v 4.5 ,PIC18F2523 ПО Ex485FMI загрузчик
#define     PIC_RZA_V5                           0x00DE//      Плата RZA_DSP V5.0 pic18LF2420 ПО pic_RZA5
#define     PIC_RZA_V5_BOOT                      0x00DF//      Плата RZA_DSP V5.0 pic18LF2420 ПО pic_RZA5 загрузчик
#define     AIN8_V4_0                            0x00E0//      Плата Ain8 v4.0 Pic18F2520 ПО AIN8
#define     AIN8_V4_0_BOOT                       0x00E1//      Плата  Ain8 v4.0 Pic18F2520 ПО AIN8 загрузчик
#define     RTU3_D0_5A_MASTER                    0x00E2//      Плата RTU3 v2.3 DO-5A ведущий процессор PIC18F6520
#define     RTU3_D0_5A_MASTER_BOOT               0x00E3//      Плата RTU3 v2.3 DO-5A ведущий процессор PIC18F6520 загрузчик
#define     RZA_5                                0x00E4//      Плата RZA_DSP v5.0 TMS320F2812
#define     RZA_5_BOOT                           0x00E5//      Плата RZA_DSP v5.0 TMS320F2812 загрузчик
#define     _AIN8_V4_0                           0x00E6//      Плата Ain8 v4.0 Pic18F252 ПО AIN8
#define     _AIN8_V4_0_BOOT                      0x00E7//      Плата  Ain8 v4.0 Pic18F252 ПО AIN8 загрузчик
#define     _EXDI8_V3_4                          0x00E8//      плата EXDI8 v3.4 PIC18F4520 ПО- EXDI8
#define     _EXDI8_V3_4_BOOT                     0x00E9//      плата EXDI8 v3.4 PIC18F4520 ПО- EXDI8 загрузчик
#define     R3IN6_V2_4                           0x00EA//      плата R3IN6 v2.4 PIC18F252 ПО- R3IN6
#define     R3IN6_V2_4_BOOT                      0x00EB//      плата R3IN6 v2.4 PIC18F252 ПО- R3IN6  загрузчик
#define     _R3IN6_V2_4                          0x00EC//      плата R3IN6 v2.4 PIC18F2520 ПО- R3IN6
#define     _R3IN6_V2_4_BOOT                     0x00ED//      плата R3IN6 v2.4 PIC18F2520 ПО- R3IN6  загрузчик
#define     DSW505T                              0x00EE//      плата DSW505T v1.3 Pic18LF2420
#define     DSW505T_BOOT                         0x00EF//      плата DSW505T v1.3 Pic18LF2420 загрузчик
#define     EM3_V30                              0x00F0//      плата EM3 v3.0 PIC18F25K20 ПО EM3M
#define     EM3_V30_BOOT                         0x00F1//      плата EM3 v3.0 PIC18F25K20 ПО EM3M загрузчик
#define     _EM3_V30                             0x00F2//      плата EM3 v3.0 PIC18F26K20 ПО EM3M
#define     _EM3_V30_BOOT                        0x00F3//      плата EM3 v3.0 PIC18F26K20 ПО EM3M загрузчик
#define     A9_V_3_4_SERV_2xK20                  0x00F4//      плата DECONT A9 V3.4-V4.4 pic18F25K20 ПО A9_SERV
#define     A9_V_3_4_SERV_2xK20_BOOT             0x00F5//      плата DECONT A9 V3.4-V4.4 pic18F25K20 ПО A9_SERV загрузчик
#define     A9_V_4_6_SERV_2xK20                  0x00F6//      Плата DECONT A9 V4.6 и выше или ExA9cpu V1.2 pic25K20 ПО A9_SERV
#define     Ex_A9_SERV_2xK20                     0x00F7//      Плата ExA9cpu V2.0 pic18F25K20 ПО A9_SERV
#define     Ex_A9_SERV_2xK20_BOOT                0x00F8//      Плата ExA9cpu V2.0 pic18F25K20 ПО A9_SERV загрузчик
#define     ExPNL                                0x00F9//      Плата ExDP3-IO16 v1.1 pic18F67k22
#define     ExPNL_BOOT                           0x00FA//      Плата ExDP3-IO16 v1.1 pic18F67k22 boot
#define     _ExDP3_IO16                          0x00FB//      Плата ExDP3-IO16 v1.0 pic18F6527
#define     _ExDP3_IO16_BOOT                     0x00FC//      Плата ExDP3-IO16 v1.0 pic18F6527 boot
#define     SRV_RPR485T3                         0x00FD//      Плата RPR485T3 v1.1 Pic18F25K20 ПО MNGRPT
#define     SRV_RPR485T3_BOOT                    0x00FE//      Плата RPR485T3 v1.1 Pic18F25K20 ПО MNGRPT-загрузчик
#define     _SRV_RPR485T3                        0x00FF//      Плата RPR485T3 v1.1 Pic18F26K20 ПО MNGRPT
#define     _SRV_RPR485T3_BOOT                   0x0100//      Плата RPR485T3 v1.1 Pic18F26K20 ПО MNGRPT-загрузчик
#define     ExPrg_V11                            0x0101//      Плата ExPrg v1.1 Pic18F6520 ПО ExPrg
#define     ExPrg_V11_BOOT                       0x0102//      Плата ExPrg v1.1 Pic18F6520 ПО ExPrg -загрузчик
#define     _ExPrg_V11                           0x0103//      Плата ExPrg v1.1 Pic18F6527 ПО ExPrg
#define     _ExPrg_V11_BOOT                      0x0104//      Плата ExPrg v1.1 Pic18F6527 ПО ExPrg -загрузчик
#define     A9_V_3_4_SERV_26K20                  0x0105//      плата DECONT A9 V3.4-V4.4 pic18F26K20 ПО A9_SERV
#define     A9_V_3_4_SERV_26K20_BOOT             0x0106//      плата DECONT A9 V3.4-V4.4 pic18F26K20 ПО A9_SERV загрузчик
#define     A9_V_4_6_SERV_26K20                  0x0107//      Плата DECONT A9 V4.6 и выше или ExA9cpu V1.2 pic26K20 ПО A9_SERV
#define     Ex_A9_SERV_26K20                     0x0108//      Плата ExA9cpu V2.0 pic18F26K20 ПО A9_SERV
#define     Ex_A9_SERV_26K20_BOOT                0x0109//      Плата ExA9cpu V2.0 pic18F26K20 ПО A9_SERV загрузчик
#define     ExCom_Charger                        0x010A//      Плата EXCHRG V1.0 pic18f4520 ПО EXCHRG
#define     ExCom_Charger_BOOT                   0x010B//      Плата EXCHRG V1.0 pic18f4520 ПО EXCHRG - загрузчик
#define     TCS3D8                               0x010C//      Плата TCS3D8 V1.0 PIC18F452 ПО TCS2D8
#define     TCS3D8_BOOT                          0x010D//      Плата TCS3D8 V1.0 PIC18F452 ПО TCS2D8 - загрузчик
#define     _TCS3D8                              0x010E//      Плата TCS3D8 V1.0 PIC18F4520 ПО TCS2D8
#define     _TCS3D8_BOOT                         0x010F//      Плата TCS3D8 V1.0 PIC18F4520 ПО TCS2D8 - загрузчик
#define     TMCT4                                0x0110//      Плата T-MCT4 v4.1 PIC18LF2523 ПО TMCT4
#define     TMCT4_BOOT                           0x0111//      Плата T-MCT4 v4.1 PIC18LF2523 ПО TMCT4 - загрузчик
#define     EXBSC                                0x0112//      Плата ExBSC v1.0 PIC18F2520 ПО EXBSC
#define     EXBSC_BOOT                           0x0113//      Плата ExBSC v1.0 PIC18F2520 ПО EXBSC - загрузчик
#define     TEMPL_RTU3                           0x0114//      Плата RTU3-CPU v3.2 PIC18F67K22 ПО TEMPLATE
#define     TEMPL_RTU3_BOOT                      0x0115//      Плата RTU3-CPU v3.2 PIC18F67K22 ПО TEMPLATE-загрузчик
#define     RTU_9                                0x0116//      Плата RTU9-U100 v1.0 ПО RTU9
#define     RTU_9_BOOT                           0x0117//      Плата RTU9-U100 v1.0 ПО RTU9 - загрузчик
#define     SSI4                                 0x0118//      Плата SSI4 v1.0 PIC18F252 ПО SSI4
#define     SSI4_BOOT                            0x0119//      Плата SSI4 v1.0 PIC18F252 ПО SSI4 - загрузчик
#define     _SSI4                                0x011A//      Плата SSI4 v1.0 PIC18F2520 ПО SSI4
#define     _SSI4_BOOT                           0x011B//      Плата SSI4 v1.0 PIC18F2520 ПО SSI4 - загрузчик
#define     RTU3_M                               0x011C//      Плата RTU3-CPU v3.2 PIC18F67K22 ПО RTU3_M
#define     RTU3_M_BOOT                          0x011D//      Плата RTU3-CPU v3.2 PIC18F67K22 ПО RTU3_M - загрузчик
#define     TCS3D8_V20                           0x011E//      Плата TCS3D8 V2.0 PIC18F452 ПО TCS2D8
#define     TCS3D8_V20_BOOT                      0x011F//      Плата TCS3D8 V2.0 PIC18F452 ПО TCS2D8 - загрузчик
#define     _TCS3D8_V20                          0x0120//      Плата TCS3D8 V2.0 PIC18F4520 ПО TCS2D8
#define     _TCS3D8_V20_BOOT                     0x0121//      Плата TCS3D8 V2.0 PIC18F4520 ПО TCS2D8 - загрузчик
#define     ExML                                 0x0122//      Плата ExML V1.1 PIC18F252 ПО ExML
#define     ExML_BOOT                            0x0123//      Плата ExML V1.1 PIC18F252 ПО ExML - загрузчик
#define     _ExML                                0x0124//      Плата ExML V1.1 PIC18F2520 ПО ExML
#define     _ExML_BOOT                           0x0125//      Плата ExML V1.1 PIC18F2520 ПО ExML - загрузчик
#define     SSI4_IMIT                            0x0126//      Плата имитатора энкодера для SSI4 на базе SSI4 V1.0 PIC18F252 ПО SSI4_IMIT
#define     SSI4_IMIT_BOOT                       0x0127//      Плата имитатора энкодера для SSI4 на базе SSI4 V1.0 PIC18F252 ПО SSI4_IMIT - загрузчик
#define     _SSI4_IMIT                           0x0128//      Плата имитатора энкодера для SSI4 на базе SSI4 V1.0 PIC18F2520 ПО SSI4_IMIT
#define     _SSI4_IMIT_BOOT                      0x0129//      Плата имитатора энкодера для SSI4 на базе SSI4 V1.0 PIC18F2520 ПО SSI4_IMIT - загрузчик
#define     ExEM2_2812                           0x012A//      плата Ex_EM2 v1.0 TMS320F2812 ExEM2
#define     ExEM2_2812_BOOT                      0x012B//      плата Ex_EM2 v1.0 TMS320F2812 ExEM2 загрузчик
#define     _ExHB5_25K20                         0x012C//      Плата ExHB5-0_v10 pic18F25K20 ПО ExHB5
#define     _ExHB5_25K20_BOOT                    0x012D//      Плата ExHB5-0_v10 pic18F25K20 ПО ExHB5 загрузчик
#define     _DSW505T_25K20                       0x012E//      плата DSW505T v1.3 Pic18F25K20
#define     _DSW505T_25K20_BOOT                  0x012F//      плата DSW505T v1.3 Pic18F25K20 загрузчик
#define     _ExHB5_26K20                         0x0130//      Плата ExHB5-0_v10 pic18F26K20 ПО ExHB5
#define     _ExHB5_26K20_BOOT                    0x0131//      Плата ExHB5-0_v10 pic18F26K20 ПО ExHB5 загрузчик
#define     _DSW505T_26K20                       0x0132//      плата DSW505T v1.3 Pic18F26K20
#define     _DSW505T_26K20_BOOT                  0x0133//      плата DSW505T v1.3 Pic18LF26K20 загрузчик
#define     DLOCK_V1_0                           0x0134//      Плата DLOCK v1.0 PIC18F252
#define     DLOCK_V1_0_BOOT                      0x0135//      Плата DLOCK v1.0 PIC18F252 - загрузчик
#define     _DLOCK_V1_0                          0x0136//      Плата DLOCK v1.0 PIC18F2520
#define     _DLOCK_V1_0_BOOT                     0x0137//      Плата DLOCK v1.0 PIC18F2520 - загрузчик
#define     _ExIND_2520                          0x0138//      плата EXIND v3.2, PIC18F2520 ПО ExINDIC
#define     _ExIND_BOOT_2520                     0x0139//      плата EXIND v3.2, PIC18F2520 ПО ExINDIC - загрузчик
#define     _IND_4_2520                          0x013A//      плата IND v3.3, PIC18F2520 ПО INDIC
#define     _IND_4_BOOT_2520                     0x013B//      плата IND v3.3, PIC18F2520 ПО INDIC - загрузчик
#define     _EM3_V47                             0x013C//      плата EM3 v4.7 PIC18F26K20 ПО EM3M
#define     _EM3_V47_BOOT                        0x013D//      плата EM3 v4.7 PIC18F26K20 ПО EM3M- загрузчик
#define     TDIN8_220_V1_0                       0x013E//      Плата TDIN8_220 V1.0 PIC18F452 ПО TDIN8_220
#define     TDIN8_220_V1_0_BOOT                  0x013F//      Плата TDIN8_220 V1.0 PIC18F452 ПО TDIN8_220 - загрузчик
#define     _TDIN8_220_V1_0                      0x0140//      Плата TDIN8_220 V1.0 PIC18F4520 ПО TDIN8_220
#define     _TDIN8_220_V1_0_BOOT                 0x0141//      Плата TDIN8_220 V1.0 PIC18F4520 ПО TDIN8_220 - загрузчик
#define     TDIN16_V12                           0x0142//      Плата TDIN16 V1.2 PIC18F252 ПО DIN16U (не исп)
#define     _TDIN16_V12                          0x0143//      Плата TDIN16 V1.2 PIC18F2520 ПО DIN16U (не исп)
#define     ExVBR                                0x0144//      Плата ExVBR V1.0 PIC18F46K20 ПО ExVBR
#define     ExVBR_BOOT                           0x0145//      Плата ExVBR V1.0 PIC18F46K20 ПО ExVBR - загрузчик
#define     EXAI                                 0x0146//      Плата ExAI V1.0 PIC18F2523 ПО EXAI
#define     EXAI_BOOT                            0x0147//      Плата ExAI V1.0 PIC18F2523 ПО EXAI - загрузчик
#define     _EXAI                                0x0148//      Плата ExAI V1.0 PIC18FXXXX ПО EXAI
#define     _EXAI_BOOT                           0x0149//      Плата ExAI V1.0 PIC18FXXXX ПО EXAI - загрузчик
#define     _ExHB5_SPI_FIX_25K20                 0x014A//      Плата ExHB5-0_v1.3 pic18F25K20 ПО ExHB5
#define     _ExHB5_SPI_FIX_26K20                 0x014B//      Плата ExHB5-0_v1.3 pic18F26K20 ПО ExHB5
#define     ExHB5_SPI_FIX                        0x014C//      Плата ExHB5-0_v1.3 pic18LF2420 ПО ExHB5
#define     TDIN16F_V12                          0x014D//      Плата TDIN16 V1.2 PIC18F2620 ПО DIN16U
#define     TDIN16F_V12_BOOT                     0x014E//      Плата TDIN16 V1.2 PIC18F2620 ПО DIN16U - загрузчик
#define     DIN16A_V17                           0x014F//      плата DIN16 v1.7 ,PIC18F2620, ПО - DIN16U
#define     DIN16A_V17_BOOT                      0x0150//      плата DIN16 v1.7 ,PIC18F2620, ПО - DIN16U- загрузчик
#define     DIN16W_V17                           0x0151//      плата DIN16 v1.7 ,PIC18F26K22, ПО - DIN16W
#define     DIN16W_V17_BOOT                      0x0152//      плата DIN16 v1.7 ,PIC18F26K22, ПО - DIN16W- загрузчик
#define     TDIN16W_V12                          0x0153//      Плата TDIN16 V1.2 PIC18F26K22 ПО DIN16W
#define     TDIN16W_V12_BOOT                     0x0154//      Плата TDIN16 V1.2 PIC18F26K22 ПО DIN16W - загрузчик
#define     TDIN8                                0x0155//      Плата TDIN8 PIC18F452 ПО TDIN8
#define     TDIN8_BOOT                           0x0156//      Плата TDIN8 PIC18F452 ПО TDIN8 - загрузчик
#define     _TDIN8                               0x0157//      Плата TDIN8 PIC18F4520 ПО TDIN8
#define     _TDIN8_BOOT                          0x0158//      Плата TDIN8 PIC18F4520 ПО TDIN8 - загрузчик
#define     DOUT64_T80_46K20                     0x0159//      Плата SR100-v1.0 PIC18F46K20 ПО DOUT64
#define     DOUT64_T80_46K20_BOOT                0x015A//      Плата SR100-v1.0 PIC18F46K20 ПО DOUT64 загрузчик
#define     DIN64_46K20                          0x015B//      Плата SR100-v1.0 PIC18F46K20 ПО DIN64
#define     DIN64_46K20_BOOT                     0x015C//      Плата SR100-v1.0 PIC18F46K20 ПО DIN64 загрузчик
#define     RTU6                                 0x015D//      Плата RTU6 v1.1  PIC18F67K22  ПО RTU6
#define     RTU6_BOOT                            0x015E//      Плата RTU6 v1.1  PIC18F67K22  ПО RTU6 загрузчик
#define     RPR485F2                             0x015F//      Плата RPR485F2 V1.1 PIC18F25K20 ПО RPR485F2
#define     RPR485F2_BOOT                        0x0160//      Плата RPR485F2 V1.1 PIC18F25K20 ПО RPR485F2 - загрузчик
#define     _RPR485F2                            0x0161//      Плата RPR485F2 V1.1 PIC18F26K20 ПО RPR485F2
#define     _RPR485F2_BOOT                       0x0162//      Плата RPR485F2 V1.1 PIC18F26K20 ПО RPR485F2 - загрузчик
#define     TDIN8_220_S                          0x0163//      Плата TDIN8_220_S V1.0 PIC18F45K22 ПО TDIN8_220_S
#define     TDIN8_220_S_BOOT                     0x0164//      Плата TDIN8_220_S V1.0 PIC18F45K22 ПО TDIN8_220_S - загрузчик
#define     _TDIN8_220_S                         0x0165//      Плата TDIN8_220_S V1.0 PIC18F46K22 ПО TDIN8_220_S
#define     _TDIN8_220_S_BOOT                    0x0166//      Плата TDIN8_220_S V1.0 PIC18F46K22 ПО TDIN8_220_S - загрузчик
#define     TR3IN8                               0x0167//      Плата TR3IN8 - PIC18F25K22 - ПО TR3IN8
#define     TR3IN8_BOOT                          0x0168//      Плата TR3IN8 - PIC18F25K22 - ПО TR3IN8 - загрузчик
#define     _TR3IN8                              0x0169//      Плата TR3IN8 - PIC18F26K22 - ПО TR3IN8
#define     _TR3IN8_BOOT                         0x016A//      Плата TR3IN8 - PIC18F26K22 - ПО TR3IN8 - загрузчик
#define     ExDLOCK                              0x016B//      Плата ExDLOCK - PIC18F25K22 - ПО ExDLOCK
#define     ExDLOCK_BOOT                         0x016C//      Плата ExDLOCK - PIC18F25K22 - ПО ExDLOCK - загрузчик
#define     _TDIN8_220_M                         0x016D//      Плата TDIN8_220VDC-FSK V12 PIC18F26K22 ПО TDIN8_220_M
#define     _TDIN8_220_M_BOOT                    0x016E//      Плата TDIN8_220VDC-FSK V12 PIC18F26K22 ПО TDIN8_220_M - загрузчик
#define     TCS3D8_46K22                         0x016F//      Плата TCS3D8 V2.2 PIC18F46K22 ПО CS3D8
#define     TCS3D8_46K22_BOOT                    0x0170//      Плата TCS3D8 V2.2 PIC18F46K22 ПО CS3D8 - загрузчик
#define     TCS3D8A1_46K22                       0x0171//      Плата TCS3D8-A1 - PIC18F46K22 ПО CS3D8-A1
#define     TCS3D8A1_46K22_BOOT                  0x0172//      Плата TCS3D8-A1 - PIC18F46K22 ПО CS3D8-A1 - загрузчик
#define     _ExDLOCK                             0x0173//      Плата ExDLOCK - PIC18F26K22 - ПО ExDLOCK
#define     _ExDLOCK_BOOT                        0x0174//      Плата ExDLOCK - PIC18F26K22 - ПО ExDLOCK - загрузчик
#define     EM_4                                 0x0175//      Плата EM4 - PIC18F67K22 - ПО EM_4
#define     EM_4_BOOT                            0x0176//      Плата EM4 - PIC18F67K22 - ПО EM_4 -загрузчик
#define     PPVV                                 0x0177//      Плата PPVV-STM32F207  - ПО PPVV
#define     PPVV_BOOT                            0x0178//      Плата PPVV-STM32F207  - ПО PPVV-загрузчик
#define     RTU3MM                               0x0179//      Плата RTU3M-STM32F427 -ПО RTU3MM
#define     RTU3MM_BOOT                          0x017A//      Плата RTU3M-STM32F427 -ПО RTU3MM-загрузчик
#define     _EX_BARI                             0x017B//      Плата ExPW24-BARI-V10 - ПО ExBARI
#define     _EX_BARI_BOOT                        0x017C//      Плата ExPW24-BARI-V10 - ПО ExBARI - загрузчик
#define     DEPRTU11                             0x017D//      Плата DSUCPUFX-V14 -ПО DEPRTU11
#define     DEPRTU11_BOOT                        0x017E//      Плата DSUCPUFX-V14 -ПО DEPRTU11 - загрузчик
#define     DSU_4I5P_4IS02                       0x017F//      Плата модуля с AFE 4 тока 0,2S + 4 тока 5P
#define     DSU_4IS02_4IS02                      0x0180//      Плата модуля с AFE 4 тока 0,2S + 4 тока 0,2S
#define     DSU_4I5P_4I5P                        0x0181//      Плата модуля с AFE 4 тока 5P + 4 тока 5P
#define     DSU_4U_4IS02                         0x0182//      Плата модуля с AFE 4 напряжения 0.2s + 4 тока 0.2s
#define     DSU_4U_4I5P                          0x0183//      Плата модуля с AFE 4 напряжения 0.2s + 4 тока 5P
#define     DSU_4IS02                            0x0184//      Плата модуля с AFE 4 тока 0,2S
#define     DSU_4I5P                             0x0185//      Плата модуля с AFE 4 тока  5P
#define     DSU_4U                               0x0186//      Плата модуля с AFE 4 напряжения 0.2s
#define     DSU_12DI220R                         0x0187//      Плата модуля 12 дискретных входов 220в с режекцией
#define     DSU_12DO1_220                        0x0188//      Плата модуля 12 дискретных выходов 220в 1A
#define     DSU_2DO5C_4DO1_220                   0x0189//      Плата модуля 2 выхода 5А 4 выхода 2А, с контролем
#define     DSU_CPUAFE                           0x0190//      Плата AFE с процессором
#define     DSU_CPUAFE_BOOT                      0x0191//      Плата AFE с процессором - загрузчик
#define     DEPRTU11_TX                          0x0192//      Плата DSUCPUTX-V14 -ПО DEPRTU11
#define     EX_PRW_IND                           0x0193//      Плата EX_PRW_IND STM32F205 ПО EX_PRW_IND
#define     EX_PRW_IND_BOOT                      0x0194//      Плата EX_PRW_IND STM32F205 ПО EX_PRW_IND-загрузчик
#define     EX_RPW_CPU                           0x0195//      Плата ExRPW-CPU STM32F205 ПО EX_PRW_CPU
#define     EX_RPW_CPU_BOOT                      0x0196//      Плата ExRPW-CPU STM32F205 ПО EX_PRW_CPU-загрузчик
#define     DEPRTU_TX_QUAL                       0x0197//      Плата DSUCPUTX-V22 -ПО DEPRTU_PKE
#define     DEPRTU_FX_QUAL                       0x0198//      Плата DSUCPUFX-V22 -ПО DEPRTU_PKE
#define     RELTER_FX                            0x0199//      Плата RELTER_v10 ПО RELTER (защита БДВ 835V для метро)
#define     RELTER_FX_BOOT                       0x019A//      Плата RELTER_v10 ПО RELTER (защита БДВ 835V для метро)- загрузчик
#define     DSU_PW220                            0x019B//      Блок питания 220в
#define     DSU_PW24                             0x019C//      Блок питания 24в
#define     PPTT_4I5P                            0x019D//      Плата ППТТ 4I5P v12
#define     PPTT_4I5P_BOOT                       0x019E//      Плата ППТТ 4I5P загрузчик
#define     DEPRTU_TX                            0x019F//      Плата DSUCPUTX-V17 -ПО depRTU
#define     DEPRTU_FX                            0x01A0//      Плата DSUCPUFX-V17 -ПО depRTU
#define     A10_ETH8RS                           0x01A1//      Плата A10-8RS-ETH
#define     A10_ETH8RS_BOOT                      0x01A2//      Плата A10-8RS-ETH - загрузчик
#define     ETH6RS                               0x01A3//      Плата M485x6
#define     ETH6RS_BOOT                          0x01A4//      Плата M485x6 - загрузчик
#define     PNL4_D16                             0x01A5//      Плата PNL4-D16
#define     PNL4_D16_BOOT                        0x01A6//      Плата PNL4-D16 - загрузчик
#define     DSU_16DI_24                          0x01A7//      Плата модуля 16 дискретных входов 24в
#define     RZA_4U_4I_CPUAFE                     0x01A8//      Плата DSU-CPU-FX-V22 -ПО RZA_4U_4I (РЗА 4 тока 4 напряжения интелектуальня плата AFE)
#define     TEMPL_DEPRTU_TX                      0x01A9//      Плата DSU-CPU-TX-V22   шаблон для процессорного модуля
#define     TEMPL_DEPRTU_FX                      0x01AA//      Плата DSU-CPU-FX-V22   шаблон для процессорного модуля
#define     TEMPL_DEPRTU_BOOT                    0x01AB//      Плата DSU-CPU-TX-FX-V22  загрузчик
#define     TDOUT8_R07_26K22                     0x01AC//      Плата TDOUT8-R07 PIC18F26K22 ПО - DOUT8
#define     TDOUT8_R07_26K22_BOOT                0x01AD//      Плата TDOUT8-R07 PIC18F26K22 ПО - DOUT8 - загрузчик
#define     DRZA_PW_220                          0x01AE//      Плата DRZA_PW-220
#define     DRZA_AFE                             0x01AF//      Плата DRZA_AFE
#define     DRZA_DIO_220                         0x01B0//      Плата DRZA_DIO_220
#define     DRZA_ETH                             0x01B1//      DRZA_ETH
#define     DEPRTU_TX_v22                        0x01B2//      Плата DSUCPUTX-V22 -ПО depRTU
#define     DEPRTU_FX_v22                        0x01B3//      Плата DSUCPUFX-V22 -ПО depRTU
#define     DEPRTU_FXTX_v22_BOOT                 0x01B4//      Плата DSUCPUFX_TX-V22 - загрузчик
#define     DSC_12DI_110                         0x01B5//      Плата DSC_12DI_110 и ПО
#define     DSC_2DOSC4DO1C4DO1                   0x01B6//      Плата DSC_2DOSC4DO1C4DO1 и ПО
#define     DSU_10DOR                            0x01B7//      Плата DSU_10DOR
#define     RZA_4U_4I_CPUAFE_TX                  0x01B8//      Плата DSU-CPU-TX-V22 -ПО RZA_4U_4I (РЗА 4 тока 4 напряжения интелектуальня плата AFE)
#define     EXHB2RS4                             0x01B9//      Плата ExHB2RS4
#define     EXHB2RS4_BOOT                        0x01BA//      Плата ExHB2RS4 - загрузчик
#define     HB2RS8                               0x01BB//      Плата HB2RS8
#define     HB2RS8_BOOT                          0x01BC//      Плата HB2RS8 - загрузчик
#define     _EM3M_SLOT                           0x01BD//      плата S-EM3 v11 PIC18F26K20 ПО EM3M
#define     DSU_20DI_24                          0x01BE//      Плата модуля 20 дискретных входов 24в
#define     _EXR3I4_V3_3_                        0x01BF//      плата EXR3I4 v3.3 ,PIC18F2520, EXR3I4
#define     _EXR3I4_V3_3_BOOT_                   0x01C0//      плата EXR3I4 v3.3 ,PIC18F2520, загрузчик
#define     PNL5_D16                             0x01C1//      Плата PNL5-D16, STM32F429
#define     PNL5_D16_BOOT                        0x01C2//      Плата PNL5-D16, STM32F429, загрузчик
#define     DRZA_CPU_V21                         0x01C3//      Плата DRZA-CPU_v21 STM32F427
#define     DRZA_CPU_V21_BOOT                    0x01C4//      Плата DRZA-CPU_v21 STM32F427, загрузчик
#define     DEPRTU_SV_TX_v22                     0x01C5//      Плата DSUCPUTX-V22 -ПО depRTU SV (прямое с AFE)
#define     DEPRTU_SV_FX_v22                     0x01C6//      Плата DSUCPUFX-V22 -ПО depRTU SV (прямое с AFE)
#define     DEPRTU_iSV_TX_v22                    0x01C7//      Плата DSUCPUTX-V22 -ПО depRTU iSV (интеллектуальная плата)
#define     DEPRTU_iSV_FX_v22                    0x01C8//      Плата DSUCPUFX-V22 -ПО depRTU iSV (интеллектуальная плата)
#define     DSC_4IEXT                            0x01C9//      Плата DSC-4IEXT (интеллектуальная плата)
#define     DSU_PW220_3DO5C                      0x01CA//      Плата DSU_PW220_3DO5C  ПО  CS3DO8
#define     AIN8_I20C                            0x01CB//      Плата AIN16-I20C PIC18F452 ПО AIN8_C
#define     AIN8_I20C_BOOT                       0x01CC//      Плата AIN16-I20C PIC18F452 ПО AIN8_C загрузчик
#define     _AIN8_I20C                           0x01CD//      Плата AIN16-I20C PIC18F4520 ПО AIN8_C
#define     _AIN8_I20C_BOOT                      0x01CE//      Плата AIN16-I20C PIC18F4520 ПО AIN8_C загрузчик
#define     DEPRTU_EM_TX                         0x01CF//      Счетчик ЭЭ с ПКЭ
#define     DEPRTU_EM_BOOT                       0x01D0//      Счетчик ЭЭ с ПКЭ загрузчик
#define     DEPRTU_LT_R                          0x01D1//      DEPRTU-LT-R c RS485 v20
#define     DEPRTU_LT_RS_BOOT                    0x01D2//      DEPRTU-LT c RS485 загрузчик
#define     DEPRTU_LT_T                          0x01D3//      DEPRTU-LT c ETHERNET TX v20
#define     DEPRTU_LT_TX_BOOT                    0x01D4//      DEPRTU-LT c ETHERNET TX загрузчик
#define     DEPRTU_LT_RB                         0x01D5//      DEPRTU-LT-RB c RS485  v20
#define     DEPRTU_LT_TB                         0x01D6//      DEPRTU-LT-TB c ETHERNET TX
#define     DEPRTU_LT_F                          0x01D7//      DEPRTU-LT c ETHERNET FX v20
#define     DEPRTU_LT_FB                         0x01D8//      DEPRTU-LT c ETHERNET FX  v20
#define     EXHG3                                0x01D9//      ExHG3 STM32F427
#define     EXHG3_BOOT                           0x01DA//      ExHG3 STM32F427 - загрузчик
#define     DISPLAY_4_ROW                        0x01DB//      Текстовый 4-строчный дисплей (STM32F207)
#define     DISPLAY_4_ROW_BOOT                   0x01DC//      Текстовый 4-строчный дисплей (STM32F207) - загрузчик
#define     DEPRTU_EM_DISPLAY_4_ROW              0x01DD//      Текстовый 4-строчный дисплей на DepRTU_EM_CPU (STM32F205)
#define     DEPRTU_EM_DISPLAY_4_ROW_BOOT         0x01DE//      Текстовый 4-строчный дисплей на DepRTU_EM_CPU (STM32F205) - загрузчик
#define     DRZA_DISPLAY_4_ROW                   0x01DF//      Текстовый 4-строчный дисплей на DRZA_CPU (STM32F205)
#define     DRZA_DISPLAY_4_ROW_BOOT              0x01E0//      Текстовый 4-строчный дисплей на DRZA_CPU (STM32F205) - загрузчик
#define     DRZA_CPU_V22                         0x01E1//      Плата DRZA-CPU_v22 STM32F427
#define     DRZA_CPU_V22_BOOT                    0x01E2//      Плата DRZA-CPU_v22 STM32F427, загрузчик
#define     DEPRTU_SF_BOOT                       0x01E3//      Плата с sFlash DSUCPU-TX/FX STM32F427-STM32F429, загрузчик
#define     EX_TRACK                             0x01E4//      Плата ExTRACK  STM32F405
#define     EX_TRACK_BOOT                        0x01E5//      Плата ExTRACK  STM32F405 загрузчик
#define     LED_3X4                              0x01E6//      Дисплей 3*4 (7-сегментный) (STM32F205)
#define     LED_3X4_BOOT                         0x01E7//      Дисплей 3*4 (7-сегментный) (STM32F205), загрузчик
#define     BAY_4I02S_4U_4I5P                    0x01E8//      Плата с AFE для А10
#define     BAY_4I02S_4U_4I5P_BOOT               0x01E9//      Плата с AFE для А10, загрузчик
#define     EXSW2                                0x01EA//      Плата ExSW2(ex ExHG3)
#define     EXSW2_BOOT                           0x01EB//      Плата ExSW2(ex ExHG3) - загрузчик
#define     DEPRTU_RZA_FX_V23                    0x01EC//      Плата DSUCPUFX-V23 -ПО depRTU RZA
#define     DEPRTU_RZA_TX_V23                    0x01ED//      Плата DSUCPUTX-V23 -ПО depRTU RZA
#define     DEPRTU_SV_FX_V23                     0x01EE//      Плата DSUCPUFX-V23 -ПО depRTU SV (прямое с AFE)
#define     DEPRTU_SV_TX_V23                     0x01EF//      Плата DSUCPUTX-V23 -ПО depRTU SV (прямое с AFE)
#define     DEPRTU_FX_V23                        0x0200//      Плата DSUCPUFX-V23 -ПО depRTU
#define     DEPRTU_TX_V23                        0x0201//      Плата DSUCPUTX-V23 -ПО depRTU
#define     DEPRTU_iSV_FX_V23                    0x0202//      Плата DSUCPUFX-V23 -ПО depRTU iSV (интеллектуальная плата)
#define     DEPRTU_iSV_TX_V23                    0x0203//      Плата DSUCPUTX-V23 -ПО depRTU iSV (интеллектуальная плата)
#define     DEPRTU_QUAL_FX_V23                   0x0204//      Плата DSUCPUFX-V23 -ПО depRTU -ПО DEPRTU_PKE
#define     DEPRTU_QUAL_TX_V23                   0x0205//      Плата DSUCPUTX-V23 -ПО depRTU -ПО DEPRTU_PKE
#define     DEPRTU_SF_BOOT_V23                   0x0206//      Плата с sFlash DSUCPU-TX/FX STM32F429, загрузчик
#define     TEMPL_DEPRTU_FX_V23                  0x0207//      Плата DSU-CPU-FX-V23   шаблон для процессорного модуля
#define     TEMPL_DEPRTU_TX_V23                  0x0208//      Плата DSU-CPU-TX-V23   шаблон для процессорного модуля
#define     DRZA_PWCSH                           0x0209//      Плата DRZA_PWCSH v30 STM205 токовое питание с датчиком дуги.
#define     BAY_16DO_220R                        0x020A//      Плата BAY-16Do-220R, STMF407
#define     BAY_16DO_220R_BOOT                   0x020B//      Плата BAY-16Do-220R, STMF407, загрузчик
#define     DRZA_AFE7                            0x020C//      Плата AFE DRZA 7 токовых каналов
#define     DEPRTU_LTA_R                         0x020D//      DEPRTU-LT-R v30 c RS485
#define     DEPRTU_LTA_T                         0x020E//      DEPRTU-LT-T v30  c ETHERNET TX
#define     DEPRTU_LTA_RB                        0x020F//      DEPRTU-LT-RB v30 c RS485
#define     DEPRTU_LTA_TB                        0x0210//      DEPRTU-LT-TB v30 c ETHERNET TX
#define     DEPRTU_LTA_F                         0x0211//      DEPRTU-LT-F v30 c ETHERNET FX
#define     DEPRTU_LTA_FB                        0x0212//      DEPRTU-LT-FB v30  c ETHERNET FX
#define     ExCOM_V4_9_45K20                     0x0213//      Плата ExCOM, PCB V4.9, PIC18F45K20, ПО ExCOM
#define     ExCOM_V4_9_45K20_BOOT                0x0214//      Плата ExCOM, PCB V4.9, PIC18F45K20, ПО ExCOM, загрузчик
#define     ExCOM_V4_9_46K20                     0x0215//      Плата ExCOM, PCB V4.9, PIC18F46K20, ПО ExCOM
#define     ExCOM_V4_9_46K20_BOOT                0x0216//      Плата ExCOM, PCB V4.9, PIC18F46K20, ПО ExCOM, загрузчик
#define     DEPRTU_GSM                           0x0217//      DEPRTU_GSM
#define     DEPRTU_GSM_BOOT                      0x0218//      DEPRTU_GSM загрузчик
#define     ExPWA                                0x0219//      ExPWA - STM32F405
#define     ExPWA_BOOT                           0x021A//      ExPWA - STM32F405 - загрузчик
#define     DEPRTU_LT_PARENT                     0x021B//      DEPRTU_LT-XX  прототип для DEPRTU_LT
#define     DRZA_PWAC_205                        0x021C//      Плата блока питания для DeProtec DRZA_PWAC_205 STM32F205
#define     DRZA_PWAC_205_BOOT                   0x021D//      Плата блока питания для DeProtec DRZA_PWAC_205 STM32F205 - загрузчик
#define     SKEY                                 0x021E//      Плата SKEY - STM32F072
#define     SKEY_BOOT                            0x021F//      Плата SKEY - STM32F072 - загрузчик
#define     DRZA_2RS                             0x0220//      Плата интерфейсная для DeProtec: 2 порта RS485
#define     DRZA_2RS_ETH_2TX                     0x0221//      Плата интерфейсная для DeProtec: 2 порта RS485 + 2 медных Ethernet KSZ8463FRL
#define     DRZA_2RS_ETH_2FX                     0x0222//      Плата интерфейсная для DeProtec: 2 порта RS485 + 2 оптических Ethernet KSZ8463FRL
#define     DRZA_2TX_3FS                         0x0223//      Плата интерфейсная для DeProtec: "DRZA-2TX-3FS" 2 медных Ethernet KSZ8463FRL
#define     DRZA_2FX_3FS                         0x0224//      Плата интерфейсная для DeProtec: "DRZA-2FX-3FS" 2 оптических Ethernet KSZ8463FRL
#define     DEPRTU_GSM_v30                       0x0225//      DEPRTU_GSM v30
#define     EX_RPW_CPU_v2                        0x0226//      Плата ExRPW-CPU_V2 STM32F205 ПО EX_PRW_CPU_V2
#define     EX_RPW_CPU_v2_BOOT                   0x0227//      Плата ExRPW-CPU_V2 STM32F205 ПО EX_PRW_CPU_V2 загрузчик
#define     RTU7_UTKZ                            0x0228//      Плата RTU7 v10 STM32F405 ПО RTU7_UTKZ
#define     RTU7_UTKZ_BOOT                       0x0229//      Плата RTU7 v10 STM32F405 ПО RTU7_UTKZ загрузчик
#define     DEPRTU_SV_TX_v22_STEND               0x022A//      Плата DSUCPUTX-V22 -ПО depRTU SV (прямое с AFE-СТЕНД наладки)
#define     DRZA_AFE11                           0x022B//      Плата AFE DRZA 11 токовых каналов
#define     DRZA_AFE47                           0x022C//      Плата AFE DRZA 4 напряжения +7 токовых каналов
#define     EX_ART_2                             0x022D//      Плата ExWMSU-v24  ПО ExART-2
#define     Ex_ART_BOOT                          0x022E//      Плата ExWMSU-v24  ПО ExART-2 загрузчик
#define     EX_SEARCH                            0x022F//      Плата ExSrchAn  ПО ExSearch
#define     EX_SEARCH_BOOT                       0x0230//      Плата ExSrchAn  ПО ExSearch загрузчик
#define     EX_SEARCH_PNL                        0x0231//      Плата ExShPNL ПО ExSearch_PNL
#define     EX_SEARCH_PNL_BOOT                   0x0232//      Плата ExShPNL ПО ExSearch_PNL загрузчик
#define     ExWMSU_CHRG8                         0x0233//      Плата ExWMSU_CHRG8   ПО ExWMSU_CHRG8
#define     ExWMSU_CHRG8_BOOT                    0x0234//      Плата ExWMSU_CHRG8   ПО ExWMSU_CHRG8 загрузчик
#define     ExTAG                                0x0235//      Плата ExTAG    ПО  ExTAG
#define     ExTAG_BOOT                           0x0236//      Плата ExTAG    ПО  ExTAG загрузчик
#define     ExTRACK_NANOSTEND                    0x0237//      Плата ExTRACK  STM32F405 стенд радиоканала
#define     EX_SEARCH_BMS                        0x0238//      Плата ExSearchBMS ПО ExSearchBMS
#define     EX_SEARCH_BMS_BOOT                   0x0239//      Плата ExSearchBMS ПО ExSearchBMS загрузчик
#define     DEPRTU_LT_RGM_DIO                    0x023A//      Плата RTU-LT-RGM-IO-v33 ПО DEPRTU_LT_RGM_DIO
#define     DEPRTU_LT_RGM_DIO_BOOT               0x023B//      Плата RTU-LT-RGM-IO-v33 загрузчик
#define     DEPRTU_LT_RGM_TX                     0x023C//      Плата RTU-LT-RGM-CPU_TX-v33 ПО DEPRTU_LT_RGM
#define     DEPRTU_LT_RGM_BOOT                   0x023D//      Плата RTU-LT-RGM-CPU-v33 загрузчик
#define     DEPRTU_LT_RGM_FX                     0x023E//      Плата RTU-LT-RGM-CPU_FX-v33 ПО DEPRTU_LT_RGM
#define     DEPRTU_LT_TB_RAM32                   0x023F//      Плата DEPRTU-LT-TB v30 c ОЗУ 32МБ  ПО DEPRTU_LT_TB
#define     DEPRTU_LT_FB_RAM32                   0x0240//      Плата DEPRTU-LT-FB v30 c ОЗУ 32МБ  ПО DEPRTU_LT_FB
#define     EX_SFB2                              0x0241//      ExSFB2
#define     EX_SFB2_BOOT                         0x0242//      EXSFB2 загрузчик
#define     ExRZA                                0x0243//      Плата ExRZA STM32F427II ПО ExRZA
#define     ExRZA_BOOT                           0x0244//      Плата ExRZA STM32F427II загрузчик
#define     ExCOM_BMS                            0x0245//      Плата ExCOM_BMS STM32L432KC
#define     ExCOM_BMS_BOOT                       0x0246//      Плата ExCOM_BMS STM32L432KC загрузчик
#define     EMMC_PREPARE                         0x0247//      Плата DSU-CPU-TX-V22  стенд для модулей eMMC
#define     ExCOM_SPCH                           0x0248//      Плата ExCOM_CPU переговорное устройство STM32L452
#define     ExCOM_SPCH_BOOT                      0x0249//      Плата ExCOM_CPU загрузчик STM32L452
#define     DEPRTU_FXSV_TESTER                   0x024A//      DEP_RTU SV Tester FX
#define     EXCAM_SERVICE                        0x024B//      ExCAM service STM32L432
#define     EXCAM_SERVICE_BOOT                   0x024C//      ExCAM service загрузчик STM32L432
#define     DEPRTU_TXSV_TESTER                   0x024D//      DEP_RTU SV Tester TX
#define     DEPRTU_RECLOSER_TX                   0x024E//      Плата depRTU_LT_TB переработанная под управление разъединителем
#define     DEPRTU_LT_T_RAM32                    0x024F//      Плата DEPRTU-LT-T v30 c ОЗУ 32МБ  ПО DEPRTU_LT_T
#define     DEPRTU_LT_F_RAM32                    0x0250//      Плата DEPRTU-LT-F v30 c ОЗУ 32МБ  ПО DEPRTU_LT_F
#define     EX_ART_V30                           0x0251//      Плата ExART_v30
#define     EXSW_3FG_5TX_4RS                     0x0252//      Плата EXSW_3FG_5TX_4RS
#define     EXSW_3FG_5TX_4RS_BOOT                0x0253//      Плата EXSW_3FG_5TX_4RS - загрузчик
#define     EXLUCH                               0x0254//      Плата EXLUCH v63
#define     EXLUCH_BOOT                          0x0255//      Плата EXLUCH v63 - загрузчик
#define     ETH_TRACK                            0x0256//      Плата Eth_Track_V10
#define     ETH_TRACK_BOOT                       0x0257//      Плата Eth_Track_V10 - загрузчик
#define     EXLUCH_CHRG05                        0x0258//      Плата ExLUCH-CHRG05_v20
#define     EXLUCH_CHRG05_BOOT                   0x0259//      Плата ExLUCH-CHRG05_v20 - загрузчик
#define     EXTAG_L                              0x025A//      Плата ExTAG_L v50
#define     EXTAG_L_BOOT                         0x025B//      Плата ExTAG_L v50 -загрузчик
#define     EX_SEARCH_TST                        0x025C//      Плата ExSh-TST  ПО ExSearchTst
#define     EX_SEARCH_TST_BOOT                   0x025D//      Плата ExSh-TST  ПО ExSearchTst загрузчик
#define     depRTU_P_SCi                         0x025E//      Плата depRTU-P-Sci
#define     depRTU_P_SCi_BOOT                    0x025F//      Плата depRTU-P-SCi загрузчик
#define     depRTU_P_SCI_LCD                     0x0260//      Плата depRTU-P-SCI-LCD
#define     depRTU_P_SCI_LCD_BOOT                0x0261//      Плата depRTU-P-SCI-LCD загрузчик
#define     EXSW_3FG_3TX_4RS                     0x0262//      Плата EXSW_3FG_3TX_4RS
#define     EXSW_3FG_3TX_4RS_BOOT                0x0263//      Плата EXSW_3FG_3TX_4RS - загрузчик
#define     ExCOM_STM32                          0x0264//      Плата ExCOM_BMS_v70+
#define     ExCOM_STM32_BOOT                     0x0265//      Плата ExCOM_BMS_v70+ - загрузчик
#define     EXTAG_L_452                          0x0266//      Плата ExTAG_L v50 процесор STM32L452
#define     EXTAG_L_BOOT_452                     0x0267//      Плата ExTAG_L v50 -загрузчик процесор STM32L452
#define     DRZA_DIO_DC                          0x0268//      Плата DRZA-8DIDC-4DOR_v32 процессор STM32F205RG
#define     DRZA_PWDC                            0x0269//      Плата DRZA-PWDC процессор STM32F205RG
#define     EXTAG_L_BEE                          0x026A//      Плата EXTAG_L_BEE процесор STM32L451
#define     EXTAG_L_BOOT_BEE                     0x026B//      Плата EXTAG_L_BEE процесор STM32L451 - загрузчик
#define     EX_TRACK_G4                          0x026C//      Плата ExTRACK  STM32G4
#define     EX_TRACK_G4_BOOT                     0x026D//      Плата ExTRACK  STM32G4 загрузчик
#define     EXLUCH_CHRG05_LAN9303                0x026E//      Плата ExLUCH-CHRG05_v30_STM_LAN9303
#define     EXLUCH_CHRG05_GD                     0x026F//      Плата ExLUCH-CHRG05_v30_GD_LAN9303
#define     EXLUCH_CHRG05_GD_BOOT                0x0270//      Плата ExLUCH-CHRG05_v30_GD_LAN9303 - загрузчик
#define     DZ2_V2_2_4520                        0x0271//      плата DZ2 v2.2 PIC18F4520 ПО- DZ2
#define     DZ2_V2_2_4520_BOOT                   0x0272//      плата DZ2 v2.2 PIC18F4520 ПО- DZ2 - загрузчик
#define     DEPRTU_GSM_v30_GD32                  0x0273//      DEPRTU_GSM v30 GD32F407
#define     EXADR                                0x0274//      Плата ExADR PIC16F876A (PIC16F886)
#define     EXADR_BOOT                           0x0275//      Плата ExADR PIC16F876A (PIC16F886) - загрузчик
#define     DSC_A9P_4U_ST                        0x0276//      Плата  A9P-4U с процессором STM32F407
#define     DSC_A9P_4U_ST_BOOT                   0x0277//      Плата  A9P-4U с процессором STM32F407 загрузчик
#define     DSC_A9P_4U_GD                        0x0278//      Плата  A9P-4U с процессором GD32F407
#define     DSC_A9P_4U_GD_BOOT                   0x0279//      Плата  A9P-4U с процессором GD32F407 загрузчик
#define     DRZA_CPU_GD                          0x027A//      Плата DRZA-CPU_v34 GD32F450
#define     DRZA_CPU_GD_BOOT                     0x027B//      Плата DRZA-CPU_v34 GD32F450 загрузчик
#define     EX_BARI                              0x027C//      Плата ExPW24-BARI-V10 - ПО ExBARI процессор PIC18F46K22
#define     EX_BARI_BOOT                         0x027D//      Плата ExPW24-BARI-V10 - ПО ExBARI процессор PIC18F46K22 - загрузчик
#define     EX_TRACK_UWB                         0x027E//      Плата ExTrack с dw1000 (UWB). Позиционирование
#define     EX_TRACK_UWB_BOOT                    0x027F//      Плата ExTrack с dw1000 (UWB). Позиционирование - загрузчик
#define     EX_ANC_CAS                           0x0280//      Антенна CAS. Плата ExANC-CPU_v10 GD32F427
#define     EX_ANC_CAS_BOOT                      0x0281//      Антенна CAS. Плата ExANC-CPU_v10 GD32F427 - загрузчик
#define     EXTAG_L_UWB                          0x0282//      Плата EXTAG_L с dw1000 (UWB). Позиционирование
#define     EXTAG_L_UWB_BOOT                     0x0283//      Плата EXTAG_L с dw1000 (UWB). Позиционирование - загрузчик
#define     EXTAG_L_UWB_CAS                      0x0284//      Плата EXTAG_L с dw1000 (UWB). Система предотвращения столкновений
#define     EXTAG_L_UWB_CAS_BOOT                 0x0285//      Плата EXTAG_L с dw1000 (UWB). Система предотвращения столкновений - загрузчик
#define     RPR485T3                             0x0286//      Плата RPR485T3 - процессор GD32F427RGT+ FPGA GW1N-UV2QN48
#define     RPR485T3_BOOT                        0x0287//      Плата RPR485T3 - процессор GD32F427RGT+ FPGA GW1N-UV2QN48 - загрузчик
#define     SW10P_3ONE                           0x0288//      Плата SW-10P-BAZA процессор GD32F427RKT
#define     SW10P_3ONE_BOOT                      0x0289//      Плата SW-10P-BAZA процессор GD32F427RKT-  загрузчик
#define     T2_EM3M                              0x028A//      Плата T2-EM3M - процессор GD32F427RGT
#define     T2_EM3M_BOOT                         0x028B//      Плата T2-EM3M - процессор GD32F427RGT - загрузчик
#define     T2_DOUT8R                            0x028C//      Плата T2-DOUT8R - процессор GD32F427RK
#define     T2_DOUT8R_BOOT                       0x028D//      Плата T2-DOUT8R - процессор GD32F427RK - загрузчик
#define     T2_DIN16_24_1F                       0x028E//      Плата T2-DIN16-B1-v20 процессор GD32F427VGT (1MB Flash)
#define     T2_DIN16_24_BOOT_1F                  0x028F//      Плата T2-DIN16-B1-v20 процессор GD32F427VGT (1MB Flash) - загрузчик
#define     T2_DIN16_24_3F                       0x0290//      Плата T2-DIN16-B1-v20 процессор GD32F427VKT (3MB Flash)
#define     T2_DIN16_24_3FE                      0x0291//      Плата T2-DIN16-B1-v20 процессор GD32F427VKT (3MB Flash)+ ETHERNET
#define     T2_DIN16_24_BOOT_3F                  0x0292//      Плата T2-DIN16-B1-v20 процессор GD32F427VKT (3MB Flash) - загрузчик
#define     T2_TEMPLATE                          0x0293//      Плата T2-DIN16-B1-v20 процессор GD32F427VGT (1MB Flash)
#define     T2_TEMPLATE_BOOT                     0x0294//      Плата T2-DIN16-B1-v20 процессор GD32F427VGT (1MB Flash) - загрузчик
#define     RTU_3M_P                             0x0295//      Плата RTU-3M-P-v50-1 процессор GD32F427RKT (3MB Flash)
#define     RTU_3M_P_BOOT                        0x0296//      Плата RTU-3M-P-v50-1 процессор GD32F427RKT (3MB Flash) - загрузчик
#define     EX_TRACK_U_RANGE                     0x0297//      Плата ExTrack с dw1000 (UWB). Измерение расстояния
#define     EX_TRACK_U_RANGE_BOOT                0x0298//      Плата ExTrack с dw1000 (UWB). Измерение расстояния - загрузчик
#define     T2_MCT4_3F                           0x0299//      Плата T2-MCT4 - v1.0 процессор GD32F427RKT
#define     T2_MCT4_3F_BOOT                      0x029A//      Плата T2-MCT4 - v1.0 процессор GD32F427RKT - загрузчик
#define     ETH_TRACK_UWB                        0x029B//      Плата ExTRACK-UWB_v10
#define     ETH_TRACK_UWB_BOOT                   0x029C//      Плата ExTRACK-UWB_v10 - загрузчик
#define     A9P_AIN8_I20                         0x029D//      Плата A9P-8AIN-I20 v10
#define     A9P_AIN8_I20_BOOT                    0x029E//      Плата A9P-8AIN-I20 v10 - загрузчик
#define     EX_R485R                             0x029F//      Плата Ex-R484R
#define     EX_R485R_BOOT                        0x02A0//      Плата Ex-R484R - загрузчик
#define     T2_AOUT1                             0x02A1//      Плата T2-AOUT1
#define     T2_AOUT1_BOOT                        0x02A2//      Плата T2-AOUT1 - загрузчик
#define     T2_AOUT4_U10                         0x02A3//      Плата T2-AOUT4
#define     T2_AOUT4_U10_BOOT                    0x02A4//      Плата T2-AOUT4 - загрузчик
#define     T2_AIN16_I20                         0x02A5//      Плата T2-AIN16-I20 - v1.0 процессор GD32F427RKT
#define     T2_AIN16_I20_BOOT                    0x02A6//      Плата T2-AIN16-I20 - v1.0 процессор GD32F427RKT - загрузчик
#define     DSC_DP_4UOUT_GD                      0x02A7//      Плата  DP-4UOUT с процессором GD32F427
#define     DSC_DP_4UOUT_GD_BOOT                 0x02A8//      Плата  DP-4UOUT с процессором GD32F427 загрузчик
#define     T2_DIN16_XXX                         0x02A9//      Плата T2-DIN16-B1-v21 процессор GD32F427VKT (3MB Flash)
#define     DP_CPU_LT_XX                         0x02AA//      Плата DP_CPU_LT_TX v10 процессор GD32F450IX
#define     DP_CPU_LT_XX_BOOT                    0x02AB//      Плата DP_CPU_LT_TX v10 процессор GD32F450IX - загрузчик
#define     T2_A9_SERV                           0x02AC//      Плата DECONT T2_A9 pic26K20 ПО сервисный процессор A9_SERV
#define     EX_MON                               0x02AD//      Плата ExMON GD32F450
#define     EX_MON_BOOT                          0x02AE//      Плата ExMON GD32F450 - загрузчик
#define     A10_DIN16_XXX                        0x02AF//      Плата A10-DIN16-220 v10 GD32F427
#define     A10_DIN16_XXX_BOOT                   0x02B0//      Плата A10-DIN16-220 v10 GD32F427 - загрузчик
#define     A10_DOUT8R                           0x02B1//      Плата A10-DOUT8R v10 GD32F427
#define     A10_DOUT8R_BOOT                      0x02B2//      Плата A10-DOUT8R v10 GD32F427 - загрузчик
#define     A10_CPU_EXT                          0x02B3//      Плата A10-EXT v10
#define     A10_CPU_EXT_BOOT                     0x02B4//      Плата A10-EXT v10 - загрузчик
#define     DEPRTU_LT_LM                         0x02B5//      DEPRTU_LT_LM v10
#define     DEPRTU_LT_LM_BOOT                    0x02B6//      DEPRTU_LT_LM v10 загрузчик
#define     A10_DIN16_24                         0x02B7//      Плата A10-DIN16-24 v10 GD32F427
#define     T2_R2IN8                             0x02B8//      Плата T2-R2IN8 - v1.0 процессор GD32F427RKT
#define     T2_R2IN8_BOOT                        0x02B9//      Плата T2-R2IN8 - v1.0 процессор GD32F427RKT - загрузчик
#define     PNL10_PW                             0x02BA//      Плата PNL10-PW - v1.1 процессор GD32F427RKT
#define     PNL10_PW_BOOT                        0x02BB//      Плата PNL10-PW - v1.1 процессор GD32F427RKT - загрузчик
#define     EX_CAS                               0x02BC//      Плата ExCAS. процессор GD32F427RKT. Головной модуль CAS
#define     EX_CAS_BOOT                          0x02BD//      Плата ExCAS. процессор GD32F427RKT. Головной модуль CAS - загрузчик
#define     EX_CAS_LITE                          0x02BE//      Плата ExCAS Lite. Головной модуль CAS урезанный
#define     EX_CAS_LITE_BOOT                     0x02BF//      Плата ExCAS Lite. Головной модуль CAS урезанный - загрузчик
#define     EX_R485                              0x02C0//      Плата ExR485. MCU GD32F427. Без аккм
#define     EX_R485_BOOT                         0x02C1//      Плата ExR485. MCU GD32F427. Без аккм - загрузчик
#define     T2_AOUT4_I20                         0x02C2//      Плата T2-AOUT4-I20. Процессор GD32F427
#define     T2_AOUT4_I20_BOOT                    0x02C3//      Плата T2-AOUT4-I20. Процессор GD32F427 - загрузчик
#define     ETH_USB_RS485                        0x02C4//      Плата ETH-USB-RS485. Процессор GD32F450 (SOM-F450)
#define     ETH_USB_RS485_BOOT                   0x02C5//      Плата ETH-USB-RS485. Процессор GD32F450 (SOM-F450) - загрузчик
#define     A10_AIN16_I20                        0x02C6//      Плата A10-AIN16-I20 v10 GD32F427
#define     A10_AIN16_I20_BOOT                   0x02C7//      Плата A10-AIN16-I20 v10 GD32F427 - загрузчик
#define     EX_ANC_COMB                          0x02C8//      Антенна комбайна. Плата ExTRACK-N02_v40 GD32F427
#define     EX_ANC_COMB_BOOT                     0x02C9//      Антенна комбайна. Плата ExTRACK-N02_v40 GD32F427 - загрузчик
#define     T2_RS4E1                             0x02CA//      Плата T2-RS4E1. MCU GD32F427
#define     T2_RS4E1_BOOT                        0x02CB//      Плата T2-RS4E1. MCU GD32F427 - загрузчик

                                                       //      ПАРАМЕТРЫ
                                                       //      Локальные системные параметры
#define     SP_PICRAM                            0x5000//      Содержимое внутреннего ОЗУ
#define     SP_PICROM                            0x5001//      Содержимое внутреннего ПЗУ
#define     SP_PICEEP                            0x5002//      Содержимое внутреннего EEPROM                                                  BYTE[]
#define     SP_EXTIIC                            0x5003//      Содержимое внешнего устройства на  IIC                                         BYTE[]
#define     SP_EXTSPI                            0x5004//      Содержимое внешнего устройство на SPI                                          BYTE[]
#define     SP_FREEIIC                           0x5005//      Внешнее устройство на IIC                                                      BYTE[]
#define     SP_DATARATES_LIST                    0x5006//      список скоростей локальной сети                                                DWORD[]
#define     SP_STATCNT_LIST                      0x5007//      список названий счетчиков статистики сети                                      STRING[3]
#define     SP_NETIDENT                          0xE020//      Идентификация по XID ()                                                        NOTYPE
#define     SP_FIRSTUSE                          0x5008//      Признак первого включения                                                      STRING[6]
#define     SP_MANLOGIN                          0x5009//      Регистрация с минипульта (ввод пароля)                                         STRING[6]
#define     SP_RDNETPASSWORD                     0x500A//      Пароль для сети  (чтение)                                                      STRING[8]
#define     SP_RDMANPASSWORD                     0x500B//      Пароль для минипульта  (чтение)                                                STRING[6]
#define     SP_PSWSERV_RESET                     0x500C//      Сброс службы защиты записи паролем                                             BYTEBOOL
#define     SP_RDSYSPASSWORD                     0x500D//      Системный пароль   (чтение)                                                    STRING[8]
#define     SP_PHYADDR_                          0x500E//      физический адрес в энергонезависимой памяти                                    BYTE
#define     SP_IECRATES_LIST                     0x500F//      список скоростей локальной сети  IEC_101                                       DWORD[]
#define     SP_PHYADDR2_                         0x0050//      резервный физический адрес в энергонезависимой памяти                          BYTE
#define     SP_LOGICAL_ADDR_                     0x0150//      логический адрес первой сети                                                   BYTE
#define     SP_LOGICAL_ADDR2_                    0x0250//      логический адрес второй сети                                                   BYTE
#define     SP_MBRATES_LIST                      0x0350//      список скоростей локальной сети Modbus                                         DWORD[]
#define     SP_SOFTWARE_ID_XID                   0xF150//      Код типа ПО устройства для XID (копия 0xF420)                                  WORD


                                                       //       Физический уровень
#define     SP_DATA_RATE                         0x0120//      скорость локальной сети обмена                                                 DWORD
#define     SP_MAC_ADDR                          0x0220//      mac адрес                                                                      OUI
#define     SP_OUI_SERNUM                        0x0320//      Уникальный идентификатор устройства OUI + серийный номер (младшие 3 байта)     OUI
#define     SP_PHYADDR                           0x0D20//      физический адрес в локальной сети обмена                                       BYTE
#define     SP_PICIDCOD                          0x2020//      PIC18Fxxx ID номер                                                             WORD
#define     SP_PHYADRNETS                        0xF220//      Физические адреса в сетях                                                      BYTE[]
#define     SP_SPEEDNETS                         0xF320//      скорости в сетях                                                               DWORD[]
#define     SP_SOFTWARE_ID                       0xF420//      Код типа ПО устройства                                                         WORD
#define     SP_DRIVER_WINACK                     0xF520//      окно доверия драйвера локальной сети                                           BYTE
#define     SP_DRIVER_CFG                        0xF620//      конфигурация драйвера локальной сети                                           16BIT
#define     SP_DRIVER_RELEASE                    0xF720//      Версия драйвера локальной сети                                                 BYTE
#define     SP_SOFTWARE_VERTION                  0xF820//      Версия ПO устройства (сборка)                                                  BYTE
#define     SP_SERNUM                            0xFB20//      Серийный номер устройства                                                      DWORD
#define     SP_PART_NUMBER                       0xFC20//      Код типа устройства                                                            BYTE
#define     SP_HDWPARTNUMBER                     0xFD20//      Код устройства                                                                 STRING
#define     SP_SFTBRANCH_VERSION                 0xFE20//      Основная версия ПО устройства                                                  BYTE




                                                       //      Уровень DATA LINK
#define     SP_DL_ENTITY_STATUS                  0x0240//      Статус Data Link  уровня                                                       BYTE
#define     SP_ERROR_COUNTS                      0x0340//      Счетчики статистики локальной сети                                             WORD[8]
#define     SP_GROUP_ADDR                        0x0440//      Групповой адрес в локальной сети                                               BYTE
#define     SP_LOGICAL_ADDR                      0x0540//      Логический адрес в локальной сети                                              BYTE
#define     SP_LOGADDRNETS                       0x0641//      Логический адрес в локальных сетях                                             BYTE[]
#define     SP_MAX_BLOCK                         0x0840//      Размеры буферов                                                                WORD[2]
#define     SP_POLLING_WDT                       0x0940//      Сторожевой таймер на обращение к устройству (сек)                              WORD
#define     SP_IP4_ADDR                          0x0A40//      IP адрес                                                                       IP4ADR
#define     SP_IP4_NETMASK                       0x0B40//      Маска сети                                                                     IP4ADR
#define     SP_IP4_GW_ADDR                       0x0C40//      Шлюз                                                                           IP4ADR
#define     SP_VLAN_TAG                          0x0D40//      номер VLAN 12 бит                                                              WORD
#define     SP_IP4_PEER_ADDR                     0x0E40//      IP адрес партнеров                                                             IP4ADR
#define     SP_IPPORT                            0x0F40//      Порты UDP TCP                                                                  WORD
#define     SP_TIMER_VALUES                      0x1040//      Период опроса                                                                  WORD
#define     SP_VLAN_PRIO                         0x1140//      приоритет и CFI                                                                BYTE
#define     SP_VLANNUM                           0x1240//      номер VLAN  с приоритетом и CFI                                                WORD
#define     SP_LINKSERVICEDISABLE                0x1340//      запрет сетевого сервиса                                                        BYTE
#define     SP_RESERVE_PARAMS                    0x1440//      параметры резервирования обмена                                                DWORD[]
#define     SP_FIREWALL_IP                       0x1540//      брендмауэр IP адресов входящих соединений                                      IP4ADR
#define     SP_FIREWALL_MASK                     0x1640//      брендмауэр масок IP адресов входящих соединений                                IP4ADR
#define     SP_REMOTE_IP_ADDR                    0x1740//      IP адреса внешних устройств                                                    IP4ADR
#define     SP_REMOTE_IP_PORTS                   0x1840//      порты внешних устройств                                                        WORD
#define     SP_STATISTIC_CFG                     0xF340//      Конфигурация статистики локальной сети                                         16BIT
#define     SP_SYBUS_LINK_TOUT                   0x1940//      Таймаут активности запросов Sybus/TCP                                          DWORD[]
#define     SP_L2BUS_TIMERTICK                   0x1A40//      Такт таймера трансляций L2BUS (usec)                                           DWORD[]
#define     SP_L2BUS_DATATRM_PERIOD              0x1B40//      Периоды трансляции L2BUS (usec)                                                DWORD[]
#define     SP_L2BUS_TRM_MODES                   0x1C40//      Режимы трансляции  данных                                                      WORD[]
#define     SP_L2BUS_GBL_MODE                    0x1D40//      Режим работы L2BUS                                                             WORD[]
#define     SP_L2BUS_BUFFERS_LEN                 0x1E40//      Размеры буферов L2BUS                                                          WORD[]

                                                       //      Настройка и параметрирование встроенных алгоритмов (0x??6?)
                                                       //      Привязка сигналов заводимых в алгоритм (входы алгоритма 0x??60
#define     SP_DIFF_SCURE_WORDIN_PARAM           0x0060//      Общее для дифференциальной защиты (описание объекта) входы алгоритма           WORD[][]
#define     SP_DTO_WORDIN_PARAM                  0x0160//      ДТО входы алгоритма                                                            WORD[][]
#define     SP_DZT_WORDIN_PARAM                  0x0260//      ДЗТ входы алгоритма                                                            WORD[][]
#define     SP_TO_HV_WORDIN_PARAM                0x0360//      ТО стороны ВН входы алгоритма                                                  WORD[][]
#define     SP_MTZ_HV_WORDIN_PARAM               0x0460//      МТЗ стороны ВН входы алгоритма                                                 WORD[][]
#define     SP_MTZ_LV_WORDIN_PARAM               0x0560//      МТЗ стороны НН входы алгоритма                                                 WORD[][]
#define     SP_KCT_WORDIN_PARAM                  0x0660//      КЦТ входы алгоритма                                                            WORD[][]
#define     SP_MTZ_WORDIN_PARAM                  0x0760//      МТЗ входы алгоритма                                                            WORD[][]
#define     SP_BCI_WORDIN_PARAM                  0x0860//      входы алгоритма контроля изоляции                                              WORD[][]
#define     SP_RCLSR_WORDIN_PARAM                0x0960//      входы алгоритма управления разъединителем                                      WORD[][]
#define     SP_MTZIo_WORDIN_PARAM                0x0A60//      входы алгоритма МТЗ Io                                                         WORD[][]
#define     SP_AMTZ_WORDIN_PARAM                 0x0B60//      входы алгоритма Адаптианвой МТЗ фаз                                            WORD[][]
#define     SP_AMTZIo_WORDIN_PARAM               0x0C60//      входы алгоритма Адаптивной МТЗ Io                                              WORD[][]
#define     SP_OZZ_WORDIN_PARAM                  0x0D60//      входы алгоритма защиты от Однофазного замыкания на землю                       WORD[][]
#define     SP_AVR_VNR_WORDIN_PARAM              0x0E60//      входы алгоритма ВНР после АВР                                                  WORD[][]
#define     SP_AINCMP_WORDIN_PARAM               0x0F60//      дискретные входы алгоритма сравнения аналога с порогом                         WORD[][]
#define     SP_INDCON_WORDIN_PARAM               0x1060//      дискретные входы алгоритма формирования дискрета связи                         WORD[][]
#define     SP_TCRESTR_WORDIN_PARAM              0x1160//      дискретные входы алгоритма ограничения телеуправления                          WORD[][]
#define     SP_ACHR_1_WORDIN_PARAM               0x1260//      дискретные входы алгоритма АЧР-1                                               WORD[][]
#define     SP_ACHR_2_WORDIN_PARAM               0x1360//      дискретные входы алгоритма АЧР-2                                               WORD[][]
#define     SP_ACHR_C_WORDIN_PARAM               0x1460//      дискретные входы алгоритма АЧР-С (по скорости изменения частоты)               WORD[][]
#define     SP_PKE_WORDIN_PARAM                  0x1560//      дискретные входы алгоритмов ПКЕ                                                WORD[][]
#define     SP_PSCI_WORDIN_PARAM                 0x1660//      дискретные входы алгоритмов PSCi                                               WORD[][]
#define     SP_OZZ_HH_WORDIN_PARAM               0x1760//      дискретные входы алгоритма ОЗЗ по высшим гармоникам                            WORD[][]
#define     SP_OZZ_Po_WORDIN_PARAM               0x1860//      дискретные входы алгоритма ОЗЗ по мощности нулевой последовательности          WORD[][]
#define     SP_MTZ2_WORDIN_PARAM                 0x1960//      МТЗ_2 входы алгоритма                                                          WORD[][]
#define     SP_MTZ3_WORDIN_PARAM                 0x1A60//      МТЗ_3 входы алгоритма                                                          WORD[][]
#define     SP_MTZ4_WORDIN_PARAM                 0x1B60//      МТЗ_4 входы алгоритма                                                          WORD[][]
#define     SP_MTZ2Io_WORDIN_PARAM               0x1C60//      входы алгоритма МТЗ по Io ступень 2                                            WORD[][]
#define     SP_MTZ3Io_WORDIN_PARAM               0x1D60//      входы алгоритма МТЗ по Io ступень 3                                            WORD[][]
#define     SP_ZMN_1_WORDIN_PARAM                0x1E60//      входы алгоритма ЗМН                                                            WORD[][]
#define     SP_ZMN_MKC_WORDIN_PARAM              0x1F60//      входы алгоритма ЗМН МКС                                                        WORD[][]
#define     SP_ZPN_1_WORDIN_PARAM                0x2060//      входы алгоритма ЗПН-1                                                          WORD[][]
#define     SP_ZPN_2_WORDIN_PARAM                0x2160//      входы алгоритма ЗПН-2                                                          WORD[][]
#define     SP_I2_WORDIN_PARAM                   0x2260//      входы алгоритма защиты по току обраной последовательности I2                   WORD[][]
#define     SP_U2_WORDIN_PARAM                   0x2360//      входы алгоритма защиты по напряжению обратной последовательности U2            WORD[][]
#define     SP_UROV_WORDIN_PARAM                 0x2460//      входы алгоритма УРОВ                                                           WORD[][]
#define     SP_ACS_WORDIN_PARAM                  0x2560//      входы алгоритма ускоренной ступени МТЗ                                         WORD[][]
#define     SP_LZSH_WORDIN_PARAM                 0x2660//      входы алгоритма ЛЗШ                                                            WORD[][]
#define     SP_APV_WORDIN_PARAM                  0x2760//      входы алгоритма АПВ                                                            WORD[][]
#define     SP_AVR_WORDIN_PARAM                  0x2860//      входы алгоритма АВР                                                            WORD[][]
#define     SP_U_SECT_WORDIN_PARAM               0x2960//      входы сервиса напряжения секции                                                WORD[][]
#define     SP_XCBR_CSWI_WORDIN_PARAM            0x2A60//      входы сервиса выключателя                                                      WORD[][]
#define     SP_EXT_AZ_WORDIN_PARAM               0x2B60//      входы внешних защит                                                            WORD[][]
#define     SP_OZZ_Po2_WORDIN_PARAM              0x2C60//      дискретные входы 2-й ступени ОЗЗ по мощности нулевой последовательности        WORD[][]
#define     SP_AIN_CONVERS_WORDIN_PARAM          0x2D60//      дискретные входы алгоритма преобразования аналогов                             WORD[]
#define     SP_AQ_WORDIN_PARAM                   0x2E60//      дискретные входы алгоритма автоматической квтации                              WORD[]
#define     SP_TEMPARATURE_WORDIN_PARAM          0x2F60//      дискретные входы алгоритма сервисов по температуре                             WORD[]
#define     SP_VCOUNT_WORDDIN_PARSM              0x3060//      дискретный вход для виртуального счетчика переходов                            WORD[]
#define     SP_APPL_MANAGE_WORDDIN_PARAM         0x3160//      дискретный входы прикладного администрирования устойства                       WORD[]
#define     SP_MTZ_MV_WORDIN_PARAM               0x3260//      МТЗ стороны СН входы алгоритма                                                 WORD[][]
#define     SP_UBLNS_WORDIN_PARAM                0x3360//      Небаланс входы алгоритма                                                       WORD[][]

                                                       //      Привязка сигналов форируемых алгоритмом (выходы алгоритма) ) 0x??61
#define     SP_DIFF_SCURE_OUTWORD_PARAM          0x0061//      Общее для дифференциальной защиты (описание объекта) вsходы алгоритма          WORD[]
#define     SP_DTO_OUTWORD_PARAM                 0x0161//      ДТО выходы алгоритма                                                           WORD[]
#define     SP_DZT_OUTWORD_PARAM                 0x0261//      ДЗТ выходы алгоритма                                                           WORD[]
#define     SP_TO_HV_OUTWORD_PARAM               0x0361//      ТО стороны ВН выходы алгоритма                                                 WORD[]
#define     SP_MTZ_HV_OUTWORD_PARAM              0x0461//      МТЗ стороны ВН выходы алгоритма                                                WORD[]
#define     SP_MTZ_LV_OUTWORD_PARAM              0x0561//      МТЗ стороны НН выходы алгоритма                                                WORD[]
#define     SP_KCT_OUTWORD_PARAM                 0x0661//      КЦТ выходы алгоритма                                                           WORD[]
#define     SP_MTZ_OUTWORD_PARAM                 0x0761//      МТЗ выходы алгоритма                                                           WORD[]
#define     SP_BCI_OUTWORD_PARAM                 0x0861//      выходы алгоритма контроля изоляции                                             WORD[]
#define     SP_RCLSR_OUTWORD_PARAM               0x0961//      выходы алгоритма управления разъединителем                                     WORD[]
#define     SP_MTZIo_OUTWORD_PARAM               0x0A61//      выходы алгоритма МТЗ Io                                                        WORD[]
#define     SP_AMTZ_OUTWORD_PARAM                0x0B61//      выходы алгоритма Адаптианвой МТЗ фаз                                           WORD[]
#define     SP_AMTZIo_OUTWORD_PARAM              0x0C61//      выходы алгоритма Адаптивной МТЗ Io                                             WORD[]
#define     SP_OZZ_OUTWORD_PARAM                 0x0D61//      выходы алгоритма защиты от Однофазного замыкания на землю                      WORD[]
#define     SP_AVR_VNR_OUTWORD_PARAM             0x0E61//      выходы алгоритма ВНР после АВР                                                 WORD[]
#define     SP_AINCMP_OUTWORD_PARAM              0x0F61//      дискретные выходы алгоритма сравнения аналога с порогом                        WORD[]
#define     SP_INDCON_OUTWORD_PARAM              0x1061//      дискретные выходы алгоритма формирования дискрета связи                        WORD[]
#define     SP_TCRESTR_OUTWORD_PARAM             0x1161//      дискретные выходы алгоритма ограничения телеуправления                         WORD[]
#define     SP_ACHR_1_OUTWORD_PARAM              0x1261//      дискретные выходы алгоритма АЧР-1                                              WORD[]
#define     SP_ACHR_2_OUTWORD_PARAM              0x1361//      дискретные выходы алгоритма АЧР-2                                              WORD[]
#define     SP_ACHR_C_OUTWORD_PARAM              0x1461//      дискретные выходы алгоритма АЧР-С  (по скорости изменения частоты)             WORD[]
#define     SP_PKE_OUTWOR_PARAM                  0x1561//      дискретные выходы алгоритмов ПКЕ                                               WORD[]
#define     SP_PSCI_OUTWORD_PARAM                0x1661//      дискретные выходы алгоритмов PSCi                                              WORD[]
#define     SP_OZZ_HH_OUTWORD_PARAM              0x1761//      дискретные выходы алгоритма ОЗЗ по высшим гармоникам                           WORD[]
#define     SP_OZZ_Po_OUTWORD_PARAM              0x1861//      дискретные выходы алгоритма ОЗЗ по мощности нулевой последовательности         WORD[]
#define     SP_MTZ2_OUTWORD_PARAM                0x1961//      МТЗ_2 выходы алгоритма                                                         WORD[]
#define     SP_MTZ3_OUTWORD_PARAM                0x1A61//      МТЗ_3 выходы алгоритма                                                         WORD[]
#define     SP_MTZ4_OUTWORD_PARAM                0x1B61//      МТЗ_4 выходы алгоритма                                                         WORD[]
#define     SP_MTZ2Io_OUTWORD_PARAM              0x1C61//      выходы алгоритма МТЗ по 3Io ступень 2                                          WORD[]
#define     SP_MTZ3Io_OUTWORD_PARAM              0x1D61//      выходы алгоритма МТЗ по 3Io ступень 3                                          WORD[]
#define     SP_ZMN_1_OUTWORD_PARAM               0x1E61//      выходы алгоритма ЗМН                                                           WORD[]
#define     SP_ZMN_MKC_OUTWORD_PARAM             0x1F61//      выходы алгоритма ЗМН МКС                                                       WORD[]
#define     SP_ZPN_1_OUTWORD_PARAM               0x2061//      выходы алгоритма ЗПН-1                                                         WORD[]
#define     SP_ZPN_2_OUTWORD_PARAM               0x2161//      выходы алгоритма ЗПН-2                                                         WORD[]
#define     SP_I2_OUTWORD_PARAM                  0x2261//      выходы алгоритма защиты по току обраной последовательности I2                  WORD[]
#define     SP_U2_OUTWORD_PARAM                  0x2361//      выходы алгоритма защиты по напряжению обратной последовательности U2           WORD[]
#define     SP_UROV_OUTWORD_PARAM                0x2461//      выходы алгоритма УРОВ                                                          WORD[]
#define     SP_ACS_OUTWORD_PARAM                 0x2561//      выходы алгоритма ускоренной ступени МТЗ                                        WORD[]
#define     SP_LZSH_OUTWORD_PARAM                0x2661//      выходы алгоритма ЛЗШ                                                           WORD[]
#define     SP_APV_OUTWORD_PARAM                 0x2761//      выходы алгоритма АПВ                                                           WORD[]
#define     SP_AVR_OUTWORD_PARAM                 0x2861//      выходы алгоритма АВР                                                           WORD[]
#define     SP_U_SECT_OUTWORD_PARAM              0x2961//      выходы сервиса напряжения секции                                               WORD[]
#define     SP_XCBR_CSWI_OUTWORD_PARAM           0x2A61//      выходы сервиса выключателя                                                     WORD[]
#define     SP_OZZ_Po2_OUTWORD_PARAM             0x2C61//      дискретные выходы 2-й ступени ОЗЗ по мощности нулевой последовательности       WORD[]
#define     SP_AIN_CONVERS_OUTWORD_PARAM         0x2D61//      дискретные выходы алгоритма преобразования аналогов                            WORD[]
#define     SP_AQ_OUTWORD_PARAM                  0x2E61//      дискретные выходы алгоритма автоматической квтации                             WORD[]
#define     SP_TEMPARATURE_WORDOUT_PARAM         0x2F61//      дискретные выходы алгоритма сервисов по температуре                            WORD[]
#define     SP_APPL_MANAGE_WORDOUT_PARAM         0x3161//      дискретные выходы прикладного администрирования устойства                      WORD[]
#define     SP_MTZ_MV_OUTWORD_PARAM              0x3261//      МТЗ стороны СН выходы алгоритма                                                WORD[]
#define     SP_UBLNS_OUTWORD_PARAM               0x3361//      Небаланс выходы алгоритма                                                      WORD[]

                                                       //      Байтовые параметры настройки встроенных алгоритмов (0x??62)
#define     SP_DIFF_SCURE_BYTE_PARAM             0x0062//      Общие байтовые параметры дифференциальной защиты (описание объекта)            BYTE[]
#define     SP_DTO_BYTE_PARAM                    0x0162//      Байтовые параметры ДТО                                                         BYTE[]
#define     SP_DZT_BYTE_PARAM                    0x0262//      Байтовые параметры ДЗТ                                                         BYTE[]
#define     SP_TO_HV_BYTE_PARAM                  0x0362//      Байтовые параметры ТО стороны ВН                                               BYTE[]
#define     SP_MTZ_HV_BYTE_PARAM                 0x0462//      Байтовые параметры МТЗ стороны ВН                                              BYTE[]
#define     SP_MTZ_LV_BYTE_PARAM                 0x0562//      Байтовые параметры МТЗ стороны НН                                              BYTE[]
#define     SP_KCT_BYTE_PARAM                    0x0662//      Байтовые параметры КЦТ                                                         BYTE[]
#define     SP_MTZ_BYTE_PARAM                    0x0762//      Байтовые параметры МТЗ                                                         BYTE[]
#define     SP_BCI_BYTE_PARAM                    0x0862//      Байтовые параметры алгоритма контроля изоляции                                 BYTE[]
#define     SP_RCLSR_BYTE_PARAM                  0x0962//      Байтовые параметры алгоритма управления разъединителем                         BYTE[]
#define     SP_MTZIo_BYTE_PARAM                  0x0A62//      Байтовые параметры МТЗ Io                                                      BYTE[]
#define     SP_AMTZ_BYTE_PARAM                   0x0B62//      Байтовые параметры Адаптианвой МТЗ фаз                                         BYTE[]
#define     SP_AMTZIo_BYTE_PARAM                 0x0C62//      Байтовые параметры Адаптивной МТЗ Io                                           BYTE[]
#define     SP_OZZ_BYTE_PARAM                    0x0D62//      Байтовые параметры защиты от Однофазного замыкания на землю                    BYTE[]
#define     SP_AVR_VNR_BYTE_PARAM                0x0E62//      Байтовые параметры ВНР после АВР                                               BYTE[]
#define     SP_AINCMP_BYTE_PARAM                 0x0F62//      Байтовые параметры алгоритма сравнения аналога с порогом                       BYTE[][]
#define     SP_INDCON_BYTE_PARAM                 0x1062//      Байтовые параметры алгоритма формирования дискрета связи                       BYTE[][]
#define     SP_TCRESTR_BYTE_PARAM                0x1162//      Байтовые параметры алгоритма ограничения телеуправления                        BYTE[][]
#define     SP_ACHR_1_BYTE_PARAM                 0x1262//      Байтовые параметры алгоритма АЧР-1                                             BYTE[]
#define     SP_ACHR_2_BYTE_PARAM                 0x1362//      Байтовые параметры алгоритма АЧР-2                                             BYTE[]
#define     SP_ACHR_C_BYTE_PARAM                 0x1462//      Байтовые параметры алгоритма АЧР-С  (по скорости изменения частоты)            BYTE[]
#define     SP_PKE_BYTE_PARAM                    0x1562//      Байтовые параметры алгоритмов ПКЕ                                              BYTE[]
#define     SP_PSCI_BYTE_PARAM                   0x1662//      Байтовые параметры алгоритмов PSCi                                             BYTE[]
#define     SP_OZZ_HH_BYTE_PARAM                 0x1762//      Байтовые параметры алгоритмa ОЗЗ по высшим гармоникам                          BYTE[]
#define     SP_OZZ_Po_BYTE_PARAM                 0x1862//      Байтовые параметры алгоритмa ОЗЗ по мощности нулевой последовательности        BYTE[]
#define     SP_MTZ2_BYTE_PARAM                   0x1962//      Байтовые параметры МТЗ_2                                                       BYTE[]
#define     SP_MTZ3_BYTE_PARAM                   0x1A62//      Байтовые параметры МТЗ_3                                                       BYTE[]
#define     SP_MTZ4_BYTE_PARAM                   0x1B62//      Байтовые параметры МТЗ_4                                                       BYTE[]
#define     SP_MTZ2Io_BYTE_PARAM                 0x1C62//      Байтовые параметры МТЗ по Io ступень 2                                         BYTE[]
#define     SP_MTZ3Io_BYTE_PARAM                 0x1D62//      Байтовые параметры МТЗ по Io ступень 3                                         BYTE[]
#define     SP_ZMN_1_BYTE_PARAM                  0x1E62//      Байтовые параметры ЗМН                                                         BYTE[]
#define     SP_ZMN_MKC_BYTE_PARAM                0x1F62//      Байтовые параметры ЗМН МКС                                                     BYTE[]
#define     SP_ZPN_1_BYTE_PARAM                  0x2062//      Байтовые параметры ЗПН-1                                                       BYTE[]
#define     SP_ZPN_2_BYTE_PARAM                  0x2162//      Байтовые параметры ЗПН-2                                                       BYTE[]
#define     SP_I2_BYTE_PARAM                     0x2262//      Байтовые параметры защиты по току обраной последовательности I2                BYTE[]
#define     SP_U2_BYTE_PARAM                     0x2362//      Байтовые параметры защиты по напряжению обратной последовательности U2         BYTE[]
#define     SP_UROV_BYTE_PARAM                   0x2462//      Байтовые параметры УРОВ                                                        BYTE[]
#define     SP_ACS_BYTE_PARAM                    0x2562//      Байтовые параметры ускоренной ступени МТЗ                                      BYTE[]
#define     SP_LZSH_BYTE_PARAM                   0x2662//      Байтовые параметры ЛЗШ                                                         BYTE[]
#define     SP_APV_BYTE_PARAM                    0x2762//      Байтовые параметры АПВ                                                         BYTE[]
#define     SP_AVR_BYTE_PARAM                    0x2862//      Байтовые параметры АВР                                                         BYTE[]
#define     SP_U_SECT_BYTE_PARAM                 0x2962//      Байтовые параметры сервиса напряжения секции                                   BYTE[]
#define     SP_XCBR_CSWI_BYTE_PARAM              0x2A62//      Байтовые параметры  сервиса выключателя                                        BYTE[]
#define     SP_OZZ_Po2_BYTE_PARAM                0x2C62//      Байтовые параметры 2-й ступени ОЗЗ по мощности нулевой последовательности      BYTE[]
#define     SP_AIN_CONVERS_BYTE_PARAM            0x2D62//      Байтовые параметры алгоритма преобразования аналогов                           BYTE[]
#define     SP_AQ_BYTE_PARAM                     0x2E62//      Байтовые параметры алгоритма автоматической квтации                            BYTE[]
#define     SP_TEMPARATURE_BYTE_PARAM            0x2F62//      Байтовые параметры алгоритма сервисов температуры                              BYTE[]
#define     SP_VIRTUALCIN_BYTE_PARAM             0x3062//      Байтовые параметры счетчиков изменений состояний дискретов                     BYTE[]
#define     SP_APPL_MANAGE_BYTE_PARAM            0x3162//      Байтовые параметры прикладного администрирования устойства                     BYTE[]
#define     SP_MTZ_MV_BYTE_PARAM                 0x3262//      Байтовые параметры МТЗ стороны СН                                              BYTE[]
#define     SP_UBLNS_BYTE_PARAM                  0x3362//      Байтовые параметры Небаланс                                                    BYTE[]

                                                       //      Вещественные параметры настройки встроенных алгоритмов (0x??63)
#define     SP_DIFF_SCURE_FLOAT_PARAM            0x0063//      Общие вещественные параметры дифференциальной защиты (описание объекта)        FLOAT[]
#define     SP_DTO_FLOAT_PARAM                   0x0163//      Вещественные параметры ДТО                                                     FLOAT[]
#define     SP_DZT_FLOAT_PARAM                   0x0263//      Вещественные параметры ДЗТ                                                     FLOAT[]
#define     SP_TO_HV_FLOAT_PARAM                 0x0363//      Вещественные параметры ТО стороны ВН                                           FLOAT[]
#define     SP_MTZ_HV_FLOAT_PARAM                0x0463//      Вещественные параметры МТЗ стороны ВН                                          FLOAT[]
#define     SP_MTZ_LV_FLOAT_PARAM                0x0563//      Вещественные параметры МТЗ стороны НН                                          FLOAT[]
#define     SP_KCT_FLOAT_PARAM                   0x0663//      Вещественные параметры КЦТ                                                     FLOAT[]
#define     SP_MTZ_FLOAT_PARAM                   0x0763//      Вещественные параметры МТЗ фаз                                                 FLOAT[]
#define     SP_BCI_FLOAT_PARAM                   0x0863//      Вещественные параметры контроля изоляции                                       FLOAT[]
#define     SP_RCLSR_FLOAT_PARAM                 0x0963//      Вещественные параметры алгоритма разъединителя                                 FLOAT[]
#define     SP_MTZIo_FLOAT_PARAM                 0x0A63//      Вещественные параметры МТЗ Io                                                  FLOAT[]
#define     SP_AMTZ_FLOAT_PARAM                  0x0B63//      Вещественные параметры Адаптианвой МТЗ фаз                                     FLOAT[]
#define     SP_AMTZIo_FLOAT_PARAM                0x0C63//      Вещественные параметры Адаптивной МТЗ Io                                       FLOAT[]
#define     SP_OZZ_FLOAT_PARAM                   0x0D63//      Вещественные параметры защиты от Однофазного замыкания на землю                FLOAT[]
#define     SP_AVR_VNR_FLOAT_PARAM               0x0E63//      Вещественные параметры ВНР после АВР                                           FLOAT[]
#define     SP_AINCMP_FLOAT_PARAM                0x0F63//      Вещественные параметры алгоритма сравнения аналога с порогом                   FLOAT[][]
#define     SP_INDCON_FLOAT_PARAM                0x1063//      Вещественные параметры алгоритма формирования дискрета связи                   FLOAT[][]
#define     SP_TCRESTR_FLOAT_PARAM               0x1163//      Вещественные параметры алгоритма ограничения телеуправления                    FLOAT[][]
#define     SP_ACHR_1_FLOAT_PARAM                0x1263//      Вещественные параметры алгоритма АЧР-1                                         FLOAT[]
#define     SP_ACHR_2_FLOAT_PARAM                0x1363//      Вещественные параметры алгоритма АЧР-2                                         FLOAT[]
#define     SP_ACHR_C_FLOAT_PARAM                0x1463//      Вещественные параметры алгоритма АЧР-С (по скорости изменения частоты)         FLOAT[]
#define     SP_PKE_FLOAT_PARAM                   0x1563//      Вещественные параметры алгоритмов ПКЕ                                          FLOAT[]
#define     SP_PSCI_FLOAT_PARAM                  0x1663//      Вещественные параметры алгоритмов PSCi                                         FLOAT[]
#define     SP_OZZ_HH_FLOAT_PARAM                0x1763//      Вещественные параметры алгоритмов ОЗЗ по высшим гармоникам                     FLOAT[]
#define     SP_OZZ_Po_FLOAT_PARAM                0x1863//      Вещественные параметры алгоритмов ОЗЗ мощности нулевой последовательности      FLOAT[]
#define     SP_MTZ2_FLOAT_PARAM                  0x1963//      Вещественные параметры МТЗ_2 фаз                                               FLOAT[]
#define     SP_MTZ3_FLOAT_PARAM                  0x1A63//      Вещественные параметры МТЗ_3 фаз                                               FLOAT[]
#define     SP_MTZ4_FLOAT_PARAM                  0x1B63//      Вещественные параметры МТЗ_4 фаз                                               FLOAT[]
#define     SP_MTZ2Io_FLOAT_PARAM                0x1C63//      Вещественные параметры МТЗ по 3Io ступень 2                                    FLOAT[]
#define     SP_MTZ3Io_FLOAT_PARAM                0x1D63//      Вещественные параметры МТЗ по 3Io ступень 3                                    FLOAT[]
#define     SP_ZMN_1_FLOAT_PARAM                 0x1E63//      Вещественные параметры ЗМН                                                     FLOAT[]
#define     SP_ZMN_MKC_FLOAT_PARAM               0x1F63//      Вещественные параметры ЗМН МКС                                                 FLOAT[]
#define     SP_ZPN_1_FLOAT_PARAM                 0x2063//      Вещественные параметры ЗПН-1                                                   FLOAT[]
#define     SP_ZPN_2_FLOAT_PARAM                 0x2163//      Вещественные параметры ЗПН-2                                                   FLOAT[]
#define     SP_I2_FLOAT_PARAM                    0x2263//      Вещественные параметры защиты по току обраной последовательности I2            FLOAT[]
#define     SP_U2_FLOAT_PARAM                    0x2363//      Вещественные параметры защиты по напряжению обратной последовательности U2     FLOAT[]
#define     SP_UROV_FLOAT_PARAM                  0x2463//      Вещественные параметры УРОВ                                                    FLOAT[]
#define     SP_ACS_FLOAT_PARAM                   0x2563//      Вещественные параметры ускоренной ступени МТЗ                                  FLOAT[]
#define     SP_LZSH_FLOAT_PARAM                  0x2663//      Вещественные параметры ЛЗШ                                                     FLOAT[]
#define     SP_APV_FLOAT_PARAM                   0x2763//      Вещественные параметры АПВ                                                     FLOAT[]
#define     SP_AVR_FLOAT_PARAM                   0x2863//      Вещественные параметры АВР                                                     FLOAT[]
#define     SP_U_SECT_FLOAT_PARAM                0x2963//      Вещественные параметры сервиса напряжения секции                               FLOAT[]
#define     SP_XCBR_CSWI_FLOAT_PARAM             0x2A63//      Вещественные параметры сервиса выключателя                                     FLOAT[]
#define     SP_OZZ_Po2_FLOAT_PARAM               0x2C63//      Вещественные параметры 2-й ступени ОЗЗ мощности нулевой последовательности     FLOAT[]
#define     SP_AIN_CONVERS_FLOAT_PARAM           0x2D63//      Вещественные параметры алгоритма преобразования аналогов                       FLOAT[]
#define     SP_AQ_FLOAT_PARAM                    0x2E63//      Вещественные параметры алгоритма автоматической квтации                        FLOAT[]
#define     SP_TEMPARATURE_FLOAT_PARAM           0x2F63//      Вещественные параметры алгоритма сервисов температуры                          FLOAT[]
#define     SP_APPL_MANAGE_FLOAT_PARAM           0x3163//      Вещественные параметры прикладного администрирования устойства                 FLOAT[]
#define     SP_MTZ_MV_FLOAT_PARAM                0x3263//      Вещественные параметры МТЗ стороны СН                                          FLOAT[]
#define     SP_UBLNS_FLOAT_PARAM                 0x3363//      Вещественные параметры Небаланс                                                FLOAT[]

                                                       //      Привязка аналоговых сигналов заводимых в алгоритм (AIN входы алгоритма 0x??64
#define     SP_AVR_VNR_WORDAIN_PARAM             0x0E64//      Аналоговые входы алгоритма ВНР после АВР                                       WORD[][]
#define     SP_AINCMP_WORDAIN_PARAM              0x0F64//      Аналоговые входы алгоритма сравнения аналога с порогом                         WORD[][]
#define     SP_PKE_WORDAIN_PARAM                 0x1564//      Аналоговые входы алгоритма ПКЕ                                                 WORD[][]
#define     SP_AIN_CONVERS_WORDAIN_PARAM         0x2D64//      Аналоговые входы алгоритма преобразования аналогов                             WORD[][]
#define     SP_TEMPARATURE_WORDAIN_PARAM         0x2F64//      Аналоговые входы алгоритма сервисов температуры                                WORD[][]
#define     SP_APPL_MANAGE_WORDAIN_PARAM         0x3164//      Аналоговые входы прикладного администрирования устойства                       WORD[]

                                                       //      Привязка аналоговых сигналов формируемых в алгоритме (AOUT вsходы алгоритма 0x??65
#define     SP_AIN_CONVERS_WORDOUT_PARAM         0x2D65//      Аналоговые выходы алгоритма преобразования аналогов                            WORD[][]
#define     SP_APPL_MANAGE_WORDAOUT_PARAM        0x3165//      Аналоговые выходы прикладного администрирования устойства                      WORD[]

                                                       //      Файл описания алгоритма преобразования встроенных алгоритмов                   0x??66
#define     SP_AIN_CONVERS_RULES_FILE            0x2D66//      Файл описания алгоритма преобразования аналогов                                STRING

                                                       //      Привязка счетчиков формируемых алгоритмами
#define     SP_VCIN_CNT_OUTWORD_PARAM            0x3067//      Виртуальные счетчики переходов состояний дискретов                             WORD[]
#define     SP_APPL_MANAGE_WORDCIN_PARAM         0x3167//      Счётчики прикладного администрирования устойства                               WORD[]


                                                       //      Прикладной уровень
                                                       //      Управление устройством
#define     SP_SERVICELOCK                       0x00E0//      Блокировка сервисов устройства                                                 BYTE[16]
#define     SP_ONLINE_SWITCHERS                  0x01E0//      Оперативное переключение режимов работы                                        BYTE[]
#define     SP_APP_ENTITY_STATUS                 0x02E0//      Статус прикладного уровня                                                      BYTE
#define     SP_RESOURCE_METR                     0x05E0//      Параметр измерения ресурсов процессора                                         DWORD[]
#define     SP_CFGVER                            0x06E0//      Версия прикладной конфигурации                                                 WORD
#define     SP_CFGVERNUM                         0x07E0//      Номер прикладной конфигурации                                                  DWORD
#define     SP_CFGDATE                           0x08E0//      Дата прикладной конфигурации
#define     SP_MNFVER                            0x09E0//      Номер производственной конфигурации                                            WORD
#define     SP_MNFCFGDATE                        0x0AE0//      Дата производственной конфигурации                                             STRICT
#define     SP_SAVECFG                           0x0BE0//      Сохранение прикладной конфигурации                                             BYTE
#define     SP_SAVEMNFDATE                       0x0CE0//      Сохранение производственной конфигурации                                       BYTE
#define     SP_ENTRIES_REASSIGNS                 0x0DE0//      Логическое переназначение входов                                               BYTE
#define     SP_ENTRIES_INVERSE                   0x0EE0//      Инверсия состояния входов                                                      BYTE
#define     SP_RESET_ENTITY_APPL                 0x4DE0//      Рестарт прикладного уровня                                                     BYTE
#define     SP_ACTIVPROFILE                      0xC0E0//      Действующий профиль/настройка устройства                                       BYTE
#define     SP_NEWPROFILE                        0xC1E0//      Профиль/настройка устройства после рестарта                                    BYTE
#define     SP_PROFILE_NAME                      0xC2E0//      Название профиля                                                               STRING
#define     SP_RSTMNFDATE                        0xC3E0//      Сброс всех настроек и калибровок                                               BYTE
#define     SP_SMP_DEVICE_SOFTWARE_ID            0xD5E0//      Расширенный код типа устройства                                                WORD
#define     SP_MNFCFGVER                         0x0FE0//      Версия производственной конфигурации ПО                                        WORD
#define     SP_RESETCLB                          0x10E0//      Сброс калибровок в заводские                                                   BYTE
#define     SP_CONFIGSIZE                        0x11E0//      Размер конфигурационных данных                                                 DWORD[]
#define     SP_EXTFUNCCFG                        0x12E0//      Конфигурация расширенных функций                                               BYTE
#define     SP_SYS_LEDS_BEHAVIOUR                0x13E0//      Поведение/(период мигания) системной индикации, ms (0- отсутствие мигания)     WORD
#define     SP_GLOBAL_QUITATION                  0x14E0//      Квитация прикладных алгоритмов. Сброс предупредительной и аварийной сигнализациWORD
#define     SP_FIRMWARE_CHECK                    0x15E0//      Период проведения теста целостности встроенного ПО (в часах)                   WORD
#define     SP_FIRMWARE_CHECK_CTRL               0x16E0//      Тест  целостности встроенного ПО (старт и чтение результата)                   BYTE
#define     SP_ALTERNATE_MODE                    0x17E0//      Альтернативный режим работы                                                    BYTE

                                                       //      Задача SM
#define     SP_SM_ENTITY_STATUS                  0x02E1//      Статус SYSTEM MANAGEMENT                                                       BYTE
#define     SP_PARLIST                           0x03E1//      Список используемых параметров модуля                                          WORD[65536]
#define     SP_PARATTR                           0x04E1//      аттрибуты параметра                                                            16BIT
#define     SP_PARNAME                           0x05E1//      имя параметра                                                                  STRING
#define     SP_PARTYPE                           0x06E1//      тип параметра                                                                  BYTE
#define     SP_PARDIM                            0x07E1//      размерность параметра                                                          WORD
#define     SP_PWRSUPL_ERR_LEVL                  0x08E1//      Уровень Brown-Out питания                                                      WORD
#define     SP_PARSIZE                           0x09E1//      Размер параметра(длина в байтах)                                               WORD
#define     SP_INTTEMP                           0x0AE1//      Внутренняя температура                                                         FLOAT
#define     SP_PARNAME_PERSONAL                  0x0BE1//      имя индивидуального параметра                                                  STRING
#define     SP_DATETABLE                         0x10E1//      Таблица с данными                                                              WORD[]
#define     SP_MNGDATETABLE                      0x11E1//      Управление доступом к таблице с данными                                        WORD
#define     SP_RTCADJUST                         0x12E1//      Подстройка микросхемы RTC                                                      BYTE
#define     SP_CALENDARTYPE                      0x13E1//      Номер календаря                                                                BYTE
#define     SP_SYSMANVER                         0x14E1//      Версия системы параметров                                                      WORD
#define     SP_BOOTLDVER                         0x15E1//      Версия загрузчика ПО                                                           BYTE
#define     SP_BOOTLDID                          0x16E1//      Идентификатор загрузчика                                                       WORD
#define     SP_POWERFAIL_PROPERTIES              0x17E1//      Характеристики падения питания                                                 FLOAT[]
#define     SP_RTOS_STAT_COLLECT                 0x18E1//      Сбор статистики ОСРВ                                                           BYTE
#define     SP_RTOS_STAT_SHOW                    0x19E1//      Отображение статистики ОСРВ                                                    STRING[128]
#define     SP_DEVICE_ONLINE                     0x1AE1//      Время ONLINE устройства (сек)                                                  DWORD
#define     SP_RESET_DEVICE                      0x4CE1//      Рестарт устройства                                                             BYTE
#define     SP_RESET_DEVICE_FLAG                 0x4DE1//      Признак рестарта  устройства                                                   BYTE
#define     SP_SOFTRESET                         0x4FE1//      Универсальный рестарт (1- устройства,2- прикл.части)                           BYTE
#define     SP_JUMPERWRITEPROTECT                0x50E1//      Поддержка  защиты записи параметров                                            BYTEBOOL
#define     SP_PASSWORDNET                       0x51E1//      Пароль для локальной сети (запись)                                             STRING[8]
#define     SP_PASSWORDMAN                       0x52E1//      Пароль для минипульта  (запись)                                                STRING[6]
#define     SP_NETLOGIN                          0x53E1//      Регистрация по сети (ввод пароля)                                              STRING[8]
#define     SP_PASSWORD_ON                       0x54E1//      Включение защиты паролями                                                      T.BYTEBOOL
#define     SP_RESETWDTCNT                       0x55E1//      Счетчик рестартов по сторожевому таймеру                                       WORD
#define     SP_PASSWORDSYS                       0x56E1//      Системный пароль для локальной сети (запись)                                   STRING[8]
#define     SP_SYSLOGIN                          0x57E1//      Регистрация по сети (ввод системного пароля)                                   STRING[8]
#define     SP_FIRMUPGRADE                       0x58E1//      Режим перезаписи ПО                                                            BYTE
#define     SP_FIRMWARECODE                      0x59E1//      Код ПО устройства                                                              BYTE[]
#define     SP_EEPROMDATA                        0x5AE1//      Данные внешнего EEPROM                                                         BYTE[]
#define     SP_ROMMEMORYMAP                      0x5BE1//      Карта памяти программ                                                          DWORD[]
#define     SP_CFGDATA                           0x5CE1//      Данные конфигурации процессора                                                 BYTE[]
#define     SP_RESACCESSMAP                      0x5DE1//      карта соответствия адресов в hex- файле и параметров доступа                   DWORD[]
#define     SP_EXTENADDRESS                      0x5EE1//      Расширение адреса                                                              WORD
#define     SP_POWERSAVEFAIL_CNT                 0x5FE1//      Счетчик неудачных сохранений по падению питания                                WORD
#define     SP_FATALERROR_CNT                    0x60E1//      Счетчик рестартов по фатальным ошибкам                                         WORD
#define     SP_LASTFATALERROR_CODE               0x61E1//      Код последней фатальной ошибки                                                 BYTE
#define     SP_LOGGING                           0x62E1//      Регистрация ошибок и дамп                                                      BYTE[]
#define     SP_DIAGPARAMS8                       0x63E1//      Параметры диагностики -8 битовые                                               BYTE[]
#define     SP_DIAGPARAMS16                      0x64E1//      Параметры диагностики -16 битовые                                              WORD[]
#define     SP_DIAGPARAMS32                      0x65E1//      Параметры диагностики -32 битовые                                              DWORD[]
#define     SP_DIAGPARAMSFLOAT                   0x66E1//      Параметры диагностики -FLOAT                                                   FLOAT[]
#define     SP_FATALERROR_DUMP                   0x67E1//      Дамп фатальной ошибки                                                          BYTE[]
#define     SP_STENDDATETIME                     0x68E1//      Дата и время прохождения стенда                                                STRING[32]
#define     SP_UPGRADEDATETIME                   0x69E1//      Дата и время последнего обновления ПО                                          STRING[32]
#define     SP_TIMESYNCROPARS                    0x6AE1//      Параметры протоколов синхронизации                                             BYTE[]
#define     SP_USRLOGIN                          0x6BE1//      Регистрация пользователя -ввод пароля                                          STRING[]
#define     SP_HEATERLVL                         0x6CE1//      Пороги температуры (управление подогревом)                                     FLOAT[]
#define     SP_DEVICE_SERVICE                    0x6DE1//      Обслуживание модулей устройства                                                BYTE[]
#define     SP_PTPTIME                           0x6EE1//      Астрономическое время в формате PTP                                            T.UTC
#define     SP_DATETIME                          0x6FE1//      Дата и время                                                                   DWORD
#define     SP_MAXTIMEDIFF                       0x70E1//      Максимально допустимое отклонение времени (мкс)                                DWORD
#define     SP_MICROSECUNDS_CNT                  0xE9E1//      Системный счетчик микросекунд                                                  BYTE[3]
#define     SP_ASTRONOMIC_TIME_MCS               0xEAE1//      Астрономическое время c разрядностью 1 микросекунда                            T.UTC
#define     SP_DIAGS_CODES                       0xEBE1//      Коды диагностики и самодиагностики                                             BYTE[]
#define     SP_SM_CHIPREGS32                     0xECE1//      Регистры микросхем - 32 битовые                                                DWORD[]
#define     SP_ASTRONOMIC_TIME                   0xEDE1//      Астрономическое время                                                          T.STRICT
#define     SP_SYSTEM_COUNTER                    0xEEE1//      Системный счетчик миллисекунд                                                  WORD
#define     SP_SM_TIMEINCORCNT                   0xEFE1//      Счетчик некорректных переводов времени                                         DWORD
#define     SP_SM_TIMEDIFFIMND                   0xF0E1//      Уход времени мгновенный                                                        FLOAT
#define     SP_SM_TIMEDIFSUMM                    0xF1E1//      Уход времени суммарный (за сутки)                                              FLOAT
#define     SP_SM_CHIPREGS8                      0xF2E1//      Регистры микросхем - 8 битовые                                                 BYTE[]
#define     SP_SM_CHIPREGS16                     0xF3E1//      Регистры микросхем - 16 битовые                                                WORD[]
#define     SP_PWRMONVAL                         0xF4E1//      Текущее значение контроля питания (вольт)                                      FLOAT
#define     SP_PWRMONLVL                         0xF5E1//      Порог падения питания (вольт)                                                  FLOAT
#define     SP_PWRREMIND                         0xF6E1//      Значение питания после сохранения                                              FLOAT
#define     SP_PWRFAILTIME                       0xF7E1//      Астрономическое время рестарта устройства                                      T.STRICT
#define     SP_RESETCNT                          0xF8E1//      Счетчик рестартов                                                              WORD
#define     SP_SECCHANEL                         0xF9E1//      Режим работы второго канала                                                    BYTE
#define     SP_CHANGETIME                        0xFAE1//      Автоматический переход на летнее время                                         BYTEBOOL
#define     SP_C2TOF                             0xFBE1//      Время выключения второго канала, мин                                           WORD
#define     SP_C2TON                             0xFCE1//      Время включения второго канала, мин                                            WORD
#define     SP_FLOAT_ARRAY                       0xFDE1//      Массив FLOAT (промежуточные и служебные величины)                              FLOAT[]
#define     SP_TIMEZONE_HOUR                     0xFEE1//      Часовой пояс (смещение в часах)                                                T.SBYTE

                                                       //      Задача GBS
#define     SP_GBS_ENTITY_STATUS                 0x02E2//      Статус GBS                                                                     BYTE
#define     SP_DEVICE_NAME                       0xF4E2//      Имя устройство в ASCII                                                         STRING[6]
#define     SP_MNFDATE                           0xFAE2//      Информация ОТК                                                                 STRING[16]
#define     SP_HDWDEF                            0xFDE2//      Аппаратная реализация                                                          STRING[16]
#define     SP_AINSNUM                           0xE0E2//      Количество аналоговых входов                                                   Word
#define     SP_DINSNUM                           0xE1E2//      Количество дискретных входов                                                   Word
#define     SP_CINNUM                            0xE2E2//      Количество счетчиков                                                           Word
#define     SP_AOUTNUM                           0xE3E2//      Количество аналоговых выходов                                                  Word
#define     SP_DOUTNUM                           0xE4E2//      Количество дискретных выходов                                                  Word
#define     SP_LINKNUM                           0xE5E2//      Количество интерфейсов связи                                                   Word
#define     SP_CMDSETVER                         0xE6E2//      Версия системы прикладных команд                                               Word
#define     SP_DEPCOPYRIGHT                      0xF3E2//      Строка копирайта компании ДЭП                                                  STRING[16]
#define     SP_RESETDATA                         0xE7E2//      Сброс данных в заводские настройки                                             BYTE
#define     SP_IONUMLIST                         0xE8E2//      Список количества каналов по типам данных                                      BYTE
#define     SP_GROUP1LIST                        0xE9E2//      Список сигналов группы 1                                                       BYTE[][]
#define     SP_GROUP2LIST                        0xEAE2//      Список сигналов группы 2                                                       BYTE[][]
#define     SP_BACKGROUNDLIST                    0xEBE2//      Список сигналов циклической (фоновой) передачи                                 BYTE[][]
#define     SP_CTRL_SPORADIC                     0xECE2//      Управление спорадической передачей Sybus по типам данных                       BYTE
#define     SP_PROTRS485_LIST                    0xEDE2//      Список поддерживаемых протоколов на RS485                                      BYTE
#define     SP_PROTETH_LIST                      0xEEE2//      Список поддерживаемых протоколов на Ethernet                                   BYTE
#define     SP_TIMESYNC_LIST                     0xEFE2//      Список поддерживаемых протоколов синхронизации времени                         BYTE

                                                       //      Задача CFG Конфигурирование устройства
#define     SP_SOCKETSCFG                        0x00E3//      Состав плат в слотах - структура из 2-х слов: тип ; опция                      WORD[][]
#define     SP_XCBRCFG                           0x01E3//      Конфигурирование логического узла выключателя XCBR                             BYTE[][]
#define     SP_GOSDATASET                        0x02E3//      Конфигурирование состава набора данных исход. Goose                            BYTE[][]
#define     SP_EXTEEP_REQSRV                     0x03E3//      Обслуживание crc записи в EEPROM модуля DSU                                    BYTE
#define     SP_EXTEEP_MODTYPE                    0x04E3//      Код типа модуля DSU                                                            DWORD
#define     SP_EXTEEP_MODSN                      0x05E3//      Серийный номер модуля DSU                                                      DWORD
#define     SP_EXTEEP_MODNAME                    0x06E3//      Название модуля DSU                                                            STRING
#define     SP_EXTEEP_MODMNF                     0x07E3//      Сведения изготовителя модуля DSU                                               STRING
#define     SP_EXTEEP_MODCLB                     0x08E3//      Калибровки модуля DSU                                                          FLOAT[]
#define     SP_SOCKETSNUM                        0x09E3//      Количество слотов в крейте                                                     BYTE
#define     SP_BACKPLAIN                         0x0AE3//      Код объединительной платы                                                      BYTE
#define     SP_RESOURSE_SWT                      0x0BE3//      Ресурс выключателя, конфигурирование                                           FLOAT[]
#define     SP_OBJECT_NAME                       0x0CE3//      Название объекта                                                               STRING
#define     SP_GOSDATASET2                       0x0DE3//      Конфигурирование состава набора данных исход. Goose 2                          BYTE[][]
#define     SP_EVENTLOG                          0x0EE3//      Конфигурирование уставок для граничных событий                                 FLOAT()
#define     SP_SETALTCFGTEMPLATE                 0x0FE3//      Установа  альтернативной конфигурации                                          BYTE
#define     SP_CREATE_NUM                        0x10E3//      Номер крейта размещения                                                        BYTE

                                                       //      Задача STREAM- потоковые данные (речь, видео)
#define     SP_CALL_ABONENT                      0x00E8//      Вызов абонента(ов) для разговора                                               BYTE[]
#define     SP_STREAM_MARKER                     0x01E8//      маркер начала передачи речи                                                    BYTE[]
#define     SP_STREAM_SPEECH                     0x02E8//      Поток закодированной речи                                                      BYTE[]
#define     SP_RINGTONE                          0x03E8//      мелодия сигнала вызова                                                         WORD[]
#define     SP_SPEECHSAMPLE                      0x04E8//      образец закодированной речи                                                    BYTE[]
#define     SP_STREAM_SYNC                       0x05E8//      синхронизация потоковых данных                                                 BYTE[]
#define     SP_STREAM_GROUP                      0x06E8//      Группа (групповой адрес) потоковых данных                                      BYTE[]
#define     SP_STREAM_TUNING                     0x07E8//      настройки кодека и параметров потоковых данных                                 WORD[]
#define     SP_RDSPEECH_CODEC2                   0x08E8//      прием голосовых данных по CODEC2                                               BYTE[]
#define     SP_WRSPEECH_CODEC2                   0x09E8//      выдача голосовых данных по CODEC2                                              BYTE[]
#define     SP_RDPOS_CODEC2                      0x40E8//      прием данных положения                                                         BYTE[]

                                                       //      Задача LINK- интерфейсы связи
#define     SP_SPEED                             0x00E9//      Скорость обмена                                                                DWORD[]
#define     SP_MODEM_TUNING                      0x01E9//      настройки модема                                                               BYTE[]
#define     SP_ACKTIMEOUT                        0x03E9//      Значение таймаута ответа                                                       WORD[]
#define     SP_NETADDR                           0x04E9//      сетевой адрес                                                                  WORD[]
#define     SP_LINKMODE                          0x05E9//      Режим в сети ( мастер, слейв, шлюз…)                                           BYTE[]
#define     SP_FRAMEFORMAT                       0x06E9//      Формат фрейма                                                                  BYTE[]
#define     SP_EXCHANGE_INTERFACE                0x07E9//      Обмен по интерфейсу                                                            BYTE[]
#define     SP_EXCHANGE_STATISTIC                0x08E9//      Статистика обмена по интерфейсам                                               DWORD[][]
#define     SP_TESTSEND                          0x09E9//      Тестовая передача                                                              BYTE[][]
#define     SP_PROTOCOL                          0x0AE9//      Параметры протокола обмена                                                     BYTE[][]
#define     SP_ABONENTSNUM                       0x0BE9//      количество устройств на интерфейсе                                             BYTE[][]
#define     SP_SLAVEIDENT                        0x0CE9//      параметры идентификации устройств (XID параметры)                              BYTE[][]
#define     SP_SLAVEAPPLPARLIST                  0x0DE9//      список прикладных параметров устройств (AIN CIN,AOUT,DIN,DOUT)                 BYTE[][]
#define     SP_SLAVEINITFLOW                     0x0EE9//      Инициализационные данные устройств                                             BYTE[][]
#define     SP_EXCHANGESETTINGS                  0x0FE9//      Оперативные настройки параметров обмена                                        DWORD[]
#define     SP_CURINTERFACENUM                   0x10E9//      Номер текущего интерфейса                                                      BYTE[]
#define     SP_CFG_IEC_60870_5_101               0x11E9//      Настройка интерфейса для поддержки МЭК 60870-5-101                             BYTE[][]
#define     SP_CFG_MODBUS                        0x12E9//      Настройка интерфейса для поддержки MODBUS RTU                                  BYTE[]
#define     SP_PROTPARS_BYTES                    0x13E9//      Параметры протоколов байтовые                                                  BYTE[]
#define     SP_PROTPARS_WORD                     0x14E9//      Параметры протоколов 2-х байтовые                                              WORD[]
#define     SP_PROTPARS_DWORD                    0x15E9//      Параметры протоколов 4-х байтовые                                              DWORD[]
#define     SP_PROTPARS_STRING                   0x16E9//      Параметры протоколов строковые                                                 STRING
#define     SP_ASDU                              0x17E9//      Параметры ASDU                                                                 WORD[]
#define     SP_IEC_104_PARAMS                    0x18E9//      Параметры протокола МЭК 104                                                    WORD[]
#define     SP_ETHERNET_OPTIONS                  0x19E9//      Опции ehernet                                                                  T.32BIT
#define     SP_IEC_104_ASDU_ASSIGN               0x1AE9//      Назначение адресов, типов ASDU и групп для отдачи мастеру                      WORD[]
#define     SP_USARTPRTPAR_BYTE                  0x1BE9//      Параметры Usart сети байтовые                                                  BYTE[]
#define     SP_USARTPRTPAR_WORD                  0x1CE9//      Параметры Usart сети двухбайтовые                                              WORD[]
#define     SP_USARTPRTPAR_DWORD                 0x1DE9//      Параметры Usart сети четырехбайтовые                                           DWORD[]
#define     SP_USARTINTNET_DEFINITION            0x1EE9//      Описание внутренней системной сети USART                                       BYTE[]
#define     SP_USARTEXTNET_DEFINITION            0x1FE9//      Описание внешних сетей USART                                                   BYTE[][]
#define     SP_USARTERROR_FLAGS                  0x20E9//      Флаги ошибок сетей USART                                                       BYTE[][]
#define     SP_CHOOSE_STREAM                     0x21E9//      SV. Выбор потока для тестов                                                    BYTE[]
#define     SP_CONTROL_SIMUL_BIT                 0x22E9//      SV. Бит Simulate в поле Reserved                                               BYTE[]
#define     SP_IFACE_OPTIONS                     0x2BE9//      Опции интерфейсов                                                              BYTE[]
#define     SP_SVOUT_FRQ                         0x2CE9//      Частота дискретизации выходного потока SV                                      BYTE[]
#define     SP_SIMCARD_DATA                      0x2DE9//      Информация с SIM карты                                                         STRING[]
#define     SP_PHONE_NUMBER                      0x2EE9//      Номер телефона                                                                 STRING[]
#define     SP_SIMCARD_NUM                       0x2FE9//      Записать тдф. Номер на СИМ карту                                               BYTE
#define     SP_GSM_QUAL                          0x30E9//      Качество GSM сигнала                                                           BYTE[]
#define     SP_TCPIP_VER                         0x31E9//      Версия TCP/IP стека                                                            WORD
#define     SP_IEC_103_PARAMS                    0x32E9//      Параметры протокола МЭК 103                                                    WORD[]
#define     SP_IEC_103_MASK_DINS                 0x33E9//      Маска дискретных входов (опрос+спорадика) протокола МЭК 103                    BYTE[]
#define     SP_CONFREV_SV                        0x34E9//      Версия конфигурации  SV                                                        DWORD[]
#define     SP_SPODES_PARAMS                     0x35E9//      Параметры протокола СПОДЭС                                                     WORD[]
#define     SP_SPODES_PASSW                      0x36E9//      Пароль СПОДЭС (для ведомого)                                                   STRING
#define     SP_SPODES_MASK                       0x37E9//      Маска чтения сигналов СПОДЭС (Din,Ain,Cin)                                     BYTE[][]
#define     SP_INPUTITEM_DFN                     0x38E9//      описание и состав входящих данных                                              DWORD[]
#define     SP_OUTPUTITEM_DFN                    0x39E9//      описание и состав исходящих данных                                             DWORD[]
#define     SP_GOCBREF_NAM                       0x40E9//      Название блока управления сообщений МЭК61850                                   STRING[]
#define     SP_GODATASET_NAM                     0x41E9//      Название набора данных протоколов МЭК61850                                     STRING[]
#define     SP_SYBUSSLAVEDEFSTRUCT               0x42E9//      Размеры полей структуры описания слейвов sybus                                 WORD[]
#define     SP_61850_SUPLIST                     0x43E9//      Список поддерживаемых протоколов МЭК61850                                      BYTE[]
#define     SP_MMSBFAINSZ                        0x44E9//      Размер буфера при вывода данных в ММС                                          WORD[]
#define     SP_MODE_SV                           0x45E9//      Режим работы узла SV согласно 61850                                            BYTE[]

                                                       //      Задача DOU - выходные дискреты
#define     SP_DOUT_PHY                          0x00EA//      Обслуживание физических дискретных выходов                                     BYTE[..]
#define     SP_DOUT_TYPE                         0x06EA//      Функциональное назначение дискр. Выходов                                       BYTE[..]
#define     SP_DOUTFRMCUR                        0x07EA//      Текущий формат выдачи выходых дискр-в                                          BYTE
#define     SP_FOUTCONST                         0x08EA//      постоянная счетчика по выходной частоте                                        FLOAT[]
#define     SP_DOUT_INVERS                       0x09EA//      Инверсия состояния дискретного выхода                                          BYTE[..]
#define     SP_DOUT_FLASH_PERIOD                 0x0AEA//      Период меандра на выходе                                                       WORD[]
#define     SP_APV_TIME                          0x0BEA//      Время АПВ                                                                      WORD
#define     SP_CHARGE_TIME                       0x0CEA//      Время о окончания переходных процессов (учет инерционности нагрузки)           WORD
#define     SP_DOUT_LEVELS                       0x0DEA//      Пороги дискретного выхода                                                      WORD[]
#define     SP_DOUT_TUNING                       0x0EEA//      Оперативные настройки дискретного выхода                                       WORD[]
#define     SP_DOUT_BLOCK                        0x0FEA//      Блокировка выдачи управления                                                   BYTE[]
#define     SP_DOUT_NAME                         0x10EA//      Название дискретных выходов                                                    STRING
#define     SP_DOUT_CLONE                        0x11EA//      Номер клона выхода управления                                                  BYTE[]
#define     SP_DOUT_CTRLSELECT                   0x12EA//      Маска выходов с режимом выбор-исполнение                                       BYTE[]
#define     SP_DOUT_SAMPLTIM                     0x8AEA//      Длительность импульса по умолчанию                                             FLOAT[]
#define     SP_DOUT_FUNCLIST                     0x8BEA//       Список функций виртуальных выходов                                            BYTE[]
#define     SP_VIRTDOUT_NAME                     0x8CEA//      Названия виртуальных дискретных выходов                                        STRING


                                                       //      Задача DIN
#define     SP_DIN_PHY                           0x00EB//      Состояние физических входов                                                    BYTE[..]
#define     SP_DIN_FRMDEF                        0x01EB//      Формат выдачи входых дискр-в по умолчанию                                      BYTE
#define     SP_DINFRMCUR                         0x02EB//      Текущий формат выдачи входых дискр-в                                           BYTE
#define     SP_DIN_ALG                           0x03EB//      Алгоритм обработки DIN                                                         BYTE[]
#define     SP_DIN_BREAK                         0x04EB//      Порог 1  (обрыв)                                                               BYTE[]
#define     SP_DIN_OFF                           0x05EB//      Порог 2  (лог 0)                                                               BYTE[]
#define     SP_DIN_ON                            0x06EB//      Порог 3  (лог 1)                                                               BYTE[]
#define     SP_DIN_SHORT                         0x07EB//      Порог 4  (КЗ)                                                                  BYTE[]
#define     SP_DIN_INT_CNT                       0x08EB//      База счетчика дребезга/интегрирования                                          BYTE[]
#define     SP_DIN_TYPE                          0x09EB//      Функциональное назначение канала                                               BYTE[]
#define     SP_DIN_NAME                          0x0AEB//      Название канала DIN                                                            STRING[]
#define     SP_DIN_TYPE_NAME                     0x0BEB//      Название функции канала DIN                                                    STRING[]
#define     SP_DIN_LIN_CLB                       0x0CEB//      Коэфф. 1-го порядка калибровки канала DIN                                      FLOAT[]
#define     SP_DIN_OFFSET_CLB                    0x0DEB//      Коэфф. 0-го порядка калибровки канала DIN                                      FLOAT[]
#define     SP_DIN_TO_LOW                        0x0EEB//      Порог логического 0                                                            FLOAT[]
#define     SP_DIN_TO_HIGH                       0x0FEB//      Порог логической 1                                                             FLOAT[]
#define     SP_DIN_INVERS                        0x10EB//      Инверсия состояния дискретного входа                                           BYTE[..]
#define     SP_DIN_TO_BREAK                      0x11EB//      Порог состояния обрыв                                                          FLOAT[]
#define     SP_DIN_TO_SHORT                      0x12EB//      Порог состояния КЗ                                                             FLOAT[]
#define     SP_DIN_TABLEPARS                     0x13EB//      Таблицы параметров обработки каналов DIN                                       BYTE[][]
#define     SP_DIN_ADCOFFSET                     0x14EB//      Смещение 0-ля кода АЦП                                                         WORD[]
#define     SP_DIN_INT_WDT                       0x15EB//      База сторожевого таймера обработки дребезга                                    BYTE[][]
#define     SP_DIN_NOISE_WDT                     0x16EB//      База счетчика отстройки от помех                                               BYTE[][]
#define     SP_IDIN_UPDPAR                       0x17EB//      Параметры обработки системных дискретов                                        BYTE[][]
#define     SP_DIN_CASH_REQMASK                  0x18EB//      Маска записи изменения входных дискретов в буфер                               DWORD[][]
#define     SP_DIN_VDINSAVED                     0x19EB//      Маска виртуальных дискретов, сохраняемых при пропадении питания                BYTE[]
#define     SP_DIN_DINOSCMASK                    0x1AEB//      Маска разрешения записи осциллограммы по дискратным сигналам                   BYTE[]
#define     SP_DIN_PROPERTIES                    0x1BEB//      Флаги свойств входных дискретов                                                WORD[]
#define     SP_DIN_INITDYNAMIC                   0x1CEB//      Флаги изменения состояния из начального                                        BYTE[]
#define     SP_DIN_SENDSPORADIC                  0x1DEB//      Маска дискретов, переваемых спорадически в Sybus при изменении                 DWORD[]
#define     SP_DIN_DP_INTERMID_TIME              0x1EEB//      Максимальное время   двухпозиционного сигнала в промежуточном состоянии        BYTE[]
#define     SP_DIN_DP_COUPLES                    0x1FEB//      Номера входов двухпозиционных сигналов                                         WORD[][]
#define     SP_DIN_DP_ALG                        0x20EB//      Выбор алгоритма обработки двухпозиционного сигнала                             BYTE[]
#define     SP_DIN_VDINFIXED                     0x21EB//      Маска виртуальных дискретов,с фиксацией в 1                                    BYTE[]
#define     SP_DIN_DEBPARS_DOUBLE                0x22EB//      Коэффициент параметров обработки дребезга                                      BYTE[]
#define     SP_DIN_DPSREPRESENT                  0x23EB//      Представление дискрета в формате double point                                  T.32BIT[]
#define     SP_1PPS_EDGE                         0x24EB//      Фронт сигнала 1PPS                                                             BYTE
#define     SP_1PPS_DELAY                        0x25EB//      Задержка сигнала 1PPS                                                          FLOAT
#define     SP_DIN_DP_INTERMID_TIME_COEF         0x26EB//      Коэффициент максимального времени   двухпозиционного сигнала                   BYTE
#define     SP_VIRTDIN_NAME                      0x27EB//      Названия виртуальных дискретных входов                                         STRING
#define     SP_DIN_DP_RESNAME                    0x28EB//      Номер сформированного дискрета в формате DP                                    WORD[]
#define     SP_DIN_REJECT_MODE                   0x29EB//      Маска разрешения формирования исмпульса режекции                               WORD[]
#define     SP_DIN_REJECT_THRESHOLD              0x2AEB//      Пороги импульса режекции                                                       FLOAT[]
#define     SP_DIN_REJECT_TIME_PARAMS            0x2BEB//      Временные параметры импульса режекции                                          DWORD



                                                       //      Задача AIN
#define     SP_AIN_ALG                           0x00EC//      Алгоритм обработки AIN (режим аналога)                                         BYTE[]
#define     SP_AIN_ALGMNG                        0x01EC//      Запуск/останов алгоритмов                                                      BYTEBOOL[]
#define     SP_AIN_CONNECT_TYPE                  0x02EC//      Разновидность схемы подсоединения                                              BYTE
#define     SP_AIN_SENSIBILITY                   0x03EC//      Чувствительность аналога                                                       FLOAT[]
#define     SP_AIN_CLBK1                         0x04EC//      Коэфф. 1-го порядка калибровки канала AIN                                      FLOAT[]
#define     SP_AIN_CLBK0                         0x05EC//      Коэфф. 0-го порядка калибровки канала AIN                                      FLOAT[]
#define     SP_AIN_WORKMODE                      0x06EC//      Штатный режим работы канала                                                    BYTE[]
#define     SP_AIN_PHYK1                         0x07EC//      Коэфф. 1-го порядка калибровки физ. Вел-ны                                     FLOAT[]
#define     SP_AIN_PHYK0                         0x08EC//      Коэфф. 0-го порядка калибровки физ. Вел-ны                                     FLOAT[]
#define     SP_AIN_PHYLVL1                       0x0AEC//      Порог 1 физической величины                                                    FLOAT[]
#define     SP_AIN_PHYLVL2                       0x0BEC//      Порог 2 физической величины                                                    FLOAT[]
#define     SP_AIN_ALGLK1                        0x10EC//      Коэфф. 1  алгоритма                                                            FLOAT[]
#define     SP_AIN_ALGLK2                        0x11EC//      Коэфф. 2  алгоритма                                                            FLOAT[]
#define     SP_AIN_ALGLK3                        0x12EC//      Коэфф. 3  алгоритма                                                            FLOAT[]
#define     SP_AIN_WINDOW                        0x13EC//      Окно измерения                                                                 WORD[]
#define     SP_AIN_MODE_NAME                     0x14EC//      Название режима аналога                                                        STRING[]
#define     SP_AIN_UNIT_NAME                     0x15EC//      Единица измерениия физической величины                                         STRING[]
#define     SP_AIN_LONG_MODE_NAME                0x16EC//      Название режима аналога 16 символов                                            STRING[]
#define     SP_AIN_CLB_X_COORD                   0x17EC//      Координата X точки градуировочной таблицы                                      FLOAT[]
#define     SP_AIN_CLB_Y_COORD                   0x18EC//      Координата Y точки градуировочной таблицы                                      FLOAT[]
#define     SP_AIN_GRAD_SELECT                   0x19EC//      Привязка аналога к градуировачной таблице 0xXXYY                               WORD[]
#define     SP_AIN_DSP_PARS_FIXP                 0x1AEC//      Параметры цифровой обработки сигналов - целые                                  WORD[][]
#define     SP_AIN_DSP_PARS_FLTP                 0x1BEC//      Параметры цифровой обработки сигналов - c плав. точкой                         FLOAT[][]
#define     SP_AIN_VAL1                          0x1CEC//      Текущее значение аналогового сигнала 1                                         FLOAT[]
#define     SP_AIN_VAL2                          0x1DEC//      Текущее значение аналогового сигнала 2                                         FLOAT[]
#define     SP_AIN_PHAK1                         0x1EEC//      Коэфф. 1-го порядка калибровки фазы кнала AIN                                  FLOAT[]
#define     SP_AIN_PHAK0                         0x1FEC//      Коэфф. 0-го порядка калибровки фазы кнала AIN                                  FLOAT[]
#define     SP_QUAL_SeqPhz                       0x20EC//      Чередование фаз                                                                BYTE
#define     SP_QUAL_SubHarm                      0x21EC//      Тип группирования субгармоник                                                  BYTE
#define     SP_QUAL_Harm                         0x22EC//      Тип группирования гармоник                                                     BYTE
#define     SP_FIO_TypeFIO                       0x23EC//      Тип регистрации FIO: на полупериоде или на периоде                             BYTE
#define     SP_FIO_Opornoe                       0x24EC//      Выбор опорного напряжения: номинальное, согласованное, скользящее среднее      BYTE
#define     SP_FIO_Sogl                          0x25EC//      Значение согласованного напряжения                                             FLOAT
#define     SP_FIO_Porog                         0x26EC//      Пороги определения провалов и перенапряжений                                   FLOAT[]
#define     SP_PKE_MAIN                          0x27EC//      Основные параметры уставок для вычисления статистических показателей ПКЕ       FLOAT[]
#define     SP_PKE_HarmPD                        0x28EC//      Уставки для вычисления статистических показателей ПКЕ для гармоник предельно доFLOAT[]
#define     SP_PKE_HarmND                        0x29EC//      Уставки для вычисления статистических показателей ПКЕ для гармоник нормально доFLOAT[]
#define     SP_PKE_Time                          0x30EC//      Уставки временных интервалов нагрузок для вычисления статистических показателейDWORD[]
#define     SP_PKE_Oscl                          0x31EC//      Уставки осциллографирования (вкл/выкл)                                         BYTE[..]
#define     SP_PKE_OsclPol                       0x32EC//      Правила осцилографирования                                                     WORD[..]
#define     SP_AIN_APERTURE_VAL                  0x33EC//      Значение апертуры изменения аналогового сигнала                                FLOAT[]
#define     SP_AIN_APERTURE_TYPE                 0x34EC//      Тип апертуры изменения аналогового сигнала                                     BYTE[..]
#define     SP_AIN_CHANNAME                      0x35EC//      Задаваемые пользователем названия каналов                                      STRING[]
#define     SP_AIN_ENRGLOST                      0x36EC//      Коэффициент потерь энергии для учета                                           FLOAT[..]
#define     SP_AIN_ENTIMEPRF                     0x37EC//      Период времени для профиля нагрузки                                            BYTE[..]
#define     SP_AIN_IPUTTARIF                     0x38EC//      Тип источника для импульсов тарификации                                        BYTE
#define     SP_AIN_NUMIMPTARIF                   0x39EC//      Число импульсов тарификации на 1квт*час энергии                                DWORD
#define     SP_AIN_IREMLVL                       0x3AEC//      Пороги аналоговых сигналов для загрузки в ведомые устройтва                    FLOAT[..]
#define     SP_PKE_Unom                          0x3BEC//      Номинальное значение напряжения                                                FLOAT[..]
#define     SP_PKE_Inom                          0x3CEC//      Номинальное значение тока                                                      FLOAT[..]
#define     SP_En_StateSeal                      0x3DEC//      Состояние опечатанного объекта                                                 BYTE
#define     SP_En_DateSeal                       0x3EEC//      Дата вскрытия печати                                                           T.STRICT
#define     SP_En_ToSeal                         0x3FEC//      Опечатать объект                                                               BYTE
#define     SP_RZA_TIME                          0x40EC//      Выдержки и задержки для алгоритмов РЗА                                         FLOAT[..]
#define     SP_RZA_LEVEL                         0x41EC//      Уровни и уставки для алгоритмов РЗА                                            FLOAT[..]
#define     SP_RZA_KOEFF                         0x42EC//      Коэффициенты для алгоритмов РЗА                                                FLOAT[..]
#define     SP_RZA_ALG                           0x43EC//      Выбор алгоритмов РЗА                                                           BYTE[]
#define     SP_RZA_ALGMNG                        0x44EC//      Управление алгоритмами РЗА                                                     BYTEBOOL[]
#define     SP_RZA_THRESHOLDS                    0x45EC//      Пороги запуска алгоритмов РЗА                                                  FLOAT[..]
#define     SP_AIN_REFABR                        0x46EC//      Восстановление калибровок                                                      BYTE[]
#define     SP_AIN_PROPERTIES                    0x47EC//      Флаги свойств входных аналогов                                                 WORD[]
#define     SP_AIN_FRQPH                         0x48EC//      Номер входа для измерения частоты основной гармоники                           BYTE[]
#define     SP_AIN_SENDSPORADIC                  0x49EC//      Маска аналогов, переваемых спорадически в Sybus при изменении                  DWORD[]
#define     SP_MMSCFGFILE                        0x4AEC//      Имя файла конфигурации ММS (полный путь)                                       BYTE[]
#define     SP_JSCFGFILE                         0x4BEC//      Путь к папке с JS файлами для рисования графиков                               BYTE[]
#define     SP_MMSTRGMOD                         0x4CEC//      Режим пуска записи (осциллографирование). Внешний, внутренний, оба             BYTE
#define     SP_MMSRCDMOD                         0x4DEC//      Режим перезаписи  (осциллографирование).  (Перезапись, Останов)                BYTE
#define     SP_I_SENCE_TYPE                      0x4EEC//      Тип датчика тока                                                               BYTE
#define     SP_VECTOR_DIAGRAMMA                  0x4FEC//      Прочитать магнитуды-углы для диаграммы                                         FLOAT[..]
#define     SP_SVIN_ON                           0x50EC//      Включение приема SV (1)                                                        BYTE
#define     SP_SVIN_REMAP                        0x51EC//      Включено замещение каналами SV каналов AFE (1)                                 BYTE
#define     SP_SVIN_TYPESV                       0x52EC//      Тип входящего SV потока                                                        BYTE[]
#define     SP_SVIN_TYPEMAP                      0x53EC//      Тип отображение входных SV на входные каналы устройства (0..7,0xaa,0xff)       BYTE
#define     SP_SVIN_NUMSTREAM                    0x54EC//      Число входящих SV потоков (1..4)                                               BYTE
#define     SP_SVIN_FLTR                         0x55EC//      Включить (1) фильтр входящих SV потоков                                        BYTE
#define     SP_SVIN_FILTRMAC                     0x56EC//      MAC адреса входящих SV потоков                                                 OUI
#define     SP_SVIN_FILTRAPPID                   0x57EC//      AppID входящих SV потоков                                                      WORD[]
#define     SP_SVIN_VLAN                         0x58EC//      VLAN входящих SV потоков                                                       WORD[]
#define     SP_SVIN_NAME                         0x59EC//      Имена входящих SV потоков                                                      STRING[]
#define     SP_SVIN_TypChn                       0x5AEC//      Тип каналов SV (0-U,1-I,2-Irus)                                                BYTE[][]
#define     SP_SVIN_FPMLNG                       0x5BEC//      SV: частота дискретизации (4000,4800,12800,14400)                              WORD
#define     SP_SVIN_NMASDU                       0x5CEC//      Число ASDU в пакете SV                                                         BYTE[]
#define     SP_SVIN_NMCHAN                       0x5DEC//      Число каналов в ASDU потока                                                    BYTE[]
#define     SP_SVIN_TypeFSK                      0x5EEC//      Поток в стандарте ФСК (1)                                                      BYTE[]
#define     SP_SVIN_Maps                         0x5FEC//      Svin;канал AFE<=>SV                                                            BYTE[]
#define     SP_SVIN_SVONLY                       0x60EC//      SvIn: Использовать каналы SV. Каналы AFE не использовать!                      BYTE[]
#define     SP_LIBIEC61850_VER                   0x70EC//      61850.Номер версии пакета libiec61850                                          BYTE[]
#define     SP_SVIN_SVLOSTDATA                   0x71EC//      SVin: Бит сигнализации потери данных                                           Byte
#define     SP_SVIN_SYNCIgnor                    0x72EC//      SVin: Игнорировать отсутствие синхронизации при формировании сигнала "Ошибка прByte
#define     SP_SV_RMS                            0x73EC//      StatSvIn: Поканальное значение RMS в принимаемых потоках                       FLOAT[]
#define     SP_VIRTAIN_NAME                      0x74EC//      Названия виртуальных аналоговых входов                                         STRING[]

                                                       //      Задача CIN
#define     SP_CIN_POWERS                        0x08ED//      Мощности по каналам (накопления за секунду)                                    DWORD[]
#define     SP_CIN_SCALE                         0x09ED//      Коэфф. 1-го порядка калибровки канала CIN                                      FLOAT[]
#define     SP_CIN_POWERS_F                      0x0AED//      Мощности по каналам (накопления за секунду)                                    FLOAT[]
#define     SP_CIN_APERTURE                      0x0BED//      Апертура изменения счетчика                                                    DWORD[]
#define     SP_CIN_SENDSPORADIC                  0x0CED//      Маска счетчиков, переваемых спорадически в Sybus при изменении                 DWORD[]
#define     SP_CIN_CHANNAME                      0x0DED//      Задаваемые пользователем названия каналов                                      STRING[]
#define     SP_CIN_APERTUREDVALUES               0x0EED//      Апертурные значения счетчиков                                                  DWORD[]
#define     SP_VIRTCIN_NAME                      0x0FED//      Названия виртуальных счетчиков                                                 STRING[]
#define     SP_CIN_RESET                         0x10ED//      Сброс счетчиков                                                                BYTE[]

                                                       //      Задача AOUT
#define     SP_AOUT_ON                           0x00EF//      Включить каналы, согласно уставкам                                             BYTEBOOL
#define     SP_AOUT_CLBK1                        0x01EF//      Коэфф. 1-го порядка калибровки канала AOUT                                     FLOAT[]
#define     SP_AOUT_CLBK0                        0x02EF//      Коэфф. 0-го порядка калибровки канала AOUT                                     FLOAT[]
#define     SP_AOUT_VAL                          0X03EF//      Уставка канала AOUT                                                            FLOAT[]
#define     SP_AOUT_FREQ                         0x04EF//      Частота канала AOUT                                                            FLOAT[]
#define     SP_AOUT_GRAD                         0x05EF//      Градиент изменения аналогового выхода
#define     SP_AOUT_SELCMD                       0x06EF//      Маска выходов с режимом выбор-исполнение                                       BYTE[]

                                                       //      Задача ARHIV
#define     SP_TREND_DURATION                    0x00EE//      Продолжительность регистрации события                                          FLOAT[]
#define     SP_TREND_LEGAL                       0x01EE//      Маска достоверности трендов                                                    BYTE[..]
#define     SP_TRENDS_AMOUNT                     0x02EE//      Максимальное количество трендов                                                WORD
#define     SP_TREND_AFT_EVENT                   0x03EE//      Продолжительность регистрации после инициирующего события                      FLOAT[]
#define     SP_TREND_INITIALISE                  0x04EE//      Битовая маска событий инициирующих сохранение тренда                           DWORD
#define     SP_TREND_CH_NAME                     0x05EE//      Названия каналов составляющих тренд                                            STRING16[]
#define     SP_APERTUR_VAL                       0x06EE//      Относительная апертура архивирования аналога                                   FLOAT[]
#define     SP_SELECT_MASK                       0x07EE//      Маска наблюдения /архивирования сигналов                                       BYTE[][]
#define     SP_AIN_TO_ARCH                       0x08EE//      Заказ аналогов на архивирование                                                BYTE[]
#define     SP_AIN_TO_APERT                      0x09EE//      Назанчение аналога на апертуру                                                 BYTE[]
#define     SP_ARCDELETE                         0x0AEE//      Стереть архив                                                                  BYTE
#define     SP_QLT_FLT                           0x10EE//      Показатели качества типа FLOAT                                                 FLOAT[]
#define     SP_LOGRECNUM                         0x11EE//      Количество записей в журнале                                                   DWORD
#define     SP_LOGSTRTNUM                        0x12EE//      Номер начальной записи вычитывания                                             DWORD
#define     SP_LOGRECORDS                        0x13EE//      Записи журнала                                                                 STRING[]
#define     SP_EVARCVER                          0x14EE//      Версия журнала событий                                                         WORD
#define     SP_AINS_TO_OSC                       0x15EE//      Номера аналоговых сигналов для осциллографирования                             WORD[]
#define     SP_OSC_LIMITS                        0x16EE//      Особенности и ограничения осциллографа                                         DWORD[]
#define     SP_TREND_BODY0                       0x80EE//      осциллограмма №1                                                               byte16[]
#define     SP_TREND_BODY1                       0x81EE//      осциллограмма №2                                                               byte16[]
#define     SP_TREND_BODY2                       0x82EE//      осциллограмма №3                                                               byte16[]
#define     SP_TREND_BODY3                       0x83EE//      осциллограмма №4                                                               byte16[]
#define     SP_TREND_BODY4                       0x84EE//      осциллограмма №5                                                               byte16[]
#define     SP_TREND_BODY5                       0x85EE//      осциллограмма №6                                                               byte16[]
#define     SP_TREND_BODY6                       0x86EE//      осциллограмма №7                                                               byte16[]
#define     SP_TREND_BODY7                       0x87EE//      осциллограмма №8                                                               byte16[]
#define     SP_TREND_BODY8                       0x88EE//      осциллограмма №9                                                               byte16[]
#define     SP_TREND_BODY9                       0x89EE//      осциллограмма №10                                                              byte16[]
#define     SP_TREND_BODY10                      0x8AEE//      осциллограмма №11                                                              byte16[]
#define     SP_TREND_BODY11                      0x8BEE//      осциллограмма №12                                                              byte16[]
#define     SP_TREND_BODY12                      0x8CEE//      осциллограмма №13                                                              byte16[]
#define     SP_TREND_BODY13                      0x8DEE//      осциллограмма №14                                                              byte16[]
#define     SP_TREND_BODY14                      0x8EEE//      осциллограмма №15                                                              byte16[]
#define     SP_TREND_BODY15                      0x8FEE//      осциллограмма №16                                                              byte16[]
#define     SP_TREND_BODY16                      0x90EE//      осциллограмма №17                                                              byte16[]
#define     SP_TREND_BODY17                      0x91EE//      осциллограмма №18                                                              byte16[]
#define     SP_TREND_BODY18                      0x92EE//      осциллограмма №19                                                              byte16[]
#define     SP_TREND_BODY19                      0x93EE//      осциллограмма №20                                                              byte16[]
#define     SP_TREND_BODY20                      0x94EE//      осциллограмма №21                                                              byte16[]
#define     SP_TREND_BODY21                      0x95EE//      осциллограмма №22                                                              byte16[]
#define     SP_TREND_BODY22                      0x96EE//      осциллограмма №23                                                              byte16[]
#define     SP_TREND_BODY23                      0x97EE//      осциллограмма №24                                                              byte16[]
#define     SP_TREND_BODY24                      0x98EE//      осциллограмма №25                                                              byte16[]
#define     SP_TREND_BODY25                      0x99EE//      осциллограмма №26                                                              byte16[]
#define     SP_TREND_BODY26                      0x9AEE//      осциллограмма №27                                                              byte16[]
#define     SP_TREND_BODY27                      0x9BEE//      осциллограмма №28                                                              byte16[]
#define     SP_TREND_BODY28                      0x9CEE//      осциллограмма №29                                                              byte16[]
#define     SP_TREND_BODY29                      0x9DEE//      осциллограмма №30                                                              byte16[]
#define     SP_TREND_BODY30                      0x9EEE//      осциллограмма №31                                                              byte16[]
#define     SP_TREND_BODY31                      0x9FEE//      осциллограмма №32                                                              byte16[]
#define     SP_TREND_BODY32                      0xA0EE//      осциллограмма №33                                                              byte16[]
#define     SP_TREND_BODY33                      0xA1EE//      осциллограмма №34                                                              byte16[]
#define     SP_TREND_BODY34                      0xA2EE//      осциллограмма №35                                                              byte16[]
#define     SP_TREND_BODY35                      0xA3EE//      осциллограмма №36                                                              byte16[]
#define     SP_TREND_BODY36                      0xA4EE//      осциллограмма №37                                                              byte16[]
#define     SP_TREND_BODY37                      0xA5EE//      осциллограмма №38                                                              byte16[]
#define     SP_TREND_BODY38                      0xA6EE//      осциллограмма №39                                                              byte16[]
#define     SP_TREND_BODY39                      0xA7EE//      осциллограмма №40                                                              byte16[]
#define     SP_TREND_BODY40                      0xA8EE//      осциллограмма №41                                                              byte16[]
#define     SP_TREND_BODY41                      0xA9EE//      осциллограмма №42                                                              byte16[]
#define     SP_TREND_BODY42                      0xAAEE//      осциллограмма №43                                                              byte16[]
#define     SP_TREND_BODY43                      0xABEE//      осциллограмма №44                                                              byte16[]
#define     SP_TREND_BODY44                      0xACEE//      осциллограмма №45                                                              byte16[]
#define     SP_TREND_BODY45                      0xADEE//      осциллограмма №46                                                              byte16[]
#define     SP_TREND_BODY46                      0xAEEE//      осциллограмма №47                                                              byte16[]
#define     SP_TREND_BODY47                      0xAFEE//      осциллограмма №48                                                              byte16[]
#define     SP_TREND_BODY48                      0xB0EE//      осциллограмма №49                                                              byte16[]
#define     SP_TREND_BODY49                      0xB1EE//      осциллограмма №50                                                              byte16[]
#define     SP_TREND_BODY50                      0xB2EE//      осциллограмма №51                                                              byte16[]
#define     SP_TREND_BODY51                      0xB3EE//      осциллограмма №52                                                              byte16[]
#define     SP_TREND_BODY52                      0xB4EE//      осциллограмма №53                                                              byte16[]
#define     SP_TREND_BODY53                      0xB5EE//      осциллограмма №54                                                              byte16[]
#define     SP_TREND_BODY54                      0xB6EE//      осциллограмма №55                                                              byte16[]
#define     SP_TREND_BODY55                      0xB7EE//      осциллограмма №56                                                              byte16[]
#define     SP_TREND_BODY56                      0xB8EE//      осциллограмма №57                                                              byte16[]
#define     SP_TREND_BODY57                      0xB9EE//      осциллограмма №58                                                              byte16[]
#define     SP_TREND_BODY58                      0xBAEE//      осциллограмма №59                                                              byte16[]
#define     SP_TREND_BODY59                      0xBBEE//      осциллограмма №60                                                              byte16[]
#define     SP_TREND_BODY60                      0xBCEE//      осциллограмма №61                                                              byte16[]
#define     SP_TREND_BODY61                      0xBDEE//      осциллограмма №62                                                              byte16[]
#define     SP_TREND_BODY62                      0xBEEE//      осциллограмма №63                                                              byte16[]
#define     SP_TREND_BODY63                      0xBFEE//      осциллограмма №64                                                              byte16[]
#define     SP_TREND_BODY64                      0xC0EE//      осциллограмма №65                                                              byte16[]
#define     SP_TREND_BODY65                      0xC1EE//      осциллограмма №66                                                              byte16[]
#define     SP_TREND_BODY66                      0xC2EE//      осциллограмма №67                                                              byte16[]
#define     SP_TREND_BODY67                      0xC3EE//      осциллограмма №68                                                              byte16[]
#define     SP_TREND_BODY68                      0xC4EE//      осциллограмма №69                                                              byte16[]
#define     SP_TREND_BODY69                      0xC5EE//      осциллограмма №70                                                              byte16[]
#define     SP_TREND_BODY70                      0xC6EE//      осциллограмма №71                                                              byte16[]
#define     SP_TREND_BODY71                      0xC7EE//      осциллограмма №72                                                              byte16[]
#define     SP_TREND_BODY72                      0xC8EE//      осциллограмма №73                                                              byte16[]
#define     SP_TREND_BODY73                      0xC9EE//      осциллограмма №74                                                              byte16[]
#define     SP_TREND_BODY74                      0xCAEE//      осциллограмма №75                                                              byte16[]
#define     SP_TREND_BODY75                      0xCBEE//      осциллограмма №76                                                              byte16[]
#define     SP_TREND_BODY76                      0xCCEE//      осциллограмма №77                                                              byte16[]
#define     SP_TREND_BODY77                      0xCDEE//      осциллограмма №78                                                              byte16[]
#define     SP_TREND_BODY78                      0xCEEE//      осциллограмма №79                                                              byte16[]
#define     SP_TREND_BODY79                      0xCFEE//      осциллограмма №80                                                              byte16[]
#define     SP_TREND_BODY80                      0xD0EE//      осциллограмма №81                                                              byte16[]
#define     SP_TREND_BODY81                      0xD1EE//      осциллограмма №82                                                              byte16[]
#define     SP_TREND_BODY82                      0xD2EE//      осциллограмма №83                                                              byte16[]
#define     SP_TREND_BODY83                      0xD3EE//      осциллограмма №84                                                              byte16[]
#define     SP_TREND_BODY84                      0xD4EE//      осциллограмма №85                                                              byte16[]
#define     SP_TREND_BODY85                      0xD5EE//      осциллограмма №86                                                              byte16[]
#define     SP_TREND_BODY86                      0xD6EE//      осциллограмма №87                                                              byte16[]
#define     SP_TREND_BODY87                      0xD7EE//      осциллограмма №88                                                              byte16[]
#define     SP_TREND_BODY88                      0xD8EE//      осциллограмма №89                                                              byte16[]
#define     SP_TREND_BODY89                      0xD9EE//      осциллограмма №90                                                              byte16[]
#define     SP_TREND_BODY90                      0xDAEE//      осциллограмма №91                                                              byte16[]
#define     SP_TREND_BODY91                      0xDBEE//      осциллограмма №92                                                              byte16[]
#define     SP_TREND_BODY92                      0xDCEE//      осциллограмма №93                                                              byte16[]
#define     SP_TREND_BODY93                      0xDDEE//      осциллограмма №94                                                              byte16[]
#define     SP_TREND_BODY94                      0xDEEE//      осциллограмма №95                                                              byte16[]
#define     SP_TREND_BODY95                      0xDFEE//      осциллограмма №96                                                              byte16[]
#define     SP_TREND_BODY96                      0xE0EE//      осциллограмма №97                                                              byte16[]
#define     SP_TREND_BODY97                      0xE1EE//      осциллограмма №98                                                              byte16[]
#define     SP_TREND_BODY98                      0xE2EE//      осциллограмма №99                                                              byte16[]
#define     SP_TREND_BODY99                      0xE3EE//      осциллограмма №100                                                             byte16[]


                                                       //      Задача Статистики
#define     SP_PKE_StartDayWeek                  0x00F0//      Начальный день расчета недельной статистики (0..6)                             BYTE[..]
#define     SP_PKE_WeekStatDayly                 0x01F0//      Недельная статистика ежедневно (1)                                             BYTE[..]
#define     SP_PKE_TypePKE                       0x02F0//      Тип среза ПКЕ (0..3)                                                           BYTE[..]
#define     SP_PKE_TimeUstPKE                    0x03F0//      Параметры времени для Уставок ПКЭ                                              DATETIME[..]
#define     SP_FLX_RUNSTAT                       0x04F0//      Статистика выполнения загружаемой логики                                       DWORD[][]
#define     SP_RESURS_SWT_FLT                    0x05F0//      Текущие данные ресурса выключателя FLOAT                                       FLOAT[]
#define     SP_RESURS_SWT_DWRD                   0x06F0//      Текущие данные ресурса выключателя TIME                                        DWORD[]
#define     SP_RESURS_SWT_BYTE                   0x07F0//      Текущие данные ресурса выключателя BYTE                                        BYTE[]


                                                       //      Задача файлы
#define     SP_FILE_FLEXLGFILES                  0x00F1//      Файл загружаемой логики                                                        STRING[]
#define     SP_FORMAT_VOLUME                     0x01F1//      Форматирование диска                                                           BYTE[..]
#define     SP_VOLUME_INFO                       0x02F1//      Размеры томов файловой системы                                                 FLOAT[..]
#define     SP_FILE_CTRLW                        0x03F1//      Канал записи команд                                                            STRING[]
#define     SP_FILE_CTRLR                        0x04F1//      Канал чтения параметров                                                        STRING[]
#define     SP_FILE_DATA                         0x05F1//      Канал данных запись/чтение                                                     BYTE[]
#define     SP_FILE_DATAOFFS                     0x06F1//      Указатель на данные в файле (смещение)                                         DWORD[]
#define     SP_VOLUME_STATE_IND                  0x07F1//      Индикация состояния тома                                                       BYTE[]
#define     SP_FILE_EMBLGFILES                   0x08F1//      Файл прикладных алгоритмов                                                     STRING[]

                                                       //      Задача конфигурирования
#define     SP_CROSSTABLE                        0x00F2//      Перевязочная таблица сигналов                                                  WORD[][]
#define     SP_FLEXLGCROSSTABLE                  0x01F2//      Перевязочная таблица сигналов загружаемой логики                               WORD[][]
#define     SP_CROSSTABLEDOUT                    0x02F2//      Перевязочная таблица сигналов управления выходами                              WORD[][]

                                                       //      Задача питания (основного/резервного/аккумуляторного)
#define     SP_MNFBAT_CAPACITY                   0x00F3//      Ёмкость аккумулятора по паспорту                                               FLOAT[..]
#define     SP_TIME_TO_SLEEP                     0x01F3//      Время перехода в режим энргосбережения/отключения                              FLOAT[..]
#define     SP_BAT_CELL_NUM                      0x02F3//      Число ячеек в аккумуляторной батарее                                           BYTE[..]
#define     SP_BAT_TYPE                          0x03F3//      Тип аккумуляторной батареи                                                     BYTE[..]
#define     SP_MAX_CHARGE_CURRENT                0x04F3//      Ограничение максимального тока заряда                                          FLOAT[..]
#define     SP_MAX_PW_SUPP_CURRENT               0x05F3//      Ограничение максимального тока блока питания                                   FLOAT[..]

                                                       //      Задача TASK.GRAPH
#define     SPP_GRAPHDSCR                        0x00F4//      Количество векторов и групп в диаграмме                                        BYTE[..]
#define     SPP_GRAPHNAMEVECT                    0x01F4//      Имена векторов                                                                 STRING[]
#define     SPP_GRAPHMEASHUNIT                   0x02F4//      Кединица измерения (ENUM)                                                      BYTE[..]
#define     SPP_GRAPHNUMBRKERN                   0x03F4//      Номер группы, к которой относится вектор                                       BYTE[..]
#define     SPP_GRAPHMEASVECT                    0x04F4//      Массив значений векторов                                                       FLOAT[..]
#define     SPP_GRAPHREFRESH                     0x05F4//      Период обновления данных векторной диаграммы (*100мс)                          BYTE

                                                       //      Шлюз
#define     SP_GATEHDR                           0x00FE//      Заголовок для шлюза                                                            BYTE[..]



                                                       //      Задача CONSOL
#define     SP_RUN_DELAY_START                   0x00FF//      Задержка на начеле бегущей строки                                              WORD
#define     SP_RUN_STEP_TIME                     0x01FF//      Время шага бегущей строки                                                      WORD
#define     SP_SYMBOL_GEN_TABL                   0x02FF//      Таблица символов (знакогенератор)                                              BYTE[][]
#define     SP_DISP_TIME_REFRESH                 0x03FF//      Время обновления экрана (счетчик, автоматическое листание)                     BYTE[..]
#define     SP_DISP_TIME_OUT                     0x04FF//      Таймаут выхода на основной экран                                               BYTE[..]
#define     SP_DISPLAY_TYPE                      0x05FF//      Тип и подключение дисплея                                                      BYTE[..]
#define     SP_DISPLAY_KEY_FUNC                  0x06FF//      Переопределение функции кнопки                                                 BYTE[..]
#define     SP_DISPLAY_FILENAME                  0x07FF//      Дисплей: полное имя файла                                                      STRING[]
#define     SP_DISPLAY_MAINSCR                   0x08FF//      Дисплей: ID главного экрана                                                    WORD[]
#define     SP_DISPLAY_FOLDER                    0x09FF//      Дисплей: имя директории с файлами                                              STRING[]
#define     SP_TERMINAL_KEY_TO_DIN               0x0AFF//      Привязка кнопок терминала к виртуальным входам                                 WORD[][]

#endif //__SMPARLIST_h
