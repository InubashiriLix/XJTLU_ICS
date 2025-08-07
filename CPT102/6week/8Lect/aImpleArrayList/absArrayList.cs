using System;
using System.Collections.Generic;

namespace ns_abs_ArrayList
{
    class Pack<T>
    {
        private int Size
        {
            get { return Size; }
            set { Size = Math.Min(1, Math.Max(value, Size)); }
        }

        public Pack(T size)
        {
            int _temp_size;
            if (!int.TryParse(size.ToString(), out _temp_size))
            {
                throw new Exception($"invalid size: {size.ToString()} to int");
            }
        }

        private read
    }
}
