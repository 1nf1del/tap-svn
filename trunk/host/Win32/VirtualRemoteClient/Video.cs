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
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Xml;
using System.Reflection;
using System.Runtime.InteropServices;
using DirectShowLib;

namespace VirtualRemote
{
	/// <summary>
	/// Summary description for VideoControl.
	/// </summary>
//	[ToolboxBitmap(typeof(VideoControl))]
	public class VideoControl : System.Windows.Forms.UserControl
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		// a small enum to record the graph state
		public enum PlayState 
		{
			Stopped, 
			Paused, 
			Running, 
			Init
		};

		DsROTEntry rot = null;
		// Application-defined message to notify app of filtergraph events
		public const int WM_GRAPHNOTIFY = 0x8000 + 1;

		IVideoWindow  videoWindow = null;
		IMediaControl mediaControl = null;
		IMediaEventEx mediaEventEx = null;
		IGraphBuilder graphBuilder = null;
		IBaseFilter sourceFilter = null;
		ICaptureGraphBuilder2 captureGraphBuilder = null;
		PlayState currentState = PlayState.Stopped;

		public VideoControl()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			this.components = new Container();

			if ( System.Diagnostics.Process.GetCurrentProcess().ProcessName == "devenv" )
				return;

			if ( Enabled )
				CaptureVideo();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if( components != null )
					components.Dispose();
				CloseInterfaces();  
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			// 
			// VideoControl
			// 
			this.Name = "VideoControl";
			this.Resize += new System.EventHandler(this.VideoControl_Resize);

		}
		#endregion

		public void CaptureVideo()
		{
			int hr = 0;

			try
			{
				// Get DirectShow interfaces
				GetInterfaces();

				// Attach the filter graph to the capture graph
				hr = captureGraphBuilder.SetFiltergraph(graphBuilder);
				DsError.ThrowExceptionForHR(hr);

				// Use the system device enumerator and class enumerator to find
				// a video capture/preview device, such as a desktop USB video camera.
				sourceFilter = FindCaptureDevice();

				// Add Capture filter to our graph.
				hr = graphBuilder.AddFilter(sourceFilter, "Video Capture");
				DsError.ThrowExceptionForHR(hr);

				// Render the preview pin on the video capture filter
				// Use this instead of graphBuilder.RenderFile
				hr = captureGraphBuilder.RenderStream(PinCategory.Preview, MediaType.Video, sourceFilter, null, null);
				DsError.ThrowExceptionForHR(hr);

				// Set video window style and position
				SetupVideoWindow();

				// Add our graph to the running object table, which will allow
				// the GraphEdit application to "spy" on our graph
				rot = new DsROTEntry(graphBuilder);

				// Start previewing video data
				hr = mediaControl.Run();
				DsError.ThrowExceptionForHR(hr);

				// Remember current state
				currentState = PlayState.Running;
			}
			catch
			{
				Enabled = false;
			}
		}

		// This version of FindCaptureDevice is provide for education only.
		// A second version using the DsDevice helper class is define later.
		public IBaseFilter FindCaptureDevice()
		{
			int hr = 0;
			UCOMIEnumMoniker classEnum = null;
			UCOMIMoniker[] moniker = new UCOMIMoniker[1];
			int fetched = 0;
			object source = null;

			// Create the system device enumerator
			ICreateDevEnum devEnum = (ICreateDevEnum) new CreateDevEnum();

			// Create an enumerator for the video capture devices
			hr = devEnum.CreateClassEnumerator(FilterCategory.VideoInputDevice, out classEnum, 0);
			DsError.ThrowExceptionForHR(hr);

			// The device enumerator is no more needed
			Marshal.ReleaseComObject(devEnum);

			// If there are no enumerators for the requested type, then 
			// CreateClassEnumerator will succeed, but classEnum will be NULL.
			if (classEnum == null)
			{
				throw new ApplicationException("No video capture device was detected.\r\n\r\n" +
					"This sample requires a video capture device, such as a USB WebCam,\r\n" +
					"to be installed and working properly.  The sample will now close.");
			}

			// Use the first video capture device on the device list.
			// Note that if the Next() call succeeds but there are no monikers,
			// it will return 1 (S_FALSE) (which is not a failure).  Therefore, we
			// check that the return code is 0 (S_OK).
			if (classEnum.Next (moniker.Length, moniker, out fetched) == 0)
			{
				// Bind Moniker to a filter object
				Guid iid = typeof(IBaseFilter).GUID;
				moniker[0].BindToObject(null, null, ref iid, out source);
			}
			else
			{
				throw new ApplicationException("Unable to access video capture device!");   
			}

			// Release COM objects
			Marshal.ReleaseComObject(moniker[0]);
			Marshal.ReleaseComObject(classEnum);

			// An exception is thrown if cast fail
			return (IBaseFilter) source;
		}
		/*
			// Uncomment this version of FindCaptureDevice to use the DsDevice helper class
			// (and comment the first version of course)
			public IBaseFilter FindCaptureDevice()
			{
			  System.Collections.ArrayList devices;
			  object source;

			  // Get all video input devices
			  devices = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);

			  // Take the first device
			  DsDevice device = (DsDevice)devices[0];

			  // Bind Moniker to a filter object
			  Guid iid = typeof(IBaseFilter).GUID;
			  device.Mon.BindToObject(null, null, ref iid, out source);

			  // An exception is thrown if cast fail
			  return (IBaseFilter) source;
			}
		*/
		public void GetInterfaces()
		{
			int hr = 0;

			// An exception is thrown if cast fail
			graphBuilder = (IGraphBuilder) new FilterGraph();
			captureGraphBuilder = (ICaptureGraphBuilder2) new CaptureGraphBuilder2();
			mediaControl = (IMediaControl) graphBuilder;
			videoWindow = (IVideoWindow) graphBuilder;
			mediaEventEx = (IMediaEventEx) graphBuilder;

			hr = mediaEventEx.SetNotifyWindow(Handle, WM_GRAPHNOTIFY, IntPtr.Zero);
			DsError.ThrowExceptionForHR(hr);
		}

		public void CloseInterfaces()
		{
			// Stop previewing data
			if (mediaControl != null)
				mediaControl.StopWhenReady();

			currentState = PlayState.Stopped;

			// Stop receiving events
			if (mediaEventEx != null)
				mediaEventEx.SetNotifyWindow(IntPtr.Zero, WM_GRAPHNOTIFY, IntPtr.Zero);

			// Relinquish ownership (IMPORTANT!) of the video window.
			// Failing to call put_Owner can lead to assert failures within
			// the video renderer, as it still assumes that it has a valid
			// parent window.
			if(videoWindow != null)
			{
				videoWindow.put_Visible(OABool.False);
				videoWindow.put_Owner(IntPtr.Zero);
			}

			// Remove filter graph from the running object table
			if (rot != null)
			{
				rot.Dispose();
				rot = null;
			}

			// Release DirectShow interfaces
			Marshal.ReleaseComObject(sourceFilter);
			sourceFilter = null;
			Marshal.ReleaseComObject(mediaControl);
			mediaControl = null;
			Marshal.ReleaseComObject(mediaEventEx);
			mediaEventEx = null;
			Marshal.ReleaseComObject(videoWindow);
			videoWindow = null;
			Marshal.ReleaseComObject(graphBuilder);
			graphBuilder = null;
			Marshal.ReleaseComObject(captureGraphBuilder);
			captureGraphBuilder = null;
		}

		public void SetupVideoWindow()
		{
			int hr = 0;

			// Set the video window to be a child of the main window
			hr = videoWindow.put_Owner(Handle);
			DsError.ThrowExceptionForHR(hr);

			hr = videoWindow.put_WindowStyle(WindowStyle.Child);
			DsError.ThrowExceptionForHR(hr);

			// Use helper function to position video window in client rect 
			// of main application window
			ResizeVideoWindow();

			// Make the video window visible, now that it is properly positioned
			hr = videoWindow.put_Visible(OABool.True);
			DsError.ThrowExceptionForHR(hr);
		}

		public void ResizeVideoWindow()
		{
			// Resize the video preview window to match owner window size
			if (videoWindow != null)
			{
				videoWindow.SetWindowPosition(0,0, ClientSize.Width,ClientSize.Height);
			}
		}

		public void ChangePreviewState(bool showVideo)
		{
			int hr = 0;

			// If the media control interface isn't ready, don't call it
			if (mediaControl == null)
				return;

			if (showVideo)
			{
				if (currentState != PlayState.Running)
				{
					// Start previewing video data
					hr = mediaControl.Run();
					currentState = PlayState.Running;
				}
			}
			else
			{
				// Stop previewing video data
				hr = mediaControl.StopWhenReady();
				currentState = PlayState.Stopped;
			}
		}

		public void HandleGraphEvent()
		{
			int hr = 0;
			EventCode evCode;
			int evParam1, evParam2;

			if (mediaEventEx == null)
				return;

			while(mediaEventEx.GetEvent(out evCode, out evParam1, out evParam2, 0) == 0)
			{
				// Free event parameters to prevent memory leaks associated with
				// event parameter data.  While this application is not interested
				// in the received events, applications should always process them.
				hr = mediaEventEx.FreeEventParams(evCode, evParam1, evParam2);
				DsError.ThrowExceptionForHR(hr);
        
				// Insert event processing code here, if desired
			}
		}
    
		protected override void WndProc(ref Message m)
		{
			switch (m.Msg)
			{
				case WM_GRAPHNOTIFY:
				{
					HandleGraphEvent();
					break;
				}
			}

			// Pass this message to the video window for notification of system changes
			if ( videoWindow != null )
				videoWindow.NotifyOwnerMessage(m.HWnd, m.Msg, m.WParam.ToInt32(), m.LParam.ToInt32());

			base.WndProc( ref m );
		}

		private void VideoControl_Resize(object sender, System.EventArgs e)
		{
			// Stop graph when Form is iconic
			if ( ParentForm != null )
			{
				if ( ParentForm.WindowState == FormWindowState.Minimized )
					ChangePreviewState(false);

				// Restart Graph when window come back to normal state
				if ( ParentForm.WindowState == FormWindowState.Normal )
					ChangePreviewState(true);
			}
			
			ResizeVideoWindow();
		}

		public void ShowTunerSettings()
		{
			DsHelp.ShowTunerPinDialog( captureGraphBuilder, sourceFilter, Handle );
		}

		public bool StartVideo()
		{
			if ( currentState != PlayState.Running )
				CaptureVideo();

			return true;
		}

		public bool StopVideo()
		{
			if ( currentState == PlayState.Running )
				CloseInterfaces();  

			return true;
		}

		public PlayState CurrentState
		{
			get { return currentState; }
		}
	}
}
