#include <stdio.h>
#include <iostream>

#include "Finder.h"

#include "opencv2/core/core.hpp"
#include "boost/program_options.hpp"

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
		//Dobrze by bylo zebys w tym miejscu sprawdzil czy wszystkie 3 sciezki sa poprawne (czy pliki sa, lub czy ich nie ma)

		//wyswietlenie efektow znalezienia
		//finder.findCornersAndDisplay(object, refmap, true);

		//pobranie kolejnych punktow wspolrzednych malego obrazka w duzym
		vector<Point> points = finder.findCorners(object, refmap);
		for (int i = 0; i < 4; i++)
		{
			cout << "Point: " << points[i] << std::endl;
		}

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