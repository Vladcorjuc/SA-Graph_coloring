#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace std::chrono;

constexpr unsigned int iterations = 20;
constexpr unsigned char bits = 5;
constexpr unsigned int runs = 32; 

void getFiles(vector<string>& files) {
	files.emplace_back("anna.txt");
	files.emplace_back("david.txt");
	files.emplace_back("games120.txt");
	files.emplace_back("homer.txt");
	files.emplace_back("le450_5.txt");
	files.emplace_back("le450_15.txt");
	files.emplace_back("le450_25.txt");
	files.emplace_back("miles500.txt");
	files.emplace_back("miles1500.txt");
	files.emplace_back("myciel5.txt");
	files.emplace_back("myciel7.txt");
	files.emplace_back("queen8_12.txt");
	files.emplace_back("queen13_13.txt");
}

int randomInt(int leftEnd, unsigned int rightEnd) {
	// THIS ONE IS FOR A RANDOM INT NUMBER IN [leftEnd, rightEND] :P (LIMBUTZA :P)
	const uniform_int_distribution<int> distribution(leftEnd, rightEnd);
	const int seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	return distribution(generator);
}

double randomDouble(double leftEnd, double rightEnd) {
	// THIS ONE IS FOR A RANDOM INT NUMBER IN (leftEnd, rightEnd) :P (LIMBUTZA :P)
	const uniform_real_distribution<double> distribution(leftEnd, rightEnd);
	const int seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	return distribution(generator);
}


class Graph
{
	vector<pair<int, int>> edges;
	int** adjacentMatrix;
	vector<vector<int>> population;
	string fileName;
	int nodesNumber;
	int edgesNumber;
	int maxColorsNumber;


	int dimensions;

	int* candidateArguments;
	double candidateValue;

	int* neighbourArguments;
	double neighbourValue;

	int* optimumArguments;
	double optimumValue;

	void readFile() {
		ifstream file(fileName);
		file >> nodesNumber >> edgesNumber;
		vector<pair<int, int>> newEdges;
		adjacentMatrix = new int*[nodesNumber];
		adjacentMatrix[0] = new int[nodesNumber * nodesNumber];
		for (int i = 1; i < (nodesNumber); ++i)
			adjacentMatrix[i] = adjacentMatrix[0] + i * nodesNumber;
		for (int i = 0; i < nodesNumber; i++)
			for (int j = 0; j < nodesNumber; j++)
				adjacentMatrix[i][j] = 0;
		for (int i = 0; i < edgesNumber; i++) {
			int firstNode, secondNode;
			file >> firstNode >> secondNode;
			adjacentMatrix[firstNode - 1][secondNode - 1] = 1;
		}
		maxColorsNumber = 0;
		for (int i = 0; i < nodesNumber; i++) {
			int neighbours = 0;
			for (int j = 0; j < nodesNumber; j++)
				if (adjacentMatrix[i][j] == 1)
					neighbours++;
			if (neighbours > maxColorsNumber)
				maxColorsNumber = neighbours;
		}
		maxColorsNumber++;
		file.close();
	}


	double Color_rate(int * candidate) {
		int maxIndividualColor = candidate[0];
		int badEdges = 0;
		for (int j = 0; j < nodesNumber; j++) {
			if (candidate[j] > maxIndividualColor) {
				maxIndividualColor = candidate[j];
			}
			for (int k = 0; k < nodesNumber; k++) {
				if (candidate[j] == candidate[k] && existsEdge(j, k)) {
					badEdges++;
				}
			}
		}
		double individualFitness = -((static_cast<double>(100 * badEdges) + (50 * maxIndividualColor))) / edgesNumber;
		return individualFitness;
	}
	void getAdjacentColors(int node, set<int>& adjacentColors, int* individual) {
		for (int j = 0; j < nodesNumber; j++) {
			if (adjacentMatrix[node][j] == 1) {
				adjacentColors.insert(j);
			}
		}
	}

