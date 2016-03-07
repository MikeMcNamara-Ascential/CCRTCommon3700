namespace InputServer
{
    partial class StartupMessageWindow
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
            this.m_bepLogoPictureBox = new System.Windows.Forms.PictureBox();
            this.m_startLabel = new System.Windows.Forms.Label();
            this.m_startMessageLabel = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.m_bepLogoPictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // m_bepLogoPictureBox
            // 
            this.m_bepLogoPictureBox.BackgroundImage = global::InputServer.Properties.Resources.BepLogo;
            this.m_bepLogoPictureBox.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.m_bepLogoPictureBox.Location = new System.Drawing.Point(12, 12);
            this.m_bepLogoPictureBox.Name = "m_bepLogoPictureBox";
            this.m_bepLogoPictureBox.Size = new System.Drawing.Size(172, 163);
            this.m_bepLogoPictureBox.TabIndex = 0;
            this.m_bepLogoPictureBox.TabStop = false;
            // 
            // m_startLabel
            // 
            this.m_startLabel.AutoSize = true;
            this.m_startLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_startLabel.Location = new System.Drawing.Point(199, 12);
            this.m_startLabel.Name = "m_startLabel";
            this.m_startLabel.Size = new System.Drawing.Size(426, 29);
            this.m_startLabel.TabIndex = 1;
            this.m_startLabel.Text = "Starting Windows CCRT Input Server...";
            // 
            // m_startMessageLabel
            // 
            this.m_startMessageLabel.AutoSize = true;
            this.m_startMessageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_startMessageLabel.Location = new System.Drawing.Point(218, 77);
            this.m_startMessageLabel.Name = "m_startMessageLabel";
            this.m_startMessageLabel.Size = new System.Drawing.Size(51, 20);
            this.m_startMessageLabel.TabIndex = 2;
            this.m_startMessageLabel.Text = "label1";
            // 
            // StartupMessageWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(760, 191);
            this.ControlBox = false;
            this.Controls.Add(this.m_startMessageLabel);
            this.Controls.Add(this.m_startLabel);
            this.Controls.Add(this.m_bepLogoPictureBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "StartupMessageWindow";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "StartupMessageWindow";
            ((System.ComponentModel.ISupportInitialize)(this.m_bepLogoPictureBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox m_bepLogoPictureBox;
        private System.Windows.Forms.Label m_startLabel;
        private System.Windows.Forms.Label m_startMessageLabel;
    }
}