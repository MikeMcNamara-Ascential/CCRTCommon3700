using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace ToyotaParameterEditor
{
    public partial class ToyotaParameterEditorMainWindow : Form
    {
        public ToyotaParameterEditorMainWindow()
        {
            InitializeComponent();
            m_vehicleParameters = new ParameterFile();
            m_newVehicleForm = new NewVehicleDataForm();
            m_modifiedFiles = new List<String>();
            Icon = Icon.FromHandle(ToyotaParameterEditor.Properties.Resources.BepLogo.GetHicon());
            BringToFront();
        }

        /// <summary>
        /// Display all vehicle parameters for the user.
        /// </summary>
        private void DisplayParameters()
        {   // Clear all the grids first
            m_vehicleDataGridView.Rows.Clear();
            m_transmissionDataGridView.Rows.Clear();
            m_speedTargetDataGridView.Rows.Clear();
            m_brakeDataGridView.Rows.Clear();
            m_vehicleLogoPictureBox.BorderStyle = BorderStyle.None;
            // Now display the parameters
            DisplayVehicleLogo();
            DisplayVehicleParameters();
            DisplayBrakeParameters();
            DisplayTransmissionShiftPoints();
            DisplaySpeedTargets();
        }

        /// <summary>
        /// Display the logo for the vehicle type
        /// </summary>
        private void DisplayVehicleLogo()
        {
            String imageFile = "Images\\toyota-" + m_vehicleParameters.GetVehicleParameter("BodyStyle").ToLower() + ".jpg";
            if (File.Exists(imageFile))
            {
                m_vehicleLogoPictureBox.Image = Image.FromFile(imageFile);
                m_vehicleLogoPictureBox.BorderStyle = BorderStyle.Fixed3D;
            }
        }

        /// <summary>
        /// Display the vehicle type parameters
        /// </summary>
        private void DisplayVehicleParameters()
        {
            String []bodyStyle = {m_vehicleParameters.GetParameterDisplayText("BodyStyle"), 
                                  m_vehicleParameters.GetVehicleParameter("BodyStyle"), " "};
            m_vehicleDataGridView.Rows.Add(bodyStyle);
            String []driveAxle = {m_vehicleParameters.GetParameterDisplayText("DriveAxle"), 
                                  m_vehicleParameters.GetVehicleParameter("DriveAxle"), " " };
            m_vehicleDataGridView.Rows.Add(driveAxle);
            String[] wheelbase = {m_vehicleParameters.GetParameterDisplayText("WheelbasePositionInchesX10"), 
                                  m_vehicleParameters.GetVehicleParameter("WheelbasePositionInchesX10"), 
                                  m_vehicleParameters.GetParameterUnits("WheelbasePositionInchesX10")};
            m_vehicleDataGridView.Rows.Add(wheelbase);
        }

        /// <summary>
        /// Display the brake testing parameters
        /// </summary>
        private void DisplayBrakeParameters()
        {   // Brake stopping distance start measurement speed
            String startSpeedStr = m_vehicleParameters.GetVehicleParameter("BrakeDistanceTestStartMeasureSpeed");
            double startSpeed = Convert.ToDouble(startSpeedStr != " " ? startSpeedStr : "0.0");
            startSpeed *= 1.609344;
            String[] startMeasureSpeed = {m_vehicleParameters.GetParameterDisplayText("BrakeDistanceTestStartMeasureSpeed"), 
                                          Convert.ToString(startSpeed), "kph" };
            m_brakeDataGridView.Rows.Add(startMeasureSpeed);
            // Front brake distance parameters
            String[] frontMaxStopDist = {m_vehicleParameters.GetParameterDisplayText("FrontMaximumStoppingDistance"), 
                                         m_vehicleParameters.GetVehicleParameter("FrontMaximumStoppingDistance"),
                                         m_vehicleParameters.GetParameterUnits("FrontMaximumStoppingDistance")};
            m_brakeDataGridView.Rows.Add(frontMaxStopDist);
            String[] frontMinStopDist = {m_vehicleParameters.GetParameterDisplayText("FrontMinimumStoppingDistance"), 
                                         m_vehicleParameters.GetVehicleParameter("FrontMinimumStoppingDistance"),
                                         m_vehicleParameters.GetParameterUnits("FrontMinimumStoppingDistance")};
            m_brakeDataGridView.Rows.Add(frontMinStopDist);
            // Rear brake distance parameters
            String[] rearMaxStopDist = {m_vehicleParameters.GetParameterDisplayText("RearMaximumStoppingDistance"), 
                                        m_vehicleParameters.GetVehicleParameter("RearMaximumStoppingDistance"),
                                        m_vehicleParameters.GetParameterUnits("RearMaximumStoppingDistance")};
            m_brakeDataGridView.Rows.Add(rearMaxStopDist);
            String[] rearMinStopDist = {m_vehicleParameters.GetParameterDisplayText("RearMinimumStoppingDistance"), 
                                        m_vehicleParameters.GetVehicleParameter("RearMinimumStoppingDistance"),
                                        m_vehicleParameters.GetParameterUnits("RearMinimumStoppingDistance")};
            m_brakeDataGridView.Rows.Add(rearMinStopDist);
            // Max Brake Force Parameters
            String[] minFrontBrakeForce = {m_vehicleParameters.GetParameterDisplayText("RequiredBrakeForceFront"), 
                                           m_vehicleParameters.GetVehicleParameter("RequiredBrakeForceFront"),
                                           m_vehicleParameters.GetParameterUnits("RequiredBrakeForceFront")};
            String[] minRearBrakeForce = {m_vehicleParameters.GetParameterDisplayText("RequiredBrakeForceRear"), 
                                          m_vehicleParameters.GetVehicleParameter("RequiredBrakeForceRear"),
                                          m_vehicleParameters.GetParameterUnits("RequiredBrakeForceRear")};
            String[] maxFrontDiff = {m_vehicleParameters.GetParameterDisplayText("MaxFrontDifference"), 
                                     m_vehicleParameters.GetVehicleParameter("MaxFrontDifference"),
                                     m_vehicleParameters.GetParameterUnits("MaxFrontDifference")};
            String[] maxRearDiff = {m_vehicleParameters.GetParameterDisplayText("MaxRearDifference"), 
                                    m_vehicleParameters.GetVehicleParameter("MaxRearDifference"),
                                    m_vehicleParameters.GetParameterUnits("MaxRearDifference")};
            String[] minSamples = {m_vehicleParameters.GetParameterDisplayText("MaxBrakeSamples"),
                                      m_vehicleParameters.GetVehicleParameter("MaxBrakeSamples"),
                                      m_vehicleParameters.GetParameterUnits("MaxBrakeSamples")};
            m_brakeDataGridView.Rows.Add(minFrontBrakeForce);
            m_brakeDataGridView.Rows.Add(minRearBrakeForce);
            m_brakeDataGridView.Rows.Add(maxFrontDiff);
            m_brakeDataGridView.Rows.Add(maxRearDiff);
            m_brakeDataGridView.Rows.Add(minSamples);
            // Park brake parameters
            String[] minParkBrakeForce = {m_vehicleParameters.GetParameterDisplayText("RequiredParkBrakeForce"), 
                                          m_vehicleParameters.GetVehicleParameter("RequiredParkBrakeForce"),
                                          m_vehicleParameters.GetParameterUnits("RequiredParkBrakeForce")};
            String[] minTotalParkBrakeForce = {m_vehicleParameters.GetParameterDisplayText("MinimumTotalParkBrakeForce"), 
                                               m_vehicleParameters.GetVehicleParameter("MinimumTotalParkBrakeForce"),
                                               m_vehicleParameters.GetParameterUnits("MinimumTotalParkBrakeForce")};
            m_brakeDataGridView.Rows.Add(minParkBrakeForce);
            m_brakeDataGridView.Rows.Add(minTotalParkBrakeForce);
        }

        /// <summary>
        /// Display the shift points for the transmission
        /// </summary>
        private void DisplayTransmissionShiftPoints()
        {   // Do the L4 engine first
            String[] sp12Normal = {m_vehicleParameters.GetParameterDisplayText("ShiftPoint1-2LowNormalAccel"), "L4", 
                                   m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/MPH/ShiftPoint1-2LowNormalAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/MPH/ShiftPoint1-2HighNormalAccel"),
                                   m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/KPH/ShiftPoint1-2LowNormalAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/KPH/ShiftPoint1-2HighNormalAccel")};
            String[] sp23Normal = {m_vehicleParameters.GetParameterDisplayText("ShiftPoint2-3LowNormalAccel"), "L4", 
                                   m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/MPH/ShiftPoint2-3LowNormalAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/MPH/ShiftPoint2-3HighNormalAccel"),
                                   m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/KPH/ShiftPoint2-3LowNormalAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/KPH/ShiftPoint2-3HighNormalAccel")};
            String[] sp12Full = {m_vehicleParameters.GetParameterDisplayText("ShiftPoint1-2LowFullAccel"), "L4", 
                                 m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/MPH/ShiftPoint1-2LowFullAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/MPH/ShiftPoint1-2HighFullAccel"),
                                 m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/KPH/ShiftPoint1-2LowFullAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/KPH/ShiftPoint1-2HighFullAccel")};
            String[] sp23Full = {m_vehicleParameters.GetParameterDisplayText("ShiftPoint2-3LowFullAccel"), "L4", 
                                 m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/MPH/ShiftPoint2-3LowFullAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/MPH/ShiftPoint2-3HighFullAccel"),
                                 m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/KPH/ShiftPoint2-3LowFullAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/L4/KPH/ShiftPoint2-3HighFullAccel")};
            m_transmissionDataGridView.Rows.Add(sp12Normal);
            m_transmissionDataGridView.Rows.Add(sp12Full);
            m_transmissionDataGridView.Rows.Add(sp23Normal);
            m_transmissionDataGridView.Rows.Add(sp23Full);
            // Do the V6 engine
            String[] sp12NormalV6 = {m_vehicleParameters.GetParameterDisplayText("ShiftPoint1-2LowNormalAccel"), "V6", 
                                     m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/MPH/ShiftPoint1-2LowNormalAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/MPH/ShiftPoint1-2HighNormalAccel"),
                                     m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/KPH/ShiftPoint1-2LowNormalAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/KPH/ShiftPoint1-2HighNormalAccel")};
            String[] sp23NormalV6 = {m_vehicleParameters.GetParameterDisplayText("ShiftPoint2-3LowNormalAccel"), "V6", 
                                     m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/MPH/ShiftPoint2-3LowNormalAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/MPH/ShiftPoint2-3HighNormalAccel"),
                                     m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/KPH/ShiftPoint2-3LowNormalAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/KPH/ShiftPoint2-3HighNormalAccel")};
            String[] sp12FullV6 = {m_vehicleParameters.GetParameterDisplayText("ShiftPoint1-2LowFullAccel"), "V6", 
                                   m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/MPH/ShiftPoint1-2LowFullAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/MPH/ShiftPoint1-2HighFullAccel"),
                                   m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/KPH/ShiftPoint1-2LowFullAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/KPH/ShiftPoint1-2HighFullAccel")};
            String[] sp23FullV6 = {m_vehicleParameters.GetParameterDisplayText("ShiftPoint2-3LowFullAccel"), "V6", 
                                   m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/MPH/ShiftPoint2-3LowFullAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/MPH/ShiftPoint2-3HighFullAccel"),
                                   m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/KPH/ShiftPoint2-3LowFullAccel") + " - " + m_vehicleParameters.GetVehicleParameter("TransmissionShiftPoints/V6/KPH/ShiftPoint2-3HighFullAccel")};
            m_transmissionDataGridView.Rows.Add(sp12NormalV6);
            m_transmissionDataGridView.Rows.Add(sp12FullV6);
            m_transmissionDataGridView.Rows.Add(sp23NormalV6);
            m_transmissionDataGridView.Rows.Add(sp23FullV6);
        }

        /// <summary>
        /// Display all speed targets
        /// </summary>
        private void DisplaySpeedTargets()
        {   // Display the speedometer target speeds
            String[] speedometerTargetMph = {m_vehicleParameters.GetParameterDisplayText("MPH_SpeedometerSpeedTargetLow"),
                                             m_vehicleParameters.GetVehicleParameter("SpeedTargets/MPH/SpeedometerSpeedTargetLow") + " - " + m_vehicleParameters.GetVehicleParameter("SpeedTargets/MPH/SpeedometerSpeedTargetHi"),
                                             m_vehicleParameters.GetParameterUnits("SpeedTargets/MPH/SpeedometerSpeedTargetLow")};
            String[] speedometerTargetKph = {m_vehicleParameters.GetParameterDisplayText("KPH_SpeedometerSpeedTargetLow"),
                                             m_vehicleParameters.GetVehicleParameter("SpeedTargets/KPH/SpeedometerSpeedTargetLow") + " - " + m_vehicleParameters.GetVehicleParameter("SpeedTargets/KPH/SpeedometerSpeedTargetHi"),
                                             m_vehicleParameters.GetParameterUnits("SpeedTargets/KPH/SpeedometerSpeedTargetLow")};
            m_speedTargetDataGridView.Rows.Add(speedometerTargetMph);
            m_speedTargetDataGridView.Rows.Add(speedometerTargetKph);
            // Reverse test target speed
            String[] reverseTarget = {m_vehicleParameters.GetParameterDisplayText("ReverseTestSpeed"),
                                      m_vehicleParameters.GetVehicleParameter("ReverseTestSpeed"),
                                      m_vehicleParameters.GetParameterUnits("ReverseTestSpeed")};
            m_speedTargetDataGridView.Rows.Add(reverseTarget);
            // Add the all wheel drive target
            String[] awdParams = {m_vehicleParameters.GetParameterDisplayText("AwdTestSpeed"),
                                  m_vehicleParameters.GetVehicleParameter("AwdTestSpeed"),
                                  m_vehicleParameters.GetParameterUnits("AwdTestSpeed")};
            String[] awdDiff = {m_vehicleParameters.GetParameterDisplayText("AwdMaxAxleDiff"),
                                m_vehicleParameters.GetVehicleParameter("AwdMaxAxleDiff"),
                                m_vehicleParameters.GetParameterUnits("AwdMaxAxleDiff")};
            m_speedTargetDataGridView.Rows.Add(awdParams);
            m_speedTargetDataGridView.Rows.Add(awdDiff);
        }

        /// <summary>
        /// Update the selected parameter with the new value.
        /// </summary>
        /// <param name="parameterGrid">Parameter Grid the update is coming from.</param>
        /// <param name="cellData">Event data containing the cell information for the cell that was updated.</param>
        private void UpdateVehicleParameter(DataGridView parameterGrid, DataGridViewCellEventArgs cellData)
        {
            String paramName = m_vehicleParameters.GetParameterName(parameterGrid.Rows[cellData.RowIndex].Cells[0].Value.ToString());
            String updatedValue = parameterGrid.Rows[cellData.RowIndex].Cells[cellData.ColumnIndex].Value.ToString();
            // Check if the parameter has changed
            if (updatedValue != m_vehicleParameters.GetVehicleParameter(paramName))
            {
                m_vehicleParameters.UpdateVehicleParameter(paramName, updatedValue);
            }
        }

        /// <summary>
        /// Update the selected parameters with the new values.
        /// </summary>
        /// <param name="parameterGrid">Parameter grid the update is coming from.</param>
        /// <param name="cellData">Event data containing the cell informtaion for the cell that was updated.</param>
        /// <param name="parameterPath">Path in the parameter file to the parameter.</param>
        /// <param name="paramNameHi">Text to use to replace the "Low" string to get the high range parameter.</param>
        private void UpdateVehicleParameter(DataGridView parameterGrid, DataGridViewCellEventArgs cellData, String parameterPath, String paramNameHi)
        {   // Get the parameter name and convert any "_" to "/"
            String paramName = m_vehicleParameters.GetParameterName(parameterGrid.Rows[cellData.RowIndex].Cells[0].Value.ToString());
            String paramNameTemp = parameterPath + "/";
            if (paramName.IndexOf("_") != -1)
            {
                paramNameTemp += paramName.Substring(0, paramName.IndexOf("_")) + "/" + paramName.Substring(paramName.IndexOf("_") + 1);
            }
            else
            {
                paramNameTemp += paramName;
            }
            paramName = paramNameTemp;
            // Get the low and high values from the new cell value
            String []paramValues = parameterGrid.Rows[cellData.RowIndex].Cells[cellData.ColumnIndex].Value.ToString().Split('-');
            // Update both parameters in the parameter file
            m_vehicleParameters.UpdateVehicleParameter(paramName, paramValues[0].Trim());
            String paramNameHi1 = paramName.Substring(0, paramName.IndexOf("Low"));
            String paramNameHi2 = paramName.Substring(paramName.IndexOf("Low") + 3);
            paramNameHi = paramNameHi1 + paramNameHi + paramNameHi2;
            m_vehicleParameters.UpdateVehicleParameter(paramNameHi, paramValues[1].Trim());
        }

        /// <summary>
        /// If the parameter list has been changed, the user will be prompted to optionally save the modifications.
        /// </summary>
        private void SaveParameterList()
        {
            if (m_vehicleParameters.ParametersChanged)
            {
                if (MessageBox.Show("Parameters Have Changed, Save Updates?", "Save Changes", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    m_vehicleParameters.SaveParameterFile();
                    if (!m_modifiedFiles.Contains(m_vehicleParameters.ParameterFileName))
                    {   // File name is not yet in the list, add it to the list
                        m_modifiedFiles.Add(m_vehicleParameters.ParameterFileName);
                    }
                }
            }
        }

        /// <summary>
        /// Export all modified files to a user specified location.
        /// </summary>
        private void ExportModifiedFiles()
        {   // Get an export location from the user
            FolderBrowserDialog folderChooser = new FolderBrowserDialog();
            folderChooser.Description = "Select folder to export modified parameter files to";
            folderChooser.ShowNewFolderButton = true;
            if (folderChooser.ShowDialog() == DialogResult.OK)
            {   // Copy all modified files to the selected location
                foreach (String fileName in m_modifiedFiles)
                {   // Copy the current file to the selected folder
                    FileInfo file = new FileInfo(fileName);
                    file.CopyTo(folderChooser.SelectedPath + fileName.Substring(fileName.LastIndexOf("\\")), true);
                }
                // Clear the list of exported files
                m_modifiedFiles.Clear();
            }
        }

        /// <summary>
        /// Start the import process for all modified files.
        /// </summary>
        private void ImportUpdatedParameters()
        {   // Get the location to import files from
            FolderBrowserDialog folderChooser = new FolderBrowserDialog();
            folderChooser.Description = " Select folder to import updated parameter files from";
            folderChooser.ShowNewFolderButton = false;
            if (folderChooser.ShowDialog() == DialogResult.OK)
            {   // Import the files
                ImportFiles(folderChooser.SelectedPath);
                // Inform user to restart system so updates can be loaded
                MessageBox.Show("Updated parameters have been imported." + Environment.NewLine + "Restart system for updates to take affect.",
                                "Restart System", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// Import the files to the correct locations.
        /// </summary>
        private void ImportFiles(String filePath)
        {   // Open the folder
            DirectoryInfo directory = new DirectoryInfo(filePath);
            foreach (FileInfo file in directory.GetFiles())
            {   // Find where the file should be moved to
                String destination;
                if (file.Name.StartsWith("Vin"))
                {
                    destination = ToyotaParameterEditor.Properties.Resources.ParameterFilePath + "\\" + file.Name;
                }
                else if (file.Name.Contains("SelectionTable"))
                {
                    destination = ToyotaParameterEditor.Properties.Resources.SelectionTableLocation + "\\" + file.Name;
                }
                else if (file.Name == "Toyota.InputServerConfig.xml")
                {
                    destination = ToyotaParameterEditor.Properties.Resources.InputServerFileName;
                }
                else if (file.Name.StartsWith("toyota-"))
                {
                    destination = ToyotaParameterEditor.Properties.Resources.CcrtClientLogoFileLocation + "\\" + file.Name;
                }
                else if (file.Name == "CRTPanelLabels.constants")
                {
                    destination = ToyotaParameterEditor.Properties.Resources.CcrtClientLogoSelectFile;
                }
                else
                {   // Move the file to the images location
                    destination = ToyotaParameterEditor.Properties.Resources.VehicleLogoFileLocation + "\\" + file.Name;
                }
                // If the file already exists, remove it and then move the file
                if (File.Exists(destination))
                {
                    File.Delete(destination);
                }
                // Move the file to it's new home
                file.MoveTo(destination);
            }
        }

        /// <summary>
        /// Exit the parameter editor Application
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Check if the parameter file needs to be saved
            SaveParameterList();
            // Check if modified files need to be exported
            if (m_modifiedFiles.Count > 0)
            {
                if (MessageBox.Show("Export modified parameter files?", "Export Parameters", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    ExportModifiedFiles();
                }
            }
            Environment.Exit(0);
        }

        /// <summary>
        /// Bring up a selection box for the user to choose the parameter file to open.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void openVehicleToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Check if parameters should be saved before opening a new file
            SaveParameterList();
            // Allow the user to select a new parameter file
            OpenFileDialog openDlg = new OpenFileDialog();
            openDlg.DefaultExt = "xml";
            openDlg.Filter = "xml | *.xml";
            openDlg.InitialDirectory = "G:\\Rewrite\\Configuration\\VehicleTest\\BuildRecords";
            openDlg.Multiselect = false;
            openDlg.RestoreDirectory = true;
            openDlg.Title = "Select Vehicle Parameter List";
            if (openDlg.ShowDialog() == DialogResult.OK)
            {   // Get the name of the selected file
                String file = openDlg.FileName;
                m_vehicleParameters.LoadParameterFile(file);
                DisplayParameters();
            }
        }

        private ParameterFile m_vehicleParameters;
        private NewVehicleDataForm m_newVehicleForm;
        private List<String> m_modifiedFiles;

        /// <summary>
        /// If a parameter value has been updated, save the update to the parameter file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_vehicleDataGridView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {   // Update the parameter
            UpdateVehicleParameter(m_vehicleDataGridView, e);
        }

        /// <summary>
        /// Save the parameter file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void saveUpdatesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_vehicleParameters.SaveParameterFile();
            if (!m_modifiedFiles.Contains(m_vehicleParameters.ParameterFileName))
            {   // File name is not yet in the list, add it to the list
                m_modifiedFiles.Add(m_vehicleParameters.ParameterFileName);
            }
        }

        /// <summary>
        /// Update the brake parameters.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_brakeDataGridView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {   // Update the parameter
            UpdateVehicleParameter(m_brakeDataGridView, e);
        }

        /// <summary>
        /// Update the speed target parameters.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_speedTargetDataGridView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex > 1)
            {   // This is not a range parameter
                UpdateVehicleParameter(m_speedTargetDataGridView, e);
            }
            else
            {   // Update a range parameter
                UpdateVehicleParameter(m_speedTargetDataGridView, e, "SpeedTargets", "Hi");
            }
        }

        /// <summary>
        /// Update the transmission shift point parameters
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_transmissionDataGridView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            String path = "TransmissionShiftPoints/" + m_transmissionDataGridView.Rows[e.RowIndex].Cells[1].Value.ToString() +
                          "/" + m_transmissionDataGridView.Columns[e.ColumnIndex].HeaderText;
            UpdateVehicleParameter(m_transmissionDataGridView, e, path, "High");
        }

        /// <summary>
        /// Set up the form to allow the user to create a parameter list for a new vehicle.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void newVehicleToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Make sure to save any changes before loading the new vehicle template
            SaveParameterList();
            // Let the user enter the vehicle name
            m_newVehicleForm.ShowDialog();
            if (m_newVehicleForm.VehicleName.Length > 0)
            {   // Load the template
                m_vehicleParameters.LoadParameterFile(ToyotaParameterEditor.Properties.Resources.NewVehicleTemplateFileName);
                m_vehicleParameters.UpdateVehicleParameter("BodyStyle", m_newVehicleForm.VehicleName);
                m_modifiedFiles.AddRange(m_newVehicleForm.UpdatedFiles);
                DisplayParameters();
            }
        }

        /// <summary>
        /// Export all files that have been updated during this session.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exportUpdatedFilesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ExportModifiedFiles();
        }

        /// <summary>
        /// Import updated parameter files from a different machine.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void importUpdatedFilesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ImportUpdatedParameters();
        }
    }
}