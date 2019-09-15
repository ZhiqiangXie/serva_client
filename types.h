#ifndef TYPES_H
#define TYPES_H

#define SERIAL_SOF_HIGH_BYTE 0X5A
#define SERIAL_SOF_LOW_BYTE  0XA5
#define SERIAL_SOF           0xA55A

#define KEY_CTRL_STEP        5

#pragma pack(1)

/**
 * @name
 * @{ */
typedef enum
{
    eSerialFdbkCmd  = 0xaa,
    eSerialCtrCmd   = 0x55
}SerialCMDTypes_t;
/**  @} */

/**
 * @name
 * @{ */
typedef enum
{
    eUdpFdbk_BroacastCmd     = 0x00,
    eUdpFdbk_Status          = 0x01
}UdpFdbkID_t;
/**  @} */

/**
 * @name
 * @{ */
typedef enum
{
    eSerialCtrl_BroadcastCMD = 0x00,
    eSerialCtrl_VelCMD       = 0x01,
}SerialCtrlID_t;
/**  @} */

/**
 * @name
 * @{ */
typedef union
{
   struct
   {
        unsigned short sof;
        signed char lens;
        unsigned char cmd_types;
        unsigned char fdbk_id;
        unsigned char data[3];
        unsigned char check_sum;
   }frame;
   char row[1];
}UdpFdbkFrame_t;
/**  @} */

/**
 * @name
 * @{ */
typedef struct
{
    unsigned char status;
    unsigned char ctrl_mode;
    float volt;
    signed char linear;
    signed char angular;
    signed char resever;
} ChassisStatusFeedback_t;
/**  @} */

/**
 * @name chassis current status
 * @{ */
typedef enum
{
    eDisableExternalCMD      = 0x00,
    eEnableCanExternalCMD    = 0x01,
    eEnableUsartExternalCMD  = 0x02,
}CmdModdeStutus_t;
/**  @} */

/**
 * @name
 * @{ */
typedef struct
{
    CmdModdeStutus_t cmd_mode;
    signed char linear;
    signed char angular;
    unsigned char counter;
    unsigned char checkseum;
} ChassisCMD_t;
/**  @} */

/**
 * @name
 * @{ */

typedef union
{
   struct
   {
        unsigned short sof;
        signed char lens;
        unsigned char cmd_types;
        unsigned char ctrl_id;
        unsigned char data[3];
        unsigned char check_sum;
   }frame;
   char row[1];
}SerialCtrlFrame_t;
/**  @} */

#endif // TYPES_H
