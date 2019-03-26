using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.Resources
{
    class Utilities
    {
        private static Utilities instance;
        private Utilities()
        {
        }

        public static Utilities Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new Utilities();
                }
                return instance;
            }
        }

        public List<byte> ConvertInt16ToByteArray(Int16 number)
        {
            List<byte> reverseBytes = new List<byte>();
            reverseBytes.AddRange(BitConverter.GetBytes(number).Reverse());
            return reverseBytes;
        }

        public List<byte> ConvertInt32ToByteArray(Int32 number)
        {
            List<byte> reverseBytes = new List<byte>();
            reverseBytes.AddRange(BitConverter.GetBytes(number).Reverse());
            return reverseBytes;
        }
    }
}
