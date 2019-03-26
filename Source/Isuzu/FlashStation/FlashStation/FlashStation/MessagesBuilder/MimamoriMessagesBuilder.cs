using Common.Models;
using Common.Resources;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.MessagesBuilder
{
    class MimamoriMessagesBuilder
    {
        public List<MimamoriModuleCanMessage> MimamoriMessageList = new List<MimamoriModuleCanMessage>();
        MimamoriFlashMessageId flashId = MimamoriFlashMessageId.Instance;
        private List<byte> requestMessageId = new List<byte>();
        private List<byte> responseMessageId = new List<byte>();
        public MimamoriMessagesBuilder()
        {
           
            AddMimamoriMessages();
        }
        private List<byte> SetRequestMessageId(List<byte> index)
        {
            requestMessageId.Clear();
            requestMessageId.AddRange(flashId.GetRequestMessageId());
            if (index != null)
                requestMessageId.AddRange(index);
            return requestMessageId;
        }

        private List<byte> SetRequestMessageId(byte index)
        {
            requestMessageId.Clear();
            requestMessageId.AddRange(flashId.GetRequestMessageId());
            if (index != null)
                requestMessageId.Add(index);
            return requestMessageId;
        }

        private List<byte> SetResponseMessageId(List<byte> index)
        {
            responseMessageId.Clear();
            responseMessageId.AddRange(flashId.GetResponseMessageId());
            if (index != null)
                responseMessageId.AddRange(index);
            return responseMessageId;
        }
        private List<byte> SetResponseMessageId(byte index)
        {
            responseMessageId.Clear();
            responseMessageId.AddRange(flashId.GetResponseMessageId());
            if (index != null)
                responseMessageId.Add(index);
            return responseMessageId;
        }
        public MimamoriModuleCanMessage CanSettingMessage()
        {
            return new MimamoriModuleCanMessage(
             "Can setting",
               SetRequestMessageId(0x35),
             SetResponseMessageId(0x35),
             new List<byte>(){0x12}
             );

        }

        public MimamoriModuleCanMessage CommunicationStart()
        {
            return new MimamoriModuleCanMessage(
             "Enter Programming Mode",
                new List<byte>() { 0x10, 0x01 },
                new List<byte>() { 0x50, 0x01, 0x00, 0x32, 0x01, 0xF4 });

        }

        public MimamoriModuleCanMessage ProgramModeMessage()
        {
            return new MimamoriModuleCanMessage(
             "Enter Programming Mode",
                new List<byte>() { 0x10, 0x03 },
                new List<byte>() { 0x50, 0x03, 0x00, 0x32, 0x01, 0xF4 });
            
        }
        public MimamoriModuleCanMessage SecurityAccessMessage()
        {
            return new MimamoriModuleCanMessage(
                "Security Access",
                new List<byte>() { 0x27, 0x01 },
                new List<byte>() { 0x67, 0x01, 0x00, 0x00 }
           );
        }
        public MimamoriModuleCanMessage MimamoriModuleVinFlash(String vin)
        {
            List<byte> data = new List<byte>();
            byte[] temp = Encoding.ASCII.GetBytes(vin);
            data.AddRange(temp);
            return new MimamoriModuleCanMessage(
            "Program VIN",
            new List<byte>() { 0x2E, 0xF1, 0x90 },
            new List<byte>() { 0x6E, 0xF1, 0x90 },
            data
             );
        }
        public MimamoriModuleCanMessage TireRadiusMessage( Int16 TireSize)
        {
            List<byte> data = new List<byte>();
            data.AddRange(Utilities.Instance.ConvertInt16ToByteArray(TireSize));
            return new MimamoriModuleCanMessage(
             "Tire Radius Data",
             SetRequestMessageId(0x32),
             SetResponseMessageId(0x32),
             data
              );
        }
        public MimamoriModuleCanMessage FinalGearRatioMessage(Int16 RearAxle)
        {
            List<byte> data = new List<byte>();
            data.AddRange(Utilities.Instance.ConvertInt16ToByteArray(RearAxle));

            return new MimamoriModuleCanMessage(
            "Final Gear Ratio",
            SetRequestMessageId(0x33),
            SetResponseMessageId(0x33),
            data
             ); 
        }
        public MimamoriModuleCanMessage TransmissionModuleModelDataMessage(String transmissionModel)
        {
            List<byte> data = new List<byte>();
            byte[] temp1 = Encoding.ASCII.GetBytes(transmissionModel);
            //   byte[] temp1 = Encoding.ASCII.GetBytes("Charlott");
            data.AddRange(temp1);
            for (int j = 0; j < (8 - transmissionModel.Length); j++)
            {
                data.Add(0x20);
            }
            return new MimamoriModuleCanMessage(
             "T/M Model Data",
               SetRequestMessageId(0x31),
               SetResponseMessageId(0x31),
               data
                );
        }
        public void AddMimamoriMessages()
        {

            MimamoriMessageList.Clear();

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
           "Diesel ECM Equipped",
           SetRequestMessageId(0x00),
           flashId.GetResponseMessageId(), new List<byte>(){0x01}
            ));
            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
                "TCM Equipped",
            SetRequestMessageId(0x01),
            flashId.GetResponseMessageId(), new List<byte>() { 0x01 }
           ));
            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
                "Turbo Controller Equipped",
         SetRequestMessageId(0x02),
         flashId.GetResponseMessageId(), new List<byte>() { 0x01 }
          ));
            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
            "SRC Equipped",
           SetRequestMessageId(0x03),
           flashId.GetResponseMessageId(), new List<byte>() { 0x01 }
            ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
             "ABS Equipped Message",
               SetRequestMessageId(0x05),
               flashId.GetResponseMessageId(), new List<byte>() { 0x01 }
                 )
                 );

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
              "Multimeter Equipped",
               SetRequestMessageId(0x09),
               flashId.GetResponseMessageId(), new List<byte>(){0x01}
                ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
              "ENG Model Data",
               SetRequestMessageId(0x30),
               SetResponseMessageId(0x30),
               new List<byte>() {  0x34, 0x48, 0x4B, 0x31, 0x54, 0x43, 0x53, 0x20 }
                ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
             "Gear Ratio",
              SetRequestMessageId( 0x34 ),
               SetResponseMessageId(0x34),
               new List<byte>() {0x06, 0x00, 0x0D, 0xB6, 0x07, 0x6C, 0x05, 0xA0, 0x03, 0xE8, 0x02, 0xE4, 0x02, 0x80, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
                ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
             "Control Swiching 1",
               SetRequestMessageId(0x36),
               SetResponseMessageId(0x36),
               new List<byte>(){0x9C}
                ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
              "Vehicle Speed Correction Coefficient",
               SetRequestMessageId(0x37),
               SetResponseMessageId(0x37),
               new List<byte>(){0x03}
                ));

           /* MimamoriMessageList.Add(new MimamoriModuleCanMessage(
            "Vehicle Model Data",
            SetRequestMessageId(0x44),
               SetResponseMessageId(0x44),
               new List<byte>() { 0x06 }
                ));
           
            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
             "Electrical Equipment Data",
               SetRequestMessageId(0x45),
                SetResponseMessageId(0x45),
                new List<byte>() { 0x0C}
                ));*/

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
             "Engine Oil Life Warning",
              SetRequestMessageId( 0x50),
                flashId.GetResponseMessageId(),
                 new List<byte>() { 0x00, 0xF5, 0x8F, 0x48 }
                ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
             "Engine Oil Life Pre-Warning",
              SetRequestMessageId(0x51),
                SetResponseMessageId(null),
                 new List<byte>() { 0x00, 0x18, 0x8E, 0x54 }
                ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
             "Starter Life Warning",
              SetRequestMessageId(0x52),
                SetResponseMessageId(null),
               new List<byte>() { 0x00, 0x00, 0x75, 0x30 }
                ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
             "Empty Weight Calculation",
              SetRequestMessageId(0x71),
                SetResponseMessageId(null),
               new List<byte>() { 0x00, 0x01}
                ));

            /*
            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
              "ACG OH Life Warning",
               SetRequestMessageId(0x5B),
                flashId.GetResponseMessageId(),
                new List<byte>() { 0x0E, 0xE6, 0xB2, 0x80 }
                ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
               "Engine Speed ACG OH Life Warning",
                SetRequestMessageId(0x5C),
               flashId.GetResponseMessageId(),
                new List<byte>() { 0x00, 0x0F, 0x42, 0x40 }
               ));

            MimamoriMessageList.Add(new MimamoriModuleCanMessage(
              "T/M OH Warning",
               SetRequestMessageId(0x5D),
               flashId.GetResponseMessageId(),
                new List<byte>() { 0x00, 0x5B, 0x8D, 0x80 }
                ));*/
           /* messageNames.Add("Read Part Number");
            txMessages.Add(new byte[] { 0x82, 0x80, 0xF1, 0x1A, 0x91 });
            messageNames.Add("Program VIN");
            txMessages.Add(new byte[] { 0x93, 0x80, 0xF1, 0x3B, 0x90 });*/
           
        }
        public List<MimamoriModuleCanMessage> getMessageList()
        {
            return MimamoriMessageList;
        }
    }
}
