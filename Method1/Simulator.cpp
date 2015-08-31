#include "Simulator.hpp"
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace cv;

/*Constructors and Destructors*/
Simulator::Simulator()
{
	_sensingRange = DEFAULT_SENSING_RANGE;
	_beltWidth = DEFAULT_BELT_WIDTH;
	_beltHeight = DEFAULT_BELT_HEIGHT;
	_simulationCount = 0;
	_viewerMode = DEFAULT_VIEWER_MODE;
	_saveImages = DEFAULT_SAVE_IMAGES;
	_sensorCount = DEFAULT_SENSOR_COUNT;
	_horizontalVelocityThreshold = DEFAULT_VERTICAL_VELOCITY_THRESHOLD;
	_verticalVelocityThereshold = DEFAULT_HORIZONTAL_VELOCITY_THRESHOLD;
	_belt.create(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);
	_imageCount = 0;
	srand(time(NULL));
	randomizeSensors();
}

Simulator::~Simulator()
{}

#define REPULSIVE_WEIGHT (_beltWidth / (_sensorCount * _sensingRange) * 0.025)
#define ATTRACTIVE_WEIGHT 0.05
double Simulator::horizontalVelocity(double horizontalGap, double stableZone)
{
	double sign = 1.0;
	if(horizontalGap < 0.0)
	{
		sign = -1.0;
		horizontalGap = -horizontalGap;
	}
	double velocity;
	// Velocity will flip the direction at stableZone
	// INVERSE FUNCTION WRT GAP
	/*if(horizontalGap <= stableZone)
	{
		// repulsive force
		velocity = -1.0 * REPULSIVE_WEIGHT * (stableZone / horizontalGap - 1);
	}
	else
	{
		// attractive force
		velocity = ATTRACTIVE_WEIGHT * (horizontalGap / stableZone);
	}*/
	// LINEAR FUNCTION WRT GAP
	velocity = 10.0 * REPULSIVE_WEIGHT * (horizontalGap - stableZone);
	return sign * velocity;
}

#define VERTICAL_ATTRACTIVE_WEIGHT 0.5
double Simulator::verticalVelocity(double verticalGap)
{
	double sign = 1.0;
	if(verticalGap < 0.0)
	{
		sign = -1.0;
		verticalGap = -verticalGap;
	}
	return sign * VERTICAL_ATTRACTIVE_WEIGHT * verticalGap * verticalGap;
}

// TODO: Imageprocessing
void Simulator::initImage()
{
	// White Background
	_belt.setTo(Scalar(255,255,255));
	// Draw Rectangle
	if(((double)MAX_RECTANGLE_WIDTH/(double)MAX_RECTANGLE_HEIGHT) <= (_beltWidth/_beltHeight))
	{
		//width preference
		_rectangleHeight = (MAX_RECTANGLE_WIDTH * _beltHeight / _beltWidth);
		_rectangleWidth = MAX_RECTANGLE_WIDTH;
	}
	else
	{
		//height preference
		_rectangleHeight = MAX_RECTANGLE_HEIGHT;
		_rectangleWidth = (MAX_RECTANGLE_HEIGHT * _beltWidth / _beltHeight);
	}
	rectangle(_belt, Point(RECTANGLE_START_X,RECTANGLE_START_Y), 
		Point(RECTANGLE_START_X + _rectangleWidth, RECTANGLE_START_Y + _rectangleHeight),Scalar(0,0,0));
	putText(_belt,"(0,0)",Point(1,RECTANGLE_START_Y), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));
	char endVertexString[20];
	sprintf(endVertexString,"(%.2lf,%.2lf)",_beltWidth,_beltHeight);
	putText(_belt,endVertexString,Point(RECTANGLE_START_X + _rectangleWidth, RECTANGLE_START_Y + _rectangleHeight), FONT_HERSHEY_PLAIN, 1, Scalar(0,0,0));
}

