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
using System.Collections;

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
            m_resultDirectory.Text = MesReportGenerator.Properties.Settings.Default.ResultDirectory;
            m_reportDirectory.Text = MesReportGenerator.Properties.Settings.Default.ReportDirectory;
            m_schema.Text = MesReportGenerator.Properties.Settings.Default.XsltFile;
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
        /// Generate the pdf report.
        /// </summary>
        private void GenerateReport()
        {   // Make sure we are all setup
            m_reportTimer.Stop();  // Stop the timer so we do not get multiple entries
            if (VerifySystemSetup())
            {
                // Load the XSLT file
                XslCompiledTransform xForm = new XslCompiledTransform();
                xForm.Load(MesReportGenerator.Properties.Settings.Default.XsltFile);
                // Get the list of result files to be processed
                foreach (String file in Directory.GetFiles(MesReportGenerator.Properties.Settings.Default.ResultDirectory))
                {
                    String reportFileName = Path.Combine(MesReportGenerator.Properties.Settings.Default.ReportDirectory,
                                                         Path.GetFileName(file));
                    xForm.Transform(file, reportFileName);
                    DisplayStatusMessage("Prcessing result file " + file + " to " + reportFileName);

                    // Remove the temp file and the result file
                    File.Delete(file);
                }
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
            while (!Directory.Exists(MesReportGenerator.Properties.Settings.Default.ResultDirectory) && (attempts-- > 0))
            {
                SelectResultFolder();
            }
            if (Directory.Exists(MesReportGenerator.Properties.Settings.Default.ResultDirectory))
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
            while (!Directory.Exists(MesReportGenerator.Properties.Settings.Default.ReportDirectory) && (attempts-- > 0))
            {
                SelectReportFolder();
            }
            if (Directory.Exists(MesReportGenerator.Properties.Settings.Default.ReportDirectory))
            {
                m_reportDirectory.ForeColor = Color.Green;
            }
            else
            {
                m_reportDirectory.ForeColor = Color.Red;
                setupComplete = false;
            }
            return setupComplete;
        }
        /// <summary>
        /// Obsolete 
        /// </summary>
        private void ConvertXml()
        {
            XslTransform xForm = new XslTransform();
            xForm.Load(@"C:\Users\lmorgan\Desktop\testreults\ReportSchem.xsl");
            xForm.Transform(@"C:\Users\lmorgan\Desktop\testreults\SampleResult", @"C:\Users\lmorgan\Desktop\testreults\newEx.xml");
        }

        /// <summary>
        /// Allow the user to select the folder to be used for storing the generated reports.
        /// </summary>
        private void SelectReportFolder()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select Test Report Folder";
            if (m_reportDirectory.Text != null && (Directory.Exists(m_reportDirectory.Text)))
            {
                dlg.SelectedPath = m_reportDirectory.Text;
            }
            dlg.RootFolder = Environment.SpecialFolder.MyComputer;
            dlg.ShowNewFolderButton = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MesReportGenerator.Properties.Settings.Default.ReportDirectory = dlg.SelectedPath;
                MesReportGenerator.Properties.Settings.Default.Save();
                m_reportDirectory.Text = MesReportGenerator.Properties.Settings.Default.ReportDirectory;
            }
        }

        /// <summary>
        /// Allow the user to select the folder to grab raw result files from.
        /// </summary>
        private void SelectResultFolder()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select Test Result Folder";
            dlg.RootFolder = Environment.SpecialFolder.MyComputer;
            if (m_resultDirectory.Text != null && (Directory.Exists(m_resultDirectory.Text)))
            {
                dlg.SelectedPath = m_resultDirectory.Text;
            }
            dlg.ShowNewFolderButton = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MesReportGenerator.Properties.Settings.Default.ResultDirectory = dlg.SelectedPath;
                MesReportGenerator.Properties.Settings.Default.Save();
                m_resultDirectory.Text = MesReportGenerator.Properties.Settings.Default.ResultDirectory;
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

        //-----------------------------------------------------------------------------------------
        // Callbacks, Delegates and Events
        //-----------------------------------------------------------------------------------------


        //m_checkForResultsButton
        private void button1_Click(object sender, EventArgs e)
        {
            //ConvertXml();
            MesReportGenerator.Properties.Settings.Default.ResultDirectory = m_resultDirectory.Text;
            MesReportGenerator.Properties.Settings.Default.ReportDirectory = m_reportDirectory.Text;
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

            // Stop the timer so it does not fire while we are checking
            m_reportTimer.Stop();
            GenerateReport();
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
            MesReportGenerator.Properties.Settings.Default.ResultDirectory = m_resultDirectory.SelectedText;
            MesReportGenerator.Properties.Settings.Default.Save();
        }

        private void m_schema_SelectedIndexChanged(object sender, EventArgs e)
        {
            MesReportGenerator.Properties.Settings.Default.XsltFile = m_schema.SelectedText;
            MesReportGenerator.Properties.Settings.Default.Save();
        }

        private void m_reportDirectory_SelectedIndexChanged(object sender, EventArgs e)
        {
            MesReportGenerator.Properties.Settings.Default.ResultDirectory = m_reportDirectory.SelectedText;
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
            }
        }
    }
}
