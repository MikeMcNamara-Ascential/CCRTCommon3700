namespace VehicleCommServer
{
    partial class VehicleCommSettingsForm
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
            this.m_settingsLabel = new System.Windows.Forms.Label();
            this.m_commNameLabel = new System.Windows.Forms.Label();
            this.m_commNameComboBox = new System.Windows.Forms.ComboBox();
            this.m_deviceComboBox = new System.Windows.Forms.ComboBox();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_saveSettingsButton = new System.Windows.Forms.Button();
            this.m_removeDeviceButton = new System.Windows.Forms.Button();
            this.m_protocolLabel = new System.Windows.Forms.Label();
            this.m_protocolComboBox = new System.Windows.Forms.ComboBox();
            this.m_channelLabel = new System.Windows.Forms.Label();
            this.m_baudRateLabel = new System.Windows.Forms.Label();
            this.m_connectFlagLabel = new System.Windows.Forms.Label();
            this.m_channelComboBox = new System.Windows.Forms.ComboBox();
            this.m_baudRateComboBox = new System.Windows.Forms.ComboBox();
            this.m_connectFlagComboBox = new System.Windows.Forms.ComboBox();
            this.m_deviceLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // m_settingsLabel
            // 
            this.m_settingsLabel.AutoSize = true;
            this.m_settingsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_settingsLabel.Location = new System.Drawing.Point(12, 65);
            this.m_settingsLabel.Name = "m_settingsLabel";
            this.m_settingsLabel.Size = new System.Drawing.Size(68, 16);
            this.m_settingsLabel.TabIndex = 0;
            this.m_settingsLabel.Text = "Settings:";
            // 
            // m_commNameLabel
            // 
            this.m_commNameLabel.AutoSize = true;
            this.m_commNameLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_commNameLabel.Location = new System.Drawing.Point(12, 37);
            this.m_commNameLabel.Name = "m_commNameLabel";
            this.m_commNameLabel.Size = new System.Drawing.Size(160, 16);
            this.m_commNameLabel.TabIndex = 1;
            this.m_commNameLabel.Text = "Comm Channel Name:";
            // 
            // m_commNameComboBox
            // 
            this.m_commNameComboBox.FormattingEnabled = true;
            this.m_commNameComboBox.Location = new System.Drawing.Point(181, 36);
            this.m_commNameComboBox.Name = "m_commNameComboBox";
            this.m_commNameComboBox.Size = new System.Drawing.Size(164, 21);
            this.m_commNameComboBox.TabIndex = 2;
            this.m_commNameComboBox.SelectedIndexChanged += new System.EventHandler(this.m_commNameComboBox_SelectedIndexChanged);
            // 
            // m_deviceComboBox
            // 
            this.m_deviceComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_deviceComboBox.FormattingEnabled = true;
            this.m_deviceComboBox.Location = new System.Drawing.Point(181, 4);
            this.m_deviceComboBox.Name = "m_deviceComboBox";
            this.m_deviceComboBox.Size = new System.Drawing.Size(164, 21);
            this.m_deviceComboBox.TabIndex = 4;
            this.m_deviceComboBox.SelectedIndexChanged += new System.EventHandler(this.m_deviceComboBox_SelectedIndexChanged);
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(12, 283);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 5;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            this.m_closeButton.Click += new System.EventHandler(this.m_closeButton_Click);
            // 
            // m_saveSettingsButton
            // 
            this.m_saveSettingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveSettingsButton.Location = new System.Drawing.Point(306, 283);
            this.m_saveSettingsButton.Name = "m_saveSettingsButton";
            this.m_saveSettingsButton.Size = new System.Drawing.Size(89, 23);
            this.m_saveSettingsButton.TabIndex = 19;
            this.m_saveSettingsButton.Text = "Save Settings";
            this.m_saveSettingsButton.UseVisualStyleBackColor = true;
            this.m_saveSettingsButton.Click += new System.EventHandler(this.m_saveSettingsButton_Click);
            // 
            // m_removeDeviceButton
            // 
            this.m_removeDeviceButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_removeDeviceButton.Location = new System.Drawing.Point(200, 283);
            this.m_removeDeviceButton.Name = "m_removeDeviceButton";
            this.m_removeDeviceButton.Size = new System.Drawing.Size(100, 23);
            this.m_removeDeviceButton.TabIndex = 20;
            this.m_removeDeviceButton.Text = "Remove Device";
            this.m_removeDeviceButton.UseVisualStyleBackColor = true;
            this.m_removeDeviceButton.Click += new System.EventHandler(this.m_removeDeviceButton_Click);
            // 
            // m_protocolLabel
            // 
            this.m_protocolLabel.AutoSize = true;
            this.m_protocolLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_protocolLabel.Location = new System.Drawing.Point(12, 96);
            this.m_protocolLabel.Name = "m_protocolLabel";
            this.m_protocolLabel.Size = new System.Drawing.Size(49, 13);
            this.m_protocolLabel.TabIndex = 21;
            this.m_protocolLabel.Text = "Protocol:";
            // 
            // m_protocolComboBox
            // 
            this.m_protocolComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_protocolComboBox.FormattingEnabled = true;
            this.m_protocolComboBox.Location = new System.Drawing.Point(93, 93);
            this.m_protocolComboBox.Name = "m_protocolComboBox";
            this.m_protocolComboBox.Size = new System.Drawing.Size(94, 21);
            this.m_protocolComboBox.TabIndex = 22;
            this.m_protocolComboBox.SelectedIndexChanged += new System.EventHandler(this.m_protocolComboBox_SelectedIndexChanged);
            // 
            // m_channelLabel
            // 
            this.m_channelLabel.AutoSize = true;
            this.m_channelLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_channelLabel.Location = new System.Drawing.Point(210, 96);
            this.m_channelLabel.Name = "m_channelLabel";
            this.m_channelLabel.Size = new System.Drawing.Size(49, 13);
            this.m_channelLabel.TabIndex = 23;
            this.m_channelLabel.Text = "Channel:";
            // 
            // m_baudRateLabel
            // 
            this.m_baudRateLabel.AutoSize = true;
            this.m_baudRateLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_baudRateLabel.Location = new System.Drawing.Point(198, 145);
            this.m_baudRateLabel.Name = "m_baudRateLabel";
            this.m_baudRateLabel.Size = new System.Drawing.Size(61, 13);
            this.m_baudRateLabel.TabIndex = 24;
            this.m_baudRateLabel.Text = "Baud Rate:";
            // 
            // m_connectFlagLabel
            // 
            this.m_connectFlagLabel.AutoSize = true;
            this.m_connectFlagLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_connectFlagLabel.Location = new System.Drawing.Point(14, 145);
            this.m_connectFlagLabel.Name = "m_connectFlagLabel";
            this.m_connectFlagLabel.Size = new System.Drawing.Size(73, 13);
            this.m_connectFlagLabel.TabIndex = 25;
            this.m_connectFlagLabel.Text = "Connect Flag:";
            // 
            // m_channelComboBox
            // 
            this.m_channelComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_channelComboBox.DropDownWidth = 94;
            this.m_channelComboBox.FormattingEnabled = true;
            this.m_channelComboBox.Location = new System.Drawing.Point(274, 93);
            this.m_channelComboBox.Name = "m_channelComboBox";
            this.m_channelComboBox.Size = new System.Drawing.Size(94, 21);
            this.m_channelComboBox.TabIndex = 26;
            // 
            // m_baudRateComboBox
            // 
            this.m_baudRateComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_baudRateComboBox.DropDownWidth = 94;
            this.m_baudRateComboBox.FormattingEnabled = true;
            this.m_baudRateComboBox.Location = new System.Drawing.Point(274, 142);
            this.m_baudRateComboBox.Name = "m_baudRateComboBox";
            this.m_baudRateComboBox.Size = new System.Drawing.Size(94, 21);
            this.m_baudRateComboBox.TabIndex = 27;
            // 
            // m_connectFlagComboBox
            // 
            this.m_connectFlagComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_connectFlagComboBox.FormattingEnabled = true;
            this.m_connectFlagComboBox.Location = new System.Drawing.Point(93, 142);
            this.m_connectFlagComboBox.Name = "m_connectFlagComboBox";
            this.m_connectFlagComboBox.Size = new System.Drawing.Size(94, 21);
            this.m_connectFlagComboBox.TabIndex = 28;
            // 
            // m_deviceLabel
            // 
            this.m_deviceLabel.AutoSize = true;
            this.m_deviceLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_deviceLabel.Location = new System.Drawing.Point(12, 9);
            this.m_deviceLabel.Name = "m_deviceLabel";
            this.m_deviceLabel.Size = new System.Drawing.Size(160, 16);
            this.m_deviceLabel.TabIndex = 29;
            this.m_deviceLabel.Text = "Vehicle Comm Device";
            // 
            // VehicleCommSettingsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(407, 318);
            this.ControlBox = false;
            this.Controls.Add(this.m_deviceLabel);
            this.Controls.Add(this.m_connectFlagComboBox);
            this.Controls.Add(this.m_baudRateComboBox);
            this.Controls.Add(this.m_channelComboBox);
            this.Controls.Add(this.m_connectFlagLabel);
            this.Controls.Add(this.m_baudRateLabel);
            this.Controls.Add(this.m_channelLabel);
            this.Controls.Add(this.m_protocolComboBox);
            this.Controls.Add(this.m_protocolLabel);
            this.Controls.Add(this.m_removeDeviceButton);
            this.Controls.Add(this.m_saveSettingsButton);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_deviceComboBox);
            this.Controls.Add(this.m_commNameComboBox);
            this.Controls.Add(this.m_commNameLabel);
            this.Controls.Add(this.m_settingsLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "VehicleCommSettingsForm";
            this.Text = "Vehicle Communication Settings";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Label m_settingsLabel;
        private System.Windows.Forms.Label m_commNameLabel;
        private System.Windows.Forms.ComboBox m_commNameComboBox;
        private System.Windows.Forms.ComboBox m_deviceComboBox;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Button m_saveSettingsButton;
        private System.Windows.Forms.Button m_removeDeviceButton;
        private System.Windows.Forms.Label m_protocolLabel;
        private System.Windows.Forms.ComboBox m_protocolComboBox;
        private System.Windows.Forms.Label m_channelLabel;
        private System.Windows.Forms.Label m_baudRateLabel;
        private System.Windows.Forms.Label m_connectFlagLabel;
        private System.Windows.Forms.ComboBox m_channelComboBox;
        private System.Windows.Forms.ComboBox m_baudRateComboBox;
        private System.Windows.Forms.ComboBox m_connectFlagComboBox;
        private System.Windows.Forms.Label m_deviceLabel;
    }
}

