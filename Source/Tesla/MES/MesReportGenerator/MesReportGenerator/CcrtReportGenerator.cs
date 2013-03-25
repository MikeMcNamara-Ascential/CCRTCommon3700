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
using System.Xml.Xsl;
using System.Xml.XPath;
using System.Collections;
using System.Text.RegularExpressions;

namespace MesReportGenerator
{
    public partial class CcrtReportGenerator : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Report Generator Application
        /// </summary>
        public CcrtReportGenerator()
        {
            InitializeComponent();
            m_resultDirectory.Text = MesReportGenerator.Properties.Settings.Default.SourceDirectory;
            m_reportDirectory.Text = MesReportGenerator.Properties.Settings.Default.OutputDirectory;
            m_archiveDirectory.Text = MesReportGenerator.Properties.Settings.Default.ArchiveDirectory;
            m_schema.Text = MesReportGenerator.Properties.Settings.Default.XsltFile;
            //if (MesReportGenerator.Properties.Settings.Default.usedResults != null)
            //{
            //    m_usedTestResultFiles = MesReportGenerator.Properties.Settings.Default.usedResults;
            //}
            String title;
            if (!System.Deployment.Application.ApplicationDeployment.IsNetworkDeployed)
            {
                title = String.Format("Report Generator - Version {0}", ProductVersion);
            }
            else
            {
                title = String.Format("Report Generator - Version {0}",
                                      System.Deployment.Application.ApplicationDeployment.CurrentDeployment.CurrentVersion.ToString());
            }
            Text = title;
            VerifySystemSetup();


        }

        /// <summary>
        /// Display the provided status message in the message box.
        /// </summary>
        /// <param name="message">Message to display.</param>
        private void DisplayStatusMessage(String message)
        {
            if (m_msgTextBox.InvokeRequired)
            {
                LogMessageCallback callback = new LogMessageCallback(DisplayStatusMessage);
                m_msgTextBox.Invoke(callback, new object[] { message });
            }
            else
            {
                String msg = DateTime.Now.ToString("MMM dd, yyyy HH:mm:ss") + "\t" + message;
                m_msgTextBox.AppendText(msg + Environment.NewLine);
            }
        }

        /// <summary>
        /// Convert to MES Report
        /// </summary>
        private String ConvertTestResultFile(String file, XslCompiledTransform xForm)
        {
            String reportFileName = null;
            string newFileName = null;
            //Get the result of OverallTest
            try
            {
                XmlDocument xDoc = new XmlDocument();
                xDoc.Load(file);
                XmlNode xNode = xDoc.SelectSingleNode("//BepVehicleTestResult/TestResult/OverallTest");
                string result = xNode.Attributes["Result"].Value;
                if (result == "Pass")
                {
                    newFileName = "P_" + Path.GetFileName(file);
                    reportFileName = Path.Combine(MesReportGenerator.Properties.Settings.Default.OutputDirectory,
                                                     newFileName);
                }
                else if (result == "Abort")
                {
                    newFileName = "A_" + Path.GetFileName(file);
                    reportFileName = Path.Combine(MesReportGenerator.Properties.Settings.Default.OutputDirectory,
                                                     newFileName);
                }
                else
                {
                    newFileName = "F_" + Path.GetFileName(file);
                    reportFileName = Path.Combine(MesReportGenerator.Properties.Settings.Default.OutputDirectory,
                                                     newFileName);
                }
            }
            catch (NullReferenceException)
            {
                DisplayStatusMessage("Attempt to process Test Result file: " + file + " not successful. Check file format.");
                //AddToUsedTestResults(Path.GetFileName(file));
                return "NullExeption";
            }
            catch (Exception)
            {
                DisplayStatusMessage("Attempt to process Test Result file: " + file + " not successful. Reason unknown");
            }
            if (reportFileName != null || newFileName != null)
            {
                xForm.Transform(file, reportFileName);

                // PRETTY PRINT XML??
                //XmlDocument xDoc = new XmlDocument();
                //xDoc.Load(reportFileName);
                //xDoc.Save(Console.Out);
                //Console.WriteLine(Beautify(xDoc));


                DisplayStatusMessage("Processing result file " + file + " to " + reportFileName);

                // Remove the temp file and the result file
                //File.Delete(file);

            }
            //m_usedTestResultFiles.Add(Path.GetFileName(file));
            return Path.GetFileName(file);
        }

