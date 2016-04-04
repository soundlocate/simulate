#ifndef _COMMANDLINEOPTIONS_H
#define _COMMANDLINEOPTIONS_H

#include <string>
#include <argp.h>

#include <iostream>
#include <cassert>

#include "CSVTable.h"
#include "config.h"

class CommandLineOptions {
public:
	CommandLineOptions(int argc, char ** argv) {
		argp_parse(&argp, argc, argv, 0, 0, &args);
	}

    unsigned short audioPort() {
		return args.audioPort;
	}

    unsigned short guiPort() {
		return args.guiPort;
	}

	unsigned int samplerate() {
		return args.samplerate;
	}

	bool log() {
		return args.log;
	}

	char * logfilename() {
		return args.logfilename;
	}

	int micCount() {
		return args.micCount;
	}

	double * mics() {
		return args.mics;
	}
private:

	// documentation of the program
	std::string doc = "soundSimulate -- simulate multiple sound sources and microphones\v"
		"AUDIOPORT\tport to open the audio server on\n"
		"GUIPORT\t\tport to open the gui server on\n";

	// documentation of non option arguments
    char args_doc[255] = "AUDIOPORT GUIPORT";

	// supported options (no custom options for now)
    struct argp_option options[5] = {
		{"samplerate", 's', "COUNT", 0, "samplerate"},
		{"soundfile", 'f', "FILE", 0, "soundfile to use"},
		{"logfile", 'l', "FILE", 0, "if set write positions to set logfile"},
		{"positionfile", 'p', "FILE", 0, "filename of micfile"},
		{ 0 }
	};

	// struct for holding the argument info
	struct arguments
	{
		unsigned int samplerate = 192000;
		double * mics;
		unsigned int micCount = 8;
		bool log;
		char * logfilename;
		char * soundfile;
		unsigned short audioPort;
		unsigned short guiPort;

		arguments() {
			mics = new double[micCount * 3];
			double distBetween = 0.42;
			int i = 0;

			mics[i++] = 0;
			mics[i++] = 0;
			mics[i++] = 0;

			mics[i++] = 0;
			mics[i++] = distBetween;
			mics[i++] = 0;

			mics[i++] = distBetween;
			mics[i++] = 0;
			mics[i++] = 0;

			mics[i++] = distBetween;
			mics[i++] = distBetween;
			mics[i++] = 0;

			mics[i++] = 0;
			mics[i++] = 0;
			mics[i++] = distBetween;

			mics[i++] = distBetween;
			mics[i++] = 0;
			mics[i++] = distBetween;

			mics[i++] = 0;
			mics[i++] = distBetween;
			mics[i++] = distBetween;

			mics[i++] = distBetween;
			mics[i++] = distBetween;
			mics[i++] = distBetween;
		}
	};

	// parsing of a single argument
	static error_t parse_opt (int key, char *arg, struct argp_state *state) {

		// get the struct for saving the arguments
		arguments * arguments = (struct arguments *) state->input;

		switch (key)
		{
		case 's':
			try {
				arguments->samplerate = std::stoi(arg);
			} catch(std::invalid_argument) {
				std::cout << "invalid samplerate " << arg << std::endl;
				argp_usage(state);
			}
			break;
		case 'f':
			arguments->soundfile = arg;
			break;
		case 'l':
			arguments->log = true;
			arguments->logfilename = arg;
			break;
		case 'p': {
      std::cout << "reading micsfile " << arg << std::endl; 

      CSVTable t(arg);
			auto rows = t.getRows();
			arguments->micCount = rows.size();
			arguments->mics = new double[rows.size() * 3];
			int i = 0;

			try {
				for(auto row : rows) {
					assert(row.size() == 3);

					arguments->mics[i++] = std::stod(row[0]);
					arguments->mics[i++] = std::stod(row[1]);
					arguments->mics[i++] = std::stod(row[2]);
				}
			} catch(std::invalid_argument) {
				std::cout << "invalid positionfile " << arg << std::endl;
				argp_usage(state);
			}

      std::cout << "read micsfile " << arg << std::endl; 

			break;
		}
		case ARGP_KEY_ARG: /* non option argument */
            // check if more than all needed arguments are passed (and abort if so)
			if (state->arg_num >= 2) {
				argp_usage(state);
			}

			// parse the arguments
			try {
				switch (state->arg_num) {
				case 0: /* audioport */
					arguments->audioPort = std::stoi(arg);
					break;
				case 1: /* guiport */
					arguments->guiPort = std::stoi(arg);
					break;
				}
			} catch(std::invalid_argument) {
				std::cout << "invalid port: " << arg << std::endl;
				argp_usage(state);
			}

			break;

		case ARGP_KEY_END: /* end of arguments */
			// check for too few non option arguments
			if (state->arg_num < 2) {
				argp_usage(state);
			}
			break;

		default:
			return ARGP_ERR_UNKNOWN;
		}
		return 0;
	}

	// argp parser definiton
	struct argp argp = { options, parse_opt, args_doc, doc.c_str() };

	arguments args;
};

#endif
