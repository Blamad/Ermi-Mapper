#include <stdio.h>
#include <iostream>

#include "Finder.h"
#include "gdal_priv.h"
#include "cpl_conv.h"
#include <string>
#include "gdal_utils.h"
#include <Windows.h>

#include "opencv2/core/core.hpp"
#include "boost/program_options.hpp"

using namespace std;

GDALDataset *poDataset;
GDALDataset *destDataset;
GDALDataset *tempDataset;
GDALRasterBand *rasterBand;
GDALDriver  *poDriver;

double t[6];
int Xp = 1, Yp = 1, L = 0, P = 0;

std::string object;
std::string refmap;
std::string dest;

bool displayMatches;

Finder finder;

bool parseArguments(int argc, char** argv);

int main(int argc, char** argv)
{
	if (parseArguments(argc, argv))
	{

		//pobranie kolejnych punktow wspolrzednych malego obrazka w duzym i ewentualne wyswietlenie wynikow
		vector<Point> points = finder.findCorners(object, refmap, displayMatches);

		//Dobrze by bylo zebys w tym miejscu sprawdzil czy wszystkie 3 sciezki sa poprawne (czy pliki sa, lub czy ich nie ma)
		GDALAllRegister();
		poDataset = (GDALDataset*)GDALOpen(refmap.c_str(), GA_ReadOnly);
		if (poDataset == nullptr)
		{
			cout << "Error Dataset\n";
			return -1;
		}
		poDataset->GetGeoTransform(t);
		poDriver = poDataset->GetDriver();
		string argv="gdal_translate -of ";
		argv += poDataset->GetDriver()->GetDescription();
		argv.append(" ");
		tempDataset = (GDALDataset*)GDALOpen(object.c_str(), GA_ReadOnly);
		if (poDataset == nullptr)
		{
			cout << "Error Dataset\n";
			return -1;
		}
		for (int i = 0; i < 4; i++)
		{
			if (points[i].x >= 0 && points[i].y >= 0) {
			Xp = t[0] + points[i].x*t[1] + points[i].y*t[2];
			Yp = t[3] + points[i].x*t[4] + points[i].y*t[5];
			if (i == 0)
				argv.append("-gcp 0 0 " + to_string(Xp) + " " + to_string(Yp));
			if (i == 1)
				argv.append(" -gcp " + to_string(tempDataset->GetRasterXSize()) + " 0 " + to_string(Xp) + " " + to_string(Yp));
			if (i == 2)
				argv.append(" -gcp " + to_string(tempDataset->GetRasterXSize()) + " " + to_string(tempDataset->GetRasterYSize()) + " " + to_string(Xp) + " " + to_string(Yp));
			if (i == 3)
				argv.append(" -gcp 0 " + to_string(tempDataset->GetRasterYSize()) + " " + to_string(Xp) + " " + to_string(Yp));
		}
		}
		argv.append(" "+dest+" temp987654321.tif");

		destDataset = poDriver->CreateCopy(dest.c_str(), tempDataset, FALSE, NULL, NULL, NULL);
		GDALClose(tempDataset);
		destDataset->SetGeoTransform(t);
		destDataset->SetProjection(poDataset->GetProjectionRef());
		//WinExec(argv.c_str(), SW_HIDE);
		system(argv.c_str());		
		
		tempDataset = (GDALDataset*)GDALOpen("temp987654321.tif", GA_ReadOnly);
		if (poDataset == nullptr)
		{
			cout << "Error Dataset\n";
			return -1;
		}
		tempDataset->GetGeoTransform(t);
		destDataset->SetGeoTransform(t);
		
		destDataset->SetGCPs(tempDataset->GetGCPCount(),tempDataset->GetGCPs(),tempDataset->GetGCPProjection());
		destDataset->SetProjection(poDataset->GetProjectionRef());
		GDALClose(tempDataset);
		GDALClose(destDataset);
		GDALClose(poDataset);
		remove("temp987654321.tif");

		//TODO
		//dodanie referencji do pliczku i zapis wyniku pod sciezka dest
	}
	system("pause");
	return 0;
}

//Pobranie argumentow, 3 sa konieczne: object - maly obrazek, refmap - duza mapa, dest - zapis do
//Ustaw sobie dane w Properties projektu -> Debugging -> Command Arguments. Przyklad:
//-o map.jpg -r map_full.jpg -d dest.jpg
//-h
bool parseArguments(int argc, char** argv)
{
	namespace po = boost::program_options;

	//TODO
	//Dodaj opcjonalny argument -s pokazujacy wyniki porownywania i znajdowania map w sobie.

	po::options_description desc("Options");
	desc.add_options()
		("help,h", "print help messages")
		("object,o", po::value<std::string>(), "path to image with no georeferences")
		("refmap,r", po::value<std::string>(), "path to map with georeferences, should contain object")
		("dest,d", po::value<std::string>(), "path to result")
		("show,s", "show matching results")
		;

	po::variables_map vm;
	po::store(parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		
		std::cout << "Ermi Mapper is simple command-line program"
			<< " used for generating georeference to image being part of given source map."
			<< std::endl << std::endl;
		std::cout << desc << "\n";
		return 0;
	}

	if (vm.count("object"))
		object = vm["object"].as<std::string>();
	else
	{
		std::cout << "Object was not set.\n";
		return 0;
	}

	if (vm.count("refmap"))
		refmap = vm["refmap"].as<std::string>();
	else
	{
		std::cout << "Refmap was not set.\n";
		return 0;
	}

	if(vm.count("show"))
		displayMatches = true;
	else
		displayMatches = false;
	
	if (vm.count("dest"))
		dest = vm["dest"].as<std::string>();
	else
	{
		std::cout << "Dest was not set.\n";
		return 0;
	}
	return 1;
}