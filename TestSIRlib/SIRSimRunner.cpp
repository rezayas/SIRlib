#include "SIRSimRunner.h"

SIRSimRunner::SIRSimRunner(string _fileName, int _nTrajectories, double _λ, double _Ɣ,   \
               long _nPeople, unsigned int _ageMin, unsigned int _ageMax,    \
               unsigned int _ageBreak, unsigned int _tMax, unsigned int _Δt, \
               unsigned int _pLength)
{
    fileName      = _fileName;
    nTrajectories = _nTrajectories;
    λ             = _λ;
    Ɣ             = _Ɣ;
    nPeople       = _nPeople;
    ageMin        = _ageMin;
    ageMax        = _ageMax;
    ageBreak      = _ageBreak;
    tMax          = _tMax;
    Δt            = _Δt;
    pLength       = _pLength;

    if (nTrajectories < 1)
        throw out_of_range("nTrajectories < 1");
}

SIRSimRunner::~SIRSimRunner(void) {
    delete [] SIRsims;
}

using RunType = SIRSimRunner::RunType;

template<>
bool SIRSimRunner::Run<RunType::Serial>(void) {
    RNG *rng;
    bool succ = true;

    rng = new RNG(time(NULL));

    // Allocate array of SIRSimulation pointers, then instantiate SIRSimulations
    SIRsims = new SIRSimulation *[nTrajectories];
    for (int i = 0; i < nTrajectories; ++i)
        SIRsims[i] = new SIRSimulation(rng, λ, Ɣ, nPeople, ageMin, ageMax, ageBreak, tMax, Δt, pLength);

    // Run each SIRSimulation
    for (int i = 0; i < nTrajectories; ++i)
        succ &= SIRsims[i]->Run();

    delete rng;

    return succ;
}

// NOT PROPERLY IMPLEMENTED YET
template<>
bool SIRSimRunner::Run<RunType::Parallel>(void) {
    RNG *rng;
    bool succ = true;

    rng = new RNG(time(NULL));

    // Allocate array of SIRSimulation pointers, then instantiate SIRSimulations
    SIRsims = new SIRSimulation *[nTrajectories];
    for (int i = 0; i < nTrajectories; ++i)
        SIRsims[i] = new SIRSimulation(rng, λ, Ɣ, nPeople, ageMin, ageMax, ageBreak, tMax, Δt, pLength);

    // Run each SIRSimulation
    for (int i = 0; i < nTrajectories; ++i)
        succ &= SIRsims[i]->Run();

    delete rng;

    return succ;
}

