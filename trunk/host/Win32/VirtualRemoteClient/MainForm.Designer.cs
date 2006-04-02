namespace VirtualRemote
{
	partial class MainForm
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
			this.menuStrip1 = new System.Windows.Forms.MenuStrip();
			this.connectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.connectToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
			this.disconnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
			this.launchTAPToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.screenshotToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.memoryUsageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.systemStateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem6 = new System.Windows.Forms.ToolStripSeparator();
			this.timeshiftOffToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.timeshiftOnToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem5 = new System.Windows.Forms.ToolStripSeparator();
			this.rebootToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.shutdownToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
			this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.tuneVideoCardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
			this.clearDebugOutputToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
			this.toolStripMenuItem7 = new System.Windows.Forms.ToolStripSeparator();
			this.sendToppy2CodesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.serialPort = new System.IO.Ports.SerialPort(this.components);
			this.repeatTimer = new System.Windows.Forms.Timer(this.components);
			this.debugOutput = new System.Windows.Forms.TextBox();
			this.showVideoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.showDebugOutputToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.alwaysOnTopToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.tF5800RemoteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.remotePanel = new VirtualRemote.HotspotPanel();
			this.videoDisplay = new VirtualRemote.VideoControl();
			this.menuStrip1.SuspendLayout();
			this.SuspendLayout();
			// 
			// menuStrip1
			// 
			this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connectToolStripMenuItem,
            this.viewToolStripMenuItem,
            this.helpToolStripMenuItem});
			this.menuStrip1.Location = new System.Drawing.Point(0, 0);
			this.menuStrip1.Name = "menuStrip1";
			this.menuStrip1.Size = new System.Drawing.Size(569, 24);
			this.menuStrip1.TabIndex = 0;
			this.menuStrip1.Text = "menuStrip1";
			// 
			// connectToolStripMenuItem
			// 
			this.connectToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connectToolStripMenuItem1,
            this.disconnectToolStripMenuItem,
            this.toolStripMenuItem3,
            this.launchTAPToolStripMenuItem,
            this.screenshotToolStripMenuItem,
            this.memoryUsageToolStripMenuItem,
            this.systemStateToolStripMenuItem,
            this.toolStripMenuItem6,
            this.timeshiftOffToolStripMenuItem,
            this.timeshiftOnToolStripMenuItem,
            this.toolStripMenuItem5,
            this.rebootToolStripMenuItem,
            this.shutdownToolStripMenuItem,
            this.toolStripMenuItem4,
            this.exitToolStripMenuItem});
			this.connectToolStripMenuItem.Name = "connectToolStripMenuItem";
			this.connectToolStripMenuItem.Size = new System.Drawing.Size(66, 20);
			this.connectToolStripMenuItem.Text = "&Command";
			this.connectToolStripMenuItem.DropDownOpening += new System.EventHandler(this.menuFile_Popup);
			// 
			// connectToolStripMenuItem1
			// 
			this.connectToolStripMenuItem1.Name = "connectToolStripMenuItem1";
			this.connectToolStripMenuItem1.Size = new System.Drawing.Size(177, 22);
			this.connectToolStripMenuItem1.Text = "&Connect";
			this.connectToolStripMenuItem1.Click += new System.EventHandler(this.menuConnect_Click);
			// 
			// disconnectToolStripMenuItem
			// 
			this.disconnectToolStripMenuItem.Name = "disconnectToolStripMenuItem";
			this.disconnectToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.disconnectToolStripMenuItem.Text = "&Disconnect";
			this.disconnectToolStripMenuItem.Click += new System.EventHandler(this.menuDisconnect_Click);
			// 
			// toolStripMenuItem3
			// 
			this.toolStripMenuItem3.Name = "toolStripMenuItem3";
			this.toolStripMenuItem3.Size = new System.Drawing.Size(174, 6);
			// 
			// launchTAPToolStripMenuItem
			// 
			this.launchTAPToolStripMenuItem.Enabled = false;
			this.launchTAPToolStripMenuItem.Image = global::VirtualRemote.Properties.Resources.run;
			this.launchTAPToolStripMenuItem.Name = "launchTAPToolStripMenuItem";
			this.launchTAPToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.launchTAPToolStripMenuItem.Text = "Launch TAP";
			this.launchTAPToolStripMenuItem.Click += new System.EventHandler(this.launchTAPToolStripMenuItem_Click);
			// 
			// screenshotToolStripMenuItem
			// 
			this.screenshotToolStripMenuItem.Image = global::VirtualRemote.Properties.Resources.photo;
			this.screenshotToolStripMenuItem.Name = "screenshotToolStripMenuItem";
			this.screenshotToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.P)));
			this.screenshotToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.screenshotToolStripMenuItem.Text = "Screenshot";
			this.screenshotToolStripMenuItem.Click += new System.EventHandler(this.screenshotToolStripMenuItem_Click);
			// 
			// memoryUsageToolStripMenuItem
			// 
			this.memoryUsageToolStripMenuItem.Image = ((System.Drawing.Image)(resources.GetObject("memoryUsageToolStripMenuItem.Image")));
			this.memoryUsageToolStripMenuItem.Name = "memoryUsageToolStripMenuItem";
			this.memoryUsageToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.memoryUsageToolStripMenuItem.Text = "Memory Usage";
			this.memoryUsageToolStripMenuItem.Click += new System.EventHandler(this.memoryUsageToolStripMenuItem_Click);
			// 
			// systemStateToolStripMenuItem
			// 
			this.systemStateToolStripMenuItem.Name = "systemStateToolStripMenuItem";
			this.systemStateToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.systemStateToolStripMenuItem.Text = "System State";
			this.systemStateToolStripMenuItem.Click += new System.EventHandler(this.systemStateToolStripMenuItem_Click);
			// 
			// toolStripMenuItem6
			// 
			this.toolStripMenuItem6.Name = "toolStripMenuItem6";
			this.toolStripMenuItem6.Size = new System.Drawing.Size(174, 6);
			// 
			// timeshiftOffToolStripMenuItem
			// 
			this.timeshiftOffToolStripMenuItem.Name = "timeshiftOffToolStripMenuItem";
			this.timeshiftOffToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.timeshiftOffToolStripMenuItem.Text = "Timeshift off";
			this.timeshiftOffToolStripMenuItem.Click += new System.EventHandler(this.timeshiftOffToolStripMenuItem_Click);
			// 
			// timeshiftOnToolStripMenuItem
			// 
			this.timeshiftOnToolStripMenuItem.Name = "timeshiftOnToolStripMenuItem";
			this.timeshiftOnToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.timeshiftOnToolStripMenuItem.Text = "Timeshift on";
			this.timeshiftOnToolStripMenuItem.Click += new System.EventHandler(this.timeshiftOnToolStripMenuItem_Click);
			// 
			// toolStripMenuItem5
			// 
			this.toolStripMenuItem5.Name = "toolStripMenuItem5";
			this.toolStripMenuItem5.Size = new System.Drawing.Size(174, 6);
			// 
			// rebootToolStripMenuItem
			// 
			this.rebootToolStripMenuItem.Image = global::VirtualRemote.Properties.Resources.restart;
			this.rebootToolStripMenuItem.Name = "rebootToolStripMenuItem";
			this.rebootToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.rebootToolStripMenuItem.Text = "Reboot";
			this.rebootToolStripMenuItem.Click += new System.EventHandler(this.rebootToolStripMenuItem_Click);
			// 
			// shutdownToolStripMenuItem
			// 
			this.shutdownToolStripMenuItem.Image = global::VirtualRemote.Properties.Resources.shutdown;
			this.shutdownToolStripMenuItem.Name = "shutdownToolStripMenuItem";
			this.shutdownToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.shutdownToolStripMenuItem.Text = "Shutdown";
			this.shutdownToolStripMenuItem.Click += new System.EventHandler(this.shutdownToolStripMenuItem_Click);
			// 
			// toolStripMenuItem4
			// 
			this.toolStripMenuItem4.Name = "toolStripMenuItem4";
			this.toolStripMenuItem4.Size = new System.Drawing.Size(174, 6);
			// 
			// exitToolStripMenuItem
			// 
			this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
			this.exitToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
			this.exitToolStripMenuItem.Text = "E&xit";
			this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
			// 
			// viewToolStripMenuItem
			// 
			this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showVideoToolStripMenuItem,
            this.tuneVideoCardToolStripMenuItem,
            this.toolStripMenuItem1,
            this.showDebugOutputToolStripMenuItem,
            this.clearDebugOutputToolStripMenuItem,
            this.toolStripMenuItem2,
            this.alwaysOnTopToolStripMenuItem,
            this.toolStripMenuItem7,
            this.tF5800RemoteToolStripMenuItem,
            this.sendToppy2CodesToolStripMenuItem});
			this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
			this.viewToolStripMenuItem.Size = new System.Drawing.Size(56, 20);
			this.viewToolStripMenuItem.Text = "&Options";
			this.viewToolStripMenuItem.Click += new System.EventHandler(this.menuView_Popup);
			// 
			// tuneVideoCardToolStripMenuItem
			// 
			this.tuneVideoCardToolStripMenuItem.Name = "tuneVideoCardToolStripMenuItem";
			this.tuneVideoCardToolStripMenuItem.Size = new System.Drawing.Size(221, 22);
			this.tuneVideoCardToolStripMenuItem.Text = "&Tune Video Card";
			this.tuneVideoCardToolStripMenuItem.Click += new System.EventHandler(this.menuTune_Click);
			// 
			// toolStripMenuItem1
			// 
			this.toolStripMenuItem1.Name = "toolStripMenuItem1";
			this.toolStripMenuItem1.Size = new System.Drawing.Size(218, 6);
			// 
			// clearDebugOutputToolStripMenuItem
			// 
			this.clearDebugOutputToolStripMenuItem.Name = "clearDebugOutputToolStripMenuItem";
			this.clearDebugOutputToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.X)));
			this.clearDebugOutputToolStripMenuItem.Size = new System.Drawing.Size(221, 22);
			this.clearDebugOutputToolStripMenuItem.Text = "Clear Debug Output";
			this.clearDebugOutputToolStripMenuItem.Click += new System.EventHandler(this.menuClearDebug_Click);
			// 
			// toolStripMenuItem2
			// 
			this.toolStripMenuItem2.Name = "toolStripMenuItem2";
			this.toolStripMenuItem2.Size = new System.Drawing.Size(218, 6);
			// 
			// toolStripMenuItem7
			// 
			this.toolStripMenuItem7.Name = "toolStripMenuItem7";
			this.toolStripMenuItem7.Size = new System.Drawing.Size(218, 6);
			// 
			// sendToppy2CodesToolStripMenuItem
			// 
			this.sendToppy2CodesToolStripMenuItem.Checked = global::VirtualRemote.Properties.Settings.Default.Toppy2Remote;
			this.sendToppy2CodesToolStripMenuItem.CheckOnClick = true;
			this.sendToppy2CodesToolStripMenuItem.Name = "sendToppy2CodesToolStripMenuItem";
			this.sendToppy2CodesToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D2)));
			this.sendToppy2CodesToolStripMenuItem.Size = new System.Drawing.Size(221, 22);
			this.sendToppy2CodesToolStripMenuItem.Text = "Send Toppy2 Codes";
			// 
			// helpToolStripMenuItem
			// 
			this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutToolStripMenuItem});
			this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
			this.helpToolStripMenuItem.Size = new System.Drawing.Size(40, 20);
			this.helpToolStripMenuItem.Text = "&Help";
			// 
			// aboutToolStripMenuItem
			// 
			this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
			this.aboutToolStripMenuItem.Size = new System.Drawing.Size(126, 22);
			this.aboutToolStripMenuItem.Text = "&About...";
			this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
			// 
			// serialPort
			// 
			this.serialPort.BaudRate = 115200;
			this.serialPort.StopBits = System.IO.Ports.StopBits.Two;
			this.serialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort_DataReceived);
			// 
			// repeatTimer
			// 
			this.repeatTimer.Tick += new System.EventHandler(this.repeatTimer_Tick);
			// 
			// debugOutput
			// 
			this.debugOutput.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.debugOutput.BackColor = System.Drawing.SystemColors.Window;
			this.debugOutput.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.debugOutput.Location = new System.Drawing.Point(243, 264);
			this.debugOutput.Margin = new System.Windows.Forms.Padding(0);
			this.debugOutput.MaxLength = 0;
			this.debugOutput.Multiline = true;
			this.debugOutput.Name = "debugOutput";
			this.debugOutput.ReadOnly = true;
			this.debugOutput.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.debugOutput.Size = new System.Drawing.Size(326, 411);
			this.debugOutput.TabIndex = 4;
			// 
			// showVideoToolStripMenuItem
			// 
			this.showVideoToolStripMenuItem.Checked = global::VirtualRemote.Properties.Settings.Default.ShowVideo;
			this.showVideoToolStripMenuItem.CheckOnClick = true;
			this.showVideoToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.showVideoToolStripMenuItem.Name = "showVideoToolStripMenuItem";
			this.showVideoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.V)));
			this.showVideoToolStripMenuItem.Size = new System.Drawing.Size(221, 22);
			this.showVideoToolStripMenuItem.Text = "Show &Video";
			this.showVideoToolStripMenuItem.Click += new System.EventHandler(this.menuVideo_Click);
			// 
			// showDebugOutputToolStripMenuItem
			// 
			this.showDebugOutputToolStripMenuItem.Checked = global::VirtualRemote.Properties.Settings.Default.ShowDebugOutput;
			this.showDebugOutputToolStripMenuItem.CheckOnClick = true;
			this.showDebugOutputToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.showDebugOutputToolStripMenuItem.Name = "showDebugOutputToolStripMenuItem";
			this.showDebugOutputToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D)));
			this.showDebugOutputToolStripMenuItem.Size = new System.Drawing.Size(221, 22);
			this.showDebugOutputToolStripMenuItem.Text = "Show &Debug Output";
			// 
			// alwaysOnTopToolStripMenuItem
			// 
			this.alwaysOnTopToolStripMenuItem.Checked = global::VirtualRemote.Properties.Settings.Default.AlwaysOnTop;
			this.alwaysOnTopToolStripMenuItem.CheckOnClick = true;
			this.alwaysOnTopToolStripMenuItem.Name = "alwaysOnTopToolStripMenuItem";
			this.alwaysOnTopToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.T)));
			this.alwaysOnTopToolStripMenuItem.Size = new System.Drawing.Size(221, 22);
			this.alwaysOnTopToolStripMenuItem.Text = "Always &On Top";
			this.alwaysOnTopToolStripMenuItem.Click += new System.EventHandler(this.menuTopmost_Click);
			// 
			// tF5800RemoteToolStripMenuItem
			// 
			this.tF5800RemoteToolStripMenuItem.Checked = global::VirtualRemote.Properties.Settings.Default.TF5800Remote;
			this.tF5800RemoteToolStripMenuItem.CheckOnClick = true;
			this.tF5800RemoteToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.tF5800RemoteToolStripMenuItem.Name = "tF5800RemoteToolStripMenuItem";
			this.tF5800RemoteToolStripMenuItem.Size = new System.Drawing.Size(221, 22);
			this.tF5800RemoteToolStripMenuItem.Text = "TF5800 Remote";
			this.tF5800RemoteToolStripMenuItem.Click += new System.EventHandler(this.tF5800RemoteToolStripMenuItem_Click);
			// 
			// remotePanel
			// 
			this.remotePanel.BackgroundImage = global::VirtualRemote.Properties.Resources.TF5800Remote;
			this.remotePanel.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
			this.remotePanel.Dock = System.Windows.Forms.DockStyle.Left;
			this.remotePanel.HotspotImage = global::VirtualRemote.Properties.Resources.RemoteKeyMask;
			this.remotePanel.Location = new System.Drawing.Point(0, 24);
			this.remotePanel.Name = "remotePanel";
			this.remotePanel.Size = new System.Drawing.Size(240, 651);
			this.remotePanel.TabIndex = 5;
			this.remotePanel.RegionMouseUp += new VirtualRemote.HotspotPanel.RegionMouseUpDelegate(this.remotePanel_RegionMouseUp);
			this.remotePanel.RegionMouseDown += new VirtualRemote.HotspotPanel.RegionMouseDownDelegate(this.remotePanel_RegionMouseDown);
			// 
			// videoDisplay
			// 
			this.videoDisplay.BackColor = System.Drawing.Color.Black;
			this.videoDisplay.Enabled = false;
			this.videoDisplay.Location = new System.Drawing.Point(243, 24);
			this.videoDisplay.Margin = new System.Windows.Forms.Padding(0);
			this.videoDisplay.Name = "videoDisplay";
			this.videoDisplay.Size = new System.Drawing.Size(326, 240);
			this.videoDisplay.TabIndex = 3;
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Window;
			this.ClientSize = new System.Drawing.Size(569, 675);
			this.Controls.Add(this.remotePanel);
			this.Controls.Add(this.videoDisplay);
			this.Controls.Add(this.debugOutput);
			this.Controls.Add(this.menuStrip1);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.KeyPreview = true;
			this.MainMenuStrip = this.menuStrip1;
			this.Name = "MainForm";
			this.Text = "Virtual Remote";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyDown);
			this.Load += new System.EventHandler(this.MainForm_Load);
			this.menuStrip1.ResumeLayout(false);
			this.menuStrip1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.MenuStrip menuStrip1;
		private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
		private System.IO.Ports.SerialPort serialPort;
		private System.Windows.Forms.ToolStripMenuItem connectToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem connectToolStripMenuItem1;
		private System.Windows.Forms.ToolStripMenuItem disconnectToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem showVideoToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem tuneVideoCardToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
		private System.Windows.Forms.ToolStripMenuItem showDebugOutputToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem clearDebugOutputToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
		private System.Windows.Forms.ToolStripMenuItem alwaysOnTopToolStripMenuItem;
		private System.Windows.Forms.Timer repeatTimer;
		private VideoControl videoDisplay;
		private System.Windows.Forms.TextBox debugOutput;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
		private System.Windows.Forms.ToolStripMenuItem timeshiftOffToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem timeshiftOnToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem shutdownToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem4;
		private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem launchTAPToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem rebootToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem screenshotToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem memoryUsageToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem6;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem5;
		private HotspotPanel remotePanel;
		private System.Windows.Forms.ToolStripMenuItem systemStateToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem7;
		private System.Windows.Forms.ToolStripMenuItem tF5800RemoteToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem sendToppy2CodesToolStripMenuItem;
	}
}