        /// <summary>
        /// Add file to usedTestResults
        /// </summary>
        //private void AddToUsedTestResults(String file)
        //{
        //    if (m_usedTestResultFiles != null)
        //        if (!m_usedTestResultFiles.Contains(file))
        //            m_usedTestResultFiles.Add(file);
        //}

        /// <summary>
        /// Move source test result file to the archive directory
        /// </summary>
        /// <param name="filePath">Path of the file to archive</param>
        private void ArchiveTestResult(String filePath)
        {
            if((filePath != null) && File.Exists(filePath))
            {
                String fileName = Path.GetFileName(filePath);
                String destinationPath = Path.Combine(MesReportGenerator.Properties.Settings.Default.ArchiveDirectory, fileName);
                if(File.Exists(destinationPath))
                {
                    DisplayStatusMessage("Removing previously archived file with same name: " + fileName);
                    File.Delete(destinationPath);
                }
                DisplayStatusMessage("Moving raw results file " + fileName + " to archive folder");
                File.Move(filePath,destinationPath);
            }
        }

        /// <summary>
        /// Generate the pdf report.
        /// </summary>
        private void GenerateReport()
        {   // Make sure we are all setup
            m_reportTimer.Stop();  // Stop the timer so we do not get multiple entries
            if (VerifySystemSetup())
            {
                //On initial check fill array with existing Test Result file names - mark them as converted 
                //(without actually converting them)

                //On subsequent checks, compare files in directory to previously converted files
                //if (m_usedTestResultFiles == null)
                //{
                //    m_usedTestResultFiles = new ArrayList();
                //    foreach (String file in Directory.GetFiles(MesReportGenerator.Properties.Settings.Default.SourceDirectory))
                //    {
                //        if (Regex.IsMatch(Path.GetFileName(file), @"[A-Za-z0-9]+\.[0-9]{4}"))
                //        {
                //            m_usedTestResultFiles.Add(Path.GetFileName(file));
                //        }
                //    }
                //    MesReportGenerator.Properties.Settings.Default.usedResults = m_usedTestResultFiles;
                //    MesReportGenerator.Properties.Settings.Default.Save();
                //}
                //else
                //{
                    // Load the XSLT file
                    XslCompiledTransform xForm = new XslCompiledTransform();
                    xForm.Load(MesReportGenerator.Properties.Settings.Default.XsltFile);
                    // Get the list of result files to be processed
                    foreach (String file in Directory.GetFiles(MesReportGenerator.Properties.Settings.Default.SourceDirectory))
                    {

                        if (Regex.IsMatch(Path.GetFileName(file), @"[A-Za-z0-9]+\.[0-9]{4}"))
                        {
                            //if (!m_usedTestResultFiles.Contains(Path.GetFileName(file)))
                            //{
                            //    String result = ConvertTestResultFile(file, xForm);
                            //        AddToUsedTestResults(result);
                            //}
                            String result = ConvertTestResultFile(file, xForm);
                        }
                        // Move the test result file to the Archive folder once the conversion has taken place
                        ArchiveTestResult(file);
                    }
                    
                    //MesReportGenerator.Properties.Settings.Default.usedResults = m_usedTestResultFiles;
                    //MesReportGenerator.Properties.Settings.Default.Save();
                //}
            }
            else
            {
                DisplayStatusMessage("System setup not complete, not generating results");
            }
            m_reportTimer.Enabled = true;
        }

