//basestation.h
#define _FILE_OFFSET_BITS 64
#define STRINGIZING(x) #x
#include "basic.h"
#include "list.h"

typedef struct settings_struct {
	double sampletime;		// inter-sample time
	double arrivalmean1;		// arrival mean class 1
	double arrivalmean2;		// arrival mean class 2
	double servicemeanclet1;	// clet1 stands for cloudlet class 1
	double servicemeanclet2;	// clet2 stands for cloudlet class 2
	double servicemeanc1;		// c1 stands for cloud class 1
	double servicemeanc2;		// c2 stands for cloud class 2
	double migrationdelay;		// migration mean delay
	double simulationlength;
	double starttoprint;		// time when start to print statistic (usually 0.0)
	int capacity;			// max number of jobs at the same time in service
	int threshold;
	int seed;
	int arrivalstream1;
	int arrivalstream2;
	int leavestreamclet1;
	int leavestreamclet2;
	int leavestreamc1;
	int leavestreamc2;
	int migrationdelaystream;
	int iteration;			// number of run's iterations
	int finalvaluefiles;
	int printinters;		//print throughput, interarrival and interdeparture
} settings_struct;

typedef struct folder_struct {		// folders and paths
	char *folderSIMULATIONS;
	char *folderACTUAL;
	char *pathThroughputMean;
	char *pathResponseTimeMean;
	char *pathCloudletResponseTimeMean;
	char *pathCloudResponseTimeMean;
	char *pathMigrationPercentageMean;
	char *pathClass1ResponseTimeMean;
	char *pathClass2ResponseTimeMean;
	char *pathClass1CletJobsMean;
	char *pathClass2CletJobsMean;
	char *pathClass1JobsMean;
	char *pathClass1CJobsMean;
	char *pathClass2CJobsMean;
	char *pathClass2JobsMean;
	char *pathJobsMean;
	char *pathUtilizationMean;
	char *pathJoinedUsers;
	char *pathLeavedUsers;
	char *pathArrivalFreq;
	char *pathThroughput;
	char *pathInterdeparture;
	char *pathInterarrival;
} folder_struct;

typedef struct fd_struct {		//file descriptors
	int fdThroughputMean;
	int fdResponseTimeMean;
	int fdClass1ResponseTimeMean;
	int fdClass2ResponseTimeMean;
	int fdClass1CletJobsMean;
	int fdClass2CletJobsMean;
	int fdClass1JobsMean;
	int fdClass1CJobsMean;
	int fdClass2CJobsMean;
	int fdClass2JobsMean;
	int fdJobsMean;
	int fdUtilizationMean;
	int fdJoinedUsers;
	int fdLeavedUsers;
	int fdGeneralInfo;
} fd_struct;

typedef struct fdFinalValue_struct {
	int fdThroughputMeanFinal;
	int fdResponseTimeMeanFinal;
	int fdClass1ResponseTimeMeanFinal;
	int fdClass2ResponseTimeMeanFinal;
	int fdClass1CletJobsMeanFinal;
	int fdClass2CletJobsMeanFinal;
	int fdClass1JobsMeanFinal;
	int fdClass1CJobsMeanFinal;
	int fdClass2CJobsMeanFinal;
	int fdClass2JobsMeanFinal;
	int fdJobsMeanFinal;
	int fdUtilizationMeanFinal;
	int fdJoinedUsersFinal;
	int fdLeavedUsersFinal;
	int fdMigrationProbabilityFinal;
} fdFinalValue_struct;

typedef struct fdRun_struct {
	int fdThroughputMeanTemp;
	int fdResponseTimeMeanTemp;
	int fdResponseCloudletTimeMeanTemp;
	int fdResponseCloudTimeMeanTemp;
	int fdMigrationPercentageMeanTemp;
	int fdClass1ResponseTimeMeanTemp;
	int fdClass2ResponseTimeMeanTemp;
	int fdClass1CletJobsMeanTemp;
	int fdClass2CletJobsMeanTemp;
	int fdClass1JobsMeanTemp;
	int fdClass1CJobsMeanTemp;
	int fdClass2CJobsMeanTemp;
	int fdClass2JobsMeanTemp;
	int fdJobsMeanTemp;
	int fdUtilizationMeanTemp;
	int fdJoinedUsersTemp;
	int fdLeavedUsersTemp;
	int fdArrivalFreqTemp;
	int fdThroughputTemp;
	int fdInterdepartureTemp;
	int fdInterarrivalTemp;
} fdRun_struct;

typedef struct fptr_struct {			// file pointers
	FILE* fptrThroughputMean;
	FILE* fptrResponseTimeMean;
	FILE* fptrClass1ResponseTimeMean;
	FILE* fptrClass2ResponseTimeMean;
	FILE* fptrClass1CletJobsMean;
	FILE* fptrClass2CletJobsMean;
	FILE* fptrClass1JobsMean;
	FILE* fptrClass1CJobsMean;
	FILE* fptrClass2CJobsMean;
	FILE* fptrClass2JobsMean;
	FILE* fptrJobsMean;
	FILE* fptrUtilizationMean;
	FILE* fptrJoinedUsers;
	FILE* fptrLeavedUsers;
} fptr_struct;


