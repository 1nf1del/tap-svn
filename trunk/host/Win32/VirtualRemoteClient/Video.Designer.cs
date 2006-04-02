namespace VirtualRemote
{
	partial class VideoControl
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
				if (System.Diagnostics.Process.GetCurrentProcess().ProcessName.ToLower() != "devenv" &&
					System.Diagnostics.Process.GetCurrentProcess().ProcessName.ToLower() != "vcsexpress")
					CloseInterfaces();
			}
			base.Dispose(disposing);
		}

		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.SuspendLayout();
			// 
			// VideoControl
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Name = "VideoControl";
			this.Resize += new System.EventHandler(this.VideoControl_Resize);
			this.ResumeLayout(false);

		}

		#endregion
	}
}
