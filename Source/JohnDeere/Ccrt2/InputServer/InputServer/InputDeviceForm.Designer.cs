namespace InputServer
{
    partial class InputDeviceForm
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
            this.m_portSettingsLabel = new System.Windows.Forms.Label();
            this.m_inputDeviceLabel = new System.Windows.Forms.Label();
            this.m_inputDeviceComboBox = new System.Windows.Forms.ComboBox();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_monitorDataCheckBox = new System.Windows.Forms.CheckBox();
            this.m_saveSettingsButton = new System.Windows.Forms.Button();
            this.m_removeDeviceButton = new System.Windows.Forms.Button();
            this.m_devTypeLabel = new System.Windows.Forms.Label();
            this.m_devTypeComboBox = new System.Windows.Forms.ComboBox();
            this.m_dropDownPortSettingsDataGridView = new System.Windows.Forms.DataGridView();
            this.DropDownLabels = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DropDownItems = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.m_textItemPortSettingsDataGridView = new System.Windows.Forms.DataGridView();
            this.TextItemLabel = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TextItemValue = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.m_dropDownPortSettingsDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_textItemPortSettingsDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_portSettingsLabel
            // 
            this.m_portSettingsLabel.AutoSize = true;
            this.m_portSettingsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_portSettingsLabel.Location = new System.Drawing.Point(13, 98);
            this.m_portSettingsLabel.Name = "m_portSettingsLabel";
            this.m_portSettingsLabel.Size = new System.Drawing.Size(100, 16);
            this.m_portSettingsLabel.TabIndex = 0;
            this.m_portSettingsLabel.Text = "Port Settings:";
            // 
            // m_inputDeviceLabel
            // 
            this.m_inputDeviceLabel.AutoSize = true;
            this.m_inputDeviceLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_inputDeviceLabel.Location = new System.Drawing.Point(12, 9);
            this.m_inputDeviceLabel.Name = "m_inputDeviceLabel";
            this.m_inputDeviceLabel.Size = new System.Drawing.Size(98, 16);
            this.m_inputDeviceLabel.TabIndex = 1;
            this.m_inputDeviceLabel.Text = "Input Device:";
            // 
            // m_inputDeviceComboBox
            // 
            this.m_inputDeviceComboBox.FormattingEnabled = true;
            this.m_inputDeviceComboBox.Location = new System.Drawing.Point(234, 8);
            this.m_inputDeviceComboBox.Name = "m_inputDeviceComboBox";
            this.m_inputDeviceComboBox.Size = new System.Drawing.Size(164, 21);
            this.m_inputDeviceComboBox.TabIndex = 2;
            this.m_inputDeviceComboBox.SelectedIndexChanged += new System.EventHandler(this.m_inputDeviceComboBox_SelectedIndexChanged);
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(12, 401);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 5;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            // 
            // m_monitorDataCheckBox
            // 
            this.m_monitorDataCheckBox.AutoSize = true;
            this.m_monitorDataCheckBox.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.m_monitorDataCheckBox.Location = new System.Drawing.Point(426, 56);
            this.m_monitorDataCheckBox.Name = "m_monitorDataCheckBox";
            this.m_monitorDataCheckBox.Size = new System.Drawing.Size(105, 17);
            this.m_monitorDataCheckBox.TabIndex = 18;
            this.m_monitorDataCheckBox.Text = "Monitor For Data";
            this.m_monitorDataCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_saveSettingsButton
            // 
            this.m_saveSettingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveSettingsButton.Location = new System.Drawing.Point(573, 401);
            this.m_saveSettingsButton.Name = "m_saveSettingsButton";
            this.m_saveSettingsButton.Size = new System.Drawing.Size(89, 23);
            this.m_saveSettingsButton.TabIndex = 19;
            this.m_saveSettingsButton.Text = "Save Settings";
            this.m_saveSettingsButton.UseVisualStyleBackColor = true;
            this.m_saveSettingsButton.Click += new System.EventHandler(this.m_saveSettingsButton_Click);
            // 
            // m_removeDeviceButton
            // 
            this.m_removeDeviceButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_removeDeviceButton.Location = new System.Drawing.Point(467, 401);
            this.m_removeDeviceButton.Name = "m_removeDeviceButton";
            this.m_removeDeviceButton.Size = new System.Drawing.Size(100, 23);
            this.m_removeDeviceButton.TabIndex = 20;
            this.m_removeDeviceButton.Text = "Remove Device";
            this.m_removeDeviceButton.UseVisualStyleBackColor = true;
            this.m_removeDeviceButton.Click += new System.EventHandler(this.m_removeDeviceButton_Click);
            // 
            // m_devTypeLabel
            // 
            this.m_devTypeLabel.AutoSize = true;
            this.m_devTypeLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_devTypeLabel.Location = new System.Drawing.Point(13, 55);
            this.m_devTypeLabel.Name = "m_devTypeLabel";
            this.m_devTypeLabel.Size = new System.Drawing.Size(103, 17);
            this.m_devTypeLabel.TabIndex = 21;
            this.m_devTypeLabel.Text = "Device Type:";
            // 
            // m_devTypeComboBox
            // 
            this.m_devTypeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_devTypeComboBox.FormattingEnabled = true;
            this.m_devTypeComboBox.Items.AddRange(new object[] {
            "Generic",
            "Maple Keypad"});
            this.m_devTypeComboBox.Location = new System.Drawing.Point(234, 54);
            this.m_devTypeComboBox.Name = "m_devTypeComboBox";
            this.m_devTypeComboBox.Size = new System.Drawing.Size(116, 21);
            this.m_devTypeComboBox.TabIndex = 22;
            // 
            // m_dropDownPortSettingsDataGridView
            // 
            this.m_dropDownPortSettingsDataGridView.AllowUserToAddRows = false;
            this.m_dropDownPortSettingsDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_dropDownPortSettingsDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_dropDownPortSettingsDataGridView.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
            this.m_dropDownPortSettingsDataGridView.BackgroundColor = System.Drawing.SystemColors.Control;
            this.m_dropDownPortSettingsDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_dropDownPortSettingsDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_dropDownPortSettingsDataGridView.ColumnHeadersVisible = false;
            this.m_dropDownPortSettingsDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.DropDownLabels,
            this.DropDownItems});
            this.m_dropDownPortSettingsDataGridView.Location = new System.Drawing.Point(16, 117);
            this.m_dropDownPortSettingsDataGridView.Name = "m_dropDownPortSettingsDataGridView";
            this.m_dropDownPortSettingsDataGridView.RowHeadersVisible = false;
            this.m_dropDownPortSettingsDataGridView.Size = new System.Drawing.Size(315, 253);
            this.m_dropDownPortSettingsDataGridView.TabIndex = 23;
            // 
            // DropDownLabels
            // 
            this.DropDownLabels.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.DropDownLabels.HeaderText = "Drop Down Items";
            this.DropDownLabels.Name = "DropDownLabels";
            this.DropDownLabels.ReadOnly = true;
            // 
            // DropDownItems
            // 
            this.DropDownItems.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.DropDownItems.HeaderText = "Drop Down Items";
            this.DropDownItems.Name = "DropDownItems";
            this.DropDownItems.Sorted = true;
            this.DropDownItems.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
            // 
            // m_textItemPortSettingsDataGridView
            // 
            this.m_textItemPortSettingsDataGridView.AllowUserToAddRows = false;
            this.m_textItemPortSettingsDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_textItemPortSettingsDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_textItemPortSettingsDataGridView.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
            this.m_textItemPortSettingsDataGridView.BackgroundColor = System.Drawing.SystemColors.Control;
            this.m_textItemPortSettingsDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_textItemPortSettingsDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_textItemPortSettingsDataGridView.ColumnHeadersVisible = false;
            this.m_textItemPortSettingsDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.TextItemLabel,
            this.TextItemValue});
            this.m_textItemPortSettingsDataGridView.Location = new System.Drawing.Point(336, 117);
            this.m_textItemPortSettingsDataGridView.Name = "m_textItemPortSettingsDataGridView";
            this.m_textItemPortSettingsDataGridView.RowHeadersVisible = false;
            this.m_textItemPortSettingsDataGridView.Size = new System.Drawing.Size(326, 253);
            this.m_textItemPortSettingsDataGridView.TabIndex = 24;
            // 
            // TextItemLabel
            // 
            this.TextItemLabel.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.TextItemLabel.HeaderText = "Text Item Label";
            this.TextItemLabel.Name = "TextItemLabel";
            this.TextItemLabel.ReadOnly = true;
            // 
            // TextItemValue
            // 
            this.TextItemValue.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.TextItemValue.HeaderText = "Text Item Value";
            this.TextItemValue.Name = "TextItemValue";
            // 
            // InputDeviceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(674, 436);
            this.Controls.Add(this.m_textItemPortSettingsDataGridView);
            this.Controls.Add(this.m_dropDownPortSettingsDataGridView);
            this.Controls.Add(this.m_devTypeComboBox);
            this.Controls.Add(this.m_devTypeLabel);
            this.Controls.Add(this.m_removeDeviceButton);
            this.Controls.Add(this.m_saveSettingsButton);
            this.Controls.Add(this.m_monitorDataCheckBox);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_inputDeviceComboBox);
            this.Controls.Add(this.m_inputDeviceLabel);
            this.Controls.Add(this.m_portSettingsLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "InputDeviceForm";
            this.Text = "Input Device Configuration";
            ((System.ComponentModel.ISupportInitialize)(this.m_dropDownPortSettingsDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_textItemPortSettingsDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_portSettingsLabel;
        private System.Windows.Forms.Label m_inputDeviceLabel;
        private System.Windows.Forms.ComboBox m_inputDeviceComboBox;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.CheckBox m_monitorDataCheckBox;
        private System.Windows.Forms.Button m_saveSettingsButton;
        private System.Windows.Forms.Button m_removeDeviceButton;
        private System.Windows.Forms.Label m_devTypeLabel;
        private System.Windows.Forms.ComboBox m_devTypeComboBox;
        private System.Windows.Forms.DataGridView m_dropDownPortSettingsDataGridView;
        private System.Windows.Forms.DataGridView m_textItemPortSettingsDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn TextItemLabel;
        private System.Windows.Forms.DataGridViewTextBoxColumn TextItemValue;
        private System.Windows.Forms.DataGridViewTextBoxColumn DropDownLabels;
        private System.Windows.Forms.DataGridViewComboBoxColumn DropDownItems;
    }
}