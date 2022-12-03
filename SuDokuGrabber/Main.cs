using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Drawing.Imaging;
using System.IO;
using System.Threading;

namespace SuDokuGrabber {
  public partial class Main : Form {
    Bitmap bmpMain;

    public Main() {
      InitializeComponent();
      pbImage.SizeMode = PictureBoxSizeMode.StretchImage;
    }

    // Load a new image
    private void btLoad_Click(object sender, EventArgs e) {
      OpenFileDialog dlg = new OpenFileDialog();
      //dlg.InitialDirectory = "c:\\";
      dlg.Filter = "All supported graphics|*.jpg;*.jpeg;*.png|" +
                   "JPEG (*.jpg;*.jpeg)|*.jpg;*.jpeg|" +
                   "Portable Network Graphic (*.png)|*.png";
      dlg.RestoreDirectory = true;

      if (dlg.ShowDialog() == DialogResult.OK) {
        string selectedFileName = dlg.FileName;
        tbFile.Text = System.IO.Path.GetFileName(selectedFileName);
        bmpMain = new Bitmap(selectedFileName);
        pbImage.Image = bmpMain;
      }
    }

    // Run the grid detection algorithm
    private void btRun_Click(object sender, EventArgs e) {
      Bitmap pbShow = new Bitmap(bmpMain.Width, bmpMain.Height);
      int[,] imgGreyscale = GetGreyscaleImage2();
      // Show greyscale image
      //for (int y = 0; y < bmpMain.Height; y++) {
      //  for (int x = 0; x < bmpMain.Width; x++) {
      //    pbShow.SetPixel(x, y, Color.FromArgb(255, imgGreyscale[x, y], imgGreyscale[x, y], imgGreyscale[x, y]));
      //  }
      //}
      //pbImage.Image = pbShow;
      
      bool[,] bThreshold = AdaptiveThreshold(imgGreyscale);
      //for (int y = 0; y < bmpMain.Height; y++) {
      //  for (int x = 0; x < bmpMain.Width; x++) {
      //    if (bThreshold[x, y]) {
      //      pbShow.SetPixel(x, y, Color.FromArgb(255, 100, 255, 100));
      //    }
      //    else {
      //      pbShow.SetPixel(x, y, Color.FromArgb(255, 70, 70, 70));
      //    }
      //  }
      //}
      //pbImage.Image = pbShow;

      bool[,] bRemoveNoise = RemoveNoise(bThreshold);
      for (int y = 0; y < bmpMain.Height; y++) {
        for (int x = 0; x < bmpMain.Width; x++) {
          if (bRemoveNoise[x, y]) {
            pbShow.SetPixel(x, y, Color.FromArgb(255, 100, 100, 255));
          }
          else {
            pbShow.SetPixel(x, y, Color.FromArgb(255, 70, 70, 70));
          }
        }
      }
      pbImage.Image = pbShow;    
    }


    // Get a version of the image as a 2D array with grayscale values for each point
    private int[,] GetGreyscaleImage() {
      int[,] imgGreyscale = new int[bmpMain.Width, bmpMain.Height];
      for (int y = 0; y < bmpMain.Height; y++) {
        for (int x = 0; x < bmpMain.Width; x++) {
          Color C = bmpMain.GetPixel(x, y);
          imgGreyscale[x, y] = (int)((float)C.R * 0.3f + (float)C.G * 0.59f + (float)C.B * 0.11f);
        }
      }
      return imgGreyscale;
    }

    // Threshold the image to a boolean array
    // Assumes that the img is at least 5x5...
    private bool[,] AdaptiveThreshold(int[,] imgGreyscale) {
      bool[,] bThreshold = new bool[bmpMain.Width, bmpMain.Height];
      for (int y = 0; y < bmpMain.Height; y++) {
        for (int x = 0; x < bmpMain.Width; x++) {
          int xmin = Math.Min(Math.Max(x - 2, 0), bmpMain.Width - 7);
          int ymin = Math.Min(Math.Max(y - 2, 0), bmpMain.Height - 7);
          int total = 0;
          for (int yy = ymin; yy < ymin + 7; yy++) {
            for (int xx = xmin; xx < xmin + 7; xx++) {
              total += imgGreyscale[xx, yy];
            }
          }
          total -= (imgGreyscale[x, y] * 54);
          if (total < 0) bThreshold[x, y] = false;
          else bThreshold[x, y] = true;
        }
      }
      return bThreshold;
    }

    // Remove pixels without many neighbours
    private bool[,] RemoveNoise(bool[,] bImage) {
      bool[,] bOutput = new bool[bmpMain.Width, bmpMain.Height];
      for (int y = 0; y < bmpMain.Height; y++) {
        for (int x = 0; x < bmpMain.Width; x++) {
          int total = 0;
          if (x > 0 && bImage[x - 1, y]) total++;
          if (x + 1 < bmpMain.Width && bImage[x + 1, y]) total++;
          if (y > 0 && bImage[x, y-1]) total++;
          if (y + 1 < bmpMain.Height && bImage[x, y+1]) total++;
          if (x > 0 && y > 0 && bImage[x - 1, y - 1]) total++;
          if (x > 0 && y + 1 < bmpMain.Height && bImage[x - 1, y + 1]) total++;
          if (x + 1 < bmpMain.Width && y > 0 && bImage[x + 1, y - 1]) total++;
          if (x + 1 < bmpMain.Width && y + 1 < bmpMain.Height && bImage[x + 1, y + 1]) total++;
          if (total < 3) bOutput[x, y] = false;
          else bOutput[x, y] = true;
        }
      }
      return bOutput;
    }

    // Get Bitmap as RGB array
    private int[,] GetGreyscaleImage2() {
      int[,] imgGreyscale = new int[bmpMain.Width, bmpMain.Height]; 
      const int PixelWidth = 3;
      const PixelFormat PixelFormat = PixelFormat.Format24bppRgb;
      BitmapData data = bmpMain.LockBits(new Rectangle(0, 0, bmpMain.Width, bmpMain.Height), System.Drawing.Imaging.ImageLockMode.ReadOnly, PixelFormat);
      try {
        byte[] pixelData = new Byte[data.Stride];
        for (int scanline = 0; scanline < data.Height; scanline++) {
          Marshal.Copy(data.Scan0 + (scanline * data.Stride), pixelData, 0, data.Stride);
          for (int pixeloffset = 0; pixeloffset < data.Width; pixeloffset++) {
            // PixelFormat.Format32bppRgb means the data is stored
            // in memory as BGR. We want RGB, so we must do some 
            // bit-shuffling.
            imgGreyscale[pixeloffset, scanline] = (int)((float)(pixelData[pixeloffset * PixelWidth + 2]) * 0.3f + (float)(pixelData[pixeloffset * PixelWidth + 1]) * 0.59f + (float)pixelData[pixeloffset * PixelWidth] * 0.11f);
          }
        }
      }
      finally {
        bmpMain.UnlockBits(data);
      }
      return imgGreyscale;
    }
  }
}