typedef struct stats_struct {
	double universalTime;			//E' il current time.
	double UtilizationMean;			//
	double UtilizationMeanLastUpdate;	//Tempo nel quale è stata aggiornata per l'ultima volta-
	double ThroughputMean;			//la rispettiva variabile.
	double Class1CletJobsMean;		//Con Clet si intende cloudlet.
	double Class1CletJobsMeanLastUpdate;	//
	double Class2CletJobsMean;		//
	double Class2CletJobsMeanLastUpdate;	//
	double Class1JobsMean;			//
	double Class1JobsMeanLastUpdate;	//
	double Class1CJobsMean;			//Con C si intende cloud.
	double Class1CJobsMeanLastUpdate;	//
	double Class2CJobsMean;			//
	double Class2CJobsMeanLastUpdate;	//
	double Class2JobsMean;			//
	double Class2JobsMeanLastUpdate;	//
	double JobsMean;			//
	double JobsMeanLastUpdate;		//
	double ResponseTimeMean;		//
	double CloudletResponseTimeMean;	//
	double CloudResponseTimeMean;		//
	double Class1ResponseTimeMean;		//
	double Class2ResponseTimeMean;		//
	double LastArrivalTime;			//
	double LastDepartureTime;		//
	int samples;				//
	int ArrivedJobsFromLastSample;		//
	int LeavedJobsFromLastSample;		//
	int ArrivedJobsFromBigBang;		//
	int LeavedJobsFromBigBang;		//
	int LeavedClass1JobsFromBigBang;	//
	int LeavedClass2JobsFromBigBang;	//
	int ArrivedClass2CletJobsFromBigBang;	//
	int MigratedJobsFromBigBang;		//
	int Class1CletJobs;			//Il numero di jobs della rispettiva classe nel-
	int Class2CletJobs;			//rispettivo server al tempo universalTime.
	int Class1CJobs;			//
	int Class2CJobs;			//
} stats_struct;

void checksettings(settings_struct*);

void initoutputfiles(folder_struct,fd_struct*);

void initoutputfilesFinalValue(folder_struct,fdFinalValue_struct*);

void initEventList(list*,settings_struct settingsSettings);

void samplingEvent(list*,settings_struct,stats_struct *,fdRun_struct);

void C1ArrivalEvent(list*,settings_struct,stats_struct*,fdRun_struct);

void C2ArrivalEvent(list*,settings_struct,stats_struct*,fdRun_struct);

void Clet1DepartureEvent(stats_struct*,double,settings_struct,fdRun_struct);
void Clet2DepartureEvent(stats_struct*,double,settings_struct,fdRun_struct);
void C1DepartureEvent(stats_struct*,double,settings_struct,fdRun_struct);
void C2DepartureEvent(stats_struct*,double,settings_struct,fdRun_struct);

void updateResponseTimeMean(double arrivalTime, stats_struct *);
void updateClass1ResponseTimeMean(double arrivalTime, stats_struct *);
void updateClass2ResponseTimeMean(double arrivalTime, stats_struct *);
void updateUtilization(settings_struct, stats_struct *);
void updateClass1CletJobsMean(stats_struct *statsStruct);
void updateClass2CletJobsMean(stats_struct *statsStruct);
void updateClass1JobsMean(stats_struct *statsStruct);
void updateClass1CJobsMean(stats_struct *statsStruct);
void updateClass2CJobsMean(stats_struct *statsStruct);
void updateClass2JobsMean(stats_struct *statsStruct);
void updateJobsMean(stats_struct *statsStruct);
void updateInterarrival(settings_struct settingsStruct,stats_struct *statsStruct,fdRun_struct fdRunStruct);
void updateInterdeparture(settings_struct settingsStruct,stats_struct *statsStruct,fdRun_struct fdRunStruct);

void printFinalInformations(uintmax_t,uintmax_t,struct tm,fd_struct,settings_struct);

void getElapsedTime(struct timeval,struct timeval,uintmax_t *,uintmax_t *);

void closeFiles(fd_struct);

void closeRunFiles(fdRun_struct);

void closeFinalValueFiles(fdFinalValue_struct);

void createFolders(struct tm lt, folder_struct*);

void initRunOutputFiles(int,folder_struct,fdRun_struct*);

void mergeRunFiles(settings_struct,folder_struct,fd_struct);

void openOnlyReadFiles(int,folder_struct,fptr_struct*);

void samplingFinalValue(settings_struct,stats_struct,fdFinalValue_struct);

void closeOnlyReadFiles(fptr_struct);

void wellfordOnePass(double*,double,int);
void wellfordSamplePath(double*,double,double,double);

double getWellfordOnePass(double,double,int);
double getWellfordSamplePath(double,double,double,double);

void readAndWrite(int,uintmax_t *,
		int8_t *,FILE *,
		int ,off_t *,
		char **,size_t *,uintmax_t *);


void addNewLine(uintmax_t, int, int);
void addC1departure(list*, stats_struct, settings_struct);
void addClet1departure(list *, stats_struct, settings_struct);
void addArrival1(list*, stats_struct, settings_struct);
void addC2departure(list *, stats_struct, settings_struct);
void addClet2departure(list *, stats_struct, settings_struct);
void addArrival2(list *, stats_struct, settings_struct);

void migrateClass2FromCletToC(list *,settings_struct, stats_struct);