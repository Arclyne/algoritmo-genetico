#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <iomanip>
#include <cmath>
#include <algorithm>

constexpr double LOWER_BOUND = -10.0;
constexpr double UPPER_BOUND = 10.0;
constexpr double PRECISION = 0.000001;
constexpr int NUMBER_OF_VARIABLES = 2;

const long long INTERVALS_PER_VARIABLE = std::llround((UPPER_BOUND - LOWER_BOUND) / PRECISION);
const long long VALUES_PER_VARIABLE = INTERVALS_PER_VARIABLE + 1;
const int BITS_PER_VARIABLE = static_cast<int>(std::ceil(std::log2(static_cast<double>(VALUES_PER_VARIABLE))));
const int CHROMOSOME_LENGTH = NUMBER_OF_VARIABLES * BITS_PER_VARIABLE;
const double MAX_ENCODED_VALUE = std::pow(2.0, BITS_PER_VARIABLE) - 1.0;

constexpr int POPULATION_SIZE = 100;
constexpr int MAX_GENERATIONS = 200;
constexpr double CROSSOVER_PROBABILITY = 0.9;
const double MUTATION_PROBABILITY = 1.0 / CHROMOSOME_LENGTH;
constexpr int TOURNAMENT_SIZE = 3;
constexpr int ELITE_COUNT = 1;
constexpr int NUMBER_OF_RUNS = 30;
constexpr unsigned int BASE_SEED = 1000;

struct Individual {
    std::vector<int> bits;
    double x1;
    double x2;
    double fitness;
};

struct RunResult {
    unsigned int seed;
    double bestX1;
    double bestX2;
    double bestFx;
    long evaluations;
};

double decode(const std::vector<int>& bits, int start, int length) {
    long long encodedValue = 0;
    for (int position = start; position < start + length; ++position) {
        encodedValue = encodedValue * 2 + bits[position];
    }

    double fractionOfRange = encodedValue / MAX_ENCODED_VALUE;
    return LOWER_BOUND + fractionOfRange * (UPPER_BOUND - LOWER_BOUND);
}

double objectiveFunction(double x1, double x2) {
    return x1 * x1 + x2 * x2;
}

void evaluate(Individual& individual, long& evaluationCounter) {
    individual.x1 = decode(individual.bits, 0, BITS_PER_VARIABLE);
    individual.x2 = decode(individual.bits, BITS_PER_VARIABLE, BITS_PER_VARIABLE);
    individual.fitness = objectiveFunction(individual.x1, individual.x2);
    ++evaluationCounter;
}

Individual createRandomIndividual(std::mt19937& generator) {
    std::uniform_int_distribution<int> bitDistribution(0, 1);
    Individual individual;
    individual.bits.resize(CHROMOSOME_LENGTH);
    for (int& bit : individual.bits) {
        bit = bitDistribution(generator);
    }
    return individual;
}

Individual tournamentSelection(const std::vector<Individual>& population, std::mt19937& generator) {
    std::uniform_int_distribution<int> indexDistribution(0, static_cast<int>(population.size()) - 1);
    int winnerIndex = indexDistribution(generator);
    for (int round = 1; round < TOURNAMENT_SIZE; ++round) {
        int challengerIndex = indexDistribution(generator);
        if (population[challengerIndex].fitness < population[winnerIndex].fitness) {
            winnerIndex = challengerIndex;
        }
    }
    return population[winnerIndex];
}

void crossover(const Individual& parent1, const Individual& parent2, Individual& child1, Individual& child2, std::mt19937& generator) {
    std::uniform_real_distribution<double> probabilityDistribution(0.0, 1.0);
    child1 = parent1;
    child2 = parent2;

    if (probabilityDistribution(generator) >= CROSSOVER_PROBABILITY) {
        return;
    }

    std::uniform_int_distribution<int> cutPointDistribution(1, CHROMOSOME_LENGTH - 1);
    int cutPoint = cutPointDistribution(generator);
    for (int position = cutPoint; position < CHROMOSOME_LENGTH; ++position) {
        child1.bits[position] = parent2.bits[position];
        child2.bits[position] = parent1.bits[position];
    }
}

