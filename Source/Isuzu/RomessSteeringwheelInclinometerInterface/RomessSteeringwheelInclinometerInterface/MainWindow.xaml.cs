using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
//using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace RomessSteeringwheelInclinometerInterface
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        //Collect all Fileds here
        private Inclinometer swi;
        private string messages;
        private string reading;
        private bool closing;

        //Collect all of the Properties here
        public string Connection
        {
            get { return SWI.Connection.Type; }
            set 
            { 
                SWI.Connection.Type = value;
               //ContentGrid.Children.IndexOf 
                ConnectionTypeBox.Text = SWI.Connection.Type;
            }
        }
        public bool Connected
        {
            get { return SWI.Connection.Connected; }
            set
            {
                SWI.Connection.Connected = value;
                if (SWI.Connection.Connected)
                {
                    ConnectionStausBox.Text = "Connected";
                    ConnectionStausBox.Background = new SolidColorBrush(Color.FromRgb(88, 243, 29));
                }
                else
                {
                    ConnectionStausBox.Text = "Disconnected";
                    ConnectionStausBox.Background = new SolidColorBrush(Color.FromRgb(243, 29, 29));
                }
            }
        }
        public string CCRTConnection
        {
            get;
            set;
        }
        public string CCRTConnected
        {
            get;
            set;
        }
        public string Messages
        {
            get { return messages; }
            set 
            { 
                messages = value;
                MessageBox.Text = messages;
            }
        }
        public bool isClosing 
        {
            get { return closing; }
            set { closing = value; }
        }

        public Inclinometer SWI
        {
            get { return swi; }
            set { swi = value; }
        }
        public string Reading
        {
            get { return reading; }
            set
            {
                reading = value;
                CurrentReadingBox.Text = reading;
            }
        }


      //Constructor
        public MainWindow()
        {
            isClosing = false;
            InitializeComponent();
            SWI = LoadConfiguration();
            Connection = Connection;
            Connected = Connected;
        }

        public void Append(String inStr)
        {
            Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, new ThreadStart(delegate
            {
                if (!isClosing)
                    MessageBox.Text += inStr;
            }));
        }

        public Inclinometer LoadConfiguration()
        {
            Inclinometer temp = new Inclinometer();
            

            return temp;
        }

        private void MenuItem_Click_1(object sender, RoutedEventArgs e)
        {
            Close();
        }

        public void OnPlugged(int pHandle)
        {
            if ((HIDDLLInterface.hidGetVendorID(pHandle) == 1240) & (HIDDLLInterface.hidGetProductID(pHandle) == 63))
            {
                //Label3.Visible = true;
                //Label3.Text = "Gerät erkannt";
                //Thread.Sleep(300);
                //Messwert_Hohlen = true;
                ((MainWindow)Application.Current.MainWindow).Connected = true;
                ((MainWindow)Application.Current.MainWindow).Append("Connected to Inclinometer!" + Environment.NewLine);
            }
        }
        public void OnUnplugged(int pHandle)
        {
            if ((HIDDLLInterface.hidGetVendorID(pHandle) == 1240) & (HIDDLLInterface.hidGetProductID(pHandle) == 63))
            {
                HIDDLLInterface.hidSetReadNotify(HIDDLLInterface.hidGetHandle(1240, 63), pValue: false);
                Connected = false;
                Append("Disonnected from Inclinometer!" + Environment.NewLine);
            }
        }

        public void OnChanged()
        {
            HIDDLLInterface.hidGetHandle(1240, 63);
            HIDDLLInterface.hidSetReadNotify(HIDDLLInterface.hidGetHandle(1240, 63), pValue: true);
        }
        public void OnRead(int pHandle)
        {
            SWI.Connection.ReadData(pHandle);

        }

        private void Window_Closing_1(object sender, System.ComponentModel.CancelEventArgs e)
        {
            SWI.Connection.IsClosing = true;
            Connected = false;
            SWI.Connection.DisconnectFromSWI();
        }

        private void Window_Loaded_1(object sender, RoutedEventArgs e)
        {

            HIDDLLInterface.ConnectToHID((object)this, new WindowInteropHelper(this).Handle.ToInt32());
        }
    }
}
