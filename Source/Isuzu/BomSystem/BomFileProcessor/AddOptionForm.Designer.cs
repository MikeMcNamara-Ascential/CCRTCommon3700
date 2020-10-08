namespace BomFileProcessor
{
    partial class AddOptionForm
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
            this.m_modelCodeLabel = new System.Windows.Forms.Label();
            this.m_modelCodeTextBox = new System.Windows.Forms.TextBox();
            this.m_valueTextBox = new System.Windows.Forms.TextBox();
            this.m_valueLabel = new System.Windows.Forms.Label();
            this.m_okButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_modelCodeLabel
            // 
            this.m_modelCodeLabel.AutoSize = true;
            this.m_modelCodeLabel.Location = new System.Drawing.Point(63, 15);
            this.m_modelCodeLabel.Name = "m_modelCodeLabel";
            this.m_modelCodeLabel.Size = new System.Drawing.Size(67, 13);
            this.m_modelCodeLabel.TabIndex = 0;
            this.m_modelCodeLabel.Text = "Model Code:";
            // 
            // m_modelCodeTextBox
            // 
            this.m_modelCodeTextBox.Location = new System.Drawing.Point(136, 12);
            this.m_modelCodeTextBox.Name = "m_modelCodeTextBox";
            this.m_modelCodeTextBox.Size = new System.Drawing.Size(100, 20);
            this.m_modelCodeTextBox.TabIndex = 1;
            // 
            // m_valueTextBox
            // 
            this.m_valueTextBox.Location = new System.Drawing.Point(136, 38);
            this.m_valueTextBox.Name = "m_valueTextBox";
            this.m_valueTextBox.Size = new System.Drawing.Size(100, 20);
            this.m_valueTextBox.TabIndex = 2;
            // 
            // m_valueLabel
            // 
            this.m_valueLabel.AutoSize = true;
            this.m_valueLabel.Location = new System.Drawing.Point(12, 41);
            this.m_valueLabel.Name = "m_valueLabel";
            this.m_valueLabel.Size = new System.Drawing.Size(118, 13);
            this.m_valueLabel.TabIndex = 3;
            this.m_valueLabel.Text = "Wheelbase Position (in)";
            // 
            // m_okButton
            // 
            this.m_okButton.Location = new System.Drawing.Point(161, 94);
            this.m_okButton.Name = "m_okButton";
            this.m_okButton.Size = new System.Drawing.Size(75, 23);
            this.m_okButton.TabIndex = 4;
            this.m_okButton.Text = "OK";
            this.m_okButton.UseVisualStyleBackColor = true;
            this.m_okButton.Click += new System.EventHandler(this.m_okButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(12, 94);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 5;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // AddOptionForm
            // 
            this.AcceptButton = this.m_okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(249, 129);
            this.ControlBox = false;
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_okButton);
            this.Controls.Add(this.m_valueLabel);
            this.Controls.Add(this.m_valueTextBox);
            this.Controls.Add(this.m_modelCodeTextBox);
            this.Controls.Add(this.m_modelCodeLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Name = "AddOptionForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "AddOptionForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_modelCodeLabel;
        private System.Windows.Forms.TextBox m_modelCodeTextBox;
        private System.Windows.Forms.TextBox m_valueTextBox;
        private System.Windows.Forms.Label m_valueLabel;
        private System.Windows.Forms.Button m_okButton;
        private System.Windows.Forms.Button m_cancelButton;
    }
}