#include <iostream>
#include <cmath>

using namespace std;

// Structure to represent an option contract and its Greeks and related metrics
struct Contract
{
    double premium;               // Price of the option
    int dte;                      // Days till expiration
    double delta;                 // Sensitivity to stock price
    double gamma;                 // Sensitivity of delta to stock price
    double theta;                 // Sensitivity to time decay
    double vega;                  // Sensitivity to volatility
    double rho;                   // Sensitivity to interest rate
    double implied_volatility;   // Approximation of implied volatility
    double intrinsic_value;      // Difference between spot and strike
};

// Approximation of the error function used in normal distribution
double erf(double value){
    const double A1 = 0.254829592;
    const double A2 = -0.284496736;
    const double A3 = 1.421413741;
    const double A4 = -1.453152027;
    const double A5 = 1.061405429;
    const double P = 0.3275911;

    int sign = (value >= 0) ? 1 : -1;
    value = fabs(value);

    double t = 1.0 / (1.0 + P * value);
    double y = 1.0 - (((((A5 * t + A4) * t + A3) * t + A2) * t + A1) * t * exp(-value * value));
    return sign * y;
}

// Cumulative distribution function for standard normal distribution
double cumulativeStandardNormal(double value){
    return 0.5 * (1.0 + erf(value / sqrt(2.0)));
}

// Black-Scholes option pricing and Greeks computation
Contract blackScholesOptionPricing(double stockPrice, double strikePrice, double interestRate,
                                   double volatility, double timeToMaturity, bool isCallOption) {
    
    Contract option; // Output object containing results

    int daysToExpiry = timeToMaturity * 365.2425;  // Approximate calendar days in a year
    option.dte = daysToExpiry;

    // Common terms
    double d1 = (log(stockPrice / strikePrice) + 
                (interestRate + (volatility * volatility) / 2.0) * timeToMaturity) / 
                (volatility * sqrt(timeToMaturity));
    double d2 = d1 - (volatility * sqrt(timeToMaturity));

    // Call Option Calculations
    if (isCallOption)
    {
        option.premium = stockPrice * cumulativeStandardNormal(d1) -
                         strikePrice * exp(-interestRate * timeToMaturity) * cumulativeStandardNormal(d2);

        option.delta = cumulativeStandardNormal(d1);  // Rate of change of option price w.r.t stock price

        option.gamma = cumulativeStandardNormal(d1) / (stockPrice * volatility * sqrt(timeToMaturity));

        option.theta = (-(stockPrice * cumulativeStandardNormal(d1) * volatility) / (2 * sqrt(timeToMaturity)))
                        - (interestRate * strikePrice * exp(-interestRate * timeToMaturity) * cumulativeStandardNormal(d2));

        option.vega = stockPrice * cumulativeStandardNormal(d1) * sqrt(timeToMaturity);

        option.rho = strikePrice * timeToMaturity * exp(-interestRate * timeToMaturity) * cumulativeStandardNormal(d2);

        // Approximate IV adjustment for educational/demo purposes
        option.implied_volatility = volatility - ((option.premium - (option.premium - 0.01)) / option.vega);

        option.intrinsic_value = max(stockPrice - strikePrice, 0.0);  // Value if exercised immediately
    }
    // Put Option Calculations
    else
    {
        option.premium = strikePrice * exp(-interestRate * timeToMaturity) * cumulativeStandardNormal(-d2) -
                         stockPrice * cumulativeStandardNormal(-d1);

        option.delta = cumulativeStandardNormal(d1) - 1;

        option.gamma = cumulativeStandardNormal(d1) / (stockPrice * volatility * sqrt(timeToMaturity));

        option.theta = (-(stockPrice * cumulativeStandardNormal(d1) * volatility) / (2 * sqrt(timeToMaturity))) +
                        (interestRate * strikePrice * exp(-interestRate * timeToMaturity) * cumulativeStandardNormal(-d2));

        option.vega = stockPrice * cumulativeStandardNormal(d1) * sqrt(timeToMaturity);

        option.rho = -strikePrice * timeToMaturity * exp(-interestRate * timeToMaturity) * cumulativeStandardNormal(-d2);

        option.implied_volatility = volatility - (((option.premium - 0.01) - option.premium) / option.vega);

        option.intrinsic_value = max(strikePrice - stockPrice, 0.0);
    }

    return option;
}

int main() {
    // Sample input parameters
    double S0 = 100.0;      // Current stock price
    double K = 100.0;       // Strike price
    double r = 0.05;        // Risk-free interest rate
    double sigma = 0.2;     // Volatility (standard deviation of returns)
    double T = 1;           // Time to maturity in years

    // Calculate call and put option contracts
    Contract callOption = blackScholesOptionPricing(S0, K, r, sigma, T, true);
    Contract putOption  = blackScholesOptionPricing(S0, K, r, sigma, T, false);

    // Display results for call option
    cout << "European Call Option Price: " << callOption.premium 
         << ", dte: " << callOption.dte 
         << ", delta: " << callOption.delta 
         << ", gamma: " << callOption.gamma 
         << ", theta: " << callOption.theta 
         << ", vega: " << callOption.vega  
         << ", rho: " << callOption.rho 
         << ", implied volatility: " << callOption.implied_volatility  
         << ", intrinsic value: " << callOption.intrinsic_value << endl;

    // Display results for put option
    cout << "European Put Option Price: " << putOption.premium 
         << ", dte: " << putOption.dte 
         << ", delta: " << putOption.delta 
         << ", gamma: " << putOption.gamma 
         << ", theta: " << putOption.theta 
         << ", vega: " << putOption.vega 
         << ", rho: " << putOption.rho 
         << ", implied volatility: " << putOption.implied_volatility  
         << ", intrinsic value: " << putOption.intrinsic_value << endl;

    return 0;
}
