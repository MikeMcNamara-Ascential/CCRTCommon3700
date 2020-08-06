/* LIN (SD_CNVT) target definitions */


/* SD_CNVT command types: */
#define CMD_CNVT_GET_VALUES         0x78
#define CMD_CNVT_GET_UNITS          0x79
#define CMD_CNVT_SET_VALUES         0x7A
#define CMD_DESTROY_SESSION         0x7B
#define CMD_READ_CNVT_CONFIG        0x7C
#define CMD_CNVT_REQ_VALUES         0x7D
#define CMD_CNVT_REQ_SUSPEND        0x7E
#define CMD_CNVT_REQ_RESUME         0x7F
#define CMD_CNVT_REQ_MODIFY         0x80
#define CMD_CNVT_GET_MSG_NAMES      0x81
#define CMD_CNVT_GET_SIG_NAMES      0x82
#define CMD_CNVT_REQ_CANCEL         0x83

#define CNVT_FLOAT                  0x01
#define CNVT_INTEGER                0x02
#define CNVT_STRING                 0x04
#define CNVT_UNDER_RANGE            0x08
#define CNVT_OVER_RANGE             0x10
