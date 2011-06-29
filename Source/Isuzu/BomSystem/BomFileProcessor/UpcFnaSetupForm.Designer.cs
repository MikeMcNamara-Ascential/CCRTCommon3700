namespace BomFileProcessor
{
    partial class UpcFnaSetupForm
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
            this.m_upcFnaDataGridView = new System.Windows.Forms.DataGridView();
            this.Module = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.UPC = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.FNA = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_addItemButton = new System.Windows.Forms.Button();
            this.m_removeButton = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.m_upcFnaDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_upcFnaDataGridView
            // 
            this.m_upcFnaDataGridView.AllowUserToAddRows = false;
            this.m_upcFnaDataGridView.AllowUserToDeleteRows = false;
            this.m_upcFnaDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_upcFnaDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_upcFnaDataGridView.BackgroundColor = System.Drawing.Color.LightSteelBlue;
            this.m_upcFnaDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_upcFnaDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_upcFnaDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Module,
            this.UPC,
            this.FNA});
            this.m_upcFnaDataGridView.Location = new System.Drawing.Point(12, 12);
            this.m_upcFnaDataGridView.Name = "m_upcFnaDataGridView";
            this.m_upcFnaDataGridView.RowHeadersVisible = false;
            this.m_upcFnaDataGridView.Size = new System.Drawing.Size(340, 233);
            this.m_upcFnaDataGridView.TabIndex = 0;
            // 
            // Module
            // 
            this.Module.HeaderText = "Item";
            this.Module.Name = "Module";
            this.Module.ReadOnly = true;
            // 
            // UPC
            // 
            this.UPC.HeaderText = "UPC";
            this.UPC.Name = "UPC";
            // 
            // FNA
            // 
            this.FNA.HeaderText = "FNA";
            this.FNA.Name = "FNA";
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.Location = new System.Drawing.Point(277, 251);
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
            this.m_cancelButton.Location = new System.Drawing.Point(196, 251);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 2;
            this.m_cancelButton.Text = "Close";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // m_addItemButton
            // 
            this.m_addItemButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_addItemButton.Location = new System.Drawing.Point(12, 251);
            this.m_addItemButton.Name = "m_addItemButton";
            this.m_addItemButton.Size = new System.Drawing.Size(75, 23);
            this.m_addItemButton.TabIndex = 3;
            this.m_addItemButton.Text = "Add Item";
            this.m_addItemButton.UseVisualStyleBackColor = true;
            this.m_addItemButton.Click += new System.EventHandler(this.m_addItemButton_Click);
            // 
            // m_removeButton
            // 
            this.m_removeButton.Location = new System.Drawing.Point(93, 251);
            this.m_removeButton.Name = "m_removeButton";
            this.m_removeButton.Size = new System.Drawing.Size(83, 23);
            this.m_removeButton.TabIndex = 4;
            this.m_removeButton.Text = "Remove Item";
            this.m_removeButton.UseVisualStyleBackColor = true;
            this.m_removeButton.Click += new System.EventHandler(this.m_removeButton_Click);
            // 
            // UpcFnaSetupForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(369, 282);
            this.ControlBox = false;
            this.Controls.Add(this.m_removeButton);
            this.Controls.Add(this.m_addItemButton);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_upcFnaDataGridView);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "UpcFnaSetupForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "UPC + FNA Configuration";
            ((System.ComponentModel.ISupportInitialize)(this.m_upcFnaDataGridView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView m_upcFnaDataGridView;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.DataGridViewTextBoxColumn Module;
        private System.Windows.Forms.DataGridViewTextBoxColumn UPC;
        private System.Windows.Forms.DataGridViewTextBoxColumn FNA;
        private System.Windows.Forms.Button m_addItemButton;
        private System.Windows.Forms.Button m_removeButton;
    }
}