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
            this.vehicleBuildFileFolderToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.uPCFNASetupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.bOMFileSetupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.axleTypesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.wheelbaseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.brakeForcesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.passConfirmationFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.logsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewCurrentLogToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewPreviousLogToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exportLogFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_fileCheckTimer = new System.Windows.Forms.Timer(this.components);
            this.m_logMessageLabel = new System.Windows.Forms.Label();
            this.m_logMessageTextBox = new System.Windows.Forms.RichTextBox();
            this.m_passFileCheckTimer = new System.Windows.Forms.Timer(this.components);
            this.m_esnFileCheckTimer = new System.Windows.Forms.Timer(this.components);
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
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(8, 2, 0, 2);
            this.menuStrip1.Size = new System.Drawing.Size(855, 28);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(44, 24);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(102, 24);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.existingParameterSetsToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(47, 24);
            this.editToolStripMenuItem.Text = "Edit";
            // 
            // existingParameterSetsToolStripMenuItem
            // 
            this.existingParameterSetsToolStripMenuItem.Name = "existingParameterSetsToolStripMenuItem";
            this.existingParameterSetsToolStripMenuItem.Size = new System.Drawing.Size(232, 24);
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
            this.passConfirmationFilesToolStripMenuItem});
            this.setupToolStripMenuItem.Name = "setupToolStripMenuItem";
            this.setupToolStripMenuItem.Size = new System.Drawing.Size(59, 24);
            this.setupToolStripMenuItem.Text = "Setup";
            // 
            // fileLocationsToolStripMenuItem
            // 
            this.fileLocationsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectBOMFolderToolStripMenuItem1,
            this.selectBOMArchiveFolderToolStripMenuItem,
            this.selectBOMResultFolderToolStripMenuItem1,
            this.vehicleBuildFileFolderToolStripMenuItem});
            this.fileLocationsToolStripMenuItem.Name = "fileLocationsToolStripMenuItem";
            this.fileLocationsToolStripMenuItem.Size = new System.Drawing.Size(230, 24);
            this.fileLocationsToolStripMenuItem.Text = "File Locations";
            // 
            // selectBOMFolderToolStripMenuItem1
            // 
            this.selectBOMFolderToolStripMenuItem1.Name = "selectBOMFolderToolStripMenuItem1";
            this.selectBOMFolderToolStripMenuItem1.Size = new System.Drawing.Size(254, 24);
            this.selectBOMFolderToolStripMenuItem1.Text = "Select BOM Folder";
            this.selectBOMFolderToolStripMenuItem1.Click += new System.EventHandler(this.selectBOMFolderToolStripMenuItem_Click);
            // 
            // selectBOMArchiveFolderToolStripMenuItem
            // 
            this.selectBOMArchiveFolderToolStripMenuItem.Name = "selectBOMArchiveFolderToolStripMenuItem";
            this.selectBOMArchiveFolderToolStripMenuItem.Size = new System.Drawing.Size(254, 24);
            this.selectBOMArchiveFolderToolStripMenuItem.Text = "Select BOM Archive Folder";
            this.selectBOMArchiveFolderToolStripMenuItem.Click += new System.EventHandler(this.selectBOMArchiveFolderToolStripMenuItem_Click);
            // 
            // selectBOMResultFolderToolStripMenuItem1
            // 
            this.selectBOMResultFolderToolStripMenuItem1.Name = "selectBOMResultFolderToolStripMenuItem1";
            this.selectBOMResultFolderToolStripMenuItem1.Size = new System.Drawing.Size(254, 24);
            this.selectBOMResultFolderToolStripMenuItem1.Text = "Select BOM Result Folder";
            this.selectBOMResultFolderToolStripMenuItem1.Click += new System.EventHandler(this.selectBOMResultFolderToolStripMenuItem_Click);
            // 
            // vehicleBuildFileFolderToolStripMenuItem
            // 
            this.vehicleBuildFileFolderToolStripMenuItem.Name = "vehicleBuildFileFolderToolStripMenuItem";
            this.vehicleBuildFileFolderToolStripMenuItem.Size = new System.Drawing.Size(254, 24);
            this.vehicleBuildFileFolderToolStripMenuItem.Text = "Vehicle Build File Folder";
            this.vehicleBuildFileFolderToolStripMenuItem.Click += new System.EventHandler(this.vehcleBuildFileFolderToolStripMenuItem_Click);
            // 
            // uPCFNASetupToolStripMenuItem
            // 
            this.uPCFNASetupToolStripMenuItem.Name = "uPCFNASetupToolStripMenuItem";
            this.uPCFNASetupToolStripMenuItem.Size = new System.Drawing.Size(230, 24);
            this.uPCFNASetupToolStripMenuItem.Text = "UPC + FNA Setup";
            this.uPCFNASetupToolStripMenuItem.Click += new System.EventHandler(this.uPCFNASetupToolStripMenuItem_Click);
            // 
            // bOMFileSetupToolStripMenuItem
            // 
            this.bOMFileSetupToolStripMenuItem.Name = "bOMFileSetupToolStripMenuItem";
            this.bOMFileSetupToolStripMenuItem.Size = new System.Drawing.Size(230, 24);
            this.bOMFileSetupToolStripMenuItem.Text = "BOM File Setup";
            this.bOMFileSetupToolStripMenuItem.Click += new System.EventHandler(this.bOMFileSetupToolStripMenuItem_Click);
            // 
            // axleTypesToolStripMenuItem
            // 
            this.axleTypesToolStripMenuItem.Name = "axleTypesToolStripMenuItem";
            this.axleTypesToolStripMenuItem.Size = new System.Drawing.Size(230, 24);
            this.axleTypesToolStripMenuItem.Text = "Axle Types";
            this.axleTypesToolStripMenuItem.Click += new System.EventHandler(this.axleTypesToolStripMenuItem_Click);
            // 
            // wheelbaseToolStripMenuItem
            // 
            this.wheelbaseToolStripMenuItem.Name = "wheelbaseToolStripMenuItem";
            this.wheelbaseToolStripMenuItem.Size = new System.Drawing.Size(230, 24);
            this.wheelbaseToolStripMenuItem.Text = "Wheelbase";
            this.wheelbaseToolStripMenuItem.Click += new System.EventHandler(this.wheelbaseToolStripMenuItem_Click);
            // 
            // brakeForcesToolStripMenuItem
            // 
            this.brakeForcesToolStripMenuItem.Name = "brakeForcesToolStripMenuItem";
            this.brakeForcesToolStripMenuItem.Size = new System.Drawing.Size(230, 24);
            this.brakeForcesToolStripMenuItem.Text = "Brake Forces";
            this.brakeForcesToolStripMenuItem.Click += new System.EventHandler(this.brakeForcesToolStripMenuItem_Click);
            // 
            // passConfirmationFilesToolStripMenuItem
            // 
            this.passConfirmationFilesToolStripMenuItem.Name = "passConfirmationFilesToolStripMenuItem";
            this.passConfirmationFilesToolStripMenuItem.Size = new System.Drawing.Size(230, 24);
            this.passConfirmationFilesToolStripMenuItem.Text = "Pass Confirmation Files";
            this.passConfirmationFilesToolStripMenuItem.Click += new System.EventHandler(this.passConfirmationFilesToolStripMenuItem_Click);
            // 
            // logsToolStripMenuItem
            // 
            this.logsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.viewCurrentLogToolStripMenuItem,
            this.viewPreviousLogToolStripMenuItem,
            this.exportLogFilesToolStripMenuItem});
            this.logsToolStripMenuItem.Name = "logsToolStripMenuItem";
            this.logsToolStripMenuItem.Size = new System.Drawing.Size(52, 24);
            this.logsToolStripMenuItem.Text = "Logs";
            // 
            // viewCurrentLogToolStripMenuItem
            // 
            this.viewCurrentLogToolStripMenuItem.Name = "viewCurrentLogToolStripMenuItem";
            this.viewCurrentLogToolStripMenuItem.Size = new System.Drawing.Size(198, 24);
            this.viewCurrentLogToolStripMenuItem.Text = "View Current Log";
            this.viewCurrentLogToolStripMenuItem.Click += new System.EventHandler(this.viewCurrentLogToolStripMenuItem_Click);
            // 
            // viewPreviousLogToolStripMenuItem
            // 
            this.viewPreviousLogToolStripMenuItem.Name = "viewPreviousLogToolStripMenuItem";
            this.viewPreviousLogToolStripMenuItem.Size = new System.Drawing.Size(198, 24);
            this.viewPreviousLogToolStripMenuItem.Text = "View Previous Log";
            this.viewPreviousLogToolStripMenuItem.Click += new System.EventHandler(this.viewPreviousLogToolStripMenuItem_Click);
            // 
            // exportLogFilesToolStripMenuItem
            // 
            this.exportLogFilesToolStripMenuItem.Name = "exportLogFilesToolStripMenuItem";
            this.exportLogFilesToolStripMenuItem.Size = new System.Drawing.Size(198, 24);
            this.exportLogFilesToolStripMenuItem.Text = "Export Log Files";
            this.exportLogFilesToolStripMenuItem.Click += new System.EventHandler(this.exportLogFilesToolStripMenuItem_Click);
            // 
            // m_fileCheckTimer
            // 
            this.m_fileCheckTimer.Interval = 1000;
            this.m_fileCheckTimer.Tick += new System.EventHandler(this.m_fileCheckTimer_Tick);
            // 
            // m_logMessageLabel
            // 
            this.m_logMessageLabel.AutoSize = true;
            this.m_logMessageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_logMessageLabel.Location = new System.Drawing.Point(16, 185);
            this.m_logMessageLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_logMessageLabel.Name = "m_logMessageLabel";
            this.m_logMessageLabel.Size = new System.Drawing.Size(80, 17);
            this.m_logMessageLabel.TabIndex = 2;
            this.m_logMessageLabel.Text = "Messages";
            // 
            // m_logMessageTextBox
            // 
            this.m_logMessageTextBox.BackColor = System.Drawing.Color.White;
            this.m_logMessageTextBox.Location = new System.Drawing.Point(20, 204);
            this.m_logMessageTextBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.m_logMessageTextBox.Name = "m_logMessageTextBox";
            this.m_logMessageTextBox.ReadOnly = true;
            this.m_logMessageTextBox.Size = new System.Drawing.Size(817, 232);
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
            // BomProcessorMainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(855, 452);
            this.Controls.Add(this.m_logMessageTextBox);
            this.Controls.Add(this.m_logMessageLabel);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Name = "BomProcessorMainForm";
            this.Text = "BOM Processor";
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
        private System.Windows.Forms.Timer m_fileCheckTimer;
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
        private System.Windows.Forms.ToolStripMenuItem vehicleBuildFileFolderToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem bOMFileSetupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem axleTypesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem wheelbaseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem brakeForcesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem existingParameterSetsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem passConfirmationFilesToolStripMenuItem;
        private System.Windows.Forms.Timer m_passFileCheckTimer;
        private System.Windows.Forms.Timer m_esnFileCheckTimer;
    }
}

