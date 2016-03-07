namespace MaintenanceReminder
{
    partial class ViewNotesForm
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
            this.m_notesTextBox = new System.Windows.Forms.TextBox();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_addNoteButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_notesTextBox
            // 
            this.m_notesTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_notesTextBox.Location = new System.Drawing.Point(12, 12);
            this.m_notesTextBox.Multiline = true;
            this.m_notesTextBox.Name = "m_notesTextBox";
            this.m_notesTextBox.ReadOnly = true;
            this.m_notesTextBox.Size = new System.Drawing.Size(647, 327);
            this.m_notesTextBox.TabIndex = 0;
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(584, 345);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 1;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            this.m_closeButton.Click += new System.EventHandler(this.m_closeButton_Click);
            // 
            // m_addNoteButton
            // 
            this.m_addNoteButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_addNoteButton.Location = new System.Drawing.Point(492, 345);
            this.m_addNoteButton.Name = "m_addNoteButton";
            this.m_addNoteButton.Size = new System.Drawing.Size(86, 23);
            this.m_addNoteButton.TabIndex = 2;
            this.m_addNoteButton.Text = "Add New Note";
            this.m_addNoteButton.UseVisualStyleBackColor = true;
            this.m_addNoteButton.Click += new System.EventHandler(this.m_addNoteButton_Click);
            // 
            // ViewNotesForm
            // 
            this.AcceptButton = this.m_addNoteButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(671, 380);
            this.Controls.Add(this.m_addNoteButton);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_notesTextBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "ViewNotesForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Notes";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox m_notesTextBox;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Button m_addNoteButton;
    }
}