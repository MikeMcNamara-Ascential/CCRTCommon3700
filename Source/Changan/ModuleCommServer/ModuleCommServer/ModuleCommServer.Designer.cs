namespace ModuleCommServer
{
    partial class ModuleCommServerForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ModuleCommServerForm));
            this.m_logoPictureBox = new System.Windows.Forms.PictureBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.configurationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.brakeModulesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.logToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.iPAddressesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.vehicleCommunicationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_logGroupBox = new System.Windows.Forms.GroupBox();
            this.m_logTextBox = new System.Windows.Forms.TextBox();
            this.m_vehicleInterfaceLabel = new System.Windows.Forms.Label();
            this.m_vehicleInterfaceComboBox = new System.Windows.Forms.ComboBox();
            this.m_brakeModuleConfigGroupBox = new System.Windows.Forms.GroupBox();
            this.m_brakeModuleDataGridView = new System.Windows.Forms.DataGridView();
            this.AbsType = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Protocol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.CommBus = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ConfigFile = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_qnxConnectLabel = new System.Windows.Forms.Label();
            this.m_qnxIpTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtBox_txMsg = new System.Windows.Forms.TextBox();
            this.btnSend = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.txtBox_MsgToQnx = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.chBxHideKeepAlive = new System.Windows.Forms.CheckBox();
            this.chBx_txToQnx = new System.Windows.Forms.CheckBox();
            this.chBx_simulateVehicle = new System.Windows.Forms.CheckBox();
            this.button4 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.m_logoPictureBox)).BeginInit();
            this.menuStrip1.SuspendLayout();
            this.m_logGroupBox.SuspendLayout();
            this.m_brakeModuleConfigGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_brakeModuleDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_logoPictureBox
            // 
            this.m_logoPictureBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_logoPictureBox.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.m_logoPictureBox.Image = ((System.Drawing.Image)(resources.GetObject("m_logoPictureBox.Image")));
            this.m_logoPictureBox.Location = new System.Drawing.Point(0, 27);
            this.m_logoPictureBox.Name = "m_logoPictureBox";
            this.m_logoPictureBox.Size = new System.Drawing.Size(1082, 68);
            this.m_logoPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.m_logoPictureBox.TabIndex = 0;
            this.m_logoPictureBox.TabStop = false;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.toolsToolStripMenuItem,
            this.configurationToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1082, 24);
            this.menuStrip1.TabIndex = 1;
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
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(48, 20);
            this.toolsToolStripMenuItem.Text = "Tools";
            // 
            // configurationToolStripMenuItem
            // 
            this.configurationToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.brakeModulesToolStripMenuItem,
            this.logToolStripMenuItem,
            this.iPAddressesToolStripMenuItem,
            this.vehicleCommunicationToolStripMenuItem});
            this.configurationToolStripMenuItem.Name = "configurationToolStripMenuItem";
            this.configurationToolStripMenuItem.Size = new System.Drawing.Size(93, 20);
            this.configurationToolStripMenuItem.Text = "Configuration";
            // 
            // brakeModulesToolStripMenuItem
            // 
            this.brakeModulesToolStripMenuItem.Name = "brakeModulesToolStripMenuItem";
            this.brakeModulesToolStripMenuItem.Size = new System.Drawing.Size(202, 22);
            this.brakeModulesToolStripMenuItem.Text = "Brake Modules";
            this.brakeModulesToolStripMenuItem.Click += new System.EventHandler(this.brakeModulesToolStripMenuItem_Click);
            // 
            // logToolStripMenuItem
            // 
            this.logToolStripMenuItem.Name = "logToolStripMenuItem";
            this.logToolStripMenuItem.Size = new System.Drawing.Size(202, 22);
            this.logToolStripMenuItem.Text = "Log Folder";
            this.logToolStripMenuItem.Click += new System.EventHandler(this.logToolStripMenuItem_Click);
            // 
            // iPAddressesToolStripMenuItem
            // 
            this.iPAddressesToolStripMenuItem.Name = "iPAddressesToolStripMenuItem";
            this.iPAddressesToolStripMenuItem.Size = new System.Drawing.Size(202, 22);
            this.iPAddressesToolStripMenuItem.Text = "IP Addresses";
            this.iPAddressesToolStripMenuItem.Click += new System.EventHandler(this.iPAddressesToolStripMenuItem_Click);
            // 
            // vehicleCommunicationToolStripMenuItem
            // 
            this.vehicleCommunicationToolStripMenuItem.Name = "vehicleCommunicationToolStripMenuItem";
            this.vehicleCommunicationToolStripMenuItem.Size = new System.Drawing.Size(202, 22);
            this.vehicleCommunicationToolStripMenuItem.Text = "Vehicle Communication";
            this.vehicleCommunicationToolStripMenuItem.Click += new System.EventHandler(this.vehicleCommunicationToolStripMenuItem_Click);
            // 
            // m_logGroupBox
            // 
            this.m_logGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_logGroupBox.Controls.Add(this.m_logTextBox);
            this.m_logGroupBox.Font = new System.Drawing.Font("Arial", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_logGroupBox.Location = new System.Drawing.Point(12, 354);
            this.m_logGroupBox.Name = "m_logGroupBox";
            this.m_logGroupBox.Size = new System.Drawing.Size(1058, 219);
            this.m_logGroupBox.TabIndex = 3;
            this.m_logGroupBox.TabStop = false;
            this.m_logGroupBox.Text = "Log Messages";
            // 
            // m_logTextBox
            // 
            this.m_logTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_logTextBox.BackColor = System.Drawing.Color.WhiteSmoke;
            this.m_logTextBox.Cursor = System.Windows.Forms.Cursors.Default;
            this.m_logTextBox.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_logTextBox.Location = new System.Drawing.Point(6, 25);
            this.m_logTextBox.Multiline = true;
            this.m_logTextBox.Name = "m_logTextBox";
            this.m_logTextBox.ReadOnly = true;
            this.m_logTextBox.Size = new System.Drawing.Size(1040, 188);
            this.m_logTextBox.TabIndex = 0;
            // 
            // m_vehicleInterfaceLabel
            // 
            this.m_vehicleInterfaceLabel.AutoSize = true;
            this.m_vehicleInterfaceLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vehicleInterfaceLabel.Location = new System.Drawing.Point(15, 111);
            this.m_vehicleInterfaceLabel.Name = "m_vehicleInterfaceLabel";
            this.m_vehicleInterfaceLabel.Size = new System.Drawing.Size(117, 17);
            this.m_vehicleInterfaceLabel.TabIndex = 4;
            this.m_vehicleInterfaceLabel.Text = "Vehicle Interface:";
            // 
            // m_vehicleInterfaceComboBox
            // 
            this.m_vehicleInterfaceComboBox.FormattingEnabled = true;
            this.m_vehicleInterfaceComboBox.Location = new System.Drawing.Point(152, 111);
            this.m_vehicleInterfaceComboBox.Name = "m_vehicleInterfaceComboBox";
            this.m_vehicleInterfaceComboBox.Size = new System.Drawing.Size(198, 21);
            this.m_vehicleInterfaceComboBox.TabIndex = 5;
            this.m_vehicleInterfaceComboBox.SelectedIndexChanged += new System.EventHandler(this.m_vehicleInterfaceComboBox_SelectedIndexChanged);
            // 
            // m_brakeModuleConfigGroupBox
            // 
            this.m_brakeModuleConfigGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_brakeModuleConfigGroupBox.Controls.Add(this.m_brakeModuleDataGridView);
            this.m_brakeModuleConfigGroupBox.Font = new System.Drawing.Font("Arial", 12F);
            this.m_brakeModuleConfigGroupBox.Location = new System.Drawing.Point(18, 187);
            this.m_brakeModuleConfigGroupBox.Name = "m_brakeModuleConfigGroupBox";
            this.m_brakeModuleConfigGroupBox.Size = new System.Drawing.Size(1046, 161);
            this.m_brakeModuleConfigGroupBox.TabIndex = 6;
            this.m_brakeModuleConfigGroupBox.TabStop = false;
            this.m_brakeModuleConfigGroupBox.Text = "Brake Modules";
            // 
            // m_brakeModuleDataGridView
            // 
            this.m_brakeModuleDataGridView.AllowUserToAddRows = false;
            this.m_brakeModuleDataGridView.AllowUserToDeleteRows = false;
            this.m_brakeModuleDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.AllCells;
            this.m_brakeModuleDataGridView.BackgroundColor = System.Drawing.Color.White;
            this.m_brakeModuleDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_brakeModuleDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_brakeModuleDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.AbsType,
            this.Protocol,
            this.CommBus,
            this.ConfigFile});
            this.m_brakeModuleDataGridView.Location = new System.Drawing.Point(6, 25);
            this.m_brakeModuleDataGridView.Name = "m_brakeModuleDataGridView";
            this.m_brakeModuleDataGridView.ReadOnly = true;
            this.m_brakeModuleDataGridView.RowHeadersVisible = false;
            this.m_brakeModuleDataGridView.Size = new System.Drawing.Size(1034, 130);
            this.m_brakeModuleDataGridView.TabIndex = 0;
            // 
            // AbsType
            // 
            this.AbsType.HeaderText = "ABS Type";
            this.AbsType.Name = "AbsType";
            this.AbsType.ReadOnly = true;
            this.AbsType.Width = 95;
            // 
            // Protocol
            // 
            this.Protocol.HeaderText = "Protocol";
            this.Protocol.Name = "Protocol";
            this.Protocol.ReadOnly = true;
            this.Protocol.Width = 91;
            // 
            // CommBus
            // 
            this.CommBus.HeaderText = "Communication Bus";
            this.CommBus.Name = "CommBus";
            this.CommBus.ReadOnly = true;
            this.CommBus.Width = 158;
            // 
            // ConfigFile
            // 
            this.ConfigFile.HeaderText = "Configuration File";
            this.ConfigFile.Name = "ConfigFile";
            this.ConfigFile.ReadOnly = true;
            this.ConfigFile.Width = 142;
            // 
            // m_qnxConnectLabel
            // 
            this.m_qnxConnectLabel.AutoSize = true;
            this.m_qnxConnectLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_qnxConnectLabel.Location = new System.Drawing.Point(815, 111);
            this.m_qnxConnectLabel.Name = "m_qnxConnectLabel";
            this.m_qnxConnectLabel.Size = new System.Drawing.Size(117, 17);
            this.m_qnxConnectLabel.TabIndex = 7;
            this.m_qnxConnectLabel.Text = "QNX Connection:";
            // 
            // m_qnxIpTextBox
            // 
            this.m_qnxIpTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_qnxIpTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_qnxIpTextBox.Location = new System.Drawing.Point(938, 108);
            this.m_qnxIpTextBox.Name = "m_qnxIpTextBox";
            this.m_qnxIpTextBox.ReadOnly = true;
            this.m_qnxIpTextBox.Size = new System.Drawing.Size(120, 23);
            this.m_qnxIpTextBox.TabIndex = 8;
            this.m_qnxIpTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(358, 148);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(75, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "Msg From Qnx";
            // 
            // txtBox_txMsg
            // 
            this.txtBox_txMsg.Location = new System.Drawing.Point(361, 164);
            this.txtBox_txMsg.Name = "txtBox_txMsg";
            this.txtBox_txMsg.Size = new System.Drawing.Size(150, 20);
            this.txtBox_txMsg.TabIndex = 12;
            this.txtBox_txMsg.Text = "SetAbsType:Mk70";
            // 
            // btnSend
            // 
            this.btnSend.Location = new System.Drawing.Point(449, 141);
            this.btnSend.Name = "btnSend";
            this.btnSend.Size = new System.Drawing.Size(62, 20);
            this.btnSend.TabIndex = 11;
            this.btnSend.Text = "Send";
            this.btnSend.UseVisualStyleBackColor = true;
            this.btnSend.Click += new System.EventHandler(this.button1_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(531, 145);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 13);
            this.label1.TabIndex = 16;
            this.label1.Text = "Msg To Qnx";
            // 
            // txtBox_MsgToQnx
            // 
            this.txtBox_MsgToQnx.Location = new System.Drawing.Point(534, 161);
            this.txtBox_MsgToQnx.Name = "txtBox_MsgToQnx";
            this.txtBox_MsgToQnx.Size = new System.Drawing.Size(150, 20);
            this.txtBox_MsgToQnx.TabIndex = 15;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(622, 138);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(62, 20);
            this.button1.TabIndex = 14;
            this.button1.Text = "Send";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(734, 138);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(84, 20);
            this.button2.TabIndex = 17;
            this.button2.Text = "Start Threads";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click_1);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(734, 163);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(84, 20);
            this.button3.TabIndex = 18;
            this.button3.Text = "Stop Threads";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // chBxHideKeepAlive
            // 
            this.chBxHideKeepAlive.AutoSize = true;
            this.chBxHideKeepAlive.Location = new System.Drawing.Point(836, 163);
            this.chBxHideKeepAlive.Name = "chBxHideKeepAlive";
            this.chBxHideKeepAlive.Size = new System.Drawing.Size(125, 17);
            this.chBxHideKeepAlive.TabIndex = 19;
            this.chBxHideKeepAlive.Text = "Hide Keep Alive Msg";
            this.chBxHideKeepAlive.UseVisualStyleBackColor = true;
            this.chBxHideKeepAlive.CheckedChanged += new System.EventHandler(this.chBxHideKeepAlive_CheckedChanged);
            // 
            // chBx_txToQnx
            // 
            this.chBx_txToQnx.AutoSize = true;
            this.chBx_txToQnx.Location = new System.Drawing.Point(836, 141);
            this.chBx_txToQnx.Name = "chBx_txToQnx";
            this.chBx_txToQnx.Size = new System.Drawing.Size(76, 17);
            this.chBx_txToQnx.TabIndex = 20;
            this.chBx_txToQnx.Text = "Tx to QNX";
            this.chBx_txToQnx.UseVisualStyleBackColor = true;
            // 
            // chBx_simulateVehicle
            // 
            this.chBx_simulateVehicle.AutoSize = true;
            this.chBx_simulateVehicle.Location = new System.Drawing.Point(918, 140);
            this.chBx_simulateVehicle.Name = "chBx_simulateVehicle";
            this.chBx_simulateVehicle.Size = new System.Drawing.Size(104, 17);
            this.chBx_simulateVehicle.TabIndex = 21;
            this.chBx_simulateVehicle.Text = "Simulate Vehicle";
            this.chBx_simulateVehicle.UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(974, 164);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(84, 20);
            this.button4.TabIndex = 22;
            this.button4.Text = "Reconnect";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // ModuleCommServerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(1082, 585);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.chBx_simulateVehicle);
            this.Controls.Add(this.chBx_txToQnx);
            this.Controls.Add(this.chBxHideKeepAlive);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtBox_MsgToQnx);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtBox_txMsg);
            this.Controls.Add(this.btnSend);
            this.Controls.Add(this.m_qnxIpTextBox);
            this.Controls.Add(this.m_qnxConnectLabel);
            this.Controls.Add(this.m_brakeModuleConfigGroupBox);
            this.Controls.Add(this.m_vehicleInterfaceComboBox);
            this.Controls.Add(this.m_vehicleInterfaceLabel);
            this.Controls.Add(this.m_logGroupBox);
            this.Controls.Add(this.m_logoPictureBox);
            this.Controls.Add(this.menuStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "ModuleCommServerForm";
            this.Text = "Module Communication Server";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.ModuleCommServerForm_FormClosed);
            ((System.ComponentModel.ISupportInitialize)(this.m_logoPictureBox)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.m_logGroupBox.ResumeLayout(false);
            this.m_logGroupBox.PerformLayout();
            this.m_brakeModuleConfigGroupBox.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.m_brakeModuleDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox m_logoPictureBox;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem configurationToolStripMenuItem;
        private System.Windows.Forms.GroupBox m_logGroupBox;
        private System.Windows.Forms.ToolStripMenuItem logToolStripMenuItem;
        private System.Windows.Forms.Label m_vehicleInterfaceLabel;
        private System.Windows.Forms.ComboBox m_vehicleInterfaceComboBox;
        private System.Windows.Forms.ToolStripMenuItem brakeModulesToolStripMenuItem;
        private System.Windows.Forms.GroupBox m_brakeModuleConfigGroupBox;
        private System.Windows.Forms.DataGridView m_brakeModuleDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn AbsType;
        private System.Windows.Forms.DataGridViewTextBoxColumn Protocol;
        private System.Windows.Forms.DataGridViewTextBoxColumn CommBus;
        private System.Windows.Forms.DataGridViewTextBoxColumn ConfigFile;
        private System.Windows.Forms.TextBox m_logTextBox;
        private System.Windows.Forms.Label m_qnxConnectLabel;
        private System.Windows.Forms.TextBox m_qnxIpTextBox;
        private System.Windows.Forms.ToolStripMenuItem iPAddressesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem vehicleCommunicationToolStripMenuItem;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtBox_txMsg;
        private System.Windows.Forms.Button btnSend;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtBox_MsgToQnx;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.CheckBox chBxHideKeepAlive;
        private System.Windows.Forms.CheckBox chBx_txToQnx;
        private System.Windows.Forms.CheckBox chBx_simulateVehicle;
        private System.Windows.Forms.Button button4;

    }
}

