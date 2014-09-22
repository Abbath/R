#ifndef RESULTS_HPP
#define RESULTS_HPP

#include <QtCore>

/*!
 * \brief The Results class
 */
class Results
{
public:
    Results();
    QVector<int> resultsNumbers;
    QVector<double> resultMeans;
    QVector<double> timeStamps;
    unsigned frameCount;
    unsigned fps;
};

#endif // RESULTS_HPP
