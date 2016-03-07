namespace MaintenanceReminder
{
    partial class ExpiredReminderForm
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
            this.m_maintenanceItemsDueLabel = new System.Windows.Forms.Label();
            this.m_expiredReminderDataGridView = new System.Windows.Forms.DataGridView();
            this.ReminderItem = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaintenanceText = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ViewWorkInstruction = new System.Windows.Forms.DataGridViewButtonColumn();
            this.MaintenanceComplete = new System.Windows.Forms.DataGridViewButtonColumn();
            ((System.ComponentModel.ISupportInitialize)(this.m_expiredReminderDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_maintenanceItemsDueLabel
            // 
            this.m_maintenanceItemsDueLabel.AutoSize = true;
            this.m_maintenanceItemsDueLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_maintenanceItemsDueLabel.Location = new System.Drawing.Point(9, 9);
            this.m_maintenanceItemsDueLabel.Name = "m_maintenanceItemsDueLabel";
            this.m_maintenanceItemsDueLabel.Size = new System.Drawing.Size(243, 13);
            this.m_maintenanceItemsDueLabel.TabIndex = 0;
            this.m_maintenanceItemsDueLabel.Text = "The following maintenance items are due:";
            // 
            // m_expiredReminderDataGridView
            // 
            this.m_expiredReminderDataGridView.AllowUserToAddRows = false;
            this.m_expiredReminderDataGridView.AllowUserToDeleteRows = false;
            this.m_expiredReminderDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_expiredReminderDataGridView.BackgroundColor = System.Drawing.Color.Yellow;
            this.m_expiredReminderDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_expiredReminderDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_expiredReminderDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ReminderItem,
            this.MaintenanceText,
            this.ViewWorkInstruction,
            this.MaintenanceComplete});
            this.m_expiredReminderDataGridView.Location = new System.Drawing.Point(12, 25);
            this.m_expiredReminderDataGridView.Name = "m_expiredReminderDataGridView";
            this.m_expiredReminderDataGridView.ReadOnly = true;
            this.m_expiredReminderDataGridView.RowHeadersVisible = false;
            this.m_expiredReminderDataGridView.Size = new System.Drawing.Size(904, 250);
            this.m_expiredReminderDataGridView.TabIndex = 1;
            this.m_expiredReminderDataGridView.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_expiredReminderDataGridView_CellClick);
            // 
            // ReminderItem
            // 
            this.ReminderItem.FillWeight = 25.72973F;
            this.ReminderItem.HeaderText = "Maintenance Item";
            this.ReminderItem.Name = "ReminderItem";
            this.ReminderItem.ReadOnly = true;
            this.ReminderItem.Width = 150;
            // 
            // MaintenanceText
            // 
            this.MaintenanceText.FillWeight = 152.2843F;
            this.MaintenanceText.HeaderText = "Maintenance Text";
            this.MaintenanceText.Name = "MaintenanceText";
            this.MaintenanceText.ReadOnly = true;
            this.MaintenanceText.Width = 500;
            // 
            // ViewWorkInstruction
            // 
            this.ViewWorkInstruction.HeaderText = "Work Instruction";
            this.ViewWorkInstruction.Name = "ViewWorkInstruction";
            this.ViewWorkInstruction.ReadOnly = true;
            this.ViewWorkInstruction.Text = "View";
            this.ViewWorkInstruction.UseColumnTextForButtonValue = true;
            // 
            // MaintenanceComplete
            // 
            this.MaintenanceComplete.FillWeight = 121.986F;
            this.MaintenanceComplete.HeaderText = "Complete";
            this.MaintenanceComplete.Name = "MaintenanceComplete";
            this.MaintenanceComplete.ReadOnly = true;
            this.MaintenanceComplete.Text = "Close Maintenance Item";
            this.MaintenanceComplete.UseColumnTextForButtonValue = true;
            this.MaintenanceComplete.Width = 150;
            // 
            // ExpiredReminderForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Yellow;
            this.ClientSize = new System.Drawing.Size(928, 287);
            this.ControlBox = false;
            this.Controls.Add(this.m_expiredReminderDataGridView);
            this.Controls.Add(this.m_maintenanceItemsDueLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "ExpiredReminderForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Reminder Expired";
            this.TopMost = true;
            ((System.ComponentModel.ISupportInitialize)(this.m_expiredReminderDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_maintenanceItemsDueLabel;
        private System.Windows.Forms.DataGridView m_expiredReminderDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn ReminderItem;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaintenanceText;
        private System.Windows.Forms.DataGridViewButtonColumn ViewWorkInstruction;
        private System.Windows.Forms.DataGridViewButtonColumn MaintenanceComplete;
    }
}