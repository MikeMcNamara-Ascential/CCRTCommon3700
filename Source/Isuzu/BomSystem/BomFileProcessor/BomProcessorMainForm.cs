using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Xml;
using FtpFileMonitorNamespace;

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

            // Load the build item collection
            m_buildItems = new BuildItemCollection(m_logger);
            m_buildItems.Load();
            // Create and load vehicle option files
            m_axleTypes = new VehicleOptionCollection(m_logger, "Axle");
            m_axleTypes.Load();
            m_wheelbase = new VehicleOptionCollection(m_logger, "WheelbasePosition");
            m_wheelbase.Load();
            m_modelCodeOptions = new ModelCodeOptionsCollection(m_logger);
            m_modelCodeOptions.Load();
            m_retRollPressures = new VehicleOptionCollection(m_logger, "FrontReductionPressure");
            m_retRollPressures.Load();
            m_rearAxel = new VehicleOptionCollection(m_logger, "RearAxel");
            m_rearAxel.Load();
            m_transmission = new VehicleOptionCollection(m_logger, "Transmission");
            m_transmission.Load();
            m_tireSize = new VehicleOptionCollection(m_logger, "TireSize");
            m_tireSize.Load();
            m_speedMeter = new VehicleOptionCollection(m_logger, "SpeedMeter");
            m_speedMeter.Load();


            // Create and load the brake force parameter sets
            m_brakeForces = new BrakeForceCollection(m_logger);
            // Start the timer to look for new BOM files
            StartFileProcessorThread();

            List<string> remotePaths = new List<string>();
            remotePaths.Add(BomFileProcessor.Properties.Settings.Default.RealTimePCESNFileLocation);
            m_logger.Log("Added " + BomFileProcessor.Properties.Settings.Default.RealTimePCESNFileLocation + " to the remote paths");
            remotePaths.Add(BomFileProcessor.Properties.Settings.Default.RealTimePCESNFlashTransferLocation);
            m_logger.Log("Added " + BomFileProcessor.Properties.Settings.Default.RealTimePCESNFlashTransferLocation + " to the remote paths");
            // If two flash stations, remove otherwise
            remotePaths.Add(BomFileProcessor.Properties.Settings.Default.RealTimePCESNFlashTransferLocation2);
            m_logger.Log("Added " + BomFileProcessor.Properties.Settings.Default.RealTimePCESNFlashTransferLocation2 + " to the remote paths");

            List<string> users = new List<string>();
            users.Add("burke");
            users.Add("burke");

            List<string> passwords = new List<string>();
            passwords.Add("porter");
            passwords.Add("porter");

            List<string> ipaddresses = new List<string>();
            ipaddresses.Add("192.168.1.3");
            ipaddresses.Add("192.168.1.3");
            //create monitor to upload files to dvt.  do not start actual ftp file monitor since we are only transmitting
            m_engineSerialNumberFileMonitor = new BomFtpFileMonitor(remotePaths, BomFileProcessor.Properties.Settings.Default.WindowsPCESNFileLocation,
                users, passwords, ipaddresses,m_logger);

           //m_engineSerialNumberFileMonitor.StartFileMonitorThread();


            remotePaths.Clear();
            m_logger.Log("Added " + BomFileProcessor.Properties.Settings.Default.VehicleBuildDirectory + " to the remote paths");
            remotePaths.Add(BomFileProcessor.Properties.Settings.Default.VehicleBuildDirectory);
            m_logger.Log("Added " + BomFileProcessor.Properties.Settings.Default.VehicleBuildFlashTransferLocation + " to the remote paths");
            remotePaths.Add(BomFileProcessor.Properties.Settings.Default.VehicleBuildFlashTransferLocation);
            // If two flash stations, remove otherwise
            m_logger.Log("Added " + BomFileProcessor.Properties.Settings.Default.VehicleBuildFlashTransferLocation2 + " to the remote paths");
            remotePaths.Add(BomFileProcessor.Properties.Settings.Default.VehicleBuildFlashTransferLocation2);
            //create monitor to upload files to dvt.  do not start actual ftp file monitor since we are only transmitting
            
            m_buildRecordFileMonitor = new BomFtpFileMonitor(remotePaths, BomFileProcessor.Properties.Settings.Default.VehicleBuildTempDirectory,
                users, passwords, ipaddresses,m_logger);

            //m_buildRecordFileMonitor.StartFileMonitorThread();

            m_esnFileCheckTimer.Interval = BomFileProcessor.Properties.Settings.Default.PassConfirmationCheckDelay;
            m_esnFileCheckTimer.Start();

            // Check if we should start monitoring for pass confirmation files
            if (BomFileProcessor.Properties.Settings.Default.CheckForPassConfirmationFiles)
            {
                m_dvtPassConfirmationMonitor = new BomFtpFileMonitor(
                    BomFileProcessor.Properties.Settings.Default.CcrtFileLocation,
                    BomFileProcessor.Properties.Settings.Default.SpartanFileLocation,
                    BomFileProcessor.Properties.Settings.Default.SpartanFileTempLocation,
                     BomFileProcessor.Properties.Settings.Default.PassConfirmationCheckDelay,
                     "burke",
                     "porter",
                     "192.168.1.3", m_logger, "*.DVT");
                m_dvtPassConfirmationMonitor.StartFileMonitorThread();

                m_ecmPassConfirmationMonitor = new BomFtpFileMonitor(
                BomFileProcessor.Properties.Settings.Default.CcrtFileLocation,
                BomFileProcessor.Properties.Settings.Default.SpartanFlashFileLocation,
                BomFileProcessor.Properties.Settings.Default.SpartanFlashFileTempLocation,
                 BomFileProcessor.Properties.Settings.Default.PassConfirmationCheckDelay,
                 "burke",
                 "porter",
                 "192.168.1.3", m_logger, "*.ECM");
                m_ecmPassConfirmationMonitor.StartFileMonitorThread();
            }

            m_vinStampingFileMonitor = new BomFtpFileMonitor(
            BomFileProcessor.Properties.Settings.Default.CcrtVINStamperFileLocation,
            BomFileProcessor.Properties.Settings.Default.VINStamperFileLocation,
            BomFileProcessor.Properties.Settings.Default.VINStamperFileTempLocation,
            BomFileProcessor.Properties.Settings.Default.PassConfirmationCheckDelay,
            "burke",
            "porter",
            "192.168.1.3", m_logger, "*.STP");
            m_vinStampingFileMonitor.StartFileMonitorThread();

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
            StreamReader reader = TryOpen(file,10,500);
            if (reader != null)
            {// Read the header line
                    String line = reader.ReadLine();
                    if (line != null)
                    {
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
                                    String buildRecordFileName = BomFileProcessor.Properties.Settings.Default.VehicleBuildTempDirectory + "\\";
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
                                            buildRecordFileName = buildRecordFileName + "Ps" + modelCode + "_" + bookCode + "_" + modelYear + "_" + lotNumber + ".xml";
                                            m_logger.Log("INFO:  Build data will be written to " + buildRecordFileName);
                                            // Set the wheelbase and axle type
                                            VehicleOption wheelbaseOption = m_wheelbase.Find(modelCode);
                                            if (wheelbaseOption != null)
                                            {
                                                Int32 wheelbase = Convert.ToInt32(Convert.ToDouble(wheelbaseOption.OptionValue) * 25.4);
                                                AddVehicleBuildParameter(buildData, "VehicleBuild", "WheelbasePositionInchesX10", wheelbase.ToString());
                                            }
                                            VehicleOption retRollRelaxPressureOption = m_retRollPressures.Find(modelCode);
                                            if (retRollRelaxPressureOption != null)
                                            {
                                                Int32 retRollRelaxPressure = Convert.ToInt32(retRollRelaxPressureOption.OptionValue);
                                                AddVehicleBuildParameter(buildData, "VehicleBuild", "FrontReductionPressure", retRollRelaxPressure.ToString());
                                            }
                                            VehicleOption axleType = m_axleTypes.Find(modelCode);
                                            if (axleType != null)
                                            {
                                                AddVehicleBuildParameter(buildData, "VehicleBuild", "Axle", axleType.OptionValue);
                                            }
                                            ModelCodeOptions modelOptions = m_modelCodeOptions.Find(modelCode);
                                            if (modelOptions != null)
                                            {
                                                AddVehicleBuildParameter(buildData, "VehicleBuild", "WriteESN", modelOptions.WriteESN);
                                                AddVehicleBuildParameter(buildData, "VehicleBuild", "ESNLeadingCharacters", modelOptions.ESNLeadingCharacters);
                                            }
                                            //TODO Add the RearAxel, Transmission, Tire Size and SpeedMeter Build Record writing code here.
                                            VehicleOption RearAxleType = m_rearAxel.Find(modelCode);
                                            if (RearAxleType != null)
                                            {
                                                AddVehicleBuildParameter(buildData, "VehicleBuild", "Rearaxelratio", RearAxleType.OptionValue);
                                            }
                                            VehicleOption Transmission = m_transmission.Find(modelCode);
                                            if (Transmission != null)
                                            {
                                                AddVehicleBuildParameter(buildData, "VehicleBuild", "Transmission", Transmission.OptionValue);
                                            }
                                            VehicleOption TireSize = m_tireSize.Find(modelCode);
                                            if (TireSize != null)
                                            {
                                                AddVehicleBuildParameter(buildData, "VehicleBuild", "Tiresize", TireSize.OptionValue);
                                            } VehicleOption SpeedMeter = m_speedMeter.Find(modelCode);
                                            if (SpeedMeter != null)
                                            {
                                                AddVehicleBuildParameter(buildData, "VehicleBuild", "Speedmeter", SpeedMeter.OptionValue);
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
                                        buildData.Save(buildRecordFileName);
                                        m_logger.Log("INFO:  Wrote build data to " + buildRecordFileName);
                                    }
                                    // The directory where build data was to be saved was not found or unavailable
                                    catch (System.IO.DirectoryNotFoundException e)
                                    {
                                        m_logger.Log("ERROR:  Location " + buildRecordFileName + " is unavailable. Build data has not been saved!");
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
                    }
                    else
                    {
                        m_logger.Log("ERROR:  Unable to read from File: " + file);
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
            if (IsTransferComplete(filePath, maximumAttempts, attemptWaitMS))
            {
                try
                {
                    fs = new StreamReader(filePath);
                }
                catch (Exception e)
                {
                    m_logger.Log("Exception opening file: " + e.ToString());
                }
            }
            // Return the stream, may be valid or null
            return fs;
        }
        public bool IsTransferComplete(string filePath, int maximumAttempts, int attemptWaitMS)
        {
            int attempts = 0;
            FileInfo file = new FileInfo(filePath);
            while (IsFileLocked(file))
            {
                if (attempts > maximumAttempts)
                {
                    m_logger.Log("Error waiting for file transfer complete");
                    return false;
                }
                System.Threading.Thread.Sleep(attemptWaitMS);
                attempts++;
            }
            return true;
        }
        /// <summary>
        /// Code by ChrisW -> http://stackoverflow.com/questions/876473/is-there-a-way-to-check-if-a-file-is-in-use
        /// </summary>
        protected virtual bool IsFileLocked(FileInfo file)
        {
            FileStream stream = null;

            try
            {
                stream = file.Open(FileMode.Open, FileAccess.ReadWrite, FileShare.None);
            }
            catch (IOException)
            {
                return true;
            }
            finally
            {
                if (stream != null)
                    stream.Close();
            }

            //file is not locked
            return false;
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

        //Create a separate thread for processing BOM files
        public void StartFileProcessorThread()
        {
            ThreadStart taskDelegate = null;
            taskDelegate = new ThreadStart(ProcessAllFiles);
            m_fileProcessorThread = new Thread(taskDelegate);
            m_processFiles = true;

            m_fileProcessorThread.Start();
        }

        //Function for processing BOM files
        public void ProcessAllFiles()
        {
            while (m_processFiles)
            {// Check if there are any new files to process
                String[] newFiles = Directory.GetFiles(BomDirectory);
                // Make sure we are not processing directories
                if (newFiles.Count() > 0)
                {
                    m_logger.Log("INFO:  Found " + Convert.ToString(newFiles.Count()) + " new BOM file(s)");
                    System.Threading.Thread.Sleep(3000);
                    foreach (String file in newFiles)
                    {   // Process the new files
                        ProcessBomFile(file);
                    }
                    m_logger.Log("INFO:  Done processing BOM files");

                    try
                    {
                        String[] transferFiles = Directory.GetFiles(BomFileProcessor.Properties.Settings.Default.VehicleBuildTempDirectory);
                        if (transferFiles.Count() > 0)
                        {   // Process the CCRT generated pass confirmation files
                            m_logger.Log("INFO: Transfering " + Convert.ToString(transferFiles.Count()) + " Build File(s)");
                            // Process each file
                            if (m_buildRecordFileMonitor.UploadToSources(transferFiles.ToList()))
                            {
                                m_logger.Log("INFO:  Build file Transmit Successful");
                            }
                            else
                            {
                                m_logger.Log("INFO:  Build file Transmit Error, Retransmitting...");
                            }
                        }
                    }
                    catch (System.IO.IOException ex)
                    {
                        m_logger.Log("ERROR: IOException accessing file or folder - " + ex.Message);
                    }
                }
                else
                    System.Threading.Thread.Sleep(3000);
            }
        }

        //Function for viewing current log files using the LogViewerForm
        private void ViewCurrentLogFile()
        {
            LogViewerForm viewer = new LogViewerForm(m_logger, m_logger.CurrentLogFileName);
            viewer.ShowDialog();
        }

        //Function for viewing multiple log files using the LogViewerForm
        private void ViewMultipleLogFiles(string logFileName)
        {
            LogViewerForm viewer = new LogViewerForm(m_logger, logFileName);
            viewer.ShowDialog();       
        }

        //Function for viewing previous log files using the LogViewerForm
        private void ViewPreviousLogFiles()
        {
            // Display the dialog to allow the user to select a log file
            LogFileChooserDialog dialog = new LogFileChooserDialog(m_logger.LogFileListing());
            if (dialog.ShowDialog() == DialogResult.OK)
            {   // Get the list of log files
                List<String> logFiles = dialog.SelectedLogFiles;
                // Open each log file
                foreach (String log in logFiles)
                {   //Create a thread for each log window
                    Thread thread = new Thread(() => ViewMultipleLogFiles(log));
                    thread.Start();
                }
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
                    //EditVehicleBuildFileForm editForm = new EditVehicleBuildFileForm(file, m_axleTypes, m_brakeForces, m_wheelbase, m_retRollPressures);
                    //TODO ADD rearAxel Transmission TireSize and Speedo to this method.
                    EditVehicleBuildFileForm editForm = new EditVehicleBuildFileForm(file, m_axleTypes, m_brakeForces, m_wheelbase, m_retRollPressures, m_rearAxel, m_transmission, m_tireSize, m_speedMeter);
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
        /// check for new Pass Confirmation files to move from the CCRT directory to the Spartan directory.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_passFileCheckTimer_Tick(object sender, EventArgs e)
        {   // Stop the timer while processing any new pass confirmation files
            m_passFileCheckTimer.Stop();
            // Check for any new pass confirmation files
            try
            {
                String[] newFiles = Directory.GetFiles(BomFileProcessor.Properties.Settings.Default.CcrtFileLocation);
                if (newFiles.Count() > 0)
                {   // Process the CCRT generated pass confirmation files
                    m_logger.Log("INFO: Found " + Convert.ToString(newFiles.Count()) + " new CCRT Pass Confirmation File(s)");
                    System.Threading.Thread.Sleep(3000);   // Make sure any current files are closed
                    // Process each pass confirmation file
                    foreach (String file in newFiles)
                    {
                        if (IsTransferComplete(file, 10, 500))
                        {
                            String dest = BomFileProcessor.Properties.Settings.Default.SpartanFileLocation + "\\" + file.Substring(file.LastIndexOf('\\'));
                            String flashDest = BomFileProcessor.Properties.Settings.Default.SpartanFlashFileLocation + "\\" + file.Substring(file.LastIndexOf('\\'));
                            try
                            {
                                if (file.Substring(file.LastIndexOf('.')) == ".DVT")
                                {
                                    if (System.IO.File.Exists(dest))
                                    {
                                        File.Delete(dest);
                                    }
                                    File.Copy(file, dest);
                                    if (System.IO.File.Exists(dest))
                                    {//file successfully copied, delete local
                                        File.Delete(file);
                                    } m_logger.Log("INFO: Moved " + file + " to " + dest);
                                }
                                else
                                {
                                    if (System.IO.File.Exists(flashDest))
                                    {
                                        File.Delete(flashDest);
                                    }
                                    File.Copy(file, flashDest);
                                    if (System.IO.File.Exists(flashDest))
                                    {//file successfully copied, delete local
                                        File.Delete(file);
                                        }
                                    m_logger.Log("INFO: Moved " + file + " to " + flashDest);
                                }

                            }
                            catch (NotSupportedException excpt)
                            {
                                m_logger.Log("ERROR: NotSupportedException attempting to move pass confirmation file - " + excpt.Message);
                            }
                        }
                    }
                }
            }
            
            catch(System.IO.IOException ex)
            {
                m_logger.Log("ERROR: IOException accessing file or folder - " + ex.Message);
            }
            // Restart the timer to look for more pass confirmation files
            m_passFileCheckTimer.Start();
        }

        /// <summary>
        /// check for new ESN files to move from the Windows directory to the CCRT directory.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_esnFileCheckTimer_Tick(object sender, EventArgs e)
        {   // Stop the timer while processing any new pass confirmation files
            m_esnFileCheckTimer.Stop();
            // Check for any new pass confirmation files
            try
            {
                String[] newFiles = Directory.GetFiles(BomFileProcessor.Properties.Settings.Default.WindowsPCESNFileLocation);
                if (newFiles.Count() > 0)
                {   // Process the CCRT generated pass confirmation files
                    m_logger.Log("INFO: Found " + Convert.ToString(newFiles.Count()) + " new ESN File(s)");
                    System.Threading.Thread.Sleep(3000);   // Make sure any current files are closed
                    // Process each esn file

                    if (m_engineSerialNumberFileMonitor.UploadToSources(newFiles.ToList()))
                    {
                        m_logger.Log("INFO:  ESN Transmit Successful");

                        //delete oldest files if max exceeded
                        m_engineSerialNumberFileMonitor.ManageRemoteLocationFileCount(BomFileProcessor.Properties.Settings.Default.RealTimePCESNFileLocation, 1000);
                    }
                    else
                    {
                        m_logger.Log("INFO:  ESN Transmit Error, Retransmitting...");
                    }
                }
            }
            catch (System.IO.IOException ex)
            {
                m_logger.Log("ERROR: IOException accessing file or folder - " + ex.Message);
            }
            // Restart the timer to look for more pass confirmation files
            m_esnFileCheckTimer.Start();
        }

        /// <summary>
        /// Check for new STP files to move from the CCRT directory to the Windows directory.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_stpFileCheckTimer_Tick(object sender, EventArgs e)
        {
            m_stpFileCheckTimer.Stop();
            try
            {
                m_logger.Log("Attempting to get VIN Stamper files\n");
                string[] files = Directory.GetFiles("\\\\192.168.1.1\\" + BomFileProcessor.Properties.Settings.Default.CcrtVINStamperFileLocation);

                if (files.Count() > 0)
                {
                    
                    foreach (string s in files)
                    {
                       m_logger.Log("Retrieving VIN Stamper files from: " + s.Substring(s.LastIndexOf('\\') + 1) + "\n");
                       //m_vinStampingFileMonitor.TransferFileFromFtpLocation(s.Substring(s.LastIndexOf('\\') + 1));
                       m_logger.Log("Not actually retreving the files as that should be done in the FTPThreads.");
                    }
                    m_logger.Log("Retrieved VIN Stamper files\n");
                }
            }
            catch
            {
                ;
            }
            m_stpFileCheckTimer.Start();
        }

        /// <summary>
        /// Bring up the form to allow the user to configure data for Pass Confirmation files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void passConfirmationFilesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            PassConfirmationFileSetupForm frm = new PassConfirmationFileSetupForm();
            frm.ShowDialog();
            //if (BomFileProcessor.Properties.Settings.Default.CheckForPassConfirmationFiles)
            //{
            //    m_passFileCheckTimer.Start();
            //}
            //else
            //{
            //    m_passFileCheckTimer.Stop();
            //}
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
        /// Creates a thread to display the current log file in the log viewer form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void viewCurrentLogToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ThreadStart taskDelegate = null;
            taskDelegate = new ThreadStart(ViewCurrentLogFile);
            m_logViewerThread = new Thread(taskDelegate);
            m_logViewerThread.Start();
        }

        /// <summary>
        /// The user will be presented with a list of log files to view.
        /// After selecting a log file, the file will be displayed in the log file viewer window.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void viewPreviousLogToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ViewPreviousLogFiles();
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

        /// <summary>
        /// Setup the Area Axel associations
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void rearAxelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            VehicleOptionForm rearAxelForm = new VehicleOptionForm(m_rearAxel, m_rearAxel.OptionName);
            rearAxelForm.ShowDialog();
        }

        /// <summary>
        /// Setup the Transmission associations
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void transmissionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            VehicleOptionForm transmissionForm = new VehicleOptionForm(m_transmission, m_transmission.OptionName);
            transmissionForm.ShowDialog();
        }

        /// <summary>
        /// Setup the Tire Size associations
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void tireSizeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            VehicleOptionForm tireSizeForm = new VehicleOptionForm(m_tireSize, m_tireSize.OptionName);
            tireSizeForm.ShowDialog();
        }

        /// <summary>
        /// Setup the Speed Meter associations
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void speedMeterToolStripMenuItem_Click(object sender, EventArgs e)
        {
            VehicleOptionForm SpeedMeterForm = new VehicleOptionForm(m_speedMeter, m_speedMeter.OptionName);
            SpeedMeterForm.ShowDialog();
        }

        /// <summary>
        /// Setup the retaining roller relaxation pressure associations.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void retainingRollerPressureToolStripMenuItem_Click(object sender, EventArgs e)
        {
            VehicleOptionForm pressureForm = new VehicleOptionForm(m_retRollPressures, m_retRollPressures.OptionName);
            pressureForm.ShowDialog();
        }

        private void BomProcessorMainForm_Closed(object sender, System.EventArgs e)
        {
            m_processFiles = false;
            //Make sure all windows are closed
            Environment.Exit(0);
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

        /// <summary>
        /// Rear Axel association.
        /// </summary>
        private VehicleOptionCollection m_rearAxel;

        /// <summary>
        /// Transmission association.
        /// </summary>
        private VehicleOptionCollection m_transmission;

        /// <summary>
        /// Tire Size association.
        /// </summary>
        private VehicleOptionCollection m_tireSize;

        /// <summary>
        /// Speed Meter association.
        /// </summary>
        private VehicleOptionCollection m_speedMeter;

        /// <summary>
        /// Retaiing roller relaxation pressure associations.
        /// </summary>
        private VehicleOptionCollection m_retRollPressures;

        private BomFtpFileMonitor m_dvtPassConfirmationMonitor;
        private BomFtpFileMonitor m_ecmPassConfirmationMonitor;
        private BomFtpFileMonitor m_engineSerialNumberFileMonitor;
        private BomFtpFileMonitor m_buildRecordFileMonitor;
        private BomFtpFileMonitor m_vinStampingFileMonitor;

        /// <summary>
        /// ESN Settings.
        /// </summary>
        private ModelCodeOptionsCollection m_modelCodeOptions;

        /// <summary>
        /// Thread for processing the BOM Files.
        /// </summary>
        Thread m_fileProcessorThread;

        /// <summary>
        /// Thread for viewing Log Files.
        /// </summary>
        Thread m_logViewerThread;

        /// <summary>
        /// Flag for processing the BOM Files.
        /// </summary>
        bool m_processFiles;

        private void modeCodeSpcificSttingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ModelCodeSpecificSettings modelSpecific = new ModelCodeSpecificSettings(m_modelCodeOptions);
            modelSpecific.ShowDialog();

        }

        private void BomProcessorMainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_dvtPassConfirmationMonitor.StopFileMonitorThread();
            m_ecmPassConfirmationMonitor.StopFileMonitorThread();
            m_engineSerialNumberFileMonitor.StopFileMonitorThread();
            m_buildRecordFileMonitor.StopFileMonitorThread();
            m_vinStampingFileMonitor.StopFileMonitorThread();
        }

        private void BomProcessorMainForm_Load(object sender, EventArgs e)
        {

        }


    }
    public class BomFtpFileMonitor : FtpFileMonitor
    {
                public BomFtpFileMonitor(string source, string target, string temp, int fileCheckInterval,
            string userLogin, string password, string ftpServerIp,Logger logger, string fileMask = "*")
            : base(source, target, temp, fileCheckInterval,
                    userLogin, password, ftpServerIp, fileMask)/*base(source, target, temp, fileCheckInterval,
                userLogin, password, ftpServerIp,fileMask)*/
        {
            m_logFunction = logger;
        }
                
                public BomFtpFileMonitor(List<string> remoteLocations, string localLocation, List<string> userLogins,
            List<string> passwords, List<string> ftpServerIps, Logger logger, string fileMask = "*")
            : base(remoteLocations, localLocation, userLogins, passwords, ftpServerIps,fileMask)
        {
            m_logFunction = logger;
        }

        public override void Log(string message)
        {
            if (message.Contains("INFO:"))
            {
                m_logFunction.Log(message);
            }
        }

        Logger m_logFunction;
    }
}