void Simulator::drawSensors()
{
	int i;
	double heightScale = _rectangleHeight / _beltHeight;
	double widthScale = _rectangleWidth / _beltWidth;
	for(i = 0; i < _coordinates.size(); i++)
	{
		// Center of the sensor
		circle(_belt, Point(RECTANGLE_START_X + _coordinates[i].x * widthScale, RECTANGLE_START_Y + _coordinates[i].y * heightScale)
			, 1, Scalar(255,0,0), 2);
		// sensing range of the sensor
		circle(_belt, Point(RECTANGLE_START_X + _coordinates[i].x * widthScale, RECTANGLE_START_Y + _coordinates[i].y * heightScale)
			, _sensingRange * heightScale, Scalar(255,0,0), 1);
	}
}

void Simulator::displayProperties()
{
	/* Things to display are follows:
	Quality
	Average Sensor Movement
	Number Of sensors
	Number of critical regions
	*/
	char printString[100];
	sprintf(printString, "Quality : %lf", _geometricGraph.getQuality());
	putText(_belt,printString,Point(RECTANGLE_START_X, LINE1), FONT_HERSHEY_PLAIN, 1, Scalar(255,0,255));
	sprintf(printString, "Avg. Sensor Movement : %lf", getAverageSensorMovement());
	putText(_belt,printString,Point((IMAGE_WIDTH>>1), LINE1), FONT_HERSHEY_PLAIN, 1, Scalar(255,0,255));
	sprintf(printString, "Number of Sensors : %d", _sensorCount);
	putText(_belt,printString,Point(RECTANGLE_START_X, LINE2), FONT_HERSHEY_PLAIN, 1, Scalar(255,0,255));
	sprintf(printString, "Number of critical regions : %d", _geometricGraph.getNumberOfCriticalRegions());
	putText(_belt,printString,Point((IMAGE_WIDTH>>1), LINE2), FONT_HERSHEY_PLAIN, 1, Scalar(255,0,255));
}

void Simulator::generateAndDisplaySensorsAndData()
{
	initImage();
	drawSensors();
	displayProperties();
	namedWindow(DISPLAY_WINDOW, WINDOW_AUTOSIZE);
	imshow(DISPLAY_WINDOW, _belt);
	if(_viewerMode)
	{
		cv::waitKey(100);
	}
	saveImage();
}

void Simulator::connectEdge(const Coordinate& first, const Coordinate& second)
{
	double heightScale = _rectangleHeight / _beltHeight;
	double widthScale = _rectangleWidth / _beltWidth;
	line(_belt
		,Point(RECTANGLE_START_X + first.x * widthScale, RECTANGLE_START_Y + first.y * heightScale)
		,Point(RECTANGLE_START_X + second.x * widthScale, RECTANGLE_START_Y + second.y * heightScale)
		,Scalar(0,255,0));
}

void Simulator::appendGraphToImage()
{
	int i,j;
	vector<Coordinate> coordinates = _geometricGraph.getCoordinates();
	vector<vector<int> > graph = _geometricGraph.getGraph();
	int n = graph.size();
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < (int)graph[i].size(); j++)
		{
			// Connect edge on image
			connectEdge(coordinates[i],coordinates[graph[i][j]]);
		}
	}
	double heightScale = _rectangleHeight / _beltHeight;
	double widthScale = _rectangleWidth / _beltWidth;
	double verticalCenterOfMass = _geometricGraph.getVerticalCenterOfMass();
	line(_belt, Point(RECTANGLE_START_X ,RECTANGLE_START_Y + verticalCenterOfMass * heightScale)
		, Point(RECTANGLE_START_X + _rectangleWidth ,RECTANGLE_START_Y + verticalCenterOfMass * heightScale)
		, Scalar(0,0,255));
	namedWindow(DISPLAY_WINDOW, WINDOW_AUTOSIZE);
	imshow(DISPLAY_WINDOW, _belt);
	if(_viewerMode)
	{
		cv::waitKey(100);
	}
	saveImage();
}

