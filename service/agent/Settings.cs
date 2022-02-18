using System;
using System.Windows.Forms;

namespace agent
{
    public partial class Settings : Form
    {
        public Settings()
        {
            this.InitializeComponent();
        }

        private void onLoad(object sender, EventArgs e)
        {
            this.managerEndpointUri.Text = Properties.Settings.Default.ManagerEndpointUri;
        }

        private void okButtonClicked(object sender, EventArgs e)
        {
            Properties.Settings.Default.ManagerEndpointUri = this.managerEndpointUri.Text;
            Properties.Settings.Default.Save();

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void cancelButtonClicked(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}
