using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.Interfaces
{
    interface ICanMessageId
    {
        List<byte> GetRequestMessageId();
        List<byte> GetResponseMessageId();
    }
}
