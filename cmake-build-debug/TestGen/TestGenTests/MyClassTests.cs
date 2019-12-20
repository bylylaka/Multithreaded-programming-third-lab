using System.Collections.Generic;
using MathNet.Numerics.Distributions;
using System;
using System.Linq;
using NUnit.Framework;

namespace TestGen.Tests
{
	public class MyClassTests
	{
		private MyClass _myClass;

		public MyClassTests()
		{
			_myClass = new MyClass();
		}

		[Test]
		[Repeat(25)]
		public void GenerateStructureTest()
		{
			_myClass._distribution = new ContinuousUniform(0, 100);
			var structure = _myClass.GenerateStructure();

			switch (structure.type)
			{
				case MyClass.EType.FIBONACCI:
					Assert.IsTrue(structure.size == 1);
					Assert.IsTrue(structure.data.Count == 1);
					break;
				case MyClass.EType.POW:
					Assert.IsTrue(structure.size == 2);
					Assert.IsTrue(structure.data.Count == 2);
					break;
				case MyClass.EType.BUBBLE_SORT_UINT64:
					Assert.IsTrue(structure.size == structure.data.Count);
					break;
			}
		}

		[Test]
		public void SerializeStructTest()
		{
			var structure = new MyClass.TMessage()
			{
				type = MyClass.EType.FIBONACCI,
				size = 1,
				data = new List<int>() { 123 }
			};

			var serializedStructure = _myClass.SerializeStruct(structure);

			var deserialiedType = BitConverter.ToInt32(serializedStructure.Take(4).ToArray(), 0);
			serializedStructure = serializedStructure.Skip(4).ToList();

			var deserialiedSize = BitConverter.ToInt32(serializedStructure.Take(4).ToArray(), 0);
			serializedStructure = serializedStructure.Skip(4).ToList();

			var deserializedArray = DeserializeArray(serializedStructure);

			Assert.AreEqual(structure.type, (MyClass.EType) deserialiedType);
			Assert.AreEqual(structure.size, deserialiedSize);
			Assert.IsTrue(Enumerable.SequenceEqual(structure.data, deserializedArray));
		}

		[Test]
		public void SerializeArrayTest()
		{
			var array = new List<int>() { 1, 2, 3, 4, 5 };
			var serializedArray = _myClass.SerializeArray(array);
			var deserializedArray = DeserializeArray(serializedArray);

			Assert.IsTrue(Enumerable.SequenceEqual(array, deserializedArray));
		}

		[Test]
		public void TryGetDistributionTest()
		{
			var args = new string[] { "", "-mode", "uniform", "-param", "1", "100" };
			_myClass.TryGetDistribution(args, out var distribution);
			Assert.IsTrue(distribution.GetType() == typeof(ContinuousUniform));

			args = new string[] { "", "-mode", "uniform", "-param", "100" };
			Assert.IsFalse(_myClass.TryGetDistribution(args, out distribution));

			args = new string[] { "", "-mode", "exponencial", "-param", "100" };
			_myClass.TryGetDistribution(args, out distribution);
			Assert.IsTrue(distribution.GetType() == typeof(Exponential));

			args = new string[] { "", "-mode", "exponencial", "-param" };
			Assert.IsFalse(_myClass.TryGetDistribution(args, out distribution));

			args = new string[] { "", "-mode", "normal", "-param", "100", "500" };
			_myClass.TryGetDistribution(args, out distribution);
			Assert.IsTrue(distribution.GetType() == typeof(Normal));

			args = new string[] { "", "-mode", "normal", "-param" };
			Assert.IsFalse(_myClass.TryGetDistribution(args, out distribution));

			args = new string[] { "", "-mode", "notExisted", "-param" };
			Assert.IsFalse(_myClass.TryGetDistribution(args, out distribution));
		}

		private List<int> DeserializeArray(List<byte> serializedArray)
		{
			var deserializedArray = new List<int>();

			for (var i = 0; i < serializedArray.Count; i += 4)
			{
				var serializedNumber = serializedArray.Skip(i).Take(4);
				var deserializeNumber = BitConverter.ToInt32(serializedNumber.ToArray(), 0);
				deserializedArray.Add(deserializeNumber);
			}
			return deserializedArray;
		}
	}
}