namespace BomFileProcessor
{
    partial class BomProcessorMainForm
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
            this.components = new System.ComponentModel.Container();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.existingParameterSetsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fileLocationsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.selectBOMFolderToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.selectBOMArchiveFolderToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.selectBOMResultFolderToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.uPCFNASetupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.bOMFileSetupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.axleTypesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.wheelbaseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.brakeForcesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.passConfirmationFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.modeCodeSpcificSttingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.retainingRollerPressureToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.rearAxelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.transmissionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tireSizeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.speedMeterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.logsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewCurrentLogToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewPreviousLogToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exportLogFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_logMessageLabel = new System.Windows.Forms.Label();
            this.m_logMessageTextBox = new System.Windows.Forms.RichTextBox();
            this.m_passFileCheckTimer = new System.Windows.Forms.Timer(this.components);
            this.m_esnFileCheckTimer = new System.Windows.Forms.Timer(this.components);
            this.m_tsnFileCheckTimer = new System.Windows.Forms.Timer(this.components);
            this.m_stpFileCheckTimer = new System.Windows.Forms.Timer(this.components);
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.setupToolStripMenuItem,
            this.logsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(641, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(92, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.existingParameterSetsToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(39, 20);
            this.editToolStripMenuItem.Text = "Edit";
            // 
            // existingParameterSetsToolStripMenuItem
            // 
            this.existingParameterSetsToolStripMenuItem.Name = "existingParameterSetsToolStripMenuItem";
            this.existingParameterSetsToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.existingParameterSetsToolStripMenuItem.Text = "Existing Parameter Sets";
            this.existingParameterSetsToolStripMenuItem.Click += new System.EventHandler(this.existingParameterSetsToolStripMenuItem_Click);
            // 
            // setupToolStripMenuItem
            // 
            this.setupToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileLocationsToolStripMenuItem,
            this.uPCFNASetupToolStripMenuItem,
            this.bOMFileSetupToolStripMenuItem,
            this.axleTypesToolStripMenuItem,
            this.wheelbaseToolStripMenuItem,
            this.brakeForcesToolStripMenuItem,
            this.passConfirmationFilesToolStripMenuItem,
            this.modeCodeSpcificSttingsToolStripMenuItem,
            this.retainingRollerPressureToolStripMenuItem,
            this.rearAxelToolStripMenuItem,
            this.transmissionToolStripMenuItem,
            this.tireSizeToolStripMenuItem,
            this.speedMeterToolStripMenuItem});
            this.setupToolStripMenuItem.Name = "setupToolStripMenuItem";
            this.setupToolStripMenuItem.Size = new System.Drawing.Size(49, 20);
            this.setupToolStripMenuItem.Text = "Setup";
            // 
            // fileLocationsToolStripMenuItem
            // 
            this.fileLocationsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectBOMFolderToolStripMenuItem1,
            this.selectBOMArchiveFolderToolStripMenuItem,
            this.selectBOMResultFolderToolStripMenuItem1});
            this.fileLocationsToolStripMenuItem.Name = "fileLocationsToolStripMenuItem";
            this.fileLocationsToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.fileLocationsToolStripMenuItem.Text = "File Locations";
            // 
            // selectBOMFolderToolStripMenuItem1
            // 
            this.selectBOMFolderToolStripMenuItem1.Name = "selectBOMFolderToolStripMenuItem1";
            this.selectBOMFolderToolStripMenuItem1.Size = new System.Drawing.Size(214, 22);
            this.selectBOMFolderToolStripMenuItem1.Text = "Select BOM Folder";
            this.selectBOMFolderToolStripMenuItem1.Click += new System.EventHandler(this.selectBOMFolderToolStripMenuItem_Click);
            // 
            // selectBOMArchiveFolderToolStripMenuItem
            // 
            this.selectBOMArchiveFolderToolStripMenuItem.Name = "selectBOMArchiveFolderToolStripMenuItem";
            this.selectBOMArchiveFolderToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
            this.selectBOMArchiveFolderToolStripMenuItem.Text = "Select BOM Archive Folder";
            this.selectBOMArchiveFolderToolStripMenuItem.Click += new System.EventHandler(this.selectBOMArchiveFolderToolStripMenuItem_Click);
            // 
            // selectBOMResultFolderToolStripMenuItem1
            // 
            this.selectBOMResultFolderToolStripMenuItem1.Name = "selectBOMResultFolderToolStripMenuItem1";
            this.selectBOMResultFolderToolStripMenuItem1.Size = new System.Drawing.Size(214, 22);
            this.selectBOMResultFolderToolStripMenuItem1.Text = "Select BOM Result Folder";
            this.selectBOMResultFolderToolStripMenuItem1.Click += new System.EventHandler(this.selectBOMResultFolderToolStripMenuItem_Click);
            // 
            // uPCFNASetupToolStripMenuItem
            // 
            this.uPCFNASetupToolStripMenuItem.Name = "uPCFNASetupToolStripMenuItem";
            this.uPCFNASetupToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.uPCFNASetupToolStripMenuItem.Text = "UPC + FNA Setup";
            this.uPCFNASetupToolStripMenuItem.Click += new System.EventHandler(this.uPCFNASetupToolStripMenuItem_Click);
            // 
            // bOMFileSetupToolStripMenuItem
            // 
            this.bOMFileSetupToolStripMenuItem.Name = "bOMFileSetupToolStripMenuItem";
            this.bOMFileSetupToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.bOMFileSetupToolStripMenuItem.Text = "BOM File Setup";
            this.bOMFileSetupToolStripMenuItem.Click += new System.EventHandler(this.bOMFileSetupToolStripMenuItem_Click);
            // 
            // axleTypesToolStripMenuItem
            // 
            this.axleTypesToolStripMenuItem.Name = "axleTypesToolStripMenuItem";
            this.axleTypesToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.axleTypesToolStripMenuItem.Text = "Axle Types";
            this.axleTypesToolStripMenuItem.Click += new System.EventHandler(this.axleTypesToolStripMenuItem_Click);
            // 
            // wheelbaseToolStripMenuItem
            // 
            this.wheelbaseToolStripMenuItem.Name = "wheelbaseToolStripMenuItem";
            this.wheelbaseToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.wheelbaseToolStripMenuItem.Text = "Wheelbase";
            this.wheelbaseToolStripMenuItem.Click += new System.EventHandler(this.wheelbaseToolStripMenuItem_Click);
            // 
            // brakeForcesToolStripMenuItem
            // 
            this.brakeForcesToolStripMenuItem.Name = "brakeForcesToolStripMenuItem";
            this.brakeForcesToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.brakeForcesToolStripMenuItem.Text = "Brake Forces";
            this.brakeForcesToolStripMenuItem.Click += new System.EventHandler(this.brakeForcesToolStripMenuItem_Click);
            // 
            // passConfirmationFilesToolStripMenuItem
            // 
            this.passConfirmationFilesToolStripMenuItem.Name = "passConfirmationFilesToolStripMenuItem";
            this.passConfirmationFilesToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.passConfirmationFilesToolStripMenuItem.Text = "Pass Confirmation Files";
            this.passConfirmationFilesToolStripMenuItem.Click += new System.EventHandler(this.passConfirmationFilesToolStripMenuItem_Click);
            // 
            // modeCodeSpcificSttingsToolStripMenuItem
            // 
            this.modeCodeSpcificSttingsToolStripMenuItem.Name = "modeCodeSpcificSttingsToolStripMenuItem";
            this.modeCodeSpcificSttingsToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.modeCodeSpcificSttingsToolStripMenuItem.Text = "ESN Settings";
            this.modeCodeSpcificSttingsToolStripMenuItem.Click += new System.EventHandler(this.modeCodeSpcificSttingsToolStripMenuItem_Click);
            // 
            // retainingRollerPressureToolStripMenuItem
            // 
            this.retainingRollerPressureToolStripMenuItem.Name = "retainingRollerPressureToolStripMenuItem";
            this.retainingRollerPressureToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.retainingRollerPressureToolStripMenuItem.Text = "Retaining Roller Pressure";
            this.retainingRollerPressureToolStripMenuItem.Click += new System.EventHandler(this.retainingRollerPressureToolStripMenuItem_Click);
            // 
            // rearAxelToolStripMenuItem
            // 
            this.rearAxelToolStripMenuItem.Name = "rearAxelToolStripMenuItem";
            this.rearAxelToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.rearAxelToolStripMenuItem.Text = "Rear Axel";
            this.rearAxelToolStripMenuItem.Click += new System.EventHandler(this.rearAxelToolStripMenuItem_Click);
            // 
            // transmissionToolStripMenuItem
            // 
            this.transmissionToolStripMenuItem.Name = "transmissionToolStripMenuItem";
            this.transmissionToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.transmissionToolStripMenuItem.Text = "Transmission";
            this.transmissionToolStripMenuItem.Click += new System.EventHandler(this.transmissionToolStripMenuItem_Click);
            // 
            // tireSizeToolStripMenuItem
            // 
            this.tireSizeToolStripMenuItem.Name = "tireSizeToolStripMenuItem";
            this.tireSizeToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.tireSizeToolStripMenuItem.Text = "Tire Size";
            this.tireSizeToolStripMenuItem.Click += new System.EventHandler(this.tireSizeToolStripMenuItem_Click);
            // 
            // speedMeterToolStripMenuItem
            // 
            this.speedMeterToolStripMenuItem.Name = "speedMeterToolStripMenuItem";
            this.speedMeterToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.speedMeterToolStripMenuItem.Text = "Speed Meter";
            this.speedMeterToolStripMenuItem.Click += new System.EventHandler(this.speedMeterToolStripMenuItem_Click);
            // 
            // logsToolStripMenuItem
            // 
            this.logsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.viewCurrentLogToolStripMenuItem,
            this.viewPreviousLogToolStripMenuItem,
            this.exportLogFilesToolStripMenuItem});
            this.logsToolStripMenuItem.Name = "logsToolStripMenuItem";
            this.logsToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.logsToolStripMenuItem.Text = "Logs";
            // 
            // viewCurrentLogToolStripMenuItem
            // 
            this.viewCurrentLogToolStripMenuItem.Name = "viewCurrentLogToolStripMenuItem";
            this.viewCurrentLogToolStripMenuItem.Size = new System.Drawing.Size(170, 22);
            this.viewCurrentLogToolStripMenuItem.Text = "View Current Log";
            this.viewCurrentLogToolStripMenuItem.Click += new System.EventHandler(this.viewCurrentLogToolStripMenuItem_Click);
            // 
            // viewPreviousLogToolStripMenuItem
            // 
            this.viewPreviousLogToolStripMenuItem.Name = "viewPreviousLogToolStripMenuItem";
            this.viewPreviousLogToolStripMenuItem.Size = new System.Drawing.Size(170, 22);
            this.viewPreviousLogToolStripMenuItem.Text = "View Previous Log";
            this.viewPreviousLogToolStripMenuItem.Click += new System.EventHandler(this.viewPreviousLogToolStripMenuItem_Click);
            // 
            // exportLogFilesToolStripMenuItem
            // 
            this.exportLogFilesToolStripMenuItem.Name = "exportLogFilesToolStripMenuItem";
            this.exportLogFilesToolStripMenuItem.Size = new System.Drawing.Size(170, 22);
            this.exportLogFilesToolStripMenuItem.Text = "Export Log Files";
            this.exportLogFilesToolStripMenuItem.Click += new System.EventHandler(this.exportLogFilesToolStripMenuItem_Click);
            // 
            // m_logMessageLabel
            // 
            this.m_logMessageLabel.AutoSize = true;
            this.m_logMessageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_logMessageLabel.Location = new System.Drawing.Point(12, 150);
            this.m_logMessageLabel.Name = "m_logMessageLabel";
            this.m_logMessageLabel.Size = new System.Drawing.Size(63, 13);
            this.m_logMessageLabel.TabIndex = 2;
            this.m_logMessageLabel.Text = "Messages";
            // 
            // m_logMessageTextBox
            // 
            this.m_logMessageTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.m_logMessageTextBox.BackColor = System.Drawing.Color.White;
            this.m_logMessageTextBox.Location = new System.Drawing.Point(15, 166);
            this.m_logMessageTextBox.Name = "m_logMessageTextBox";
            this.m_logMessageTextBox.ReadOnly = true;
            this.m_logMessageTextBox.Size = new System.Drawing.Size(614, 189);
            this.m_logMessageTextBox.TabIndex = 3;
            this.m_logMessageTextBox.Text = "";
            // 
            // m_passFileCheckTimer
            // 
            this.m_passFileCheckTimer.Tick += new System.EventHandler(this.m_passFileCheckTimer_Tick);
            // 
            // m_esnFileCheckTimer
            // 
            this.m_esnFileCheckTimer.Tick += new System.EventHandler(this.m_esnFileCheckTimer_Tick);
            // 
            // m_tsnFileCheckTimer
            // 
            this.m_tsnFileCheckTimer.Tick += new System.EventHandler(this.m_tsnFileCheckTimer_Tick);
            // 
            // m_stpFileCheckTimer
            // 
            this.m_stpFileCheckTimer.Interval = 1000;
            this.m_stpFileCheckTimer.Tick += new System.EventHandler(this.m_stpFileCheckTimer_Tick);
            // 
            // BomProcessorMainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(641, 367);
            this.Controls.Add(this.m_logMessageTextBox);
            this.Controls.Add(this.m_logMessageLabel);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "BomProcessorMainForm";
            this.Text = "BOM Processor";
            this.Closed += new System.EventHandler(this.BomProcessorMainForm_Closed);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.BomProcessorMainForm_FormClosing);
            this.Load += new System.EventHandler(this.BomProcessorMainForm_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem setupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.Label m_logMessageLabel;
        private System.Windows.Forms.ToolStripMenuItem logsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewCurrentLogToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewPreviousLogToolStripMenuItem;
        private System.Windows.Forms.RichTextBox m_logMessageTextBox;
        private System.Windows.Forms.ToolStripMenuItem uPCFNASetupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exportLogFilesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fileLocationsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem selectBOMFolderToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem selectBOMArchiveFolderToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem selectBOMResultFolderToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem bOMFileSetupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem axleTypesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem wheelbaseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem brakeForcesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem existingParameterSetsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem passConfirmationFilesToolStripMenuItem;
        private System.Windows.Forms.Timer m_passFileCheckTimer;
        private System.Windows.Forms.Timer m_esnFileCheckTimer;
        private System.Windows.Forms.Timer m_tsnFileCheckTimer;
        private System.Windows.Forms.ToolStripMenuItem modeCodeSpcificSttingsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem retainingRollerPressureToolStripMenuItem;
        private System.Windows.Forms.Timer m_stpFileCheckTimer;
        private System.Windows.Forms.ToolStripMenuItem rearAxelToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem transmissionToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem tireSizeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem speedMeterToolStripMenuItem;
    }
}

