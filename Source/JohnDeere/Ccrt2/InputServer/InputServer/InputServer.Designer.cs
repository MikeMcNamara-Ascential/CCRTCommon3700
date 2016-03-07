namespace InputServer
{
    partial class InputServerForm
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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.configureToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addInputDeviceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.dataItemsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.databaseLocationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.qNXIPAddressToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.logFileDirectoryToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.maximumLogFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.displayLanguageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.databaseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.logFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.previousLogFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_inputDevLabel = new System.Windows.Forms.Label();
            this.m_inputDevDataGridView = new System.Windows.Forms.DataGridView();
            this.DevName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DevType = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DataItem = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.InputData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.RshStatus = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_qnxIpLabel = new System.Windows.Forms.Label();
            this.m_qnxIpTextBox = new System.Windows.Forms.TextBox();
            this.m_logMessagesLabel = new System.Windows.Forms.Label();
            this.m_logTextBox = new System.Windows.Forms.TextBox();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_inputDevDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.configureToolStripMenuItem,
            this.viewToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(776, 24);
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
            // configureToolStripMenuItem
            // 
            this.configureToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addInputDeviceToolStripMenuItem,
            this.dataItemsToolStripMenuItem,
            this.databaseLocationToolStripMenuItem,
            this.qNXIPAddressToolStripMenuItem,
            this.logFileDirectoryToolStripMenuItem,
            this.maximumLogFilesToolStripMenuItem,
            this.displayLanguageToolStripMenuItem});
            this.configureToolStripMenuItem.Name = "configureToolStripMenuItem";
            this.configureToolStripMenuItem.Size = new System.Drawing.Size(72, 20);
            this.configureToolStripMenuItem.Text = "Configure";
            // 
            // addInputDeviceToolStripMenuItem
            // 
            this.addInputDeviceToolStripMenuItem.Name = "addInputDeviceToolStripMenuItem";
            this.addInputDeviceToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
            this.addInputDeviceToolStripMenuItem.Text = "Port Settings";
            this.addInputDeviceToolStripMenuItem.Click += new System.EventHandler(this.addInputDeviceToolStripMenuItem_Click);
            // 
            // dataItemsToolStripMenuItem
            // 
            this.dataItemsToolStripMenuItem.Name = "dataItemsToolStripMenuItem";
            this.dataItemsToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
            this.dataItemsToolStripMenuItem.Text = "Data Items";
            this.dataItemsToolStripMenuItem.Click += new System.EventHandler(this.dataItemsToolStripMenuItem_Click);
            // 
            // databaseLocationToolStripMenuItem
            // 
            this.databaseLocationToolStripMenuItem.Name = "databaseLocationToolStripMenuItem";
            this.databaseLocationToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
            this.databaseLocationToolStripMenuItem.Text = "Database Location";
            this.databaseLocationToolStripMenuItem.Click += new System.EventHandler(this.databaseLocationToolStripMenuItem_Click);
            // 
            // qNXIPAddressToolStripMenuItem
            // 
            this.qNXIPAddressToolStripMenuItem.Name = "qNXIPAddressToolStripMenuItem";
            this.qNXIPAddressToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
            this.qNXIPAddressToolStripMenuItem.Text = "QNX IP Address";
            this.qNXIPAddressToolStripMenuItem.Click += new System.EventHandler(this.qNXIPAddressToolStripMenuItem_Click);
            // 
            // logFileDirectoryToolStripMenuItem
            // 
            this.logFileDirectoryToolStripMenuItem.Name = "logFileDirectoryToolStripMenuItem";
            this.logFileDirectoryToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
            this.logFileDirectoryToolStripMenuItem.Text = "Log File Directory";
            this.logFileDirectoryToolStripMenuItem.Click += new System.EventHandler(this.logFileDirectoryToolStripMenuItem_Click);
            // 
            // maximumLogFilesToolStripMenuItem
            // 
            this.maximumLogFilesToolStripMenuItem.Name = "maximumLogFilesToolStripMenuItem";
            this.maximumLogFilesToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
            this.maximumLogFilesToolStripMenuItem.Text = "Maximum Log Files";
            this.maximumLogFilesToolStripMenuItem.Click += new System.EventHandler(this.maximumLogFilesToolStripMenuItem_Click);
            // 
            // displayLanguageToolStripMenuItem
            // 
            this.displayLanguageToolStripMenuItem.Name = "displayLanguageToolStripMenuItem";
            this.displayLanguageToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
            this.displayLanguageToolStripMenuItem.Text = "Display Language";
            this.displayLanguageToolStripMenuItem.Click += new System.EventHandler(this.displayLanguageToolStripMenuItem_Click);
            // 
            // viewToolStripMenuItem
            // 
            this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.databaseToolStripMenuItem,
            this.logFileToolStripMenuItem,
            this.previousLogFilesToolStripMenuItem});
            this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            this.viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.viewToolStripMenuItem.Text = "View";
            // 
            // databaseToolStripMenuItem
            // 
            this.databaseToolStripMenuItem.Name = "databaseToolStripMenuItem";
            this.databaseToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.databaseToolStripMenuItem.Text = "Database";
            this.databaseToolStripMenuItem.Click += new System.EventHandler(this.databaseToolStripMenuItem_Click);
            // 
            // logFileToolStripMenuItem
            // 
            this.logFileToolStripMenuItem.Name = "logFileToolStripMenuItem";
            this.logFileToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.logFileToolStripMenuItem.Text = "Current Log File";
            this.logFileToolStripMenuItem.Click += new System.EventHandler(this.logFileToolStripMenuItem_Click);
            // 
            // previousLogFilesToolStripMenuItem
            // 
            this.previousLogFilesToolStripMenuItem.Name = "previousLogFilesToolStripMenuItem";
            this.previousLogFilesToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.previousLogFilesToolStripMenuItem.Text = "Previous Log Files";
            this.previousLogFilesToolStripMenuItem.Click += new System.EventHandler(this.previousLogFilesToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
            this.aboutToolStripMenuItem.Text = "About";
            this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
            // 
            // m_inputDevLabel
            // 
            this.m_inputDevLabel.AutoSize = true;
            this.m_inputDevLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_inputDevLabel.Location = new System.Drawing.Point(12, 89);
            this.m_inputDevLabel.Name = "m_inputDevLabel";
            this.m_inputDevLabel.Size = new System.Drawing.Size(90, 13);
            this.m_inputDevLabel.TabIndex = 1;
            this.m_inputDevLabel.Text = "Input Devices:";
            // 
            // m_inputDevDataGridView
            // 
            this.m_inputDevDataGridView.AllowUserToAddRows = false;
            this.m_inputDevDataGridView.AllowUserToDeleteRows = false;
            this.m_inputDevDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_inputDevDataGridView.BackgroundColor = System.Drawing.SystemColors.Control;
            this.m_inputDevDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_inputDevDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_inputDevDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.DevName,
            this.DevType,
            this.DataItem,
            this.InputData,
            this.RshStatus});
            this.m_inputDevDataGridView.Location = new System.Drawing.Point(15, 105);
            this.m_inputDevDataGridView.Name = "m_inputDevDataGridView";
            this.m_inputDevDataGridView.ReadOnly = true;
            this.m_inputDevDataGridView.RowHeadersVisible = false;
            this.m_inputDevDataGridView.Size = new System.Drawing.Size(752, 173);
            this.m_inputDevDataGridView.TabIndex = 2;
            // 
            // DevName
            // 
            this.DevName.HeaderText = "Device Name";
            this.DevName.Name = "DevName";
            this.DevName.ReadOnly = true;
            // 
            // DevType
            // 
            this.DevType.HeaderText = "Device Type";
            this.DevType.Name = "DevType";
            this.DevType.ReadOnly = true;
            // 
            // DataItem
            // 
            this.DataItem.HeaderText = "Data Item";
            this.DataItem.Name = "DataItem";
            this.DataItem.ReadOnly = true;
            // 
            // InputData
            // 
            this.InputData.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.InputData.HeaderText = "Input Data";
            this.InputData.Name = "InputData";
            this.InputData.ReadOnly = true;
            // 
            // RshStatus
            // 
            this.RshStatus.HeaderText = "Send To QNX";
            this.RshStatus.Name = "RshStatus";
            this.RshStatus.ReadOnly = true;
            // 
            // m_qnxIpLabel
            // 
            this.m_qnxIpLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_qnxIpLabel.AutoSize = true;
            this.m_qnxIpLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_qnxIpLabel.Location = new System.Drawing.Point(531, 34);
            this.m_qnxIpLabel.Name = "m_qnxIpLabel";
            this.m_qnxIpLabel.Size = new System.Drawing.Size(102, 13);
            this.m_qnxIpLabel.TabIndex = 3;
            this.m_qnxIpLabel.Text = "QNX IP Address:";
            // 
            // m_qnxIpTextBox
            // 
            this.m_qnxIpTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_qnxIpTextBox.BackColor = System.Drawing.Color.White;
            this.m_qnxIpTextBox.Location = new System.Drawing.Point(673, 31);
            this.m_qnxIpTextBox.Name = "m_qnxIpTextBox";
            this.m_qnxIpTextBox.ReadOnly = true;
            this.m_qnxIpTextBox.Size = new System.Drawing.Size(91, 20);
            this.m_qnxIpTextBox.TabIndex = 4;
            this.m_qnxIpTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // m_logMessagesLabel
            // 
            this.m_logMessagesLabel.AutoSize = true;
            this.m_logMessagesLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_logMessagesLabel.Location = new System.Drawing.Point(12, 295);
            this.m_logMessagesLabel.Name = "m_logMessagesLabel";
            this.m_logMessagesLabel.Size = new System.Drawing.Size(92, 13);
            this.m_logMessagesLabel.TabIndex = 5;
            this.m_logMessagesLabel.Text = "Log Messages:";
            // 
            // m_logTextBox
            // 
            this.m_logTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_logTextBox.BackColor = System.Drawing.Color.White;
            this.m_logTextBox.Location = new System.Drawing.Point(15, 311);
            this.m_logTextBox.Multiline = true;
            this.m_logTextBox.Name = "m_logTextBox";
            this.m_logTextBox.ReadOnly = true;
            this.m_logTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.m_logTextBox.Size = new System.Drawing.Size(749, 185);
            this.m_logTextBox.TabIndex = 6;
            // 
            // InputServerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(776, 508);
            this.Controls.Add(this.m_logTextBox);
            this.Controls.Add(this.m_logMessagesLabel);
            this.Controls.Add(this.m_qnxIpTextBox);
            this.Controls.Add(this.m_qnxIpLabel);
            this.Controls.Add(this.m_inputDevDataGridView);
            this.Controls.Add(this.m_inputDevLabel);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "InputServerForm";
            this.Text = "Input Server";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_inputDevDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem configureToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addInputDeviceToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem dataItemsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem databaseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem databaseLocationToolStripMenuItem;
        private System.Windows.Forms.Label m_inputDevLabel;
        private System.Windows.Forms.DataGridView m_inputDevDataGridView;
        private System.Windows.Forms.ToolStripMenuItem qNXIPAddressToolStripMenuItem;
        private System.Windows.Forms.Label m_qnxIpLabel;
        private System.Windows.Forms.TextBox m_qnxIpTextBox;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.Label m_logMessagesLabel;
        private System.Windows.Forms.TextBox m_logTextBox;
        private System.Windows.Forms.ToolStripMenuItem logFileDirectoryToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem maximumLogFilesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem logFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem previousLogFilesToolStripMenuItem;
        private System.Windows.Forms.DataGridViewTextBoxColumn DevName;
        private System.Windows.Forms.DataGridViewTextBoxColumn DevType;
        private System.Windows.Forms.DataGridViewTextBoxColumn DataItem;
        private System.Windows.Forms.DataGridViewTextBoxColumn InputData;
        private System.Windows.Forms.DataGridViewTextBoxColumn RshStatus;
        private System.Windows.Forms.ToolStripMenuItem displayLanguageToolStripMenuItem;
    }
}

