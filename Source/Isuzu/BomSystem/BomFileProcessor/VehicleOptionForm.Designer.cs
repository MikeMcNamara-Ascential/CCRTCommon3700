namespace BomFileProcessor
{
    partial class VehicleOptionForm
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
            this.m_optionDataGridView = new System.Windows.Forms.DataGridView();
            this.ModelCode = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Value = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_addButton = new System.Windows.Forms.Button();
            this.m_removeButton = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.m_optionDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_optionDataGridView
            // 
            this.m_optionDataGridView.AllowUserToAddRows = false;
            this.m_optionDataGridView.AllowUserToDeleteRows = false;
            this.m_optionDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.m_optionDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_optionDataGridView.BackgroundColor = System.Drawing.Color.LightSteelBlue;
            this.m_optionDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_optionDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_optionDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ModelCode,
            this.Value});
            this.m_optionDataGridView.Location = new System.Drawing.Point(12, 12);
            this.m_optionDataGridView.Name = "m_optionDataGridView";
            this.m_optionDataGridView.RowHeadersVisible = false;
            this.m_optionDataGridView.Size = new System.Drawing.Size(353, 213);
            this.m_optionDataGridView.TabIndex = 0;
            // 
            // ModelCode
            // 
            this.ModelCode.HeaderText = "Model Code";
            this.ModelCode.Name = "ModelCode";
            this.ModelCode.ReadOnly = true;
            // 
            // Value
            // 
            this.Value.HeaderText = "Value";
            this.Value.Name = "Value";
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.Location = new System.Drawing.Point(290, 231);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 1;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(209, 231);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 2;
            this.m_cancelButton.Text = "Close";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // m_addButton
            // 
            this.m_addButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_addButton.Location = new System.Drawing.Point(12, 231);
            this.m_addButton.Name = "m_addButton";
            this.m_addButton.Size = new System.Drawing.Size(75, 23);
            this.m_addButton.TabIndex = 3;
            this.m_addButton.Text = "Add";
            this.m_addButton.UseVisualStyleBackColor = true;
            this.m_addButton.Click += new System.EventHandler(this.m_addButton_Click);
            // 
            // m_removeButton
            // 
            this.m_removeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_removeButton.Location = new System.Drawing.Point(93, 231);
            this.m_removeButton.Name = "m_removeButton";
            this.m_removeButton.Size = new System.Drawing.Size(75, 23);
            this.m_removeButton.TabIndex = 4;
            this.m_removeButton.Text = "Remove";
            this.m_removeButton.UseVisualStyleBackColor = true;
            this.m_removeButton.Click += new System.EventHandler(this.m_removeButton_Click);
            // 
            // VehicleOptionForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(377, 266);
            this.ControlBox = false;
            this.Controls.Add(this.m_removeButton);
            this.Controls.Add(this.m_addButton);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_optionDataGridView);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Name = "VehicleOptionForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Vehicle Option Setup";
            ((System.ComponentModel.ISupportInitialize)(this.m_optionDataGridView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView m_optionDataGridView;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Button m_addButton;
        private System.Windows.Forms.Button m_removeButton;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModelCode;
        private System.Windows.Forms.DataGridViewTextBoxColumn Value;
    }
}