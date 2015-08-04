#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "GeometricGraph.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

#define DEFAULT_VIEWER_MODE false
#define DEFAULT_SAVE_IMAGES true
#define DEFAULT_SENSING_RANGE 0.4
#ifndef DEFAULT_BELT_WIDTH
#define DEFAULT_BELT_WIDTH 10.0
#endif
#define DEFAULT_BELT_HEIGHT 5.0
#define MAX_SENSING_RANGE 2.5
#define MAX_BELT_WIDTH 100.0
#define MAX_BELT_HEIGHT 50.0
#define DEFAULT_SENSOR_COUNT 10
#define MAX_SENSOR_COUNT 100
#define DEFAULT_VERTICAL_VELOCITY_THRESHOLD (_sensingRange)
#define DEFAULT_HORIZONTAL_VELOCITY_THRESHOLD (_sensingRange)
#define VERTICAL_PERMISSIBLE_LIMIT (_sensingRange / 60.0)
#define HORIZONTAL_PERMISSIBLE_LIMIT (_sensingRange / 60.0)
// #define VERTICAL_PERMISSIBLE_LIMIT 0.001
// #define HORIZONTAL_PERMISSIBLE_LIMIT 0.001

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720
#define MAX_RECTANGLE_WIDTH 1120
#define MAX_RECTANGLE_HEIGHT 600
#define RECTANGLE_START_X 40
#define RECTANGLE_START_Y 110
#define LINE1 20
#define LINE2 40
#define DISPLAY_WINDOW "Display Window"

using namespace std;
using namespace cv;

class Simulator
{
private:
	double _sensingRange;
	double _beltWidth;
	double _beltHeight;
	int _simulationCount;			// Number of Simulations Carried out
	int _simulationIterationCount;
	bool _viewerMode;				// If true then a delay will be there for easy visibility
	bool _saveImages;
	int _sensorCount;				// Number of sensor in current Simulation
	double _horizontalVelocityThreshold;
	double _verticalVelocityThereshold;
	vector<Coordinate> _coordinates;
	double _avgSensorMovement;
	vector<double> _horizontalVelocities;
	vector<double> _verticalVelocities;
	GeometricGraph _geometricGraph;
	//OpenCV data for image Generation
	Mat _belt;
	double _rectangleRatio;
	int _rectangleHeight;
	int _rectangleWidth;
	int _imageCount;
	char _simulationDirectory[300];

	// Internal ForceModel Private Functions
	double horizontalVelocity(double, double);
	double verticalVelocity(double);
	void clearVelocities();
	void loopForces();
	void moveSensors();
	void initImage();
	void drawSensors();
	void displayProperties();
	void generateAndDisplaySensorsAndData();
	void connectEdge(const Coordinate& first, const Coordinate& second);
	void appendGraphToImage();
	void saveImage();
	void generateAndSaveVideo();
public:
	Simulator();
	~Simulator();
	void randomizeSensors();
	void setCoordinates(const vector<Coordinate>&);
	void simulate();
	bool isStable();


	void setSensingRange(double);
	double getSensingRange();
	void setBeltWidth(double);
	double getBeltWidth();
	void setBeltHeight(double);
	double getBeltHeight();
	void setSensorCount(int);
	int getSensorCount();
	void setHorizontalVelocityThreshold(double);
	double getHorizontalVelocityThreshold();
	void setVerticalVelocityThreshold(double);
	double getVerticalVelocityThreshold();
	void setViewerMode(bool);
	void saveImages(bool);
	int getSimulationCount();
	double getQuality();
	double getNumberOfCriticalRegions();
	double getAverageSensorMovement();
	int getNumberOfIterations();
};


#endif
