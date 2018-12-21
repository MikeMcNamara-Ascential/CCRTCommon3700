using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RomessSteeringwheelInclinometerInterface
{
    public interface InclinometerConnection
    {
        bool Connected{ get; set; }
        string Type{ get; set; }
        bool IsClosing { get; set; }
        //byte[] Buffer { get; set; }
        /*int VenderID{ get; set; }
        int ProductID { get; set; }
        
        byte[] BufferOut { get; set; }*/

        bool ConnectToSWI();
        bool DisconnectFromSWI();
        Single ReadData(int pHandle);
    }
}
