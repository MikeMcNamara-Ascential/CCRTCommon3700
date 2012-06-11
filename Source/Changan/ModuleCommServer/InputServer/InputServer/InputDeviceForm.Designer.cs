namespace InputServer
{
    partial class InputDeviceForm
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
            this.m_portSettingsLabel = new System.Windows.Forms.Label();
            this.m_inputDeviceLabel = new System.Windows.Forms.Label();
            this.m_inputDeviceComboBox = new System.Windows.Forms.ComboBox();
            this.m_portLabel = new System.Windows.Forms.Label();
            this.m_portComboBox = new System.Windows.Forms.ComboBox();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_baudRateLabel = new System.Windows.Forms.Label();
            this.m_baudRateTextBox = new System.Windows.Forms.TextBox();
            this.m_parityLabel = new System.Windows.Forms.Label();
            this.m_parityComboBox = new System.Windows.Forms.ComboBox();
            this.m_dataBitsLabel = new System.Windows.Forms.Label();
            this.m_dataBitsTextBox = new System.Windows.Forms.TextBox();
            this.m_stopBitsLabel = new System.Windows.Forms.Label();
            this.m_stopBitsComboBox = new System.Windows.Forms.ComboBox();
            this.m_rxTimeoutLabel = new System.Windows.Forms.Label();
            this.m_txTimeoutLabel = new System.Windows.Forms.Label();
            this.m_rxTimeoutTextBox = new System.Windows.Forms.TextBox();
            this.m_txTimeoutTextBox = new System.Windows.Forms.TextBox();
            this.m_monitorDataCheckBox = new System.Windows.Forms.CheckBox();
            this.m_saveSettingsButton = new System.Windows.Forms.Button();
            this.m_removeDeviceButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_portSettingsLabel
            // 
            this.m_portSettingsLabel.AutoSize = true;
            this.m_portSettingsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_portSettingsLabel.Location = new System.Drawing.Point(12, 65);
            this.m_portSettingsLabel.Name = "m_portSettingsLabel";
            this.m_portSettingsLabel.Size = new System.Drawing.Size(100, 16);
            this.m_portSettingsLabel.TabIndex = 0;
            this.m_portSettingsLabel.Text = "Port Settings:";
            // 
            // m_inputDeviceLabel
            // 
            this.m_inputDeviceLabel.AutoSize = true;
            this.m_inputDeviceLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_inputDeviceLabel.Location = new System.Drawing.Point(12, 9);
            this.m_inputDeviceLabel.Name = "m_inputDeviceLabel";
            this.m_inputDeviceLabel.Size = new System.Drawing.Size(98, 16);
            this.m_inputDeviceLabel.TabIndex = 1;
            this.m_inputDeviceLabel.Text = "Input Device:";
            // 
            // m_inputDeviceComboBox
            // 
            this.m_inputDeviceComboBox.FormattingEnabled = true;
            this.m_inputDeviceComboBox.Location = new System.Drawing.Point(116, 8);
            this.m_inputDeviceComboBox.Name = "m_inputDeviceComboBox";
            this.m_inputDeviceComboBox.Size = new System.Drawing.Size(164, 21);
            this.m_inputDeviceComboBox.TabIndex = 2;
            this.m_inputDeviceComboBox.SelectedIndexChanged += new System.EventHandler(this.m_inputDeviceComboBox_SelectedIndexChanged);
            // 
            // m_portLabel
            // 
            this.m_portLabel.AutoSize = true;
            this.m_portLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_portLabel.Location = new System.Drawing.Point(81, 96);
            this.m_portLabel.Name = "m_portLabel";
            this.m_portLabel.Size = new System.Drawing.Size(29, 13);
            this.m_portLabel.TabIndex = 3;
            this.m_portLabel.Text = "Port:";
            // 
            // m_portComboBox
            // 
            this.m_portComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_portComboBox.FormattingEnabled = true;
            this.m_portComboBox.Location = new System.Drawing.Point(116, 93);
            this.m_portComboBox.Name = "m_portComboBox";
            this.m_portComboBox.Size = new System.Drawing.Size(71, 21);
            this.m_portComboBox.TabIndex = 4;
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
            // 
            // m_baudRateLabel
            // 
            this.m_baudRateLabel.AutoSize = true;
            this.m_baudRateLabel.Location = new System.Drawing.Point(51, 131);
            this.m_baudRateLabel.Name = "m_baudRateLabel";
            this.m_baudRateLabel.Size = new System.Drawing.Size(61, 13);
            this.m_baudRateLabel.TabIndex = 6;
            this.m_baudRateLabel.Text = "Baud Rate:";
            // 
            // m_baudRateTextBox
            // 
            this.m_baudRateTextBox.Location = new System.Drawing.Point(118, 128);
            this.m_baudRateTextBox.Name = "m_baudRateTextBox";
            this.m_baudRateTextBox.Size = new System.Drawing.Size(69, 20);
            this.m_baudRateTextBox.TabIndex = 7;
            // 
            // m_parityLabel
            // 
            this.m_parityLabel.AutoSize = true;
            this.m_parityLabel.Location = new System.Drawing.Point(74, 164);
            this.m_parityLabel.Name = "m_parityLabel";
            this.m_parityLabel.Size = new System.Drawing.Size(36, 13);
            this.m_parityLabel.TabIndex = 8;
            this.m_parityLabel.Text = "Parity:";
            // 
            // m_parityComboBox
            // 
            this.m_parityComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_parityComboBox.FormattingEnabled = true;
            this.m_parityComboBox.Items.AddRange(new object[] {
            "Even",
            "None",
            "Odd"});
            this.m_parityComboBox.Location = new System.Drawing.Point(118, 161);
            this.m_parityComboBox.Name = "m_parityComboBox";
            this.m_parityComboBox.Size = new System.Drawing.Size(69, 21);
            this.m_parityComboBox.TabIndex = 9;
            // 
            // m_dataBitsLabel
            // 
            this.m_dataBitsLabel.AutoSize = true;
            this.m_dataBitsLabel.Location = new System.Drawing.Point(57, 202);
            this.m_dataBitsLabel.Name = "m_dataBitsLabel";
            this.m_dataBitsLabel.Size = new System.Drawing.Size(53, 13);
            this.m_dataBitsLabel.TabIndex = 10;
            this.m_dataBitsLabel.Text = "Data Bits:";
            // 
            // m_dataBitsTextBox
            // 
            this.m_dataBitsTextBox.Location = new System.Drawing.Point(118, 199);
            this.m_dataBitsTextBox.Name = "m_dataBitsTextBox";
            this.m_dataBitsTextBox.Size = new System.Drawing.Size(69, 20);
            this.m_dataBitsTextBox.TabIndex = 11;
            // 
            // m_stopBitsLabel
            // 
            this.m_stopBitsLabel.AutoSize = true;
            this.m_stopBitsLabel.Location = new System.Drawing.Point(60, 238);
            this.m_stopBitsLabel.Name = "m_stopBitsLabel";
            this.m_stopBitsLabel.Size = new System.Drawing.Size(52, 13);
            this.m_stopBitsLabel.TabIndex = 12;
            this.m_stopBitsLabel.Text = "Stop Bits:";
            // 
            // m_stopBitsComboBox
            // 
            this.m_stopBitsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_stopBitsComboBox.FormattingEnabled = true;
            this.m_stopBitsComboBox.Items.AddRange(new object[] {
            "None",
            "1",
            "1.5",
            "2"});
            this.m_stopBitsComboBox.Location = new System.Drawing.Point(118, 235);
            this.m_stopBitsComboBox.Name = "m_stopBitsComboBox";
            this.m_stopBitsComboBox.Size = new System.Drawing.Size(69, 21);
            this.m_stopBitsComboBox.TabIndex = 13;
            // 
            // m_rxTimeoutLabel
            // 
            this.m_rxTimeoutLabel.AutoSize = true;
            this.m_rxTimeoutLabel.Location = new System.Drawing.Point(233, 96);
            this.m_rxTimeoutLabel.Name = "m_rxTimeoutLabel";
            this.m_rxTimeoutLabel.Size = new System.Drawing.Size(99, 13);
            this.m_rxTimeoutLabel.TabIndex = 14;
            this.m_rxTimeoutLabel.Text = "Read Timeout (ms):";
            // 
            // m_txTimeoutLabel
            // 
            this.m_txTimeoutLabel.AutoSize = true;
            this.m_txTimeoutLabel.Location = new System.Drawing.Point(234, 131);
            this.m_txTimeoutLabel.Name = "m_txTimeoutLabel";
            this.m_txTimeoutLabel.Size = new System.Drawing.Size(98, 13);
            this.m_txTimeoutLabel.TabIndex = 15;
            this.m_txTimeoutLabel.Text = "Write Timeout (ms):";
            // 
            // m_rxTimeoutTextBox
            // 
            this.m_rxTimeoutTextBox.Location = new System.Drawing.Point(335, 93);
            this.m_rxTimeoutTextBox.Name = "m_rxTimeoutTextBox";
            this.m_rxTimeoutTextBox.Size = new System.Drawing.Size(53, 20);
            this.m_rxTimeoutTextBox.TabIndex = 16;
            // 
            // m_txTimeoutTextBox
            // 
            this.m_txTimeoutTextBox.Location = new System.Drawing.Point(335, 128);
            this.m_txTimeoutTextBox.Name = "m_txTimeoutTextBox";
            this.m_txTimeoutTextBox.Size = new System.Drawing.Size(53, 20);
            this.m_txTimeoutTextBox.TabIndex = 17;
            // 
            // m_monitorDataCheckBox
            // 
            this.m_monitorDataCheckBox.AutoSize = true;
            this.m_monitorDataCheckBox.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.m_monitorDataCheckBox.Location = new System.Drawing.Point(237, 201);
            this.m_monitorDataCheckBox.Name = "m_monitorDataCheckBox";
            this.m_monitorDataCheckBox.Size = new System.Drawing.Size(105, 17);
            this.m_monitorDataCheckBox.TabIndex = 18;
            this.m_monitorDataCheckBox.Text = "Monitor For Data";
            this.m_monitorDataCheckBox.UseVisualStyleBackColor = true;
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
            // InputDeviceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(407, 318);
            this.ControlBox = false;
            this.Controls.Add(this.m_removeDeviceButton);
            this.Controls.Add(this.m_saveSettingsButton);
            this.Controls.Add(this.m_monitorDataCheckBox);
            this.Controls.Add(this.m_txTimeoutTextBox);
            this.Controls.Add(this.m_rxTimeoutTextBox);
            this.Controls.Add(this.m_txTimeoutLabel);
            this.Controls.Add(this.m_rxTimeoutLabel);
            this.Controls.Add(this.m_stopBitsComboBox);
            this.Controls.Add(this.m_stopBitsLabel);
            this.Controls.Add(this.m_dataBitsTextBox);
            this.Controls.Add(this.m_dataBitsLabel);
            this.Controls.Add(this.m_parityComboBox);
            this.Controls.Add(this.m_parityLabel);
            this.Controls.Add(this.m_baudRateTextBox);
            this.Controls.Add(this.m_baudRateLabel);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_portComboBox);
            this.Controls.Add(this.m_portLabel);
            this.Controls.Add(this.m_inputDeviceComboBox);
            this.Controls.Add(this.m_inputDeviceLabel);
            this.Controls.Add(this.m_portSettingsLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "InputDeviceForm";
            this.Text = "Input Device Configuration";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_portSettingsLabel;
        private System.Windows.Forms.Label m_inputDeviceLabel;
        private System.Windows.Forms.ComboBox m_inputDeviceComboBox;
        private System.Windows.Forms.Label m_portLabel;
        private System.Windows.Forms.ComboBox m_portComboBox;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Label m_baudRateLabel;
        private System.Windows.Forms.TextBox m_baudRateTextBox;
        private System.Windows.Forms.Label m_parityLabel;
        private System.Windows.Forms.ComboBox m_parityComboBox;
        private System.Windows.Forms.Label m_dataBitsLabel;
        private System.Windows.Forms.TextBox m_dataBitsTextBox;
        private System.Windows.Forms.Label m_stopBitsLabel;
        private System.Windows.Forms.ComboBox m_stopBitsComboBox;
        private System.Windows.Forms.Label m_rxTimeoutLabel;
        private System.Windows.Forms.Label m_txTimeoutLabel;
        private System.Windows.Forms.TextBox m_rxTimeoutTextBox;
        private System.Windows.Forms.TextBox m_txTimeoutTextBox;
        private System.Windows.Forms.CheckBox m_monitorDataCheckBox;
        private System.Windows.Forms.Button m_saveSettingsButton;
        private System.Windows.Forms.Button m_removeDeviceButton;
    }
}