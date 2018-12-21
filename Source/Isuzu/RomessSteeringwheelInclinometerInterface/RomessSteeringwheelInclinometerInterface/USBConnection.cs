using Microsoft.VisualBasic.CompilerServices;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace RomessSteeringwheelInclinometerInterface
{
    class USBConnection: InclinometerConnection
    {
      //Collect all of the Fields Here
        //Interface Fields:
        private bool connected;
        private string type;
        private bool isClosing;
        //private byte[] zbuffer;

        //Other Fields
        private Single inclinometerValue;

      //Collect all of the Parameters Here
        //Interface Parameters:
        public bool Connected
        {
            get { return connected; }
            set {
                connected = value;
                if (connected)
                    StartDataThread();
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
        public Single InclinometerValue
        {
            get { return inclinometerValue; }
            set { inclinometerValue = value; }
        }

      //Instantiators
        public USBConnection()
        {
            type = "USB";
            connected = false;
            //Buffer = new byte[66];
        }
      //Interface Methods
        public Single ReadData(int pHandle)
        {
            byte[] buffer = new byte[66];
            int num = 0;
            int num2 = 0;
            if (!HIDDLLInterface.hidRead(pHandle, ref buffer[0]))
            {
                return 0;
            }
            byte num3 = buffer[1];
            num = buffer[2];
            if (num3 == 2)
            {
                if (num == 2)
                {
                    num2 = 1;
                }
                if ((num == 6) & (buffer.Length > 25))
                {
                    num2 = 2;
                }
                if (num == 7)
                {
                    num2 = 3;
                }
                if (num == 8)
                {
                    num2 = 4;
                }
                if (num == 9)
                {
                    num2 = 5;
                }
                if (num == 20)
                {
                    num2 = 20;
                }
            }
            if (num2 == 3)
            {
                getValueFromBuffer(buffer);
            }
            return 0;
        }

        private void getValueFromBuffer(byte[] buffer)
        {
            string text = "";
            int num = 0;
            int num2 = 0;
            int num3 = 0;
            int num4 = 0;
            int num5 = 0;
            int num6 = 0;
            string text2 = "";
            num2 = 0;
            checked
            {
                while (true)
                {
                    num = buffer[num2];
                    if (num > 10)
                    {
                        break;
                    }
                    num2++;
                }
                num3 = 0;
                while (num > 32)
                {
                    num = buffer[num2];
                    if (num2 == 6)
                    {
                        text += ",";
                    }
                    if (num > 32)
                    {
                        text += (char)(num);
                    }
                    if (num > 47 && num3 == 1)
                    {
                        num4 = (num - 48) * 1000;
                    }
                    if (num > 47 && num3 == 2)
                    {
                        num4 += (num - 48) * 100;
                    }
                    if (num > 47 && num3 == 3)
                    {
                        num4 += (num - 48) * 10;
                    }
                    if (num > 47 && num3 == 4)
                    {
                        num4 += num - 48;
                    }
                    num2++;
                    num3++;
                }
                num2 = unchecked(num4 % 10);
                num4 -= num2;
                if (num2 >= 5)
                {
                    num4 += 10;
                }
                num4 = (int)Math.Round(unchecked((double)num4 / 10.0));
                num6 = unchecked(num4 % 10);
                num5 = num4 - num6;
                num5 = (int)Math.Round(unchecked((double)num5 / 10.0));
                text2 = Conversions.ToString(text[0]);
                text2 = text2 + num5.ToString() + ".";
                text2 = text2 + num6.ToString() + "°";
                ((MainWindow)Application.Current.MainWindow).Reading = text2;
                //Messwert_X = text;
                text = "";
            }
        }

        public bool ConnectToSWI()
        {
            return true;
        }
        public bool DisconnectFromSWI()
        {
            HIDDLLInterface.DisconnectFromHID();
            return true;
        }
      //Class Methods
        public void sendRequestValueMessage()
        {
            byte[] BufferOut = new Byte[9];
            BufferOut[0] = 0;
            BufferOut[1] = 33;//!
            BufferOut[2] = 77;//M
            BufferOut[3] = 87;//W
            BufferOut[4] = 69;//E
            BufferOut[5] = 72;//H
            BufferOut[6] = 10;//LF
            BufferOut[7] = 13;//CR
            BufferOut[8] = 0;
            HIDDLLInterface.hidWriteEx(1240, 63, ref BufferOut[0]);
        }

        private void StartDataThread()
        {

            Thread thread = new Thread(delegate()
            {
                Thread.Sleep(1000);
                while (Connected)
                {
                    try
                    {
                        sendRequestValueMessage();
                        getDataFromDevice();
                    }
                    catch (Exception e)
                    {
                        Append("Error in USB Data Thread:" + e.ToString()+ Environment.NewLine);
                    }
                    Thread.Sleep(100);
                }
            });
            thread.Start();
        }

        private void getDataFromDevice()
        {

            InclinometerValue = 2 / 5;
        }
        
        public void Append(String inStr)
        {
            if (!IsClosing)
            Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, new ThreadStart(delegate
            {
                    ((MainWindow)Application.Current.MainWindow).MessageBox.Text += inStr;
            }));
        }
    }
}