void Simulator::saveImage()
{
	if(!_saveImages)
	{
		return;
	}
	_imageCount++;
	char imageName[20];
	sprintf(imageName,"img%4d.jpg",_imageCount);
	char fullImagePath[400];
	sprintf(fullImagePath,"%s/%s",_simulationDirectory,imageName);
	imwrite(fullImagePath,_belt);
}

void Simulator::generateAndSaveVideo()
{
	if(!_saveImages)
	{
		return;
	}
	char inputPath[400];
	sprintf(inputPath,"%s/img%%4d.jpg",_simulationDirectory);
	printf("InputPath : %s\n", inputPath);
	VideoCapture in_capture(inputPath);
	Mat img;
	char outputPath[50];
	sprintf(outputPath,"%s.avi",_simulationDirectory);
	printf("outputPath : %s\n", outputPath);
	VideoWriter out_capture(outputPath, CV_FOURCC('M','J','P','G'), 4, Size(IMAGE_WIDTH,IMAGE_HEIGHT));
	while (true)
	{
		in_capture >> img;
		if(img.empty())
			break;	
		namedWindow("video", WINDOW_AUTOSIZE);
		imshow("video", img);
		if(_viewerMode)
		{
			cv::waitKey(10);
		}
		out_capture.write(img);
	}
	out_capture.release();
}

void Simulator::randomizeSensors()
{
	_coordinates.clear();
	_coordinates.resize(_sensorCount);
	for(int i = 0; i < _sensorCount; i++)
	{
		_coordinates[i].x = (double)rand() / (double)RAND_MAX * _beltWidth;
		_coordinates[i].y = (double)rand() / (double)RAND_MAX * _beltHeight;
		//printf("%d %lf,%lf\n",i,_coordinates[i].x, _coordinates[i].y );
	}
	//printf("Size of coordinates vector here %ld\n", _coordinates.size());
	_geometricGraph.setCoordinates(_coordinates);
	generateAndDisplaySensorsAndData();
	appendGraphToImage();
}

void Simulator::setCoordinates(const vector<Coordinate>& inputCoordinates)
{
	//Initialze the Coordinates proper input present
	if(inputCoordinates.size() > 0)
	{
		_coordinates.assign(inputCoordinates.begin(), inputCoordinates.end());
		_geometricGraph.setCoordinates(_coordinates);
	}
}

void Simulator::clearVelocities()
{
	int i;
	_horizontalVelocities.resize(_coordinates.size());
	_verticalVelocities.resize(_coordinates.size());
	for(i = 0; i < _horizontalVelocities.size(); i++)
	{
		_horizontalVelocities[i] = _verticalVelocities[i] = 0.0;
	}
}

