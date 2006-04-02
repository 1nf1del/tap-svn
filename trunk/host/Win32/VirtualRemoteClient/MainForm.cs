#region License

/*
Copyright (C) 2006 Simon Capewell

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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;

namespace VirtualRemote
{
	public partial class MainForm : Form
	{
		enum TopfieldKeys
		{
			 N0			= 0x10000,
			 N1			= 0x10001,
			 N2			= 0x10002,
			 N3			= 0x10003,
			 N4			= 0x10004,
			 N5			= 0x10005,
			 N6			= 0x10006,
			 N7			= 0x10007,
			 N8			= 0x10008,
			 N9			= 0x10009,
			 Power		= 0x1000a,
			 Recall 	= 0x1000b,
			 Mute		= 0x1000c,
			 NoUse		= 0x1000d,
			 Uhf		= 0x1000e,
			 VolDown	= 0x1000f,
			 Sleep		= 0x10010,
			 Easy		= 0x10011,
			 ChUp		= 0x10012,
			 AudioTrk	= 0x10013,
			 Info		= 0x10014,
			 VolUp		= 0x10015,
			 Guide		= 0x10016,
			 Exit		= 0x10017,
			 TvRadio	= 0x10018,
			 NoUse4 	= 0x10019,
			 Pause		= 0x1001a,
			 NoUse5 	= 0x1001b,
			 Menu		= 0x1001c,
			 ChDown 	= 0x1001d,
			 Ok 		= 0x1001e,

			 Fav		= 0x10020,
			 Subt		= 0x10021,
			 TvSat		= 0x10022,

			 F1 		= 0x10023,
			 F2 		= 0x10024,
			 F3 		= 0x10025,
			 F4 		= 0x10026,

			 Cmd_0		= 0x10027,
			 Cmd_1		= 0x10028,
			 Cmd_2		= 0x10029,
			 Cmd_3		= 0x1002a,
			 Cmd_4		= 0x1002b,
			 Cmd_5		= 0x1002c,
			 Cmd_6		= 0x1002d,
			 Cmd_7		= 0x1002e,
			 Cmd_8		= 0x1002f,
			 Cmd_9		= 0x10030,
			 Cmd_a		= 0x10031,
			 Cmd_b		= 0x10032,
			 Cmd_c		= 0x10033,
			 Cmd_d		= 0x10034,
			 Cmd_e		= 0x10035,
			 Cmd_f		= 0x10036,

			 Teletext	= 0x10037,
			 Rewind 	= 0x10038,
			 Play		= 0x10039,
			 Forward	= 0x1003a,
			 Stop		= 0x1003c,
			 Record 	= 0x1003d,

			 NewF1		= 0x1003f,
			 Sat		= 0x10040,
			 Prev		= 0x10041,
			 PlayList	= 0x10042,
			 Next		= 0x10043,

			 Bookmark	= F2,
			 Goto		= F3,
			 Check		= F4,

			 Ab 		= 0x1003e,
			 Slow		= 0x1003b
		}
		enum TopfieldRawKeys
		{
			FrontChUp		= 0x1,
			FrontChDn		= 0x2,
			FrontVolUp		= 0x3,
			FrontVolDn		= 0x4,
			FrontOk			= 0x5,
			FrontMenu		= 0x6,
			Up				= 0x100,
			Down			= 0x101,
			Right			= 0x102,
			Left			= 0x103,
			ListTVRad		= 0x104,
			ExitAudioTrk	= 0x105,
			Pause			= 0x106,
			ChUpSubt		= 0x107,
			TvSat			= 0x108,
			VolUpFav		= 0x109,
			Mute	 		= 0x10c,
			Green			= 0x10d,
			Yellow			= 0x10e,
			Blue	 		= 0x10f,
			N0	 			= 0x110,
			N1	 			= 0x111,
			N2	 			= 0x112,
			N3	 			= 0x113,
			N4	 			= 0x114,
			N5	 			= 0x115,
			N6	 			= 0x116,
			N7	 			= 0x117,
			N8	 			= 0x118,
			N9	 			= 0x119,
			Menu	 		= 0x11a,
			Guide	 		= 0x11b,
			VolDownExit		= 0x11c,
			Info	 		= 0x11d,
			Recall			= 0x11e,
			Ok	 			= 0x11f,
			TVRadioUhf		= 0x143,
			OptSleep		= 0x144,
			Rewind			= 0x145,
			Play	 		= 0x146,
			ChDownTeletext	= 0x147,
			Forward	 		= 0x148,
			Slow	 		= 0x149,
			Stop	 		= 0x14a,
			Record	 		= 0x14b,
			WhiteAb	 		= 0x14c,
			Red	 			= 0x14d,
			PIPPrev	 		= 0x150,
			PlayList	 	= 0x151,
			TeletextNext	= 0x152,
			PIPSwapSat	 	= 0x15e,
			Power	 		= 0x1f0,

			Up2				= 0x120,
			Down2			= 0x121,
			Right2			= 0x122,
			Left2			= 0x123,
			ListTVRad2		= 0x124,
			ExitAudioTrk2	= 0x125,
			Pause2			= 0x126,
			ChUpSubt2		= 0x127,
			TvSat2			= 0x128,
			VolUpFav2		= 0x129,
			Mute2	 		= 0x12c,
			Green2			= 0x12d,
			Yellow2			= 0x12e,
			Blue2	 		= 0x12f,
			N02	 			= 0x130,
			N12	 			= 0x131,
			N22	 			= 0x132,
			N32	 			= 0x133,
			N42	 			= 0x134,
			N52	 			= 0x135,
			N62	 			= 0x136,
			N72	 			= 0x137,
			N82	 			= 0x138,
			N92	 			= 0x139,
			Menu2	 		= 0x13a,
			Guide2	 		= 0x13b,
			VolDownExit2	= 0x13c,
			Info2	 		= 0x13d,
			Recall2			= 0x13e,
			Ok2	 			= 0x13f,
			TVRadioUhf2		= 0x153,
			OptSleep2		= 0x154,
			Rewind2			= 0x155,
			Play2	 		= 0x156,
			ChDownTeletext2	= 0x157,
			Forward2	 	= 0x158,
			Slow2	 		= 0x159,
			Stop2	 		= 0x15a,
			Record2	 		= 0x15b,
			WhiteAb2 		= 0x15c,
			Red2 			= 0x15d,
			PIPPrev2 		= 0x160,
			PlayList2	 	= 0x161,
			TeletextNext2	= 0x162,
			PIPSwapSat2	 	= 0x16e,
		}
		TopfieldKeys[] keyValues = 
		{
			TopfieldKeys.Power,								TopfieldKeys.Mute,
			TopfieldKeys.Uhf,		TopfieldKeys.TvSat,		TopfieldKeys.Sleep,
			TopfieldKeys.N1,		TopfieldKeys.N2,		TopfieldKeys.N3,
			TopfieldKeys.N4,		TopfieldKeys.N5,		TopfieldKeys.N6,
			TopfieldKeys.N7,		TopfieldKeys.N8,		TopfieldKeys.N9,
			TopfieldKeys.Recall,	TopfieldKeys.N0,		TopfieldKeys.Info,
			TopfieldKeys.Menu,		TopfieldKeys.Guide,		TopfieldKeys.TvRadio, TopfieldKeys.AudioTrk,
			TopfieldKeys.ChUp,		TopfieldKeys.VolDown,	TopfieldKeys.Ok, TopfieldKeys.VolUp, TopfieldKeys.ChDown,
			TopfieldKeys.Fav,		TopfieldKeys.Exit,		TopfieldKeys.Teletext, TopfieldKeys.Subt,
			TopfieldKeys.Rewind,	TopfieldKeys.Play,		TopfieldKeys.Forward,
			TopfieldKeys.Slow,		TopfieldKeys.Stop,		TopfieldKeys.Record, TopfieldKeys.Pause,
			TopfieldKeys.Prev,		TopfieldKeys.PlayList,	TopfieldKeys.Next,
			TopfieldKeys.NewF1,		TopfieldKeys.F2, TopfieldKeys.F3, TopfieldKeys.F4,
			TopfieldKeys.Sat,		TopfieldKeys.Ab
		};
		TopfieldRawKeys[] rawkeyValues = 
		{
			TopfieldRawKeys.Power,								TopfieldRawKeys.Mute,
			TopfieldRawKeys.TVRadioUhf,		TopfieldRawKeys.TvSat,		TopfieldRawKeys.OptSleep,
			TopfieldRawKeys.N1,		TopfieldRawKeys.N2,		TopfieldRawKeys.N3,
			TopfieldRawKeys.N4,		TopfieldRawKeys.N5,		TopfieldRawKeys.N6,
			TopfieldRawKeys.N7,		TopfieldRawKeys.N8,		TopfieldRawKeys.N9,
			TopfieldRawKeys.Recall,	TopfieldRawKeys.N0,		TopfieldRawKeys.Info,
			TopfieldRawKeys.Menu,		TopfieldRawKeys.Guide,		TopfieldRawKeys.ListTVRad, TopfieldRawKeys.ExitAudioTrk,
			TopfieldRawKeys.Up,		TopfieldRawKeys.Left,	TopfieldRawKeys.Ok, TopfieldRawKeys.Right, TopfieldRawKeys.Down,
			TopfieldRawKeys.VolUpFav,	TopfieldRawKeys.VolDownExit,		TopfieldRawKeys.ChDownTeletext, TopfieldRawKeys.ChUpSubt,
			TopfieldRawKeys.Rewind,	TopfieldRawKeys.Play,		TopfieldRawKeys.Forward,
			TopfieldRawKeys.Slow,		TopfieldRawKeys.Stop,		TopfieldRawKeys.Record, TopfieldRawKeys.Pause,
			TopfieldRawKeys.PIPPrev,		TopfieldRawKeys.PlayList,	TopfieldRawKeys.TeletextNext,
			TopfieldRawKeys.Red,		TopfieldRawKeys.Green, TopfieldRawKeys.Yellow, TopfieldRawKeys.Blue,
			TopfieldRawKeys.PIPSwapSat,	TopfieldRawKeys.WhiteAb
		};
		TopfieldRawKeys[] rawkeyValues2 = 
		{
			TopfieldRawKeys.Power,								TopfieldRawKeys.Mute2,
			TopfieldRawKeys.TVRadioUhf2,	TopfieldRawKeys.TvSat2,		TopfieldRawKeys.OptSleep2,
			TopfieldRawKeys.N12,		TopfieldRawKeys.N22,		TopfieldRawKeys.N32,
			TopfieldRawKeys.N42,		TopfieldRawKeys.N52,		TopfieldRawKeys.N62,
			TopfieldRawKeys.N72,		TopfieldRawKeys.N82,		TopfieldRawKeys.N92,
			TopfieldRawKeys.Recall2,	TopfieldRawKeys.N02,		TopfieldRawKeys.Info2,
			TopfieldRawKeys.Menu2,		TopfieldRawKeys.Guide2,		TopfieldRawKeys.ListTVRad2, TopfieldRawKeys.ExitAudioTrk2,
			TopfieldRawKeys.Up2,		TopfieldRawKeys.Left2,	TopfieldRawKeys.Ok2, TopfieldRawKeys.Right2, TopfieldRawKeys.Down2,
			TopfieldRawKeys.VolUpFav2,	TopfieldRawKeys.VolDownExit2,		TopfieldRawKeys.ChDownTeletext2, TopfieldRawKeys.ChUpSubt2,
			TopfieldRawKeys.Rewind2,	TopfieldRawKeys.Play2,		TopfieldRawKeys.Forward2,
			TopfieldRawKeys.Slow2,		TopfieldRawKeys.Stop2,		TopfieldRawKeys.Record2, TopfieldRawKeys.Pause2,
			TopfieldRawKeys.PIPPrev2,		TopfieldRawKeys.PlayList2,	TopfieldRawKeys.TeletextNext2,
			TopfieldRawKeys.Red2,		TopfieldRawKeys.Green2, TopfieldRawKeys.Yellow2, TopfieldRawKeys.Blue2,
			TopfieldRawKeys.PIPSwapSat2,	TopfieldRawKeys.WhiteAb2
		};

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
			"<<",		"playlist",	">>",
			"newf1",	"f2", "f3", "f4",
			"sat", "check"
		};

		// PC keyboard shortcuts
		Keys[] shortcutKeys =
		{
			Keys.F12, Keys.U,
			Keys.T, Keys.S, Keys.O,
			Keys.D1, Keys.D2, Keys.D3,
			Keys.D4, Keys.D5, Keys.D6,
			Keys.D7, Keys.D8, Keys.D9,
			Keys.R, Keys.D0, Keys.I,
			Keys.M, Keys.G, Keys.L, Keys.Escape,
			Keys.Up, Keys.Left, Keys.Enter, Keys.Right, Keys.Down,
			0, 0, 0, 0,
			Keys.End, 0, Keys.PageDown,
			0, 0, 0, 0,
			0, Keys.A, 0,
			Keys.F1, Keys.F2, Keys.F3, Keys.F4,
			Keys.F5, Keys.F6
		};

		private int m_currentKeyIndex;

		public MainForm()
		{
			InitializeComponent();

			string[] args = Environment.GetCommandLineArgs();
			if ( Array.FindIndex<string>(args, delegate(string value) { return value == "/connect"; } ) >= 0 )
				serialPort.Open();
		}

		private void menuTune_Click(object sender, System.EventArgs e)
		{
			if ( showVideoToolStripMenuItem.Checked )
				videoDisplay.ShowTunerSettings();
		}

		private void MainForm_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			// Key pressed, look for shortcuts
			for ( int i = 0; i < shortcutKeys.Length; ++i )
			{
				if ( e.KeyCode == shortcutKeys[i] )
				{
					if ( CheckPortOpen() )
					{
						SendKey(i);
						e.Handled = true;
						break;
					}
				}
			}
		}

		private delegate void WriteMessageDelegate(string s, bool linefeed);
 
		private void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
		{
			// Serial port data received, output to text control
			string s = serialPort.ReadExisting();
			if (showDebugOutputToolStripMenuItem.Checked)
			{
				object[] args = { s, false };
				BeginInvoke(new WriteMessageDelegate(WriteMessage), args);
			}
		}

		private bool CheckPortOpen()
		{
			if (!serialPort.IsOpen)
			{
				if ( MessageBox.Show( "You are not connected. Connect now?", "Virtual Remote", MessageBoxButtons.YesNo, MessageBoxIcon.Question ) == DialogResult.No )
					return false;
				try
				{
					serialPort.Open();
				}
				catch
				{
					MessageBox.Show("Could not connect to " + serialPort.PortName, "Virtual Remote", MessageBoxButtons.OK, MessageBoxIcon.Information);
					return false;
				}
			}
			return true;
		}

		enum Command
		{
			Key = 1,
			Screenshot,
			Shutdown,
			Reboot,
			LaunchTap,
			MemoryReport,
			StateReport,
			SetSystemVariable
		}

		private void SendCommand(Command command, uint param1, uint param2)
		{
			if (CheckPortOpen())
			{
				byte[] buffer = new byte[14];
				buffer[0] = 236; // ID byte
				// Length
				buffer[1] = (byte)buffer.Length;
				// CRC byte
				buffer[2] = 0;
				// Command
				buffer[3] = (byte)(((uint)command >> 8) & 0xff);
				buffer[4] = (byte)((uint)command & 0xff);
				// Param
				buffer[5] = (byte)(((uint)param1 >> 24) & 0xff);
				buffer[6] = (byte)(((uint)param1 >> 16) & 0xff);
				buffer[7] = (byte)(((uint)param1 >> 8) & 0xff);
				buffer[8] = (byte)((uint)param1 & 0xff);
				buffer[9] = (byte)(((uint)param2 >> 24) & 0xff);
				buffer[10] = (byte)(((uint)param2 >> 16) & 0xff);
				buffer[11] = (byte)(((uint)param2 >> 8) & 0xff);
				buffer[12] = (byte)((uint)param2 & 0xff);
				buffer[13] = 236;
				for (int i = 3; i < buffer.Length; ++i)
				{
					buffer[2] += buffer[i];
				}

				serialPort.Write(buffer, 0, buffer.Length);
			}
		}

		private void SendCommand(Command command)
		{
			SendCommand(command, 0, 0);
		}

		private void SendKey(int index)
		{
			SendCommand(Command.Key, (uint)keyValues[index],
				(uint)(sendToppy2CodesToolStripMenuItem.Checked ? rawkeyValues2[index] : rawkeyValues[index]) );
	}

		private void WriteMessage(string message, bool linefeed )
		{
			debugOutput.Text += message;
			debugOutput.SelectionStart = debugOutput.TextLength;
			debugOutput.ScrollToCaret();

			if (linefeed)
				debugOutput.Text += Environment.NewLine;
		}

		private void remotePanel_RegionMouseDown(int index)
		{
			bool open = serialPort.IsOpen;
			if ( CheckPortOpen() )
			{
				// Button clicked, look up the key code to send
				m_currentKeyIndex = index-128;
				SendKey( m_currentKeyIndex );
				if (open)
				{
					repeatTimer.Interval = 500;
					repeatTimer.Enabled = true;
				}
			}
		}

		private void remotePanel_RegionMouseUp(int index)
		{
			repeatTimer.Enabled = false;
		}


		private void repeatTimer_Tick(object sender, EventArgs e)
		{
			SendKey( m_currentKeyIndex );
			repeatTimer.Interval = 250;
		}

		private void menuTopmost_Click(object sender, System.EventArgs e)
		{
			TopMost = alwaysOnTopToolStripMenuItem.Checked;
		}

		private void menuFile_Popup(object sender, System.EventArgs e)
		{
			connectToolStripMenuItem1.Enabled = !serialPort.IsOpen;
			disconnectToolStripMenuItem.Enabled = serialPort.IsOpen;
		}

		private void menuConnect_Click(object sender, System.EventArgs e)
		{
			try
			{
				serialPort.Open();
			}
			catch
			{
				MessageBox.Show("Could not connect to " + serialPort.PortName, "Virtual Remote", MessageBoxButtons.OK, MessageBoxIcon.Information);
			}
		}

		private void menuDisconnect_Click(object sender, System.EventArgs e)
		{
			serialPort.Close();
		}

		private void menuView_Popup(object sender, System.EventArgs e)
		{
			showVideoToolStripMenuItem.Checked = videoDisplay.CurrentState == VideoControl.PlayState.Running;
			tuneVideoCardToolStripMenuItem.Enabled = showVideoToolStripMenuItem.Checked;
		}

		private void menuVideo_Click(object sender, System.EventArgs e)
		{
			if (showVideoToolStripMenuItem.Checked)
			{
				videoDisplay.StartVideo();
				videoDisplay.Show();
				debugOutput.SetBounds(debugOutput.Left, videoDisplay.Bottom, debugOutput.Width, debugOutput.Height - videoDisplay.Height);
			}
			else
			{
				debugOutput.SetBounds(debugOutput.Left, videoDisplay.Top, debugOutput.Width, debugOutput.Bottom - videoDisplay.Top);
				videoDisplay.Hide();
				videoDisplay.StopVideo();
			}
		}

		private void menuClearDebug_Click(object sender, System.EventArgs e)
		{
			debugOutput.Text = "";
		}

		private void WriteMessage( string message )
		{
			WriteMessage( message, true );
		}

		private void MainForm_Load(object sender, EventArgs e)
		{
			SetBounds(Properties.Settings.Default.WindowPos.X, Properties.Settings.Default.WindowPos.Y,
				Properties.Settings.Default.WindowPos.Width, Properties.Settings.Default.WindowPos.Height);
			if (Properties.Settings.Default.WindowState == FormWindowState.Maximized)
				WindowState = Properties.Settings.Default.WindowState;
		}

		private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
		{
			// Save window position and state
			if (WindowState == FormWindowState.Normal)
				Properties.Settings.Default.WindowPos = new Rectangle(Left, Top, Width, Height);
			else
				Properties.Settings.Default.WindowPos = RestoreBounds;
			Properties.Settings.Default.WindowState = WindowState;

			// Save settings
			Properties.Settings.Default.Save();
		}

		private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
		{
			AboutBox dlg = new AboutBox();
			dlg.ShowDialog();
		}

		private void timeshiftOffToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SendCommand(Command.SetSystemVariable, 15, 0);
		}

		private void timeshiftOnToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SendCommand(Command.SetSystemVariable, 15, 1);
		}

		private void rebootToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SendCommand(Command.Reboot);
		}

		private void shutdownToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SendCommand(Command.Shutdown);
		}

		private void launchTAPToolStripMenuItem_Click(object sender, EventArgs e)
		{
//			TAPForm f = new TAPForm();
//			f.ShowDialog();
//			SendCommand("launch tap.tap");
		}

		private void screenshotToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SendCommand(Command.Screenshot, 6, 0);
		}

		private void memoryUsageToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SendCommand(Command.MemoryReport);
		}

		private void exitToolStripMenuItem_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void systemStateToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SendCommand(Command.StateReport);
		}

		private void tF5800RemoteToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (tF5800RemoteToolStripMenuItem.Checked)
				remotePanel.BackgroundImage = VirtualRemote.Properties.Resources.TF5800Remote;
			else
				remotePanel.BackgroundImage = VirtualRemote.Properties.Resources.TF5000Remote;
		}
	}
}
