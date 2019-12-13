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
			TMessage structure;

			for (var i = 0; i < 10; i++)
			{
				structure = GenerateStructure();
				WriteStructure(structure);
			}

			var stopMesage = new TMessage
			{
				type = EType.STOP,
				size = 0,
				data = new List<int>()
			};
			WriteStructure(stopMesage);
		}

		public static void WriteStructure(TMessage structure)
		{
			using (var stdout = Console.OpenStandardOutput())
			{
				var serializedStruct = SerializeStruct(structure);
				stdout.Write(serializedStruct.ToArray(), 0, serializedStruct.Count);
			}
		}

		public static TMessage GenerateStructure()
		{
			var random = new Random();

			var type = random.Next((int)EType.FIBONACCI, (int)EType.STOP);
			var size = 0;
			var data = new List<int>();

			switch (type)
			{
				case (int)EType.FIBONACCI:
					size = 1;
					data = new List<int>() { random.Next(30) };
					break;
				case (int)EType.POW:
					size = 2;
					data = new List<int>() { random.Next(0, 5), random.Next(1, 10) };
					break;
				case (int)EType.BUBBLE_SORT_UINT64:
					size = random.Next(1, 50);
					data = new List<int>();
					for (var elementDataIndex = 0; elementDataIndex < size; elementDataIndex++)
					{
						var elementDataValue = random.Next(0, 500);
						data.Add(elementDataValue);
					}
					break;
				case (int)EType.STOP:
					break;
			}

			var structure = new TMessage()
			{
				type = (EType)type,
				size = size,
				data = data
			};

			return structure;
		}

		public static List<byte> SerializeStruct(TMessage structure)
		{
			var typeAsBites = BitConverter.GetBytes((int)structure.type).ToList();
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
