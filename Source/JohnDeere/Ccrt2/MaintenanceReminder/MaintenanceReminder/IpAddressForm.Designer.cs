namespace MaintenanceReminder
{
    partial class IpAddressForm
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
            this.m_ipAddressLabel = new System.Windows.Forms.Label();
            this.m_ipAddressTextBox = new System.Windows.Forms.TextBox();
            this.m_errorLabel = new System.Windows.Forms.Label();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_ipAddressLabel
            // 
            this.m_ipAddressLabel.AutoSize = true;
            this.m_ipAddressLabel.Location = new System.Drawing.Point(40, 38);
            this.m_ipAddressLabel.Name = "m_ipAddressLabel";
            this.m_ipAddressLabel.Size = new System.Drawing.Size(119, 13);
            this.m_ipAddressLabel.TabIndex = 0;
            this.m_ipAddressLabel.Text = "QNX CCRT IP Address:";
            // 
            // m_ipAddressTextBox
            // 
            this.m_ipAddressTextBox.Location = new System.Drawing.Point(194, 35);
            this.m_ipAddressTextBox.Name = "m_ipAddressTextBox";
            this.m_ipAddressTextBox.Size = new System.Drawing.Size(99, 20);
            this.m_ipAddressTextBox.TabIndex = 1;
            this.m_ipAddressTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // m_errorLabel
            // 
            this.m_errorLabel.AutoSize = true;
            this.m_errorLabel.ForeColor = System.Drawing.Color.Red;
            this.m_errorLabel.Location = new System.Drawing.Point(153, 9);
            this.m_errorLabel.Name = "m_errorLabel";
            this.m_errorLabel.Size = new System.Drawing.Size(35, 13);
            this.m_errorLabel.TabIndex = 2;
            this.m_errorLabel.Text = "label1";
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_saveButton.Location = new System.Drawing.Point(296, 77);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 3;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(215, 77);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 4;
            this.m_cancelButton.Text = "Close";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // IpAddressForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(383, 109);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_errorLabel);
            this.Controls.Add(this.m_ipAddressTextBox);
            this.Controls.Add(this.m_ipAddressLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "IpAddressForm";
            this.Text = "IpAddressForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_ipAddressLabel;
        private System.Windows.Forms.TextBox m_ipAddressTextBox;
        private System.Windows.Forms.Label m_errorLabel;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Button m_cancelButton;
    }
}