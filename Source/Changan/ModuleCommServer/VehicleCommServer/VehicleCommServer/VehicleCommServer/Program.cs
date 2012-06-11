using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace VehicleCommServer
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
            VehicleCommServerInterface vcsInterface = new VehicleCommServerInterface();
            Application.Run(new VehicleCommSettingsForm(vcsInterface));
            Application.Run(new ECUCommunicationTestForm(vcsInterface));
        }
    }
}
