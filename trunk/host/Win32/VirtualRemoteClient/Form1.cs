#region license

/*
Copyright (C) 2005 Simon Capewell

This file is part of the TAPs for Topfield PVRs project.
	http://tap.berlios.de/

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#endregion

using System;
using System.Diagnostics;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Text;
using OpenNETCF.IO.Ports;

namespace VirtualRemote
{
	public class Form1 : System.Windows.Forms.Form
	{
		// Key names used by Toppy built in debugger functions
		string[] keyNames = 
		{
			"power",				"mute",
			"uhf",		"tvsat",	"sleep",
			"1",		"2",		"3",
			"4",		"5",		"6",
			"7",		"8",		"9",
			"recall",	"0",		"info",
			"menu", "guide", "tvradio", "audiotrk",
			"chup",	"voldown",	"ok", "volup", "chdown",
			"fav", "exit", "ttx", "subt",
			"rw",		"play",		"ff",
			"ab", "stop", "rec", "pause",
			"<<",		"f1",		">>",
			"newf1",	"f2", "f3", "f4",
			"sat", "check"
		};

		// PC keyboard shortcuts
		Keys[] shortcutKeys =
		{
			0, Keys.M,
			Keys.T, Keys.S, Keys.O,
			Keys.D1, Keys.D2, Keys.D3,
			Keys.D4, Keys.D5, Keys.D6,
			Keys.D7, Keys.D8, Keys.D9,
			Keys.R, Keys.D0, Keys.I,
			Keys.M, Keys.G, 0, Keys.Escape,
			Keys.Up, Keys.Left, Keys.Enter, Keys.Right, Keys.Down,
			0, 0, 0, 0,
			Keys.Decimal, 0, Keys.OemPeriod,
			0, 0, 0, 0,
			0, Keys.A, 0,
			Keys.F1, Keys.F2, Keys.F3, Keys.F4,
			Keys.F5, Keys.F6
		};

		private SerialPort m_comPort = new SerialPort("com1");

		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.TextBox debugOutput;
		private DotNetOpenSource.Controls.ImageMap remotePicture;
		private VirtualRemote.VideoControl videoDisplay;
		private System.Windows.Forms.MenuItem menuView;
		private System.Windows.Forms.MenuItem menuExit;
		private System.Windows.Forms.MenuItem menuItem5;
		private System.Windows.Forms.MenuItem menuDebugOutput;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuClearDebug;
		private System.Windows.Forms.MenuItem menuVideo;
		private System.Windows.Forms.MenuItem menuConnect;
		private System.Windows.Forms.MenuItem menuDisconnect;
		private System.Windows.Forms.MenuItem menuFile;
		private System.Windows.Forms.MenuItem menuTune;

		public Form1()
		{
			// Hook up the event handler for the serial port class
			m_comPort.ReceivedEvent += new SerialReceivedEventHandler(DataReceived);
			// Toppy uses 115200, 8 bit, no parity and 2 stop bits
			m_comPort.BaudRate = 115200;
			m_comPort.DataBits = 8;
			m_comPort.Parity = Parity.None;
			m_comPort.StopBits = StopBits.Two;
			try
			{
				m_comPort.Open();
			}
			catch
			{
			}

			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if ( m_comPort != null )
				m_comPort.Dispose();

			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(Form1));
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.menuView = new System.Windows.Forms.MenuItem();
			this.menuTune = new System.Windows.Forms.MenuItem();
			this.menuExit = new System.Windows.Forms.MenuItem();
			this.debugOutput = new System.Windows.Forms.TextBox();
			this.remotePicture = new DotNetOpenSource.Controls.ImageMap();
			this.videoDisplay = new VirtualRemote.VideoControl();
			this.menuFile = new System.Windows.Forms.MenuItem();
			this.menuConnect = new System.Windows.Forms.MenuItem();
			this.menuDisconnect = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.menuDebugOutput = new System.Windows.Forms.MenuItem();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.menuClearDebug = new System.Windows.Forms.MenuItem();
			this.menuVideo = new System.Windows.Forms.MenuItem();
			this.SuspendLayout();
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuFile,
																					  this.menuView});
			// 
			// menuView
			// 
			this.menuView.Index = 1;
			this.menuView.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.menuVideo,
																					 this.menuTune,
																					 this.menuItem1,
																					 this.menuDebugOutput,
																					 this.menuClearDebug});
			this.menuView.Text = "&View";
			this.menuView.Popup += new System.EventHandler(this.menuView_Popup);
			// 
			// menuTune
			// 
			this.menuTune.Index = 1;
			this.menuTune.Text = "&Tune Video Card";
			this.menuTune.Click += new System.EventHandler(this.menuTune_Click);
			// 
			// menuExit
			// 
			this.menuExit.Index = 3;
			this.menuExit.Text = "E&xit";
			// 
			// debugOutput
			// 
			this.debugOutput.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.debugOutput.Location = new System.Drawing.Point(296, 240);
			this.debugOutput.MaxLength = 0;
			this.debugOutput.Multiline = true;
			this.debugOutput.Name = "debugOutput";
			this.debugOutput.ReadOnly = true;
			this.debugOutput.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.debugOutput.Size = new System.Drawing.Size(320, 456);
			this.debugOutput.TabIndex = 1;
			this.debugOutput.Text = "";
			this.debugOutput.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form1_KeyDown);
			// 
			// remotePicture
			// 
			this.remotePicture.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left)));
			this.remotePicture.Image = ((System.Drawing.Image)(resources.GetObject("remotePicture.Image")));
			this.remotePicture.Location = new System.Drawing.Point(0, 0);
			this.remotePicture.Name = "remotePicture";
			this.remotePicture.Size = new System.Drawing.Size(296, 696);
			this.remotePicture.TabIndex = 2;
			this.remotePicture.RegionClick += new DotNetOpenSource.Controls.ImageMap.RegionClickDelegate(this.imageMap1_RegionClick);
			this.remotePicture.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form1_KeyDown);
			// 
			// videoDisplay
			// 
			this.videoDisplay.Location = new System.Drawing.Point(296, 0);
			this.videoDisplay.Name = "videoDisplay";
			this.videoDisplay.Size = new System.Drawing.Size(320, 240);
			this.videoDisplay.TabIndex = 4;
			// 
			// menuFile
			// 
			this.menuFile.Index = 0;
			this.menuFile.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					 this.menuConnect,
																					 this.menuDisconnect,
																					 this.menuItem5,
																					 this.menuExit});
			this.menuFile.Text = "&Connect";
			this.menuFile.Popup += new System.EventHandler(this.menuFile_Popup);
			// 
			// menuConnect
			// 
			this.menuConnect.Index = 0;
			this.menuConnect.Text = "&Connect";
			this.menuConnect.Click += new System.EventHandler(this.menuConnect_Click);
			// 
			// menuDisconnect
			// 
			this.menuDisconnect.Index = 1;
			this.menuDisconnect.Text = "&Disconnect";
			this.menuDisconnect.Click += new System.EventHandler(this.menuDisconnect_Click);
			// 
			// menuItem5
			// 
			this.menuItem5.Index = 2;
			this.menuItem5.Text = "-";
			// 
			// menuDebugOutput
			// 
			this.menuDebugOutput.Checked = true;
			this.menuDebugOutput.Index = 3;
			this.menuDebugOutput.Text = "Show &Debug Output";
			this.menuDebugOutput.Click += new System.EventHandler(this.menuDebugOutput_Click);
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 2;
			this.menuItem1.Text = "-";
			// 
			// menuClearDebug
			// 
			this.menuClearDebug.Index = 4;
			this.menuClearDebug.Text = "Clear Debug Output";
			this.menuClearDebug.Click += new System.EventHandler(this.menuClearDebug_Click);
			// 
			// menuVideo
			// 
			this.menuVideo.Index = 0;
			this.menuVideo.Text = "Show &Video";
			this.menuVideo.Click += new System.EventHandler(this.menuVideo_Click);
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(616, 695);
			this.Controls.Add(this.videoDisplay);
			this.Controls.Add(this.remotePicture);
			this.Controls.Add(this.debugOutput);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Menu = this.mainMenu1;
			this.Name = "Form1";
			this.Text = "Topfield Virtual Remote";
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form1_KeyDown);
			this.ResumeLayout(false);

		}
		#endregion

		[STAThread]
		static void Main() 
		{
			using(SpecialServices.SingleProgramInstance spi = new SpecialServices.SingleProgramInstance("virtualremote"))
			{
				if (spi.IsSingleInstance)
				{
					Application.Run(new Form1());
				}
				else
				{
					spi.RaiseOtherProcess();
				}
			}
		}

		#region Event Handlers

		private void menuTune_Click(object sender, System.EventArgs e)
		{
			if ( menuVideo.Checked )
				videoDisplay.ShowTunerSettings();
		}

		private void imageMap1_RegionClick(int index, string key)
		{
			if ( CheckPortOpen() )
			{
				// Button clicked, look up the key code to send
				SendKey( keyNames[index] );
			}
		}

		private void Form1_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if ( CheckPortOpen() )
			{
				// Key pressed, look for shortcuts
				for ( int i = 0; i < shortcutKeys.Length; ++i )
				{
					if ( e.KeyCode == shortcutKeys[i] )
					{
						SendKey( keyNames[i] );
						break;
					}
				}
			}
		}

		private void DataReceived(object sender, SerialReceivedEventArgs e)
		{
			// Serial port data received, output to text control
			string s = m_comPort.ReadExisting();
			if ( menuDebugOutput.Checked )
				WriteMessage( s, false );
		}

		#endregion

		private bool CheckPortOpen()
		{
			if ( !m_comPort.IsOpen )
			{
				if ( MessageBox.Show( "You are not connected. Connect now?", "Virtual Remote", MessageBoxButtons.YesNo ) == DialogResult.No )
					return false;
				try
				{
					m_comPort.Open();
				}
				catch
				{
					MessageBox.Show("Could not connect to COM1", "Virtual Remote");
					return false;
				}
			}
			return true;
		}


		private void SendKey( string keyName )
		{
			m_comPort.WriteLine( "echo off\r" );
			m_comPort.WriteLine( "key " + keyName + "\r" );
		}

		private void WriteMessage(string message, bool linefeed )
		{
			debugOutput.Text += message;
			debugOutput.SelectionStart = debugOutput.TextLength - 1;
			debugOutput.ScrollToCaret();

			if (linefeed)
				debugOutput.Text += Environment.NewLine;
		}

		private void menuFile_Popup(object sender, System.EventArgs e)
		{
			menuConnect.Enabled = !m_comPort.IsOpen;
			menuDisconnect.Enabled = m_comPort.IsOpen;
		}

		private void menuConnect_Click(object sender, System.EventArgs e)
		{
			try
			{
				m_comPort.Open();
			}
			catch
			{
				MessageBox.Show("Could not connect to COM1", "Virtual Remote");
			}
		}

		private void menuDisconnect_Click(object sender, System.EventArgs e)
		{
			m_comPort.Close();
		}

		private void menuView_Popup(object sender, System.EventArgs e)
		{
			menuVideo.Checked = videoDisplay.CurrentState == VideoControl.PlayState.Running;
			menuTune.Enabled = menuVideo.Checked;
		}

		private void menuVideo_Click(object sender, System.EventArgs e)
		{
			menuVideo.Checked = !menuVideo.Checked;
			if (menuVideo.Checked)
			{
				videoDisplay.StartVideo();
			}
			else
				videoDisplay.StopVideo();
		}

		private void menuDebugOutput_Click(object sender, System.EventArgs e)
		{
			menuDebugOutput.Checked = !menuDebugOutput.Checked;
		}

		private void menuClearDebug_Click(object sender, System.EventArgs e)
		{
			debugOutput.Text = "";
		}

		private void WriteMessage( string message )
		{
			WriteMessage( message, true );
		}
	}
}
