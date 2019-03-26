using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.Models
{
    class MimamoriModuleCanMessage
    {
        public List<byte>RequestMessage;
        public List<byte> ResponseMessage;
        public string MessageDescription;
        static public List<byte> CanRequestId = new List<byte>(){ 0x00, 0x00, 0x02, 0x69 };
        static public List<byte> CanResponseId = new List<byte>() { 0x00, 0x00, 0x06, 0x09 };

        public MimamoriModuleCanMessage(string MessageDescription, List<byte> MessageRequestId,
            List<byte> MessageResponseId,  List<byte>data = null)
        {
            this.RequestMessage = new List<byte>();
            //this.RequestMessage.AddRange(CanRequestId);
            this.RequestMessage.AddRange(MessageRequestId);
            if (data != null)
                this.RequestMessage.AddRange(data);

            this.ResponseMessage = new List<byte>();
            //this.ResponseMessage.AddRange(CanResponseId);
            this.ResponseMessage.AddRange(MessageResponseId);
            this.MessageDescription = MessageDescription;
        }
    }
}
