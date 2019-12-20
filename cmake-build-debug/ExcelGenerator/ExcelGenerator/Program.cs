using OfficeOpenXml;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace ExcelGenerator
{
	class Program
	{
		static void Main(string[] args)
		{
			var runningFibonacci = new List<int>();
			var runningPow = new List<int>();
			var runningBubbleSort = new List<int>();

			var readingFibonacci = new List<int>();
			var readingPow = new List<int>();
			var readingBubbleSort = new List<int>();

			var writingFibonacci = new List<int>();
			var writingPow = new List<int>();
			var writingBubbleSort = new List<int>();

			var metricFile = new StreamReader("D:\\CLionProjects\\lab3\\cmake-build-debug\\metric.txt");

			var a = metricFile.ReadLine();
			for (var i = 0; i < 100; i++)
			{
				var line = metricFile.ReadLine();
				var parsedValue = int.Parse(line);
				runningFibonacci.Add(parsedValue);
			}
			a = metricFile.ReadLine();
			a = metricFile.ReadLine();
			a = metricFile.ReadLine();
			for (var i = 0; i < 100; i++)
			{
				var line = metricFile.ReadLine();
				var parsedValue = int.Parse(line);
				runningPow.Add(parsedValue);
			}
			a = metricFile.ReadLine();
			a = metricFile.ReadLine();
			a = metricFile.ReadLine();
			for (var i = 0; i < 100; i++)
			{
				var line = metricFile.ReadLine();
				var parsedValue = int.Parse(line);
				runningBubbleSort.Add(parsedValue);
			}
			a = metricFile.ReadLine();
			a = metricFile.ReadLine();
			a = metricFile.ReadLine();
			for (var i = 0; i < 100; i++)
			{
				var line = metricFile.ReadLine();
				var parsedValue = int.Parse(line);
				readingFibonacci.Add(parsedValue);
			}
			metricFile.ReadLine();
			metricFile.ReadLine();
			metricFile.ReadLine();
			for (var i = 0; i < 100; i++)
			{
				var line = metricFile.ReadLine();
				var parsedValue = int.Parse(line);
				readingPow.Add(parsedValue);
			}
			metricFile.ReadLine();
			metricFile.ReadLine();
			metricFile.ReadLine();
			for (var i = 0; i < 100; i++)
			{
				var line = metricFile.ReadLine();
				var parsedValue = int.Parse(line);
				readingBubbleSort.Add(parsedValue);
			}
			metricFile.ReadLine();
			metricFile.ReadLine();
			metricFile.ReadLine();
			for (var i = 0; i < 100; i++)
			{
				var line = metricFile.ReadLine();
				var parsedValue = int.Parse(line);
				writingFibonacci.Add(parsedValue);
			}
			metricFile.ReadLine();
			metricFile.ReadLine();
			metricFile.ReadLine();
			for (var i = 0; i < 100; i++)
			{
				var line = metricFile.ReadLine();
				var parsedValue = int.Parse(line);
				writingPow.Add(parsedValue);
			}
			metricFile.ReadLine();
			metricFile.ReadLine();
			metricFile.ReadLine();
			for (var i = 0; i < 100; i++)
			{
				var line = metricFile.ReadLine();
				var parsedValue = int.Parse(line);
				writingBubbleSort.Add(parsedValue);
			}
			metricFile.ReadLine();
			metricFile.Close();

			var excelFile = new FileInfo("D:\\CLionProjects\\lab3\\многопоточка 3.xlsx");
			var excelPackage = new ExcelPackage(excelFile);
			var excelWorkBook = excelPackage.Workbook;
			var excelWorksheet = excelWorkBook.Worksheets.First();

			foreach (var item in runningFibonacci.Select((value, index) => (value, index)))
			{
				excelWorksheet.Cells[item.index + 2, 1].Value = item.value;
			}
			foreach (var item in runningPow.Select((value, index) => (value, index)))
			{
				excelWorksheet.Cells[item.index + 2, 2].Value = item.value;
			}
			foreach (var item in runningBubbleSort.Select((value, index) => (value, index)))
			{
				excelWorksheet.Cells[item.index + 2, 3].Value = item.value;
			}

			foreach (var item in readingFibonacci.Select((value, index) => (value, index)))
			{
				excelWorksheet.Cells[item.index + 2, 4].Value = item.value;
			}
			foreach (var item in readingPow.Select((value, index) => (value, index)))
			{
				excelWorksheet.Cells[item.index + 2, 5].Value = item.value;
			}
			foreach (var item in readingBubbleSort.Select((value, index) => (value, index)))
			{
				excelWorksheet.Cells[item.index + 2, 6].Value = item.value;
			}

			foreach (var item in writingFibonacci.Select((value, index) => (value, index)))
			{
				excelWorksheet.Cells[item.index + 2, 7].Value = item.value;
			}
			foreach (var item in writingPow.Select((value, index) => (value, index)))
			{
				excelWorksheet.Cells[item.index + 2, 8].Value = item.value;
			}
			foreach (var item in writingBubbleSort.Select((value, index) => (value, index)))
			{
				excelWorksheet.Cells[item.index + 2, 9].Value = item.value;
			}

			excelPackage.Save();
		}
	}
}
