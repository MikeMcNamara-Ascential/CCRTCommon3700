namespace MaintenanceReminder
{
    partial class UserLogin
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
            this.m_passwordLabel = new System.Windows.Forms.Label();
            this.m_passwordTextBox = new System.Windows.Forms.TextBox();
            this.m_loginButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_passwordLabel
            // 
            this.m_passwordLabel.AutoSize = true;
            this.m_passwordLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_passwordLabel.Location = new System.Drawing.Point(13, 13);
            this.m_passwordLabel.Name = "m_passwordLabel";
            this.m_passwordLabel.Size = new System.Drawing.Size(171, 13);
            this.m_passwordLabel.TabIndex = 0;
            this.m_passwordLabel.Text = "Please Enter Your Password:";
            // 
            // m_passwordTextBox
            // 
            this.m_passwordTextBox.Location = new System.Drawing.Point(190, 10);
            this.m_passwordTextBox.MaxLength = 8;
            this.m_passwordTextBox.Name = "m_passwordTextBox";
            this.m_passwordTextBox.PasswordChar = '*';
            this.m_passwordTextBox.Size = new System.Drawing.Size(125, 20);
            this.m_passwordTextBox.TabIndex = 1;
            // 
            // m_loginButton
            // 
            this.m_loginButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.m_loginButton.Location = new System.Drawing.Point(13, 46);
            this.m_loginButton.Name = "m_loginButton";
            this.m_loginButton.Size = new System.Drawing.Size(75, 23);
            this.m_loginButton.TabIndex = 2;
            this.m_loginButton.Text = "Login";
            this.m_loginButton.UseVisualStyleBackColor = true;
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(240, 46);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 3;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // UserLogin
            // 
            this.AcceptButton = this.m_loginButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(328, 81);
            this.ControlBox = false;
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_loginButton);
            this.Controls.Add(this.m_passwordTextBox);
            this.Controls.Add(this.m_passwordLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Name = "UserLogin";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "User Login";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_passwordLabel;
        private System.Windows.Forms.TextBox m_passwordTextBox;
        private System.Windows.Forms.Button m_loginButton;
        private System.Windows.Forms.Button m_cancelButton;
    }
}