/* LIN (SD_LIN) target definitions */


/* SD_LIN command types: */
#define CMD_LDF_DESC                0xB8
#define CMD_LDF_UPLOAD              0xB9
#define CMD_LDF_LIST                0xBA
#define CMD_LDF_DELETE              0xBB
#define CMD_LDF_PARSE               0xBC
#define CMD_GET_LDF_INFO            0xBD    
#define CMD_GET_NODE_NAMES          0xBE    
#define CMD_EMULATE_NODES           0xBF    
#define CMD_GET_FRAMES              0xB0   
#define CMD_GET_FRAME_INFO          0xC1       
#define CMD_GET_SIGNAL_INFO         0xC2        
#define CMD_GET_SIGNAL_DETAIL       0xC3         
#define CMD_GET_ENCODING_INFO       0xC4         
#define CMD_GET_SCHEDULES           0xC5       
#define CMD_START_SCHEDULE          0xC6       
#define CMD_STOP_SCHEDULE           0xC7   
#define CMD_STORE_DATA              0xC8   
#define CMD_SEND_ID                 0xC9
#define CMD_SEND_ID_DATA            0xCA
#define CMD_SAVE_SESSION            0xCB
#define CMD_RESTORE_SESSION         0xCC
#define CMD_GET_NODE_SIGNALS        0xCD
