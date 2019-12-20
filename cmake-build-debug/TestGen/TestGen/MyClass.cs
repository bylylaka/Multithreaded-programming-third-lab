using MathNet.Numerics.Distributions;
using System;
using System.Collections.Generic;
using System.Linq;

namespace TestGen
{
	public class MyClass
	{
		public IContinuousDistribution _distribution;

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

		public void WriteStructure(TMessage structure)
		{
			using (var stdout = Console.OpenStandardOutput())
			{
				var serializedStruct = SerializeStruct(structure);
				stdout.Write(serializedStruct.ToArray(), 0, serializedStruct.Count);
			}
		}

		public TMessage GenerateStructure()
		{
			var random = new Random();

			var type = random.Next((int)EType.FIBONACCI, (int)EType.STOP);
			var size = 0;
			var data = new List<int>();

			switch (type)
			{
				case (int)EType.FIBONACCI:
					size = 1;
					data = new List<int>() { (int)Math.Round(_distribution.Sample()) };
					break;
				case (int)EType.POW:
					size = 2;
					data = new List<int>() { (int)Math.Round(_distribution.Sample()), (int)Math.Round(_distribution.Sample()) };
					break;
				case (int)EType.BUBBLE_SORT_UINT64:
					size = 50;
					data = new List<int>();
					for (var elementDataIndex = 0; elementDataIndex < size; elementDataIndex++)
					{
						data.Add((int)Math.Round(_distribution.Sample()));
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

		public List<byte> SerializeStruct(TMessage structure)
		{
			var typeAsBites = BitConverter.GetBytes((int)structure.type).ToList();
			var sizeAsBites = BitConverter.GetBytes(structure.size).ToList();

			var dataAsBytes = SerializeArray(structure.data).ToList();
			var structAsBytes = typeAsBites.Concat(sizeAsBites).Concat(dataAsBytes);
			return structAsBytes.ToList();
		}

		public List<byte> SerializeArray(List<int> array)
		{
			var intBytes = new List<byte>();

			foreach (var item in array)
			{
				intBytes = intBytes.Concat(BitConverter.GetBytes(item)).ToList();
			}

			return intBytes;
		}

		public bool TryGetDistribution(string[] args, out IContinuousDistribution distribution)
		{
			distribution = null;
			var mode = string.Empty;
			var parameters = new List<int>();
			var parametersIndex = args.ToList().FindIndex(arg => string.Equals("-param", arg));

			foreach (var arg in args.Select((value, index) => (value, index)).SkipLast(1))
			{
				if (string.Equals(arg.value, "-mode"))
				{
					mode = args[arg.index + 1].ToLower();
					break;
				}
			}

			for (var i = parametersIndex + 1; i < args.Count() && parametersIndex != -1; i++)
			{
				if (int.TryParse(args[i], out var parsedNumber))
				{
					parameters.Add(parsedNumber);
				}
			}

			switch (mode)
			{
				case "uniform":
					if (parameters.Count != 2)
					{
						return false;
					}
					distribution = new ContinuousUniform(parameters[0], parameters[1]);
					return true;
				case "exponencial":
					if (parameters.Count != 1)
					{
						return false;
					}
					distribution = new Exponential(parameters[0]);
					return true;
				case "normal":
					if (parameters.Count != 2)
					{
						return false;
					}
					distribution = new Normal(parameters[0], parameters[1]);
					return true;
				default:
					return false;
			}
		}
	}
}
