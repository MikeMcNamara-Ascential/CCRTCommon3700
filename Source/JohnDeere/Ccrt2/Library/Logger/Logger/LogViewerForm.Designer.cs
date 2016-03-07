namespace Logger
{
    partial class LogViewerForm
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
            this.m_searchLabel = new System.Windows.Forms.Label();
            this.m_searchTextBox = new System.Windows.Forms.TextBox();
            this.m_findButton = new System.Windows.Forms.Button();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_reloadButton = new System.Windows.Forms.Button();
            this.m_logTextBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // m_searchLabel
            // 
            this.m_searchLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_searchLabel.AutoSize = true;
            this.m_searchLabel.Location = new System.Drawing.Point(12, 301);
            this.m_searchLabel.Name = "m_searchLabel";
            this.m_searchLabel.Size = new System.Drawing.Size(44, 13);
            this.m_searchLabel.TabIndex = 0;
            this.m_searchLabel.Text = "Search:";
            // 
            // m_searchTextBox
            // 
            this.m_searchTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_searchTextBox.Location = new System.Drawing.Point(62, 298);
            this.m_searchTextBox.Name = "m_searchTextBox";
            this.m_searchTextBox.Size = new System.Drawing.Size(212, 20);
            this.m_searchTextBox.TabIndex = 1;
            this.m_searchTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.m_searchTextBox_KeyDown);
            // 
            // m_findButton
            // 
            this.m_findButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_findButton.Location = new System.Drawing.Point(280, 298);
            this.m_findButton.Name = "m_findButton";
            this.m_findButton.Size = new System.Drawing.Size(43, 21);
            this.m_findButton.TabIndex = 2;
            this.m_findButton.Text = "Find";
            this.m_findButton.UseVisualStyleBackColor = true;
            this.m_findButton.Click += new System.EventHandler(this.m_findButton_Click);
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_closeButton.Location = new System.Drawing.Point(569, 298);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(64, 20);
            this.m_closeButton.TabIndex = 3;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            this.m_closeButton.Click += new System.EventHandler(this.m_closeButton_Click);
            // 
            // m_reloadButton
            // 
            this.m_reloadButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_reloadButton.Location = new System.Drawing.Point(499, 298);
            this.m_reloadButton.Name = "m_reloadButton";
            this.m_reloadButton.Size = new System.Drawing.Size(64, 20);
            this.m_reloadButton.TabIndex = 4;
            this.m_reloadButton.Text = "Reload";
            this.m_reloadButton.UseVisualStyleBackColor = true;
            this.m_reloadButton.Click += new System.EventHandler(this.m_reloadButton_Click);
            // 
            // m_logTextBox
            // 
            this.m_logTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_logTextBox.BackColor = System.Drawing.Color.White;
            this.m_logTextBox.Location = new System.Drawing.Point(12, 12);
            this.m_logTextBox.Multiline = true;
            this.m_logTextBox.Name = "m_logTextBox";
            this.m_logTextBox.ReadOnly = true;
            this.m_logTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.m_logTextBox.Size = new System.Drawing.Size(621, 280);
            this.m_logTextBox.TabIndex = 5;
            this.m_logTextBox.MouseClick += new System.Windows.Forms.MouseEventHandler(this.m_logTextBox_MouseClick);
            // 
            // LogViewerForm
            // 
            this.AcceptButton = this.m_findButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(645, 323);
            this.ControlBox = false;
            this.Controls.Add(this.m_logTextBox);
            this.Controls.Add(this.m_reloadButton);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_findButton);
            this.Controls.Add(this.m_searchTextBox);
            this.Controls.Add(this.m_searchLabel);
            this.Name = "LogViewerForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Log Viewer";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_searchLabel;
        private System.Windows.Forms.TextBox m_searchTextBox;
        private System.Windows.Forms.Button m_findButton;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Button m_reloadButton;
        private System.Windows.Forms.TextBox m_logTextBox;
    }
}