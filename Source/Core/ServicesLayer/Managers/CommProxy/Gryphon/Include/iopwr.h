/* IOPWR (SD_IOPWR) definitions */


/* SD_IOPWR commands: */
#define CMD_IOPWR_GETINP        0x40    /*  Read current digital inputs     */
#define CMD_IOPWR_GETLATCH      0x41    /*  Read latched digital inputs     */
#define CMD_IOPWR_CLRLATCH      0x42    /*  Read & clear latched inputs     */
#define CMD_IOPWR_GETOUT        0x43    /*  Read digital outputs            */
#define CMD_IOPWR_SETOUT        0x44    /*  Write digital outputs           */
#define CMD_IOPWR_SETBIT        0x45    /*  Set indicated output bit(s)     */
#define CMD_IOPWR_CLRBIT        0x46    /*  Clear indicated output bit(s)   */
#define CMD_IOPWR_GETPOWER      0x47    /*  Read inputs at power on time    */
