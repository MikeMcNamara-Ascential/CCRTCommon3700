namespace ModuleCommServer
{
    partial class BrakeModuleConfigForm
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
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_moduleLabel = new System.Windows.Forms.Label();
            this.m_absTypeComboBox = new System.Windows.Forms.ComboBox();
            this.m_reqIdLabel = new System.Windows.Forms.Label();
            this.m_reqId = new System.Windows.Forms.Label();
            this.m_responseLabel = new System.Windows.Forms.Label();
            this.m_respId = new System.Windows.Forms.Label();
            this.m_protocolLabel = new System.Windows.Forms.Label();
            this.m_protocol = new System.Windows.Forms.Label();
            this.m_busLabel = new System.Windows.Forms.Label();
            this.m_bus = new System.Windows.Forms.Label();
            this.m_msgDataGridView = new System.Windows.Forms.DataGridView();
            this.MessageName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TransmitMessage = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_addButton = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.m_msgDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(579, 371);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 0;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            this.m_closeButton.Click += new System.EventHandler(this.m_closeButton_Click);
            // 
            // m_moduleLabel
            // 
            this.m_moduleLabel.AutoSize = true;
            this.m_moduleLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_moduleLabel.Location = new System.Drawing.Point(12, 23);
            this.m_moduleLabel.Name = "m_moduleLabel";
            this.m_moduleLabel.Size = new System.Drawing.Size(89, 17);
            this.m_moduleLabel.TabIndex = 1;
            this.m_moduleLabel.Text = "ABS Module:";
            // 
            // m_absTypeComboBox
            // 
            this.m_absTypeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_absTypeComboBox.FormattingEnabled = true;
            this.m_absTypeComboBox.Location = new System.Drawing.Point(107, 22);
            this.m_absTypeComboBox.Name = "m_absTypeComboBox";
            this.m_absTypeComboBox.Size = new System.Drawing.Size(156, 21);
            this.m_absTypeComboBox.TabIndex = 2;
            this.m_absTypeComboBox.SelectedIndexChanged += new System.EventHandler(this.m_absTypeComboBox_SelectedIndexChanged);
            // 
            // m_reqIdLabel
            // 
            this.m_reqIdLabel.AutoSize = true;
            this.m_reqIdLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_reqIdLabel.Location = new System.Drawing.Point(12, 76);
            this.m_reqIdLabel.Name = "m_reqIdLabel";
            this.m_reqIdLabel.Size = new System.Drawing.Size(82, 17);
            this.m_reqIdLabel.TabIndex = 3;
            this.m_reqIdLabel.Text = "Request ID:";
            // 
            // m_reqId
            // 
            this.m_reqId.AutoSize = true;
            this.m_reqId.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_reqId.Location = new System.Drawing.Point(119, 76);
            this.m_reqId.Name = "m_reqId";
            this.m_reqId.Size = new System.Drawing.Size(46, 17);
            this.m_reqId.TabIndex = 4;
            this.m_reqId.Text = "label1";
            // 
            // m_responseLabel
            // 
            this.m_responseLabel.AutoSize = true;
            this.m_responseLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_responseLabel.Location = new System.Drawing.Point(12, 102);
            this.m_responseLabel.Name = "m_responseLabel";
            this.m_responseLabel.Size = new System.Drawing.Size(93, 17);
            this.m_responseLabel.TabIndex = 5;
            this.m_responseLabel.Text = "Response ID:";
            // 
            // m_respId
            // 
            this.m_respId.AutoSize = true;
            this.m_respId.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_respId.Location = new System.Drawing.Point(119, 102);
            this.m_respId.Name = "m_respId";
            this.m_respId.Size = new System.Drawing.Size(46, 17);
            this.m_respId.TabIndex = 6;
            this.m_respId.Text = "label1";
            // 
            // m_protocolLabel
            // 
            this.m_protocolLabel.AutoSize = true;
            this.m_protocolLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_protocolLabel.Location = new System.Drawing.Point(263, 76);
            this.m_protocolLabel.Name = "m_protocolLabel";
            this.m_protocolLabel.Size = new System.Drawing.Size(164, 17);
            this.m_protocolLabel.TabIndex = 7;
            this.m_protocolLabel.Text = "Communication Protocol:";
            // 
            // m_protocol
            // 
            this.m_protocol.AutoSize = true;
            this.m_protocol.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_protocol.Location = new System.Drawing.Point(451, 76);
            this.m_protocol.Name = "m_protocol";
            this.m_protocol.Size = new System.Drawing.Size(46, 17);
            this.m_protocol.TabIndex = 8;
            this.m_protocol.Text = "label1";
            // 
            // m_busLabel
            // 
            this.m_busLabel.AutoSize = true;
            this.m_busLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_busLabel.Location = new System.Drawing.Point(263, 102);
            this.m_busLabel.Name = "m_busLabel";
            this.m_busLabel.Size = new System.Drawing.Size(136, 17);
            this.m_busLabel.TabIndex = 9;
            this.m_busLabel.Text = "Communication Bus:";
            // 
            // m_bus
            // 
            this.m_bus.AutoSize = true;
            this.m_bus.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_bus.Location = new System.Drawing.Point(451, 102);
            this.m_bus.Name = "m_bus";
            this.m_bus.Size = new System.Drawing.Size(46, 17);
            this.m_bus.TabIndex = 10;
            this.m_bus.Text = "label1";
            // 
            // m_msgDataGridView
            // 
            this.m_msgDataGridView.AllowUserToAddRows = false;
            this.m_msgDataGridView.AllowUserToDeleteRows = false;
            this.m_msgDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_msgDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.AllCells;
            this.m_msgDataGridView.BackgroundColor = System.Drawing.Color.White;
            this.m_msgDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_msgDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_msgDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.MessageName,
            this.TransmitMessage});
            this.m_msgDataGridView.Location = new System.Drawing.Point(15, 146);
            this.m_msgDataGridView.Name = "m_msgDataGridView";
            this.m_msgDataGridView.ReadOnly = true;
            this.m_msgDataGridView.RowHeadersVisible = false;
            this.m_msgDataGridView.Size = new System.Drawing.Size(639, 219);
            this.m_msgDataGridView.TabIndex = 11;
            // 
            // MessageName
            // 
            this.MessageName.HeaderText = "Message Name";
            this.MessageName.Name = "MessageName";
            this.MessageName.ReadOnly = true;
            this.MessageName.Width = 97;
            // 
            // TransmitMessage
            // 
            this.TransmitMessage.HeaderText = "Transmit Message";
            this.TransmitMessage.Name = "TransmitMessage";
            this.TransmitMessage.ReadOnly = true;
            this.TransmitMessage.Width = 108;
            // 
            // m_addButton
            // 
            this.m_addButton.Location = new System.Drawing.Point(12, 371);
            this.m_addButton.Name = "m_addButton";
            this.m_addButton.Size = new System.Drawing.Size(75, 23);
            this.m_addButton.TabIndex = 12;
            this.m_addButton.Text = "Add Module";
            this.m_addButton.UseVisualStyleBackColor = true;
            this.m_addButton.Click += new System.EventHandler(this.m_addButton_Click);
            // 
            // BrakeModuleConfigForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(666, 406);
            this.ControlBox = false;
            this.Controls.Add(this.m_addButton);
            this.Controls.Add(this.m_msgDataGridView);
            this.Controls.Add(this.m_bus);
            this.Controls.Add(this.m_busLabel);
            this.Controls.Add(this.m_protocol);
            this.Controls.Add(this.m_protocolLabel);
            this.Controls.Add(this.m_respId);
            this.Controls.Add(this.m_responseLabel);
            this.Controls.Add(this.m_reqId);
            this.Controls.Add(this.m_reqIdLabel);
            this.Controls.Add(this.m_absTypeComboBox);
            this.Controls.Add(this.m_moduleLabel);
            this.Controls.Add(this.m_closeButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "BrakeModuleConfigForm";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Brake Module Configuration";
            ((System.ComponentModel.ISupportInitialize)(this.m_msgDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Label m_moduleLabel;
        private System.Windows.Forms.ComboBox m_absTypeComboBox;
        private System.Windows.Forms.Label m_reqIdLabel;
        private System.Windows.Forms.Label m_reqId;
        private System.Windows.Forms.Label m_responseLabel;
        private System.Windows.Forms.Label m_respId;
        private System.Windows.Forms.Label m_protocolLabel;
        private System.Windows.Forms.Label m_protocol;
        private System.Windows.Forms.Label m_busLabel;
        private System.Windows.Forms.Label m_bus;
        private System.Windows.Forms.DataGridView m_msgDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn MessageName;
        private System.Windows.Forms.DataGridViewTextBoxColumn TransmitMessage;
        private System.Windows.Forms.Button m_addButton;
    }
}