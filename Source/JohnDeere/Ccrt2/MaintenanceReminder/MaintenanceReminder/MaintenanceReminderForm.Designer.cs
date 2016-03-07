namespace MaintenanceReminder
{
    partial class MaintenanceReminderForm
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
            this.maintenanceItemsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.changePasswordToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.languageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.logFileLocationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.qNXIPAddressToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.currentLogFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.currentToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.previousToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_reminderDataGridView = new System.Windows.Forms.DataGridView();
            this.m_logTextBox = new System.Windows.Forms.TextBox();
            this.m_elapsedTimeTimer = new System.Windows.Forms.Timer(this.components);
            this.ReminderName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Interval = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ElapsedTime = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TimeRemaining = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ViewNotes = new System.Windows.Forms.DataGridViewButtonColumn();
            this.ViewWorkInstruction = new System.Windows.Forms.DataGridViewButtonColumn();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_reminderDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.viewToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(809, 24);
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
            this.maintenanceItemsToolStripMenuItem,
            this.toolStripSeparator1,
            this.changePasswordToolStripMenuItem,
            this.languageToolStripMenuItem,
            this.logFileLocationToolStripMenuItem,
            this.qNXIPAddressToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(72, 20);
            this.editToolStripMenuItem.Text = "Configure";
            // 
            // maintenanceItemsToolStripMenuItem
            // 
            this.maintenanceItemsToolStripMenuItem.Name = "maintenanceItemsToolStripMenuItem";
            this.maintenanceItemsToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.maintenanceItemsToolStripMenuItem.Text = "Maintenance Items";
            this.maintenanceItemsToolStripMenuItem.Click += new System.EventHandler(this.maintenanceItemsToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(172, 6);
            // 
            // changePasswordToolStripMenuItem
            // 
            this.changePasswordToolStripMenuItem.Name = "changePasswordToolStripMenuItem";
            this.changePasswordToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.changePasswordToolStripMenuItem.Text = "Change Password";
            this.changePasswordToolStripMenuItem.Click += new System.EventHandler(this.changePasswordToolStripMenuItem_Click);
            // 
            // languageToolStripMenuItem
            // 
            this.languageToolStripMenuItem.Name = "languageToolStripMenuItem";
            this.languageToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.languageToolStripMenuItem.Text = "Language";
            this.languageToolStripMenuItem.Click += new System.EventHandler(this.languageToolStripMenuItem_Click);
            // 
            // logFileLocationToolStripMenuItem
            // 
            this.logFileLocationToolStripMenuItem.Name = "logFileLocationToolStripMenuItem";
            this.logFileLocationToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.logFileLocationToolStripMenuItem.Text = "Log File Location";
            this.logFileLocationToolStripMenuItem.Click += new System.EventHandler(this.logFileLocationToolStripMenuItem_Click);
            // 
            // qNXIPAddressToolStripMenuItem
            // 
            this.qNXIPAddressToolStripMenuItem.Name = "qNXIPAddressToolStripMenuItem";
            this.qNXIPAddressToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.qNXIPAddressToolStripMenuItem.Text = "QNX IP Address";
            this.qNXIPAddressToolStripMenuItem.Click += new System.EventHandler(this.qNXIPAddressToolStripMenuItem_Click);
            // 
            // viewToolStripMenuItem
            // 
            this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.currentLogFileToolStripMenuItem});
            this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            this.viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.viewToolStripMenuItem.Text = "View";
            // 
            // currentLogFileToolStripMenuItem
            // 
            this.currentLogFileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.currentToolStripMenuItem,
            this.previousToolStripMenuItem});
            this.currentLogFileToolStripMenuItem.Name = "currentLogFileToolStripMenuItem";
            this.currentLogFileToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.currentLogFileToolStripMenuItem.Text = "Log File";
            // 
            // currentToolStripMenuItem
            // 
            this.currentToolStripMenuItem.Name = "currentToolStripMenuItem";
            this.currentToolStripMenuItem.Size = new System.Drawing.Size(119, 22);
            this.currentToolStripMenuItem.Text = "Current";
            this.currentToolStripMenuItem.Click += new System.EventHandler(this.currentToolStripMenuItem_Click);
            // 
            // previousToolStripMenuItem
            // 
            this.previousToolStripMenuItem.Name = "previousToolStripMenuItem";
            this.previousToolStripMenuItem.Size = new System.Drawing.Size(119, 22);
            this.previousToolStripMenuItem.Text = "Previous";
            this.previousToolStripMenuItem.Click += new System.EventHandler(this.previousToolStripMenuItem_Click);
            // 
            // m_reminderDataGridView
            // 
            this.m_reminderDataGridView.AllowUserToAddRows = false;
            this.m_reminderDataGridView.AllowUserToDeleteRows = false;
            this.m_reminderDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_reminderDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_reminderDataGridView.BackgroundColor = System.Drawing.Color.LightSteelBlue;
            this.m_reminderDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_reminderDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_reminderDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ReminderName,
            this.Interval,
            this.ElapsedTime,
            this.TimeRemaining,
            this.ViewNotes,
            this.ViewWorkInstruction});
            this.m_reminderDataGridView.Location = new System.Drawing.Point(12, 62);
            this.m_reminderDataGridView.Name = "m_reminderDataGridView";
            this.m_reminderDataGridView.RowHeadersVisible = false;
            this.m_reminderDataGridView.Size = new System.Drawing.Size(785, 177);
            this.m_reminderDataGridView.TabIndex = 1;
            this.m_reminderDataGridView.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_reminderDataGridView_CellClick);
            // 
            // m_logTextBox
            // 
            this.m_logTextBox.Location = new System.Drawing.Point(12, 245);
            this.m_logTextBox.Multiline = true;
            this.m_logTextBox.Name = "m_logTextBox";
            this.m_logTextBox.ReadOnly = true;
            this.m_logTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.m_logTextBox.Size = new System.Drawing.Size(785, 230);
            this.m_logTextBox.TabIndex = 2;
            // 
            // m_elapsedTimeTimer
            // 
            this.m_elapsedTimeTimer.Enabled = true;
            this.m_elapsedTimeTimer.Interval = 30000;
            this.m_elapsedTimeTimer.Tick += new System.EventHandler(this.m_elapsedTimeTimer_Tick);
            // 
            // ReminderName
            // 
            this.ReminderName.HeaderText = "Reminder Item";
            this.ReminderName.Name = "ReminderName";
            this.ReminderName.ReadOnly = true;
            // 
            // Interval
            // 
            this.Interval.HeaderText = "Interval";
            this.Interval.Name = "Interval";
            this.Interval.ReadOnly = true;
            // 
            // ElapsedTime
            // 
            this.ElapsedTime.HeaderText = "Elapsed Time";
            this.ElapsedTime.Name = "ElapsedTime";
            this.ElapsedTime.ReadOnly = true;
            // 
            // TimeRemaining
            // 
            this.TimeRemaining.HeaderText = "Time Remaining";
            this.TimeRemaining.Name = "TimeRemaining";
            this.TimeRemaining.ReadOnly = true;
            // 
            // ViewNotes
            // 
            this.ViewNotes.HeaderText = "View Notes";
            this.ViewNotes.Name = "ViewNotes";
            this.ViewNotes.ReadOnly = true;
            this.ViewNotes.Text = "View Maintenance Notes";
            this.ViewNotes.UseColumnTextForButtonValue = true;
            // 
            // ViewWorkInstruction
            // 
            this.ViewWorkInstruction.HeaderText = "View Work Instruction";
            this.ViewWorkInstruction.Name = "ViewWorkInstruction";
            this.ViewWorkInstruction.ReadOnly = true;
            this.ViewWorkInstruction.Text = "View Work Instruction";
            this.ViewWorkInstruction.UseColumnTextForButtonValue = true;
            // 
            // MaintenanceReminderForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.ClientSize = new System.Drawing.Size(809, 487);
            this.ControlBox = false;
            this.Controls.Add(this.m_logTextBox);
            this.Controls.Add(this.m_reminderDataGridView);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MaintenanceReminderForm";
            this.Text = "Maintenance Reminder";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_reminderDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem maintenanceItemsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem languageToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem logFileLocationToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem currentLogFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem currentToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem previousToolStripMenuItem;
        private System.Windows.Forms.DataGridView m_reminderDataGridView;
        private System.Windows.Forms.TextBox m_logTextBox;
        private System.Windows.Forms.Timer m_elapsedTimeTimer;
        private System.Windows.Forms.ToolStripMenuItem qNXIPAddressToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem changePasswordToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.DataGridViewTextBoxColumn ReminderName;
        private System.Windows.Forms.DataGridViewTextBoxColumn Interval;
        private System.Windows.Forms.DataGridViewTextBoxColumn ElapsedTime;
        private System.Windows.Forms.DataGridViewTextBoxColumn TimeRemaining;
        private System.Windows.Forms.DataGridViewButtonColumn ViewNotes;
        private System.Windows.Forms.DataGridViewButtonColumn ViewWorkInstruction;
    }
}

