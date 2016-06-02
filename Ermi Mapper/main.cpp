#include <stdio.h>
#include <iostream>

#include "Finder.h"
#include "gdal_priv.h"
#include "cpl_conv.h"
using namespace std;
#include "opencv2/core/core.hpp"
#include "boost/program_options.hpp"
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
Finder finder;

bool parseArguments(int argc, char** argv);

int main(int argc, char** argv)
{
	if (parseArguments(argc, argv))
	{
		//TODO


		//wyswietlenie efektow znalezienia
		finder.findCornersAndDisplay(object, refmap, true);



		//Dobrze by bylo zebys w tym miejscu sprawdzil czy wszystkie 3 sciezki sa poprawne (czy pliki sa, lub czy ich nie ma)
		GDALAllRegister();

		poDataset = (GDALDataset*)GDALOpen("map_full.tif", GA_ReadOnly);
		if (poDataset == nullptr)
		{
			cout << "Error Dataset\n";
			return -1;
		}
		poDataset->GetGeoTransform(t);
		cout << "Szerokosc: " << poDataset->GetRasterXSize(); //Rozmiar
		cout << "\nWysokosc: " << poDataset->GetRasterYSize();
		cout << "\nLiczba pasm: " << poDataset->GetRasterCount(); //Pasma
		cout << "\nProjekcja:\n" << poDataset->GetProjectionRef(); //Projekcja
		cout << "\nParametry transformacji: " << t[0] << ", " << t[1] << ", " << t[2]	//Parametry transformacji afinicznej
			<< ", " << t[3] << ", " << t[4] << ", " << t[5];
		//0 i 3 to lewy gorny rog obrazka
		cout << "\nSterownik: " << poDataset->GetDriver()->GetDescription(); //Sterownik
		poDriver = poDataset->GetDriver();
		cout << "\n\n";
		int xx = 1000000;
		int yy = 1000000;
		//pobranie kolejnych punktow wspolrzednych malego obrazka w duzym
		vector<Point> points = finder.findCorners(object, refmap);
		for (int i = 0; i < 4; i++)
		{
			//cout << "Point: " << points[i] << std::endl;
			//cout << points[i].x << " " << points[i].y << endl;
			if ((points[i].x <= xx) && (points[i].y <= yy)) {
				xx = points[i].x;
				yy = points[i].y;

				Xp = t[0] + points[i].x*t[1] + points[i].y*t[2];
				Yp = t[3] + points[i].x*t[4] + points[i].y*t[5];
				//wspolrzedne rogów malego obrazka na prawdziwej mapie
				cout << "x: " << Xp << " y: " << Yp << endl;
			}
		}

		
		//tempDataset = (GDALDataset*)GDALOpen(object.c_str(), GA_ReadOnly);
		//if (poDataset == nullptr)
		//{
		//	cout << "Error Dataset\n";
		//	return -1;
		//}
		tempDataset = (GDALDataset*)GDALOpen(object.c_str(), GA_ReadOnly);
		if (poDataset == nullptr)
		{
			cout << "Error Dataset\n";
			return -1;
		}
		
		cout << "x: " << Xp << " y: " << Yp << "P: " << xx << " L: " << yy << endl;

		destDataset = poDriver->CreateCopy(dest.c_str(), tempDataset, FALSE, NULL, NULL, NULL);
		//GDALClose(tempDataset);
		t[0] = Xp;
		t[3] = Yp;
		//t[2] = (Xp - xx*t[1] - t[0]) / yy;
		//t[4] = (Yp - yy*t[5] - t[3]) / xx;
		destDataset->SetGeoTransform(t);
		//destDataset->GetGeoTransform(t);
		destDataset->SetProjection(poDataset->GetProjectionRef());
		cout << "Szerokosc: " << destDataset->GetRasterXSize(); //Rozmiar
		cout << "\nWysokosc: " << destDataset->GetRasterYSize();
		cout << "\nLiczba pasm: " << destDataset->GetRasterCount(); //Pasma
		cout << "\nProjekcja:\n" << destDataset->GetProjectionRef(); //Projekcja
		//GDALSetProjection(poDataset->GetProjectionRef());
		cout << "\nParametry transformacji: " << t[0] << ", " << t[1] << ", " << t[2]	//Parametry transformacji afinicznej
			<< ", " << t[3] << ", " << t[4] << ", " << t[5];
		
		GDALClose(destDataset);
		GDALClose(poDataset);		

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

	po::options_description desc("Options");
	desc.add_options()
		("help,h", "print help messages")
		("object,o", po::value<std::string>(), "path to image with no georeferences")
		("refmap,r", po::value<std::string>(), "path to map with georeferences, should contain object")
		("dest,d", po::value<std::string>(), "path to result")
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

	if (vm.count("dest"))
		dest = vm["dest"].as<std::string>();
	else
	{
		std::cout << "Dest was not set.\n";
		return 0;
	}
	return 1;
}