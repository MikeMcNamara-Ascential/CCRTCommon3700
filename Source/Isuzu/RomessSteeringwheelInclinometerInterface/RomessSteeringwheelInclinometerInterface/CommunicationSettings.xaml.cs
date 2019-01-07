using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace RomessSteeringwheelInclinometerInterface
{
    /// <summary>
    /// Interaction logic for CommunicationSettings.xaml
    /// </summary>
    public partial class CommunicationSettings : Window, INotifyPropertyChanged
    {
        //Fields

        //Parameters

        //Constructor
        public CommunicationSettings()
        {
            InitializeComponent();
        }

        public void SetCommSettingFields()
        {
            if (((MainWindow)Application.Current.MainWindow).SWI.Connection.Type.Equals("USB"))
            {
                if (CommSelectBox.SelectedIndex == 0)
                {
                    //dissable all of the Bluetooth Fields.
                    SetBluetoothFields(false);
                }
                else
                {
                    //Change the Selected Value to USB
                    CommSelectBox.SelectedIndex = 0;
                    SetBluetoothFields(false);
                }
            }
            else
            {
                if (CommSelectBox.SelectedIndex == 1)
                {
                    //dissable all of the Bluetooth Fields.
                    SetBluetoothFields(true);
                }
                else
                {
                    //Change the Selected Value to USB
                    CommSelectBox.SelectedIndex = 1;
                    SetBluetoothFields(true);
                }
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyChanged)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                var e = new PropertyChangedEventArgs(propertyChanged);
                handler(this, e);
            }
        }

        protected void SetBluetoothFields(bool enabled)
        {

        }

        private void CommSettingsCancelButton_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();

        }

        private void CommSelectBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (CommSelectBox.SelectedIndex == 0 && ((MainWindow)Application.Current.MainWindow).SWI.Connection.Type.Equals("Bluetooth"))
            {
                //Close out the Bluetooth conenction and establish the USB Connection
            }
            else if (CommSelectBox.SelectedIndex == 1 && ((MainWindow)Application.Current.MainWindow).SWI.Connection.Type.Equals("USB"))
            {
                //Close out the USB Connection and establish the Bluetooth connection 
            }
        }

    }
}
