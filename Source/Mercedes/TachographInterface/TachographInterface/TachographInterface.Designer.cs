namespace TachographInterface
{
    partial class m_tacographInterface
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(m_tacographInterface));
            this.m_exitButton = new System.Windows.Forms.Button();
            this.m_logo = new System.Windows.Forms.PictureBox();
            this.m_logBox = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.m_logo)).BeginInit();
            this.SuspendLayout();
            // 
            // m_exitButton
            // 
            this.m_exitButton.Location = new System.Drawing.Point(515, 346);
            this.m_exitButton.Name = "m_exitButton";
            this.m_exitButton.Size = new System.Drawing.Size(75, 23);
            this.m_exitButton.TabIndex = 0;
            this.m_exitButton.Text = "Exit";
            this.m_exitButton.UseVisualStyleBackColor = true;
            this.m_exitButton.Click += new System.EventHandler(this.m_exitButton_Click);
            // 
            // m_logo
            // 
            this.m_logo.ErrorImage = ((System.Drawing.Image)(resources.GetObject("m_logo.ErrorImage")));
            this.m_logo.Image = ((System.Drawing.Image)(resources.GetObject("m_logo.Image")));
            this.m_logo.Location = new System.Drawing.Point(501, 12);
            this.m_logo.Name = "m_logo";
            this.m_logo.Size = new System.Drawing.Size(89, 90);
            this.m_logo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.m_logo.TabIndex = 1;
            this.m_logo.TabStop = false;
            // 
            // m_logBox
            // 
            this.m_logBox.BackColor = System.Drawing.Color.White;
            this.m_logBox.Location = new System.Drawing.Point(13, 13);
            this.m_logBox.Multiline = true;
            this.m_logBox.Name = "m_logBox";
            this.m_logBox.ReadOnly = true;
            this.m_logBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.m_logBox.Size = new System.Drawing.Size(471, 356);
            this.m_logBox.TabIndex = 2;
            // 
            // m_tacographInterface
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(602, 381);
            this.Controls.Add(this.m_logBox);
            this.Controls.Add(this.m_logo);
            this.Controls.Add(this.m_exitButton);
            this.Name = "m_tacographInterface";
            this.Text = "Tacograph Interface";
            ((System.ComponentModel.ISupportInitialize)(this.m_logo)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_exitButton;
        private System.Windows.Forms.PictureBox m_logo;
        private System.Windows.Forms.TextBox m_logBox;
    }
}

