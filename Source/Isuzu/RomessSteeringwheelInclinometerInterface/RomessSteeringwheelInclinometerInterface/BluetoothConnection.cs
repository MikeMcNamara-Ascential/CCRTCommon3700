using System;
using System.Collections.Generic;
using System.Linq;
using System.IO.Ports;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace RomessSteeringwheelInclinometerInterface
{
    class BluetoothConnection : InclinometerConnection
    {
      //Collect all of the Fields Here
        //Interface Fields:
        private bool connected;
        private string type;
        private bool isClosing;
        //OtherFields
        private SerialPort serial;
        int sensorType = 0;
        string Ebuffer = "";
        //bool connection = false;
        string measuredValue = "";  // set up as member variable
        string devInterface = "";
        System.Collections.ArrayList lstCOMPorts = new System.Collections.ArrayList();
        string portNumber;
        int Baud_Rate = 38400;

      //Collect all of the Parameters Here
        //Interface Parameters:
        public bool Connected
        {
            get { return connected; }
            set
            {
                connected = value;
            }
        }

        public string Type
        {
            get { return type; }
            set { type = value; }
        }
        public bool IsClosing
        {
            get { return isClosing; }
            set { isClosing = value; }
        }
        //Other Parameters
        public SerialPort Serial
        {
            get { return serial; }
            set { serial = value; }
        }
 
      //Class Methods
        public BluetoothConnection()
        {
            type = "Bluetooth";
            connected = false;
            serial = new SerialPort();
            ((MainWindow)Application.Current.MainWindow).Messages += ("Using Bluetooth!" + Environment.NewLine);
            devInterface = "COM15";
            if(!ConnectToSWI())
                StartDataThread();
        }

        public Single ReadData(int pHandle)
        {
            return 2/5;
        }
        private void StartDataThread()
        {

            Thread thread = new Thread(delegate()
            {
                Thread.Sleep(1000);
                while (Serial.IsOpen)
                {
                    if (!Connected)
                        Send_Recognition();
                    else
                    {
                        try
                        {
                            Collect_MeasuredValue();
                        }
                        catch (Exception e)
                        {
                            Append("Error in USB Data Thread:" + e.ToString() + Environment.NewLine);
                        }
                    }
                    Thread.Sleep(100);
                }
            });
            thread.Start();
        }
        public bool ConnectToSWI()
        {
            if (Serial.IsOpen)
            {
                Serial.Close();
            }
            try
            {
                if (devInterface != "")
                {
                    Serial.PortName = devInterface;
                    Serial.BaudRate = Baud_Rate;
                    Serial.Parity = Parity.None;
                    Serial.DataBits = 8;
                    Serial.StopBits = StopBits.One;
                    Serial.ReadBufferSize = 4096;
                    Serial.DtrEnable = false;
                    Serial.Open();
                    Serial.DataReceived += new SerialDataReceivedEventHandler(Data_Received);
                    Append("Reading data from " + devInterface + Environment.NewLine);
                    //Logger.Log("Reading data from " + devInterface);
                }
                else
                {
                    Serial.PortName = SerialPort.GetPortNames()[0];
                    Serial.BaudRate = Baud_Rate;
                    Serial.Parity = Parity.None;
                    Serial.DataBits = 8;
                    Serial.StopBits = StopBits.One;
                    Serial.ReadBufferSize = 4096;
                    Serial.Open();
                    Serial.DataReceived += new SerialDataReceivedEventHandler(Data_Received);

                    Append("Reading data from " + Serial.PortName + Environment.NewLine);
                    //((MainWindow)Application.Current.MainWindow).Append("Reading data from " + Serial.PortName);
                }
//TODO ADD THREAD                //timer1.Start();


            }
            catch (Exception ex)
            {
                Append("Error 1: Can not open a devInterface!\n" +
                            "Close the program, connect the device and switch it on, then restart the program." + Environment.NewLine + ex.ToString() + Environment.NewLine);
                Connection(false);
            }
            return Connected;

        }
        public bool DisconnectFromSWI()
        {
            Serial.Close();
            return true;
        }

        private void Data_Received(Object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                UpdateData();
            }
            catch (Exception ex)
            {
                Append("Error 12: Can not open a devInterface!\n" +
                               "Close the program, connect the device and switch it on, then restart the program." + Environment.NewLine + ex.ToString() + Environment.NewLine);
                Connection(false);
                //Console.WriteLine(errorMessage + ex.Message + "\n");
                //Logger.Log("Error: " + ex.Message);
            }
        }
        public void UpdateData()
        {
            string text1 = "";
            int c, d, i, x;
            string buffer = "";
            decimal temp1;
            double temp2;

            Ebuffer += Serial.ReadExisting();

            try
            {
                if (Ebuffer.Length < 9)
                {
                    return;
                }
                if (Ebuffer.Length > 20)
                {
                    Ebuffer = "";
                }

                c = Ebuffer[0];

                if (c > 2)
                {
                    d = 0;
                    while (true)
                    {
                        c = Ebuffer[d];
                        if (c == 2)
                        {
                            x = Ebuffer.Length;
                            for (i = d; i < x; i++)
                            {
                                c = Ebuffer[i];
                                buffer += Ebuffer[i];
                            }
                            Ebuffer = "";

                            x = buffer.Length;
                            for (i = 0; i < x; i++)
                            {
                                c = buffer[i];
                                Ebuffer += buffer[i];
                            }
                            break;
                        }
                        d = d + 1;
                    }
                }

                c = Ebuffer[0];
                d = Ebuffer[1];
                if (c == 2 && d == 2 || d == 3 && !Connected)  // connection test
                {
                    i = 2;
                    while (true)
                    {
                        c = Ebuffer[i];
                        if (c < 48)
                        {
                            break;
                        }
                        text1 += Ebuffer[i];
                        i++;
                    }

                    i = string.Compare(text1, "ROMESS");
                    if (i == 0)
                    {
                        Connection(true);
                        //timer2.Stop();
                        //timer2.Enabled = false;
                    }
                    Ebuffer = "";
                    c = 0;
                    return;
                }

                c = Ebuffer[0];
                d = Ebuffer[1];
                if (c == 2 && d == 7)
                {
                    i = 2;
                    c = Ebuffer[i];
                    while (c > 32)
                    {
                        c = Ebuffer[i];
                        text1 += (char)c;
                        i++;
                        c = Ebuffer[i];
                    }

                    temp1 = decimal.Parse(text1);
                    temp2 = (double)temp1 / 10;
                    i = (int)temp2;

                    temp1 = (decimal)i / 10;

                    text1 = "";

                    if (temp1 < 10)
                    {
                        text1 += " ";
                    }

                    text1 += temp1.ToString();

                    text1 += "°";

                    ReportValue(text1);       // Console test readout
                    measuredValue = text1;

                    c = Ebuffer[7];
                    sensorType = Ebuffer[6];

                    Ebuffer = "";
                    text1 = "";
                }

                if (c == 2 && d == 7 && !Connected)
                {
                    Ebuffer = "";
                    text1 = "";
                }

                if (c == 2 && d == 8)
                {
                    text1 = "";
                    i = 2;
                    c = 33;
                    while (c > 32)
                    {
                        c = Ebuffer[i];
                        if (c > 31)
                        {
                            text1 += (char)c;
                        }
                        i++;
                    }
                    Ebuffer = "";
                }
            }
            catch (Exception ex)
            {
                //timer1.Enabled = true;
                Ebuffer = "";
                Append("Error: " + ex.Message + Environment.NewLine);
                //Logger.Log("Error: " + ex.Message);
            }
        }

        private void Collect_MeasuredValue()
        {
            byte[] buffer = new byte[65];
            buffer[0] = 33;     // !
            buffer[1] = 77;     // M
            buffer[2] = 87;     // W
            buffer[3] = 69;     // E
            buffer[4] = 72;     // H
            buffer[5] = 10;     // LF
            buffer[6] = 13;     // CR
            buffer[7] = 0;      //
            Send(buffer);
        }

        private void Send_Recognition()
        {
            byte[] buffer = new byte[65];

            buffer[0] = 33;          //  !
            buffer[1] = 72;          //  H
            buffer[2] = 65;          //  A
            buffer[3] = 76;          //  L    
            buffer[4] = 79;          //  O 
            buffer[5] = 10;          //  LF
            buffer[6] = 13;          //  CR
            buffer[7] = 0;           //  
            Send(buffer);
        }

        private void Send(byte[] buffer)
        {
            string errorMessage;

            try
            {
                Serial.WriteTimeout = 100;
                Serial.ReadTimeout = 100;
                Serial.Write(buffer, 0, buffer.Length);
            }
            catch (Exception ex)
            {
                //timer2.Stop();
                errorMessage = "Error: 14\n" +
                               "Can not open the devInterface!\n" +
                               "Please use another ComPort\n" +
                               "Please check stick\n" +
                               ex.Message;
                Connected = false;

                Append(errorMessage + ex.Message + Environment.NewLine);
                //Logger.Log("Error: " + ex.Message);
            }
        }
        public void Append(String inStr)
        {
            if (!IsClosing)
            Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, new ThreadStart(delegate
            {
                ((MainWindow)Application.Current.MainWindow).MessageBox.Text += inStr;
            }));
        }
        public void Connection(bool inBool)
        {
            if (!IsClosing)
            Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, new ThreadStart(delegate
            {
                ((MainWindow)Application.Current.MainWindow).Connected = inBool;
            }));
        }
        public void ReportValue(string inValue)
        {
            if (!IsClosing)
            Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, new ThreadStart(delegate
            {
                    ((MainWindow)Application.Current.MainWindow).Reading = inValue;
            }));
        }
    }
}
