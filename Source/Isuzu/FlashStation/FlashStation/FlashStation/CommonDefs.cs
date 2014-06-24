using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.Lib
{

        public enum Status
        {
            ERROR = 0xFFFFFFF,
            SUCCESS = 0,
            FAILURE,
            ABORT,
            TIMEOUT,
            SOFTWARE,
            HARDWARE,
            SKIP,
            IN_PROGRESS,
            TERMINATE
        }
        public enum StateName
        {
            WAIT_FOR_USER_INPUT = 0,
            OBTAIN_BUILD_DATA,
            WAIT_FOR_CABLECONNECT,
            //WAIT_FOR_KEY_ON,
            FLASH_ECUS,
            REPORT_DATA,
            //WAIT_FOR_KEY_OFF,
            WAIT_FOR_CABLEDISCONNECT
        }
        public class Prompt
        {
            public string OBTAIN_BUILD_DATA1 = "Obtaining Build Data",
            OBTAIN_BUILD_DATA2 = "Please Wait...",
            WAIT_FOR_CABLECONNECT1 = "Waiting For Vehicle Connect",
            WAIT_FOR_CABLECONNECT2 = "Connect Cable, Key On",
            FLASH_ECUS1 = "Flashing ECUs",
            FLASH_ECUS2 = "Do Not Disconnect Cable",
            WAIT_FOR_USER_INPUT1 = "Waiting for Barcode Input",
            WAIT_FOR_USER_INPUT2 = "Scan or Key In Barcode",
            ABORT_INVALID_BUILD_DATA1 = "Build Data Invalid",
            ABORT_INVALID_BUILD_DATA2 = "Aborting Flash Sequence",
            ABORT_INVALID_ESN_LENGTH = "ESN Invalid Length",
            ABORT_ESN_NOT_PRESENT = "No ESN File Found Vehicle",
            ABORT_INVALID_ESN_VALUE = "ESN Invalid Value",
            ABORT_USER1 = "User Aborted Flash Sequence",
            REPORT_DATA1 = "Reporting Data",
            REPORT_DATA2 = "Please Wait...",
            FAILURE = "Flash Sequence Failure",
            PASS = "Flash Sequence Complete",
            WAIT_FOR_CABLEDISCONNECT = "Key Off, Disconnect Cable",
            KEY_ON = "Key On",
            KEY_OFF = "Key Off";
        }
}
