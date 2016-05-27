#pragma once

#include <iostream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace cv;
using namespace std;

class Finder
{
public:
	Finder();
	vector<Point> findCorners(string &object, string &scene);
	void findCornersAndDisplay(string &object, string &scene, bool displayMatches);

	vector<Point> test();

};