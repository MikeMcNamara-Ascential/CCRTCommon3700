namespace InputDataItemLibrary
{
    partial class InputDataItemForm
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
            this.m_dataItemNameLabel = new System.Windows.Forms.Label();
            this.m_minLengthLabel = new System.Windows.Forms.Label();
            this.m_maxLengthLabel = new System.Windows.Forms.Label();
            this.m_dataItemNameTextBox = new System.Windows.Forms.TextBox();
            this.m_minLenghtTextBox = new System.Windows.Forms.TextBox();
            this.m_maxLengthTextBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // m_dataItemNameLabel
            // 
            this.m_dataItemNameLabel.AutoSize = true;
            this.m_dataItemNameLabel.Location = new System.Drawing.Point(32, 9);
            this.m_dataItemNameLabel.Name = "m_dataItemNameLabel";
            this.m_dataItemNameLabel.Size = new System.Drawing.Size(87, 13);
            this.m_dataItemNameLabel.TabIndex = 0;
            this.m_dataItemNameLabel.Text = "Data Item Name:";
            // 
            // m_minLengthLabel
            // 
            this.m_minLengthLabel.AutoSize = true;
            this.m_minLengthLabel.Location = new System.Drawing.Point(32, 34);
            this.m_minLengthLabel.Name = "m_minLengthLabel";
            this.m_minLengthLabel.Size = new System.Drawing.Size(87, 13);
            this.m_minLengthLabel.TabIndex = 1;
            this.m_minLengthLabel.Text = "Minimum Length:";
            // 
            // m_maxLengthLabel
            // 
            this.m_maxLengthLabel.AutoSize = true;
            this.m_maxLengthLabel.Location = new System.Drawing.Point(29, 63);
            this.m_maxLengthLabel.Name = "m_maxLengthLabel";
            this.m_maxLengthLabel.Size = new System.Drawing.Size(90, 13);
            this.m_maxLengthLabel.TabIndex = 2;
            this.m_maxLengthLabel.Text = "Maximum Length:";
            // 
            // m_dataItemNameTextBox
            // 
            this.m_dataItemNameTextBox.BackColor = System.Drawing.Color.White;
            this.m_dataItemNameTextBox.Location = new System.Drawing.Point(125, 6);
            this.m_dataItemNameTextBox.Name = "m_dataItemNameTextBox";
            this.m_dataItemNameTextBox.ReadOnly = true;
            this.m_dataItemNameTextBox.Size = new System.Drawing.Size(210, 20);
            this.m_dataItemNameTextBox.TabIndex = 7;
            // 
            // m_minLenghtTextBox
            // 
            this.m_minLenghtTextBox.BackColor = System.Drawing.Color.White;
            this.m_minLenghtTextBox.Location = new System.Drawing.Point(125, 31);
            this.m_minLenghtTextBox.Name = "m_minLenghtTextBox";
            this.m_minLenghtTextBox.ReadOnly = true;
            this.m_minLenghtTextBox.Size = new System.Drawing.Size(65, 20);
            this.m_minLenghtTextBox.TabIndex = 8;
            // 
            // m_maxLengthTextBox
            // 
            this.m_maxLengthTextBox.BackColor = System.Drawing.Color.White;
            this.m_maxLengthTextBox.Location = new System.Drawing.Point(125, 60);
            this.m_maxLengthTextBox.Name = "m_maxLengthTextBox";
            this.m_maxLengthTextBox.ReadOnly = true;
            this.m_maxLengthTextBox.Size = new System.Drawing.Size(65, 20);
            this.m_maxLengthTextBox.TabIndex = 9;
            // 
            // InputDataItemForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightBlue;
            this.ClientSize = new System.Drawing.Size(356, 102);
            this.Controls.Add(this.m_maxLengthTextBox);
            this.Controls.Add(this.m_minLenghtTextBox);
            this.Controls.Add(this.m_dataItemNameTextBox);
            this.Controls.Add(this.m_maxLengthLabel);
            this.Controls.Add(this.m_minLengthLabel);
            this.Controls.Add(this.m_dataItemNameLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "InputDataItemForm";
            this.Text = "Input Data Item";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_dataItemNameLabel;
        private System.Windows.Forms.Label m_minLengthLabel;
        private System.Windows.Forms.Label m_maxLengthLabel;
        private System.Windows.Forms.TextBox m_dataItemNameTextBox;
        private System.Windows.Forms.TextBox m_minLenghtTextBox;
        private System.Windows.Forms.TextBox m_maxLengthTextBox;
    }
}