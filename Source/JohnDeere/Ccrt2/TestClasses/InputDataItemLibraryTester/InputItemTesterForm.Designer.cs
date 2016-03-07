namespace InputDataItemLibraryTester
{
    partial class InputItemTesterForm
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
            this.m_dataItemLabel = new System.Windows.Forms.Label();
            this.m_minLenLabel = new System.Windows.Forms.Label();
            this.m_maxLenLabel = new System.Windows.Forms.Label();
            this.m_headerDataLabel = new System.Windows.Forms.Label();
            this.m_headerLengthLabel = new System.Windows.Forms.Label();
            this.m_trailerLabel = new System.Windows.Forms.Label();
            this.m_trailerLengthLabel = new System.Windows.Forms.Label();
            this.m_dataItemTextBox = new System.Windows.Forms.TextBox();
            this.m_minLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_maxLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_headerTextBox = new System.Windows.Forms.TextBox();
            this.m_headerLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_trailerTextBox = new System.Windows.Forms.TextBox();
            this.m_trailerLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_configuredDataItemsLabel = new System.Windows.Forms.Label();
            this.m_diDataGridView = new System.Windows.Forms.DataGridView();
            this.ItemName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MinLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.HeaderData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.HeaderLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TrailerData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TrailerLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ViewDataButtonCol = new System.Windows.Forms.DataGridViewButtonColumn();
            this.m_addItemButton = new System.Windows.Forms.Button();
            this.m_updateItemButton = new System.Windows.Forms.Button();
            this.m_removeItemButton = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.m_diDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_dataItemLabel
            // 
            this.m_dataItemLabel.AutoSize = true;
            this.m_dataItemLabel.Location = new System.Drawing.Point(43, 19);
            this.m_dataItemLabel.Name = "m_dataItemLabel";
            this.m_dataItemLabel.Size = new System.Drawing.Size(56, 13);
            this.m_dataItemLabel.TabIndex = 0;
            this.m_dataItemLabel.Text = "Data Item:";
            // 
            // m_minLenLabel
            // 
            this.m_minLenLabel.AutoSize = true;
            this.m_minLenLabel.Location = new System.Drawing.Point(12, 47);
            this.m_minLenLabel.Name = "m_minLenLabel";
            this.m_minLenLabel.Size = new System.Drawing.Size(87, 13);
            this.m_minLenLabel.TabIndex = 1;
            this.m_minLenLabel.Text = "Minimum Length:";
            // 
            // m_maxLenLabel
            // 
            this.m_maxLenLabel.AutoSize = true;
            this.m_maxLenLabel.Location = new System.Drawing.Point(9, 75);
            this.m_maxLenLabel.Name = "m_maxLenLabel";
            this.m_maxLenLabel.Size = new System.Drawing.Size(90, 13);
            this.m_maxLenLabel.TabIndex = 2;
            this.m_maxLenLabel.Text = "Maximum Length:";
            // 
            // m_headerDataLabel
            // 
            this.m_headerDataLabel.AutoSize = true;
            this.m_headerDataLabel.Location = new System.Drawing.Point(248, 19);
            this.m_headerDataLabel.Name = "m_headerDataLabel";
            this.m_headerDataLabel.Size = new System.Drawing.Size(71, 13);
            this.m_headerDataLabel.TabIndex = 3;
            this.m_headerDataLabel.Text = "Header Data:";
            // 
            // m_headerLengthLabel
            // 
            this.m_headerLengthLabel.AutoSize = true;
            this.m_headerLengthLabel.Location = new System.Drawing.Point(238, 47);
            this.m_headerLengthLabel.Name = "m_headerLengthLabel";
            this.m_headerLengthLabel.Size = new System.Drawing.Size(81, 13);
            this.m_headerLengthLabel.TabIndex = 4;
            this.m_headerLengthLabel.Text = "Header Length:";
            // 
            // m_trailerLabel
            // 
            this.m_trailerLabel.AutoSize = true;
            this.m_trailerLabel.Location = new System.Drawing.Point(474, 19);
            this.m_trailerLabel.Name = "m_trailerLabel";
            this.m_trailerLabel.Size = new System.Drawing.Size(65, 13);
            this.m_trailerLabel.TabIndex = 5;
            this.m_trailerLabel.Text = "Trailer Data:";
            // 
            // m_trailerLengthLabel
            // 
            this.m_trailerLengthLabel.AutoSize = true;
            this.m_trailerLengthLabel.Location = new System.Drawing.Point(464, 47);
            this.m_trailerLengthLabel.Name = "m_trailerLengthLabel";
            this.m_trailerLengthLabel.Size = new System.Drawing.Size(75, 13);
            this.m_trailerLengthLabel.TabIndex = 6;
            this.m_trailerLengthLabel.Text = "Trailer Length:";
            // 
            // m_dataItemTextBox
            // 
            this.m_dataItemTextBox.Location = new System.Drawing.Point(105, 16);
            this.m_dataItemTextBox.Name = "m_dataItemTextBox";
            this.m_dataItemTextBox.Size = new System.Drawing.Size(112, 20);
            this.m_dataItemTextBox.TabIndex = 7;
            // 
            // m_minLengthTextBox
            // 
            this.m_minLengthTextBox.Location = new System.Drawing.Point(105, 44);
            this.m_minLengthTextBox.Name = "m_minLengthTextBox";
            this.m_minLengthTextBox.Size = new System.Drawing.Size(60, 20);
            this.m_minLengthTextBox.TabIndex = 8;
            // 
            // m_maxLengthTextBox
            // 
            this.m_maxLengthTextBox.Location = new System.Drawing.Point(105, 72);
            this.m_maxLengthTextBox.Name = "m_maxLengthTextBox";
            this.m_maxLengthTextBox.Size = new System.Drawing.Size(60, 20);
            this.m_maxLengthTextBox.TabIndex = 9;
            // 
            // m_headerTextBox
            // 
            this.m_headerTextBox.Location = new System.Drawing.Point(325, 16);
            this.m_headerTextBox.Name = "m_headerTextBox";
            this.m_headerTextBox.Size = new System.Drawing.Size(112, 20);
            this.m_headerTextBox.TabIndex = 10;
            // 
            // m_headerLengthTextBox
            // 
            this.m_headerLengthTextBox.Location = new System.Drawing.Point(325, 44);
            this.m_headerLengthTextBox.Name = "m_headerLengthTextBox";
            this.m_headerLengthTextBox.Size = new System.Drawing.Size(60, 20);
            this.m_headerLengthTextBox.TabIndex = 11;
            // 
            // m_trailerTextBox
            // 
            this.m_trailerTextBox.Location = new System.Drawing.Point(545, 16);
            this.m_trailerTextBox.Name = "m_trailerTextBox";
            this.m_trailerTextBox.Size = new System.Drawing.Size(112, 20);
            this.m_trailerTextBox.TabIndex = 12;
            // 
            // m_trailerLengthTextBox
            // 
            this.m_trailerLengthTextBox.Location = new System.Drawing.Point(545, 42);
            this.m_trailerLengthTextBox.Name = "m_trailerLengthTextBox";
            this.m_trailerLengthTextBox.Size = new System.Drawing.Size(60, 20);
            this.m_trailerLengthTextBox.TabIndex = 13;
            // 
            // m_configuredDataItemsLabel
            // 
            this.m_configuredDataItemsLabel.AutoSize = true;
            this.m_configuredDataItemsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_configuredDataItemsLabel.Location = new System.Drawing.Point(12, 138);
            this.m_configuredDataItemsLabel.Name = "m_configuredDataItemsLabel";
            this.m_configuredDataItemsLabel.Size = new System.Drawing.Size(137, 13);
            this.m_configuredDataItemsLabel.TabIndex = 14;
            this.m_configuredDataItemsLabel.Text = "Configured Data Items:";
            // 
            // m_diDataGridView
            // 
            this.m_diDataGridView.AllowUserToAddRows = false;
            this.m_diDataGridView.AllowUserToDeleteRows = false;
            this.m_diDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_diDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_diDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ItemName,
            this.MinLength,
            this.MaxLength,
            this.HeaderData,
            this.HeaderLength,
            this.TrailerData,
            this.TrailerLength,
            this.ViewDataButtonCol});
            this.m_diDataGridView.Location = new System.Drawing.Point(15, 154);
            this.m_diDataGridView.Name = "m_diDataGridView";
            this.m_diDataGridView.ReadOnly = true;
            this.m_diDataGridView.RowHeadersVisible = false;
            this.m_diDataGridView.Size = new System.Drawing.Size(803, 122);
            this.m_diDataGridView.TabIndex = 15;
            this.m_diDataGridView.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_diDataGridView_CellClick);
            // 
            // ItemName
            // 
            this.ItemName.HeaderText = "Item";
            this.ItemName.Name = "ItemName";
            this.ItemName.ReadOnly = true;
            // 
            // MinLength
            // 
            this.MinLength.HeaderText = "Min Length";
            this.MinLength.Name = "MinLength";
            this.MinLength.ReadOnly = true;
            // 
            // MaxLength
            // 
            this.MaxLength.HeaderText = "Max Length";
            this.MaxLength.Name = "MaxLength";
            this.MaxLength.ReadOnly = true;
            // 
            // HeaderData
            // 
            this.HeaderData.HeaderText = "Header";
            this.HeaderData.Name = "HeaderData";
            this.HeaderData.ReadOnly = true;
            // 
            // HeaderLength
            // 
            this.HeaderLength.HeaderText = "Header Length";
            this.HeaderLength.Name = "HeaderLength";
            this.HeaderLength.ReadOnly = true;
            // 
            // TrailerData
            // 
            this.TrailerData.HeaderText = "Trailer";
            this.TrailerData.Name = "TrailerData";
            this.TrailerData.ReadOnly = true;
            // 
            // TrailerLength
            // 
            this.TrailerLength.HeaderText = "Trailer Length";
            this.TrailerLength.Name = "TrailerLength";
            this.TrailerLength.ReadOnly = true;
            // 
            // ViewDataButtonCol
            // 
            this.ViewDataButtonCol.HeaderText = "View Data";
            this.ViewDataButtonCol.Name = "ViewDataButtonCol";
            this.ViewDataButtonCol.ReadOnly = true;
            this.ViewDataButtonCol.Text = "View Settings";
            this.ViewDataButtonCol.UseColumnTextForButtonValue = true;
            // 
            // m_addItemButton
            // 
            this.m_addItemButton.Location = new System.Drawing.Point(737, 14);
            this.m_addItemButton.Name = "m_addItemButton";
            this.m_addItemButton.Size = new System.Drawing.Size(81, 23);
            this.m_addItemButton.TabIndex = 16;
            this.m_addItemButton.Text = "Add Item";
            this.m_addItemButton.UseVisualStyleBackColor = true;
            this.m_addItemButton.Click += new System.EventHandler(this.m_addItemButton_Click);
            // 
            // m_updateItemButton
            // 
            this.m_updateItemButton.Location = new System.Drawing.Point(737, 43);
            this.m_updateItemButton.Name = "m_updateItemButton";
            this.m_updateItemButton.Size = new System.Drawing.Size(81, 23);
            this.m_updateItemButton.TabIndex = 17;
            this.m_updateItemButton.Text = "Update Item";
            this.m_updateItemButton.UseVisualStyleBackColor = true;
            this.m_updateItemButton.Click += new System.EventHandler(this.m_updateItemButton_Click);
            // 
            // m_removeItemButton
            // 
            this.m_removeItemButton.Location = new System.Drawing.Point(737, 72);
            this.m_removeItemButton.Name = "m_removeItemButton";
            this.m_removeItemButton.Size = new System.Drawing.Size(81, 23);
            this.m_removeItemButton.TabIndex = 18;
            this.m_removeItemButton.Text = "Remove Item";
            this.m_removeItemButton.UseVisualStyleBackColor = true;
            this.m_removeItemButton.Click += new System.EventHandler(this.m_removeItemButton_Click);
            // 
            // InputItemTesterForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(830, 288);
            this.Controls.Add(this.m_removeItemButton);
            this.Controls.Add(this.m_updateItemButton);
            this.Controls.Add(this.m_addItemButton);
            this.Controls.Add(this.m_diDataGridView);
            this.Controls.Add(this.m_configuredDataItemsLabel);
            this.Controls.Add(this.m_trailerLengthTextBox);
            this.Controls.Add(this.m_trailerTextBox);
            this.Controls.Add(this.m_headerLengthTextBox);
            this.Controls.Add(this.m_headerTextBox);
            this.Controls.Add(this.m_maxLengthTextBox);
            this.Controls.Add(this.m_minLengthTextBox);
            this.Controls.Add(this.m_dataItemTextBox);
            this.Controls.Add(this.m_trailerLengthLabel);
            this.Controls.Add(this.m_trailerLabel);
            this.Controls.Add(this.m_headerLengthLabel);
            this.Controls.Add(this.m_headerDataLabel);
            this.Controls.Add(this.m_maxLenLabel);
            this.Controls.Add(this.m_minLenLabel);
            this.Controls.Add(this.m_dataItemLabel);
            this.Name = "InputItemTesterForm";
            this.Text = "Input Data Item Library Tester";
            ((System.ComponentModel.ISupportInitialize)(this.m_diDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_dataItemLabel;
        private System.Windows.Forms.Label m_minLenLabel;
        private System.Windows.Forms.Label m_maxLenLabel;
        private System.Windows.Forms.Label m_headerDataLabel;
        private System.Windows.Forms.Label m_headerLengthLabel;
        private System.Windows.Forms.Label m_trailerLabel;
        private System.Windows.Forms.Label m_trailerLengthLabel;
        private System.Windows.Forms.TextBox m_dataItemTextBox;
        private System.Windows.Forms.TextBox m_minLengthTextBox;
        private System.Windows.Forms.TextBox m_maxLengthTextBox;
        private System.Windows.Forms.TextBox m_headerTextBox;
        private System.Windows.Forms.TextBox m_headerLengthTextBox;
        private System.Windows.Forms.TextBox m_trailerTextBox;
        private System.Windows.Forms.TextBox m_trailerLengthTextBox;
        private System.Windows.Forms.Label m_configuredDataItemsLabel;
        private System.Windows.Forms.DataGridView m_diDataGridView;
        private System.Windows.Forms.Button m_addItemButton;
        private System.Windows.Forms.DataGridViewTextBoxColumn ItemName;
        private System.Windows.Forms.DataGridViewTextBoxColumn MinLength;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxLength;
        private System.Windows.Forms.DataGridViewTextBoxColumn HeaderData;
        private System.Windows.Forms.DataGridViewTextBoxColumn HeaderLength;
        private System.Windows.Forms.DataGridViewTextBoxColumn TrailerData;
        private System.Windows.Forms.DataGridViewTextBoxColumn TrailerLength;
        private System.Windows.Forms.DataGridViewButtonColumn ViewDataButtonCol;
        private System.Windows.Forms.Button m_updateItemButton;
        private System.Windows.Forms.Button m_removeItemButton;
    }
}

