namespace SerialPortLibrary
{
    partial class PortDataForm
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
            this.m_serialPortLabel = new System.Windows.Forms.Label();
            this.m_baudRateLabel = new System.Windows.Forms.Label();
            this.m_parityLabel = new System.Windows.Forms.Label();
            this.m_dataBitsLabel = new System.Windows.Forms.Label();
            this.m_stopBitsLabel = new System.Windows.Forms.Label();
            this.m_readTimeoutLabel = new System.Windows.Forms.Label();
            this.m_writeTimeoutLabel = new System.Windows.Forms.Label();
            this.m_monitorDataCheckBox = new System.Windows.Forms.CheckBox();
            this.m_portNameTextBox = new System.Windows.Forms.TextBox();
            this.m_baudRateTextBox = new System.Windows.Forms.TextBox();
            this.m_parityTextBox = new System.Windows.Forms.TextBox();
            this.m_dataBitsTextBox = new System.Windows.Forms.TextBox();
            this.m_stopBitsTextBox = new System.Windows.Forms.TextBox();
            this.m_rxTimeoutTextBox = new System.Windows.Forms.TextBox();
            this.m_txTimeoutTextBox = new System.Windows.Forms.TextBox();
            this.m_portDataLabel = new System.Windows.Forms.Label();
            this.m_portDataTextBox = new System.Windows.Forms.TextBox();
            this.m_clearDataButton = new System.Windows.Forms.Button();
            this.m_portOpenCheckBox = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // m_portSettingsLabel
            // 
            this.m_portSettingsLabel.AutoSize = true;
            this.m_portSettingsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_portSettingsLabel.Location = new System.Drawing.Point(12, 9);
            this.m_portSettingsLabel.Name = "m_portSettingsLabel";
            this.m_portSettingsLabel.Size = new System.Drawing.Size(100, 16);
            this.m_portSettingsLabel.TabIndex = 0;
            this.m_portSettingsLabel.Text = "Port Settings:";
            // 
            // m_serialPortLabel
            // 
            this.m_serialPortLabel.AutoSize = true;
            this.m_serialPortLabel.Location = new System.Drawing.Point(68, 36);
            this.m_serialPortLabel.Name = "m_serialPortLabel";
            this.m_serialPortLabel.Size = new System.Drawing.Size(58, 13);
            this.m_serialPortLabel.TabIndex = 1;
            this.m_serialPortLabel.Text = "Serial Port:";
            // 
            // m_baudRateLabel
            // 
            this.m_baudRateLabel.AutoSize = true;
            this.m_baudRateLabel.Location = new System.Drawing.Point(65, 61);
            this.m_baudRateLabel.Name = "m_baudRateLabel";
            this.m_baudRateLabel.Size = new System.Drawing.Size(61, 13);
            this.m_baudRateLabel.TabIndex = 2;
            this.m_baudRateLabel.Text = "Baud Rate:";
            // 
            // m_parityLabel
            // 
            this.m_parityLabel.AutoSize = true;
            this.m_parityLabel.Location = new System.Drawing.Point(90, 86);
            this.m_parityLabel.Name = "m_parityLabel";
            this.m_parityLabel.Size = new System.Drawing.Size(36, 13);
            this.m_parityLabel.TabIndex = 3;
            this.m_parityLabel.Text = "Parity:";
            // 
            // m_dataBitsLabel
            // 
            this.m_dataBitsLabel.AutoSize = true;
            this.m_dataBitsLabel.Location = new System.Drawing.Point(73, 112);
            this.m_dataBitsLabel.Name = "m_dataBitsLabel";
            this.m_dataBitsLabel.Size = new System.Drawing.Size(53, 13);
            this.m_dataBitsLabel.TabIndex = 4;
            this.m_dataBitsLabel.Text = "Data Bits:";
            // 
            // m_stopBitsLabel
            // 
            this.m_stopBitsLabel.AutoSize = true;
            this.m_stopBitsLabel.Location = new System.Drawing.Point(74, 138);
            this.m_stopBitsLabel.Name = "m_stopBitsLabel";
            this.m_stopBitsLabel.Size = new System.Drawing.Size(52, 13);
            this.m_stopBitsLabel.TabIndex = 5;
            this.m_stopBitsLabel.Text = "Stop Bits:";
            // 
            // m_readTimeoutLabel
            // 
            this.m_readTimeoutLabel.AutoSize = true;
            this.m_readTimeoutLabel.Location = new System.Drawing.Point(224, 36);
            this.m_readTimeoutLabel.Name = "m_readTimeoutLabel";
            this.m_readTimeoutLabel.Size = new System.Drawing.Size(77, 13);
            this.m_readTimeoutLabel.TabIndex = 6;
            this.m_readTimeoutLabel.Text = "Read Timeout:";
            // 
            // m_writeTimeoutLabel
            // 
            this.m_writeTimeoutLabel.AutoSize = true;
            this.m_writeTimeoutLabel.Location = new System.Drawing.Point(225, 61);
            this.m_writeTimeoutLabel.Name = "m_writeTimeoutLabel";
            this.m_writeTimeoutLabel.Size = new System.Drawing.Size(76, 13);
            this.m_writeTimeoutLabel.TabIndex = 7;
            this.m_writeTimeoutLabel.Text = "Write Timeout:";
            // 
            // m_monitorDataCheckBox
            // 
            this.m_monitorDataCheckBox.AutoCheck = false;
            this.m_monitorDataCheckBox.AutoSize = true;
            this.m_monitorDataCheckBox.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.m_monitorDataCheckBox.Location = new System.Drawing.Point(227, 85);
            this.m_monitorDataCheckBox.Name = "m_monitorDataCheckBox";
            this.m_monitorDataCheckBox.Size = new System.Drawing.Size(133, 17);
            this.m_monitorDataCheckBox.TabIndex = 8;
            this.m_monitorDataCheckBox.Text = "Monitor Incoming Data";
            this.m_monitorDataCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_portNameTextBox
            // 
            this.m_portNameTextBox.BackColor = System.Drawing.Color.White;
            this.m_portNameTextBox.Location = new System.Drawing.Point(132, 33);
            this.m_portNameTextBox.Name = "m_portNameTextBox";
            this.m_portNameTextBox.ReadOnly = true;
            this.m_portNameTextBox.Size = new System.Drawing.Size(48, 20);
            this.m_portNameTextBox.TabIndex = 9;
            // 
            // m_baudRateTextBox
            // 
            this.m_baudRateTextBox.BackColor = System.Drawing.Color.White;
            this.m_baudRateTextBox.Location = new System.Drawing.Point(132, 58);
            this.m_baudRateTextBox.Name = "m_baudRateTextBox";
            this.m_baudRateTextBox.ReadOnly = true;
            this.m_baudRateTextBox.Size = new System.Drawing.Size(48, 20);
            this.m_baudRateTextBox.TabIndex = 10;
            // 
            // m_parityTextBox
            // 
            this.m_parityTextBox.BackColor = System.Drawing.Color.White;
            this.m_parityTextBox.Location = new System.Drawing.Point(132, 83);
            this.m_parityTextBox.Name = "m_parityTextBox";
            this.m_parityTextBox.ReadOnly = true;
            this.m_parityTextBox.Size = new System.Drawing.Size(48, 20);
            this.m_parityTextBox.TabIndex = 11;
            // 
            // m_dataBitsTextBox
            // 
            this.m_dataBitsTextBox.BackColor = System.Drawing.Color.White;
            this.m_dataBitsTextBox.Location = new System.Drawing.Point(132, 109);
            this.m_dataBitsTextBox.Name = "m_dataBitsTextBox";
            this.m_dataBitsTextBox.ReadOnly = true;
            this.m_dataBitsTextBox.Size = new System.Drawing.Size(48, 20);
            this.m_dataBitsTextBox.TabIndex = 12;
            // 
            // m_stopBitsTextBox
            // 
            this.m_stopBitsTextBox.BackColor = System.Drawing.Color.White;
            this.m_stopBitsTextBox.Location = new System.Drawing.Point(132, 135);
            this.m_stopBitsTextBox.Name = "m_stopBitsTextBox";
            this.m_stopBitsTextBox.ReadOnly = true;
            this.m_stopBitsTextBox.Size = new System.Drawing.Size(48, 20);
            this.m_stopBitsTextBox.TabIndex = 13;
            // 
            // m_rxTimeoutTextBox
            // 
            this.m_rxTimeoutTextBox.BackColor = System.Drawing.Color.White;
            this.m_rxTimeoutTextBox.Location = new System.Drawing.Point(312, 33);
            this.m_rxTimeoutTextBox.Name = "m_rxTimeoutTextBox";
            this.m_rxTimeoutTextBox.ReadOnly = true;
            this.m_rxTimeoutTextBox.Size = new System.Drawing.Size(48, 20);
            this.m_rxTimeoutTextBox.TabIndex = 14;
            // 
            // m_txTimeoutTextBox
            // 
            this.m_txTimeoutTextBox.BackColor = System.Drawing.Color.White;
            this.m_txTimeoutTextBox.Location = new System.Drawing.Point(312, 58);
            this.m_txTimeoutTextBox.Name = "m_txTimeoutTextBox";
            this.m_txTimeoutTextBox.ReadOnly = true;
            this.m_txTimeoutTextBox.Size = new System.Drawing.Size(48, 20);
            this.m_txTimeoutTextBox.TabIndex = 15;
            // 
            // m_portDataLabel
            // 
            this.m_portDataLabel.AutoSize = true;
            this.m_portDataLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_portDataLabel.Location = new System.Drawing.Point(12, 179);
            this.m_portDataLabel.Name = "m_portDataLabel";
            this.m_portDataLabel.Size = new System.Drawing.Size(77, 16);
            this.m_portDataLabel.TabIndex = 16;
            this.m_portDataLabel.Text = "Port Data:";
            // 
            // m_portDataTextBox
            // 
            this.m_portDataTextBox.BackColor = System.Drawing.Color.White;
            this.m_portDataTextBox.Location = new System.Drawing.Point(15, 203);
            this.m_portDataTextBox.Multiline = true;
            this.m_portDataTextBox.Name = "m_portDataTextBox";
            this.m_portDataTextBox.ReadOnly = true;
            this.m_portDataTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.m_portDataTextBox.Size = new System.Drawing.Size(364, 142);
            this.m_portDataTextBox.TabIndex = 17;
            // 
            // m_clearDataButton
            // 
            this.m_clearDataButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_clearDataButton.Location = new System.Drawing.Point(304, 177);
            this.m_clearDataButton.Name = "m_clearDataButton";
            this.m_clearDataButton.Size = new System.Drawing.Size(75, 20);
            this.m_clearDataButton.TabIndex = 18;
            this.m_clearDataButton.Text = "Clear Data";
            this.m_clearDataButton.UseVisualStyleBackColor = true;
            this.m_clearDataButton.Click += new System.EventHandler(this.m_clearDataButton_Click);
            // 
            // m_portOpenCheckBox
            // 
            this.m_portOpenCheckBox.AutoCheck = false;
            this.m_portOpenCheckBox.AutoSize = true;
            this.m_portOpenCheckBox.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.m_portOpenCheckBox.Location = new System.Drawing.Point(283, 112);
            this.m_portOpenCheckBox.Name = "m_portOpenCheckBox";
            this.m_portOpenCheckBox.Size = new System.Drawing.Size(77, 17);
            this.m_portOpenCheckBox.TabIndex = 19;
            this.m_portOpenCheckBox.Text = "Port Open:";
            this.m_portOpenCheckBox.UseVisualStyleBackColor = true;
            // 
            // PortDataForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightBlue;
            this.ClientSize = new System.Drawing.Size(391, 357);
            this.Controls.Add(this.m_portOpenCheckBox);
            this.Controls.Add(this.m_clearDataButton);
            this.Controls.Add(this.m_portDataTextBox);
            this.Controls.Add(this.m_portDataLabel);
            this.Controls.Add(this.m_txTimeoutTextBox);
            this.Controls.Add(this.m_rxTimeoutTextBox);
            this.Controls.Add(this.m_stopBitsTextBox);
            this.Controls.Add(this.m_dataBitsTextBox);
            this.Controls.Add(this.m_parityTextBox);
            this.Controls.Add(this.m_baudRateTextBox);
            this.Controls.Add(this.m_portNameTextBox);
            this.Controls.Add(this.m_monitorDataCheckBox);
            this.Controls.Add(this.m_writeTimeoutLabel);
            this.Controls.Add(this.m_readTimeoutLabel);
            this.Controls.Add(this.m_stopBitsLabel);
            this.Controls.Add(this.m_dataBitsLabel);
            this.Controls.Add(this.m_parityLabel);
            this.Controls.Add(this.m_baudRateLabel);
            this.Controls.Add(this.m_serialPortLabel);
            this.Controls.Add(this.m_portSettingsLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "PortDataForm";
            this.Text = "Serial Port Data";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_portSettingsLabel;
        private System.Windows.Forms.Label m_serialPortLabel;
        private System.Windows.Forms.Label m_baudRateLabel;
        private System.Windows.Forms.Label m_parityLabel;
        private System.Windows.Forms.Label m_dataBitsLabel;
        private System.Windows.Forms.Label m_stopBitsLabel;
        private System.Windows.Forms.Label m_readTimeoutLabel;
        private System.Windows.Forms.Label m_writeTimeoutLabel;
        private System.Windows.Forms.CheckBox m_monitorDataCheckBox;
        private System.Windows.Forms.TextBox m_portNameTextBox;
        private System.Windows.Forms.TextBox m_baudRateTextBox;
        private System.Windows.Forms.TextBox m_parityTextBox;
        private System.Windows.Forms.TextBox m_dataBitsTextBox;
        private System.Windows.Forms.TextBox m_stopBitsTextBox;
        private System.Windows.Forms.TextBox m_rxTimeoutTextBox;
        private System.Windows.Forms.TextBox m_txTimeoutTextBox;
        private System.Windows.Forms.Label m_portDataLabel;
        private System.Windows.Forms.TextBox m_portDataTextBox;
        private System.Windows.Forms.Button m_clearDataButton;
        private System.Windows.Forms.CheckBox m_portOpenCheckBox;
    }
}