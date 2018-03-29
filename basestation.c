#include "basestation.h"

int main(){

/*#####################################################################
##############################CHECK SETTINGS###########################
#####################################################################*/
	
	settings_struct settingsStruct = { .capacity=0,.threshold=0,.seed=0,
				.arrivalstream1=0,.arrivalstream2=0,
				.leavestreamclet1=0,.leavestreamclet2=0,
				.leavestreamc1=0,.leavestreamc2=0,
				.migrationdelaystream=0,.iteration=0,
				.finalvaluefiles=0,.printinters=0};
 
	//errors checks are in the function.
	checksettings(&settingsStruct);

/*#####################################################################
##############################STAMP SETTINGS###########################
#####################################################################*/

	printf("Starting simulation.\n You may want to change some setting.You'll find default (modifiable) settings in 'basic.h'.\n  Current settings are:\n");
	printf("sampletime:%f (sampling interval)\n",settingsStruct.sampletime);
	printf("arrivalmean1:%f (interarrival1 mean)\n",settingsStruct.arrivalmean1);
	printf("arrivalmean2:%f (interarrival2 mean)\n",settingsStruct.arrivalmean2);
	printf("servicemeanclet1:%f \n",settingsStruct.servicemeanclet1);
	printf("servicemeanclet2:%f \n",settingsStruct.servicemeanclet2);
	printf("servicemeanc1:%f \n",settingsStruct.servicemeanc1);
	printf("servicemeanc2:%f \n",settingsStruct.servicemeanc2);
	printf("migrationdelay:%f \n",settingsStruct.migrationdelay);
	printf("simulationlength:%f \n",settingsStruct.simulationlength);
	printf("capacity:%d \n",settingsStruct.capacity);
	printf("threshold:%d \n",settingsStruct.threshold);
	printf("Seed:%d \n",settingsStruct.seed);
	printf("joinstream1:%d \n",settingsStruct.arrivalstream1);
	printf("joinstream2:%d \n",settingsStruct.arrivalstream2);
	printf("leavestreamclet1:%d \n",settingsStruct.leavestreamclet1);
	printf("leavestreamclet2:%d \n",settingsStruct.leavestreamclet2);
	printf("leavestreamc1:%d \n",settingsStruct.leavestreamc1);
	printf("leavestreamc2:%d \n",settingsStruct.leavestreamc2);
	printf("migration delay stream:%d \n",settingsStruct.migrationdelaystream);
	printf("start to print after:%f seconds\n",settingsStruct.starttoprint);
	printf("final value files:%d (1/0 activated/deactivated), it only prints the last value of each statistic \n",settingsStruct.finalvaluefiles);
	printf("printinters:%d (1/0 activated/deactivated), it prints the instantaneous throughput, interdepartures and the interarrivals \n",settingsStruct.printinters);

/*####################################################################
##############################GET TIME################################
####################################################################*/


	time_t t = time(NULL);
	struct tm lt = *localtime(&t);

/*####################################################################
#######################CREATE SIMULATIONs FOLDER######################
####################################################################*/


	folder_struct folderStruct;

	createFolders(lt,&folderStruct);

/*####################################################################
#######################INIT GLOBAL OUTPUT FILES#######################
####################################################################*/

	fd_struct fdStruct;

	//errors checks are in the function.
	initoutputfiles(folderStruct,&fdStruct);


/*#####################################################################
#######################INIT FINAL MEAN VALUE FILES#####################
#####################################################################*/

#if FINALVALUEFILES

	fdFinalValue_struct fdFinalValueStruct;

	//errors checks are in the function.
	initoutputfilesFinalValue(folderStruct,&fdFinalValueStruct);

#endif



/*#######################################################################
##############################INIT EVENT LIST############################
#######################################################################*/

	PlantSeeds(settingsStruct.seed);
	//Getting time
	struct timeval timenow;
	if(gettimeofday(&timenow,NULL)!=0){
		perror("Error: gettimeofday before starting");
		exit(EXIT_FAILURE);
	}

	printf("Starting %d-%d-%d %d:%d:%d\n", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);

	int z;
	for(z=0;z<settingsStruct.iteration;z++){ // The run's cycles are starting here.
		list mylist;
		list_init(&mylist);
		initEventList(&mylist,settingsStruct);

/*#########################################################################
######################INIT OUTPUT FILES FOR EACH RUN#######################
#########################################################################*/

	fdRun_struct fdRunStruct;

	//errors checks are in the function.
	initRunOutputFiles(z,folderStruct,&fdRunStruct);


/*#########################################################################
##############################START SIMULATION#############################
##########################################################################*/


		stats_struct statsStruct = {
			.universalTime=0.0,
			//.RealBusyBand=0.0,
			.UtilizationMean=0.0,
			.UtilizationMeanLastUpdate=0.0,
			.ThroughputMean=0.0,
			.ResponseTimeMean=0.0,
			.Class1ResponseTimeMean=0.0,
			.Class2ResponseTimeMean=0.0,
			.Class1CletJobsMean=0.0,
			.Class1CletJobsMeanLastUpdate=0.0,
			.Class2CletJobsMean=0.0,
			.Class2CletJobsMeanLastUpdate=0.0,
			.Class1JobsMean=0.0,
			.Class1JobsMeanLastUpdate=0.0,
			.Class1CJobsMean=0.0,
			.Class1CJobsMeanLastUpdate=0.0,
			.Class2CJobsMean=0.0,
			.Class2CJobsMeanLastUpdate=0.0,
			.Class2JobsMean=0.0,
			.Class2JobsMeanLastUpdate=0.0,
			.JobsMean=0.0,
			.JobsMeanLastUpdate=0.0,
			.samples=1,

			.LastArrivalTime=0,
			.LastDepartureTime=0,
			.ArrivedJobsFromLastSample=0,
			.LeavedJobsFromLastSample=0,
			.ArrivedJobsFromBigBang=0,
			.LeavedJobsFromBigBang=0,
			.LeavedClass1JobsFromBigBang=0,
			.LeavedClass2JobsFromBigBang=0,
			.MigratedJobsFromBigBang=0,
			.ArrivedClass2CletJobsFromBigBang=0,

			.Class1CletJobs=0,
			.Class2CletJobs=0,
			.Class1CJobs=0,
			.Class2CJobs=0
		};

		double **JoinTimePtr=(double**)malloc(sizeof(double*));
		double **JoinListInsertionTimePtr=(double**)malloc(sizeof(double*));
		double **JoinListMigrationTimePtr=(double**)malloc(sizeof(double*));
		char **JoinTypePtr=(char**)malloc(sizeof(char*));

		if(JoinTimePtr==NULL || JoinTypePtr==NULL || JoinListInsertionTimePtr == NULL){
			fprintf(stderr,"Error:malloc JoinTimePtr or JoinTypePtr or JoinListInsertionTimePtr returned null.");
		}
		memset(JoinTimePtr,0,sizeof(double*));
		memset(JoinTypePtr,0,sizeof(char*));
		memset(JoinListInsertionTimePtr,0,sizeof(double*));
		memset(JoinListMigrationTimePtr,0,sizeof(double*));

		while(statsStruct.universalTime<=settingsStruct.simulationlength){
			if(list_empty(&mylist)==(unsigned char)1){
				fprintf(stderr,"Warning:Simulation ended at universalTime %f, that's before the decided simulationlength %f, because the event list is empty\n",statsStruct.universalTime,settingsStruct.simulationlength);
				break;
			}
			list_pop_min(&mylist,JoinTimePtr,JoinTypePtr,JoinListInsertionTimePtr,JoinListMigrationTimePtr); // getting the forthcoming event: JoinTime is the time when the event must to be "popped", JoinType is the event's type, 
												// JoinListInsertionTime is the time when the job starts to be served (so JoinTime and JoinListInsertionTime
												// are the same thing for an arrival event, but they are different for a departure event)
			if(*JoinTimePtr == NULL || *JoinTypePtr == NULL){
				fprintf(stderr,"Error: NULL pointer returned from list_pop_min\n");
				exit(EXIT_FAILURE);
			}
			statsStruct.universalTime=(**JoinTimePtr); // Asynchronous clock update (synced with the JoinTime of the popped event)
			if(statsStruct.universalTime>settingsStruct.simulationlength)
				break;
			switch (**JoinTypePtr) {
				case 's':	
					samplingEvent(&mylist,settingsStruct,&statsStruct,fdRunStruct);
					break;
				case 'A':
					C1ArrivalEvent(&mylist,settingsStruct,&statsStruct,fdRunStruct);
					break;
				case 'a':
					C2ArrivalEvent(&mylist,settingsStruct,&statsStruct,fdRunStruct);
					break;
				case 'D': // Departure job class 1 from cloudlet
					if(*JoinListInsertionTimePtr == NULL){
						fprintf(stderr,"Error: NULL pointer returned from list_pop_min\n");
						exit(EXIT_FAILURE);
					}
					Clet1DepartureEvent(&statsStruct,**JoinListInsertionTimePtr,settingsStruct,fdRunStruct);
					break;
				case 'd': // Departure job class 2 from cloudlet
					if(*JoinListInsertionTimePtr == NULL){
						fprintf(stderr,"Error: NULL pointer returned from list_pop_min\n");
						exit(EXIT_FAILURE);
					}
					Clet2DepartureEvent(&statsStruct,**JoinListInsertionTimePtr,settingsStruct,fdRunStruct);
					break;
				case 'L': //Departure job class 1 from cloud
					if(*JoinListInsertionTimePtr == NULL){
						fprintf(stderr,"Error: NULL pointer returned from list_pop_min\n");
						exit(EXIT_FAILURE);
					}
					C1DepartureEvent(&statsStruct,**JoinListInsertionTimePtr,settingsStruct,fdRunStruct);
					break;
				case 'l': // Departure job class 2 from cloud
					if(*JoinListInsertionTimePtr == NULL){
						fprintf(stderr,"Error: NULL pointer returned from list_pop_min\n");
						exit(EXIT_FAILURE);
					}
					C2DepartureEvent(&statsStruct,**JoinListInsertionTimePtr,settingsStruct,fdRunStruct);
					break;
				default:
					fprintf(stderr,"Error:Event type unkwown");
					exit(EXIT_FAILURE);
			}
			free(*JoinTimePtr);
			free(*JoinTypePtr);
			free(*JoinListInsertionTimePtr);
			if(*JoinListMigrationTimePtr!=NULL)
				free(*JoinListMigrationTimePtr);

		}
		free(JoinTimePtr);
		free(JoinTypePtr);
		free(JoinListInsertionTimePtr);

#if FINALVALUEFILES

		samplingFinalValue(settingsStruct,statsStruct,fdFinalValueStruct); // Just printing the last value of each statistic for each run in an unique file for each statistic

#endif

/*##################################################################
##############################CLOSE RUN FILES#######################
##################################################################*/

	closeRunFiles(fdRunStruct);

/*################################################################*/
	}


/*##############################MERGING RUNs FILES################
################################################################*/

	mergeRunFiles(settingsStruct,folderStruct,fdStruct); // Merging the runs files in one big csv file for each statistic

/*###############################################################*/

	struct timeval timenowafter;
	if(gettimeofday(&timenowafter,NULL)!=0){
		perror("Error: gettimeofday in the end");
		exit(EXIT_FAILURE);
	}
	uintmax_t elapsedSeconds;
	uintmax_t elapsedUSeconds;
	getElapsedTime(timenowafter,timenow,&elapsedSeconds,&elapsedUSeconds);

	printFinalInformations(elapsedSeconds,elapsedUSeconds,lt,fdStruct,settingsStruct);


/*#####################################################################
##############################CLOSE FILES##############################
#####################################################################*/

	closeFiles(fdStruct);

/*#####################################################################
##############################CLOSE FINAL VALUE FILES##################
#####################################################################*/

#if FINALVALUEFILES

	closeFinalValueFiles(fdFinalValueStruct);

#endif


/*####################################################################*/
	
	free(folderStruct.folderSIMULATIONS);
	free(folderStruct.folderACTUAL);
	free(folderStruct.pathThroughputMean);
	free(folderStruct.pathResponseTimeMean);
	free(folderStruct.pathClass1ResponseTimeMean);
	free(folderStruct.pathClass2ResponseTimeMean);
	free(folderStruct.pathClass1CletJobsMean);
	free(folderStruct.pathClass2CletJobsMean);
	free(folderStruct.pathClass1JobsMean);
	free(folderStruct.pathClass1CJobsMean);
	free(folderStruct.pathClass2CJobsMean);
	free(folderStruct.pathClass2JobsMean);
	free(folderStruct.pathJobsMean);
	free(folderStruct.pathUtilizationMean);
	free(folderStruct.pathJoinedUsers);
	free(folderStruct.pathLeavedUsers);
	
	printf("Simulation terminated with success. Done in %ju s and %ju ms.\n",elapsedSeconds,elapsedUSeconds);
	return EXIT_SUCCESS;
}




