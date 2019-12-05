using System;
using System.Collections.Generic;
using System.Linq;

namespace TestGen
{
	class Program
	{
		public enum EType
		{
			FIBONACCI,
			POW,
			BUBBLE_SORT_UINT64,
			STOP
		}

		public struct TMessage
		{
			public EType type;
			public int size;
			public List<int> data;
		}

		static void Main(string[] args)
		{
			var structure = new TMessage()
			{
				type = EType.STOP,
				size = 5,
				data = new List<int>() { 11111, 22222, 33333, 55555, 44444 }
			};

			using (var stdout = Console.OpenStandardOutput())
			{
				var serializedStruct = SerializeStruct(structure);

				stdout.Write(serializedStruct.ToArray(), 0, serializedStruct.Count);


				//byte[] intBytes = BitConverter.GetBytes(intValue);
				////Array.Reverse(intBytes);
				//byte[] bytes = intBytes;
				//stdout.Write(bytes, 0, bytes.Length);
			}
		}

		public static List<byte> SerializeStruct(TMessage structure)
		{
			var typeAsBites = BitConverter.GetBytes((int) structure.type).ToList();
			var sizeAsBites = BitConverter.GetBytes(structure.size).ToList();

			var dataAsBytes = SerializeArray(structure.data).ToList();
			var structAsBytes = typeAsBites.Concat(sizeAsBites).Concat(dataAsBytes);
			return structAsBytes.ToList();
		}

		public static List<byte> SerializeArray(List<int> array)
		{
			var intBytes = new List<byte>();

			foreach (var item in array)
			{
				intBytes = intBytes.Concat(BitConverter.GetBytes(item)).ToList();
			}

			return intBytes;
		}
	}
}
