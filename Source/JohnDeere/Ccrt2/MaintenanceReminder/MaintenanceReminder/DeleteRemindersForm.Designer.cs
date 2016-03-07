namespace MaintenanceReminder
{
    partial class DeleteRemindersForm
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
            this.m_reminderLabel = new System.Windows.Forms.Label();
            this.m_reminderCheckedListBox = new System.Windows.Forms.CheckedListBox();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_deleteButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_reminderLabel
            // 
            this.m_reminderLabel.AutoSize = true;
            this.m_reminderLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_reminderLabel.Location = new System.Drawing.Point(12, 9);
            this.m_reminderLabel.Name = "m_reminderLabel";
            this.m_reminderLabel.Size = new System.Drawing.Size(70, 13);
            this.m_reminderLabel.TabIndex = 0;
            this.m_reminderLabel.Text = "Reminders:";
            // 
            // m_reminderCheckedListBox
            // 
            this.m_reminderCheckedListBox.CheckOnClick = true;
            this.m_reminderCheckedListBox.FormattingEnabled = true;
            this.m_reminderCheckedListBox.Location = new System.Drawing.Point(15, 25);
            this.m_reminderCheckedListBox.Name = "m_reminderCheckedListBox";
            this.m_reminderCheckedListBox.Size = new System.Drawing.Size(257, 199);
            this.m_reminderCheckedListBox.TabIndex = 1;
            // 
            // m_closeButton
            // 
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(197, 230);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 2;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            // 
            // m_deleteButton
            // 
            this.m_deleteButton.Location = new System.Drawing.Point(116, 230);
            this.m_deleteButton.Name = "m_deleteButton";
            this.m_deleteButton.Size = new System.Drawing.Size(75, 23);
            this.m_deleteButton.TabIndex = 3;
            this.m_deleteButton.Text = "Delete";
            this.m_deleteButton.UseVisualStyleBackColor = true;
            this.m_deleteButton.Click += new System.EventHandler(this.m_deleteButton_Click);
            // 
            // DeleteRemindersForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.m_deleteButton);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_reminderCheckedListBox);
            this.Controls.Add(this.m_reminderLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "DeleteRemindersForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Delete Reminders";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_reminderLabel;
        private System.Windows.Forms.CheckedListBox m_reminderCheckedListBox;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Button m_deleteButton;
    }
}