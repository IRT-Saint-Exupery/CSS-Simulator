
#include <cstring>
#include <csignal>
#include "FE_flagsconfig.h"
#include "FE_util.h"
#include "FE_error.h"

using namespace std;

#define PEAK_PORT		7373

const char *flagsFilename = nullptr;
bool peakSpaceSideFlag = false;
bool peakGroundSideFlag = false;
char peakIpAdress[512];

bool attack17Flag = false;
bool attack16Flag = false;

bool tcVerboseFlag = false;

SOCKADDR_IN peakAddr;

static bool peakAdressFlag = false;

void sigHandler (int sigNum)
{
	if (sigNum == SIGUSR1) {
		readFlagsConfiguration (flagsFilename);
	}
}

void installSigHandler ()
{
	signal (SIGUSR1, sigHandler);
}

void buildPeakConnection(const char *ipAdress)
{
    peakAdressFlag = false;

	memset(&peakAddr, 0, sizeof(peakAddr));
	peakAddr.sin_family = AF_INET;
	peakAddr.sin_port = htons(PEAK_PORT);
	if (inet_pton(AF_INET, ipAdress, &peakAddr.sin_addr) <= 0) {
		feWarning("bad adress format %s", ipAdress);
	}
	else {
		peakAdressFlag = true;
	}
}

void parseFlagsConfigurationFile (const char *filename)
{
	FILE *fileIn = fopen (filename, "r");
	if (fileIn == NULL) {
		feWarning ("Unable to open flag configuration file (%s)", filename);
		return;
	}


	char *ret;
	char line[512];
	int	flag;
	int nItems;

	while (!feof(fileIn)) {
		ret = fgets(line, sizeof(line), fileIn);
		if (ret == nullptr || lineToIgnore(line)) {
		}
		else if ((nItems = sscanf (line, "PEAK_SPACE_SIDE %d",
					&flag)) == 1) {
			peakSpaceSideFlag = flag;
		}
		else if ((nItems = sscanf (line, "PEAK_GROUND_SIDE %d",
						&flag)) == 1) {
				peakGroundSideFlag = flag;
		}
		else if ((nItems = sscanf (line, "ATTACK_17 %d",
						&flag)) == 1) {
				attack17Flag = flag;
		}
		else if ((nItems = sscanf (line, "ATTACK_16 %d",
						&flag)) == 1) {
				attack16Flag = flag;
		}
		else if ((nItems = sscanf (line, "TC_VERBOSE %d",
						&flag)) == 1) {
				tcVerboseFlag = flag;
		}
		else if ((nItems = sscanf (line, "PEAK_IP_ADRESS %s",
						peakIpAdress)) == 1) {
				buildPeakConnection(peakIpAdress);
		}
		else {
			feWarning ("Flag configuration line not parsed |%s|", line);
		}
	}
	fclose (fileIn);
}

static void checkFlagsConfiguration ()
{
	if ((peakSpaceSideFlag || peakGroundSideFlag) && ! peakAdressFlag ) {
		feWarning ("No ip adress specified for peaking");
		peakSpaceSideFlag = false;
		peakGroundSideFlag = false;
	}
	feInfo ("Current flag configuration\n");
	feInfo ("PEAK_SPACE_SIDE	%d", peakSpaceSideFlag);
	feInfo ("PEAK_GROUND_SIDE	%d", peakGroundSideFlag);
	feInfo ("PEAK_IP_ADRESS	%s", (peakAdressFlag)? peakIpAdress: "NA");
	feInfo ("ATTACK_16	%d", attack16Flag);
	feInfo ("ATTACK_17	%d", attack17Flag);

}

void readFlagsConfiguration (const char *filename)
{
	flagsFilename = filename;
	parseFlagsConfigurationFile (filename);
	checkFlagsConfiguration ();
}


