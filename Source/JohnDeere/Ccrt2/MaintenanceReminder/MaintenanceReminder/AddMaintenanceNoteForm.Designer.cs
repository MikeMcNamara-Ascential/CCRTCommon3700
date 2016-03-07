namespace MaintenanceReminder
{
    partial class AddMaintenanceNoteForm
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
            this.m_nameLabel = new System.Windows.Forms.Label();
            this.m_noteLabel = new System.Windows.Forms.Label();
            this.m_nameTextBox = new System.Windows.Forms.TextBox();
            this.m_noteTextBox = new System.Windows.Forms.TextBox();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_nameLabel
            // 
            this.m_nameLabel.AutoSize = true;
            this.m_nameLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_nameLabel.Location = new System.Drawing.Point(12, 20);
            this.m_nameLabel.Name = "m_nameLabel";
            this.m_nameLabel.Size = new System.Drawing.Size(43, 13);
            this.m_nameLabel.TabIndex = 0;
            this.m_nameLabel.Text = "Name:";
            // 
            // m_noteLabel
            // 
            this.m_noteLabel.AutoSize = true;
            this.m_noteLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_noteLabel.Location = new System.Drawing.Point(12, 53);
            this.m_noteLabel.Name = "m_noteLabel";
            this.m_noteLabel.Size = new System.Drawing.Size(38, 13);
            this.m_noteLabel.TabIndex = 1;
            this.m_noteLabel.Text = "Note:";
            // 
            // m_nameTextBox
            // 
            this.m_nameTextBox.Location = new System.Drawing.Point(61, 17);
            this.m_nameTextBox.Name = "m_nameTextBox";
            this.m_nameTextBox.Size = new System.Drawing.Size(149, 20);
            this.m_nameTextBox.TabIndex = 2;
            // 
            // m_noteTextBox
            // 
            this.m_noteTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_noteTextBox.Location = new System.Drawing.Point(15, 69);
            this.m_noteTextBox.Multiline = true;
            this.m_noteTextBox.Name = "m_noteTextBox";
            this.m_noteTextBox.Size = new System.Drawing.Size(498, 152);
            this.m_noteTextBox.TabIndex = 3;
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(438, 227);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 4;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // m_saveButton
            // 
            this.m_saveButton.Location = new System.Drawing.Point(357, 227);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 5;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // AddMaintenanceNoteForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(525, 262);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_noteTextBox);
            this.Controls.Add(this.m_nameTextBox);
            this.Controls.Add(this.m_noteLabel);
            this.Controls.Add(this.m_nameLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "AddMaintenanceNoteForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add Maintenance Note";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_nameLabel;
        private System.Windows.Forms.Label m_noteLabel;
        private System.Windows.Forms.TextBox m_nameTextBox;
        private System.Windows.Forms.TextBox m_noteTextBox;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Button m_saveButton;
    }
}