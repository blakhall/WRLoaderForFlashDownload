using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Threading;

namespace DSPKLoader
{
    public enum Command
    {
        data,
        fileEnd,
        extendedSegementAddress,
        startSegmentAddress,
        startLienarAddress
    }
    public struct IntelFormat
    {
        public int[] data;
        public int checkSum;
        public int length;
        public Command command;
        public int address;
    }
    public partial class Form1 : Form
    {
        const string startCode = ":";
        private Stream myStream;
        private List<IntelFormat> fileDataList = new List<IntelFormat>();
        private List<string> str = new List<string>();
        private int iCnt = 0;
        delegate void SetTextCallback(string text);
        private Progress progressForm;
        private byte keyCode = 0x00;
        public Form1()
        {
            InitializeComponent();
            //this.progressFrom = new Progress(this.serialPort1);
        }

        private void ThreadComm()
        {
            byte[] bTemp = new byte[0x100];
            try
            {
                this.serialPort1.Open();
                this.serialPort1.ReadTimeout = 1000;
                bTemp[0] = (byte)'[';
                bTemp[1] = this.keyCode;
                bTemp[2] = (byte)']';
                this.serialPort1.Write(bTemp, 0, 0x03);
                Thread.Sleep(1);
                if (this.keyCode == (byte)'G')
                {
                    for (int i = 0; i < 0x03; i++) bTemp[i] = (byte)this.serialPort1.ReadByte();
                    if (bTemp[0] == 0x5B && bTemp[2] == 0x5D)
                    {
                        if (bTemp[1] != (byte)'R') MessageBox.Show("Load.. Fail!" + bTemp[1].ToString() + "..." + ((byte)'R').ToString());
                        else MessageBox.Show("전송완료!!");
                    }
                }
                else if(this.keyCode==(byte)'I')
                {
                    for (int i = 0; i < 0x03; i++) bTemp[i] = (byte)this.serialPort1.ReadByte();
                    if (bTemp[0] == 0x5B && bTemp[2] == 0x5D)
                    {
                        MessageBox.Show("Info=" + bTemp[1].ToString());
                    }

                }
                //MessageBox.Show("파일전송완료!!");
            }
            catch
            {
                MessageBox.Show("전송포트열기실패l!");
            }
            this.serialPort1.Close();
            //MessageBox.Show("전송포트열기실패l!");
        }
        private void SetText( string text)
        {
            if (this.richTextBox1.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { text });
            }
            else this.richTextBox1.AppendText(text);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string stringTemp;
            if (this.openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                this.myStream = this.openFileDialog1.OpenFile();
                StreamReader sr;
                sr = new StreamReader(myStream, Encoding.Default);
                while ((stringTemp = sr.ReadLine()) != null)
                {
                    str.Add(stringTemp);
                    richTextBox1.AppendText(stringTemp+"\n");
                    //Thread.Sleep(100);
                }
                richTextBox1.AppendText("filesize=" + str.Count.ToString());
                this.progressForm = new Progress(this.serialPort1);
                this.progressForm.FileName = this.openFileDialog1.FileName;
                this.progressForm.SetSerialdata(str);
                this.progressForm.ShowDialog();
            }
        }
        private void sendData()
        {
            for(int i=0;i< this.fileDataList.Count;i++)
            {

            }

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            //this.richTextBox1.AppendText("count=" + this.iCnt.ToString());
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.keyCode = (byte)'G';
            Thread t1 = new Thread(new ThreadStart(ThreadComm));
            t1.Start();

            // 메인쓰레드에서 Run() 실행
            //Run();

        }

        private void button3_Click(object sender, EventArgs e)
        {
            this.keyCode = (byte)'I';
            Thread t1 = new Thread(new ThreadStart(ThreadComm));
            t1.Start();
        }
        private byte[] getExtract(string _s)
        {
            string s= ":02000004000BEF";
            byte[] bTemp = new byte[s.Length / 2 + 0x01];
            int _icnt = 0;
            bTemp[_icnt++] =Convert.ToByte(Convert.ToChar(s.Substring(0, 1)));
            for (int i = 1; i < s.Length; i += 2)
            {
                bTemp[_icnt++] = Convert.ToByte(s.Substring(i, 2), 0x10);
            }
            return bTemp;

        }

        private void button4_Click(object sender, EventArgs e)
        {
            this.richTextBox1.Clear();
        }
    }
}
