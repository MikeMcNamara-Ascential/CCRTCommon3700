using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Security.AccessControl;
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
            updateAvailableButtonNumbers();
            m_updatedFiles = new List<String>();
        }

        /// <summary>
        /// Allows only the unused button numbers in the selection drop box so dupicates can't be created
        /// </summary>
        private void updateAvailableButtonNumbers()
        {
            try
            {
                XmlDocument inputServer = new XmlDocument();
                inputServer.Load(ToyotaParameterEditor.Properties.Resources.InputServerFileName);
                XmlNode vehicleTypeNodes = inputServer.DocumentElement.SelectSingleNode("Setup/InputDevices/PlcDataInput/Setup/VehicleTypeDecode");
                
                foreach (XmlNode vehicleType in vehicleTypeNodes.ChildNodes)
                {
                    m_vehicleButtonNumberComboBox.Items.Remove(vehicleType.Name.Substring(vehicleType.Name.Length - 1, 1));
                    Console.WriteLine("Removed number {0}", vehicleType.Name.Substring(vehicleType.Name.Length - 1, 1));
                }
            }
            catch
            {   // do nothing, file access issues will be handled at save time
            }
        }

        /// <summary>
        /// Save the new vehicle information to the CCRT files where needed.
        /// </summary>
        private void UpdateCcrtFiles()
        {
            // Update the CCRT Server Files
            UpdateCcrtServerFiles();
            if (DialogResult == DialogResult.OK)
            {   // If the server file update was successful, 
                // Update the CCRT Client file so the vehicle logo is displayed when selected
                UpdateCcrtClientFiles();
            }
            
        }

        /// <summary>
        /// Update the CCRT client files with the new vehicle type information.
        /// </summary>
        private void UpdateCcrtClientFiles()
        {   // Add the new vehicle to the display
            bool clientFileWritten = true;
            Int32 attempts = 2;
            do
            {
                try
                {
                    using (StreamWriter writer = new StreamWriter(ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile, true))
                    {   // Write the new selection criteria to the file
                        writer.Write("widget.platformlogo.image." + VehicleName + "=images/toyota-" + VehicleName.ToLower() + ".jpg" + writer.NewLine);
                    }
                }
                catch (UnauthorizedAccessException)
                {
                    clientFileWritten = false;
                    FileSecurity fSec = File.GetAccessControl(ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile);
                    fSec.AddAccessRule(new FileSystemAccessRule(@"DomainName\AccountName", FileSystemRights.Write, AccessControlType.Allow));
                    File.SetAccessControl(ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile, fSec);
                }
            } while ((0 <= attempts--) && !clientFileWritten);
            if (!clientFileWritten)
            {
                MessageBox.Show("Unable to access " + ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile + "." + 
                                "Vehicle image cannot be displayed until file is updated manually.",
                                "File Access Error",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            if (!UpdatedFiles.Contains(ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile))
            {
                m_updatedFiles.Add(ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile);
            }
            // Save the image file to the CCRT Client path
            String clientImageFileName = ToyotaParameterEditor.Properties.Resources.CcrtClientLogoFileLocation + "toyota-" + VehicleName.ToLower() + ".jpg";
            if (!File.Exists(clientImageFileName))
            {
                try
                {
                    m_vehicleLogoPictureBox.Image.Save(clientImageFileName);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Unable to save " + clientImageFileName + ":" +
                                Environment.NewLine + ex.Message + Environment.NewLine +
                                "Vehicle image cannot be displayed until file is placed here manually.",
                                "File Access Error",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            if (!UpdatedFiles.Contains(clientImageFileName))
            {
                m_updatedFiles.Add(clientImageFileName);
            }
        }

        /// <summary>
        /// Update the CCRT server files with the new vehicle type information.
        /// </summary>
        private void UpdateCcrtServerFiles()
        {
            String missingFiles = "";

            if (!File.Exists(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable))
            {
                missingFiles += ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable + Environment.NewLine;
            }
            if (!File.Exists(ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable))
            {
                missingFiles += ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable + Environment.NewLine;
            }
            if (!File.Exists(ToyotaParameterEditor.Properties.Resources.InputServerFileName))
            {
                missingFiles += ToyotaParameterEditor.Properties.Resources.InputServerFileName + Environment.NewLine;
            }
            if (!File.Exists(ToyotaParameterEditor.Properties.Resources.NewVehicleTemplateFileName))
            {
                missingFiles += Directory.GetCurrentDirectory() + ToyotaParameterEditor.Properties.Resources.NewVehicleTemplateFileName + Environment.NewLine;
            }
            if (missingFiles.Length > 0)
            {
                MessageBox.Show("The following required files could not be located:  \n\n" +
                                missingFiles + Environment.NewLine +
                                "New vehicle type was not added.",
                                "Required Files Missing",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
                DialogResult = DialogResult.Abort;
            }
            else
            {
                // Update the roll test selection file
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
                    if (m_performAbsTest.Checked)
                    {
                        driveCurve = ToyotaParameterEditor.Properties.Resources.TwoWheelDriveAbsTestSequence;
                    }
                    else
                    {
                        driveCurve = ToyotaParameterEditor.Properties.Resources.TwoWheelDriveTestSequence;
                    }
                }
                else if (m_4wdRadioButton.Checked)
                {
                    if (m_performAbsTest.Checked)
                    {
                        driveCurve = ToyotaParameterEditor.Properties.Resources.FourWheelDriveAbsTestSequence;
                    }
                    else
                    {
                        driveCurve = ToyotaParameterEditor.Properties.Resources.FourWheelDriveTestSequence;
                    }
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
        }

        /// <summary>
        /// Save the vehicle information entered by the user.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_doneButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.None;
            // Verify required directories are accessable
            bool inputIsValid = true;
            
            // Validate Input & Required file access
            if (m_vehicleNameTextBox.Text.Length == 0 && inputIsValid)
            {
                inputIsValid = false;
                MessageBox.Show("Vehicle Name is required",
                                "Required Data Missing",
                                MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
            if (VehicleLogoFile == null && inputIsValid)
            {
                inputIsValid = false;
                MessageBox.Show("Vehicle image is required",
                                "Required Data Missing",
                                MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }

            // Store the name of the vehicle
            VehicleName = m_vehicleNameTextBox.Text;
            if (inputIsValid)
            {
                DialogResult = DialogResult.OK;
                // Copy the image to our default location
                FileInfo imageFile = new FileInfo(VehicleLogoFile);
                DirectoryInfo imagesDir = new DirectoryInfo(ToyotaParameterEditor.Properties.Resources.VehicleLogoFileLocation);
                if (!imagesDir.Exists)
                {
                    try
                    {
                        imagesDir.Create();
                    }
                    catch
                    {
                        MessageBox.Show("Error creating local images directory",
                                        "Folder Access Error",
                                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
                String imageFileName = ToyotaParameterEditor.Properties.Resources.VehicleLogoFileLocation + "\\toyota-" + VehicleName.ToLower() + ".jpg";
                if (!File.Exists(imageFileName))
                {
                    try
                    {
                        imageFile.CopyTo(imageFileName, true);
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Unable to access " + ToyotaParameterEditor.Properties.Resources.VehicleLogoFileLocation + " :"
                                        + ex.Message,
                                        "Folder Access Error",
                                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
                if (!UpdatedFiles.Contains(imageFileName))
                {
                    m_updatedFiles.Add(imageFileName);
                }
                // Get the pushbutton number and update the CCRT files
                PushButtonNumber = m_vehicleButtonNumberComboBox.Items[m_vehicleButtonNumberComboBox.SelectedIndex].ToString();
                UpdateCcrtFiles();

                // Hide the window
                Visible = false;                
            }
            Console.WriteLine(DialogResult);
        }

        /// <summary>
        /// Abandon the new vehicle addtion 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_cancelButton_Click(object sender, EventArgs e)
        {
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
            openDialog.InitialDirectory = "C:\\";
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