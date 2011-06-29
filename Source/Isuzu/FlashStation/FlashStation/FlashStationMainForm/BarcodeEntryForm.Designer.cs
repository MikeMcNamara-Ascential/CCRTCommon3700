namespace FlashStationMainForm
{
    partial class BarcodeEntryForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BarcodeEntryForm));
            this.m_barcodeLabel = new System.Windows.Forms.Label();
            this.m_barcodeTextBox = new System.Windows.Forms.TextBox();
            this.m_submitButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_barcodeLabel
            // 
            this.m_barcodeLabel.AutoSize = true;
            this.m_barcodeLabel.Location = new System.Drawing.Point(5, 39);
            this.m_barcodeLabel.Name = "m_barcodeLabel";
            this.m_barcodeLabel.Size = new System.Drawing.Size(50, 13);
            this.m_barcodeLabel.TabIndex = 0;
            this.m_barcodeLabel.Text = "Barcode:";
            // 
            // m_barcodeTextBox
            // 
            this.m_barcodeTextBox.Location = new System.Drawing.Point(61, 36);
            this.m_barcodeTextBox.Name = "m_barcodeTextBox";
            this.m_barcodeTextBox.Size = new System.Drawing.Size(262, 20);
            this.m_barcodeTextBox.TabIndex = 1;
            // 
            // m_submitButton
            // 
            this.m_submitButton.Location = new System.Drawing.Point(248, 66);
            this.m_submitButton.Name = "m_submitButton";
            this.m_submitButton.Size = new System.Drawing.Size(75, 23);
            this.m_submitButton.TabIndex = 2;
            this.m_submitButton.Text = "Submit";
            this.m_submitButton.UseVisualStyleBackColor = true;
            this.m_submitButton.Click += new System.EventHandler(this.m_submitButton_Click);
            // 
            // BarcodeEntryForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(335, 101);
            this.Controls.Add(this.m_submitButton);
            this.Controls.Add(this.m_barcodeTextBox);
            this.Controls.Add(this.m_barcodeLabel);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "BarcodeEntryForm";
            this.Text = "BarcodeEntryForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_barcodeLabel;
        private System.Windows.Forms.TextBox m_barcodeTextBox;
        private System.Windows.Forms.Button m_submitButton;
    }
}