namespace BomFileProcessor
{
    partial class BrakeForceSetupForm
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
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_modelCodeLabel = new System.Windows.Forms.Label();
            this.m_brakeForceParamLabel = new System.Windows.Forms.Label();
            this.m_brkFrcParamDataGridView = new System.Windows.Forms.DataGridView();
            this.Axle = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxDragForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MinBrakeForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxBrakeForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.SideToSideBalance = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.FrontToRearBalMin = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.FrontToRearBalMax = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_modelCodeListComboBox = new System.Windows.Forms.ComboBox();
            ((System.ComponentModel.ISupportInitialize)(this.m_brkFrcParamDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.Location = new System.Drawing.Point(386, 259);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 0;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(12, 259);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 1;
            this.m_cancelButton.Text = "Close";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // m_modelCodeLabel
            // 
            this.m_modelCodeLabel.AutoSize = true;
            this.m_modelCodeLabel.Location = new System.Drawing.Point(12, 24);
            this.m_modelCodeLabel.Name = "m_modelCodeLabel";
            this.m_modelCodeLabel.Size = new System.Drawing.Size(67, 13);
            this.m_modelCodeLabel.TabIndex = 2;
            this.m_modelCodeLabel.Text = "Model Code:";
            // 
            // m_brakeForceParamLabel
            // 
            this.m_brakeForceParamLabel.AutoSize = true;
            this.m_brakeForceParamLabel.Location = new System.Drawing.Point(12, 63);
            this.m_brakeForceParamLabel.Name = "m_brakeForceParamLabel";
            this.m_brakeForceParamLabel.Size = new System.Drawing.Size(124, 13);
            this.m_brakeForceParamLabel.TabIndex = 4;
            this.m_brakeForceParamLabel.Text = "Brake Force Parameters:";
            // 
            // m_brkFrcParamDataGridView
            // 
            this.m_brkFrcParamDataGridView.AllowUserToAddRows = false;
            this.m_brkFrcParamDataGridView.AllowUserToDeleteRows = false;
            this.m_brkFrcParamDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.m_brkFrcParamDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_brkFrcParamDataGridView.BackgroundColor = System.Drawing.Color.LightSteelBlue;
            this.m_brkFrcParamDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_brkFrcParamDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_brkFrcParamDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Axle,
            this.MaxDragForce,
            this.MinBrakeForce,
            this.MaxBrakeForce,
            this.SideToSideBalance,
            this.FrontToRearBalMin,
            this.FrontToRearBalMax});
            this.m_brkFrcParamDataGridView.Location = new System.Drawing.Point(15, 79);
            this.m_brkFrcParamDataGridView.Name = "m_brkFrcParamDataGridView";
            this.m_brkFrcParamDataGridView.RowHeadersVisible = false;
            this.m_brkFrcParamDataGridView.Size = new System.Drawing.Size(446, 174);
            this.m_brkFrcParamDataGridView.TabIndex = 5;
            this.m_brkFrcParamDataGridView.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_brkFrcParamDataGridView_CellContentClick);
            // 
            // Axle
            // 
            this.Axle.HeaderText = "Axle";
            this.Axle.Name = "Axle";
            this.Axle.ReadOnly = true;
            // 
            // MaxDragForce
            // 
            this.MaxDragForce.HeaderText = "Maximum Drag Force (lbf)";
            this.MaxDragForce.Name = "MaxDragForce";
            // 
            // MinBrakeForce
            // 
            this.MinBrakeForce.HeaderText = "Minimum Brake Force (lbf)";
            this.MinBrakeForce.Name = "MinBrakeForce";
            // 
            // MaxBrakeForce
            // 
            this.MaxBrakeForce.HeaderText = "Maximum Brake Force (lbf)";
            this.MaxBrakeForce.Name = "MaxBrakeForce";
            // 
            // SideToSideBalance
            // 
            this.SideToSideBalance.HeaderText = "Side to Side Balance (offset from 50%)";
            this.SideToSideBalance.Name = "SideToSideBalance";
            // 
            // FrontToRearBalMin
            // 
            this.FrontToRearBalMin.HeaderText = "Front To Rear Balance Minimum (%)";
            this.FrontToRearBalMin.Name = "FrontToRearBalMin";
            // 
            // FrontToRearBalMax
            // 
            this.FrontToRearBalMax.HeaderText = "Front To Rear Balance Maximum (%)";
            this.FrontToRearBalMax.Name = "FrontToRearBalMax";
            // 
            // m_modelCodeListComboBox
            // 
            this.m_modelCodeListComboBox.FormattingEnabled = true;
            this.m_modelCodeListComboBox.Location = new System.Drawing.Point(85, 21);
            this.m_modelCodeListComboBox.Name = "m_modelCodeListComboBox";
            this.m_modelCodeListComboBox.Size = new System.Drawing.Size(121, 21);
            this.m_modelCodeListComboBox.TabIndex = 6;
            this.m_modelCodeListComboBox.SelectedIndexChanged += new System.EventHandler(this.m_modelCodeListComboBox_SelectedIndexChanged);
            // 
            // BrakeForceSetupForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(473, 294);
            this.ControlBox = false;
            this.Controls.Add(this.m_modelCodeListComboBox);
            this.Controls.Add(this.m_brkFrcParamDataGridView);
            this.Controls.Add(this.m_brakeForceParamLabel);
            this.Controls.Add(this.m_modelCodeLabel);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_saveButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Name = "BrakeForceSetupForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Brake Force Setup";
            ((System.ComponentModel.ISupportInitialize)(this.m_brkFrcParamDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Label m_modelCodeLabel;
        private System.Windows.Forms.Label m_brakeForceParamLabel;
        private System.Windows.Forms.DataGridView m_brkFrcParamDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn Axle;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxDragForce;
        private System.Windows.Forms.DataGridViewTextBoxColumn MinBrakeForce;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxBrakeForce;
        private System.Windows.Forms.DataGridViewTextBoxColumn SideToSideBalance;
        private System.Windows.Forms.ComboBox m_modelCodeListComboBox;
        private System.Windows.Forms.DataGridViewTextBoxColumn FrontToRearBalMin;
        private System.Windows.Forms.DataGridViewTextBoxColumn FrontToRearBalMax;

    }
}