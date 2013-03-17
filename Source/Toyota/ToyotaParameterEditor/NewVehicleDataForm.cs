using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace ToyotaParameterEditor
{
    public partial class NewVehicleDataForm : Form
    {
        public NewVehicleDataForm()
        {
            InitializeComponent();
            Visible = false;
            m_vehicleButtonNumberComboBox.SelectedIndex = 0;
            m_updatedFiles = new List<String>();
        }

        /// <summary>
        /// Save the new vehicle information to the CCRT files where needed.
        /// </summary>
        private void UpdateCcrtFiles()
        {   // Update the CCRT Client file so the vehicle logo is displayed when selected
            UpdateCcrtClientFiles();
            // Update the CCRT Server Files
            UpdateCcrtServerFiles();
        }

        /// <summary>
        /// Update the CCRT client files with the new vehicle type information.
        /// </summary>
        private void UpdateCcrtClientFiles()
        {   // Add the new vehicle to the display
            using (StreamWriter writer = new StreamWriter(ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile, true))
            {   // Write the new selection criteria to the file
                writer.Write("widget.platformlogo.image." + VehicleName + "=images/toyota-" + VehicleName.ToLower() + ".jpg" + writer.NewLine);
            }
            if (!UpdatedFiles.Contains(ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile))
            {
                m_updatedFiles.Add(ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile);
            }
            // Save the image file to the CCRT Client path
            String clientLogFileName = ToyotaParameterEditor.Properties.Resources.CcrtClientLogoFileLocation + "toyota-" + VehicleName.ToLower() + ".jpg";
            m_vehicleLogoPictureBox.Image.Save(clientLogFileName);
            if (!UpdatedFiles.Contains(clientLogFileName))
            {
                m_updatedFiles.Add(clientLogFileName);
            }
        }

        /// <summary>
        /// Update the CCRT server files with the new vehicle type information.
        /// </summary>
        private void UpdateCcrtServerFiles()
        {   // Update the roll test selection file
            XmlDocument rollTestSelectionTable = new XmlDocument();
            rollTestSelectionTable.Load(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable);
            XmlElement rollSelection = rollTestSelectionTable.CreateElement(VehicleName);
            rollSelection.SetAttribute("DriveCurve", ToyotaParameterEditor.Properties.Resources.RollTestDriveCurve);
            XmlElement rollTestBodyStyleNode = rollTestSelectionTable.CreateElement("BodyStyle");
            rollTestBodyStyleNode.InnerText = VehicleName;
            rollSelection.AppendChild(rollTestBodyStyleNode);
            rollTestSelectionTable.DocumentElement.AppendChild(rollSelection);
            rollTestSelectionTable.Save(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable);
            if (!UpdatedFiles.Contains(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable))
            {
                m_updatedFiles.Add(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable);
            }
            // Update the brake test selection file
            XmlDocument brakeTestSelectionTable = new XmlDocument();
            brakeTestSelectionTable.Load(ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable);
            XmlElement brakeSelection = brakeTestSelectionTable.CreateElement(VehicleName);
            String driveCurve = " ";
            if (m_2wdRadioButton.Checked)
            {
                driveCurve = ToyotaParameterEditor.Properties.Resources.TwoWheelDriveTestSequence;
            }
            else if (m_4wdRadioButton.Checked)
            {
                driveCurve = ToyotaParameterEditor.Properties.Resources.FourWheelDriveTestSequence;
            }
            brakeSelection.SetAttribute("DriveCurve", driveCurve);
            XmlElement brakeTestBodyStyleNode = brakeTestSelectionTable.CreateElement("BodyStyle");
            brakeTestBodyStyleNode.InnerText = VehicleName;
            brakeSelection.AppendChild(brakeTestBodyStyleNode);
            brakeTestSelectionTable.DocumentElement.AppendChild(brakeSelection);
            brakeTestSelectionTable.Save(ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable);
            if (!UpdatedFiles.Contains(ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable))
            {
                m_updatedFiles.Add(ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable);
            }
            // Update the InputServer Config
            XmlDocument inputServer = new XmlDocument();
            inputServer.Load(ToyotaParameterEditor.Properties.Resources.InputServerFileName);
            XmlElement vehicleType = inputServer.CreateElement("Type0" + PushButtonNumber);
            vehicleType.InnerText = VehicleName;
            inputServer.DocumentElement.SelectSingleNode("Setup/InputDevices/PlcDataInput/Setup/VehicleTypeDecode").AppendChild(vehicleType);
            inputServer.Save(ToyotaParameterEditor.Properties.Resources.InputServerFileName);
            if (!UpdatedFiles.Contains(ToyotaParameterEditor.Properties.Resources.InputServerFileName))
            {
                m_updatedFiles.Add(ToyotaParameterEditor.Properties.Resources.InputServerFileName);
            }
        }

        /// <summary>
        /// Save the vehicle information entered by the user.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_doneButton_Click(object sender, EventArgs e)
        {   // Store the name of the vehicle
            VehicleName = m_vehicleNameTextBox.Text;
            if (VehicleName.Length > 0)
            {   // Copy the image to our default location
                FileInfo imageFile = new FileInfo(VehicleLogoFile);
                DirectoryInfo imagesDir = new DirectoryInfo(ToyotaParameterEditor.Properties.Resources.VehicleLogoFileLocation);
                if (!imagesDir.Exists)
                {
                    imagesDir.Create();
                }
                String imageFileName = ToyotaParameterEditor.Properties.Resources.VehicleLogoFileLocation + "\\toyota-" + VehicleName.ToLower() + ".jpg";
                imageFile.CopyTo(imageFileName);
                if (!UpdatedFiles.Contains(imageFileName))
                {
                    m_updatedFiles.Add(imageFileName);
                }
                // Get the pushbutton number and update the CCRT files
                PushButtonNumber = m_vehicleButtonNumberComboBox.Items[m_vehicleButtonNumberComboBox.SelectedIndex].ToString();
                UpdateCcrtFiles();
            }
            // Hide the window
            Visible = false;
        }

        /// <summary>
        /// Get/Set the name of the vehicle.
        /// </summary>
        private String m_vehicleName;
        public String VehicleName
        {
            get { return m_vehicleName; }
            set { m_vehicleName = value; }
        }

        /// <summary>
        /// Get/Set the original file name of the vehicle logo.
        /// </summary>
        private String m_vehicleLogoFile;
        private String VehicleLogoFile
        {
            get{return m_vehicleLogoFile;}
            set{m_vehicleLogoFile = value;}
        }

        /// <summary>
        /// Get/Set the push button to use for this vehicle.
        /// </summary>
        private String m_pushButtonNumber;
        private String PushButtonNumber
        {
            get { return m_pushButtonNumber; }
            set { m_pushButtonNumber = value; }
        }

        /// <summary>
        /// Get the list of files that were updated while creating a new vehicle.
        /// </summary>
        private List<String> m_updatedFiles;
        public List<String> UpdatedFiles
        {
            get { return m_updatedFiles; }
        }

        /// <summary>
        /// Provide a dialog box so the user can select an image for display
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_selectImageButton_Click(object sender, EventArgs e)
        {   // Allow the user to select an image for display
            OpenFileDialog openDialog = new OpenFileDialog();
            openDialog.DefaultExt = "jpg";
            openDialog.Filter = "jpg | *.jpg";
            openDialog.Multiselect = false;
            openDialog.RestoreDirectory = true;
            openDialog.Title = "Select Vehicle Type Image";
            if (openDialog.ShowDialog() == DialogResult.OK)
            {   // Load the selected image
                m_vehicleLogoPictureBox.Image = Image.FromFile(openDialog.FileName);
                VehicleLogoFile = openDialog.FileName;
            }
        }
    }
}