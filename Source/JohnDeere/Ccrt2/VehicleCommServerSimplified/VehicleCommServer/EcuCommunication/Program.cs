using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace EcuCommunications
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            VehicleCommServer.VehicleCommServerInterface vcsInterface = new VehicleCommServer.VehicleCommServerInterface();
            vcsInterface.OpenVehicleCommSettingsForm();
            Application.Run(new ECUCommunicationTestForm(vcsInterface));
        }
    }
}
