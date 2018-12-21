using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RomessSteeringwheelInclinometerInterface
{
    public class Inclinometer
    {

        //Collect all the Fields here:
        private InclinometerConnection connection;

        //Collect all of the Properties Here:
        public InclinometerConnection Connection
        {
            get { return connection; }
            set { connection = value; }
        }

        public Inclinometer()
        {
            connection = new USBConnection();
            //connection = new BluetoothConnection();
        }
    }
}
