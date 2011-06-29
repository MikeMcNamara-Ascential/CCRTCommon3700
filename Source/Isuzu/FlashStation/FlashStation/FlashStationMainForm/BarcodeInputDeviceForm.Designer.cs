namespace FlashStationMainForm
{
    partial class BarcodeInputDeviceForm
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
            this.m_advancedButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_deviceLabel = new System.Windows.Forms.Label();
            this.m_devicesComboBox = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // m_advancedButton
            // 
            this.m_advancedButton.Location = new System.Drawing.Point(190, 12);
            this.m_advancedButton.Name = "m_advancedButton";
            this.m_advancedButton.Size = new System.Drawing.Size(75, 23);
            this.m_advancedButton.TabIndex = 13;
            this.m_advancedButton.Text = "Advanced";
            this.m_advancedButton.UseVisualStyleBackColor = true;
            this.m_advancedButton.Click += new System.EventHandler(this.m_advancedButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Location = new System.Drawing.Point(190, 94);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 12;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            this.m_cancelButton.Click += new System.EventHandler(this.m_cancelButton_Click);
            // 
            // m_saveButton
            // 
            this.m_saveButton.Location = new System.Drawing.Point(109, 94);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 11;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_deviceLabel
            // 
            this.m_deviceLabel.AutoSize = true;
            this.m_deviceLabel.Location = new System.Drawing.Point(16, 17);
            this.m_deviceLabel.Name = "m_deviceLabel";
            this.m_deviceLabel.Size = new System.Drawing.Size(41, 13);
            this.m_deviceLabel.TabIndex = 9;
            this.m_deviceLabel.Text = "Device";
            // 
            // m_devicesComboBox
            // 
            this.m_devicesComboBox.FormattingEnabled = true;
            this.m_devicesComboBox.Location = new System.Drawing.Point(63, 14);
            this.m_devicesComboBox.Name = "m_devicesComboBox";
            this.m_devicesComboBox.Size = new System.Drawing.Size(121, 21);
            this.m_devicesComboBox.TabIndex = 7;
            // 
            // BarcodeInputDeviceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(277, 121);
            this.Controls.Add(this.m_advancedButton);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_deviceLabel);
            this.Controls.Add(this.m_devicesComboBox);
            this.Name = "BarcodeInputDeviceForm";
            this.Text = "BarcodeInputDeviceForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_advancedButton;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Label m_deviceLabel;
        private System.Windows.Forms.ComboBox m_devicesComboBox;
    }
}