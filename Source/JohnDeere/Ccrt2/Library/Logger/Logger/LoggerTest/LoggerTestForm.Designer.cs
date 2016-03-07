namespace LoggerTest
{
    partial class LoggerTestForm
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
            this.components = new System.ComponentModel.Container();
            this.m_LogMessageLabel = new System.Windows.Forms.Label();
            this.m_logMsgTextBox = new System.Windows.Forms.TextBox();
            this.m_exitButton = new System.Windows.Forms.Button();
            this.m_logTimer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // m_LogMessageLabel
            // 
            this.m_LogMessageLabel.AutoSize = true;
            this.m_LogMessageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_LogMessageLabel.Location = new System.Drawing.Point(12, 9);
            this.m_LogMessageLabel.Name = "m_LogMessageLabel";
            this.m_LogMessageLabel.Size = new System.Drawing.Size(92, 13);
            this.m_LogMessageLabel.TabIndex = 0;
            this.m_LogMessageLabel.Text = "Log Messages:";
            // 
            // m_logMsgTextBox
            // 
            this.m_logMsgTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_logMsgTextBox.Location = new System.Drawing.Point(12, 25);
            this.m_logMsgTextBox.Multiline = true;
            this.m_logMsgTextBox.Name = "m_logMsgTextBox";
            this.m_logMsgTextBox.Size = new System.Drawing.Size(431, 305);
            this.m_logMsgTextBox.TabIndex = 1;
            // 
            // m_exitButton
            // 
            this.m_exitButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_exitButton.Location = new System.Drawing.Point(368, 336);
            this.m_exitButton.Name = "m_exitButton";
            this.m_exitButton.Size = new System.Drawing.Size(75, 23);
            this.m_exitButton.TabIndex = 2;
            this.m_exitButton.Text = "Exit";
            this.m_exitButton.UseVisualStyleBackColor = true;
            this.m_exitButton.Click += new System.EventHandler(this.m_exitButton_Click);
            // 
            // m_logTimer
            // 
            this.m_logTimer.Tick += new System.EventHandler(this.m_logTimer_Tick);
            // 
            // LoggerTestForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(455, 371);
            this.Controls.Add(this.m_exitButton);
            this.Controls.Add(this.m_logMsgTextBox);
            this.Controls.Add(this.m_LogMessageLabel);
            this.Name = "LoggerTestForm";
            this.Text = "Logger Tester";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_LogMessageLabel;
        private System.Windows.Forms.TextBox m_logMsgTextBox;
        private System.Windows.Forms.Button m_exitButton;
        private System.Windows.Forms.Timer m_logTimer;
    }
}