        // <summary>
        /// Verify the system has been setup properly.
        /// If any of the files or folders are missing, the user will be provided the opportunity
        /// to specify the location of the files.
        /// </summary>
        private Boolean VerifySystemSetup()
        {   // Verify the XSLT file exists.
            Boolean setupComplete = true;
            Int16 attempts = 3;
            while (!File.Exists(MesReportGenerator.Properties.Settings.Default.XsltFile) && (attempts-- > 0))
            {
                SelectXsltFile();
            }
            if (File.Exists(MesReportGenerator.Properties.Settings.Default.XsltFile))
            {
                m_schema.ForeColor = Color.Green;
            }
            else
            {
                m_schema.ForeColor = Color.Red;
                setupComplete = false;
            }
            // Verify the Result folder exists
            attempts = 3;
            while (!Directory.Exists(MesReportGenerator.Properties.Settings.Default.SourceDirectory) && (attempts-- > 0))
            {
                SelectResultFolder();
            }
            if (Directory.Exists(MesReportGenerator.Properties.Settings.Default.SourceDirectory))
            {
                m_resultDirectory.ForeColor = Color.Green;
            }
            else
            {
                m_resultDirectory.ForeColor = Color.Red;
                setupComplete = false;
            }
            // Verify the report folder exists
            attempts = 3;
            while (!Directory.Exists(MesReportGenerator.Properties.Settings.Default.OutputDirectory) && (attempts-- > 0))
            {
                SelectReportFolder();
            }
            if (Directory.Exists(MesReportGenerator.Properties.Settings.Default.OutputDirectory))
            {
                m_reportDirectory.ForeColor = Color.Green;
            }
            else
            {
                m_reportDirectory.ForeColor = Color.Red;
                setupComplete = false;
            }
            // Verify the archive folder exists
            attempts = 3;
            while (!Directory.Exists(MesReportGenerator.Properties.Settings.Default.ArchiveDirectory) && (attempts-- > 0))
            {
                SelectArchiveFolder();
            }
            if (Directory.Exists(MesReportGenerator.Properties.Settings.Default.ArchiveDirectory))
            {
                m_archiveDirectory.ForeColor = Color.Green;
            }
            else
            {
                m_archiveDirectory.ForeColor = Color.Red;
                setupComplete = false;
            }
            return setupComplete;
        }
        ///// <summary>
        ///// Obsolete 
        ///// </summary>
        //private void ConvertXml()
        //{
        //    XslTransform xForm = new XslTransform();
        //    xForm.Load(@"C:\Users\lmorgan\Desktop\testreults\ReportSchem.xsl");
        //    xForm.Transform(@"C:\Users\lmorgan\Desktop\testreults\SampleResult", @"C:\Users\lmorgan\Desktop\testreults\newEx.xml");
        //}

        /// <summary>
        /// Allow the user to select the folder to be used for storing the generated reports.
        /// </summary>
        private void SelectReportFolder()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select Test Report Output Folder";
            
