namespace SuDokuGrabber {
  partial class Main {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing) {
      if (disposing && (components != null)) {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent() {
      this.pbImage = new System.Windows.Forms.PictureBox();
      this.btLoad = new System.Windows.Forms.Button();
      this.btRun = new System.Windows.Forms.Button();
      this.tbFile = new System.Windows.Forms.TextBox();
      ((System.ComponentModel.ISupportInitialize)(this.pbImage)).BeginInit();
      this.SuspendLayout();
      // 
      // pbImage
      // 
      this.pbImage.Location = new System.Drawing.Point(-2, -1);
      this.pbImage.Name = "pbImage";
      this.pbImage.Size = new System.Drawing.Size(502, 400);
      this.pbImage.TabIndex = 0;
      this.pbImage.TabStop = false;
      // 
      // btLoad
      // 
      this.btLoad.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.btLoad.Location = new System.Drawing.Point(-2, 405);
      this.btLoad.Name = "btLoad";
      this.btLoad.Size = new System.Drawing.Size(117, 37);
      this.btLoad.TabIndex = 1;
      this.btLoad.Text = "Load Image";
      this.btLoad.UseVisualStyleBackColor = true;
      this.btLoad.Click += new System.EventHandler(this.btLoad_Click);
      // 
      // btRun
      // 
      this.btRun.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.btRun.Location = new System.Drawing.Point(383, 405);
      this.btRun.Name = "btRun";
      this.btRun.Size = new System.Drawing.Size(117, 37);
      this.btRun.TabIndex = 2;
      this.btRun.Text = "Run Grabber";
      this.btRun.UseVisualStyleBackColor = true;
      this.btRun.Click += new System.EventHandler(this.btRun_Click);
      // 
      // tbFile
      // 
      this.tbFile.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.tbFile.Location = new System.Drawing.Point(122, 408);
      this.tbFile.Name = "tbFile";
      this.tbFile.ReadOnly = true;
      this.tbFile.Size = new System.Drawing.Size(252, 26);
      this.tbFile.TabIndex = 3;
      this.tbFile.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // Main
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(499, 440);
      this.Controls.Add(this.tbFile);
      this.Controls.Add(this.btRun);
      this.Controls.Add(this.btLoad);
      this.Controls.Add(this.pbImage);
      this.MaximizeBox = false;
      this.Name = "Main";
      this.Text = "Form1";
      ((System.ComponentModel.ISupportInitialize)(this.pbImage)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.PictureBox pbImage;
    private System.Windows.Forms.Button btLoad;
    private System.Windows.Forms.Button btRun;
    private System.Windows.Forms.TextBox tbFile;
  }
}

