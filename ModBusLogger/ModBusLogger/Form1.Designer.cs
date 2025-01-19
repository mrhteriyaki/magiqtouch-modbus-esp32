namespace ModBusLogger
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            this.btnStart = new System.Windows.Forms.Button();
            this.btnStop = new System.Windows.Forms.Button();
            this.chkHexMode = new System.Windows.Forms.CheckBox();
            this.txtCRCTest = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.lblcrcresult = new System.Windows.Forms.Label();
            this.tmrUpdate = new System.Windows.Forms.Timer(this.components);
            this.txtMonitor = new System.Windows.Forms.TextBox();
            this.btnCopy = new System.Windows.Forms.Button();
            this.txtByteMatchCount = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.chkLength = new System.Windows.Forms.CheckBox();
            this.chkExcludeCRC = new System.Windows.Forms.CheckBox();
            this.txtFilter = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.chkTimestamp = new System.Windows.Forms.CheckBox();
            this.txtCOMA = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txtCOMB = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.rbtStringMode = new System.Windows.Forms.RadioButton();
            this.rbtByteMode = new System.Windows.Forms.RadioButton();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(12, 12);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(75, 23);
            this.btnStart.TabIndex = 1;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // btnStop
            // 
            this.btnStop.Enabled = false;
            this.btnStop.Location = new System.Drawing.Point(93, 12);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(75, 23);
            this.btnStop.TabIndex = 2;
            this.btnStop.Text = "Stop";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // chkHexMode
            // 
            this.chkHexMode.AutoSize = true;
            this.chkHexMode.Location = new System.Drawing.Point(409, 114);
            this.chkHexMode.Name = "chkHexMode";
            this.chkHexMode.Size = new System.Drawing.Size(83, 17);
            this.chkHexMode.TabIndex = 3;
            this.chkHexMode.Text = "HEX Output";
            this.chkHexMode.UseVisualStyleBackColor = true;
            this.chkHexMode.CheckedChanged += new System.EventHandler(this.chkHexMode_CheckedChanged);
            // 
            // txtCRCTest
            // 
            this.txtCRCTest.Location = new System.Drawing.Point(164, 39);
            this.txtCRCTest.Name = "txtCRCTest";
            this.txtCRCTest.Size = new System.Drawing.Size(172, 20);
            this.txtCRCTest.TabIndex = 4;
            this.txtCRCTest.TextChanged += new System.EventHandler(this.txtCRCTest_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 42);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(113, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "CRC Packet Validator:";
            // 
            // lblcrcresult
            // 
            this.lblcrcresult.AutoSize = true;
            this.lblcrcresult.Location = new System.Drawing.Point(131, 42);
            this.lblcrcresult.Name = "lblcrcresult";
            this.lblcrcresult.Size = new System.Drawing.Size(27, 13);
            this.lblcrcresult.TabIndex = 6;
            this.lblcrcresult.Text = "N/A";
            // 
            // tmrUpdate
            // 
            this.tmrUpdate.Enabled = true;
            this.tmrUpdate.Interval = 200;
            this.tmrUpdate.Tick += new System.EventHandler(this.tmrUpdate_Tick);
            // 
            // txtMonitor
            // 
            this.txtMonitor.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtMonitor.Location = new System.Drawing.Point(12, 137);
            this.txtMonitor.Multiline = true;
            this.txtMonitor.Name = "txtMonitor";
            this.txtMonitor.Size = new System.Drawing.Size(1040, 574);
            this.txtMonitor.TabIndex = 7;
            this.txtMonitor.TextChanged += new System.EventHandler(this.txtMonitor_TextChanged);
            // 
            // btnCopy
            // 
            this.btnCopy.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCopy.Location = new System.Drawing.Point(942, 112);
            this.btnCopy.Name = "btnCopy";
            this.btnCopy.Size = new System.Drawing.Size(110, 23);
            this.btnCopy.TabIndex = 8;
            this.btnCopy.Text = "Copy to Clipboard";
            this.btnCopy.UseVisualStyleBackColor = true;
            this.btnCopy.Click += new System.EventHandler(this.button1_Click);
            // 
            // txtByteMatchCount
            // 
            this.txtByteMatchCount.Location = new System.Drawing.Point(100, 65);
            this.txtByteMatchCount.Name = "txtByteMatchCount";
            this.txtByteMatchCount.Size = new System.Drawing.Size(32, 20);
            this.txtByteMatchCount.TabIndex = 9;
            this.txtByteMatchCount.Text = "2";
            this.txtByteMatchCount.TextChanged += new System.EventHandler(this.txtByteMatchCount_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(13, 68);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(81, 13);
            this.label2.TabIndex = 10;
            this.label2.Text = "Bytes to Match:";
            // 
            // chkLength
            // 
            this.chkLength.AutoSize = true;
            this.chkLength.Checked = true;
            this.chkLength.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkLength.Location = new System.Drawing.Point(150, 67);
            this.chkLength.Name = "chkLength";
            this.chkLength.Size = new System.Drawing.Size(92, 17);
            this.chkLength.TabIndex = 11;
            this.chkLength.Text = "Length Match";
            this.chkLength.UseVisualStyleBackColor = true;
            this.chkLength.CheckedChanged += new System.EventHandler(this.chkLength_CheckedChanged);
            // 
            // chkExcludeCRC
            // 
            this.chkExcludeCRC.AutoSize = true;
            this.chkExcludeCRC.Location = new System.Drawing.Point(190, 114);
            this.chkExcludeCRC.Name = "chkExcludeCRC";
            this.chkExcludeCRC.Size = new System.Drawing.Size(89, 17);
            this.chkExcludeCRC.TabIndex = 12;
            this.chkExcludeCRC.Text = "Exclude CRC";
            this.chkExcludeCRC.UseVisualStyleBackColor = true;
            this.chkExcludeCRC.CheckedChanged += new System.EventHandler(this.chkShowCRC_CheckedChanged);
            // 
            // txtFilter
            // 
            this.txtFilter.Location = new System.Drawing.Point(12, 112);
            this.txtFilter.Name = "txtFilter";
            this.txtFilter.Size = new System.Drawing.Size(172, 20);
            this.txtFilter.TabIndex = 13;
            this.txtFilter.TextChanged += new System.EventHandler(this.txtFilter_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 96);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(69, 13);
            this.label3.TabIndex = 14;
            this.label3.Text = "Display Filter:";
            // 
            // chkTimestamp
            // 
            this.chkTimestamp.AutoSize = true;
            this.chkTimestamp.Location = new System.Drawing.Point(285, 114);
            this.chkTimestamp.Name = "chkTimestamp";
            this.chkTimestamp.Size = new System.Drawing.Size(118, 17);
            this.chkTimestamp.TabIndex = 15;
            this.chkTimestamp.Text = "Exclude Timestamp";
            this.chkTimestamp.UseVisualStyleBackColor = true;
            this.chkTimestamp.CheckedChanged += new System.EventHandler(this.chkTimestamp_CheckedChanged);
            // 
            // txtCOMA
            // 
            this.txtCOMA.Location = new System.Drawing.Point(213, 14);
            this.txtCOMA.Name = "txtCOMA";
            this.txtCOMA.Size = new System.Drawing.Size(65, 20);
            this.txtCOMA.TabIndex = 16;
            this.txtCOMA.Text = "COM4";
            this.txtCOMA.TextChanged += new System.EventHandler(this.txtCOMA_TextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(173, 17);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(34, 13);
            this.label4.TabIndex = 17;
            this.label4.Text = "COM:";
            // 
            // txtCOMB
            // 
            this.txtCOMB.Location = new System.Drawing.Point(325, 14);
            this.txtCOMB.Name = "txtCOMB";
            this.txtCOMB.Size = new System.Drawing.Size(65, 20);
            this.txtCOMB.TabIndex = 18;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(285, 17);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(40, 13);
            this.label5.TabIndex = 19;
            this.label5.Text = "COM2:";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.rbtStringMode);
            this.groupBox1.Controls.Add(this.rbtByteMode);
            this.groupBox1.Location = new System.Drawing.Point(409, 9);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(308, 75);
            this.groupBox1.TabIndex = 20;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Data Input Mode";
            // 
            // rbtStringMode
            // 
            this.rbtStringMode.AutoSize = true;
            this.rbtStringMode.Location = new System.Drawing.Point(6, 42);
            this.rbtStringMode.Name = "rbtStringMode";
            this.rbtStringMode.Size = new System.Drawing.Size(187, 17);
            this.rbtStringMode.TabIndex = 1;
            this.rbtStringMode.Text = "String (S1 D1 D2 D3 CRC1 CRC2)";
            this.rbtStringMode.UseVisualStyleBackColor = true;
            this.rbtStringMode.CheckedChanged += new System.EventHandler(this.rbtStringMode_CheckedChanged);
            // 
            // rbtByteMode
            // 
            this.rbtByteMode.AutoSize = true;
            this.rbtByteMode.Checked = true;
            this.rbtByteMode.Location = new System.Drawing.Point(6, 19);
            this.rbtByteMode.Name = "rbtByteMode";
            this.rbtByteMode.Size = new System.Drawing.Size(128, 17);
            this.rbtByteMode.TabIndex = 0;
            this.rbtByteMode.TabStop = true;
            this.rbtByteMode.Text = "Data sent as raw byte";
            this.rbtByteMode.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1064, 723);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.txtCOMB);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.txtCOMA);
            this.Controls.Add(this.chkTimestamp);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txtFilter);
            this.Controls.Add(this.chkExcludeCRC);
            this.Controls.Add(this.chkLength);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtByteMatchCount);
            this.Controls.Add(this.btnCopy);
            this.Controls.Add(this.txtMonitor);
            this.Controls.Add(this.lblcrcresult);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtCRCTest);
            this.Controls.Add(this.chkHexMode);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.btnStart);
            this.Name = "Form1";
            this.Text = "Modbus Monitor and Relay";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.CheckBox chkHexMode;
        private System.Windows.Forms.TextBox txtCRCTest;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lblcrcresult;
        private System.Windows.Forms.Timer tmrUpdate;
        private System.Windows.Forms.TextBox txtMonitor;
        private System.Windows.Forms.Button btnCopy;
        private System.Windows.Forms.TextBox txtByteMatchCount;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox chkLength;
        private System.Windows.Forms.CheckBox chkExcludeCRC;
        private System.Windows.Forms.TextBox txtFilter;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox chkTimestamp;
        private System.Windows.Forms.TextBox txtCOMA;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtCOMB;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton rbtStringMode;
        private System.Windows.Forms.RadioButton rbtByteMode;
    }
}

