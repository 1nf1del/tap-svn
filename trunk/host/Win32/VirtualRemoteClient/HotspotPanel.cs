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
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace VirtualRemote
{
	class HotspotPanel : Panel
	{
		public delegate void RegionClickDelegate(int index);
		[Category("Action")]
		public event RegionClickDelegate RegionClick;

		public delegate void RegionMouseDownDelegate(int index);
		[Category("Action")]
		public event RegionMouseDownDelegate RegionMouseDown;

		public delegate void RegionMouseUpDelegate(int index);
		[Category("Action")]
		public event RegionMouseUpDelegate RegionMouseUp;


		[DefaultValue((string)null), Localizable(true), Description("Hotspot image"), Category("Behavior")]
		public Image HotspotImage
		{
			get { return hotspotImage; }
			set { hotspotImage = value; }
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			int newIndex = this.getActiveIndexAtPoint(new Point(e.X, e.Y));
			if (newIndex > -1)
			{
				Cursor = Cursors.Hand;
				//if (this.activeIndex != newIndex)
				//	this.toolTip.SetToolTip(this.pictureBox, this._pathsArray[newIndex].ToString());
			}
			else
			{
				Cursor = Cursors.Default;
				//this.toolTip.RemoveAll();
			}
			this.activeIndex = newIndex;
			base.OnMouseMove(e);
		}

		protected override void OnMouseLeave(EventArgs e)
		{
			activeIndex = -1;
			Cursor = Cursors.Default;
			base.OnMouseLeave(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			Capture = true;
			Point p = this.PointToClient(Cursor.Position);
			if (this.activeIndex == -1)
				this.getActiveIndexAtPoint(p);
			if (this.activeIndex > -1 && this.RegionMouseDown != null)
				this.RegionMouseDown(this.activeIndex);
			base.OnMouseDown(e);
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			Capture = false;
			Point p = this.PointToClient(Cursor.Position);
			if (activeIndex == -1)
				getActiveIndexAtPoint(p);
			if (RegionMouseUp != null)
				RegionMouseUp(this.activeIndex);
			base.OnMouseUp(e);
		}

		protected override void OnClick(EventArgs e)
		{
			Point p = this.PointToClient(Cursor.Position);
			if (this.activeIndex == -1)
				this.getActiveIndexAtPoint(p);
			if (this.activeIndex > -1 && this.RegionClick != null)
				this.RegionClick(this.activeIndex);
			base.OnClick(e);
		}

		private int getActiveIndexAtPoint(Point point)
		{
			if ( hotspotImage == null )
				return -1;
			Bitmap b = (Bitmap)hotspotImage;
			if (point.X >= b.Width || point.Y >= b.Height)
				return -1;
			Color c = b.GetPixel(point.X, point.Y);
			return c.R != 0 ? c.R : -1;
		}

		private int activeIndex = -1;
		private ToolTip toolTip;

		private Image hotspotImage;	
	}
}
