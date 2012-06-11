namespace SerialPortTester
{
    partial class SerialPortTester
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
            this.m_portConfigLabel = new System.Windows.Forms.Label();
            this.m_serialPortLabel = new System.Windows.Forms.Label();
            this.m_baudeRateLabel = new System.Windows.Forms.Label();
            this.m_parityLabel = new System.Windows.Forms.Label();
            this.m_dataBitsLabel = new System.Windows.Forms.Label();
            this.m_stopBitsLabel = new System.Windows.Forms.Label();
            this.m_rxTimeoutLabel = new System.Windows.Forms.Label();
            this.m_txTimeoutLabel = new System.Windows.Forms.Label();
            this.m_monitorPortCheckBox = new System.Windows.Forms.CheckBox();
            this.m_txDataLabel = new System.Windows.Forms.Label();
            this.m_serialPortComboBox = new System.Windows.Forms.ComboBox();
            this.m_baudRateTextBox = new System.Windows.Forms.TextBox();
            this.m_parityComboBox = new System.Windows.Forms.ComboBox();
            this.m_dataBitsTextBox = new System.Windows.Forms.TextBox();
            this.m_stopBitsComboBox = new System.Windows.Forms.ComboBox();
            this.m_rxTimeoutTextBox = new System.Windows.Forms.TextBox();
            this.m_txTimeoutTextBox = new System.Windows.Forms.TextBox();
            this.m_txDataTextBox = new System.Windows.Forms.TextBox();
            this.m_openPortButton = new System.Windows.Forms.Button();
            this.m_closePortButton = new System.Windows.Forms.Button();
            this.m_viewPortDataButton = new System.Windows.Forms.Button();
            this.m_updatePortButton = new System.Windows.Forms.Button();
            this.m_txDataButton = new System.Windows.Forms.Button();
            this.m_exitButton = new System.Windows.Forms.Button();
            this.m_readPortButton = new System.Windows.Forms.Button();
            this.m_rxDataLabel = new System.Windows.Forms.Label();
            this.m_rxdataTextBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // m_portConfigLabel
            // 
            this.m_portConfigLabel.AutoSize = true;
            this.m_portConfigLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_portConfigLabel.Location = new System.Drawing.Point(12, 9);
            this.m_portConfigLabel.Name = "m_portConfigLabel";
            this.m_portConfigLabel.Size = new System.Drawing.Size(135, 16);
            this.m_portConfigLabel.TabIndex = 0;
            this.m_portConfigLabel.Text = "Port Configuration:";
            // 
            // m_serialPortLabel
            // 
            this.m_serialPortLabel.AutoSize = true;
            this.m_serialPortLabel.Location = new System.Drawing.Point(45, 38);
            this.m_serialPortLabel.Name = "m_serialPortLabel";
            this.m_serialPortLabel.Size = new System.Drawing.Size(58, 13);
            this.m_serialPortLabel.TabIndex = 1;
            this.m_serialPortLabel.Text = "Serial Port:";
            // 
            // m_baudeRateLabel
            // 
            this.m_baudeRateLabel.AutoSize = true;
            this.m_baudeRateLabel.Location = new System.Drawing.Point(42, 68);
            this.m_baudeRateLabel.Name = "m_baudeRateLabel";
            this.m_baudeRateLabel.Size = new System.Drawing.Size(61, 13);
            this.m_baudeRateLabel.TabIndex = 2;
            this.m_baudeRateLabel.Text = "Baud Rate:";
            // 
            // m_parityLabel
            // 
            this.m_parityLabel.AutoSize = true;
            this.m_parityLabel.Location = new System.Drawing.Point(67, 98);
            this.m_parityLabel.Name = "m_parityLabel";
            this.m_parityLabel.Size = new System.Drawing.Size(36, 13);
            this.m_parityLabel.TabIndex = 3;
            this.m_parityLabel.Text = "Parity:";
            // 
            // m_dataBitsLabel
            // 
            this.m_dataBitsLabel.AutoSize = true;
            this.m_dataBitsLabel.Location = new System.Drawing.Point(50, 128);
            this.m_dataBitsLabel.Name = "m_dataBitsLabel";
            this.m_dataBitsLabel.Size = new System.Drawing.Size(53, 13);
            this.m_dataBitsLabel.TabIndex = 4;
            this.m_dataBitsLabel.Text = "Data Bits:";
            // 
            // m_stopBitsLabel
            // 
            this.m_stopBitsLabel.AutoSize = true;
            this.m_stopBitsLabel.Location = new System.Drawing.Point(51, 158);
            this.m_stopBitsLabel.Name = "m_stopBitsLabel";
            this.m_stopBitsLabel.Size = new System.Drawing.Size(52, 13);
            this.m_stopBitsLabel.TabIndex = 5;
            this.m_stopBitsLabel.Text = "Stop Bits:";
            // 
            // m_rxTimeoutLabel
            // 
            this.m_rxTimeoutLabel.AutoSize = true;
            this.m_rxTimeoutLabel.Location = new System.Drawing.Point(209, 38);
            this.m_rxTimeoutLabel.Name = "m_rxTimeoutLabel";
            this.m_rxTimeoutLabel.Size = new System.Drawing.Size(77, 13);
            this.m_rxTimeoutLabel.TabIndex = 6;
            this.m_rxTimeoutLabel.Text = "Read Timeout:";
            // 
            // m_txTimeoutLabel
            // 
            this.m_txTimeoutLabel.AutoSize = true;
            this.m_txTimeoutLabel.Location = new System.Drawing.Point(210, 68);
            this.m_txTimeoutLabel.Name = "m_txTimeoutLabel";
            this.m_txTimeoutLabel.Size = new System.Drawing.Size(76, 13);
            this.m_txTimeoutLabel.TabIndex = 7;
            this.m_txTimeoutLabel.Text = "Write Timeout:";
            // 
            // m_monitorPortCheckBox
            // 
            this.m_monitorPortCheckBox.AutoSize = true;
            this.m_monitorPortCheckBox.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.m_monitorPortCheckBox.Location = new System.Drawing.Point(212, 127);
            this.m_monitorPortCheckBox.Name = "m_monitorPortCheckBox";
            this.m_monitorPortCheckBox.Size = new System.Drawing.Size(109, 17);
            this.m_monitorPortCheckBox.TabIndex = 8;
            this.m_monitorPortCheckBox.Text = "Monitor Port Data";
            this.m_monitorPortCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_txDataLabel
            // 
            this.m_txDataLabel.AutoSize = true;
            this.m_txDataLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_txDataLabel.Location = new System.Drawing.Point(12, 192);
            this.m_txDataLabel.Name = "m_txDataLabel";
            this.m_txDataLabel.Size = new System.Drawing.Size(109, 16);
            this.m_txDataLabel.TabIndex = 9;
            this.m_txDataLabel.Text = "Transmit Data:";
            // 
            // m_serialPortComboBox
            // 
            this.m_serialPortComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_serialPortComboBox.FormattingEnabled = true;
            this.m_serialPortComboBox.Location = new System.Drawing.Point(109, 35);
            this.m_serialPortComboBox.Name = "m_serialPortComboBox";
            this.m_serialPortComboBox.Size = new System.Drawing.Size(75, 21);
            this.m_serialPortComboBox.TabIndex = 10;
            // 
            // m_baudRateTextBox
            // 
            this.m_baudRateTextBox.Location = new System.Drawing.Point(109, 65);
            this.m_baudRateTextBox.Name = "m_baudRateTextBox";
            this.m_baudRateTextBox.Size = new System.Drawing.Size(75, 20);
            this.m_baudRateTextBox.TabIndex = 11;
            // 
            // m_parityComboBox
            // 
            this.m_parityComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_parityComboBox.FormattingEnabled = true;
            this.m_parityComboBox.Items.AddRange(new object[] {
            "Even",
            "None",
            "Odd"});
            this.m_parityComboBox.Location = new System.Drawing.Point(109, 95);
            this.m_parityComboBox.Name = "m_parityComboBox";
            this.m_parityComboBox.Size = new System.Drawing.Size(75, 21);
            this.m_parityComboBox.TabIndex = 12;
            // 
            // m_dataBitsTextBox
            // 
            this.m_dataBitsTextBox.Location = new System.Drawing.Point(109, 125);
            this.m_dataBitsTextBox.Name = "m_dataBitsTextBox";
            this.m_dataBitsTextBox.Size = new System.Drawing.Size(75, 20);
            this.m_dataBitsTextBox.TabIndex = 13;
            // 
            // m_stopBitsComboBox
            // 
            this.m_stopBitsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_stopBitsComboBox.FormattingEnabled = true;
            this.m_stopBitsComboBox.Items.AddRange(new object[] {
            "1",
            "1.5",
            "2"});
            this.m_stopBitsComboBox.Location = new System.Drawing.Point(109, 155);
            this.m_stopBitsComboBox.Name = "m_stopBitsComboBox";
            this.m_stopBitsComboBox.Size = new System.Drawing.Size(75, 21);
            this.m_stopBitsComboBox.TabIndex = 14;
            // 
            // m_rxTimeoutTextBox
            // 
            this.m_rxTimeoutTextBox.Location = new System.Drawing.Point(292, 35);
            this.m_rxTimeoutTextBox.Name = "m_rxTimeoutTextBox";
            this.m_rxTimeoutTextBox.Size = new System.Drawing.Size(62, 20);
            this.m_rxTimeoutTextBox.TabIndex = 15;
            // 
            // m_txTimeoutTextBox
            // 
            this.m_txTimeoutTextBox.Location = new System.Drawing.Point(292, 65);
            this.m_txTimeoutTextBox.Name = "m_txTimeoutTextBox";
            this.m_txTimeoutTextBox.Size = new System.Drawing.Size(62, 20);
            this.m_txTimeoutTextBox.TabIndex = 16;
            // 
            // m_txDataTextBox
            // 
            this.m_txDataTextBox.Location = new System.Drawing.Point(15, 211);
            this.m_txDataTextBox.Name = "m_txDataTextBox";
            this.m_txDataTextBox.Size = new System.Drawing.Size(373, 20);
            this.m_txDataTextBox.TabIndex = 17;
            // 
            // m_openPortButton
            // 
            this.m_openPortButton.Location = new System.Drawing.Point(394, 33);
            this.m_openPortButton.Name = "m_openPortButton";
            this.m_openPortButton.Size = new System.Drawing.Size(113, 23);
            this.m_openPortButton.TabIndex = 18;
            this.m_openPortButton.Text = "Open Port";
            this.m_openPortButton.UseVisualStyleBackColor = true;
            this.m_openPortButton.Click += new System.EventHandler(this.m_openPortButton_Click);
            // 
            // m_closePortButton
            // 
            this.m_closePortButton.Location = new System.Drawing.Point(394, 63);
            this.m_closePortButton.Name = "m_closePortButton";
            this.m_closePortButton.Size = new System.Drawing.Size(113, 23);
            this.m_closePortButton.TabIndex = 19;
            this.m_closePortButton.Text = "Close Port";
            this.m_closePortButton.UseVisualStyleBackColor = true;
            this.m_closePortButton.Click += new System.EventHandler(this.m_closePortButton_Click);
            // 
            // m_viewPortDataButton
            // 
            this.m_viewPortDataButton.Location = new System.Drawing.Point(394, 123);
            this.m_viewPortDataButton.Name = "m_viewPortDataButton";
            this.m_viewPortDataButton.Size = new System.Drawing.Size(113, 23);
            this.m_viewPortDataButton.TabIndex = 20;
            this.m_viewPortDataButton.Text = "View Port Data";
            this.m_viewPortDataButton.UseVisualStyleBackColor = true;
            this.m_viewPortDataButton.Click += new System.EventHandler(this.m_viewPortDataButton_Click);
            // 
            // m_updatePortButton
            // 
            this.m_updatePortButton.Location = new System.Drawing.Point(394, 93);
            this.m_updatePortButton.Name = "m_updatePortButton";
            this.m_updatePortButton.Size = new System.Drawing.Size(113, 23);
            this.m_updatePortButton.TabIndex = 21;
            this.m_updatePortButton.Text = "Update Port Config";
            this.m_updatePortButton.UseVisualStyleBackColor = true;
            this.m_updatePortButton.Click += new System.EventHandler(this.m_updatePortButton_Click);
            // 
            // m_txDataButton
            // 
            this.m_txDataButton.Location = new System.Drawing.Point(394, 211);
            this.m_txDataButton.Name = "m_txDataButton";
            this.m_txDataButton.Size = new System.Drawing.Size(113, 20);
            this.m_txDataButton.TabIndex = 22;
            this.m_txDataButton.Text = "Send Message";
            this.m_txDataButton.UseVisualStyleBackColor = true;
            this.m_txDataButton.Click += new System.EventHandler(this.m_txDataButton_Click);
            // 
            // m_exitButton
            // 
            this.m_exitButton.Location = new System.Drawing.Point(432, 305);
            this.m_exitButton.Name = "m_exitButton";
            this.m_exitButton.Size = new System.Drawing.Size(75, 23);
            this.m_exitButton.TabIndex = 23;
            this.m_exitButton.Text = "Exit";
            this.m_exitButton.UseVisualStyleBackColor = true;
            this.m_exitButton.Click += new System.EventHandler(this.m_exitButton_Click);
            // 
            // m_readPortButton
            // 
            this.m_readPortButton.Location = new System.Drawing.Point(394, 262);
            this.m_readPortButton.Name = "m_readPortButton";
            this.m_readPortButton.Size = new System.Drawing.Size(113, 20);
            this.m_readPortButton.TabIndex = 24;
            this.m_readPortButton.Text = "Read Port";
            this.m_readPortButton.UseVisualStyleBackColor = true;
            this.m_readPortButton.Click += new System.EventHandler(this.m_readPortButton_Click);
            // 
            // m_rxDataLabel
            // 
            this.m_rxDataLabel.AutoSize = true;
            this.m_rxDataLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_rxDataLabel.Location = new System.Drawing.Point(12, 243);
            this.m_rxDataLabel.Name = "m_rxDataLabel";
            this.m_rxDataLabel.Size = new System.Drawing.Size(116, 16);
            this.m_rxDataLabel.TabIndex = 25;
            this.m_rxDataLabel.Text = "Received Data:";
            // 
            // m_rxdataTextBox
            // 
            this.m_rxdataTextBox.BackColor = System.Drawing.Color.Wheat;
            this.m_rxdataTextBox.Location = new System.Drawing.Point(15, 262);
            this.m_rxdataTextBox.Name = "m_rxdataTextBox";
            this.m_rxdataTextBox.ReadOnly = true;
            this.m_rxdataTextBox.Size = new System.Drawing.Size(373, 20);
            this.m_rxdataTextBox.TabIndex = 26;
            // 
            // SerialPortTester
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(519, 340);
            this.Controls.Add(this.m_rxdataTextBox);
            this.Controls.Add(this.m_rxDataLabel);
            this.Controls.Add(this.m_readPortButton);
            this.Controls.Add(this.m_exitButton);
            this.Controls.Add(this.m_txDataButton);
            this.Controls.Add(this.m_updatePortButton);
            this.Controls.Add(this.m_viewPortDataButton);
            this.Controls.Add(this.m_closePortButton);
            this.Controls.Add(this.m_openPortButton);
            this.Controls.Add(this.m_txDataTextBox);
            this.Controls.Add(this.m_txTimeoutTextBox);
            this.Controls.Add(this.m_rxTimeoutTextBox);
            this.Controls.Add(this.m_stopBitsComboBox);
            this.Controls.Add(this.m_dataBitsTextBox);
            this.Controls.Add(this.m_parityComboBox);
            this.Controls.Add(this.m_baudRateTextBox);
            this.Controls.Add(this.m_serialPortComboBox);
            this.Controls.Add(this.m_txDataLabel);
            this.Controls.Add(this.m_monitorPortCheckBox);
            this.Controls.Add(this.m_txTimeoutLabel);
            this.Controls.Add(this.m_rxTimeoutLabel);
            this.Controls.Add(this.m_stopBitsLabel);
            this.Controls.Add(this.m_dataBitsLabel);
            this.Controls.Add(this.m_parityLabel);
            this.Controls.Add(this.m_baudeRateLabel);
            this.Controls.Add(this.m_serialPortLabel);
            this.Controls.Add(this.m_portConfigLabel);
            this.Name = "SerialPortTester";
            this.Text = "Serial Port Tester";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_portConfigLabel;
        private System.Windows.Forms.Label m_serialPortLabel;
        private System.Windows.Forms.Label m_baudeRateLabel;
        private System.Windows.Forms.Label m_parityLabel;
        private System.Windows.Forms.Label m_dataBitsLabel;
        private System.Windows.Forms.Label m_stopBitsLabel;
        private System.Windows.Forms.Label m_rxTimeoutLabel;
        private System.Windows.Forms.Label m_txTimeoutLabel;
        private System.Windows.Forms.CheckBox m_monitorPortCheckBox;
        private System.Windows.Forms.Label m_txDataLabel;
        private System.Windows.Forms.ComboBox m_serialPortComboBox;
        private System.Windows.Forms.TextBox m_baudRateTextBox;
        private System.Windows.Forms.ComboBox m_parityComboBox;
        private System.Windows.Forms.TextBox m_dataBitsTextBox;
        private System.Windows.Forms.ComboBox m_stopBitsComboBox;
        private System.Windows.Forms.TextBox m_rxTimeoutTextBox;
        private System.Windows.Forms.TextBox m_txTimeoutTextBox;
        private System.Windows.Forms.TextBox m_txDataTextBox;
        private System.Windows.Forms.Button m_openPortButton;
        private System.Windows.Forms.Button m_closePortButton;
        private System.Windows.Forms.Button m_viewPortDataButton;
        private System.Windows.Forms.Button m_updatePortButton;
        private System.Windows.Forms.Button m_txDataButton;
        private System.Windows.Forms.Button m_exitButton;
        private System.Windows.Forms.Button m_readPortButton;
        private System.Windows.Forms.Label m_rxDataLabel;
        private System.Windows.Forms.TextBox m_rxdataTextBox;
    }
}

