using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using SerialPortLibrary;
using InputDataItemLibrary;
using Logger;
using QnxCcrtInterface;


namespace InputServer
{
    public class MapleKeypadDevice : InputDevice
    {
        //-----------------------------------------------------------------------------------------
        // Class Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// The parameters will be saved off to member variables.  A new CCRT serial port will be
        /// created using the supplied port settings.  The serial port will then be opened.
        /// </summary>
        /// <param name="deviceName">Name of the input device.</param>
        /// <param name="portSettings">Port settings for this input device.</param>
        /// <param name="dataGridRow">Data grid view row used for displaying info from this device.</param>
        /// <param name="columns">List of columns in the data grid we will be updating with status information.</param>
        /// <param name="dataItems">List of valid data items this device is looking for.</param>
        /// <param name="logFile">Logger object to use for logging information.</param>
        /// <param name="qnxCcrtSystemInterface">QNX CCRT system interface object.</param>
        public MapleKeypadDevice(String deviceName, PortSettings portSettings, List<InputDataItem> dataItems,
                                 DataGridViewRow dataGridRow, DataGridViewColumnCollection columns, CcrtLogger logFile,
                                 ref CcrtInterface qnxCcrtSystemInterface, InputServerForm inputServer) :
            base(deviceName, portSettings, dataItems, dataGridRow, columns, logFile, ref qnxCcrtSystemInterface, inputServer)
        {   // Display a message on the keypad for the operator to allow production number entry
            LogFile.Log("Displaying Enter VIN message", DeviceName);
            DisplayKeypadMessage("Please Enter VIN #", 1, 11);
            PositionCursor(2,  11);
            EnableBlockMode();
            // Subscribe for the new data event
            inputServer.NewInputDataEvent += HandleNewInputDataEvent;
        }

        /// <summary>
        /// Clear the display on the keypad.
        /// </summary>
        private void ClearKeypadDisplay()
        {
            LogFile.Log("Clearing keypad display", DeviceName);
            SendDeviceMessage(ClearDisplay);
        }

        /// <summary>
        /// Close the input device.
        /// A message will be displayed on the keypad indicating the device is not ready.
        /// </summary>
        public override void CloseInputDevice()
        {   // Display a message indicating the device is not ready
            LogFile.Log("Closing device", DeviceName);
            ClearKeypadDisplay();
            DisableBlockMode();
            DisplayKeypadMessage("Not Ready to Test", 1, 11);
            DisplayKeypadMessage("Please Wait", 2, 12, false);
            base.CloseInputDevice();
        }

        /// <summary>
        /// Disable block mode on the keypad - disables character entry.
        /// </summary>
        public void DisableBlockMode()
        {   // Build the command to disable block mode
            LogFile.Log("Disabling block mode - disable character input", DeviceName);
            String command = "}1";
            SendKeypadCommand(command);
        }

        /// <summary>
        /// Handle the new input data event.
        /// The data will be displayed on the keypad for a short period of time.
        /// </summary>
        /// <param name="sender">Object that raised the event.</param>
        /// <param name="args">Information about the new data.</param>
        public void HandleNewInputDataEvent(object sender, NewInputDataEventArgs args)
        {   // Kick off a thread to display the data so we can return immediately
            Thread displayDataThread = new Thread(DisplayInputDeviceData);
            displayDataThread.Start(args.Data);
        }

        /// <summary>
        /// Place the received data on the display of this input device.
        /// </summary>
        /// <param name="data">Data to display on the input device.</param>
        public override void DisplayInputDeviceData(object data)
        {   // Display the data on the screen
            // Assuming this is a VIN/Production Number for now and it will be displayed in row 2.  If something
            // more is needed this will need to change.
            ClearKeypadDisplay();
            DisplayKeypadMessage("Test Ready, Enter Booth", 1, 5);
            DisplayKeypadMessage(data.ToString(), 2, 11, false);
            Thread.Sleep(10000);
            ClearKeypadDisplay();
            DisplayKeypadMessage("Please Enter VIN #", 1, 11);
            PositionCursor(2, 11);
            EnableBlockMode();
        }


        /// <summary>
        /// Enable block mode on the keypad - allows character entry.
        /// </summary>
        public void EnableBlockMode()
        {   // Build the command to enable block mode
            LogFile.Log("Enabling block mode - enable character input", DeviceName);
            String command = "}2";
            SendKeypadCommand(command);
        }

        /// <summary>
        /// Send a message to be displayed on the keypad display.
        /// </summary>
        /// <param name="message">Message to display on the keypad.</param>
        /// <param name="row">Row on the display to display the message.</param>
        /// <param name="column">Column on the display to start the mesage.</param>
        /// <param name="clearDisplay">Flag to indicate if the display should be cleared before displaying the message.</param>
        public void DisplayKeypadMessage(String message, Int32 row, Int32 column, Boolean clearDisplay = true)
        {   // Clear any existing message from the display
            LogFile.Log("Displaying message: {" + message + "}", DeviceName);
            if(clearDisplay)  ClearKeypadDisplay();
            // Position the cursor and then display the message
            PositionCursor(row, column);
            SendDeviceMessage(message.ToArray(), message.Length);
        }

        /// <summary>
        /// Process the data received from the device.
        /// The type of data will be identified and displayed on the main form.
        /// </summary>
        /// <param name="data">Data received from the input device.</param>
        public override Boolean ProcessDeviceData(char[] data, int byteCount)
        {
            DisplayKeypadMessage("Processing VIN", 1, 11);
            DisplayKeypadMessage("Please Wait", 2, 13, false);
            Boolean result = base.ProcessDeviceData(data, byteCount);
            ClearKeypadDisplay();
            DisplayKeypadMessage("Please Enter VIN #", 1, 11);
            PositionCursor(2, 11);
            EnableBlockMode();
            return result;
        }

        /// <summary>
        /// Position the cursor on the display to the specified row and column.
        /// </summary>
        /// <param name="row">Row to position the cursor in.</param>
        /// <param name="column">Column to position the cursor in.</param>
        private void PositionCursor(Int32 row, Int32 column)
        {
            String command = "x" + column.ToString() + row.ToString();
            LogFile.Log("Positioning cursor at row: " + row.ToString() + " column: " + column.ToString(), DeviceName);
            SendKeypadCommand(command);
        }

        /// <summary>
        /// Send a command to the keypad.
        /// </summary>
        /// <param name="command">Command to send to the keypad.</param>
        private void SendKeypadCommand(String command)
        {
            char[] cmd = command.ToArray();
            SendDeviceMessage(CommandStart);
            SendDeviceMessage(cmd, command.Length);
            SendDeviceMessage(CommandTerminator);
        }


        //-----------------------------------------------------------------------------------------
        // Class Variable/Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Command for clearing the keypad display.
        /// </summary>
        private char ClearDisplay { get { return '\x00C'; } }

        /// <summary>
        /// Command start sequence.
        /// </summary>
        private char CommandStart { get { return '\x01B'; } }

        /// <summary>
        /// command terminator to use to signal the end of a command sequence.
        /// </summary>
        private char CommandTerminator { get { return '\x002'; } }
    }
}
