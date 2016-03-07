namespace Logger
{
    partial class LogFileChooserDialog
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
            this.m_selectFilesLabel = new System.Windows.Forms.Label();
            this.m_logFilesCheckedListBox = new System.Windows.Forms.CheckedListBox();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_openButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_selectFilesLabel
            // 
            this.m_selectFilesLabel.AutoSize = true;
            this.m_selectFilesLabel.Location = new System.Drawing.Point(12, 9);
            this.m_selectFilesLabel.Name = "m_selectFilesLabel";
            this.m_selectFilesLabel.Size = new System.Drawing.Size(120, 13);
            this.m_selectFilesLabel.TabIndex = 0;
            this.m_selectFilesLabel.Text = "Select Log Files to View";
            // 
            // m_logFilesCheckedListBox
            // 
            this.m_logFilesCheckedListBox.CheckOnClick = true;
            this.m_logFilesCheckedListBox.FormattingEnabled = true;
            this.m_logFilesCheckedListBox.Location = new System.Drawing.Point(15, 25);
            this.m_logFilesCheckedListBox.Name = "m_logFilesCheckedListBox";
            this.m_logFilesCheckedListBox.Size = new System.Drawing.Size(257, 199);
            this.m_logFilesCheckedListBox.TabIndex = 1;
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(197, 228);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 22);
            this.m_cancelButton.TabIndex = 2;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // m_openButton
            // 
            this.m_openButton.Location = new System.Drawing.Point(15, 228);
            this.m_openButton.Name = "m_openButton";
            this.m_openButton.Size = new System.Drawing.Size(75, 22);
            this.m_openButton.TabIndex = 3;
            this.m_openButton.Text = "Open Logs";
            this.m_openButton.UseVisualStyleBackColor = true;
            this.m_openButton.Click += new System.EventHandler(this.m_openButton_Click);
            // 
            // LogFileChooserDialog
            // 
            this.AcceptButton = this.m_openButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.ControlBox = false;
            this.Controls.Add(this.m_openButton);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_logFilesCheckedListBox);
            this.Controls.Add(this.m_selectFilesLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "LogFileChooserDialog";
            this.Text = "Log File Chooser";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_selectFilesLabel;
        private System.Windows.Forms.CheckedListBox m_logFilesCheckedListBox;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Button m_openButton;
    }
}