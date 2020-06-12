#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <thread>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace std::chrono;

double probCrossover = 0.3;
double probMutation = 0.01;
double probHelper = 0.4;
int runs = 1;
int generations = 20;
int individuals = 100;
int tournament = 5;
int patienceTime = 50;

enum class Mutation {
	adjacent,
	random
};

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
bool existsEdge(int** adjacentMatrix, int firstNode, int secondNode) {
	if (adjacentMatrix[firstNode][secondNode] == 1)
		return true;
	return false;
}

class Cross {
	class nondestructive {
	public:
		void singlePoint(vector<vector<int>>& population, int individual1, int individual2) {
			int individualSize = population[individual1].size();
			int crossPoint = randomInt(1, individualSize - 1);
			vector<int> firstSon(individualSize);
			vector<int> secondSon(individualSize);
			for (int i = 0; i < individualSize; i++) {
				firstSon[i] = population[individual1][i];
				secondSon[i] = population[individual2][i];
			}
			for (int i = 0; i < individualSize; i++)
			{
				if (i < crossPoint) {
					firstSon[i] = population[individual2][i];
					secondSon[i] = population[individual1][i];
				}
			}
			population.push_back(firstSon);
			population.push_back(secondSon);
		}

		void multiPoint(vector<vector<int>>& population, int individual1, int individual2) {
			int individualSize = population[individual1].size();
			vector<int> crossPoints(individualSize);
			vector<int> firstSon(individualSize);
			vector<int> secondSon(individualSize);
			for (int i = 0; i < individualSize; i++) {
				crossPoints[i] = randomInt(0, 1);
				firstSon[i] = population[individual1][i];
				secondSon[i] = population[individual2][i];
			}
			for (int i = 0; i < individualSize; i++) {
				if (crossPoints[i] == 1) {
					firstSon[i] = population[individual2][i];
					secondSon[i] = population[individual1][i];
				}
			}
			population.push_back(firstSon);
			population.push_back(secondSon);
		}

		void conflictElimination(int** adjacentMatrix, vector<vector<int>>& population, int individual1, int individual2) {
			vector<int> copyIndividual1(population[individual1]);
			vector<int> copyIndividual2(population[individual2]);
			int individualSize = population[individual1].size();
			vector<int> firstSon(individualSize);
			vector<int> secondSon(individualSize);
			for (int i = 0; i < individualSize - 1; i++)
				for (int j = i + 1; j < individualSize; j++)
					if (population[individual1][i] == population[individual1][j] && existsEdge(adjacentMatrix, i, j)) {
						firstSon[i] = copyIndividual2[i];
						break;
					}
			population.push_back(firstSon);
			for (int i = 0; i < individualSize - 1; i++)
				for (int j = i + 1; j < individualSize; j++)
					if (population[individual2][i] == population[individual2][j] && existsEdge(adjacentMatrix, i, j)) {
						secondSon[i] = copyIndividual1[i];
						break;
					}
			population.push_back(secondSon);
		}

		void merging(vector<vector<int>>& population, int individual1, int individual2) {
			int individualSize = population[individual1].size();
			vector<int> mergedParent(individualSize * 2);
			vector<int> firstSon;
			vector<int> secondSon;
			int left = 0;
			int right = 0;
			int max = 0;
			for (int i = 0; i < individualSize * 2; i++) {
				if (randomInt(0, 1) == 1 && left < individualSize) {
					mergedParent[i] = firstSon[left];
					left++;
				}
				else if (right < individualSize) {
					mergedParent[i] = secondSon[right];
					right++;
				}
				if (mergedParent[i] > max)
					max = mergedParent[i];
			}
			vector<bool> apparition(max + 1);
			for (int i = 0; i < individualSize * 2; i++)
				if (apparition[mergedParent[i]] == false) {
					firstSon.push_back(mergedParent[i]);
					apparition[mergedParent[i]] = true;
				}
				else
					secondSon.push_back(mergedParent[i]);
			population.push_back(firstSon);
			population.push_back(secondSon);
		}
	};

	class destructive {
	public:
		void singlePoint(vector<vector<int>>& population, int individual1, int individual2) {
			int individualSize = population[individual1].size();
			int crossPoint = randomInt(1, individualSize - 1);
			for (int i = 0; i < individualSize; i++)
				if (i < crossPoint) {
					int aux = population[individual1][i];
					population[individual1][i] = population[individual2][i];
					population[individual2][i] = aux;
				}
		}

