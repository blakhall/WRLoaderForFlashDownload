using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace DSPKLoader
{
    delegate void CloseCallback();
    public partial class Progress : Form
    {
        private Thread t;
        private List<string> str = new List<string>();
        private int percent;
        private string fileName;
        private System.IO.Ports.SerialPort serialPort1;
        public Progress()
        {
            InitializeComponent();
        }
        public Progress(System.IO.Ports.SerialPort sp)
        {
            this.serialPort1 = sp;
            InitializeComponent();
        }
        public void SetSerialdata(List<string> s)
        {
            this.str = s;

        }
        private byte[] getExtract(string s)
        {
            byte[] bTemp = new byte[s.Length / 2 + 0x01];
            int _icnt = 0;
            bTemp[_icnt++] = Convert.ToByte(Convert.ToChar(s.Substring(0, 1)));
            for (int i = 1; i < s.Length; i += 2)
            {
                bTemp[_icnt++] = Convert.ToByte(s.Substring(i, 2), 0x10);
            }
            return bTemp;

        }
        private void ThreadComm()
        {
            byte[] bTemp = new byte[0x100];
            try
            {
                this.serialPort1.Open();
                this.serialPort1.ReadTimeout = 1000;
                bTemp[0] = (byte)'[';
                bTemp[1] = (byte)'S';
                bTemp[2] = (byte)']';
                this.serialPort1.Write(bTemp, 0, 0x03);
                Thread.Sleep(1);
                for (int i = 0; i < 0x03; i++) bTemp[i] = (byte)this.serialPort1.ReadByte();
                if (bTemp[0] == 0x5B && bTemp[2] == 0x5D)
                {
                    if (bTemp[1] != (byte)'R') MessageBox.Show("Load.. Fail!" + bTemp[1].ToString() + "..." + ((byte)'R').ToString());
                }
                for (int i = 0; i < this.str.Count ; i++)
                {
                    bTemp = getExtract(str[i]);
                    this.serialPort1.Write(bTemp, 0, bTemp.Length);
                    this.percent = (i + 1) * 100 / this.str.Count;
                    Thread.Sleep(1);
                    for (int j = 0; j < 0x03; j++) bTemp[j] = (byte)this.serialPort1.ReadByte();
                    if (bTemp[0]==0x5B && bTemp[2]==0x5D)
                    {
                        if (bTemp[1] == (byte)'R') continue;
                        else MessageBox.Show("Load.. Fail!");
                        break;

                    }
                    //if (this.serialPort1.ReadByte() == 0xAA) continue;
                    //else MessageBox.Show("Load.. Fail!");
//                    Thread.Sleep(10);
                    //SetText("\n"+ this.serialPort1.ReadExisting());
                }
                this.serialPort1.Close();
                MessageBox.Show("파일전송완료!!");
            }
            catch
            {
                MessageBox.Show("전송포트열기실패l!");
                this.serialPort1.Close();

            }
            this.FormClose();


        }
        private void FormClose()
        {
            if (this.InvokeRequired)
            {
                CloseCallback d = new CloseCallback(FormClose);
                this.Invoke(d);

            }
            else
            {
                this.Close();
            }
        }
        public string FileName
        {
            set
            {
                this.fileName = value;
                this.label1.Text = value;
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            ThreadStart methodDelegate = new ThreadStart(ThreadComm);
            t = new Thread(methodDelegate);
            t.IsBackground = true;
            this.t.Start();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            this.progressBar1.Value = this.percent;
            this.percentLabel.Text = this.percent.ToString() + @"%";
        }
    }
}
