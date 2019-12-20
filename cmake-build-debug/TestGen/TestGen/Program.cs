using System;
using System.Collections.Generic;
using static TestGen.MyClass;

namespace TestGen
{
	class Program
	{
		static void Main(string[] args)
		{
			var myClass = new MyClass();

			if (!myClass.TryGetDistribution(args, out myClass._distribution))
			{
				Console.WriteLine("Plase, define -mode (\"uniform\", \"exponential\") and params for distribution.");
				Environment.Exit(1);
			}

			for (var i = 0; i < 2000; i++)
			{
				TMessage structure = myClass.GenerateStructure();
				myClass.WriteStructure(structure);
			}

			var stopMesage = new TMessage
			{
				type = EType.STOP,
				size = 0,
				data = new List<int>()
			};
			myClass.WriteStructure(stopMesage);
		}
	}
}