void mutate(Individual& individual, std::mt19937& generator) {
    std::uniform_real_distribution<double> probabilityDistribution(0.0, 1.0);
    for (int& bit : individual.bits) {
        if (probabilityDistribution(generator) < MUTATION_PROBABILITY) {
            bit = 1 - bit;
        }
    }
}

bool hasLowerFitness(const Individual& first, const Individual& second) {
    return first.fitness < second.fitness;
}

RunResult runGeneticAlgorithm(unsigned int seed) {
    std::mt19937 generator(seed);
    long evaluationCounter = 0;

    std::vector<Individual> population;
    population.reserve(POPULATION_SIZE);
    for (int index = 0; index < POPULATION_SIZE; ++index) {
        Individual individual = createRandomIndividual(generator);
        evaluate(individual, evaluationCounter);
        population.push_back(individual);
    }

    Individual bestEver = *std::min_element(population.begin(), population.end(), hasLowerFitness);

    for (int generation = 0; generation < MAX_GENERATIONS; ++generation) {
        std::sort(population.begin(), population.end(), hasLowerFitness);
        std::vector<Individual> nextPopulation(population.begin(), population.begin() + ELITE_COUNT);

        while (static_cast<int>(nextPopulation.size()) < POPULATION_SIZE) {
            Individual parent1 = tournamentSelection(population, generator);
            Individual parent2 = tournamentSelection(population, generator);
            Individual child1;
            Individual child2;
            crossover(parent1, parent2, child1, child2, generator);

            mutate(child1, generator);
            evaluate(child1, evaluationCounter);
            nextPopulation.push_back(child1);

            if (static_cast<int>(nextPopulation.size()) < POPULATION_SIZE) {
                mutate(child2, generator);
                evaluate(child2, evaluationCounter);
                nextPopulation.push_back(child2);
            }
        }

        population = nextPopulation;

        const Individual& bestOfGeneration = *std::min_element(population.begin(), population.end(), hasLowerFitness);
        if (bestOfGeneration.fitness < bestEver.fitness) {
            bestEver = bestOfGeneration;
        }
    }

    return {seed, bestEver.x1, bestEver.x2, bestEver.fitness, evaluationCounter};
}

int main() {
    std::cout << std::fixed << std::setprecision(6);

    std::cout << "Bits per variable: " << BITS_PER_VARIABLE << '\n';
    std::cout << "Chromosome length: " << CHROMOSOME_LENGTH << '\n';
    std::cout << "Population: " << POPULATION_SIZE << ", Generations: " << MAX_GENERATIONS
              << ", Crossover: " << CROSSOVER_PROBABILITY << ", Mutation: " << MUTATION_PROBABILITY
              << ", Tournament: " << TOURNAMENT_SIZE << ", Elite: " << ELITE_COUNT << "\n\n";

    std::ofstream csvFile("results.csv");
    csvFile << std::fixed << std::setprecision(6);
    csvFile << "Run,Seed,x1,x2,f(x),Evaluations\n";

    std::cout << std::setw(5) << "Run" << std::setw(8) << "Seed" << std::setw(14) << "x1"
              << std::setw(14) << "x2" << std::setw(16) << "f(x)" << std::setw(14) << "Evaluations" << '\n';

    for (int run = 1; run <= NUMBER_OF_RUNS; ++run) {
        RunResult result = runGeneticAlgorithm(BASE_SEED + run);

        std::cout << std::setw(5) << run << std::setw(8) << result.seed << std::setw(14) << result.bestX1
                  << std::setw(14) << result.bestX2 << std::setw(16) << std::scientific << result.bestFx << std::fixed
                  << std::setw(14) << result.evaluations << '\n';

        csvFile << run << ',' << result.seed << ',' << result.bestX1 << ',' << result.bestX2 << ','
                << std::scientific << result.bestFx << std::fixed << ',' << result.evaluations << '\n';
    }

    std::cout << "\nResults saved to results.csv\n";
    return 0;
}