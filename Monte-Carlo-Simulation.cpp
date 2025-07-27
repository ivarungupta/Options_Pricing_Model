#include <iostream>
#include <cmath>
#include <random>
#include <vector>

using namespace std; 

// Function to generate Gaussian (normal) noise with a given mean and standard deviation
double generateGaussianNoise(double gaussianMean, double gaussianStandardDeviation) {
    static mt19937 randomNumberGenerator(random_device{}()); // Mersenne Twister random number generator
    normal_distribution<double> normalDistribution(gaussianMean, gaussianStandardDeviation); // Create normal distribution with given mean and stddev
    return normalDistribution(randomNumberGenerator); // Generate and return a random sample from the distribution
}

// Function to calculate the payoff of a call option
// Payoff for a call option is max(S - K, 0), where S is the stock price and K is the strike price
double callOptionPayoff(double simulatedStockPriceAtMaturity, double strikePrice) {
    return max(simulatedStockPriceAtMaturity - strikePrice, 0.0);
}

// Function to calculate the payoff of a put option
// Payoff for a put option is max(K - S, 0)
double putOptionPayoff(double simulatedStockPriceAtMaturity, double strikePrice) {
    return max(strikePrice - simulatedStockPriceAtMaturity, 0.0);
}

// Function to calculate the price of an option (call or put) using the Monte Carlo method
double monteCarloOptionPricing(
    double initialStockPrice,
    double strikePrice,
    double riskFreeInterestRate,
    double stockVolatility,
    double timeToMaturityInYears,
    int numberOfSimulations,
    bool isCallOptionType
) {
    double sumOfPayoffs = 0.0; // Sum of all simulated payoffs for averaging

    // Simulate `numberOfSimulations` different possible future stock prices
    for (int simulationIndex = 0; simulationIndex < numberOfSimulations; simulationIndex++) {
        // Calculate the stock price at maturity using Geometric Brownian Motion
        // Formula: S_T = S_0 * exp((r - 0.5 * sigma^2) * T + sigma * sqrt(T) * Z)
        // where Z is a standard normal random variable
        double simulatedStockPriceAtMaturity = initialStockPrice *
            exp((riskFreeInterestRate - 0.5 * stockVolatility * stockVolatility) * timeToMaturityInYears +
                stockVolatility * sqrt(timeToMaturityInYears) * generateGaussianNoise(0.0, 1.0));

        // Calculate the option payoff based on type (call or put)
        double simulatedPayoff = isCallOptionType
            ? callOptionPayoff(simulatedStockPriceAtMaturity, strikePrice)
            : putOptionPayoff(simulatedStockPriceAtMaturity, strikePrice);

        // Add the current payoff to the running total
        sumOfPayoffs += simulatedPayoff;
    }

    // Compute the average of all simulated payoffs
    double averageSimulatedPayoff = sumOfPayoffs / static_cast<double>(numberOfSimulations);

    // Discount the average payoff back to present value using risk-free interest rate
    return exp(-riskFreeInterestRate * timeToMaturityInYears) * averageSimulatedPayoff;
}

int main() {
    // Parameters for the European option to be priced
    double initialStockPrice = 100.0;          // S0: Current price of the underlying stock
    double strikePrice = 100.0;                // K: Strike (exercise) price of the option
    double riskFreeInterestRate = 0.05;        // r: Annualized risk-free rate
    double stockVolatility = 0.2;              // sigma: Standard deviation of stock returns (volatility)
    double timeToMaturityInYears = 1.0;        // T: Time to option expiry in years
    int numberOfSimulations = 100000;          // Number of random paths for the Monte Carlo simulation

    // Calculate the price of a European call option using Monte Carlo simulation
    double europeanCallOptionPrice = monteCarloOptionPricing(
        initialStockPrice,
        strikePrice,
        riskFreeInterestRate,
        stockVolatility,
        timeToMaturityInYears,
        numberOfSimulations,
        true // Indicate it's a call option
    );

    // Calculate the price of a European put option using Monte Carlo simulation
    double europeanPutOptionPrice = monteCarloOptionPricing(
        initialStockPrice,
        strikePrice,
        riskFreeInterestRate,
        stockVolatility,
        timeToMaturityInYears,
        numberOfSimulations,
        false // Indicate it's a put option
    );

    // Output the calculated prices for both call and put options
    cout << "European Call Option Price: " << europeanCallOptionPrice << endl;
    cout << "European Put Option Price: " << europeanPutOptionPrice << endl;

    return 0;
}