	bool isColorOK(int candidate,vector<int> exception) {
		for (const auto& color : exception) {
			if (color == candidate)
				return false;
		}
		return true;
	}
	int newRandomColor(int index, int maxColor, int* individual) {
		vector<int> exception;
		exception.push_back(individual[index]);
		for (int i = 0; i < nodesNumber; i++) {
			if (adjacentMatrix[index][i] == 1) {
				exception.push_back(individual[i]);
			}
		}
		int candidate = 3;
		while (!isColorOK(candidate, exception)) {
			candidate = randomInt(1, maxColor);
		}
		return candidate;
	}
	
	bool existsEdge(int firstNode, int secondNode) {
		if (adjacentMatrix[firstNode][secondNode] == 1)
			return true;
		return false;

	}


	void GenerateCandidate() {
		for (int index = 0; index < dimensions; ++index)
		{
			candidateArguments[index] = 1;
		}
	}
	void EvaluateCandidate() {
		candidateValue = Color_rate(candidateArguments);

		if (candidateValue>optimumValue)
		{
			memcpy(optimumArguments, candidateArguments, dimensions * sizeof(int));
			optimumValue = candidateValue;
		}
	}

	void GenerateNeighbour(int position) {
		neighbourArguments[position] = newRandomColor(position, maxColorsNumber, neighbourArguments);
	}
	void RestoreNeighbour(int position, int previousColor) {
		neighbourArguments[position] = previousColor;
	}

	void Improvement(uniform_real_distribution<double> distribution, default_random_engine generator, double temperature)
	{
		memcpy(neighbourArguments, candidateArguments, dimensions * sizeof(int));

		for (unsigned short position = 0; position < dimensions; ++position)
		{
			int colorToRegenerate = neighbourArguments[position];
			GenerateNeighbour(position);
			neighbourValue = Color_rate(neighbourArguments);

			if (neighbourValue>candidateValue)
			{
				memcpy(candidateArguments, neighbourArguments, dimensions * sizeof(int));
				candidateValue = neighbourValue;
			}
			else if (distribution(generator) < exp( (neighbourValue - candidateValue) / temperature))
			{
				memcpy(candidateArguments, neighbourArguments, dimensions * sizeof(int));
				candidateValue = neighbourValue;
			}
			else
			{
				RestoreNeighbour(position,colorToRegenerate);
			}
		}
	}
public:
	void SA()
	{
		const unsigned int seed = system_clock::now().time_since_epoch().count();
		const default_random_engine generator(seed);
		const uniform_real_distribution<double> distribution(0, 1);

		constexpr double coolingRate = 0.1;


		for (int iterator = 1; iterator <= iterations; ++iterator)
		{
			GenerateCandidate();
			EvaluateCandidate();
			printf("%f\n", optimumValue);
			for (double temperature = 1000; temperature > 0.01; temperature *= 1 - coolingRate)
			{
				Improvement(distribution, generator, temperature);
			}

			EvaluateCandidate();
		}
		set<int> usedColors;
		for (int i = 0; i < nodesNumber;i++) {
			usedColors.insert(optimumArguments[i]);
		}
		printf("\nused colors: %d\nmaximum color: %d\n", usedColors.size(), maxColorsNumber);
		for (const auto& color : usedColors)
			printf("%d ", color);
		printf("\n");
	}

	Graph(const string& newFileName) {
		fileName = newFileName;
		readFile();

		dimensions = nodesNumber;

		candidateArguments = new int[dimensions];
		candidateValue = DBL_MIN;

		neighbourArguments = new int[dimensions];
		neighbourValue = DBL_MIN;

		optimumArguments = new int[dimensions];
		optimumValue = -1000;
	}

	~Graph() {
		delete[] candidateArguments;
		delete[] neighbourArguments;
		delete[] optimumArguments;
	}
};



int main()
{
	vector<string> files;
	getFiles(files);
	for (const auto& fileName : files) {
		Graph graph(fileName);
		cout << fileName << endl;
		for (int i = 0; i < runs; i++) {
			graph.SA();
		}
	}
	return 0;
}