using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Security.AccessControl;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace ToyotaParameterEditor
{
    public partial class RemoveVehicleForm : Form
    {
        public RemoveVehicleForm()
        {
            InitializeComponent();
            UpdateAvailableVehicleList();
            m_updatedFiles = new List<String>();
            m_archiveFileOutput = new StringBuilder("");
        }

        /// <summary>
        /// Fill the selection box with all vehicles that either 
        ///  - have parameter files (BuildRecords), or
        ///  - appear in the the selection table 
        /// </summary>
        private void UpdateAvailableVehicleList(object sender, MouseEventArgs e)
        {
            FileInfo[] fileList;
            m_availableVehiclesComboBox.Items.Clear();
            m_availableVehiclesComboBox.SelectedItem = null;
            m_availableVehiclesComboBox.Text = "";
            try
            {   // First make sure required files are accessible
                if (Directory.Exists(ToyotaParameterEditor.Properties.Resources.ParameterFilePath) &&
                    File.Exists(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable))
                {
                    // Then check Build Records Files
                    DirectoryInfo vehicleBuildFiles = new DirectoryInfo(ToyotaParameterEditor.Properties.Resources.ParameterFilePath);
                    fileList = vehicleBuildFiles.GetFiles("Vin*");
                    foreach (FileInfo file in fileList)
                    {
                        String shortName = file.Name.Substring(3);  // chop off leading "Vin"
                        shortName = shortName.Substring(0, shortName.IndexOf('.'));   // chop off file exten
                        m_availableVehiclesComboBox.Items.Add(shortName);
                    }

                    // Then check the selection table
                    XmlDocument rollTestSelectionTable = new XmlDocument();
                    rollTestSelectionTable.Load(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable);
                    XmlNode selectionEntryNodes = rollTestSelectionTable.DocumentElement;
                    // Look through every node name and if its not already in the list, add it
                    foreach (XmlNode entry in selectionEntryNodes.ChildNodes)
                    {
                        if (!m_availableVehiclesComboBox.Items.Contains(entry.Name))
                        {
                            m_availableVehiclesComboBox.Items.Add(entry.Name);
                        }
                    }
                }
                else
                {
                    MessageBox.Show("Unable to access " + ToyotaParameterEditor.Properties.Resources.ParameterFilePath +
                                    "\n and \n" + ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable +
                                    "\n\nChanges cannot be made until access to both directores is restored.",
                                    "Folder Access Error",
                                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                
            }
            catch (Exception ex)
            {   
                MessageBox.Show("Error accessing " + ToyotaParameterEditor.Properties.Resources.ParameterFilePath +
                                " or " + ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable +
                                "\n\n Changes cannot be made until access to both directores is restored." +
                                "\n\n Exception: " + ex.Message,
                                "Folder Access Error",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
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

            if (missingFiles.Length > 0)
            {
                MessageBox.Show("The following required files could not be located:  \n\n" +
                                missingFiles + Environment.NewLine +
                                "Vehicle type was not removed.",
                                "Required Files Missing",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
                DialogResult = DialogResult.Abort;
            }
            else
            {
                m_archiveFileOutput.Append(Environment.NewLine + "- " + DateTime.Now.ToString() + 
                                           " Removed: " + Environment.NewLine);

                // Update the roll test selection file -------------------------------------------------
                XmlDocument rollTestSelectionTable = new XmlDocument();
                rollTestSelectionTable.Load(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable);
                XmlNode selectionEntryNodes = rollTestSelectionTable.DocumentElement;
                // find the node with the name that matches the vehicle to be removed, then archive & delete it
                foreach (XmlNode entry in selectionEntryNodes.ChildNodes)
                {
                    if (entry.Name == VehicleName)
                    {
                        m_archiveFileOutput.Append("RollTest SelectionTable Entry: " + Environment.NewLine + 
                                                    "\t" + entry.OuterXml + Environment.NewLine);
                        selectionEntryNodes.RemoveChild(entry);
                    }
                }

                rollTestSelectionTable.Save(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable);
                if (!UpdatedFiles.Contains(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable))
                {
                    m_updatedFiles.Add(ToyotaParameterEditor.Properties.Resources.RollTestSelectionTable);
                }

                // Update the brake test selection file -------------------------------------------------
                XmlDocument brakeTestSelectionTable = new XmlDocument();
                brakeTestSelectionTable.Load(ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable);
                XmlNode brakeSelectionEntryNodes = brakeTestSelectionTable.DocumentElement;
                // find the node with the name that matches the vehicle to be removed, then archive & delete it
                foreach (XmlNode entry in brakeSelectionEntryNodes.ChildNodes)
                {
                    if (entry.Name == VehicleName)
                    {
                        m_archiveFileOutput.Append("BrakeTest SelectionTable Entry: " + Environment.NewLine +
                                                    "\t" + entry.OuterXml + Environment.NewLine);
                        brakeSelectionEntryNodes.RemoveChild(entry);
                    }
                }

                brakeTestSelectionTable.Save(ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable);
                if (!UpdatedFiles.Contains(ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable))
                {
                    m_updatedFiles.Add(ToyotaParameterEditor.Properties.Resources.BrakeTestSelectionTable);
                }

                // Update the InputServer Config -------------------------------------------------
                XmlDocument inputServer = new XmlDocument();
                inputServer.Load(ToyotaParameterEditor.Properties.Resources.InputServerFileName);
                // Look-up, archive, & remove entry
                XmlNode vehicleTypeNodes = inputServer.DocumentElement.SelectSingleNode("Setup/InputDevices/PlcDataInput/Setup/VehicleTypeDecode");
                // find the node with the inner text that matches bodyStyle, then archive & delete it
                foreach (XmlNode vehicleType in vehicleTypeNodes.ChildNodes)
                {
                    if (vehicleType.InnerText == VehicleName)
                    {
                        m_archiveFileOutput.Append("InputServer SelectionNumber: " + Environment.NewLine +
                                                    "\t" + vehicleType.OuterXml + Environment.NewLine);
                        vehicleTypeNodes.RemoveChild(vehicleType);
                    }
                }

                inputServer.Save(ToyotaParameterEditor.Properties.Resources.InputServerFileName);
                if (!UpdatedFiles.Contains(ToyotaParameterEditor.Properties.Resources.InputServerFileName))
                {
                    m_updatedFiles.Add(ToyotaParameterEditor.Properties.Resources.InputServerFileName);
                }

                // Remove the Parameter file (Build Record) -------------------------------------------------
                if (m_removeParamFileCheckBox.Checked)
                {
                    String paramFile = Path.Combine(ToyotaParameterEditor.Properties.Resources.ParameterFilePath, "Vin" + VehicleName + ".xml");
                    try
                    {
                        if (File.Exists(paramFile))
                        {   // Copy & Delete file because 'Move' gives file access errors
                            File.Copy(paramFile, Path.Combine(ArchiveDirectory, "Vin" + VehicleName + ".xml"));
                            File.Delete(paramFile);
                            m_archiveFileOutput.Append("Parameter File: " + Environment.NewLine +
                                                        "\t" + "Vin" + VehicleName + ".xml" + Environment.NewLine);
                        }
                    }
                    catch (IOException ioEx)
                    {
                        MessageBox.Show("Could not archive " + paramFile +
                                        " Because an archive with this name already exists." +
                                        "\n\n Please remove or rename the existing archive, then retry." +
                                        "\n\n Exception: " + ioEx.Message,
                                        "File Access Error",
                                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    catch (Exception ex)
                    {   
                        MessageBox.Show("Could not archive " + paramFile +
                                        "\n\n Pleasure ensure the file is not Read-Only" +
                                        "\n\n Exception: " + ex.Message,
                                        "File Access Error",
                                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        /// <summary>
        /// Update the CCRT client files with the new vehicle type information.
        /// </summary>
        private void UpdateCcrtClientFiles()
        {   // remove the photo-related config items
        }
        
        /// <summary>
        /// Writes the archived data from the 'RemoveVehicle' process to file.
        /// Creates the file if it does not exist.
        /// Appends to the file if it does exist.
        /// </summary>
        private void WriteArchiveFile()
        {
            bool archiveComplete = false;
            DialogResult = DialogResult.OK;

            while (!archiveComplete && DialogResult == DialogResult.OK)
            {
                try
                {
                    string archiveFile = Path.Combine(ArchiveDirectory, VehicleName + "_Archive.txt");
                    if (!File.Exists(archiveFile))
                    {   // Create the file if needed
                        using (StreamWriter sw = File.CreateText(archiveFile))
                        {
                            sw.WriteLine(VehicleName + " Archive Record" + Environment.NewLine);
                        }
                    }

                    using (StreamWriter sw = File.AppendText(archiveFile))
                    {
                        sw.Write(m_archiveFileOutput.ToString());                        
                    }
                    archiveComplete = true;
                    // Reset the buffer to log the next remove operation
                    m_archiveFileOutput.Clear();
                    // Inform the user that the process is complete
                    DialogResult = MessageBox.Show(VehicleName + " successfully removed." +
                                                    "\n\nArchive details can be found in " + ArchiveDirectory,
                                                    "Removal Complete",
                                                    MessageBoxButtons.OK, MessageBoxIcon.Information);

                }
                catch (Exception ex)
                {   
                    DialogResult = MessageBox.Show("Could not write archive data to " + ArchiveDirectory +
                                                    "\n\n Pleasure ensure the directory exists and is not Read-Only, then click OK." +
                                                    "\n\n If you wish to abandon the archive and delete the data permanently, click Cancel." + 
                                                    "\n\n Exception: " + ex.Message,
                                                    "Folder Access Error",
                                                    MessageBoxButtons.OKCancel, MessageBoxIcon.Error);
                }
            }
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
        /// Get/Set the name of the vehicle.
        /// </summary>
        private String m_vehicleName;
        public String VehicleName
        {
            get { return m_vehicleName; }
            set { m_vehicleName = value; }
        }

        /// <summary>
        /// Get/Set the name of the vehicle.
        /// </summary>
        private String m_archiveDirectory;
        public String ArchiveDirectory
        {
            get { return m_archiveDirectory; }
            set { m_archiveDirectory = value; }
        }

        private StringBuilder m_archiveFileOutput;

        private void m_cancelButton_Click(object sender, EventArgs e)
        {
            // Hide the window
            Visible = false;
        }

        private void m_okButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.None;
            // validate vehicle name
            if (m_availableVehiclesComboBox.SelectedItem != null)
            {
                DialogResult = DialogResult.OK;
                VehicleName = m_availableVehiclesComboBox.SelectedItem.ToString();
                ArchiveDirectory = Path.Combine(Directory.GetCurrentDirectory(), ToyotaParameterEditor.Properties.Resources.RemoveVehicleArchiveDirectory);
                if (!Directory.Exists(ArchiveDirectory))
                {
                    Directory.CreateDirectory(ArchiveDirectory);
                }
                DialogResult = MessageBox.Show("Please confirm you would like to remove the selected configuraiton items for " +
                                                Environment.NewLine + Environment.NewLine + "  " + VehicleName,
                                                "Vehicle Removal Confirmation",
                                                MessageBoxButtons.OKCancel, MessageBoxIcon.Exclamation);
                if (DialogResult == DialogResult.OK)
                {
                    // Call Update file functions
                    UpdateCcrtFiles();
                }

                // write the archive file if the update was successful
                if (DialogResult == DialogResult.OK)
                {
                    WriteArchiveFile();
                }
            }
            else
            {
                MessageBox.Show("Existing vehicle name is required",
                                "Required Data Missing",
                                MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }            
            
        }

        private void UpdateAvailableVehicleList()
        {

        }
    }
}
