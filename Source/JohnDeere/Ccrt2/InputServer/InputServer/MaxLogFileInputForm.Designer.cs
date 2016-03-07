namespace InputServer
{
    partial class MaxLogFileInputForm
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
            this.m_maxCountLabel = new System.Windows.Forms.Label();
            this.m_maxLogFileCountTextBox = new System.Windows.Forms.TextBox();
            this.m_okButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_maxCountLabel
            // 
            this.m_maxCountLabel.AutoSize = true;
            this.m_maxCountLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_maxCountLabel.Location = new System.Drawing.Point(12, 9);
            this.m_maxCountLabel.Name = "m_maxCountLabel";
            this.m_maxCountLabel.Size = new System.Drawing.Size(151, 13);
            this.m_maxCountLabel.TabIndex = 0;
            this.m_maxCountLabel.Text = "Maximum Number of Log Files:";
            // 
            // m_maxLogFileCountTextBox
            // 
            this.m_maxLogFileCountTextBox.Location = new System.Drawing.Point(204, 6);
            this.m_maxLogFileCountTextBox.Name = "m_maxLogFileCountTextBox";
            this.m_maxLogFileCountTextBox.Size = new System.Drawing.Size(50, 20);
            this.m_maxLogFileCountTextBox.TabIndex = 1;
            // 
            // m_okButton
            // 
            this.m_okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_okButton.Location = new System.Drawing.Point(204, 46);
            this.m_okButton.Name = "m_okButton";
            this.m_okButton.Size = new System.Drawing.Size(75, 23);
            this.m_okButton.TabIndex = 2;
            this.m_okButton.Text = "Save";
            this.m_okButton.UseVisualStyleBackColor = true;
            this.m_okButton.Click += new System.EventHandler(this.m_okButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(38, 46);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 3;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // MaxLogFileInputForm
            // 
            this.AcceptButton = this.m_okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(318, 81);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_okButton);
            this.Controls.Add(this.m_maxLogFileCountTextBox);
            this.Controls.Add(this.m_maxCountLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "MaxLogFileInputForm";
            this.Text = "Maximum Log File Count";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_maxCountLabel;
        private System.Windows.Forms.TextBox m_maxLogFileCountTextBox;
        private System.Windows.Forms.Button m_okButton;
        private System.Windows.Forms.Button m_cancelButton;
    }
}