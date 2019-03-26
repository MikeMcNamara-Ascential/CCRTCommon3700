using Common.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.MessagesBuilder
{
    public class MimamoriFlashMessageId : ICanMessageId
    {
        private static MimamoriFlashMessageId instance;

        private MimamoriFlashMessageId()
        {
        }
        public static MimamoriFlashMessageId Instance
        {
            get {
                if (instance == null)
                {
                    instance = new MimamoriFlashMessageId();
                }
                return instance; }
        }
        private List<byte> requestMessageId = new List<byte>(){0x2E, 0xD5};
        private List<byte> responseMessageId = new List<byte>() { 0x6E, 0xD5 };
        public List<byte> GetRequestMessageId()
        {
            return requestMessageId;
        }
        public List<byte> GetResponseMessageId()
        {
            return responseMessageId;
        }
    }
}
