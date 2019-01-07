using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
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
        private CCRTWrapper qnxComms;
        private string messages;
        private string reading;
        private bool closing;
        private CommunicationSettings CommWindow;

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
                if (SWI != null)
                {
                    SWI.Connection.Connected = value;
                    if (SWI.Connection.Connected)
                    {
                        ConnectionStausBox.Text = "Connected";
                        ConnectionStausBox.Background = new SolidColorBrush(Color.FromRgb(88, 243, 29));
                        QNXComms.CCRTReconnect();
                    }
                    else
                    {
                        ConnectionStausBox.Text = "Disconnected";
                        ConnectionStausBox.Background = new SolidColorBrush(Color.FromRgb(243, 29, 29));
                    }
                }
            }
        }
        public string CCRTConnection
        {
            get;
            set;
        }
        public bool CCRTConnected
        {
            get
            {
                if (qnxComms.Initialized)
                    return qnxComms.QNXCCRT.QnxConnected;
                else
                    return (qnxComms.Initialized);
            }
            set
            {
                if (value == true)
                {
                    CCRTConnectionStatusBox.Text = "Connected";
                    CCRTConnectionStatusBox.Background = new SolidColorBrush(Color.FromRgb(88, 243, 29));
                }
                else
                {
                    CCRTConnectionStatusBox.Text = "Disconnected";
                    CCRTConnectionStatusBox.Background = new SolidColorBrush(Color.FromRgb(243, 29, 29));
                }
            }
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
        public CCRTWrapper QNXComms
        {
            get { return qnxComms; }
            set { qnxComms = value; }
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
            qnxComms = new CCRTWrapper();
            SWI = LoadConfiguration();
            CommWindow = new CommunicationSettings();
            Connection = Connection;
            Connected = Connected;
            CCRTConnected = CCRTConnected;
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
            Inclinometer temp = null;
            DirectoryInfo dir = new DirectoryInfo(".");
            string filePath = System.IO.Path.Combine(dir.FullName, "RomessSWIConfig.sys");
            if (File.Exists(filePath))//check if the file exists, if it does then load the parameters.
            {
                List<string> lines = new List<string>();
                System.IO.StreamReader inFile = new System.IO.StreamReader(filePath);
                string inLine = "";
                while ((inLine = inFile.ReadLine()) != null)//read until we get to the end of the file
                {
                    lines.Add(inLine);
                    //Append(inLine + Environment.NewLine);
                }
                inFile.Close();
                int numLines = lines.Count();
                int linesProcessed = 0;
                foreach(string line in lines)
                {
                    if (linesProcessed >= numLines && (temp == null))
                    {   //there is no protocol value in the config
                        temp = new Inclinometer("USB");
                        break;
                    }
                    string[] value = line.Replace(" ", string.Empty).Split(':');
                    if(value[0].Contains("Protocol"))
                    {
                        if(value[1].Contains("USB"))
                            temp = new Inclinometer("USB");
                        else if (value[1].Contains("Bluetooth"))
                            temp = new Inclinometer("Bluetooth");
                        //Remove this line to make able to handle Bluetooth also
                        //temp = new Inclinometer("USB");
                    }
                    else if (value[0].Contains("QNXIP"))
                    {
                        qnxComms.QNXIP = value[1];
                        Append("QNX IPAddress: " + qnxComms.QNXIP + Environment.NewLine);
                    }
                    else if (value[0].Contains("QNXCCRTPort"))
                    {
                        qnxComms.QNXPort = value[1];
                        Append("QNX Port: " + qnxComms.QNXPort + Environment.NewLine);
                    }
                    else if (value[0].Contains("IPAddress"))
                    {
                        qnxComms.WindowsIP = value[1];
                        Append("Windows IPAddress: " + qnxComms.WindowsIP + Environment.NewLine);
                    }
                    else if (value[0].Contains("BaudRate"))
                    {
                        if (temp == null)
                        {
                            lines.Add(line);
                            continue;
                        }
                        if (temp.Connection.Type.Equals("Bluetooth"))
                        {
                            ((BluetoothConnection)(temp.Connection)).BaudRate = Int32.Parse(value[1]);
                            Append("Bluetooth Baud: " + ((BluetoothConnection)(temp.Connection)).BaudRate + Environment.NewLine);
                        }
                    }
                }
                //temp = new Inclinometer("USB");
            }
            else //create the file and populate it with defaults
            {
                List<string> lines = new List<string>();
                lines.Add("Protocol: USB");
                lines.Add("QNXIP: 192.168.1.1");
                lines.Add("QNXCCRTPort: 10019");
                lines.Add("IPAddress: 192.168.1.2");
                lines.Add("BaudRate: 38400");
                TextWriter outFile = new StreamWriter(filePath);
                foreach (string line in lines)
                {
                    outFile.WriteLine(line);
                }
                outFile.Close();
                temp = new Inclinometer("USB");
            }
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
            CommWindow.Close();
            SWI.Connection.IsClosing = true;
            Connected = false;
            if (qnxComms.QNXCCRT != null)
            {
                qnxComms.QNXCCRT.Shutdown();
            }
            qnxComms.IsClosing = true;
            SWI.Connection.DisconnectFromSWI();
            qnxComms.Logger.CloseLogFile();
        }

        private void Window_Loaded_1(object sender, RoutedEventArgs e)
        {
            if(SWI.Connection.Type.Equals("USB"))
                HIDDLLInterface.ConnectToHID((object)this, new WindowInteropHelper(this).Handle.ToInt32());
        }
        private void OptionsMenuCommunicationClick(object sender, RoutedEventArgs e)
        {
            //CommunicationSettings CommWindow = new CommunicationSettings();
            CommWindow.SetCommSettingFields();
            CommWindow.Show();
        }

    }
}
