namespace TeslaHostSimulator
{
    partial class PortSetupForm
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
            this.label1 = new System.Windows.Forms.Label();
            this.m_serialPortComboBox = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.m_baudRateTextBox = new System.Windows.Forms.TextBox();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.m_parityTextBox = new System.Windows.Forms.TextBox();
            this.m_dataBitsTextBox = new System.Windows.Forms.TextBox();
            this.m_stopBitsTextBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(27, 28);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(90, 17);
            this.label1.TabIndex = 0;
            this.label1.Text = "Serial Port:";
            // 
            // m_serialPortComboBox
            // 
            this.m_serialPortComboBox.FormattingEnabled = true;
            this.m_serialPortComboBox.Location = new System.Drawing.Point(123, 27);
            this.m_serialPortComboBox.Name = "m_serialPortComboBox";
            this.m_serialPortComboBox.Size = new System.Drawing.Size(91, 21);
            this.m_serialPortComboBox.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(27, 56);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(89, 17);
            this.label2.TabIndex = 2;
            this.label2.Text = "Baud Rate:";
            // 
            // m_baudRateTextBox
            // 
            this.m_baudRateTextBox.Location = new System.Drawing.Point(123, 55);
            this.m_baudRateTextBox.Name = "m_baudRateTextBox";
            this.m_baudRateTextBox.Size = new System.Drawing.Size(91, 20);
            this.m_baudRateTextBox.TabIndex = 3;
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_closeButton.BackColor = System.Drawing.Color.Gainsboro;
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(13, 174);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 4;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = false;
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.BackColor = System.Drawing.Color.Gainsboro;
            this.m_saveButton.Location = new System.Drawing.Point(158, 174);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 5;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = false;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(61, 82);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(55, 17);
            this.label3.TabIndex = 6;
            this.label3.Text = "Parity:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(37, 108);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(79, 17);
            this.label4.TabIndex = 7;
            this.label4.Text = "Data Bits:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(38, 134);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(78, 17);
            this.label5.TabIndex = 8;
            this.label5.Text = "Stop Bits:";
            // 
            // m_parityTextBox
            // 
            this.m_parityTextBox.Location = new System.Drawing.Point(123, 81);
            this.m_parityTextBox.Name = "m_parityTextBox";
            this.m_parityTextBox.Size = new System.Drawing.Size(91, 20);
            this.m_parityTextBox.TabIndex = 9;
            // 
            // m_dataBitsTextBox
            // 
            this.m_dataBitsTextBox.Location = new System.Drawing.Point(123, 107);
            this.m_dataBitsTextBox.Name = "m_dataBitsTextBox";
            this.m_dataBitsTextBox.Size = new System.Drawing.Size(91, 20);
            this.m_dataBitsTextBox.TabIndex = 10;
            // 
            // m_stopBitsTextBox
            // 
            this.m_stopBitsTextBox.Location = new System.Drawing.Point(123, 133);
            this.m_stopBitsTextBox.Name = "m_stopBitsTextBox";
            this.m_stopBitsTextBox.Size = new System.Drawing.Size(91, 20);
            this.m_stopBitsTextBox.TabIndex = 11;
            // 
            // PortSetupForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(245, 209);
            this.ControlBox = false;
            this.Controls.Add(this.m_stopBitsTextBox);
            this.Controls.Add(this.m_dataBitsTextBox);
            this.Controls.Add(this.m_parityTextBox);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_baudRateTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.m_serialPortComboBox);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Name = "PortSetupForm";
            this.Text = "Serial Port Setup";
            this.Load += new System.EventHandler(this.PortSetupForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox m_serialPortComboBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox m_baudRateTextBox;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox m_parityTextBox;
        private System.Windows.Forms.TextBox m_dataBitsTextBox;
        private System.Windows.Forms.TextBox m_stopBitsTextBox;
    }
}