		void multiPoint(vector<vector<int>>& population, int individual1, int individual2) {
			int individualSize = population[individual1].size();
			vector<int> crossPoints(individualSize);
			for (int i = 0; i < individualSize; i++)
				crossPoints[i] = randomInt(0, 1);
			for (int i = 0; i < individualSize; i++)
				if (crossPoints[i] == 1) {
					int aux = population[individual1][i];
					population[individual1][i] = population[individual2][i];
					population[individual2][i] = aux;
				}
		}

		void conflictElimination(int** adjacentMatrix, vector<vector<int>>& population, int individual1, int individual2) {
			vector<int> copyIndividual1(population[individual1]);
			vector<int> copyIndividual2(population[individual2]);
			int individualSize = population[individual1].size();
			for (int i = 0; i < individualSize - 1; i++)
				for (int j = i + 1; j < individualSize; j++)
					if (population[individual1][i] == population[individual1][j] && existsEdge(adjacentMatrix, i, j)) {
						population[individual1][i] = copyIndividual2[i];
						break;
					}
			for (int i = 0; i < individualSize - 1; i++)
				for (int j = i + 1; j < individualSize; j++)
					if (population[individual2][i] == population[individual2][j] && existsEdge(adjacentMatrix, i, j)) {
						population[individual2][i] = copyIndividual1[i];
						break;
					}
		}

		void merging(vector<vector<int>>& population, int individual1, int individual2) {
			int individualSize = population[individual1].size();
			vector<int> mergedParent(individualSize * 2);
			int left = 0;
			int right = 0;
			int max = 0;
			for (int i = 0; i < individualSize * 2; i++) {
				if (randomInt(0, 1) == 1 && left < individualSize) {
					mergedParent[i] = population[individual1][left++];
					left++;
				}
				else if (right < individualSize) {
					mergedParent[i] = population[individual2][right];
					right++;
				}
				if (mergedParent[i] > max)
					max = mergedParent[i];
			}
			vector<bool> apparition(max + 1);
			left = right = 0;
			for (int i = 0; i < individualSize * 2; i++)
				if (apparition[mergedParent[i]] == false) {
					population[individual1][left++] = mergedParent[i];
					apparition[mergedParent[i]] = true;
				}
				else
					population[individual2][right++] = mergedParent[i];
		}
	};
public:
	nondestructive nonDestructive;
	destructive Destructive;
};

class Selection {
public:
	static void Tournament(vector<vector<int>> population, vector<vector<int>>& newPopulation, vector<double> fitness) {
		int populationSize = population.size();
		for (int i = 0; i < individuals; i++) {
			int* indexes = new int[tournament];
			for (int j = 0; j < tournament; j++)
				indexes[j] = randomInt(0, populationSize - 1);
			int bestIndex = indexes[0];
			for (int j = 1; j < tournament; j++)
				if (fitness[indexes[j]] > fitness[bestIndex])
					bestIndex = indexes[j];
			newPopulation.push_back(population[bestIndex]);
			delete[] indexes;
		}
	}
};

class Graph
{
	vector<pair<int, int>> edges;
	int** adjacentMatrix;
	vector<vector<int>> population;
	string fileName;
	int nodesNumber;
	int edgesNumber;
	int maxColorsNumber;
	struct info {
		int badEdges = 0;
		double fitness = -DBL_MAX;
		vector<int> individual;
		void clear() {
			fitness = -DBL_MAX;
		}
	} currentMax, bestMax;
	int* candidateColors;
	double candidateRate;
	int candidateBadEdges;
	int* neighbourColors;
	double neighbourRate;
	int* optimumColors;
	double optimumRate;
	int optimumBadEdges;

