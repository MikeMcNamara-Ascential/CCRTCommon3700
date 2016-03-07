namespace VehicleCommServer
{
    partial class ECUCommunicationTestForm
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
            this.m_channelComboBox = new System.Windows.Forms.ComboBox();
            this.m_channelLabel = new System.Windows.Forms.Label();
            this.m_moduleIDDataGridView = new System.Windows.Forms.DataGridView();
            this.ModuleID0 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ModuleID1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ModuleID2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ModuleID3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_headerLabel = new System.Windows.Forms.Label();
            this.m_messageLabel = new System.Windows.Forms.Label();
            this.m_transmitMessageDataGridView = new System.Windows.Forms.DataGridView();
            this.m00 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m01 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m02 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m03 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m04 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m05 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m06 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m07 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_sendMessageButton = new System.Windows.Forms.Button();
            this.m_textBoxStatus = new System.Windows.Forms.TextBox();
            this.m_deviceComboBox = new System.Windows.Forms.ComboBox();
            this.m_deviceLabel = new System.Windows.Forms.Label();
            this.m_getDataButton = new System.Windows.Forms.Button();
            this.m_responseIDLabel = new System.Windows.Forms.Label();
            this.m_responseIDDataGridView = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.m_moduleIDDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_transmitMessageDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_responseIDDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_channelComboBox
            // 
            this.m_channelComboBox.FormattingEnabled = true;
            this.m_channelComboBox.Location = new System.Drawing.Point(358, 18);
            this.m_channelComboBox.Name = "m_channelComboBox";
            this.m_channelComboBox.Size = new System.Drawing.Size(157, 21);
            this.m_channelComboBox.TabIndex = 2;
            // 
            // m_channelLabel
            // 
            this.m_channelLabel.AutoSize = true;
            this.m_channelLabel.Location = new System.Drawing.Point(303, 21);
            this.m_channelLabel.Name = "m_channelLabel";
            this.m_channelLabel.Size = new System.Drawing.Size(49, 13);
            this.m_channelLabel.TabIndex = 1;
            this.m_channelLabel.Text = "Channel:";
            // 
            // m_moduleIDDataGridView
            // 
            this.m_moduleIDDataGridView.AllowUserToAddRows = false;
            this.m_moduleIDDataGridView.AllowUserToDeleteRows = false;
            this.m_moduleIDDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_moduleIDDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ModuleID0,
            this.ModuleID1,
            this.ModuleID2,
            this.ModuleID3});
            this.m_moduleIDDataGridView.Location = new System.Drawing.Point(88, 61);
            this.m_moduleIDDataGridView.Name = "m_moduleIDDataGridView";
            this.m_moduleIDDataGridView.Size = new System.Drawing.Size(244, 57);
            this.m_moduleIDDataGridView.TabIndex = 3;
            // 
            // ModuleID0
            // 
            this.ModuleID0.HeaderText = "ID0";
            this.ModuleID0.Name = "ModuleID0";
            this.ModuleID0.Width = 50;
            // 
            // ModuleID1
            // 
            this.ModuleID1.HeaderText = "ID1";
            this.ModuleID1.Name = "ModuleID1";
            this.ModuleID1.Width = 50;
            // 
            // ModuleID2
            // 
            this.ModuleID2.HeaderText = "ID2";
            this.ModuleID2.Name = "ModuleID2";
            this.ModuleID2.Width = 50;
            // 
            // ModuleID3
            // 
            this.ModuleID3.HeaderText = "ID3";
            this.ModuleID3.Name = "ModuleID3";
            this.ModuleID3.Width = 50;
            // 
            // m_headerLabel
            // 
            this.m_headerLabel.AutoSize = true;
            this.m_headerLabel.Location = new System.Drawing.Point(23, 61);
            this.m_headerLabel.Name = "m_headerLabel";
            this.m_headerLabel.Size = new System.Drawing.Size(64, 13);
            this.m_headerLabel.TabIndex = 3;
            this.m_headerLabel.Text = "Request ID:";
            // 
            // m_messageLabel
            // 
            this.m_messageLabel.AutoSize = true;
            this.m_messageLabel.Location = new System.Drawing.Point(23, 124);
            this.m_messageLabel.Name = "m_messageLabel";
            this.m_messageLabel.Size = new System.Drawing.Size(53, 13);
            this.m_messageLabel.TabIndex = 4;
            this.m_messageLabel.Text = "Message:";
            // 
            // m_transmitMessageDataGridView
            // 
            this.m_transmitMessageDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_transmitMessageDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.m00,
            this.m01,
            this.m02,
            this.m03,
            this.m04,
            this.m05,
            this.m06,
            this.m07});
            this.m_transmitMessageDataGridView.Location = new System.Drawing.Point(88, 124);
            this.m_transmitMessageDataGridView.Name = "m_transmitMessageDataGridView";
            this.m_transmitMessageDataGridView.Size = new System.Drawing.Size(443, 75);
            this.m_transmitMessageDataGridView.TabIndex = 5;
            // 
            // m00
            // 
            this.m00.HeaderText = "00";
            this.m00.Name = "m00";
            this.m00.Width = 50;
            // 
            // m01
            // 
            this.m01.HeaderText = "01";
            this.m01.Name = "m01";
            this.m01.Width = 50;
            // 
            // m02
            // 
            this.m02.HeaderText = "02";
            this.m02.Name = "m02";
            this.m02.Width = 50;
            // 
            // m03
            // 
            this.m03.HeaderText = "03";
            this.m03.Name = "m03";
            this.m03.Width = 50;
            // 
            // m04
            // 
            this.m04.HeaderText = "04";
            this.m04.Name = "m04";
            this.m04.Width = 50;
            // 
            // m05
            // 
            this.m05.HeaderText = "05";
            this.m05.Name = "m05";
            this.m05.Width = 50;
            // 
            // m06
            // 
            this.m06.HeaderText = "06";
            this.m06.Name = "m06";
            this.m06.Width = 50;
            // 
            // m07
            // 
            this.m07.HeaderText = "07";
            this.m07.Name = "m07";
            this.m07.Width = 50;
            // 
            // m_sendMessageButton
            // 
            this.m_sendMessageButton.Location = new System.Drawing.Point(88, 424);
            this.m_sendMessageButton.Name = "m_sendMessageButton";
            this.m_sendMessageButton.Size = new System.Drawing.Size(87, 27);
            this.m_sendMessageButton.TabIndex = 6;
            this.m_sendMessageButton.Text = "Send Message";
            this.m_sendMessageButton.UseVisualStyleBackColor = true;
            this.m_sendMessageButton.Click += new System.EventHandler(this.m_sendMessageButton_Click);
            // 
            // m_textBoxStatus
            // 
            this.m_textBoxStatus.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_textBoxStatus.Location = new System.Drawing.Point(88, 258);
            this.m_textBoxStatus.Multiline = true;
            this.m_textBoxStatus.Name = "m_textBoxStatus";
            this.m_textBoxStatus.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.m_textBoxStatus.Size = new System.Drawing.Size(473, 160);
            this.m_textBoxStatus.TabIndex = 7;
            // 
            // m_deviceComboBox
            // 
            this.m_deviceComboBox.FormattingEnabled = true;
            this.m_deviceComboBox.Location = new System.Drawing.Point(88, 18);
            this.m_deviceComboBox.Name = "m_deviceComboBox";
            this.m_deviceComboBox.Size = new System.Drawing.Size(157, 21);
            this.m_deviceComboBox.TabIndex = 1;
            this.m_deviceComboBox.SelectedIndexChanged += new System.EventHandler(this.m_deviceComboBox_SelectedIndexChanged);
            // 
            // m_deviceLabel
            // 
            this.m_deviceLabel.AutoSize = true;
            this.m_deviceLabel.Location = new System.Drawing.Point(23, 21);
            this.m_deviceLabel.Name = "m_deviceLabel";
            this.m_deviceLabel.Size = new System.Drawing.Size(44, 13);
            this.m_deviceLabel.TabIndex = 9;
            this.m_deviceLabel.Text = "Device:";
            // 
            // m_getDataButton
            // 
            this.m_getDataButton.Location = new System.Drawing.Point(181, 424);
            this.m_getDataButton.Name = "m_getDataButton";
            this.m_getDataButton.Size = new System.Drawing.Size(87, 27);
            this.m_getDataButton.TabIndex = 10;
            this.m_getDataButton.Text = "Get ECU Data";
            this.m_getDataButton.UseVisualStyleBackColor = true;
            this.m_getDataButton.Click += new System.EventHandler(this.m_getDataButton_Click);
            // 
            // m_responseIDLabel
            // 
            this.m_responseIDLabel.AutoSize = true;
            this.m_responseIDLabel.Location = new System.Drawing.Point(338, 61);
            this.m_responseIDLabel.Name = "m_responseIDLabel";
            this.m_responseIDLabel.Size = new System.Drawing.Size(72, 13);
            this.m_responseIDLabel.TabIndex = 11;
            this.m_responseIDLabel.Text = "Response ID:";
            // 
            // m_responseIDDataGridView
            // 
            this.m_responseIDDataGridView.AllowUserToAddRows = false;
            this.m_responseIDDataGridView.AllowUserToDeleteRows = false;
            this.m_responseIDDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_responseIDDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1,
            this.dataGridViewTextBoxColumn2,
            this.dataGridViewTextBoxColumn3,
            this.dataGridViewTextBoxColumn4});
            this.m_responseIDDataGridView.Location = new System.Drawing.Point(416, 61);
            this.m_responseIDDataGridView.Name = "m_responseIDDataGridView";
            this.m_responseIDDataGridView.Size = new System.Drawing.Size(244, 57);
            this.m_responseIDDataGridView.TabIndex = 4;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.HeaderText = "ID0";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.Width = 50;
            // 
            // dataGridViewTextBoxColumn2
            // 
            this.dataGridViewTextBoxColumn2.HeaderText = "ID1";
            this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            this.dataGridViewTextBoxColumn2.Width = 50;
            // 
            // dataGridViewTextBoxColumn3
            // 
            this.dataGridViewTextBoxColumn3.HeaderText = "ID2";
            this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
            this.dataGridViewTextBoxColumn3.Width = 50;
            // 
            // dataGridViewTextBoxColumn4
            // 
            this.dataGridViewTextBoxColumn4.HeaderText = "ID3";
            this.dataGridViewTextBoxColumn4.Name = "dataGridViewTextBoxColumn4";
            this.dataGridViewTextBoxColumn4.Width = 50;
            // 
            // ECUCommunicationTestForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(707, 463);
            this.Controls.Add(this.m_responseIDDataGridView);
            this.Controls.Add(this.m_responseIDLabel);
            this.Controls.Add(this.m_getDataButton);
            this.Controls.Add(this.m_deviceLabel);
            this.Controls.Add(this.m_deviceComboBox);
            this.Controls.Add(this.m_textBoxStatus);
            this.Controls.Add(this.m_sendMessageButton);
            this.Controls.Add(this.m_transmitMessageDataGridView);
            this.Controls.Add(this.m_messageLabel);
            this.Controls.Add(this.m_headerLabel);
            this.Controls.Add(this.m_moduleIDDataGridView);
            this.Controls.Add(this.m_channelLabel);
            this.Controls.Add(this.m_channelComboBox);
            this.Name = "ECUCommunicationTestForm";
            this.Text = "ECUCommunicationTestForm";
            ((System.ComponentModel.ISupportInitialize)(this.m_moduleIDDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_transmitMessageDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_responseIDDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox m_channelComboBox;
        private System.Windows.Forms.Label m_channelLabel;
        private System.Windows.Forms.DataGridView m_moduleIDDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModuleID0;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModuleID1;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModuleID2;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModuleID3;
        private System.Windows.Forms.Label m_headerLabel;
        private System.Windows.Forms.Label m_messageLabel;
        private System.Windows.Forms.DataGridView m_transmitMessageDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn m00;
        private System.Windows.Forms.DataGridViewTextBoxColumn m01;
        private System.Windows.Forms.DataGridViewTextBoxColumn m02;
        private System.Windows.Forms.DataGridViewTextBoxColumn m03;
        private System.Windows.Forms.DataGridViewTextBoxColumn m04;
        private System.Windows.Forms.DataGridViewTextBoxColumn m05;
        private System.Windows.Forms.DataGridViewTextBoxColumn m06;
        private System.Windows.Forms.DataGridViewTextBoxColumn m07;
        private System.Windows.Forms.Button m_sendMessageButton;
        private System.Windows.Forms.TextBox m_textBoxStatus;
        private System.Windows.Forms.ComboBox m_deviceComboBox;
        private System.Windows.Forms.Label m_deviceLabel;
        private System.Windows.Forms.Button m_getDataButton;
        private System.Windows.Forms.Label m_responseIDLabel;
        private System.Windows.Forms.DataGridView m_responseIDDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
    }
}