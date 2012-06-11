namespace TestFCM
{
    partial class FCMTestMainForm
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
            this.m_logRTB = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // m_logRTB
            // 
            this.m_logRTB.Location = new System.Drawing.Point(8, 12);
            this.m_logRTB.Name = "m_logRTB";
            this.m_logRTB.Size = new System.Drawing.Size(272, 242);
            this.m_logRTB.TabIndex = 0;
            this.m_logRTB.Text = "";
            // 
            // FCMTestMainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Controls.Add(this.m_logRTB);
            this.Name = "FCMTestMainForm";
            this.Text = "FCMTestMainForm";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FCMTestMainForm_FormClosing);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RichTextBox m_logRTB;
    }
}