void Simulator::loopForces()
{
	vector<vector<int> > graph = _geometricGraph.getGraph();
	vector<Coordinate> coordinates = _geometricGraph.getCoordinates();
	_coordinates.assign(coordinates.begin(), coordinates.end());
	clearVelocities();
	// Apply Horizontal Forces on each coordinate while traversing the graph
	int i,j;
	for(i = 0; i < graph.size(); i++)
	{
		// Calculate velocity by all the adjcent vertices
		double currentHorizontalVelocity = 0.0;
		for(j = 0; j < graph[i].size(); j++)
		{
			currentHorizontalVelocity += horizontalVelocity((_coordinates[i].x - _coordinates[graph[i][j]].x), 2.0 * _sensingRange);
		}
		//printf("Current Horizontal Velocity %d = %lf\n", i , currentHorizontalVelocity);
		if(abs(currentHorizontalVelocity) <= _horizontalVelocityThreshold)
		{
			_horizontalVelocities[i] = currentHorizontalVelocity;
		}
		else
		{
			// Assign threshold with appropriate sign
			_horizontalVelocities[i] = (currentHorizontalVelocity < 0.0)? -1.0 * _horizontalVelocityThreshold : _horizontalVelocityThreshold;
		}
	}
	// Adding horizontal velocity for first and last coordinates due to barrier edges
	double leftBarrierHorizontalVelocity = horizontalVelocity((_coordinates[0].x - 0.0), _sensingRange);
	if(abs(leftBarrierHorizontalVelocity) <= _horizontalVelocityThreshold)
	{
		_horizontalVelocities[0] += leftBarrierHorizontalVelocity;
	}
	else
	{
		_horizontalVelocities[0] = (leftBarrierHorizontalVelocity < 0.0)? -1.0 * _horizontalVelocityThreshold : _horizontalVelocityThreshold;
	}
	double rightBarrierHorizontalVelocity = horizontalVelocity(_coordinates[_coordinates.size() - 1].x - _beltWidth, _sensingRange);
	if(abs(rightBarrierHorizontalVelocity) <= _horizontalVelocityThreshold)
	{
		_horizontalVelocities[_coordinates.size() - 1] += rightBarrierHorizontalVelocity;
	}
	else
	{
		_horizontalVelocities[_coordinates.size() - 1] = (rightBarrierHorizontalVelocity < 0.0)? -1.0 * _horizontalVelocityThreshold : _horizontalVelocityThreshold;
	}
	
	//Applying Vertical Forces
	double verticalCenterOfMass = _geometricGraph.getVerticalCenterOfMass();
	for(i = 0; i < _coordinates.size(); i++)
	{
		double currentVerticalVelocity = verticalVelocity(_coordinates[i].y - verticalCenterOfMass);
		if(abs(currentVerticalVelocity) <= _verticalVelocityThereshold)
		{
			_verticalVelocities[i] = currentVerticalVelocity;
		}
		else
		{
			_verticalVelocities[i] = (currentVerticalVelocity < 0.0)? -1.0 * _verticalVelocityThereshold : _verticalVelocityThereshold;
		}
	}
}

bool Simulator::isStable()
{
	// Checks if the horizontal and vertical velocities are within permissible limits
	// Check if current configuration is k-barrier covered
	//printf("Quality of the belt = %lf width = %lf\n", _geometricGraph.getQuality(), _beltWidth);
	if(_geometricGraph.getQuality() == _beltWidth)
	{
		return true;
	}
	int i;
	for(i = 1; i < _verticalVelocities.size() - 1; i++)
	{
		//printf("%d (%lf,%lf) -> (%lf,%lf) limits (%lf,%lf) thresholds (%lf, %lf)\n",i,_coordinates[i].x,_coordinates[i].y,_horizontalVelocities[i],_verticalVelocities[i]
		//,HORIZONTAL_PERMISSIBLE_LIMIT, VERTICAL_PERMISSIBLE_LIMIT ,_horizontalVelocityThreshold,_verticalVelocityThereshold);
		if(abs(_horizontalVelocities[i]) > HORIZONTAL_PERMISSIBLE_LIMIT || abs(_verticalVelocities[i]) > VERTICAL_PERMISSIBLE_LIMIT)
		{ return false; }
	}
	return true;
}

void Simulator::moveSensors()
{
	int i;
	double currentSensorMovement = 0.0;
	for(i = 0; i < _coordinates.size(); i++)
	{
		
		_coordinates[i].y -= _verticalVelocities[i];
		if((_coordinates[i].x - _horizontalVelocities[i]) <= 0.0 || (_coordinates[i].x - _horizontalVelocities[i]) >= _beltWidth)
		{
			currentSensorMovement += abs(_verticalVelocities[i]);
			continue;
		}
		_coordinates[i].x -= _horizontalVelocities[i];
		_movement[i] += sqrt(_verticalVelocities[i]*_verticalVelocities[i] + _horizontalVelocities[i]*_horizontalVelocities[i]);
	}
	_geometricGraph.setCoordinates(_coordinates);
}