std::vector<string> SIRSimRunner::Write(void) {
    bool succ = true;

    map<TimeStatType, string> columns {
        {TimeStatType::Sum,  "Total"},
        {TimeStatType::Mean, "Average"},
        {TimeStatType::Min,  "Minimum"},
        {TimeStatType::Max,  "Maximum"}
    };

    TimeSeriesCSVExport<int> TSExSusceptible(fileName + string("-susceptible.csv"));
    TimeSeriesCSVExport<int> TSExInfected   (fileName + string("-infected.csv"));
    TimeSeriesCSVExport<int> TSExRecovered  (fileName + string("-recovered.csv"));
    TimeSeriesCSVExport<int> TSExInfections (fileName + string("-infections.csv"));
    TimeSeriesCSVExport<int> TSExRecoveries (fileName + string("-recoveries.csv"));

    TimeStatisticsCSVExport TSxExSusceptible(fileName + string("-susceptible-statistics.csv"), columns);
    TimeStatisticsCSVExport TSxExInfected   (fileName + string("-infected-statistics.csv"), columns);
    TimeStatisticsCSVExport TSxExRecovered  (fileName + string("-recovered-statistics.csv"), columns);
    TimeStatisticsCSVExport TSxExInfections (fileName + string("-infections-statistics.csv"), columns);
    TimeStatisticsCSVExport TSxExRecoveries (fileName + string("-recoveries-statistics.csv"), columns);

    PyramidTimeSeriesCSVExport PTSExSusceptible(fileName + string("-susceptible-pyramid.csv"));
    PyramidTimeSeriesCSVExport PTSExInfected   (fileName + string("-infected-pyramid.csv"));
    PyramidTimeSeriesCSVExport PTSExRecovered  (fileName + string("-recovered-pyramid.csv"));
    PyramidTimeSeriesCSVExport PTSExInfections (fileName + string("-infections-pyramid.csv"));
    PyramidTimeSeriesCSVExport PTSExRecoveries (fileName + string("-recoveries-pyramid.csv"));

    std::vector<string> writes {
        fileName + string("-susceptible.csv"),
        fileName + string("-infected.csv"),
        fileName + string("-recovered.csv"),
        fileName + string("-infections.csv"),
        fileName + string("-recoveries.csv"),
        fileName + string("-susceptible-statistics.csv"),
        fileName + string("-infected-statistics.csv"),
        fileName + string("-recovered-statistics.csv"),
        fileName + string("-infections-statistics.csv"),
        fileName + string("-recoveries-statistics.csv"),
        fileName + string("-susceptible-pyramid.csv"),
        fileName + string("-infected-pyramid.csv"),
        fileName + string("-recovered-pyramid.csv"),
        fileName + string("-infections-pyramid.csv"),
        fileName + string("-recoveries-pyramid.csv")
    };

    // For each SIRSimulation, add its data to our exporters
    for (int i = 0; i < nTrajectories; ++i)
    {
        TimeSeries<int>   *Susceptible    = SIRsims[i]->GetData<TimeSeries<int>>(SIRData::Susceptible);
        TimeSeries<int>   *Infected       = SIRsims[i]->GetData<TimeSeries<int>>(SIRData::Infected);
        TimeSeries<int>   *Recovered      = SIRsims[i]->GetData<TimeSeries<int>>(SIRData::Recovered);
        TimeSeries<int>   *Infections     = SIRsims[i]->GetData<TimeSeries<int>>(SIRData::Infections);
        TimeSeries<int>   *Recoveries     = SIRsims[i]->GetData<TimeSeries<int>>(SIRData::Recoveries);

        TimeStatistic     *SusceptibleSx  = SIRsims[i]->GetData<TimeStatistic>(SIRData::Susceptible);
        TimeStatistic     *InfectedSx     = SIRsims[i]->GetData<TimeStatistic>(SIRData::Infected);
        TimeStatistic     *RecoveredSx    = SIRsims[i]->GetData<TimeStatistic>(SIRData::Recovered);
        TimeStatistic     *InfectionsSx   = SIRsims[i]->GetData<TimeStatistic>(SIRData::Infections);
        TimeStatistic     *RecoveriesSx   = SIRsims[i]->GetData<TimeStatistic>(SIRData::Recoveries);

        PyramidTimeSeries *SusceptiblePyr = SIRsims[i]->GetData<PyramidTimeSeries>(SIRData::Susceptible);
        PyramidTimeSeries *InfectedPyr    = SIRsims[i]->GetData<PyramidTimeSeries>(SIRData::Infected);
        PyramidTimeSeries *RecoveredPyr   = SIRsims[i]->GetData<PyramidTimeSeries>(SIRData::Recovered);
        PyramidTimeSeries *InfectionsPyr  = SIRsims[i]->GetData<PyramidTimeSeries>(SIRData::Infections);
        PyramidTimeSeries *RecoveriesPyr  = SIRsims[i]->GetData<PyramidTimeSeries>(SIRData::Recoveries);

        // Add
        succ &= TSExSusceptible.Add(Susceptible);
        succ &= TSExInfected.Add(Infected);
        succ &= TSExRecovered.Add(Recovered);
        succ &= TSExInfections.Add(Infections);
        succ &= TSExRecoveries.Add(Recoveries);

        succ &= TSxExSusceptible.Add(SusceptibleSx);
        succ &= TSxExInfected.Add(InfectedSx);
        succ &= TSxExRecovered.Add(RecoveredSx);
        succ &= TSxExInfections.Add(InfectionsSx);
        succ &= TSxExRecoveries.Add(RecoveriesSx);

        succ &= PTSExSusceptible.Add(SusceptiblePyr);
        succ &= PTSExInfected.Add(InfectedPyr);
        succ &= PTSExRecovered.Add(RecoveredPyr);
        succ &= PTSExInfections.Add(InfectionsPyr);
        succ &= PTSExRecoveries.Add(RecoveriesPyr);
    }

    // Write
    succ &= TSExSusceptible.Write();
    succ &= TSExInfected.Write();
    succ &= TSExRecovered.Write();
    succ &= TSExInfections.Write();
    succ &= TSExRecoveries.Write();

    succ &= TSxExSusceptible.Write();
    succ &= TSxExInfected.Write();
    succ &= TSxExRecovered.Write();
    succ &= TSxExInfections.Write();
    succ &= TSxExRecoveries.Write();

    succ &= PTSExSusceptible.Write();
    succ &= PTSExInfected.Write();
    succ &= PTSExRecovered.Write();
    succ &= PTSExInfections.Write();
    succ &= PTSExRecoveries.Write();

    printf("Finished writing\n");

    return succ ? writes : std::vector<string>{};
}