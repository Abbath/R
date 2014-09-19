#ifndef RESULTS_HPP
#define RESULTS_HPP

#include <QtCore>

class Results
{
public:
    Results();
    QVector<int> resultsNumbers;
    QVector<double> resultMeans;
    QVector<double> timeStamps;
};

#endif // RESULTS_HPP