void Simulator::simulate()
{
	if(_saveImages)
	{
		sprintf(_simulationDirectory,"simulation%d",_simulationCount);
		char directoryCreationCommand[400];
		sprintf(directoryCreationCommand,"mkdir %s",_simulationDirectory);
		system(directoryCreationCommand);
	}	
	_imageCount = 0;
	_simulationIterationCount = 0;
	_movement.resize(_coordinates.size());
	for(int i = 0; i < (int)_movement.size(); i++)
	{
		_movement[i] = 0.0;
	}
	while(true)
	{
		// Calculate forces
		generateAndDisplaySensorsAndData();
		appendGraphToImage();
		loopForces();
		// If not stable then move sensors
		if(!isStable())
		{
			moveSensors();
		}
		else
		{
			// Else break and stop simulation
			break;
		}
		_simulationIterationCount++;
	}
	generateAndSaveVideo();
	calculateHistogram();
	_simulationCount++;
}

void Simulator::calculateHistogram()
{
	// Calculate and print the histogram
	
}

void Simulator::setSensingRange(double sensingRange)
{
	if(sensingRange > 0.0 && _sensingRange <= MAX_SENSING_RANGE)
	{
		_sensingRange = sensingRange;
	}
	else
	{
		_sensingRange = DEFAULT_SENSING_RANGE;
	}
}

double Simulator::getSensingRange()
{ return _sensingRange; }

void Simulator::setBeltWidth(double beltWidth)
{
	if(beltWidth > 0.0 && beltWidth <= MAX_BELT_WIDTH)
	{
		_beltWidth = beltWidth;
	}
	else
	{
		_beltWidth = DEFAULT_BELT_WIDTH;
	}
}
double Simulator::getBeltWidth()
{ return _beltWidth; }

void Simulator::setBeltHeight(double beltHeight)
{
	if(beltHeight > 0.0 && beltHeight <= MAX_BELT_HEIGHT)
	{
		_beltHeight = beltHeight;
	}
	else
	{
		_beltHeight = DEFAULT_BELT_HEIGHT;
	}
}

double Simulator::getBeltHeight()
{ return _beltHeight; }

void Simulator::setSensorCount(int sensorCount)
{
	if(sensorCount > 0 && sensorCount <= MAX_SENSOR_COUNT)
	{
		_sensorCount = sensorCount;
	}
	else
	{
		_sensorCount = DEFAULT_SENSOR_COUNT;
	}
	randomizeSensors();
}

int Simulator::getSensorCount()
{ return _sensorCount; }

void Simulator::setHorizontalVelocityThreshold(double horizontalThreshold)
{
	_horizontalVelocityThreshold = horizontalThreshold;
}

double Simulator::getHorizontalVelocityThreshold()
{ return _horizontalVelocityThreshold; }

void Simulator::setVerticalVelocityThreshold(double verticalThreshold)
{
	_verticalVelocityThereshold = verticalThreshold;
}

double Simulator::getVerticalVelocityThreshold()
{ return _verticalVelocityThereshold; }

void Simulator::setViewerMode(bool viewerMode)
{
	_viewerMode = viewerMode;
}

void Simulator::saveImages(bool saveImages)
{
	_saveImages = saveImages;
}

int Simulator::getSimulationCount()
{ return _simulationCount; }

double Simulator::getQuality()
{ return _geometricGraph.getQuality(); }

double Simulator::getNumberOfCriticalRegions()
{ return _geometricGraph.getNumberOfCriticalRegions(); }

double Simulator::getAverageSensorMovement()
{ 
	double avgSensorMovement = 0.0;
	for(int i = 0; i < (int)_movement.size(); i++)
	{
		avgSensorMovement += _movement[i];
	}
	avgSensorMovement /= ((double)_movement.size());
	return avgSensorMovement;
}

int Simulator::getNumberOfIterations()
{ return _simulationIterationCount; }
