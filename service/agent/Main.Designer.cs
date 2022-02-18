
namespace agent
{
    partial class Main
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージド リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main));
            this.notifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.contextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.openSettingsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitApplicationMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.forceChangeMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // notifyIcon
            // 
            this.notifyIcon.ContextMenuStrip = this.contextMenuStrip;
            this.notifyIcon.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon.Icon")));
            this.notifyIcon.Text = "通話中インジケーター";
            this.notifyIcon.Visible = true;
            this.notifyIcon.DoubleClick += new System.EventHandler(this.notifyIconDoubleClicked);
            // 
            // timer
            // 
            this.timer.Interval = 3000;
            this.timer.Tick += new System.EventHandler(this.onTimer);
            // 
            // contextMenuStrip
            // 
            this.contextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openSettingsMenuItem,
            this.forceChangeMenuItem,
            this.toolStripSeparator1,
            this.exitApplicationMenuItem});
            this.contextMenuStrip.Name = "contextMenuStrip";
            this.contextMenuStrip.Size = new System.Drawing.Size(170, 76);
            // 
            // openSettingsMenuItem
            // 
            this.openSettingsMenuItem.Name = "openSettingsMenuItem";
            this.openSettingsMenuItem.Size = new System.Drawing.Size(180, 22);
            this.openSettingsMenuItem.Text = "設定(&S)...";
            this.openSettingsMenuItem.Click += new System.EventHandler(this.openSettingsMenuItemClicked);
            // 
            // exitApplicationMenuItem
            // 
            this.exitApplicationMenuItem.Name = "exitApplicationMenuItem";
            this.exitApplicationMenuItem.Size = new System.Drawing.Size(180, 22);
            this.exitApplicationMenuItem.Text = "終了(&E)";
            this.exitApplicationMenuItem.Click += new System.EventHandler(this.exitApplicationMenuItemClicked);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(177, 6);
            // 
            // forceChangeMenuItem
            // 
            this.forceChangeMenuItem.Name = "forceChangeMenuItem";
            this.forceChangeMenuItem.Size = new System.Drawing.Size(180, 22);
            this.forceChangeMenuItem.Text = "状態を強制変更(&F)";
            this.forceChangeMenuItem.Click += new System.EventHandler(this.forceChangeMenuItemClicked);
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(254, 182);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Main";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.WindowState = System.Windows.Forms.FormWindowState.Minimized;
            this.contextMenuStrip.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NotifyIcon notifyIcon;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem openSettingsMenuItem;
        private System.Windows.Forms.ToolStripMenuItem forceChangeMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem exitApplicationMenuItem;
    }
}