            if (m_reportDirectory.Text != null && (Directory.Exists(m_reportDirectory.Text)))
            {
                dlg.SelectedPath = m_reportDirectory.Text;
            }
            dlg.RootFolder = Environment.SpecialFolder.MyComputer;
            dlg.ShowNewFolderButton = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MesReportGenerator.Properties.Settings.Default.OutputDirectory = dlg.SelectedPath;
                MesReportGenerator.Properties.Settings.Default.Save();
                m_reportDirectory.Text = MesReportGenerator.Properties.Settings.Default.OutputDirectory;
            }
        }

        /// <summary>
        /// Allow the user to select the folder to be used for storing the archived raw results.
        /// </summary>
        private void SelectArchiveFolder()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select Test Result Archive Folder";
            
            if (m_archiveDirectory.Text != null && (Directory.Exists(m_archiveDirectory.Text)))
            {
                dlg.SelectedPath = m_archiveDirectory.Text;
            }
            dlg.RootFolder = Environment.SpecialFolder.MyComputer;
            dlg.ShowNewFolderButton = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MesReportGenerator.Properties.Settings.Default.ArchiveDirectory = dlg.SelectedPath;
                MesReportGenerator.Properties.Settings.Default.Save();
                m_archiveDirectory.Text = MesReportGenerator.Properties.Settings.Default.ArchiveDirectory;
            }
        }

        /// <summary>
        /// Allow the user to select the folder to be used for storing the generated reports.
        /// </summary>
        private void SelectResultFile()
        {
            OpenFileDialog dlg = new OpenFileDialog();
            //dlg.DefaultExt = "xml";
            //dlg.Filter = "XML Files (*.xml)|*.xml|" + "All Files (*.*)|*.*";
            dlg.InitialDirectory = Convert.ToString( Environment.SpecialFolder.MyComputer);
            //dlg.InitialDirectory = @"C:\Burke_PorterCCRT\resources\local\report\";
            if (m_resultDirectory.Text != null && (Directory.Exists(m_resultDirectory.Text)))
            {
                dlg.InitialDirectory = m_resultDirectory.Text;
            }
            dlg.Multiselect = false;
            dlg.Title = "Select Result File";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                XslCompiledTransform xForm = new XslCompiledTransform();
                xForm.Load(MesReportGenerator.Properties.Settings.Default.XsltFile);
                String file = dlg.FileName;

                String result = ConvertTestResultFile(file, xForm);
                if (result == "NullExeption")
                {
                    MessageBox.Show("Test Result file not valid!" +
                                                  Environment.NewLine + "Check format.",
                                                  "MES Report Generator", MessageBoxButtons.OK,
                                                  MessageBoxIcon.Error);
                }
            }
        }

        private string Beautify(XmlDocument doc)
        {
            StringBuilder sb = new StringBuilder();
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.IndentChars = "\t";
            settings.NewLineChars = "\r\n";
            settings.NewLineHandling = NewLineHandling.Replace;
            using (XmlWriter writer = XmlWriter.Create(sb, settings)) doc.Save(writer);
            return sb.ToString();
        }

        /// <summary>
        /// Allow the user to select the folder to grab raw result files from.
        /// </summary>
        private void SelectResultFolder()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select Test Result Source Folder";
            dlg.RootFolder = Environment.SpecialFolder.MyComputer;
            if (m_resultDirectory.Text != null && (Directory.Exists(m_resultDirectory.Text)))
            {
                dlg.SelectedPath = m_resultDirectory.Text;
            }
            dlg.ShowNewFolderButton = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MesReportGenerator.Properties.Settings.Default.SourceDirectory = dlg.SelectedPath;
                MesReportGenerator.Properties.Settings.Default.Save();
                m_resultDirectory.Text = MesReportGenerator.Properties.Settings.Default.SourceDirectory;
            }
        }

        /// <summary>
        /// Allow the user to select the XSLT file to use for generating the reports.
        /// </summary>
        private void SelectXsltFile()
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.DefaultExt = "xsl";
            dlg.Filter = "XSL Files (*.xsl;*.xslt)|*.xsl;*.xslt|" + "All Files (*.*)|*.*";
            dlg.InitialDirectory = @"C:\Burke_PorterCCRT\resources\local\report\";
            if (m_schema.Text != null && (Directory.Exists(m_schema.Text)))
            {
                dlg.InitialDirectory = m_schema.Text;
            }
            dlg.Multiselect = false;
            dlg.Title = "Select Report Format File";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MesReportGenerator.Properties.Settings.Default.XsltFile = dlg.FileName;
                MesReportGenerator.Properties.Settings.Default.Save();
                m_schema.Text = MesReportGenerator.Properties.Settings.Default.XsltFile;
            }
        }

        /// <summary>
        /// Verify the user really wants to exit the program.
        /// </summary>
        /// <returns>Flag indicating program should indeed exit.</returns>
        private Boolean VerifyProgramExit()
        {
            Boolean shouldExit = false;
            DialogResult result = MessageBox.Show("Are you sure you want to exit the Report Generator?" +
                                                  Environment.NewLine + "Reports will no longer saved to network!",
                                                  "Confirm Application Exit", MessageBoxButtons.OKCancel,
                                                  MessageBoxIcon.Question);
            shouldExit = (result == DialogResult.OK);
            return shouldExit;
        }

        //private ArrayList m_usedTestResultFiles;

        //-----------------------------------------------------------------------------------------
        // Callbacks, Delegates and Events
        //-----------------------------------------------------------------------------------------


        //m_checkForResultsButton
        private void button1_Click(object sender, EventArgs e)
        {
            //ConvertXml();
            MesReportGenerator.Properties.Settings.Default.SourceDirectory = m_resultDirectory.Text;
            MesReportGenerator.Properties.Settings.Default.OutputDirectory = m_reportDirectory.Text;
            MesReportGenerator.Properties.Settings.Default.ArchiveDirectory = m_archiveDirectory.Text;
            MesReportGenerator.Properties.Settings.Default.XsltFile = m_schema.Text;
            MesReportGenerator.Properties.Settings.Default.Save();

            if (!m_resultDirectory.Items.Contains(m_resultDirectory.Text))
            {
                m_resultDirectory.Items.Add(m_resultDirectory.Text);
            }

            if (!m_schema.Items.Contains(m_schema.Text))
            {
                m_schema.Items.Add(m_schema.Text);
            }

            if (!m_reportDirectory.Items.Contains(m_reportDirectory.Text))
            {
                m_reportDirectory.Items.Add(m_reportDirectory.Text);
            }

            if (!m_archiveDirectory.Items.Contains(m_archiveDirectory.Text))
            {
                m_archiveDirectory.Items.Add(m_archiveDirectory.Text);
            }

            // Stop the timer so it does not fire while we are checking
            m_reportTimer.Stop();
            GenerateReport();
            MessageBox.Show("Auto MES Reoport Generating Enabled!" +
                                                  Environment.NewLine + "MES Reports will be output to MES Report Destination.",
                                                  "MES Report Generator", MessageBoxButtons.OK,
                                                  MessageBoxIcon.None);
            // Restart the timer so it can take over functions
            m_reportTimer.Start();
        }

        // <summary>
        /// Launch a new thread to process any new test result files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_reportTimer_Tick_1(object sender, EventArgs e)
        {
#if false
            Thread reportThread = new Thread(new ThreadStart(GenerateReport));
            reportThread.Start();
#else
            GenerateReport();
#endif
        }

        delegate void LogMessageCallback(String message);

        

        private void m_resultDirectory_SelectedIndexChanged(object sender, EventArgs e)
        {
            MesReportGenerator.Properties.Settings.Default.SourceDirectory = m_resultDirectory.SelectedText;
            MesReportGenerator.Properties.Settings.Default.Save();
        }

        private void m_schema_SelectedIndexChanged(object sender, EventArgs e)
        {
            MesReportGenerator.Properties.Settings.Default.XsltFile = m_schema.SelectedText;
            MesReportGenerator.Properties.Settings.Default.Save();
        }

        private void m_reportDirectory_SelectedIndexChanged(object sender, EventArgs e)
        {
            MesReportGenerator.Properties.Settings.Default.OutputDirectory = m_reportDirectory.SelectedText;
            MesReportGenerator.Properties.Settings.Default.Save();
        }

        private void m_archiveDirectory_SelectedIndexChanged(object sender, EventArgs e)
        {
            MesReportGenerator.Properties.Settings.Default.ArchiveDirectory = m_archiveDirectory.SelectedText;
            MesReportGenerator.Properties.Settings.Default.Save();
        }

        private void m_browseResultDirectory_Click(object sender, EventArgs e)
        {
            SelectResultFolder();
        }

        private void m_browseReportDirectory_Click(object sender, EventArgs e)
        {
            SelectReportFolder();
        }

        private void m_browseArchiveDirectory_Click(object sender, EventArgs e)
        {
            SelectArchiveFolder();
        }

        private void m_browseSchemaDirectory_Click(object sender, EventArgs e)
        {
            SelectXsltFile();
        }

        private void m_resultDirectory_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13)
            {
                if (!m_resultDirectory.Items.Contains(m_resultDirectory.Text))
                {
                    m_resultDirectory.Items.Add(m_resultDirectory.Text);
                }
            }
        }

        private void m_schema_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13)
            {
                if (!m_schema.Items.Contains(m_schema.Text))
                {
                    m_schema.Items.Add(m_schema.Text);
                }
            }
        }

        private void m_reportDirectory_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13)
            {
                if (!m_reportDirectory.Items.Contains(m_reportDirectory.Text))
                {
                    m_reportDirectory.Items.Add(m_reportDirectory.Text);
                }
            }
        }

        private void CcrtReportGenerator_FormClosing(object sender, FormClosingEventArgs e)
        {
            //ArrayList arraylist = new ArrayList(this.m_resultDirectory.Items);
            //Settings.Default.cboCollection = arraylist;
            //Settings.Default.Save();

            if (VerifyProgramExit())
            {
                Environment.Exit(0);
            }
            else
            {
                DisplayStatusMessage("Exit canceled by the user.");
                e.Cancel = true;
            }
        }

        private void m_selectResultFile_Click(object sender, EventArgs e)
        {
            SelectResultFile();
        }

    }
}
