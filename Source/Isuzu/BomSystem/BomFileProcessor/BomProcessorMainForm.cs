using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace BomFileProcessor
{
    public partial class BomProcessorMainForm : Form
    {
        public BomProcessorMainForm()
        {
            InitializeComponent();
            Icon = Icon.FromHandle(BomFileProcessor.Properties.Resources.bep_best.GetHicon());
            // Open the log file
            SetupMessageWindowColorTable();
            m_logger = new Logger(m_logMessageTextBox);
            // Setup initial BOM file locations
            SetupBomFileLocations();
            VehicleBuildDirectory = BomFileProcessor.Properties.Settings.Default.VehicleBuildDirectory;
            if (VehicleBuildDirectory == BomFileProcessor.Properties.Resources.DefaultVehicleBuildFolder)
            {
                SetVehicleBuildDirectory();
            }
            // Load the build item collection
            m_buildItems = new BuildItemCollection(m_logger);
            m_buildItems.Load();
            // Create and load vehicle option files
            m_axleTypes = new VehicleOptionCollection(m_logger, "Axle");
            m_axleTypes.Load();
            m_wheelbase = new VehicleOptionCollection(m_logger, "WheelbasePosition");
            m_wheelbase.Load();
            // Create and load the brake force parameter sets
            m_brakeForces = new BrakeForceCollection(m_logger);
            // Start the timer to look for new BOM files
            m_fileCheckTimer.Start();
        }


        // ----------------------------------------------------------------------------------------
        // Member Functions
        // ----------------------------------------------------------------------------------------
        /// <summary>
        /// Add a new vehicle build parameter to the file.
        /// </summary>
        /// <param name="buildData">Vehicle Build File</param>
        /// <param name="parameterPath">XPath to the parameter</param>
        /// <param name="parameterName">Name of the parameter to add.</param>
        /// <param name="parameterValue">Parameter value to add.</param>
        private void AddVehicleBuildParameter(XmlDocument buildData, String parameterPath, String parameterName, String parameterValue)
        {
            XmlNode paramNode;
            try
            {
                paramNode = buildData.DocumentElement.SelectSingleNode(parameterPath).SelectSingleNode(parameterName);
            }
            catch
            {
                XmlElement brakeForces = buildData.CreateElement(parameterPath.Substring(parameterPath.LastIndexOf("/") + 1));
                string vehicleBuildPath = parameterPath.Substring(0, parameterPath.LastIndexOf("/"));
                buildData.DocumentElement.SelectSingleNode(vehicleBuildPath).AppendChild(brakeForces);
                XmlElement newItem = buildData.CreateElement(parameterName);
                buildData.DocumentElement.SelectSingleNode(parameterPath).AppendChild(newItem);
                paramNode = buildData.DocumentElement.SelectSingleNode(parameterPath).SelectSingleNode(parameterName);
            }
            if (paramNode == null)
            {   // Node does not exist, create it
                XmlElement newItem = buildData.CreateElement(parameterName);
                buildData.DocumentElement.SelectSingleNode(parameterPath).AppendChild(newItem);
                paramNode = buildData.DocumentElement.SelectSingleNode(parameterPath).SelectSingleNode(parameterName);
            }
            // Update the value of the node
            if (paramNode.FirstChild != null)
            {
                if (paramNode.FirstChild.Value != parameterValue)
                {
                    paramNode.FirstChild.Value = parameterValue;
                }
            }
            else
            {
                paramNode.InnerText = parameterValue;
            }
        }

        /// <summary>
        /// Add the brake forces to the parameter file.
        /// </summary>
        /// <param name="buildData">Vehicle Build File</param>
        /// <param name="brakeForceSet">Brake Force Parameters to add to the build data.</param>
        private void AddBrakeForceParameters(XmlDocument buildData, BrakeForce brakeForceSet)
        {   // Add each brake force parameter to the build data
            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "FrontMaxDragForce", brakeForceSet.FrontMaxDragForce.ToString());
            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "RearMaxDragForce", brakeForceSet.RearMaxDragForce.ToString());

            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "FrontMinBrakeForce", brakeForceSet.FrontMinBrakeForce.ToString());
            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "RearMinBrakeForce", brakeForceSet.RearMinBrakeForce.ToString());
            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "FrontMaxBrakeForce", brakeForceSet.FrontMaxBrakeForce.ToString());
            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "RearMaxBrakeForce", brakeForceSet.RearMaxBrakeForce.ToString());

            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "FrontSideToSideBalance", brakeForceSet.FrontBalanceOffset.ToString());
            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "RearSideToSideBalance", brakeForceSet.RearBalanceOffset.ToString());

            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "BalanceFrontToRearMin", brakeForceSet.FrontToRearBalanceMin.ToString());
            AddVehicleBuildParameter(buildData, "VehicleBuild/BrakeForces", "BalanceFrontToRearMax", brakeForceSet.FrontToRearBalanceMax.ToString());
        }

        /// <summary>
        /// Parse the data from the BOM file.
        /// </summary>
        /// <param name="data">BOM data to be parsed.</param>
        /// <returns>Result of parsing the BOM data.</returns>
        private Boolean ParseBomDataLine(String line, XmlDocument buildData)
        {
            Boolean result = false;
            // Check if this line should be processed
            if (line.Length >= BomFileProcessor.Properties.Resources.BomDetailLineIndicator.Length)
            {
                if (line.Substring(0, BomFileProcessor.Properties.Resources.BomDetailLineIndicator.Length) ==
                    BomFileProcessor.Properties.Resources.BomDetailLineIndicator)
                {   // This is a detail line, get the upc and fna
                    if (line.Length >= BomFileProcessor.Properties.Settings.Default.BomUpcIndex + BomFileProcessor.Properties.Settings.Default.BomUpcLength)
                    {
                        String upc = line.Substring(BomFileProcessor.Properties.Settings.Default.BomUpcIndex,
                                                    BomFileProcessor.Properties.Settings.Default.BomUpcLength);
                        if (line.Length >= BomFileProcessor.Properties.Settings.Default.BomFnaIndex + BomFileProcessor.Properties.Settings.Default.BomFnaLength)
                        {
                            String fna = line.Substring(BomFileProcessor.Properties.Settings.Default.BomFnaIndex,
                                                        BomFileProcessor.Properties.Settings.Default.BomFnaLength);
                            // Check if this matches any of our build items
                            BuildItem item = m_buildItems.FindItem(upc, fna);
                            if (item != null)
                            {   // This is an extractable item
                                String value = line.Substring(BomFileProcessor.Properties.Settings.Default.BomPartNumberIndex,
                                                              BomFileProcessor.Properties.Settings.Default.BomPartNumberLength);
                                String name = item.ItemName;
                                result = true;
                                // Add the build item to the Vehicle Build data xml
                                AddVehicleBuildParameter(buildData, "VehicleBuild", name, value);
                            }
                            else
                            {
                                return true;
                            }
                        }
                        else
                        {//line does not contain length info
                            m_logger.Log("INFO:  Line found which is not long enough for FNA info - line contents: " + line);
                        }
                    }
                    else
                    {//line does not contain length info
                        m_logger.Log("INFO:  Line found which is not long enough for UPC info - line contents: " + line);
                    }
                }
                else
                {
                    return true;
                }
            }
            else
            {//line does not contain length info
                m_logger.Log("INFO:  Line found which does not contain length indication - line contents: " + line);
            }
            return result;
        }

        /// <summary>
        /// Process the specified BOM file.
        /// The BOM file will be parsed and then moved to the Archive directory.
        /// A result file will be created so Spartan can FTP the result back to their system.
        /// </summary>
        /// <param name="file">BOM file to be processed.</param>
        private void ProcessBomFile(String file)
        {   // Read the contents of the file
            m_logger.Log("INFO:  Processing file " + file);
            String processingResult = "c";
            // Read the BOM file and parse the data
            StreamReader reader = TryOpen(file,20,50);
            if (reader != null)
            {// Read the header line
                    String line = reader.ReadLine();
                    // Make sure this is the header line
                    if (line.Substring(0, 2) == "FH")
                    {   // Get the number of BOM records in this file
                        Int32 numberOfBoms = Convert.ToInt32(line.Substring(BomFileProcessor.Properties.Settings.Default.BomCountIndex,
                                                                            BomFileProcessor.Properties.Settings.Default.BomCountLength));
                        m_logger.Log("INFO:  Preparing to extract " + numberOfBoms.ToString() + " BOM Records");
                        // Process each BOM entry
                        for (Int32 bomIndex = 0; bomIndex < numberOfBoms; bomIndex++)
                        {   // Read the number of detail records in the current BOM
                            line = reader.ReadLine();
                            if (line == null)
                            {
                                m_logger.Log("ERROR:  Improperly formatted BOM file - missing BOM header record line (BH)");
                                processingResult = "x";
                                //break out of processing loop
                                break;
                            }
                            if (line.Substring(0, 2) == "BH")
                            {   // Get the number of detail records for this BOM
                                Int32 recordCount = Convert.ToInt32(line.Substring(BomFileProcessor.Properties.Settings.Default.BomRecordCountIndex,
                                                                                   BomFileProcessor.Properties.Settings.Default.BomRecordCountLength));
                                m_logger.Log("INFO:  Processing " + recordCount.ToString() + " records for BOM " + bomIndex.ToString());
                                // Load the Vehicle Build File Template
                                XmlDocument buildData = new XmlDocument();
                                buildData.Load("VehicleBuildFileTemplate.xml");
                                String bomFileName = BomFileProcessor.Properties.Settings.Default.VehicleBuildDirectory + "\\";
                                // Process each line of the BOM
                                for (Int32 recordIndex = 0; (recordIndex < recordCount) && (line != null); recordIndex++)
                                {   // Read the next line from the file
                                    line = reader.ReadLine();
                                    if (line == null)
                                    {
                                        m_logger.Log("ERROR:  Improperly formatted BOM file - missing expected bom data line");
                                        processingResult = "x";
                                        //break out of processing loop
                                        break;
                                    }
                                    // If this is the first record of the BOM, extract identifying data
                                    if (recordIndex == 0)
                                    {
                                        String modelCode = line.Substring(BomFileProcessor.Properties.Settings.Default.BomModelCodeIndex,
                                                                          BomFileProcessor.Properties.Settings.Default.BomModelCodeLength);
                                        String modelYear = line.Substring(BomFileProcessor.Properties.Settings.Default.BomModelYearIndex,
                                                                          BomFileProcessor.Properties.Settings.Default.BomModelYearLength);
                                        String lotNumber = line.Substring(BomFileProcessor.Properties.Settings.Default.BomLotNumberIndex,
                                                                          BomFileProcessor.Properties.Settings.Default.BomLotNumberLength);
                                        String bookCode = line.Substring(BomFileProcessor.Properties.Settings.Default.BomBookCodeIndex,
                                                                         BomFileProcessor.Properties.Settings.Default.BomBookCodeLength);
                                        bomFileName = bomFileName + "Ps" + modelCode + "_" + bookCode + "_" + modelYear + "_" + lotNumber + ".xml";
                                        m_logger.Log("INFO:  Build data will be written to " + bomFileName);
                                        // Set the wheelbase and axle type
                                        VehicleOption wheelbaseOption = m_wheelbase.Find(modelCode);
                                        if (wheelbaseOption != null)
                                        {
                                            Int32 wheelbase = Convert.ToInt32(Convert.ToDouble(wheelbaseOption.OptionValue) * 25.4);
                                            AddVehicleBuildParameter(buildData, "VehicleBuild", "WheelbasePositionInchesX10", wheelbase.ToString());
                                        }
                                        VehicleOption axleType = m_axleTypes.Find(modelCode);
                                        if (axleType != null)
                                        {
                                            AddVehicleBuildParameter(buildData, "VehicleBuild", "Axle", axleType.OptionValue);
                                        }
                                        // Add the brake force parameters
                                        BrakeForce brakeForces = m_brakeForces.Find(modelCode);
                                        if (brakeForces != null)
                                        {
                                            AddBrakeForceParameters(buildData, brakeForces);
                                        }
                                    }
                                    // Process the line
                                    if (!ParseBomDataLine(line, buildData))
                                    {//indicate failure
                                        m_logger.Log("ERROR:  Improperly formatted BOM file - missing BOM information");
                                        processingResult = "x";
                                        break;
                                    }
                                }

                                // The current BOM is complete, save the build data
                                try
                                {
                                    buildData.Save(bomFileName);
                                    m_logger.Log("INFO:  Wrote build data to " + bomFileName);
                                }
                                // The directory where build data was to be saved was not found or unavailable
                                catch (System.IO.DirectoryNotFoundException e)
                                {
                                    m_logger.Log("ERROR:  Location " + bomFileName + " is unavailable. Build data has not been saved!");
                                }
                            }
                            else
                            {   // Improperly formatted BOM file
                                m_logger.Log("ERROR:  Improperly formatted BOM file - missing BOM header record line (BH)");
                                processingResult = "x";
                            }
                        }
                    }
                    else
                    {   // Improperly formatted BOM file
                        m_logger.Log("ERROR:  Improperly formatted BOM file - missing file record header line (FH)");
                        processingResult = "x";
                    }
                reader.Close();
            }
            else
            {
                m_logger.Log("ERROR:  Unable to open File: " + file);
            }
            // Move file to archive location
            m_logger.Log("INFO:  Moving " + file + " to Archive");
            String newFileName = BomArchive + "\\" + file.Substring(file.LastIndexOf("\\"));
            if (File.Exists(newFileName))
            {//delete existing file
                File.Delete(newFileName);
                m_logger.Log("INFO:  Deleted old archive file: " + newFileName);
            }
            Directory.Move(file, newFileName);
            // Create a file name to FTP back to Spartan
            String ftpFile = file;
            int resultIndex = ftpFile.LastIndexOf(".");
            ftpFile = ftpFile.Insert(resultIndex, processingResult);
            if (File.Exists(BomResultDirectory + "\\" + ftpFile.Substring(ftpFile.LastIndexOf("\\"))))
            {//delete existing file
                File.Delete(BomResultDirectory + "\\" + ftpFile.Substring(ftpFile.LastIndexOf("\\")));
                m_logger.Log("INFO:  Deleted old result file: " + BomResultDirectory + "\\" + ftpFile.Substring(ftpFile.LastIndexOf("\\")));
            }
            using (StreamWriter writer = new StreamWriter(BomResultDirectory + "\\" + ftpFile.Substring(ftpFile.LastIndexOf("\\"))))
            {
                writer.WriteLine(processingResult);
            }
            m_logger.Log("INFO:  Created result file: " + ftpFile);
        }

        /// <summary>
        /// Tries to open a file, with a user defined number of attempt and Sleep delay between attempts.
        /// </summary>
        /// <param name="filePath">The full file path to be opened</param>
        /// <param name="maximumAttempts">The total number of attempts to make (multiply by attemptWaitMS for the maximum time the function with Try opening the file)</param>
        /// <param name="attemptWaitMS">The delay in Milliseconds between each attempt.</param>
        /// <returns>A valid FileStream object for the opened file, or null if the File could not be opened after the required attempts</returns>
        public StreamReader TryOpen(string filePath, int maximumAttempts, int attemptWaitMS)
        {
            StreamReader fs = null;
            int attempts = 0;

            // Loop allow multiple attempts
            while (true)
            {
                try
                {
                    fs = new StreamReader(filePath);
                    //If we get here, the File.Open succeeded, so break out of the loop and return the FileStream
                    break;
                }
                catch (IOException ioEx)
                {
                    // IOException is thrown if the file is in use by another process.
                    m_logger.Log("INFO:  Waiting for file Transfer completion # " + attempts.ToString());
                    // Check the numbere of attempts to ensure no infinite loop
                    attempts++;
                    if (attempts > maximumAttempts)
                    {
                        // Too many attempts,cannot Open File, break and return null 
                        fs = null;
                        break;
                    }
                    else
                    {
                        // Sleep before making another attempt
                        System.Threading.Thread.Sleep(attemptWaitMS);

                    }

                }

            }
            // Return the stream, may be valid or null
            return fs;
        }


        /// <summary>
        /// Bring up a folder browser dialog so the user can choose a new BOM file location.
        /// </summary>
        private String SelectBomDirectory(String initPath, String bomType)
        {   // Create a new directory chooser dialog
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select location of BOM " + bomType;
            dlg.ShowNewFolderButton = true;
            dlg.SelectedPath = initPath;
            String directory = "";
            if (dlg.ShowDialog() == DialogResult.OK)
            {   // Store  the new location
                directory = dlg.SelectedPath;
            }
            return directory;
        }

        /// <summary>
        /// Set the BOM file location.
        /// </summary>
        private void SetBomDirectory()
        {
            String directory = SelectBomDirectory(BomFileProcessor.Properties.Settings.Default.BomDirectory, "files");
            if (directory.Length > 0)
            {
                BomFileProcessor.Properties.Settings.Default.BomDirectory = directory;
                BomFileProcessor.Properties.Settings.Default.Save();
                BomDirectory = directory;
            }
        }

        /// <summary>
        /// Set the location for storing processed BOM files.
        /// </summary>
        private void SetBomArchiveDirectory()
        {
            String directory = SelectBomDirectory(BomFileProcessor.Properties.Settings.Default.BomArchive, "archive");
            if (directory.Length > 0)
            {
                BomFileProcessor.Properties.Settings.Default.BomArchive = directory;
                BomFileProcessor.Properties.Settings.Default.Save();
                BomArchive = directory;
            }
        }

        /// <summary>
        /// Set the location for writing the BOM processing result files.
        /// </summary>
        private void SetBomResultDirectory()
        {
            String directory = SelectBomDirectory(BomFileProcessor.Properties.Settings.Default.BomResultDirectory, "result");
            if (directory.Length > 0)
            {
                BomFileProcessor.Properties.Settings.Default.BomResultDirectory = directory;
                BomFileProcessor.Properties.Settings.Default.Save();
                BomResultDirectory = directory;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        private void SetupBomFileLocations()
        {   // Determine if the BOM file location needs to be updated
            BomDirectory = BomFileProcessor.Properties.Settings.Default.BomDirectory;
            // If still set to the default BOM directory, have user select new location
            if (BomDirectory == BomFileProcessor.Properties.Resources.DefaultBomDirectory)
            {   // Have the use select a new BOM directory location
                SetBomDirectory();
            }
            // Determine if the BOM archive location needs to be updated
            BomArchive = BomFileProcessor.Properties.Settings.Default.BomArchive;
            // If still set to default location, make the user update it
            if (BomDirectory == BomFileProcessor.Properties.Resources.DefaultBomArchive)
            {   // Have the user select a new location
                SetBomArchiveDirectory();
            }
            // Determine if the BOM result location needs to be updated
            BomResultDirectory = BomFileProcessor.Properties.Settings.Default.BomResultDirectory;
            if (BomResultDirectory == BomFileProcessor.Properties.Resources.DefaultBomResultDirectory)
            {   // Have the user select a new location
                SetBomResultDirectory();
            }
        }

        /// <summary>
        /// Setup the color table so we can display colored text.
        /// </summary>
        private void SetupMessageWindowColorTable()
        {   // Search for color table info. If it exists (it shouldn't,
            // but we'll check anyway) remove it and replace with our one
            int iCTableStart = m_logMessageTextBox.Rtf.IndexOf("colortbl;");
            if (iCTableStart != -1) //then colortbl exists
            {   // Find end of colortbl tab by searching forward from the colortbl tab itself
                int iCTableEnd = m_logMessageTextBox.Rtf.IndexOf('}', iCTableStart);
                // Remove the existing color table
                m_logMessageTextBox.Rtf = m_logMessageTextBox.Rtf.Remove(iCTableStart, iCTableEnd - iCTableStart);
                // Now insert new colour table at index of old colortbl tag
                m_logMessageTextBox.Rtf = m_logMessageTextBox.Rtf.Insert(iCTableStart,
                                       "{\\colortbl;\\red255\\green0\\blue0;\\red0\\green128\\blue0;\\red0\\green0\\blue255;\\red0\\green0\\blue0;}");
            }
            // Color table doesn't exist yet, so let's make one
            else
            {   // Find index of start of header
                int iRTFLoc = m_logMessageTextBox.Rtf.IndexOf("\\rtf");
                // Get index of where we'll insert the colour table try finding opening bracket of first property of header first                
                int iInsertLoc = m_logMessageTextBox.Rtf.IndexOf('{', iRTFLoc);
                // If there is no property, we'll insert color table just before the end bracket of the header
                if (iInsertLoc == -1) iInsertLoc = m_logMessageTextBox.Rtf.IndexOf('}', iRTFLoc) - 1;
                // Insert the colour table at our chosen location                
                m_logMessageTextBox.Rtf = m_logMessageTextBox.Rtf.Insert(iInsertLoc,
                                       "{\\colortbl;\\red255\\green0\\blue0;\\red0\\green128\\blue0;\\red0\\green0\\blue255;\\red0\\green0\\blue0;}");
            }
        }

        /// <summary>
        /// Display a folder browser dialog to allow the user to select the vehicle build 
        /// file folder.
        /// </summary>
        private void SetVehicleBuildDirectory()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select build file location";
            dlg.ShowNewFolderButton = true;
            dlg.SelectedPath = BomFileProcessor.Properties.Settings.Default.VehicleBuildDirectory;
            if (dlg.ShowDialog() == DialogResult.OK)
            {   // Store the new location
                BomFileProcessor.Properties.Settings.Default.VehicleBuildDirectory = dlg.SelectedPath;
                BomFileProcessor.Properties.Settings.Default.Save();
                VehicleBuildDirectory = dlg.SelectedPath;
            }
        }

        // ----------------------------------------------------------------------------------------
        // Callback Functions
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Setup the axle type associations
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void axleTypesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            List<String> values = new List<String>();
            values.Add("Limited Slip Differential");
            values.Add("Standard");
            VehicleOptionForm axleForm = new VehicleOptionForm(m_axleTypes, m_axleTypes.OptionName, values);
            axleForm.ShowDialog();
        }

        /// <summary>
        /// Setup the indices and lengths for fields in the BOM file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bOMFileSetupToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BomFileSetupForm bomSetup = new BomFileSetupForm();
            bomSetup.ShowDialog();
        }

        /// <summary>
        /// Display a tool window to allow the user to configre brake force parameters for a vehicle type.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void brakeForcesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BrakeForceSetupForm form = new BrakeForceSetupForm(m_brakeForces);
            form.ShowDialog();
        }

        /// <summary>
        /// Allow the user to edit an existing parameter set.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void existingParameterSetsToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Allow the user to select the desired file to edit
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "xml | *.xml";
            dlg.InitialDirectory = BomFileProcessor.Properties.Settings.Default.VehicleBuildDirectory;
            dlg.Multiselect = true;
            dlg.Title = "Select Parameter Files to Edit";
            if (dlg.ShowDialog() == DialogResult.OK)
            {   // open each selected file in a new editor window
                foreach (String file in dlg.FileNames)
                {
                    EditVehicleBuildFileForm editForm = new EditVehicleBuildFileForm(file, m_axleTypes, m_brakeForces, m_wheelbase);
                    editForm.Show();
                }
            }
        }

        /// <summary>
        /// Close down the program.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        /// <summary>
        /// Export the log files to a selected location.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exportLogFilesToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Display a folder chooser for the user to select where to export log files to
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select folder for log files";
            dlg.ShowNewFolderButton = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {   // Save the log files to the selected location
                List<FileInfo> logFiles = m_logger.LogFileListing();
                foreach (FileInfo log in logFiles)
                {   // Copy the log file to the selected directory
                    log.CopyTo(dlg.SelectedPath + "\\" + log.Name, true);
                }
            }
        }

        /// <summary>
        /// Check for new files when the timer ticks.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_fileCheckTimer_Tick(object sender, EventArgs e)
        {   // Stop the timer while we are processing the information
            m_fileCheckTimer.Stop();
            // Check if there are any new files to process
            String[] newFiles = Directory.GetFiles(BomDirectory);
            // Make sure we are not processing directories
            if (newFiles.Count() > 0)
            {
                m_logger.Log("INFO:  Found " + Convert.ToString(newFiles.Count()) + " new BOM file(s)");
                System.Threading.Thread.Sleep(30000);
                //DirectoryInfo di = new DirectoryInfo(BomDirectory);
                //DateTime dt = di.LastWriteTime;
                //System.Threading.Thread.Sleep(250);
                //di = new DirectoryInfo(BomDirectory);
                //ensure file transfer is complete
                //while (di.LastWriteTime != dt)
               // {
               //     m_logger.Log("INFO:  Waiting for complete file transfer");
               //     di = new DirectoryInfo(BomDirectory);
               //     dt = di.LastWriteTime;
               //     System.Threading.Thread.Sleep(250);                    
               // }
                foreach (String file in newFiles)
                {   // Process the new files
                    //confirm file transfer is complete by attempting to open file
                    StreamReader sr = TryOpen(file,100,250);
                    if (sr != null)
                    {
                        sr.Close();
                        ProcessBomFile(file);
                    }
                    else
                    {
                        m_logger.Log("Error:  Timeout waiting for file:" + file + " Transfer");
                    }
                }
                m_logger.Log("INFO:  Done processing BOM files");
            }
            // Start the timer again
            m_fileCheckTimer.Start();
        }

        /// <summary>
        /// Select the location of the BOM archive directory.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void selectBOMArchiveFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetBomArchiveDirectory();
        }

        /// <summary>
        /// Display the dialog box for the user to select the BOM file location.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void selectBOMFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Aid the user in selecting a new BOM location
            SetBomDirectory();
        }

        /// <summary>
        /// Allow the use to select the directory for holding BOM result files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void selectBOMResultFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetBomResultDirectory();
        }

        /// <summary>
        /// Display the form so the user can configure the UPC and FNA data for each module.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void uPCFNASetupToolStripMenuItem_Click(object sender, EventArgs e)
        {
            UpcFnaSetupForm upc = new UpcFnaSetupForm(m_buildItems);
            upc.ShowDialog();
        }

        /// <summary>
        /// Allow the user to select a new location for the vehicle build files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void vehcleBuildFileFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetVehicleBuildDirectory();
        }

        /// <summary>
        /// Display the current log file in the log viewer form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void viewCurrentLogToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LogViewerForm viewer = new LogViewerForm(m_logger.CurrentLogFileName);
            viewer.Show();
        }

        /// <summary>
        /// The user will be presented with a list of log files to view.
        /// After selecting a log file, the file will be displayed in the log file viewer window.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void viewPreviousLogToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Display the dialog to allow the user to select a log file
            LogFileChooserDialog dialog = new LogFileChooserDialog(m_logger.LogFileListing());
            if (dialog.ShowDialog() == DialogResult.OK)
            {   // Get the list of log files
                List<String> logFiles = dialog.SelectedLogFiles;
                // Open each log file
                List<LogViewerForm> logViewers = new List<LogViewerForm>();
                foreach (String log in logFiles)
                {
                    LogViewerForm viewer = new LogViewerForm(log);
                    viewer.Show();
                    logViewers.Add(new LogViewerForm(log));
                }
            }
        }

        /// <summary>
        /// Setup the wheelbase associations
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void wheelbaseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            VehicleOptionForm wheelbaseForm = new VehicleOptionForm(m_wheelbase, m_wheelbase.OptionName);
            wheelbaseForm.ShowDialog();
        }



        // ----------------------------------------------------------------------------------------
        // Member Variables and Properties
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Directory location to look for new BOM files.
        /// </summary>
        private String m_bomDirectory;
        /// <summary>
        /// Get/Set the location of the BOM directory.
        /// </summary>
        private String BomDirectory
        {
            get { return m_bomDirectory; }
            set 
            {   // Save the new directory location
                m_bomDirectory = value; 
                // Log the data that the location was changed
                m_logger.Log("INFO:  Updated BOM directory to: " + m_bomDirectory);
            }
        }

        /// <summary>
        /// Directory location for storing processed BOM files.
        /// </summary>
        private String m_bomArchive;
        /// <summary>
        /// Get/Set the location of the BOM archive.
        /// </summary>
        private String BomArchive
        {
            get { return m_bomArchive; }
            set
            {   // Save the new archive location
                m_bomArchive = value;
                // Log the data that the location was changed
                m_logger.Log("INFO:  Updated BOM Archive directory to: " + m_bomArchive);
            }
        }

        /// <summary>
        /// Directory location for storing BOM results.
        /// </summary>
        private String m_bomResultDirectory;
        /// <summary>
        /// Get/Set the location of the BOM result files.
        /// </summary>
        private String BomResultDirectory
        {
            get { return m_bomResultDirectory; }
            set
            {
                m_bomResultDirectory = value;
                m_logger.Log("INFO:  Updated BOM result directory to: " + m_bomResultDirectory);
            }
        }

        /// <summary>
        /// Directory location for storing vehicle build files.
        /// </summary>
        private String m_vehicleBuildDirectory;
        /// <summary>
        /// Get/Set the location of the vehicle build files.
        /// </summary>
        private String VehicleBuildDirectory
        {
            get { return m_vehicleBuildDirectory; }
            set
            {
                m_vehicleBuildDirectory = value;
                m_logger.Log("INFO:  Update vehicle build file directory to: " + m_vehicleBuildDirectory);
            }
        }

        /// <summary>
        /// Logger interface
        /// </summary>
        private Logger m_logger;

        /// <summary>
        /// Brake force parameter sets that have been configured.
        /// </summary>
        private BrakeForceCollection m_brakeForces;

        /// <summary>
        /// Build items to be extracted from the log file.
        /// </summary>
        private BuildItemCollection m_buildItems;

        /// <summary>
        /// Axle type associations.
        /// </summary>
        private VehicleOptionCollection m_axleTypes;

        /// <summary>
        /// Wheelbase association.
        /// </summary>
        private VehicleOptionCollection m_wheelbase;

    }
}
