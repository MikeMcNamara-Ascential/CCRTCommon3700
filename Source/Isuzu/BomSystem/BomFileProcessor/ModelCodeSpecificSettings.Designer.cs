namespace BomFileProcessor
{
    partial class ModelCodeSpecificSettings
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
            this.m_removeButton = new System.Windows.Forms.Button();
            this.m_addButton = new System.Windows.Forms.Button();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_modelOptionsDGV = new System.Windows.Forms.DataGridView();
            this.ModelCode = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.WriteESN = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.EsnLeadingChars = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.m_modelOptionsDGV)).BeginInit();
            this.SuspendLayout();
            // 
            // m_removeButton
            // 
            this.m_removeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_removeButton.Location = new System.Drawing.Point(92, 243);
            this.m_removeButton.Name = "m_removeButton";
            this.m_removeButton.Size = new System.Drawing.Size(83, 23);
            this.m_removeButton.TabIndex = 14;
            this.m_removeButton.Text = "Remove Item";
            this.m_removeButton.UseVisualStyleBackColor = true;
            this.m_removeButton.Click += new System.EventHandler(this.m_removeButton_Click);
            // 
            // m_addButton
            // 
            this.m_addButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_addButton.Location = new System.Drawing.Point(11, 242);
            this.m_addButton.Name = "m_addButton";
            this.m_addButton.Size = new System.Drawing.Size(75, 23);
            this.m_addButton.TabIndex = 13;
            this.m_addButton.Text = "Add Item";
            this.m_addButton.UseVisualStyleBackColor = true;
            this.m_addButton.Click += new System.EventHandler(this.m_addButton_Click);
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(195, 242);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 12;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            this.m_closeButton.Click += new System.EventHandler(this.m_closeButton_Click);
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.Location = new System.Drawing.Point(276, 242);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 11;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_modelOptionsDGV
            // 
            this.m_modelOptionsDGV.AllowUserToAddRows = false;
            this.m_modelOptionsDGV.AllowUserToDeleteRows = false;
            this.m_modelOptionsDGV.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.m_modelOptionsDGV.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_modelOptionsDGV.BackgroundColor = System.Drawing.Color.LightSteelBlue;
            this.m_modelOptionsDGV.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_modelOptionsDGV.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_modelOptionsDGV.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ModelCode,
            this.WriteESN,
            this.EsnLeadingChars});
            this.m_modelOptionsDGV.Location = new System.Drawing.Point(11, 3);
            this.m_modelOptionsDGV.Name = "m_modelOptionsDGV";
            this.m_modelOptionsDGV.RowHeadersVisible = false;
            this.m_modelOptionsDGV.RowTemplate.Height = 24;
            this.m_modelOptionsDGV.Size = new System.Drawing.Size(340, 233);
            this.m_modelOptionsDGV.TabIndex = 10;
            this.m_modelOptionsDGV.CellBeginEdit += new System.Windows.Forms.DataGridViewCellCancelEventHandler(this.m_modelOptionsDGV_CellBeginEdit);
            // 
            // ModelCode
            // 
            this.ModelCode.HeaderText = "Model Code";
            this.ModelCode.Name = "ModelCode";
            this.ModelCode.ReadOnly = true;
            // 
            // WriteESN
            // 
            this.WriteESN.HeaderText = "Write ESN";
            this.WriteESN.Name = "WriteESN";
            // 
            // EsnLeadingChars
            // 
            this.EsnLeadingChars.HeaderText = "ESN Engine Code";
            this.EsnLeadingChars.Name = "EsnLeadingChars";
            // 
            // ModelCodeSpecificSettings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(362, 268);
            this.Controls.Add(this.m_removeButton);
            this.Controls.Add(this.m_addButton);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_modelOptionsDGV);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "ModelCodeSpecificSettings";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "ModelCodeSpecificSettings";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ModelCodeSpecificSettings_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.m_modelOptionsDGV)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button m_removeButton;
        private System.Windows.Forms.Button m_addButton;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.DataGridView m_modelOptionsDGV;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModelCode;
        private System.Windows.Forms.DataGridViewTextBoxColumn WriteESN;
        private System.Windows.Forms.DataGridViewTextBoxColumn EsnLeadingChars;
    }
}