void checksettings(settings_struct *settingsStruct){

	#ifdef SAMPLETIME
		settingsStruct->sampletime = SAMPLETIME;
	#else

		printf("Insert sample time\n");
		if(scanf("%lf",&(settingsStruct->sampletime))!=1 || settingsStruct->sampletime<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef ARRIVALMEAN1
		settingsStruct->arrivalmean1 = ARRIVALMEAN1;
	#else
		printf("Insert arrival mean 1\n");
		if(scanf("%lf",&(settingsStruct->arrivalmean1))!=1 || settingsStruct->arrivalmean1<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef ARRIVALMEAN2
		settingsStruct->arrivalmean2 = ARRIVALMEAN2;
	#else
		printf("Insert arrival mean 2\n");
		if(scanf("%lf",&(settingsStruct->arrivalmean2))!=1 || settingsStruct->arrivalmean2<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef SERVICEMEANCLET1
		settingsStruct->servicemeanclet1 = SERVICEMEANCLET1;
	#else
		printf("Insert service mean clet 1\n");
		if(scanf("%lf",&(settingsStruct->servicemeanclet1))!=1 || settingsStruct->servicemeanclet1<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef SERVICEMEANCLET2
		settingsStruct->servicemeanclet2 = SERVICEMEANCLET2;
	#else
		printf("Insert service mean clet 2\n");
		if(scanf("%lf",&(settingsStruct->servicemeanclet2))!=1 || settingsStruct->servicemeanclet2<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef SERVICEMEANC1
		settingsStruct->servicemeanc1 = SERVICEMEANC1;
	#else
		printf("Insert service mean cloud 1\n");
		if(scanf("%lf",&(settingsStruct->servicemeanc1))!=1 || settingsStruct->servicemeanc1<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef SERVICEMEANC2
		settingsStruct->servicemeanc2 = SERVICEMEANC2;
	#else
		printf("Insert service mean cloud 2\n");
		if(scanf("%lf",&(settingsStruct->servicemeanc2))!=1 || settingsStruct->servicemeanc2<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef MIGRATIONDELAY
		settingsStruct->migrationdelay = MIGRATIONDELAY;
	#else
		printf("Insert migrationdelay\n");
		if(scanf("%lf",&(settingsStruct->migrationdelay))!=1 || settingsStruct->migrationdelay<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef SIMULATIONLENGTH
		settingsStruct->simulationlength = SIMULATIONLENGTH;
	#else
		printf("Insert simulation length\n");
		if(scanf("%lf",&(settingsStruct->simulationlength))!=1 || settingsStruct->simulationlength<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef CAPACITY
		settingsStruct->capacity = CAPACITY;
	#else
		printf("Capacity\n");
		if(scanf("%d",&(settingsStruct->capacity))!=1 || settingsStruct->capacity<=0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef THRESHOLD
		settingsStruct->threshold = THRESHOLD;
	#else
		printf("threshold\n");
		if(scanf("%d",&(settingsStruct->threshold))!=1 || settingsStruct->threshold<=0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef SEED
		settingsStruct->seed = SEED;
	#else
		printf("Insert seed\n");
		if(scanf("%d",&(settingsStruct->seed))!=1 || settingsStruct->seed<0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef ARRIVALSTREAM1
		settingsStruct->arrivalstream1 = ARRIVALSTREAM1;
	#else
		printf("Insert arrival stream 1\n");
		if(scanf("%d",&(settingsStruct->arrivalstream1))!=1 || settingsStruct->arrivalstream1<0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef ARRIVALSTREAM2
		settingsStruct->arrivalstream2 = ARRIVALSTREAM2;
	#else
		printf("Insert arrival stream 2\n");
		if(scanf("%d",&(settingsStruct->arrivalstream2))!=1 || settingsStruct->arrivalstream2<0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef LEAVESTREAMCLET1
		settingsStruct->leavestreamclet1 = LEAVESTREAMCLET1;
	#else
		printf("Insert leavestream clet1\n");
		if(scanf("%d",&(settingsStruct->leavestreamclet1))!=1 || settingsStruct->leavestreamclet1<0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef LEAVESTREAMCLET2
		settingsStruct->leavestreamclet2 = LEAVESTREAMCLET2;
	#else
		printf("Insert leavestream clet2\n");
		if(scanf("%d",&(settingsStruct->leavestreamclet2))!=1 || settingsStruct->leavestreamclet2<0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif	
	#ifdef LEAVESTREAMC1
		settingsStruct->leavestreamc1 = LEAVESTREAMC1;
	#else
		printf("Insert leavestream c1\n");
		if(scanf("%d",&(settingsStruct->leavestreamc1))!=1 || settingsStruct->leavestreamc1<0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif	
	#ifdef LEAVESTREAMC2
		settingsStruct->leavestreamc2 = LEAVESTREAMC2;
	#else
		printf("Insert leavestream c2\n");
		if(scanf("%d",&(settingsStruct->leavestreamc2))!=1 || settingsStruct->leavestreamc2<0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef MIGRATIONDELAYSTREAM
		settingsStruct->migrationdelaystream = MIGRATIONDELAYSTREAM;
	#else
		printf("Insert migrationdelaystream \n");
		if(scanf("%d",&(settingsStruct->migrationdelaystream))!=1 || settingsStruct->migrationdelaystream<0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef ITERATION
		settingsStruct->iteration = ITERATION;
	#else
		printf("Insert iteration\n");
		if(scanf("%d",&(settingsStruct->iteration))!=1 || settingsStruct->iteration<0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef STARTTOPRINT
		settingsStruct->starttoprint = STARTTOPRINT;
	#else
		printf("Insert start to print\n");
		if(scanf("%lf",&(settingsStruct->starttoprint))!=1 || settingsStruct->starttoprint<=0.0)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
	#ifdef FINALVALUEFILES
		settingsStruct->finalvaluefiles = FINALVALUEFILES;
	#else
		printf("to use finalvaluefiles -> 1, to not use finalvaluefiles -> 0\n");
		if(scanf("%d",&(settingsStruct->finalvaluefiles))!=1 || settingsStruct->finalvaluefiles<0 || settingsStruct->finalvaluefiles>1)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif	
	#ifdef PRINTINTERs
		settingsStruct->printinters = PRINTINTERs;
	#else
		printf("to use printinters -> 1, to not use printinters -> 0\n");
		if(scanf("%d",&(settingsStruct->printinters))!=1 || settingsStruct->printinters<0 || settingsStruct->printinters>1)
			exit(EXIT_FAILURE);
		dump_line(stdin);
	#endif
}

void initoutputfiles(folder_struct folderStruct, fd_struct* fdStruct){

	char* pfilename = "/throughputmean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdThroughputMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdThroughputMean<0){
		perror("open: Error creating file ThroughputMean");
		exit(EXIT_FAILURE);
	}

	pfilename = "/responsetimemean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdResponseTimeMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdResponseTimeMean<0){
		perror("open: Error creating file ResponseTimeMean");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class1responsetimemean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdClass1ResponseTimeMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdClass1ResponseTimeMean<0){
		perror("open: Error creating file Class1ResponseTimeMean");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class2responsetimemean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdClass2ResponseTimeMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdClass2ResponseTimeMean<0){
		perror("open: Error creating file Class2ResponseTimeMean");
		exit(EXIT_FAILURE);
	}

	pfilename = "/generalinfo.txt";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdGeneralInfo = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdGeneralInfo<0){
		perror("open: Error creating file GeneralInfo");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class1cletjobsmean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdClass1CletJobsMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdClass1CletJobsMean<0){
		perror("open: Error creating file Class1CletJobsMean");
		exit(EXIT_FAILURE);
	}
	pfilename = "/class2cletjobsmean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdClass2CletJobsMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdClass2CletJobsMean<0){
		perror("open: Error creating file Class2CletJobsMean");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class1jobsmean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdClass1JobsMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdClass1JobsMean<0){
		perror("open: Error creating file Class1JobsMean");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class1cjobsmean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdClass1CJobsMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdClass1CJobsMean<0){
		perror("open: Error creating file Class1CJobsMean");
		exit(EXIT_FAILURE);
	}


	pfilename = "/joinedusers.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdJoinedUsers = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdJoinedUsers<0){
		perror("open: Error creating file JoinedUsers");
		exit(EXIT_FAILURE);
	}

	pfilename = "/leavedusers.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdLeavedUsers = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdLeavedUsers<0){
		perror("open: Error creating file LeavedUsers");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class2cjobsmean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdClass2CJobsMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdClass2CJobsMean<0){
		perror("open: Error creating file Class2CJobsMean");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class2jobsmean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdClass2JobsMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdClass2JobsMean<0){
		perror("open: Error creating file Class2JobsMean");
		exit(EXIT_FAILURE);
	}

	pfilename = "/jobsmean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdJobsMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdJobsMean<0){
		perror("open: Error creating file JobsMean");
		exit(EXIT_FAILURE);
	}

	pfilename = "/utilizationmean.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdStruct->fdUtilizationMean = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdStruct->fdUtilizationMean<0){
		perror("open: Error creating file UtilizationMean");
		exit(EXIT_FAILURE);
	}

}

void initoutputfilesFinalValue(folder_struct folderStruct, fdFinalValue_struct *fdFinalValueStruct){

	char* pfilename = "/responsetimemeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdResponseTimeMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdResponseTimeMeanFinal<0){
		perror("open: Error creating file ResponseTimeMeanFinal");
		exit(EXIT_FAILURE);
	}
	pfilename = "/class1responsetimemeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdClass1ResponseTimeMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdClass1ResponseTimeMeanFinal<0){
		perror("open: Error creating file Class1ResponseTimeMeanFinal");
		exit(EXIT_FAILURE);
	}
	pfilename = "/class2responsetimemeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdClass2ResponseTimeMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdClass2ResponseTimeMeanFinal<0){
		perror("open: Error creating file Class2ResponseTimeMeanFinal");
		exit(EXIT_FAILURE);
	}
	pfilename = "/class1cletjobsmeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdClass1CletJobsMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdClass1CletJobsMeanFinal<0){
		perror("open: Error creating file Class1CletJobsMeanFinal");
		exit(EXIT_FAILURE);
	}


	pfilename = "/throughputmeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdThroughputMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdThroughputMeanFinal<0){
		perror("open: Error creating file ThroughPutMeanfinal");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class2cletjobsmeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdClass2CletJobsMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdClass2CletJobsMeanFinal<0){
		perror("open: Error creating file Class2CletJobsMeanFinal");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class1jobsmeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdClass1JobsMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdClass1JobsMeanFinal<0){
		perror("open: Error creating file Class1JobsMeanFinal");
		exit(EXIT_FAILURE);
	}

	pfilename = "/class1cjobsmeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdClass1CJobsMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdClass1CJobsMeanFinal<0){
		perror("open: Error creating file Class1CJobsMeanFinal");
		exit(EXIT_FAILURE);
	}
	
	pfilename = "/class2cjobsmeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdClass2CJobsMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdClass2CJobsMeanFinal<0){
		perror("open: Error creating file Class2CJobsMeanFinal");
		exit(EXIT_FAILURE);
	}
	
	pfilename = "/class2jobsmeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdClass2JobsMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdClass2JobsMeanFinal<0){
		perror("open: Error creating file Class2JobsMeanFinal");
		exit(EXIT_FAILURE);
	}

	pfilename = "/jobsmeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdJobsMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdJobsMeanFinal<0){
		perror("open: Error creating file JobsMeanFinal");
		exit(EXIT_FAILURE);
	}

	pfilename = "/utilizationmeanfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdUtilizationMeanFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdUtilizationMeanFinal<0){
		perror("open: Error creating file UtilizationMeanFinal");
		exit(EXIT_FAILURE);
	}

	pfilename = "/joinedusersfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdJoinedUsersFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdJoinedUsersFinal<0){
		perror("open: Error creating file JoinedUsersFinal");
		exit(EXIT_FAILURE);
	}

	pfilename = "/leavedusersfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdLeavedUsersFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdLeavedUsersFinal<0){
		perror("open: Error creating file LeavedUsersFinal");
		exit(EXIT_FAILURE);
	}
	pfilename = "/migrationprobabilityfinal.csv";
	if(1){
		char filepath[strlen(folderStruct.folderSIMULATIONS)+strlen(folderStruct.folderACTUAL)+strlen(pfilename)+1];
		filepath[0]='\0';
		fdFinalValueStruct->fdMigrationProbabilityFinal = open(strcat(strcat(strcat(filepath,folderStruct.folderSIMULATIONS),folderStruct.folderACTUAL),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdFinalValueStruct->fdMigrationProbabilityFinal<0){
		perror("open: Error creating file MigrationProbabilityFinal");
		exit(EXIT_FAILURE);
	}

}


void createFolders(struct tm lt, folder_struct *folderStruct){
	

	folderStruct->folderSIMULATIONS = malloc(14*sizeof(char));
	if(folderStruct->folderSIMULATIONS == NULL){
		fprintf(stderr,"Error:malloc folderStruct->folderSIMULATIONS returned null.");
	}

	snprintf(folderStruct->folderSIMULATIONS,14,"./simulations");

	folderStruct->folderACTUAL = malloc(23*sizeof(char));
	if(folderStruct->folderSIMULATIONS == NULL){
		fprintf(stderr,"Error:malloc folderStruct->folderACTUAL returned null.");
	}
	snprintf(folderStruct->folderACTUAL,23,"/%d-%d-%d %d:%d:%d", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);

	struct stat st;
	memset(&st,0,sizeof(struct stat));
	
	if (stat(folderStruct->folderSIMULATIONS, &st) == -1) {
   		if(mkdir(folderStruct->folderSIMULATIONS, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s", folderStruct->folderSIMULATIONS);
			perror("");
			exit(EXIT_FAILURE);
		}
	}

	memset(&st,0,sizeof(struct stat));
	
	
	char filepath[strlen(folderStruct->folderSIMULATIONS)+strlen(folderStruct->folderACTUAL)+1];
	filepath[0]='\0';
	strcat(strcat(filepath,folderStruct->folderSIMULATIONS),folderStruct->folderACTUAL);
	if (stat(filepath, &st) == -1) {
		if(mkdir(filepath, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s", filepath);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	else{
		fprintf(stderr,"stat: Directory %s already exists",filepath);
		perror("");
		exit(EXIT_FAILURE);
	}

	if(1){
		char *dirname="/ThroughputMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathThroughputMean = malloc(len*sizeof(char));
		if(folderStruct->pathThroughputMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathThroughputMean returned null.");
		}
		(folderStruct->pathThroughputMean)[0]='\0';
		strcat(strcat(folderStruct->pathThroughputMean,filepath),dirname);

		if(mkdir(folderStruct->pathThroughputMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathThroughputMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}

	if(1){
		char *dirname="/ResponseTimeMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathResponseTimeMean = malloc(len*sizeof(char));
		if(folderStruct->pathResponseTimeMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathResponseTimeMean returned null.");
		}
		(folderStruct->pathResponseTimeMean)[0]='\0';
		strcat(strcat(folderStruct->pathResponseTimeMean,filepath),dirname);

		if(mkdir(folderStruct->pathResponseTimeMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathResponseTimeMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/CloudletResponseTimeMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathCloudletResponseTimeMean = malloc(len*sizeof(char));
		if(folderStruct->pathCloudletResponseTimeMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathCloudletResponseTimeMean returned null.");
		}
		(folderStruct->pathCloudletResponseTimeMean)[0]='\0';
		strcat(strcat(folderStruct->pathCloudletResponseTimeMean,filepath),dirname);

		if(mkdir(folderStruct->pathCloudletResponseTimeMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathCloudletResponseTimeMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/ResponseCloudTimeMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathCloudResponseTimeMean = malloc(len*sizeof(char));
		if(folderStruct->pathCloudResponseTimeMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathCloudResponseTimeMean returned null.");
		}
		(folderStruct->pathCloudResponseTimeMean)[0]='\0';
		strcat(strcat(folderStruct->pathCloudResponseTimeMean,filepath),dirname);

		if(mkdir(folderStruct->pathCloudResponseTimeMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathCloudResponseTimeMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Class1ResponseTimeMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathClass1ResponseTimeMean = malloc(len*sizeof(char));
		if(folderStruct->pathClass1ResponseTimeMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathClass1ResponseTimeMean returned null.");
		}
		(folderStruct->pathClass1ResponseTimeMean)[0]='\0';
		strcat(strcat(folderStruct->pathClass1ResponseTimeMean,filepath),dirname);

		if(mkdir(folderStruct->pathClass1ResponseTimeMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathClass1ResponseTimeMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Class2ResponseTimeMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathClass2ResponseTimeMean = malloc(len*sizeof(char));
		if(folderStruct->pathClass2ResponseTimeMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathClass2ResponseTimeMean returned null.");
		}
		(folderStruct->pathClass2ResponseTimeMean)[0]='\0';
		strcat(strcat(folderStruct->pathClass2ResponseTimeMean,filepath),dirname);

		if(mkdir(folderStruct->pathClass2ResponseTimeMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathClass2ResponseTimeMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Class1CletJobsMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathClass1CletJobsMean = malloc(len*sizeof(char));
		if(folderStruct->pathClass1CletJobsMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathClass1CletJobsMean returned null.");
		}
		(folderStruct->pathClass1CletJobsMean)[0]='\0';
		strcat(strcat(folderStruct->pathClass1CletJobsMean,filepath),dirname);

		if(mkdir(folderStruct->pathClass1CletJobsMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathClass1CletJobsMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Class2CletJobsMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathClass2CletJobsMean = malloc(len*sizeof(char));
		if(folderStruct->pathClass2CletJobsMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathClass2CletJobsMean returned null.");
		}
		(folderStruct->pathClass2CletJobsMean)[0]='\0';
		strcat(strcat(folderStruct->pathClass2CletJobsMean,filepath),dirname);

		if(mkdir(folderStruct->pathClass2CletJobsMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathClass2CletJobsMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Class1JobsMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathClass1JobsMean = malloc(len*sizeof(char));
		if(folderStruct->pathClass1JobsMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathClass1JobsMean returned null.");
		}
		(folderStruct->pathClass1JobsMean)[0]='\0';
		strcat(strcat(folderStruct->pathClass1JobsMean,filepath),dirname);

		if(mkdir(folderStruct->pathClass1JobsMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathClass1JobsMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Class1CJobsMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathClass1CJobsMean = malloc(len*sizeof(char));
		if(folderStruct->pathClass1CJobsMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathClass1CJobsMean returned null.");
		}
		(folderStruct->pathClass1CJobsMean)[0]='\0';
		strcat(strcat(folderStruct->pathClass1CJobsMean,filepath),dirname);

		if(mkdir(folderStruct->pathClass1CJobsMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathClass1CJobsMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Class2CJobsMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathClass2CJobsMean = malloc(len*sizeof(char));
		if(folderStruct->pathClass2CJobsMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathClass2CJobsMean returned null.");
		}
		(folderStruct->pathClass2CJobsMean)[0]='\0';
		strcat(strcat(folderStruct->pathClass2CJobsMean,filepath),dirname);

		if(mkdir(folderStruct->pathClass2CJobsMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathClass2CJobsMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Class2JobsMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathClass2JobsMean = malloc(len*sizeof(char));
		if(folderStruct->pathClass2JobsMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathClass2JobsMean returned null.");
		}
		(folderStruct->pathClass2JobsMean)[0]='\0';
		strcat(strcat(folderStruct->pathClass2JobsMean,filepath),dirname);

		if(mkdir(folderStruct->pathClass2JobsMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathClass2JobsMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/JobsMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathJobsMean = malloc(len*sizeof(char));
		if(folderStruct->pathJobsMean == NULL){
			fprintf(stderr,"Error:malloc folderStruct->pathJobsMean returned null.");
		}
		(folderStruct->pathJobsMean)[0]='\0';
		strcat(strcat(folderStruct->pathJobsMean,filepath),dirname);

		if(mkdir(folderStruct->pathJobsMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathJobsMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/UtilizationMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathUtilizationMean = malloc(len*sizeof(char));
		if(folderStruct->pathUtilizationMean == NULL){
			fprintf(stderr,"Error:malloc *pathUtilizationMean returned null.");
		}
		(folderStruct->pathUtilizationMean)[0]='\0';
		strcat(strcat(folderStruct->pathUtilizationMean,filepath),dirname);

		if(mkdir(folderStruct->pathUtilizationMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathUtilizationMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/JoinedUsers";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathJoinedUsers = malloc(len*sizeof(char));
		if(folderStruct->pathJoinedUsers == NULL){
			fprintf(stderr,"Error:malloc *pathJoinedUsers returned null.");
		}
		(folderStruct->pathJoinedUsers)[0]='\0';
		strcat(strcat(folderStruct->pathJoinedUsers,filepath),dirname);

		if(mkdir(folderStruct->pathJoinedUsers, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathJoinedUsers);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/LeavedUsers";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathLeavedUsers = malloc(len*sizeof(char));
		if(folderStruct->pathLeavedUsers == NULL){
			fprintf(stderr,"Error:malloc *pathLeavedUsers returned null.");
		}
		(folderStruct->pathLeavedUsers)[0]='\0';
		strcat(strcat(folderStruct->pathLeavedUsers,filepath),dirname);

		if(mkdir(folderStruct->pathLeavedUsers, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathLeavedUsers);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/ArrivalFreq";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathArrivalFreq = malloc(len*sizeof(char));
		if(folderStruct->pathArrivalFreq == NULL){
			fprintf(stderr,"Error:malloc *pathArrivalFreq returned null.");
		}
		(folderStruct->pathArrivalFreq)[0]='\0';
		strcat(strcat(folderStruct->pathArrivalFreq,filepath),dirname);

		if(mkdir(folderStruct->pathArrivalFreq, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathArrivalFreq);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Throughput";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathThroughput = malloc(len*sizeof(char));
		if(folderStruct->pathThroughput == NULL){
			fprintf(stderr,"Error:malloc *pathThroughput returned null.");
		}
		(folderStruct->pathThroughput)[0]='\0';
		strcat(strcat(folderStruct->pathThroughput,filepath),dirname);

		if(mkdir(folderStruct->pathThroughput, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathThroughput);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Interdeparture";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathInterdeparture = malloc(len*sizeof(char));
		if(folderStruct->pathInterdeparture == NULL){
			fprintf(stderr,"Error:malloc *pathInterdeparture returned null.");
		}
		(folderStruct->pathInterdeparture)[0]='\0';
		strcat(strcat(folderStruct->pathInterdeparture,filepath),dirname);

		if(mkdir(folderStruct->pathInterdeparture, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathInterdeparture);
			perror("");
			exit(EXIT_FAILURE);
		}
	}
	if(1){
		char *dirname="/Interarrival";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathInterarrival = malloc(len*sizeof(char));
		if(folderStruct->pathInterarrival == NULL){
			fprintf(stderr,"Error:malloc *pathInterarrival returned null.");
		}
		(folderStruct->pathInterarrival)[0]='\0';
		strcat(strcat(folderStruct->pathInterarrival,filepath),dirname);

		if(mkdir(folderStruct->pathInterarrival, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathInterarrival);
			perror("");
			exit(EXIT_FAILURE);
		}
	}

	if(1){
		char *dirname="/MigrationPercentageMean";
		int len =  strlen(filepath)+strlen(dirname)+1;
		folderStruct->pathMigrationPercentageMean = malloc(len*sizeof(char));
		if(folderStruct->pathMigrationPercentageMean == NULL){
			fprintf(stderr,"Error:malloc *pathMigrationPercentageMean returned null.");
		}
		(folderStruct->pathMigrationPercentageMean)[0]='\0';
		strcat(strcat(folderStruct->pathMigrationPercentageMean,filepath),dirname);

		if(mkdir(folderStruct->pathMigrationPercentageMean, 0700) != 0){
			fprintf(stderr,"mkdir: Error creating the folder %s",folderStruct->pathMigrationPercentageMean);
			perror("");
			exit(EXIT_FAILURE);
		}
	}

}


void initEventList(list* mylist,settings_struct settingsStruct){

	if(1){	//Creating a class 1 arrival event
		double *FirstArrivalTimePtr = (double*)malloc(sizeof(double));
		char *FirstArrivalTypePtr = (char*)malloc(sizeof(char));
		double *FirstListInsertionTimePtr = (double*)malloc(sizeof(double)); 
		*FirstArrivalTypePtr='A';
		SelectStream(settingsStruct.arrivalstream1);
		*FirstArrivalTimePtr=Exponential(settingsStruct.arrivalmean1);
		*FirstListInsertionTimePtr=0.0;
		list_put(mylist,FirstArrivalTimePtr,FirstArrivalTypePtr,FirstListInsertionTimePtr,NULL);
	}
	if(1){ //Creating a class 2 arrival event
		double *FirstArrivalTimePtr = (double*)malloc(sizeof(double));
		char *FirstArrivalTypePtr = (char*)malloc(sizeof(char));
		double *FirstListInsertionTimePtr = (double*)malloc(sizeof(double)); 
		*FirstArrivalTypePtr='a';
		SelectStream(settingsStruct.arrivalstream2);
		*FirstArrivalTimePtr=Exponential(settingsStruct.arrivalmean2);
		*FirstListInsertionTimePtr=0.0;
		list_put(mylist,FirstArrivalTimePtr,FirstArrivalTypePtr,FirstListInsertionTimePtr,NULL);
	}

	//Creating a sample event
	double *FirstSampleTimePtr = (double*)malloc(sizeof(double));
	char *FirstSampleTypePtr = (char*)malloc(sizeof(char));
	double *FirstSampleInsertionTimePtr = (double*)malloc(sizeof(double)); 
	*FirstSampleTypePtr='s';
	*FirstSampleTimePtr=0;
	*FirstSampleInsertionTimePtr=0.0;
	list_put(mylist,FirstSampleTimePtr,FirstSampleTypePtr,FirstSampleInsertionTimePtr,NULL);

		
}


void samplingEvent(list* mylist,settings_struct settingsStruct,stats_struct *statsStruct,fdRun_struct fdRunStruct){

	if(statsStruct->universalTime>=settingsStruct.starttoprint){

		/*****************migration percentage mean**********************/
		if(statsStruct->ArrivedClass2CletJobsFromBigBang>0){
			if(dprintf(fdRunStruct.fdMigrationPercentageMeanTemp,"%f,%f,\n",statsStruct->universalTime,((double)statsStruct->MigratedJobsFromBigBang)/((double)statsStruct->ArrivedClass2CletJobsFromBigBang))<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: fdMigrationPercentageTemp dprintf didn't print");
				exit(EXIT_FAILURE);
			}
		}
		else{
			if(dprintf(fdRunStruct.fdMigrationPercentageMeanTemp,"%f,%f,\n",statsStruct->universalTime,0.0)<=0){ 											//POSIX standard since 2008.
				fprintf(stderr,"Error: fdMigrationPercentageTemp dprintf didn't print");
			exit(EXIT_FAILURE);
			}	
		}


		/*****************getting and printing throughput mean**********************/
		if(statsStruct->LeavedJobsFromBigBang>0){
			if(dprintf(fdRunStruct.fdThroughputMeanTemp,"%f,%f,\n",statsStruct->universalTime,statsStruct->LeavedJobsFromBigBang/statsStruct->universalTime)<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}
		}
		else
			if(dprintf(fdRunStruct.fdThroughputMeanTemp,"%f,%f,\n",statsStruct->universalTime,0.0)<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}
		/*****************printing response time mean**********************/
		if(dprintf(fdRunStruct.fdResponseTimeMeanTemp,"%f,%f,\n",statsStruct->universalTime,statsStruct->ResponseTimeMean)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: fdResponseTimeMean dprintf didn't print");
			exit(EXIT_FAILURE);
		}
		/*****************printing cloudlet response time mean**********************/
		if(dprintf(fdRunStruct.fdCloudletResponseTimeMeanTemp,"%f,%f,\n",statsStruct->universalTime,statsStruct->CloudletResponseTimeMean)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: fdCloudletResponseTimeMean dprintf didn't print");
			exit(EXIT_FAILURE);
		}
		/*****************printing cloud response time mean**********************/
		if(dprintf(fdRunStruct.fdCloudResponseTimeMeanTemp,"%f,%f,\n",statsStruct->universalTime,statsStruct->CloudResponseTimeMean)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: fdCloudResponseTimeMean dprintf didn't print");
			exit(EXIT_FAILURE);
		}
		/*****************printing utilization mean mean**********************/
		if(dprintf(fdRunStruct.fdUtilizationMeanTemp,"%f,%f,\n",statsStruct->universalTime,statsStruct->UtilizationMean)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
#if PRINTALL
		double temp;
		/*****************printing class 1 response time mean**********************/
		if(dprintf(fdRunStruct.fdClass1ResponseTimeMeanTemp,"%f,%f,\n",statsStruct->universalTime,statsStruct->Class1ResponseTimeMean)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: fdClass1ResponseTimeMeanTemp dprintf didn't print");
			exit(EXIT_FAILURE);
		}
		/*****************printing class 2 response time mean**********************/
		if(dprintf(fdRunStruct.fdClass2ResponseTimeMeanTemp,"%f,%f,\n",statsStruct->universalTime,statsStruct->Class2ResponseTimeMean)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: fdClass2ResponseTimeMeanTemp dprintf didn't print");
			exit(EXIT_FAILURE);
		}
		/*****************getting and printing JobsMean**********************/
		temp=getWellfordSamplePath(statsStruct->JobsMean,statsStruct->universalTime,statsStruct->JobsMeanLastUpdate,(double)(statsStruct->Class1CJobs+statsStruct->Class2CJobs+statsStruct->Class1CletJobs+statsStruct->Class2CletJobs));
		if(temp<0){
			fprintf(stderr,"negative statsStruct.JobsMean");
			exit(EXIT_FAILURE);
		}
		if(temp != temp)
			temp=0.0;
		if(dprintf(fdRunStruct.fdJobsMeanTemp,"%f,%f,\n",statsStruct->universalTime,temp)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}	
		/*****************getting and printing Class1CletJobsMean**********************/
		temp=getWellfordSamplePath(statsStruct->Class1CletJobsMean,statsStruct->universalTime,statsStruct->Class1CletJobsMeanLastUpdate,(double)(statsStruct->Class1CletJobs));
		if(temp<0.0){
			fprintf(stderr,"negative statsStruct.Class1CletJobsMean");
			exit(EXIT_FAILURE);
		}
		if(temp != temp)
			temp=0.0;
		if(dprintf(fdRunStruct.fdClass1CletJobsMeanTemp,"%f,%f,\n",statsStruct->universalTime,temp)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
		/*****************getting and printing Class2CletJobsMean**********************/
		temp=getWellfordSamplePath(statsStruct->Class2CletJobsMean,statsStruct->universalTime,statsStruct->Class2CletJobsMeanLastUpdate,(double)(statsStruct->Class2CletJobs));
		if(temp<0.0){
			fprintf(stderr,"negative statsStruct.Class2CletJobsMean");
			exit(EXIT_FAILURE);
		}
		if(temp != temp)
			temp=0.0;
		if(dprintf(fdRunStruct.fdClass2CletJobsMeanTemp,"%f,%f,\n",statsStruct->universalTime,temp)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}	
		/*****************getting and printing Class1JobsMean**********************/
		temp=getWellfordSamplePath(statsStruct->Class1JobsMean,statsStruct->universalTime,statsStruct->Class1JobsMeanLastUpdate,(double)(statsStruct->Class1CletJobs+statsStruct->Class1CJobs));
		if(temp<0.0){
			fprintf(stderr,"negative statsStruct.Class1JobsMean");
			exit(EXIT_FAILURE);
		}
		if(temp != temp)
			temp=0.0;
		if(dprintf(fdRunStruct.fdClass1JobsMeanTemp,"%f,%f,\n",statsStruct->universalTime,temp)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
		/*****************getting and printing Class1CJobsMean**********************/
		temp=getWellfordSamplePath(statsStruct->Class1CJobsMean,statsStruct->universalTime,statsStruct->Class1CJobsMeanLastUpdate,(double)(statsStruct->Class1CJobs));
		if(temp<0){
			fprintf(stderr,"negative statsStruct.Class1CJobsMean");
			exit(EXIT_FAILURE);
		}
		if(temp != temp)
			temp=0.0;
		if(dprintf(fdRunStruct.fdClass1CJobsMeanTemp,"%f,%f,\n",statsStruct->universalTime,temp)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
		/*****************getting and printing Class2CJobsMean**********************/
		temp=getWellfordSamplePath(statsStruct->Class2CJobsMean,statsStruct->universalTime,statsStruct->Class2CJobsMeanLastUpdate,(double)(statsStruct->Class2CJobs));
		if(temp<0){
			fprintf(stderr,"negative statsStruct.Class2CJobsMean");
			exit(EXIT_FAILURE);
		}
		if(temp != temp)
			temp=0.0;
		if(dprintf(fdRunStruct.fdClass2CJobsMeanTemp,"%f,%f,\n",statsStruct->universalTime,temp)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
		/*****************getting and printing Class2JobsMean**********************/
		temp=getWellfordSamplePath(statsStruct->Class2JobsMean,statsStruct->universalTime,statsStruct->Class2JobsMeanLastUpdate,(double)(statsStruct->Class2CletJobs+statsStruct->Class2CJobs));
		if(temp<0){
			fprintf(stderr,"negative statsStruct->Class2JobsMean");
			exit(EXIT_FAILURE);
		}
		if(temp != temp)
			temp=0.0;
		if(dprintf(fdRunStruct.fdClass2JobsMeanTemp,"%f,%f,\n",statsStruct->universalTime,temp)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
#endif

	}

	if(settingsStruct.printinters==1){
		/*****************printing ArrivalFreq**********************/
		if(statsStruct->ArrivedJobsFromLastSample>0){
			if(dprintf(fdRunStruct.fdArrivalFreqTemp,"%f\n",((double)statsStruct->ArrivedJobsFromLastSample)/(double)settingsStruct.sampletime)<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}
		}
		else
			if(dprintf(fdRunStruct.fdArrivalFreqTemp,"%f\n",0.0)<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}

		/*****************printing Throughput**********************/
		if(statsStruct->LeavedJobsFromLastSample>0){
			if(dprintf(fdRunStruct.fdThroughputTemp,"%f\n",((double)statsStruct->LeavedJobsFromLastSample)/(double)settingsStruct.sampletime)<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}
		}
		else
			if(dprintf(fdRunStruct.fdThroughputTemp,"%f\n",0.0)<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}
	}

	(statsStruct->samples)++;
	statsStruct->ArrivedJobsFromLastSample=0;
	statsStruct->LeavedJobsFromLastSample=0;


	//Creating a new sample event
	double *SampleTimePtr = (double*)malloc(sizeof(double));
	char *SampleTypePtr = (char*)malloc(sizeof(char));
	double *SampleInsertionTimePtr = (double*)malloc(sizeof(double)); 
	*SampleTypePtr='s';
	*SampleTimePtr=statsStruct->universalTime+settingsStruct.sampletime;
	*SampleInsertionTimePtr=0.0; //dummy
	list_put(mylist,SampleTimePtr,SampleTypePtr,SampleInsertionTimePtr,NULL);
}


void samplingFinalValue(settings_struct settingsStruct,stats_struct statsStruct,fdFinalValue_struct fdFinalValueStruct){
	double temp=0;
	/*****************printing ResponseTimeMean**********************/
	if(dprintf(fdFinalValueStruct.fdResponseTimeMeanFinal,"%f,\n",statsStruct.ResponseTimeMean)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: fdResponseTimeMeanFinal dprintf didn't print");
		exit(EXIT_FAILURE);
	}
	/*****************printing Class1ResponseTimeMean**********************/
	if(dprintf(fdFinalValueStruct.fdClass1ResponseTimeMeanFinal,"%f,\n",statsStruct.Class1ResponseTimeMean)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: fdClass1ResponseTimeMeanFinal dprintf didn't print");
		exit(EXIT_FAILURE);
	}
	/*****************printing Class2ResponseTimeMean**********************/
	if(dprintf(fdFinalValueStruct.fdClass2ResponseTimeMeanFinal,"%f,\n",statsStruct.Class2ResponseTimeMean)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: fdClass2ResponseTimeMeanFinal dprintf didn't print");
		exit(EXIT_FAILURE);
	}
	/*****************printing UtilizationMean**********************/
	if(dprintf(fdFinalValueStruct.fdUtilizationMeanFinal,"%f,\n",statsStruct.UtilizationMean)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: dprintf didn't print");
	}
	/*****************printing ThroughputMean**********************/
	if(statsStruct.LeavedJobsFromBigBang>0){
		if(dprintf(fdFinalValueStruct.fdThroughputMeanFinal,"%f,\n",statsStruct.LeavedJobsFromBigBang/statsStruct.universalTime)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
	}
	else
		if(dprintf(fdFinalValueStruct.fdThroughputMeanFinal,"%f,\n",0.0)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
	/*****************printing JobsMean**********************/
	temp=getWellfordSamplePath(statsStruct.JobsMean,statsStruct.universalTime,statsStruct.JobsMeanLastUpdate,
		(double)(statsStruct.Class1CJobs+statsStruct.Class2CJobs+statsStruct.Class1CletJobs+statsStruct.Class2CletJobs));
	if(temp<0){
		fprintf(stderr,"negative statsStruct.JobsMean");
		exit(EXIT_FAILURE);
	}
	if(dprintf(fdFinalValueStruct.fdJobsMeanFinal,"%f,\n",temp)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: dprintf didn't print");
	}	
	/*****************printing Class1CletJobsMean**********************/
	temp=getWellfordSamplePath(statsStruct.Class1CletJobsMean,statsStruct.universalTime,statsStruct.Class1CletJobsMeanLastUpdate,(double)(statsStruct.Class1CletJobs));
	if(temp<0.0){
		fprintf(stderr,"negative statsStruct.Class1CletJobsMean");
		exit(EXIT_FAILURE);
	}
	if(dprintf(fdFinalValueStruct.fdClass1CletJobsMeanFinal,"%f,\n",temp)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: dprintf didn't print");
	}
	/*****************printing Class2CletJobsMean**********************/
	temp=getWellfordSamplePath(statsStruct.Class2CletJobsMean,statsStruct.universalTime,statsStruct.Class2CletJobsMeanLastUpdate,(double)(statsStruct.Class2CletJobs));
	if(temp<0.0){
		fprintf(stderr,"negative statsStruct.Class2CletJobsMean");
		exit(EXIT_FAILURE);
	}
	if(dprintf(fdFinalValueStruct.fdClass2CletJobsMeanFinal,"%f,\n",temp)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: dprintf didn't print");
	}	
	/*****************printing Class1JobsMean**********************/
	temp=getWellfordSamplePath(statsStruct.Class1JobsMean,statsStruct.universalTime,statsStruct.Class1JobsMeanLastUpdate,(double)(statsStruct.Class1CletJobs+statsStruct.Class1CJobs));
	if(temp<0.0){
		fprintf(stderr,"negative statsStruct.Class1JobsMean");
		exit(EXIT_FAILURE);
	}
	if(dprintf(fdFinalValueStruct.fdClass1JobsMeanFinal,"%f,\n",temp)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: dprintf didn't print");
	}
	/*****************printing Class1CJobsMean**********************/
	temp=getWellfordSamplePath(statsStruct.Class1CJobsMean,statsStruct.universalTime,statsStruct.Class1CJobsMeanLastUpdate,(double)(statsStruct.Class1CJobs));
	if(temp<0){
		fprintf(stderr,"negative statsStruct.Class1CJobsMean");
		exit(EXIT_FAILURE);
	}
	if(dprintf(fdFinalValueStruct.fdClass1CJobsMeanFinal,"%f,\n",temp)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: dprintf didn't print");
	}
	
	/*****************printing Class2CJobsMean**********************/
	temp=getWellfordSamplePath(statsStruct.Class2CJobsMean,statsStruct.universalTime,statsStruct.Class2CJobsMeanLastUpdate,(double)(statsStruct.Class2CJobs));
	if(temp<0){
		fprintf(stderr,"negative statsStruct.Class2CJobsMean");
		exit(EXIT_FAILURE);
	}
	if(dprintf(fdFinalValueStruct.fdClass2CJobsMeanFinal,"%f,\n",temp)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: dprintf didn't print");
	}
	/*****************printing Class2JobsMean**********************/
	temp=getWellfordSamplePath(statsStruct.Class2JobsMean,statsStruct.universalTime,statsStruct.Class2JobsMeanLastUpdate,(double)(statsStruct.Class2CletJobs+statsStruct.Class2CJobs));
	if(temp<0){
		fprintf(stderr,"negative statsStruct->Class2JobsMean");
		exit(EXIT_FAILURE);
	}
	if(dprintf(fdFinalValueStruct.fdClass2JobsMeanFinal,"%f,\n",temp)<=0){ //POSIX standard since 2008.
		fprintf(stderr,"Error: dprintf didn't print");
	}
	/*****************printing MigrationProbability**********************/
	if(statsStruct.ArrivedClass2CletJobsFromBigBang>0){
		if(dprintf(fdFinalValueStruct.fdMigrationProbabilityFinal,"%f,\n",((double)statsStruct.MigratedJobsFromBigBang)/((double)statsStruct.ArrivedClass2CletJobsFromBigBang))<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: fdMigrationProbabilityFinal dprintf didn't print");
			exit(EXIT_FAILURE);
		}
	}
	else{
		if(dprintf(fdFinalValueStruct.fdMigrationProbabilityFinal,"%f,\n",0.0)<=0){ 											//POSIX standard since 2008.
			fprintf(stderr,"Error: fdMigrationProbabilityFinal dprintf didn't print");
		exit(EXIT_FAILURE);
		}	
	}
	
}

// Class 1 arrival event
void C1ArrivalEvent(list* mylist,settings_struct settingsStruct, stats_struct *statsStruct,fdRun_struct fdRunStruct){
	if(statsStruct->Class1CletJobs==settingsStruct.capacity){
		//Adding departure event for a class 1 job in the cloud
		updateClass1CJobsMean(statsStruct);
		updateClass1JobsMean(statsStruct);
		updateJobsMean(statsStruct);
		updateInterarrival(settingsStruct,statsStruct,fdRunStruct);
		(statsStruct->ArrivedJobsFromBigBang)++;
		(statsStruct->ArrivedJobsFromLastSample)++;
		(statsStruct->Class1CJobs)++;


		addC1departure(mylist, *statsStruct, settingsStruct);

	}
	else if(((statsStruct->Class1CletJobs)+(statsStruct->Class2CletJobs))<settingsStruct.threshold){
		//Adding departure event for a class 1 job in the cloudlet
		updateClass1CletJobsMean(statsStruct);
		updateClass1JobsMean(statsStruct);
		updateJobsMean(statsStruct);
		updateInterarrival(settingsStruct,statsStruct,fdRunStruct);
		updateUtilization(settingsStruct,statsStruct);
		(statsStruct->ArrivedJobsFromBigBang)++;
		(statsStruct->ArrivedJobsFromLastSample)++;
		(statsStruct->Class1CletJobs)++;

		addClet1departure(mylist, *statsStruct, settingsStruct);
	}
	else if((statsStruct->Class2CletJobs)>0){
		//##migration of one random class 2 job from the cloudlet to the cloud##
		migrateClass2FromCletToC(mylist,settingsStruct,*statsStruct);

		updateClass2CletJobsMean(statsStruct);
		updateClass2CJobsMean(statsStruct);
		(statsStruct->Class2CletJobs)--;
		(statsStruct->Class2CJobs)++;
		(statsStruct->MigratedJobsFromBigBang)++;



		//####Adding departure event for a class 1 job in the cloudlet######
		updateClass1CletJobsMean(statsStruct);
		updateClass1JobsMean(statsStruct);
		updateInterarrival(settingsStruct,statsStruct,fdRunStruct);
		updateJobsMean(statsStruct);
		(statsStruct->ArrivedJobsFromBigBang)++;
		(statsStruct->ArrivedJobsFromLastSample)++;
		(statsStruct->Class1CletJobs)++;


		addClet1departure(mylist, *statsStruct, settingsStruct);
	}
	else{
		//Adding departure event for a class 1 job in the cloudlet

		updateClass1CletJobsMean(statsStruct);
		updateClass1JobsMean(statsStruct);
		updateJobsMean(statsStruct);
		updateInterarrival(settingsStruct,statsStruct,fdRunStruct);
		updateUtilization(settingsStruct,statsStruct);

		(statsStruct->ArrivedJobsFromBigBang)++;
		(statsStruct->ArrivedJobsFromLastSample)++;
		(statsStruct->Class1CletJobs)++;



		addClet1departure(mylist, *statsStruct, settingsStruct);

	}
	//Adding arrival event
	addArrival1(mylist, *statsStruct, settingsStruct);

}

void C2ArrivalEvent(list* mylist,settings_struct settingsStruct, stats_struct *statsStruct,fdRun_struct fdRunStruct){
	if(((statsStruct->Class1CletJobs)+(statsStruct->Class2CletJobs))>=settingsStruct.threshold){
		//Adding departure event for a class 2 job in the cloud
		updateClass2CJobsMean(statsStruct);
		updateClass2JobsMean(statsStruct);
		updateInterarrival(settingsStruct,statsStruct,fdRunStruct);
		updateJobsMean(statsStruct);

		(statsStruct->ArrivedJobsFromBigBang)++;
		(statsStruct->ArrivedJobsFromLastSample)++;
		(statsStruct->Class2CJobs)++;



		addC2departure(mylist, *statsStruct, settingsStruct);

	}
	else{
		//Adding departure event for a class 2 job in the cloudlet
		updateClass2CletJobsMean(statsStruct);
		updateClass2JobsMean(statsStruct);
		updateJobsMean(statsStruct);
		updateInterarrival(settingsStruct,statsStruct,fdRunStruct);
		updateUtilization(settingsStruct,statsStruct);

		(statsStruct->ArrivedJobsFromBigBang)++;
		(statsStruct->ArrivedJobsFromLastSample)++;
		(statsStruct->Class2CletJobs)++;
		(statsStruct->ArrivedClass2CletJobsFromBigBang)++;




		addClet2departure(mylist, *statsStruct, settingsStruct);

	}
	//Adding arrival event
	addArrival2(mylist, *statsStruct, settingsStruct);
}

void Clet1DepartureEvent(stats_struct *statsStruct,double arrivalTime,settings_struct settingsStruct,fdRun_struct fdRunStruct){
	updateClass1CletJobsMean(statsStruct);
	updateClass1JobsMean(statsStruct);
	updateInterdeparture(settingsStruct,statsStruct,fdRunStruct);
	updateJobsMean(statsStruct);
	updateUtilization(settingsStruct,statsStruct);
	if((statsStruct->Class1CletJobs)--<0){
		fprintf(stderr,"Error: negative Class1CletJobs");
		exit(EXIT_FAILURE);
	}

	(statsStruct->LeavedJobsFromBigBang)++;
	(statsStruct->LeavedJobsFromLastSample)++;
	(statsStruct->LeavedClass1JobsFromBigBang)++;
	updateResponseTimeMean(arrivalTime,statsStruct);
	updateClass1ResponseTimeMean(arrivalTime,statsStruct);
}


void Clet2DepartureEvent(stats_struct *statsStruct,double arrivalTime,settings_struct settingsStruct,fdRun_struct fdRunStruct){
	updateClass2CletJobsMean(statsStruct);
	updateClass2JobsMean(statsStruct);
	updateInterdeparture(settingsStruct,statsStruct,fdRunStruct);
	updateJobsMean(statsStruct);
	updateUtilization(settingsStruct,statsStruct);
	if((statsStruct->Class2CletJobs)--<0){
		fprintf(stderr,"Error: negative Class2CletJobs");
		exit(EXIT_FAILURE);
	}

	(statsStruct->LeavedJobsFromBigBang)++;
	(statsStruct->LeavedJobsFromLastSample)++;
	(statsStruct->LeavedClass2JobsFromBigBang)++;
	updateResponseTimeMean(arrivalTime,statsStruct);
	updateClass2ResponseTimeMean(arrivalTime,statsStruct);
}

void C1DepartureEvent(stats_struct *statsStruct,double arrivalTime,settings_struct settingsStruct,fdRun_struct fdRunStruct){
	updateClass1CJobsMean(statsStruct);
	updateClass1JobsMean(statsStruct);
	updateInterdeparture(settingsStruct,statsStruct,fdRunStruct);
	updateJobsMean(statsStruct);
	if((statsStruct->Class1CJobs)--<0){
		fprintf(stderr,"Error: negative Class1CJobs");
		exit(EXIT_FAILURE);
	}

	(statsStruct->LeavedJobsFromBigBang)++;
	(statsStruct->LeavedJobsFromLastSample)++;
	(statsStruct->LeavedClass1JobsFromBigBang)++;
	updateResponseTimeMean(arrivalTime,statsStruct);
	updateClass1ResponseTimeMean(arrivalTime,statsStruct);
}

void C2DepartureEvent(stats_struct *statsStruct,double arrivalTime,settings_struct settingsStruct,fdRun_struct fdRunStruct){
	updateClass2CJobsMean(statsStruct);
	updateClass2JobsMean(statsStruct);
	updateInterdeparture(settingsStruct,statsStruct,fdRunStruct);
	updateJobsMean(statsStruct);
	if((statsStruct->Class2CJobs)--<0){
		fprintf(stderr,"Error: negative Class2CJobs");
		exit(EXIT_FAILURE);
	}

	(statsStruct->LeavedJobsFromBigBang)++;
	(statsStruct->LeavedJobsFromLastSample)++;
	(statsStruct->LeavedClass2JobsFromBigBang)++;
	updateResponseTimeMean(arrivalTime,statsStruct);
	updateClass2ResponseTimeMean(arrivalTime,statsStruct);
}

void updateResponseTimeMean(double arrivalTime, stats_struct *statsStruct){
	if(statsStruct->universalTime-arrivalTime<0){
		fprintf(stderr,"elapsed time");
		exit(EXIT_FAILURE);
	}
	wellfordOnePass(&(statsStruct->ResponseTimeMean),(statsStruct->universalTime-arrivalTime),statsStruct->LeavedJobsFromBigBang);
	
	if(statsStruct->ResponseTimeMean<0.0){
		fprintf(stderr,"negative statsStruct->ResponseTimeMean");
		exit(EXIT_FAILURE);
	}
}


void updateCloudletResponseTimeMean(double arrivalTime, stats_struct *statsStruct){
	if(statsStruct->universalTime-arrivalTime<0){
		fprintf(stderr,"elapsed time");
		exit(EXIT_FAILURE);
	}
	wellfordOnePass(&(statsStruct->CloudletResponseTimeMean),(statsStruct->universalTime-arrivalTime),statsStruct->LeavedJobsCloudletFromBigBang);
	
	if(statsStruct->ResponseTimeMean<0.0){
		fprintf(stderr,"negative statsStruct->ResponseTimeMean");
		exit(EXIT_FAILURE);
	}
}

void updateCloudResponseTimeMean(double arrivalTime, stats_struct *statsStruct){
	if(statsStruct->universalTime-arrivalTime<0){
		fprintf(stderr,"elapsed time");
		exit(EXIT_FAILURE);
	}
	wellfordOnePass(&(statsStruct->ResponseTimeMean),(statsStruct->universalTime-arrivalTime),statsStruct->LeavedJobsCloudFromBigBang);
	
	if(statsStruct->ResponseTimeMean<0.0){
		fprintf(stderr,"negative statsStruct->ResponseTimeMean");
		exit(EXIT_FAILURE);
	}
}


void updateClass1ResponseTimeMean(double arrivalTime, stats_struct *statsStruct){
	if(statsStruct->universalTime-arrivalTime<0){
		fprintf(stderr,"elapsed time");
		exit(EXIT_FAILURE);
	}
	wellfordOnePass(&(statsStruct->Class1ResponseTimeMean),(statsStruct->universalTime-arrivalTime),statsStruct->LeavedClass1JobsFromBigBang);

	if(statsStruct->Class1ResponseTimeMean<0.0){
		fprintf(stderr,"negative statsStruct->Class1ResponseTimeMean");
		exit(EXIT_FAILURE);
	}
}
void updateClass2ResponseTimeMean(double arrivalTime, stats_struct *statsStruct){
	if(statsStruct->universalTime-arrivalTime<0){
		fprintf(stderr,"elapsed time");
		exit(EXIT_FAILURE);
	}
	wellfordOnePass(&(statsStruct->Class2ResponseTimeMean),(statsStruct->universalTime-arrivalTime),statsStruct->LeavedClass2JobsFromBigBang);

	if(statsStruct->Class2ResponseTimeMean<0.0){
		fprintf(stderr,"negative statsStruct->Class2ResponseTimeMean");
		exit(EXIT_FAILURE);
	}
}

void updateUtilization(settings_struct settingsStruct, stats_struct *statsStruct){
	wellfordSamplePath(&(statsStruct->UtilizationMean),statsStruct->universalTime,statsStruct->UtilizationMeanLastUpdate,((double)((statsStruct->Class1CletJobs)+(statsStruct->Class2CletJobs)))/(double)(settingsStruct.capacity));
	if(statsStruct->UtilizationMean<0.0){
		fprintf(stderr,"negative statsStruct->UtilizationMean");
		exit(EXIT_FAILURE);
	}

	statsStruct->UtilizationMeanLastUpdate=statsStruct->universalTime;
}

void updateClass1CletJobsMean(stats_struct *statsStruct){
	wellfordSamplePath(&(statsStruct->Class1CletJobsMean),statsStruct->universalTime,statsStruct->Class1CletJobsMeanLastUpdate,(double)(statsStruct->Class1CletJobs));

	if(statsStruct->Class1CletJobsMean<0.0){
		fprintf(stderr,"negative statsStruct->Class1CletJobsMean");
		exit(EXIT_FAILURE);
	}
	statsStruct->Class1CletJobsMeanLastUpdate=statsStruct->universalTime;
}

void updateClass2CletJobsMean(stats_struct *statsStruct){
	wellfordSamplePath(&(statsStruct->Class2CletJobsMean),statsStruct->universalTime,statsStruct->Class2CletJobsMeanLastUpdate,(double)(statsStruct->Class2CletJobs));

	if(statsStruct->Class2CletJobsMean<0.0){
		fprintf(stderr,"negative statsStruct->Class2CletJobsMean");
		exit(EXIT_FAILURE);
	}
	statsStruct->Class2CletJobsMeanLastUpdate=statsStruct->universalTime;
}

void updateClass1JobsMean(stats_struct *statsStruct){
	wellfordSamplePath(&(statsStruct->Class1JobsMean),statsStruct->universalTime,statsStruct->Class1JobsMeanLastUpdate,(double)(statsStruct->Class1CletJobs+statsStruct->Class1CJobs));

	if(statsStruct->Class1JobsMean<0.0){
		fprintf(stderr,"negative statsStruct->Class1JobsMean");
		exit(EXIT_FAILURE);
	}
	statsStruct->Class1JobsMeanLastUpdate=statsStruct->universalTime;
}

void updateClass1CJobsMean(stats_struct *statsStruct){
	wellfordSamplePath(&(statsStruct->Class1CJobsMean),statsStruct->universalTime,statsStruct->Class1CJobsMeanLastUpdate,(double)(statsStruct->Class1CJobs));
	if(statsStruct->Class1CJobsMean<0){
		fprintf(stderr,"negative statsStruct->Class1CJobsMean");
		exit(EXIT_FAILURE);
	}
	statsStruct->Class1CJobsMeanLastUpdate=statsStruct->universalTime;
}

void updateClass2CJobsMean(stats_struct *statsStruct){
	wellfordSamplePath(&(statsStruct->Class2CJobsMean),statsStruct->universalTime,statsStruct->Class2CJobsMeanLastUpdate,(double)(statsStruct->Class2CJobs));
	if(statsStruct->Class2CJobsMean<0){
		fprintf(stderr,"negative statsStruct->Class2CJobsMean");
		exit(EXIT_FAILURE);
	}
	statsStruct->Class2CJobsMeanLastUpdate=statsStruct->universalTime;
}

void updateClass2JobsMean(stats_struct *statsStruct){
	wellfordSamplePath(&(statsStruct->Class2JobsMean),statsStruct->universalTime,statsStruct->Class2JobsMeanLastUpdate,(double)(statsStruct->Class2CletJobs+statsStruct->Class2CJobs));
	if(statsStruct->Class2JobsMean<0){
		fprintf(stderr,"negative statsStruct->Class2JobsMean");
		exit(EXIT_FAILURE);
	}
	statsStruct->Class2JobsMeanLastUpdate=statsStruct->universalTime;
}

void updateJobsMean(stats_struct *statsStruct){
	wellfordSamplePath(&(statsStruct->JobsMean),statsStruct->universalTime,statsStruct->JobsMeanLastUpdate,(double)(statsStruct->Class1CJobs+statsStruct->Class2CJobs+statsStruct->Class1CletJobs+statsStruct->Class2CletJobs));
	if(statsStruct->JobsMean<0){
		fprintf(stderr,"negative statsStruct->JobsMean");
		exit(EXIT_FAILURE);
	}
	statsStruct->JobsMeanLastUpdate=statsStruct->universalTime;
}

void updateInterarrival(settings_struct settingsStruct,stats_struct *statsStruct,fdRun_struct fdRunStruct){
	double interarrival = statsStruct->universalTime-statsStruct->LastArrivalTime;
	if(interarrival<0){
		fprintf(stderr,"negative interarrival");
		exit(EXIT_FAILURE);
	}
	if(settingsStruct.printinters==1){
		if(dprintf(fdRunStruct.fdInterarrivalTemp,"%f\n",interarrival)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
	}
	statsStruct->LastArrivalTime=statsStruct->universalTime;
}

void updateInterdeparture(settings_struct settingsStruct,stats_struct *statsStruct,fdRun_struct fdRunStruct){
	double interdeparture = statsStruct->universalTime-statsStruct->LastDepartureTime;
	if(interdeparture<0){
		fprintf(stderr,"negative interdeparture");
		exit(EXIT_FAILURE);
	}
	if(settingsStruct.printinters==1){
		if(dprintf(fdRunStruct.fdInterdepartureTemp,"%f\n",interdeparture)<=0){ //POSIX standard since 2008.
			fprintf(stderr,"Error: dprintf didn't print");
		}
	}
	statsStruct->LastDepartureTime=statsStruct->universalTime;
}

void wellfordOnePass(double* mean,double newValue,int counter){
	*mean=*mean+(newValue-*mean)/(double)(counter);
}

void wellfordSamplePath(double* mean,double currentTime,double lastUpdate,double newValue){
	double timeElapsedUpdate = (currentTime)-(lastUpdate);
	if(timeElapsedUpdate<0){
		fprintf(stderr,"negative timeElapsedUpdate");
		exit(EXIT_FAILURE);
	}
	*mean=*mean+((timeElapsedUpdate/currentTime)*(newValue-*mean));
}

double getWellfordOnePass(double mean,double newValue,int counter){
	return mean+(newValue-mean)/(double)(counter);
}

double getWellfordSamplePath(double mean,double currentTime,double lastUpdate,double newValue){
	double timeElapsedUpdate = (currentTime)-(lastUpdate);
	if(timeElapsedUpdate<0){
		fprintf(stderr,"negative timeElapsedUpdate");
		exit(EXIT_FAILURE);
	}
	return mean+((timeElapsedUpdate/currentTime)*(newValue-mean));
}

void printFinalInformations(uintmax_t elapsedSeconds,uintmax_t elapsedUSeconds,struct tm lt,fd_struct fdStruct,settings_struct settingsStruct){

	if(
		dprintf(fdStruct.fdGeneralInfo,"Starting %d-%d-%d %d:%d:%d\n", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec)<=0||
		dprintf(fdStruct.fdGeneralInfo,"Starting simulation.\n You may want to change some setting. You'll find default (modifiable) settings in 'basic.h'.\n  Current settings are:\n")<=0||
		dprintf(fdStruct.fdGeneralInfo,"sampletime:%f (sampling interval)\n",settingsStruct.sampletime)<=0||
		dprintf(fdStruct.fdGeneralInfo,"arrivalmean1:%f (interarrival1 mean)\n", settingsStruct.arrivalmean1)<=0||
		dprintf(fdStruct.fdGeneralInfo,"arrivalmean2:%f (interarrival2 mean)\n", settingsStruct.arrivalmean2)<=0||
		dprintf(fdStruct.fdGeneralInfo,"servicemean cloudlet class 1:%f \n",settingsStruct.servicemeanclet1)<=0||
		dprintf(fdStruct.fdGeneralInfo,"servicemean cloudlet class 2:%f \n",settingsStruct.servicemeanclet2)<=0||
		dprintf(fdStruct.fdGeneralInfo,"servicemean cloud class 1:%f \n",settingsStruct.servicemeanc1)<=0||
		dprintf(fdStruct.fdGeneralInfo,"servicemean cloud class 2:%f \n",settingsStruct.servicemeanc2)<=0||
		dprintf(fdStruct.fdGeneralInfo,"migration delay:%f \n",settingsStruct.migrationdelay)<=0||
		dprintf(fdStruct.fdGeneralInfo,"simulationlength:%f \n",settingsStruct.simulationlength)<=0||
		dprintf(fdStruct.fdGeneralInfo,"Capacity:%d \n",settingsStruct.capacity)<=0||
		dprintf(fdStruct.fdGeneralInfo,"Threshold:%d \n",settingsStruct.threshold)<=0||
		dprintf(fdStruct.fdGeneralInfo,"Seed:%d \n",settingsStruct.seed)<=0||
		dprintf(fdStruct.fdGeneralInfo,"joinstream class1:%d \n",settingsStruct.arrivalstream1)<=0||
		dprintf(fdStruct.fdGeneralInfo,"joinstream class 2:%d \n",settingsStruct.arrivalstream2)<=0||
		dprintf(fdStruct.fdGeneralInfo,"leavestream cloudlet class1:%d \n",settingsStruct.leavestreamclet1)<=0||
		dprintf(fdStruct.fdGeneralInfo,"leavestream cloudlet class2:%d \n",settingsStruct.leavestreamclet2)<=0||
		dprintf(fdStruct.fdGeneralInfo,"leavestream cloud class1:%d \n",settingsStruct.leavestreamc1)<=0||
		dprintf(fdStruct.fdGeneralInfo,"leavestream cloud class 2:%d \n",settingsStruct.leavestreamc2)<=0||
		dprintf(fdStruct.fdGeneralInfo,"migration delay stream:%d \n",settingsStruct.migrationdelaystream)<=0||
		dprintf(fdStruct.fdGeneralInfo,"start to print after:%f seconds\n",settingsStruct.starttoprint)<=0||
		dprintf(fdStruct.fdGeneralInfo,"final value files:%d (1/0 activated/deactivated), it only prints the last value of each statistic \n",settingsStruct.finalvaluefiles)<=0||
		dprintf(fdStruct.fdGeneralInfo,"~~~~~~~~~~~~~ Simulation Terminated ~~~~~~~~~~~~~\n")<=0||
		dprintf(fdStruct.fdGeneralInfo,"Elapsed time:%ju s and %ju ms\n",elapsedSeconds,elapsedUSeconds)<=0
	){ //POSIX standard since 2008.
		fprintf(stderr,"Error: dprintf didn't print");
	}
}

void getElapsedTime(struct timeval timenowafter,struct timeval timenow,uintmax_t *elapsedSeconds,uintmax_t *elapsedUSeconds){
	if((timenowafter.tv_sec < timenow.tv_sec)||((timenowafter.tv_sec == timenow.tv_sec)&&(timenowafter.tv_usec < timenow.tv_usec)))
		fprintf(stderr,"Errore: negative elapsed time!");
	uintmax_t intmax= ((timenowafter.tv_sec-timenow.tv_sec)*1000) + (timenowafter.tv_usec-timenow.tv_usec)/1000;
	*elapsedSeconds= (intmax/1000);
	*elapsedUSeconds=(intmax%1000);
}

void closeFiles(fd_struct fdStruct){

	if(close(fdStruct.fdThroughputMean)<0||close(fdStruct.fdResponseTimeMean)<0||close(fdStruct.fdClass1ResponseTimeMean)<0||close(fdStruct.fdClass2ResponseTimeMean)<0||close(fdStruct.fdClass1CletJobsMean)<0||close(fdStruct.fdClass2CletJobsMean)<0||close(fdStruct.fdClass1JobsMean)<0||
			close(fdStruct.fdClass1CJobsMean)<0||close(fdStruct.fdClass2CJobsMean)<0||close(fdStruct.fdClass2JobsMean)<0||close(fdStruct.fdJobsMean)<0||
			close(fdStruct.fdUtilizationMean)<0||close(fdStruct.fdJoinedUsers)<0||close(fdStruct.fdLeavedUsers)<0||close(fdStruct.fdGeneralInfo)<0){
		perror("Error closing files");
		exit(EXIT_FAILURE);
	}
}


void closeFinalValueFiles(fdFinalValue_struct fdFinalValueStruct){
	if(close(fdFinalValueStruct.fdThroughputMeanFinal)<0||close(fdFinalValueStruct.fdResponseTimeMeanFinal)<0||close(fdFinalValueStruct.fdClass1ResponseTimeMeanFinal)<0||close(fdFinalValueStruct.fdClass2ResponseTimeMeanFinal)<0||close(fdFinalValueStruct.fdClass1CletJobsMeanFinal)<0||
			close(fdFinalValueStruct.fdClass2CletJobsMeanFinal)<0||close(fdFinalValueStruct.fdClass1JobsMeanFinal)<0||close(fdFinalValueStruct.fdClass1CJobsMeanFinal)<0||
			close(fdFinalValueStruct.fdClass2CJobsMeanFinal)<0||close(fdFinalValueStruct.fdClass2JobsMeanFinal)<0||close(fdFinalValueStruct.fdJobsMeanFinal)<0||
			close(fdFinalValueStruct.fdUtilizationMeanFinal)<0||close(fdFinalValueStruct.fdJoinedUsersFinal)<0||close(fdFinalValueStruct.fdLeavedUsersFinal)<0){
		perror("Error closing final value files");
		exit(EXIT_FAILURE);
	}
}

void closeRunFiles(fdRun_struct fdRunStruct){
	if(close(fdRunStruct.fdThroughputMeanTemp)<0||close(fdRunStruct.fdResponseTimeMeanTemp)<0||close(fdRunStruct.fdClass1ResponseTimeMeanTemp)<0||close(fdRunStruct.fdClass2ResponseTimeMeanTemp)<0||close(fdRunStruct.fdClass1CletJobsMeanTemp)<0||close(fdRunStruct.fdClass2CletJobsMeanTemp)<0||
		close(fdRunStruct.fdClass1JobsMeanTemp)<0||close(fdRunStruct.fdClass1CJobsMeanTemp)<0||close(fdRunStruct.fdClass2CJobsMeanTemp)<0||close(fdRunStruct.fdClass2JobsMeanTemp)<0||close(fdRunStruct.fdJobsMeanTemp)<0||
		close(fdRunStruct.fdUtilizationMeanTemp)<0||close(fdRunStruct.fdJoinedUsersTemp)<0||close(fdRunStruct.fdLeavedUsersTemp)<0||close(fdRunStruct.fdArrivalFreqTemp)<0||close(fdRunStruct.fdThroughputTemp)<0||
		close(fdRunStruct.fdInterarrivalTemp)<0||close(fdRunStruct.fdInterdepartureTemp)<0){
		perror("Error closing run files");
		exit(EXIT_FAILURE);
	}
}

void initRunOutputFiles(int iteration,folder_struct folderStruct,fdRun_struct *fdRunStruct){

	int len1;
	int len2=1;

	int iterationTemp=iteration;
	while (iterationTemp){
      		iterationTemp /= 10;
      		len2++;
    	}

	if(1){
		len1=strlen("/throughputmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/throughputmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathThroughputMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdThroughputMeanTemp = open(strcat(strcat(filepath,folderStruct.pathThroughputMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdThroughputMeanTemp<0){
		perror("open: Error creating file ThroughputMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/responsetimemean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/responsetimemean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathResponseTimeMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdResponseTimeMeanTemp = open(strcat(strcat(filepath,folderStruct.pathResponseTimeMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdResponseTimeMeanTemp<0){
		perror("open: Error creating file ResponseTimeMean");
		exit(EXIT_FAILURE);
	}
	if(1){
		len1=strlen("/class1responsetimemean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class1responsetimemean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass1ResponseTimeMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdClass1ResponseTimeMeanTemp = open(strcat(strcat(filepath,folderStruct.pathClass1ResponseTimeMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdClass1ResponseTimeMeanTemp<0){
		perror("open: Error creating file Class1ResponseTimeMean");
		exit(EXIT_FAILURE);
	}
	if(1){
		len1=strlen("/class2responsetimemean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class2responsetimemean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass2ResponseTimeMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdClass2ResponseTimeMeanTemp = open(strcat(strcat(filepath,folderStruct.pathClass2ResponseTimeMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdClass2ResponseTimeMeanTemp<0){
		perror("open: Error creating file Class2ResponseTimeMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class1cletjobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class1cletjobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass1CletJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdClass1CletJobsMeanTemp = open(strcat(strcat(filepath,folderStruct.pathClass1CletJobsMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdClass1CletJobsMeanTemp<0){
		perror("open: Error creating file Class1CletJobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class2cletjobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class2cletjobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass2CletJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdClass2CletJobsMeanTemp = open(strcat(strcat(filepath,folderStruct.pathClass2CletJobsMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdClass2CletJobsMeanTemp<0){
		perror("open: Error creating file Class2CletJobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class1jobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class1jobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass1JobsMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdClass1JobsMeanTemp = open(strcat(strcat(filepath,folderStruct.pathClass1JobsMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdClass1JobsMeanTemp<0){
		perror("open: Error creating file Class1JobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class1cjobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class1cjobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass1CJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdClass1CJobsMeanTemp = open(strcat(strcat(filepath,folderStruct.pathClass1CJobsMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdClass1CJobsMeanTemp<0){
		perror("open: Error creating file Class1CJobsMean");
		exit(EXIT_FAILURE);
	}


	if(1){
		len1=strlen("/class2cjobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class2cjobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass2CJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdClass2CJobsMeanTemp = open(strcat(strcat(filepath,folderStruct.pathClass2CJobsMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdClass2CJobsMeanTemp<0){
		perror("open: Error creating file Class2CJobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class2jobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class2jobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass2JobsMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdClass2JobsMeanTemp = open(strcat(strcat(filepath,folderStruct.pathClass2JobsMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdClass2JobsMeanTemp<0){
		perror("open: Error creating file Class2JobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/jobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/jobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdJobsMeanTemp = open(strcat(strcat(filepath,folderStruct.pathJobsMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdJobsMeanTemp<0){
		perror("open: Error creating file JobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/utilizationmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/utilizationmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathUtilizationMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdUtilizationMeanTemp = open(strcat(strcat(filepath,folderStruct.pathUtilizationMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdUtilizationMeanTemp<0){
		perror("open: Error creating file UtilizationMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/joinedusers.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/joinedusers%d.csv",iteration);
		char filepath[strlen(folderStruct.pathJoinedUsers)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdJoinedUsersTemp = open(strcat(strcat(filepath,folderStruct.pathJoinedUsers),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdJoinedUsersTemp<0){
		perror("open: Error creating file JoinedUsers");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/leavedusers.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/leavedusers%d.csv",iteration);
		char filepath[strlen(folderStruct.pathLeavedUsers)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdLeavedUsersTemp = open(strcat(strcat(filepath,folderStruct.pathLeavedUsers),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdLeavedUsersTemp<0){
		perror("open: Error creating file LeavedUsers");
		exit(EXIT_FAILURE);
	}
	if(1){
		len1=strlen("/arrivalfreq.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/arrivalfreq%d.csv",iteration);
		char filepath[strlen(folderStruct.pathArrivalFreq)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdArrivalFreqTemp = open(strcat(strcat(filepath,folderStruct.pathArrivalFreq),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdArrivalFreqTemp<0){
		perror("open: Error creating file ArrivalFreq");
		exit(EXIT_FAILURE);
	}
	if(1){
		len1=strlen("/throughput.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/throughput%d.csv",iteration);
		char filepath[strlen(folderStruct.pathThroughput)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdThroughputTemp = open(strcat(strcat(filepath,folderStruct.pathThroughput),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdThroughputTemp<0){
		perror("open: Error creating file Throughput");
		exit(EXIT_FAILURE);
	}
	if(1){
		len1=strlen("/interdeparture.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/interdeparture%d.csv",iteration);
		char filepath[strlen(folderStruct.pathInterdeparture)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdInterdepartureTemp = open(strcat(strcat(filepath,folderStruct.pathInterdeparture),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdInterdepartureTemp<0){
		perror("open: Error creating file Interdeparture");
		exit(EXIT_FAILURE);
	}
	if(1){
		len1=strlen("/interarrival.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/interarrival%d.csv",iteration);
		char filepath[strlen(folderStruct.pathInterarrival)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdInterarrivalTemp = open(strcat(strcat(filepath,folderStruct.pathInterarrival),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdInterarrivalTemp<0){
		perror("open: Error creating file Interarrival");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/migrationpercentagemean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/migrationpercentagemean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathMigrationPercentageMean)+len1+len2+1];
		filepath[0]='\0';
		fdRunStruct->fdMigrationPercentageMeanTemp = open(strcat(strcat(filepath,folderStruct.pathMigrationPercentageMean),pfilename), O_WRONLY|O_CREAT|O_TRUNC, 0600);
	}
	if(fdRunStruct->fdMigrationPercentageMeanTemp<0){
		perror("open: Error creating file MigrationPercentageMean");
		exit(EXIT_FAILURE);
	}
	
}

void	mergeRunFiles(settings_struct settingsStruct,folder_struct folderStruct,fd_struct fdStruct){
	
	int z=settingsStruct.iteration;
	int iteration;
	off_t offset[14][z];
	int8_t isFinished[14][z];
	uintmax_t localcounter[14]={0};
	uintmax_t globalcounter = 0;
	memset(offset,0,14*z*sizeof(off_t));
	memset(isFinished,0,14*z*sizeof(int8_t));
	size_t dim = 16;
	char *lineptr;
	lineptr = malloc(dim);
	if(lineptr==NULL){
		perror("Error malloc *lineptr");
		exit(EXIT_FAILURE);
	}
	
	
	while(globalcounter<14){
		for(iteration=0;iteration<z;iteration++){
			fptr_struct fptrStruct;
		
			openOnlyReadFiles(iteration,folderStruct,&fptrStruct);

				if(z<=0){
					fprintf(stderr,"zero iterations aren't allowed");				
				}

			//read a line from the statistic run file and write the same line in the "global" statistic file.
			readAndWrite(z,&(localcounter[0]),&(isFinished[0][iteration]),fptrStruct.fptrThroughputMean,
					fdStruct.fdThroughputMean,&(offset[0][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[1]),&(isFinished[1][iteration]),fptrStruct.fptrResponseTimeMean,
					fdStruct.fdResponseTimeMean,&(offset[1][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[2]),&(isFinished[2][iteration]),fptrStruct.fptrClass1ResponseTimeMean,
					fdStruct.fdClass1ResponseTimeMean,&(offset[2][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[3]),&(isFinished[3][iteration]),fptrStruct.fptrClass2ResponseTimeMean,
					fdStruct.fdClass2ResponseTimeMean,&(offset[3][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[4]),&(isFinished[4][iteration]),fptrStruct.fptrClass1CletJobsMean,
					fdStruct.fdClass1CletJobsMean,&(offset[4][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[5]),&(isFinished[5][iteration]),fptrStruct.fptrClass2CletJobsMean,
					fdStruct.fdClass2CletJobsMean,&(offset[5][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[6]),&(isFinished[6][iteration]),fptrStruct.fptrClass1JobsMean,
					fdStruct.fdClass1JobsMean,&(offset[6][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[7]),&(isFinished[7][iteration]),fptrStruct.fptrClass1CJobsMean,
					fdStruct.fdClass1CJobsMean,&(offset[7][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[8]),&(isFinished[8][iteration]),fptrStruct.fptrClass2CJobsMean,
					fdStruct.fdClass2CJobsMean,&(offset[8][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[9]),&(isFinished[9][iteration]),fptrStruct.fptrClass2JobsMean,
					fdStruct.fdClass2JobsMean,&(offset[9][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[10]),&(isFinished[10][iteration]),fptrStruct.fptrJobsMean,
					fdStruct.fdJobsMean,&(offset[10][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[11]),&(isFinished[11][iteration]),fptrStruct.fptrUtilizationMean,
					fdStruct.fdUtilizationMean,&(offset[11][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[12]),&(isFinished[12][iteration]),fptrStruct.fptrJoinedUsers,
					fdStruct.fdJoinedUsers,&(offset[12][iteration]),&lineptr,&dim,&globalcounter);

			readAndWrite(z,&(localcounter[13]),&(isFinished[13][iteration]),fptrStruct.fptrLeavedUsers,
					fdStruct.fdLeavedUsers,&(offset[13][iteration]),&lineptr,&dim,&globalcounter);




			closeOnlyReadFiles(fptrStruct);



		}

		addNewLine(localcounter[0],z,fdStruct.fdThroughputMean);
		addNewLine(localcounter[1],z,fdStruct.fdResponseTimeMean);
		addNewLine(localcounter[2],z,fdStruct.fdClass1ResponseTimeMean);
		addNewLine(localcounter[3],z,fdStruct.fdClass2ResponseTimeMean);
		addNewLine(localcounter[4],z,fdStruct.fdClass1CletJobsMean);
		addNewLine(localcounter[5],z,fdStruct.fdClass2CletJobsMean);
		addNewLine(localcounter[6],z,fdStruct.fdClass1JobsMean);
		addNewLine(localcounter[7],z,fdStruct.fdClass1CJobsMean);
		addNewLine(localcounter[8],z,fdStruct.fdClass2CJobsMean);
		addNewLine(localcounter[9],z,fdStruct.fdClass2JobsMean);
		addNewLine(localcounter[10],z,fdStruct.fdJobsMean);
		addNewLine(localcounter[11],z,fdStruct.fdUtilizationMean);
		addNewLine(localcounter[12],z,fdStruct.fdJoinedUsers);
		addNewLine(localcounter[13],z,fdStruct.fdLeavedUsers);

	}
	free(lineptr);
}

void addNewLine(uintmax_t localcounter, int z, int fd){
	if(localcounter<(uintmax_t)z){
		if(dprintf(fd,"\n")<=0){
			fprintf(stderr,"Error: dprintf didn't print");
		}
	}

}


void openOnlyReadFiles(int iteration,folder_struct folderStruct,fptr_struct *fptrStruct){

	int len1;
	int len2=1;

	int iterationTemp=iteration;
	while (iterationTemp){
      		iterationTemp /= 10;
      		len2++;
    	}

	if(1){
		len1=strlen("/throughputmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/throughputmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathThroughputMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrThroughputMean = fopen(strcat(strcat(filepath,folderStruct.pathThroughputMean),pfilename), "r");
	}
	if(fptrStruct->fptrThroughputMean==NULL){
		perror("fopen: Error opening file ThroughputMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/responsetimemean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/responsetimemean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathResponseTimeMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrResponseTimeMean = fopen(strcat(strcat(filepath,folderStruct.pathResponseTimeMean),pfilename), "r");
	}
	if(fptrStruct->fptrResponseTimeMean==NULL){
		perror("fopen: Error opening file ResponseTimeMean");
		exit(EXIT_FAILURE);
	}	

	if(1){
		len1=strlen("/class1responsetimemean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class1responsetimemean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass1ResponseTimeMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrClass1ResponseTimeMean = fopen(strcat(strcat(filepath,folderStruct.pathClass1ResponseTimeMean),pfilename), "r");
	}
	if(fptrStruct->fptrClass1ResponseTimeMean==NULL){
		perror("fopen: Error opening file Class1ResponseTimeMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class2responsetimemean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class2responsetimemean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass2ResponseTimeMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrClass2ResponseTimeMean = fopen(strcat(strcat(filepath,folderStruct.pathClass2ResponseTimeMean),pfilename), "r");
	}
	if(fptrStruct->fptrClass2ResponseTimeMean==NULL){
		perror("fopen: Error opening file Class2ResponseTimeMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class1cletjobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class1cletjobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass1CletJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrClass1CletJobsMean = fopen(strcat(strcat(filepath,folderStruct.pathClass1CletJobsMean),pfilename), "r");
	}
	if(fptrStruct->fptrClass1CletJobsMean==NULL){
		perror("fopen: Error opening file Class1CletJobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class2cletjobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class2cletjobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass2CletJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrClass2CletJobsMean = fopen(strcat(strcat(filepath,folderStruct.pathClass2CletJobsMean),pfilename), "r");
	}
	if(fptrStruct->fptrClass2CletJobsMean==NULL){
		perror("fopen: Error opening file Class2CletJobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class1jobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class1jobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass1JobsMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrClass1JobsMean = fopen(strcat(strcat(filepath,folderStruct.pathClass1JobsMean),pfilename), "r");
	}
	if(fptrStruct->fptrClass1JobsMean==NULL){
		perror("fopen: Error opening file Class1JobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class1cjobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class1cjobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass1CJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrClass1CJobsMean = fopen(strcat(strcat(filepath,folderStruct.pathClass1CJobsMean),pfilename), "r");
	}
	if(fptrStruct->fptrClass1CJobsMean==NULL){
		perror("fopen: Error opening file Class1CJobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class2cjobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class2cjobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass2CJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrClass2CJobsMean = fopen(strcat(strcat(filepath,folderStruct.pathClass2CJobsMean),pfilename), "r");
	}
	if(fptrStruct->fptrClass2CJobsMean==NULL){
		perror("fopen: Error opening file Class2CJobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/class2jobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/class2jobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathClass2JobsMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrClass2JobsMean = fopen(strcat(strcat(filepath,folderStruct.pathClass2JobsMean),pfilename), "r");
	}
	if(fptrStruct->fptrClass2JobsMean==NULL){
		perror("fopen: Error opening file Class2JobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/jobsmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/jobsmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathJobsMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrJobsMean = fopen(strcat(strcat(filepath,folderStruct.pathJobsMean),pfilename), "r");
	}
	if(fptrStruct->fptrJobsMean==NULL){
		perror("fopen: Error opening file JobsMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/utilizationmean.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/utilizationmean%d.csv",iteration);
		char filepath[strlen(folderStruct.pathUtilizationMean)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrUtilizationMean = fopen(strcat(strcat(filepath,folderStruct.pathUtilizationMean),pfilename), "r");
	}
	if(fptrStruct->fptrUtilizationMean==NULL){
		perror("fopen: Error opening file UtilizationMean");
		exit(EXIT_FAILURE);
	}

	if(1){
		len1=strlen("/joinedusers.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/joinedusers%d.csv",iteration);
		char filepath[strlen(folderStruct.pathJoinedUsers)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrJoinedUsers = fopen(strcat(strcat(filepath,folderStruct.pathJoinedUsers),pfilename), "r");
	}
	if(fptrStruct->fptrJoinedUsers==NULL){
		perror("fopen: Error opening file JoinedUsers");
		exit(EXIT_FAILURE);
	}	

	if(1){
		len1=strlen("/leavedusers.csv");
		char pfilename[len1+len2+1];
		snprintf(pfilename,len1+len2+1,"/leavedusers%d.csv",iteration);
		char filepath[strlen(folderStruct.pathLeavedUsers)+len1+len2+1];
		filepath[0]='\0';
		fptrStruct->fptrLeavedUsers = fopen(strcat(strcat(filepath,folderStruct.pathLeavedUsers),pfilename), "r");
	}
	if(fptrStruct->fptrLeavedUsers==NULL){
		perror("fopen: Error opening file LeavedUsers");
		exit(EXIT_FAILURE);
	}

	return;

}

void closeOnlyReadFiles(fptr_struct fptrStruct){
	if(fclose(fptrStruct.fptrThroughputMean)!=0||fclose(fptrStruct.fptrResponseTimeMean)!=0||
			fclose(fptrStruct.fptrClass1ResponseTimeMean)!=0||fclose(fptrStruct.fptrClass2ResponseTimeMean)!=0||
			fclose(fptrStruct.fptrClass1CletJobsMean)!=0){
		perror("fclose Error closing run files");
		exit(EXIT_FAILURE);
	}
	if(fclose(fptrStruct.fptrClass2CletJobsMean)!=0||fclose(fptrStruct.fptrClass1JobsMean)!=0||
			fclose(fptrStruct.fptrClass1CJobsMean)!=0||fclose(fptrStruct.fptrClass2CJobsMean)!=0||
			fclose(fptrStruct.fptrClass2JobsMean)!=0||fclose(fptrStruct.fptrJobsMean)!=0||
			fclose(fptrStruct.fptrUtilizationMean)!=0||fclose(fptrStruct.fptrJoinedUsers)!=0||
			fclose(fptrStruct.fptrLeavedUsers)!=0){
		perror("fclose Error closing run files");
		exit(EXIT_FAILURE);
	}

}


void readAndWrite(int z,uintmax_t *localcounter,int8_t *isFinished,FILE *fptr,int fd,off_t *offset,char **lineptr,size_t *dim,uintmax_t *globalcounter){
	ssize_t temp=0;
	char *lineptrTemp=*lineptr;
	memset(*lineptr,0,*dim);

	if(*localcounter<(uintmax_t)z){
		if(*isFinished==0){
			if(fseeko(fptr,*offset,SEEK_SET)<0){
				perror("Error fseek *fptr");
				exit(EXIT_FAILURE);
			}

			temp = getdelim(lineptr,dim,(int)',',fptr);
			//printf("leggo");
			if(temp<0){
				if(feof(fptr)){
					//printf("%ju,",localcounter[0]);
					*isFinished=1;
					if((*localcounter)++>=(uintmax_t)z-1){
					//	printf("GLOBAL COUNTER:%ju-",globalcounter);
						(*globalcounter)++;
					}		
				}
				else{
					perror("Error getline *fptrUseMean");
					exit(EXIT_FAILURE);
				}
			}
			else if(**lineptr=='\n'){
				if(*((*lineptr)+sizeof(char))=='\0'){
					//printf("entrato");
					//printf("%ju,",localcounter[0]);
					*isFinished=1;
					if((*localcounter)++>=(uintmax_t)z-1){
					//	printf("GLOBAL COUNTER:%ju-",globalcounter);
						(*globalcounter)++;
					}
				}
				else{
					//printf("entrato2");
					lineptrTemp=(*lineptr)+sizeof(char);
					*offset=(*offset)+temp;
				}	
			}
			else{
				*offset=(*offset)+temp;
			}
			clearerr(fptr);
		}

		if(*isFinished==0 /*&& **lineptr!='\n'*/){		
			if(dprintf(fd,"%s",lineptrTemp)<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}
		}
		else{
			if(dprintf(fd,",")<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}		
		}

		temp=0;
		memset(*lineptr,0,*dim);
		lineptrTemp=*lineptr;
		if(*isFinished==0 /*&& **lineptr!='\n'*/){		
			temp = getdelim(lineptr,dim,(int)',',fptr);
			//printf("leggo");
			if(temp<0){
				if(feof(fptr)){
					//printf("%ju,",localcounter[0]);
					*isFinished=1;
					if((*localcounter)++>=(uintmax_t)z-1){
					//	printf("GLOBAL COUNTER:%ju-",globalcounter);
						(*globalcounter)++;
					}		
				}
				else{
					perror("Error getline *fptrUseMean");
					exit(EXIT_FAILURE);
				}
			}
			else if(**lineptr=='\n'){
				if(*((*lineptr)+sizeof(char))=='\0'){
					//printf("entrato");
					//printf("%ju,",localcounter[0]);
					*isFinished=1;
					if((*localcounter)++>=(uintmax_t)z-1){
					//	printf("GLOBAL COUNTER:%ju-",globalcounter);
						(*globalcounter)++;
					}
				}
				else{
					//printf("entrato2");
					lineptrTemp=(*lineptr)+sizeof(char);
					*offset=(*offset)+temp;
				}	
			}
			else{
				*offset=(*offset)+temp;
			}
			clearerr(fptr);
		}	
		

		if(*isFinished==0 /*&& **lineptr!='\n'*/){		
			if(dprintf(fd,"%s",lineptrTemp)<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}
		}
		else{
			if(dprintf(fd,",")<=0){ //POSIX standard since 2008.
				fprintf(stderr,"Error: dprintf didn't print");
			}		
		}
	}
}

void addC1departure(list* mylist, stats_struct statsStruct, settings_struct settingsStruct){
	double *TimePtr = (double*)malloc(sizeof(double));
	double *ListInsertionTimePtr = (double*)malloc(sizeof(double));
	char *TypePtr = (char*)malloc(sizeof(char));
	*TypePtr='L';
	SelectStream(settingsStruct.leavestreamc1);
	*TimePtr=(statsStruct.universalTime)+Exponential(settingsStruct.servicemeanc1);
	*ListInsertionTimePtr=statsStruct.universalTime;
	list_put(mylist,TimePtr,TypePtr,ListInsertionTimePtr,NULL);
}

void addClet1departure(list *mylist, stats_struct statsStruct, settings_struct settingsStruct){
	double *TimePtr = (double*)malloc(sizeof(double));
	double *ListInsertionTimePtr = (double*)malloc(sizeof(double));
	char *TypePtr = (char*)malloc(sizeof(char));
	*TypePtr='D';
	SelectStream(settingsStruct.leavestreamclet1);
	*TimePtr=(statsStruct.universalTime)+Exponential(settingsStruct.servicemeanclet1);
	*ListInsertionTimePtr=statsStruct.universalTime;
	list_put(mylist,TimePtr,TypePtr,ListInsertionTimePtr,NULL);
}

void addArrival1(list* mylist, stats_struct statsStruct, settings_struct settingsStruct){
	double *TimePtr = (double*)malloc(sizeof(double));
	double *ListInsertionTimePtr = (double*)malloc(sizeof(double));
	char *TypePtr = (char*)malloc(sizeof(char));
	*TypePtr='A';
	SelectStream(settingsStruct.arrivalstream1);
	*TimePtr=(statsStruct.universalTime)+Exponential(settingsStruct.arrivalmean1);
	*ListInsertionTimePtr=statsStruct.universalTime;
	list_put(mylist,TimePtr,TypePtr,ListInsertionTimePtr,NULL);
}

void addC2departure(list *mylist, stats_struct statsStruct, settings_struct settingsStruct){
	double *TimePtr = (double*)malloc(sizeof(double));
	double *ListInsertionTimePtr = (double*)malloc(sizeof(double));
	char *TypePtr = (char*)malloc(sizeof(char));
	*TypePtr='l';
	SelectStream(settingsStruct.leavestreamc2);
	*TimePtr=(statsStruct.universalTime)+Exponential(settingsStruct.servicemeanc2);
	*ListInsertionTimePtr=statsStruct.universalTime;
	list_put(mylist,TimePtr,TypePtr,ListInsertionTimePtr,NULL);
}

void addClet2departure(list *mylist, stats_struct statsStruct, settings_struct settingsStruct){
	double *TimePtr = (double*)malloc(sizeof(double));
	double *ListInsertionTimePtr = (double*)malloc(sizeof(double));
	char *TypePtr = (char*)malloc(sizeof(char));
	*TypePtr='d';
	SelectStream(settingsStruct.leavestreamclet2);
	*TimePtr=(statsStruct.universalTime)+Exponential(settingsStruct.servicemeanclet2);
	*ListInsertionTimePtr=statsStruct.universalTime;
	list_put(mylist,TimePtr,TypePtr,ListInsertionTimePtr,NULL);
}

void addArrival2(list *mylist, stats_struct statsStruct, settings_struct settingsStruct){
	double *TimePtr = (double*)malloc(sizeof(double));
	double *ListInsertionTimePtr = (double*)malloc(sizeof(double));
	char *TypePtr = (char*)malloc(sizeof(char));
	*TypePtr='a';
	SelectStream(settingsStruct.arrivalstream2);
	*TimePtr=(statsStruct.universalTime)+Exponential(settingsStruct.arrivalmean2);
	*ListInsertionTimePtr=statsStruct.universalTime;
	list_put(mylist,TimePtr,TypePtr,ListInsertionTimePtr,NULL);
}

void migrateClass2FromCletToC(list *mylist,settings_struct settingsStruct, stats_struct statsStruct){
	double **tTimePtr=(double**)malloc(sizeof(double*));
	double **tListInsertionTimePtr=(double**)malloc(sizeof(double*));
	double **tListMigrationTimePtr=(double**)malloc(sizeof(double*));
	char **tTypePtr=(char**)malloc(sizeof(char*));

	double *MigrationTimePtr = (double*)malloc(sizeof(double)); 
	*MigrationTimePtr = statsStruct.universalTime;



	if(tTimePtr==NULL || tTypePtr==NULL || tListInsertionTimePtr==NULL){
		fprintf(stderr,"Error:malloc tTimePtr or tTypePtr returned null.");
	}
	memset(tTimePtr,0,sizeof(double*));
	memset(tListInsertionTimePtr,0,sizeof(double*));
	memset(tListMigrationTimePtr,0,sizeof(double*));
	memset(tTypePtr,0,sizeof(char*));

	//#####removing the youngest class 2 job from the cloudlet (the departure event)#####
	if(list_pop_max_by_value2(mylist,tTimePtr,tTypePtr,tListInsertionTimePtr,tListMigrationTimePtr,'d')!=0){
		if(*tTimePtr==NULL || *tTypePtr==NULL || *tListInsertionTimePtr==NULL ){
			fprintf(stderr,"Error: main: Unexpected NULL pointers return from list_pop_max_by_value2");
			exit(EXIT_FAILURE);
		}
		**tTypePtr='l';
		SelectStream(settingsStruct.leavestreamc2);
		**tTimePtr=(statsStruct.universalTime)+Exponential(settingsStruct.servicemeanc2);
		SelectStream(settingsStruct.migrationdelaystream);
		**tTimePtr=(**tTimePtr)+Exponential(settingsStruct.migrationdelay);

		//printf("%f \n",**tListInsertionTimePtr);
		
//######Adding the previously removed class 2 job in the cloud (adding the departure event)######
		list_put(mylist,*tTimePtr,*tTypePtr,*tListInsertionTimePtr,MigrationTimePtr);
		free(tTimePtr);
		free(tTypePtr);
		free(tListInsertionTimePtr);
		free(tListMigrationTimePtr);
	
	}
	else{
		fprintf(stderr,"Error: main: Unexpected error from list_pop_by_value2");
		exit(EXIT_FAILURE);
	}


}



