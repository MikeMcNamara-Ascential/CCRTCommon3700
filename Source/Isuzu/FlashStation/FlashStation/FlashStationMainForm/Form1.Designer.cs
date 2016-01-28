namespace FlashStationMainForm
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.m_promptBox1 = new System.Windows.Forms.TextBox();
            this.m_overallProgressBar = new System.Windows.Forms.ProgressBar();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.enterBarcodeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.clearBuildDataToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.abortFlashProcessToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.useForcedCableConnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.forcedCableConnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.configurationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.vehicleCommunicationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.barcodeInputDeviceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.flashECMToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.flashTCMToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.flashDCUToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.flashMimamoriToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_vinLabel = new System.Windows.Forms.Label();
            this.m_vinTitleLabel = new System.Windows.Forms.Label();
            this.m_buildInfoGroupBox = new System.Windows.Forms.GroupBox();
            this.m_buildInfoTextBox = new System.Windows.Forms.RichTextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.m_logMessageTextBox = new System.Windows.Forms.RichTextBox();
            this.m_cableConnectLabel = new System.Windows.Forms.Label();
            this.m_buildDataLabel = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.m_promptBox2 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.m_modelCodeLabel = new System.Windows.Forms.Label();
            this.m_bookCodeLabel = new System.Windows.Forms.Label();
            this.m_modelYearLabel = new System.Windows.Forms.Label();
            this.m_lotNumberLabel = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.m_tcmResultBox = new System.Windows.Forms.Label();
            this.m_ecmResultBox = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.m_cableConnectPB = new System.Windows.Forms.PictureBox();
            this.m_buildDataPB = new System.Windows.Forms.PictureBox();
            this.m_dcuResultBox = new System.Windows.Forms.Label();
            this.m_mimaResultBox = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            this.m_buildInfoGroupBox.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_cableConnectPB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_buildDataPB)).BeginInit();
            this.SuspendLayout();
            // 
            // m_promptBox1
            // 
            this.m_promptBox1.BackColor = System.Drawing.SystemColors.Window;
            this.m_promptBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 48F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_promptBox1.Location = new System.Drawing.Point(17, 347);
            this.m_promptBox1.Margin = new System.Windows.Forms.Padding(4);
            this.m_promptBox1.Name = "m_promptBox1";
            this.m_promptBox1.ReadOnly = true;
            this.m_promptBox1.Size = new System.Drawing.Size(1651, 98);
            this.m_promptBox1.TabIndex = 0;
            this.m_promptBox1.TabStop = false;
            this.m_promptBox1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // m_overallProgressBar
            // 
            this.m_overallProgressBar.Enabled = false;
            this.m_overallProgressBar.Location = new System.Drawing.Point(8, 37);
            this.m_overallProgressBar.Margin = new System.Windows.Forms.Padding(4);
            this.m_overallProgressBar.Name = "m_overallProgressBar";
            this.m_overallProgressBar.Size = new System.Drawing.Size(1644, 48);
            this.m_overallProgressBar.TabIndex = 6;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.toolsToolStripMenuItem,
            this.configurationToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(8, 2, 0, 2);
            this.menuStrip1.Size = new System.Drawing.Size(1685, 28);
            this.menuStrip1.TabIndex = 7;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(44, 24);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(102, 24);
            this.exitToolStripMenuItem.Text = "E&xit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.enterBarcodeToolStripMenuItem,
            this.clearBuildDataToolStripMenuItem,
            this.abortFlashProcessToolStripMenuItem,
            this.useForcedCableConnectToolStripMenuItem,
            this.forcedCableConnectToolStripMenuItem});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(57, 24);
            this.toolsToolStripMenuItem.Text = "&Tools";
            // 
            // enterBarcodeToolStripMenuItem
            // 
            this.enterBarcodeToolStripMenuItem.Name = "enterBarcodeToolStripMenuItem";
            this.enterBarcodeToolStripMenuItem.Size = new System.Drawing.Size(251, 24);
            this.enterBarcodeToolStripMenuItem.Text = "Enter Barcode";
            this.enterBarcodeToolStripMenuItem.Click += new System.EventHandler(this.enterBarcodeToolStripMenuItem_Click);
            // 
            // clearBuildDataToolStripMenuItem
            // 
            this.clearBuildDataToolStripMenuItem.Name = "clearBuildDataToolStripMenuItem";
            this.clearBuildDataToolStripMenuItem.Size = new System.Drawing.Size(251, 24);
            this.clearBuildDataToolStripMenuItem.Text = "Clear Barcode";
            this.clearBuildDataToolStripMenuItem.Click += new System.EventHandler(this.clearBuildDataToolStripMenuItem_Click);
            // 
            // abortFlashProcessToolStripMenuItem
            // 
            this.abortFlashProcessToolStripMenuItem.Name = "abortFlashProcessToolStripMenuItem";
            this.abortFlashProcessToolStripMenuItem.Size = new System.Drawing.Size(251, 24);
            this.abortFlashProcessToolStripMenuItem.Text = "Abort Flash Process";
            this.abortFlashProcessToolStripMenuItem.Click += new System.EventHandler(this.abortFlashProcessToolStripMenuItem_Click);
            // 
            // useForcedCableConnectToolStripMenuItem
            // 
            this.useForcedCableConnectToolStripMenuItem.CheckOnClick = true;
            this.useForcedCableConnectToolStripMenuItem.Name = "useForcedCableConnectToolStripMenuItem";
            this.useForcedCableConnectToolStripMenuItem.Size = new System.Drawing.Size(251, 24);
            this.useForcedCableConnectToolStripMenuItem.Text = "Use Forced Cable Connect";
            this.useForcedCableConnectToolStripMenuItem.Click += new System.EventHandler(this.useForcedCableConnectToolStripMenuItem_Click);
            // 
            // forcedCableConnectToolStripMenuItem
            // 
            this.forcedCableConnectToolStripMenuItem.CheckOnClick = true;
            this.forcedCableConnectToolStripMenuItem.Name = "forcedCableConnectToolStripMenuItem";
            this.forcedCableConnectToolStripMenuItem.Size = new System.Drawing.Size(251, 24);
            this.forcedCableConnectToolStripMenuItem.Text = "Forced Cable Connect";
            this.forcedCableConnectToolStripMenuItem.Click += new System.EventHandler(this.forcedCableConnectToolStripMenuItem_Click);
            // 
            // configurationToolStripMenuItem
            // 
            this.configurationToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.vehicleCommunicationToolStripMenuItem,
            this.barcodeInputDeviceToolStripMenuItem,
            this.flashECMToolStripMenuItem,
            this.flashTCMToolStripMenuItem,
            this.flashDCUToolStripMenuItem,
            this.flashMimamoriToolStripMenuItem});
            this.configurationToolStripMenuItem.Name = "configurationToolStripMenuItem";
            this.configurationToolStripMenuItem.Size = new System.Drawing.Size(112, 24);
            this.configurationToolStripMenuItem.Text = "&Configuration";
            // 
            // vehicleCommunicationToolStripMenuItem
            // 
            this.vehicleCommunicationToolStripMenuItem.Name = "vehicleCommunicationToolStripMenuItem";
            this.vehicleCommunicationToolStripMenuItem.Size = new System.Drawing.Size(235, 24);
            this.vehicleCommunicationToolStripMenuItem.Text = "Vehicle Communication";
            this.vehicleCommunicationToolStripMenuItem.Click += new System.EventHandler(this.vehicleCommunicationToolStripMenuItem_Click);
            // 
            // barcodeInputDeviceToolStripMenuItem
            // 
            this.barcodeInputDeviceToolStripMenuItem.Name = "barcodeInputDeviceToolStripMenuItem";
            this.barcodeInputDeviceToolStripMenuItem.Size = new System.Drawing.Size(235, 24);
            this.barcodeInputDeviceToolStripMenuItem.Text = "Barcode Input Device";
            this.barcodeInputDeviceToolStripMenuItem.Click += new System.EventHandler(this.barcodeInputDeviceToolStripMenuItem_Click);
            // 
            // flashECMToolStripMenuItem
            // 
            this.flashECMToolStripMenuItem.Checked = true;
            this.flashECMToolStripMenuItem.CheckOnClick = true;
            this.flashECMToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.flashECMToolStripMenuItem.Name = "flashECMToolStripMenuItem";
            this.flashECMToolStripMenuItem.Size = new System.Drawing.Size(235, 24);
            this.flashECMToolStripMenuItem.Text = "Flash ECM";
            this.flashECMToolStripMenuItem.Click += new System.EventHandler(this.flashECMToolStripMenuItem_Click);
            // 
            // flashTCMToolStripMenuItem
            // 
            this.flashTCMToolStripMenuItem.Checked = true;
            this.flashTCMToolStripMenuItem.CheckOnClick = true;
            this.flashTCMToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.flashTCMToolStripMenuItem.Name = "flashTCMToolStripMenuItem";
            this.flashTCMToolStripMenuItem.Size = new System.Drawing.Size(235, 24);
            this.flashTCMToolStripMenuItem.Text = "Flash TCM";
            this.flashTCMToolStripMenuItem.Click += new System.EventHandler(this.flashTCMToolStripMenuItem_Click);
            // 
            // flashDCUToolStripMenuItem
            // 
            this.flashDCUToolStripMenuItem.Checked = true;
            this.flashDCUToolStripMenuItem.CheckOnClick = true;
            this.flashDCUToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.flashDCUToolStripMenuItem.Name = "flashDCUToolStripMenuItem";
            this.flashDCUToolStripMenuItem.Size = new System.Drawing.Size(235, 24);
            this.flashDCUToolStripMenuItem.Text = "Flash DCU";
            this.flashDCUToolStripMenuItem.Click += new System.EventHandler(this.flashDCUToolStripMenuItem_Click);
            // 
            // flashMimamoriToolStripMenuItem
            // 
            this.flashMimamoriToolStripMenuItem.Checked = true;
            this.flashMimamoriToolStripMenuItem.CheckOnClick = true;
            this.flashMimamoriToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.flashMimamoriToolStripMenuItem.Name = "flashMimamoriToolStripMenuItem";
            this.flashMimamoriToolStripMenuItem.Size = new System.Drawing.Size(235, 24);
            this.flashMimamoriToolStripMenuItem.Text = "Flash Mimamori";
            this.flashMimamoriToolStripMenuItem.Visible = false;
            this.flashMimamoriToolStripMenuItem.Click += new System.EventHandler(this.flashMimamoriToolStripMenuItem_Click);
            // 
            // m_vinLabel
            // 
            this.m_vinLabel.AutoSize = true;
            this.m_vinLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vinLabel.Location = new System.Drawing.Point(131, 148);
            this.m_vinLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_vinLabel.Name = "m_vinLabel";
            this.m_vinLabel.Size = new System.Drawing.Size(0, 46);
            this.m_vinLabel.TabIndex = 8;
            // 
            // m_vinTitleLabel
            // 
            this.m_vinTitleLabel.AutoSize = true;
            this.m_vinTitleLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vinTitleLabel.Location = new System.Drawing.Point(16, 148);
            this.m_vinTitleLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_vinTitleLabel.Name = "m_vinTitleLabel";
            this.m_vinTitleLabel.Size = new System.Drawing.Size(97, 46);
            this.m_vinTitleLabel.TabIndex = 9;
            this.m_vinTitleLabel.Text = "VIN:";
            // 
            // m_buildInfoGroupBox
            // 
            this.m_buildInfoGroupBox.Controls.Add(this.m_buildInfoTextBox);
            this.m_buildInfoGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_buildInfoGroupBox.ForeColor = System.Drawing.SystemColors.ControlText;
            this.m_buildInfoGroupBox.Location = new System.Drawing.Point(965, 586);
            this.m_buildInfoGroupBox.Margin = new System.Windows.Forms.Padding(4);
            this.m_buildInfoGroupBox.Name = "m_buildInfoGroupBox";
            this.m_buildInfoGroupBox.Padding = new System.Windows.Forms.Padding(4);
            this.m_buildInfoGroupBox.Size = new System.Drawing.Size(704, 351);
            this.m_buildInfoGroupBox.TabIndex = 16;
            this.m_buildInfoGroupBox.TabStop = false;
            this.m_buildInfoGroupBox.Text = "Build Information";
            // 
            // m_buildInfoTextBox
            // 
            this.m_buildInfoTextBox.BackColor = System.Drawing.Color.White;
            this.m_buildInfoTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_buildInfoTextBox.Location = new System.Drawing.Point(9, 37);
            this.m_buildInfoTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.m_buildInfoTextBox.Name = "m_buildInfoTextBox";
            this.m_buildInfoTextBox.ReadOnly = true;
            this.m_buildInfoTextBox.Size = new System.Drawing.Size(685, 306);
            this.m_buildInfoTextBox.TabIndex = 5;
            this.m_buildInfoTextBox.Text = "";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.m_logMessageTextBox);
            this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox1.ForeColor = System.Drawing.SystemColors.ControlText;
            this.groupBox1.Location = new System.Drawing.Point(9, 586);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox1.Size = new System.Drawing.Size(933, 506);
            this.groupBox1.TabIndex = 17;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Log";
            // 
            // m_logMessageTextBox
            // 
            this.m_logMessageTextBox.BackColor = System.Drawing.Color.White;
            this.m_logMessageTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_logMessageTextBox.Location = new System.Drawing.Point(8, 37);
            this.m_logMessageTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.m_logMessageTextBox.Name = "m_logMessageTextBox";
            this.m_logMessageTextBox.ReadOnly = true;
            this.m_logMessageTextBox.Size = new System.Drawing.Size(916, 461);
            this.m_logMessageTextBox.TabIndex = 4;
            this.m_logMessageTextBox.Text = "";
            // 
            // m_cableConnectLabel
            // 
            this.m_cableConnectLabel.AutoSize = true;
            this.m_cableConnectLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_cableConnectLabel.Location = new System.Drawing.Point(1489, 119);
            this.m_cableConnectLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_cableConnectLabel.Name = "m_cableConnectLabel";
            this.m_cableConnectLabel.Size = new System.Drawing.Size(172, 29);
            this.m_cableConnectLabel.TabIndex = 18;
            this.m_cableConnectLabel.Text = "Cable Connect";
            // 
            // m_buildDataLabel
            // 
            this.m_buildDataLabel.AutoSize = true;
            this.m_buildDataLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_buildDataLabel.Location = new System.Drawing.Point(1489, 167);
            this.m_buildDataLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_buildDataLabel.Name = "m_buildDataLabel";
            this.m_buildDataLabel.Size = new System.Drawing.Size(123, 29);
            this.m_buildDataLabel.TabIndex = 19;
            this.m_buildDataLabel.Text = "Build Data";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.m_overallProgressBar);
            this.groupBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox2.ForeColor = System.Drawing.SystemColors.ControlText;
            this.groupBox2.Location = new System.Drawing.Point(9, 1099);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox2.Size = new System.Drawing.Size(1660, 98);
            this.groupBox2.TabIndex = 22;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Flash Progress";
            // 
            // m_promptBox2
            // 
            this.m_promptBox2.BackColor = System.Drawing.SystemColors.Window;
            this.m_promptBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 48F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_promptBox2.Location = new System.Drawing.Point(16, 466);
            this.m_promptBox2.Margin = new System.Windows.Forms.Padding(4);
            this.m_promptBox2.Name = "m_promptBox2";
            this.m_promptBox2.ReadOnly = true;
            this.m_promptBox2.Size = new System.Drawing.Size(1652, 98);
            this.m_promptBox2.TabIndex = 23;
            this.m_promptBox2.TabStop = false;
            this.m_promptBox2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(19, 241);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(184, 36);
            this.label1.TabIndex = 24;
            this.label1.Text = "Model Code:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(35, 284);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(171, 36);
            this.label2.TabIndex = 25;
            this.label2.Text = "Book Code:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(459, 241);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(175, 36);
            this.label3.TabIndex = 26;
            this.label3.Text = "Model Year:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(556, 284);
            this.label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(90, 36);
            this.label4.TabIndex = 27;
            this.label4.Text = "Lot #:";
            // 
            // m_modelCodeLabel
            // 
            this.m_modelCodeLabel.AutoSize = true;
            this.m_modelCodeLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_modelCodeLabel.Location = new System.Drawing.Point(229, 241);
            this.m_modelCodeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_modelCodeLabel.Name = "m_modelCodeLabel";
            this.m_modelCodeLabel.Size = new System.Drawing.Size(0, 36);
            this.m_modelCodeLabel.TabIndex = 28;
            // 
            // m_bookCodeLabel
            // 
            this.m_bookCodeLabel.AutoSize = true;
            this.m_bookCodeLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_bookCodeLabel.Location = new System.Drawing.Point(229, 284);
            this.m_bookCodeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_bookCodeLabel.Name = "m_bookCodeLabel";
            this.m_bookCodeLabel.Size = new System.Drawing.Size(0, 36);
            this.m_bookCodeLabel.TabIndex = 29;
            // 
            // m_modelYearLabel
            // 
            this.m_modelYearLabel.AutoSize = true;
            this.m_modelYearLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_modelYearLabel.Location = new System.Drawing.Point(659, 241);
            this.m_modelYearLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_modelYearLabel.Name = "m_modelYearLabel";
            this.m_modelYearLabel.Size = new System.Drawing.Size(0, 36);
            this.m_modelYearLabel.TabIndex = 30;
            // 
            // m_lotNumberLabel
            // 
            this.m_lotNumberLabel.AutoSize = true;
            this.m_lotNumberLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_lotNumberLabel.Location = new System.Drawing.Point(659, 284);
            this.m_lotNumberLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_lotNumberLabel.Name = "m_lotNumberLabel";
            this.m_lotNumberLabel.Size = new System.Drawing.Size(0, 36);
            this.m_lotNumberLabel.TabIndex = 31;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.m_mimaResultBox);
            this.groupBox3.Controls.Add(this.m_dcuResultBox);
            this.groupBox3.Controls.Add(this.m_tcmResultBox);
            this.groupBox3.Controls.Add(this.m_ecmResultBox);
            this.groupBox3.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox3.ForeColor = System.Drawing.SystemColors.ControlText;
            this.groupBox3.Location = new System.Drawing.Point(965, 944);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox3.Size = new System.Drawing.Size(704, 148);
            this.groupBox3.TabIndex = 17;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Flash Results";
            // 
            // m_tcmResultBox
            // 
            this.m_tcmResultBox.AutoSize = true;
            this.m_tcmResultBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.m_tcmResultBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_tcmResultBox.Location = new System.Drawing.Point(294, 70);
            this.m_tcmResultBox.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_tcmResultBox.Name = "m_tcmResultBox";
            this.m_tcmResultBox.Size = new System.Drawing.Size(281, 48);
            this.m_tcmResultBox.TabIndex = 11;
            this.m_tcmResultBox.Text = " Transmission ";
            // 
            // m_ecmResultBox
            // 
            this.m_ecmResultBox.AutoSize = true;
            this.m_ecmResultBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.m_ecmResultBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_ecmResultBox.Location = new System.Drawing.Point(8, 70);
            this.m_ecmResultBox.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_ecmResultBox.Name = "m_ecmResultBox";
            this.m_ecmResultBox.Size = new System.Drawing.Size(278, 48);
            this.m_ecmResultBox.TabIndex = 10;
            this.m_ecmResultBox.Text = "      Engine      ";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(0, 30);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(4);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(1669, 86);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 1;
            this.pictureBox1.TabStop = false;
            // 
            // m_cableConnectPB
            // 
            this.m_cableConnectPB.Image = ((System.Drawing.Image)(resources.GetObject("m_cableConnectPB.Image")));
            this.m_cableConnectPB.Location = new System.Drawing.Point(1447, 119);
            this.m_cableConnectPB.Margin = new System.Windows.Forms.Padding(4);
            this.m_cableConnectPB.Name = "m_cableConnectPB";
            this.m_cableConnectPB.Size = new System.Drawing.Size(35, 30);
            this.m_cableConnectPB.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.m_cableConnectPB.TabIndex = 20;
            this.m_cableConnectPB.TabStop = false;
            // 
            // m_buildDataPB
            // 
            this.m_buildDataPB.Image = ((System.Drawing.Image)(resources.GetObject("m_buildDataPB.Image")));
            this.m_buildDataPB.Location = new System.Drawing.Point(1447, 165);
            this.m_buildDataPB.Margin = new System.Windows.Forms.Padding(4);
            this.m_buildDataPB.Name = "m_buildDataPB";
            this.m_buildDataPB.Size = new System.Drawing.Size(35, 33);
            this.m_buildDataPB.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.m_buildDataPB.TabIndex = 21;
            this.m_buildDataPB.TabStop = false;
            // 
            // m_dcuResultBox
            // 
            this.m_dcuResultBox.AutoSize = true;
            this.m_dcuResultBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.m_dcuResultBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_dcuResultBox.Location = new System.Drawing.Point(583, 70);
            this.m_dcuResultBox.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_dcuResultBox.Name = "m_dcuResultBox";
            this.m_dcuResultBox.Size = new System.Drawing.Size(109, 48);
            this.m_dcuResultBox.TabIndex = 12;
            this.m_dcuResultBox.Text = "DCU";
            this.m_dcuResultBox.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // m_mimaResultBox
            // 
            this.m_mimaResultBox.AutoSize = true;
            this.m_mimaResultBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.m_mimaResultBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_mimaResultBox.Location = new System.Drawing.Point(207, 12);
            this.m_mimaResultBox.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_mimaResultBox.Name = "m_mimaResultBox";
            this.m_mimaResultBox.Size = new System.Drawing.Size(197, 48);
            this.m_mimaResultBox.TabIndex = 13;
            this.m_mimaResultBox.Text = "Mimamori";
            this.m_mimaResultBox.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.m_mimaResultBox.Visible = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.ClientSize = new System.Drawing.Size(1685, 1057);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.m_lotNumberLabel);
            this.Controls.Add(this.m_modelYearLabel);
            this.Controls.Add(this.m_bookCodeLabel);
            this.Controls.Add(this.m_modelCodeLabel);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_promptBox2);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.m_buildDataPB);
            this.Controls.Add(this.m_cableConnectPB);
            this.Controls.Add(this.m_buildDataLabel);
            this.Controls.Add(this.m_cableConnectLabel);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.m_buildInfoGroupBox);
            this.Controls.Add(this.m_vinTitleLabel);
            this.Controls.Add(this.m_vinLabel);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.m_promptBox1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Form1";
            this.Text = "Burke E. Porter Machinery Static Station";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.m_buildInfoGroupBox.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_cableConnectPB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_buildDataPB)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox m_promptBox1;
        private System.Windows.Forms.ProgressBar m_overallProgressBar;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.Label m_vinLabel;
        private System.Windows.Forms.Label m_vinTitleLabel;
        private System.Windows.Forms.GroupBox m_buildInfoGroupBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label m_cableConnectLabel;
        private System.Windows.Forms.Label m_buildDataLabel;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem enterBarcodeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem configurationToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem barcodeInputDeviceToolStripMenuItem;
        private System.Windows.Forms.RichTextBox m_logMessageTextBox;
        private System.Windows.Forms.ToolStripMenuItem clearBuildDataToolStripMenuItem;
        private System.Windows.Forms.RichTextBox m_buildInfoTextBox;
        private System.Windows.Forms.ToolStripMenuItem abortFlashProcessToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem useForcedCableConnectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem forcedCableConnectToolStripMenuItem;
        private System.Windows.Forms.TextBox m_promptBox2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label m_modelCodeLabel;
        private System.Windows.Forms.Label m_bookCodeLabel;
        private System.Windows.Forms.Label m_modelYearLabel;
        private System.Windows.Forms.Label m_lotNumberLabel;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label m_tcmResultBox;
        private System.Windows.Forms.Label m_ecmResultBox;
        private System.Windows.Forms.ToolStripMenuItem flashECMToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem flashTCMToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem vehicleCommunicationToolStripMenuItem;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.PictureBox m_cableConnectPB;
        private System.Windows.Forms.PictureBox m_buildDataPB;
        private System.Windows.Forms.ToolStripMenuItem flashDCUToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem flashMimamoriToolStripMenuItem;
        private System.Windows.Forms.Label m_mimaResultBox;
        private System.Windows.Forms.Label m_dcuResultBox;
    }
}