	void readFile() {
		ifstream file(fileName);
		file >> nodesNumber >> edgesNumber;
		adjacentMatrix = new int* [nodesNumber];
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

	void initialization() {
		for (int i = 0; i < individuals; i++) {
			vector<int> individual(nodesNumber);
			for (int j = 0; j < nodesNumber; j++)
				individual[j] = 1;
			population.push_back(individual);
		}
	}

	void getAdjacentColors(int node, set<int>& adjacentColors) {
		for (int j = 0; j < nodesNumber; j++)
			if (adjacentMatrix[node][j] == 1)
				adjacentColors.insert(j);
	}

	bool isColorOK(int candidate, vector<int> exceptions) {
		for (const auto& color : exceptions)
			if (candidate == color)
				return false;
		return true;
	}

	int newRandomColor(int index, int maxColor, vector<int>& individual) {
		vector<int> exception;
		exception.push_back(individual[index]);
		for (int i = 0; i < nodesNumber; i++)
			if (adjacentMatrix[index][i] == 1)
				exception.push_back(individual[i]);
		int candidate = 3;
		while (!isColorOK(candidate, exception))
			candidate = randomInt(1, maxColor);
		return candidate;
	}

	void mutate(vector<int>& individual, int usedMutation) {
		int individualSize = individual.size();
		if (usedMutation == int(Mutation::adjacent)) {
			int max = 0;
			int maxIndex = 0;
			for (int i = 0; i < individualSize; i++) {
				int badEdgesNumber = 0;
				for (int j = 0; j < individualSize; j++)
					if (individual[i] == individual[j] && existsEdge(adjacentMatrix, i, j))
						badEdgesNumber++;
				if (badEdgesNumber > max) {
					max = badEdgesNumber;
					maxIndex = i;
				}
			}
			individual[maxIndex] = newRandomColor(maxIndex, maxColorsNumber, individual);
		}
		else
			for (int i = 0; i < individualSize - 1; i++)
				for (int j = i + 1; j < individualSize; j++)
					if (individual[i] == individual[j] && existsEdge(adjacentMatrix, i, j))
						individual[i] = newRandomColor(i, maxColorsNumber, individual);
	}

	void mutation(int usedMutation) {
		int populationSize = population.size();
		for (int i = 0; i < populationSize; i++)
			if (randomDouble(0, 1) < probMutation)
				mutate(population[i], usedMutation);
	}

	int HammingDistance(int individual1, unsigned short individual2) {
		int count = 0;
		int individualSize = population[individual1].size();
		for (int i = 0; i < individualSize; i++)
			if (population[individual1][i] == population[individual2][i])
				count++;
		return count;
	}

	void crossover() {
		int populationSize = population.size();
		vector<pair<double, unsigned short>> probabilities;
		probabilities.reserve(populationSize);
		for (int i = 0; i < populationSize; i++)
			probabilities.emplace_back(make_pair(randomDouble(0, 1), i));
		sort(probabilities.begin(), probabilities.end());
		for (auto i = probabilities.begin() + 1; i < probabilities.end() && i->first < probCrossover; i += 2)
		{
			Cross crossing;
			crossing.Destructive.multiPoint(population, (i - 1)->second, i->second);
			crossing.Destructive.conflictElimination(adjacentMatrix, population, (i - 1)->second, i->second);
		}
	}

	int getColorsNumber(const vector<int>& individual) {
		set<int> usedColors;
		for (const auto& color : individual)
			usedColors.insert(color);
		return usedColors.size();
	}

	void buildFitness(vector<double>& fitness) {
		int populationSize = population.size();
		int individualSize = population[0].size() - 1;
		for (int i = 0; i < populationSize; i++) {
			int maxIndividualColor = population[i][individualSize];
			int badEdges = 0;
			for (int j = 0; j < individualSize; j++) {
				if (population[i][j] > maxIndividualColor)
					maxIndividualColor = population[i][j];
				for (int k = 0; k < individualSize; k++)
					if (population[i][j] == population[i][k] && existsEdge(adjacentMatrix, j, k))
						badEdges++;
			}
			double individualFitness = -static_cast<double>(100 * badEdges + 50 * (getColorsNumber(population[i]) - 1)) / edgesNumber;
			fitness.push_back(individualFitness);
			if (individualFitness > bestMax.fitness) {
				bestMax.badEdges = badEdges;
				bestMax.fitness = individualFitness;
				bestMax.individual = population[i];
			}
		}
	}

	void selection() {
		vector<vector<int>> newPopulation;
		vector<double> fitness;
		buildFitness(fitness);
		int populationSize = population.size();
		for (int i = 0; i < populationSize; i++)
			if (fitness[i] > currentMax.fitness) {
				currentMax.fitness = fitness[i];
				currentMax.individual = population[i];
			}
		Selection::Tournament(population, newPopulation, fitness);
		population = newPopulation;
	}

	void helper() {
		int individualSize = currentMax.individual.size();
		for (int i = 0; i < individualSize; i++)
			if (randomDouble(0, 1) < probHelper)
				currentMax.individual[i] = newRandomColor(i, maxColorsNumber, currentMax.individual);
		for (int i = 0; i < 0.4 * individuals; i++)
			population[randomInt(1, individuals - 1)] = currentMax.individual;
	}

	void GenerateCandidate() {
		for (int i = 0; i < nodesNumber; i++)
			candidateColors[i] = 1;
	}

	int getColorsNumber(int* candidate) {
		set<int> usedColors;
		for (int i = 0; i < nodesNumber; i++)
			usedColors.insert(candidate[i]);
		return usedColors.size();
	}

	double rateColor(int* candidate) {
		int maxIndividualColor = candidate[0];
		int badEdges = 0;
		for (int i = 0; i < nodesNumber; i++) {
			if (candidate[i] > maxIndividualColor)
				maxIndividualColor = candidate[i];
			for (int j = 0; j < nodesNumber; j++)
				if (candidate[i] == candidate[j] && existsEdge(adjacentMatrix, i, j))
					badEdges++;
		}
		candidateBadEdges = badEdges;
		return -(static_cast<double>(100 * badEdges) + 50 * (getColorsNumber(candidate) - 1)) / edgesNumber;
	}

    void EvaluateCandidate() {
		candidateRate = rateColor(candidateColors);
		if (candidateRate > optimumRate) {
			memcpy(optimumColors, candidateColors, nodesNumber * sizeof(int));
			optimumRate = candidateRate;
			optimumBadEdges = candidateBadEdges;
		}
	}

	int newRandomColor(int index, int maxColor, int* individual) {
		vector<int> exception;
		exception.push_back(individual[index]);
		for (int i = 0; i < nodesNumber; i++)
			if (adjacentMatrix[index][i] == 1)
				exception.push_back(individual[i]);
		int candidate = 3;
		while (!isColorOK(candidate, exception))
			candidate = randomInt(1, maxColor);
		return candidate;
	}

	void GenerateNeighbour(int position) {
		neighbourColors[position] = newRandomColor(position, maxColorsNumber, neighbourColors);
	}

	void RestoreNeighbour(int position, int previousColor) {
		neighbourColors[position] = previousColor;
	}

	void Improvement(uniform_real_distribution<double> distribution, default_random_engine generator, double temperature)
	{
		memcpy(neighbourColors, candidateColors, nodesNumber * sizeof(int));
		for (int i = 0; i < nodesNumber; i++) {
			int previousColor = neighbourColors[i];
			GenerateNeighbour(i);
			neighbourRate = rateColor(neighbourColors);
			if (neighbourRate > candidateRate) {
				memcpy(candidateColors, neighbourColors, nodesNumber * sizeof(int));
				candidateRate = neighbourRate;
			}
			else if (distribution(generator) < exp((neighbourRate - candidateRate) / temperature)){
				memcpy(candidateColors, neighbourColors, nodesNumber * sizeof(int));
				candidateRate = neighbourRate;
			}
			else
                RestoreNeighbour(i, previousColor);
		}
	}
public:
	Graph(const string& newFileName) {
		fileName = newFileName;
		readFile();
		candidateColors = new int[nodesNumber];
		candidateRate = -DBL_MAX;
		candidateBadEdges = -INT_MAX;
		neighbourColors = new int[nodesNumber];
		neighbourRate = -DBL_MAX;
		optimumColors = new int[nodesNumber];
		optimumRate = -DBL_MAX;
		optimumBadEdges = -INT_MAX;
	}

	~Graph() {
		delete[] candidateColors;
		delete[] neighbourColors;
		delete[] optimumColors;
	}

	void geneticAlgorithm() {
		generations = 200;
		string results(fileName);
		results.replace(results.end() - 4, results.end(), "");
		results += "_GA.csv";
		FILE* file = fopen(results.c_str(), "w");
		fprintf(file, "Index, ");
		for (int i = 20; i <= 200; i += 20)
			fprintf(file, "%d, ", i);
		fprintf(file, "Bad Edges\n");
		int usedMutation = int(Mutation::random);
		for (int i = 0; i < runs; i++) {
			printf("%s %d\n", fileName.c_str(), i + 1);
			fprintf(file, "%d, ", i + 1);
			int patienceCount = 0;
			initialization();
			for (int generation = 0; generation < generations; generation++) {
				cout << generation << ' ';
				mutation(usedMutation);
				crossover();
				selection();
				if (currentMax.fitness == bestMax.fitness)
					patienceCount++;
				else
					patienceCount = 0;
				if (patienceCount == patienceTime) {
					helper();
					patienceCount = 0;
				}
				currentMax.clear();
				if (generation % 20 == 0)
				{
					usedMutation = 1 - usedMutation;
					fprintf(file, "%d, ", getColorsNumber(bestMax.individual));
				}
			}
			fprintf(file, "%d\n", bestMax.badEdges);
		}
		fclose(file);
	}

	void simulatedAnnealing() {
		generations = 20;
		string results(fileName);
		results.replace(results.end() - 4, results.end(), "");
		results += "_SA.csv";
		FILE* file = fopen(results.c_str(), "w");
		fprintf(file, "Index, ");
		for (int i = 1; i <= 20; i++)
			fprintf(file, "%d, ", i);
		fprintf(file, "Bad Edges\n");
		const int seed = system_clock::now().time_since_epoch().count();
		const default_random_engine generator(seed);
		const uniform_real_distribution<double> distribution(0, 1);
		constexpr double coolingRate = 0.1;
		for (int i = 0; i < runs; i++) {
			printf("%s %d\n", fileName.c_str(), i + 1);
			fprintf(file, "%d, ", i + 1);
			for (int generation = 1; generation <= generations; generation++) {
				GenerateCandidate();
				EvaluateCandidate();
				for (double temperature = 1000; temperature > 0.01; temperature *= 1 - coolingRate)
					Improvement(distribution, generator, temperature);
				EvaluateCandidate();
                fprintf(file, "%d, ", getColorsNumber(optimumColors));
			}
			fprintf(file, "%d\n", optimumBadEdges);
		}
		fclose(file);
	}
};

void function1(vector<string> files, int time) {
	this_thread::sleep_for(seconds(time));
	Graph graph1(files[0]);
	graph1.geneticAlgorithm();
	graph1.simulatedAnnealing();
	Graph graph2(files[1]);
	graph2.geneticAlgorithm();
	graph2.simulatedAnnealing();
	Graph graph3(files[2]);
	graph3.geneticAlgorithm();
	graph3.simulatedAnnealing();
}

void function2(vector<string> files, int time) {
	this_thread::sleep_for(seconds(time));
	Graph graph1(files[3]);
	graph1.geneticAlgorithm();
	graph1.simulatedAnnealing();
	Graph graph2(files[4]);
	graph2.geneticAlgorithm();
	graph2.simulatedAnnealing();
	Graph graph3(files[5]);
	graph3.geneticAlgorithm();
	graph3.simulatedAnnealing();
}

void function3(vector<string> files, int time) {
	this_thread::sleep_for(seconds(time));
	Graph graph1(files[6]);
	graph1.geneticAlgorithm();
	graph1.simulatedAnnealing();
	Graph graph2(files[7]);
	graph2.geneticAlgorithm();
	graph2.simulatedAnnealing();
	Graph graph3(files[8]);
	graph3.geneticAlgorithm();
	graph3.simulatedAnnealing();
}

void function4(vector<string> files, int time) {
	this_thread::sleep_for(seconds(time));
	Graph graph1(files[9]);
	graph1.geneticAlgorithm();
	graph1.simulatedAnnealing();
	Graph graph2(files[10]);
	graph2.geneticAlgorithm();
	graph2.simulatedAnnealing();
	Graph graph3(files[11]);
	graph3.geneticAlgorithm();
	graph3.simulatedAnnealing();
	Graph graph4(files[12]);
	graph4.geneticAlgorithm();
	graph4.simulatedAnnealing();
}

int main() {
	srand(static_cast<unsigned int>(time(nullptr) * clock()));
	//vector<string> files;
	//getFiles(files);
	//thread thread1(function1, files, 0);
	//thread thread2(function2, files, 1);
	//thread thread3(function3, files, 2);
	//thread thread4(function4, files, 3);
	//thread1.join();
	//thread2.join();
	//thread3.join();
	//thread4.join();
	Graph graph("flat1000_76_0.txt");
	graph.geneticAlgorithm();
	return 0;
}