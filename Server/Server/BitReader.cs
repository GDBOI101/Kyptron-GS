using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Kyptron.Server
{
    class BitReader
    {
        int index = 0;
        byte[] Data;
        public BitReader(byte[] Data)
        {
            this.Data = Data;
        }

        public byte[] Read()
        {
            try
            {
                int index_ = 0;
                List<byte> Bytes = new List<byte>();
                foreach (var Byte in Data)
                {
                    if (Byte == '\0')
                    {
                        if (index != index_)
                        {
                            index_++;
                        }
                        else
                        {
                            index++;
                            return Bytes.ToArray();
                        }
                    }
                    else if (index == index_)
                    {
                        Bytes.Add(Byte);
                    }
                }
                return null;
            }
            catch
            {
                return new byte[] { 0x8 };
            }
        }

        public string ReadString()
        {
            return Encoding.UTF8.GetString(Read());
        }

        public float ReadFloat()
        {
            return float.Parse(ReadString());
        }

        public int ReadInt()
        {
            return int.Parse(ReadString());
        }

        public string PeekString()
        {
            var Return = ReadString();
            index--;
            return Return;
        }

        public float PeekFloat()
        {
            var Return = ReadFloat();
            index--;
            return Return;
        }

        public int PeekInt()
        {
            var Return = ReadInt();
            index--;
            return Return;
        }
    }
}
