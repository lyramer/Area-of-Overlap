// Andy Wynden, V00746343

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct point_struct {
	double xcoord;
	double ycoord;
} point;

typedef struct rectangle_struct {
	point bottomleft;
	point bottomright;
	point topleft;
	point topright;
} rectangle;

typedef struct circle_struct {
	point origin;
	double radius;
} circle;

typedef struct distance_struct {
	double bottomleft;
	double bottomright;
	double topleft;
	double topright;
} distance;


point readPoint(int * exit_flag_point) {
	point temppoint;
	setvbuf(stdout, NULL, _IONBF, 0);
	fflush(stdout);
	if (0 == scanf("%lf", &temppoint.xcoord)) {*exit_flag_point = 1;}
	if (0 == scanf("%lf", &temppoint.ycoord)) {*exit_flag_point = 1;}
	return temppoint;
}


rectangle readRectangle(int * exit_flag) {
	int exit_flag_point = 0;
	rectangle rect1;
	rect1.bottomleft = readPoint(&exit_flag_point);
	rect1.topright = readPoint(&exit_flag_point);
	rect1.bottomright.xcoord = rect1.topright.xcoord;
	rect1.bottomright.ycoord = rect1.bottomleft.ycoord;
	rect1.topleft.xcoord = rect1.bottomleft.xcoord;
	rect1.topleft.ycoord = rect1.topright.ycoord;
	*exit_flag = exit_flag_point;
	return rect1;
}


circle readCircle() {
	int exit_flag_point;
	circle circ1;
	circ1.origin = readPoint(&exit_flag_point);
	fflush(stdout);
	scanf("%lf", &circ1.radius);
	return circ1;
}


double findDistance(point pointA, point pointB) {
	double distance = 0.0;
	distance = sqrt(pow(pointB.xcoord - pointA.xcoord, 2) + pow(pointB.ycoord - pointA.ycoord, 2));
	return distance;
}


point findEdge(circle circ1, rectangle rect1) { // FINDS RECTANGLE POINT CLOSEST TO CIRCLE'S CENTRE

	point nearestRectangleEdge;
	double temp1, temp2;

	// the following two functions take absolute value the long way since abs() rounds doubles and is therefore evil.
	if ((circ1.origin.ycoord - rect1.topright.ycoord) < 0) {
		temp1 = (circ1.origin.ycoord - rect1.topright.ycoord) * -1;
	} else {temp1 = (circ1.origin.ycoord - rect1.topright.ycoord);}

	if ((circ1.origin.ycoord - rect1.bottomleft.ycoord) < 0) {
		temp2 = (circ1.origin.ycoord - rect1.bottomleft.ycoord) * -1;
	} else {temp2 = (circ1.origin.ycoord - rect1.bottomleft.ycoord);}

	//Determine what is nearer to the circle center - the rectangle top edge or the rectangle bottom edge
    if (temp1 > temp2) {
    	nearestRectangleEdge.ycoord = rect1.bottomleft.ycoord;}
    else {nearestRectangleEdge.ycoord = rect1.topright.ycoord;}

    //Determine what is nearer to the circle center - the rectangle left edge or the rectangle right edge
    if (abs(circ1.origin.xcoord - rect1.topright.xcoord) > abs(circ1.origin.xcoord - rect1.bottomleft.xcoord)) {
    	nearestRectangleEdge.xcoord = rect1.bottomleft.xcoord;}
    else {nearestRectangleEdge.xcoord = rect1.topright.xcoord;}

    return nearestRectangleEdge;
}


double RoundToDecimal(double d){
    return round(d * 1000) / 1000;
}

double findMax(double paramA, double paramB) {
	if (paramB > paramA) {return paramB;}
	else {return paramA;}
}


double findMin(double paramA, double paramB) {
	if (paramB < paramA) {return paramB;}
	else {return paramA;}
}


// checks first if circle and rectangle overlaps, if yes, returns a 1
int overlap(circle circ1, rectangle rect1) { 
	int overlap = 0, xoverlap = 0, yoverlap = 0, xinsideflag = 0, yinsideflag = 0, insideflag = 0;
	double distance;

    //Stores the nearest vertex of the rectangle
    point nearestRectangleEdge;

	// FIRST CHECKS IF CIRCLE IS INSIDE THE RECTANGLE
	if (circ1.origin.xcoord <= rect1.topright.xcoord && circ1.origin.xcoord >= rect1.bottomleft.xcoord) {xinsideflag = 1;}
	if (circ1.origin.ycoord <= rect1.topright.ycoord && circ1.origin.ycoord >= rect1.bottomleft.ycoord) {yinsideflag = 1;}
	if (xinsideflag == 1 && yinsideflag == 1) {insideflag = 1;}

	// BASICALLY TREATS THE CIRCLE AS A SQUARE AND CHECKS WHETHER THERE IS OVERLAP BETWEEN THE SQUARE AND THE RECTANGLE
	if (insideflag == 0) {
		if (((circ1.origin.xcoord + circ1.radius) > rect1.bottomleft.xcoord) && ((circ1.origin.xcoord - circ1.radius) < rect1.topright.xcoord)) {xoverlap = 1;}
		if (((circ1.origin.ycoord + circ1.radius) > rect1.bottomleft.ycoord) && ((circ1.origin.ycoord - circ1.radius) < rect1.topright.ycoord)) {yoverlap = 1;}
	}

	nearestRectangleEdge = findEdge(circ1, rect1);

	// CALCULATES DISTANCE BETWEEN VERTEXES AND ORIGIN IF OVERLAP IS FOUND, BUT ORIGIN IS NOT INSIDE RECTANGLE
	if (xoverlap == 1 && yoverlap == 1 && insideflag == 0) {distance = findDistance(circ1.origin, nearestRectangleEdge);}

	if (insideflag == 1 || (xoverlap == 1 && yoverlap == 1 && distance < circ1.radius)) {overlap = 1;}

	return overlap;
}

//
/// BELOW FUNCTION FOR FINDING AREA OF OVERLAP
///
// Code for finding area adapted from C# application written by Emanuel Jobstl <emi@eex-dev.net>
// Link  : http://www.eex-dev.net/index.php?id=100
// The Basics:
// the function 'slices' the shape into rectangles (width is given by Resolution)
// the precision of area calculation and time it takes to execute can be modulated by increasing or decreasing Resolution.
// this function works only properly if the input is in the correct order - points for the rectangle are given bottom 
// left first, then top right. (I spent half an hour debugging an output of area = -0.0000...)
//
double IntersectionArea(rectangle rect1, circle circ1, point nearestRectangleEdge, int functoverlap) {
	double a = 0.0, Resolution = 0.0001, i, x; //Area, upperBound, lowerBound, Resolution of area calculation
	double upperBound, lowerBound, leftBound, rightBound;
	double tempboundary, circle_upper_temp, circle_lower_temp;
	upperBound = lowerBound = leftBound = rightBound = 0.0;

	//Check whether the rectangle lies completely outside of the circle.
	if (functoverlap == 0) {return 0;}

	if ((nearestRectangleEdge.ycoord - circ1.origin.ycoord) < 0) {
		tempboundary = (nearestRectangleEdge.ycoord - circ1.origin.ycoord) * -1.0;
	} else {tempboundary = (nearestRectangleEdge.ycoord - circ1.origin.ycoord);}

	// CALCULATES LEFT AND RIGHT BOUNDS OF THE TARGET AREA
	if (circ1.origin.ycoord <= rect1.topright.ycoord && circ1.origin.ycoord >= rect1.bottomleft.ycoord) { // IF CIRCLE CENTRE LIES WITHIN RECTANGLE
		leftBound = findMax((circ1.radius * -1.0) + circ1.origin.xcoord, rect1.bottomleft.xcoord);
		rightBound = findMin(circ1.radius + circ1.origin.xcoord, rect1.topright.xcoord);

	} else if (circ1.radius >= tempboundary) {
		//If the circle's center lies outside of the rectangle, we can choose optimal bounds.
		leftBound = findMax((sqrt(circ1.radius * circ1.radius - pow(nearestRectangleEdge.ycoord - circ1.origin.ycoord, 2)) + circ1.origin.xcoord) * -1, rect1.bottomleft.xcoord);
		rightBound = findMin(sqrt(circ1.radius * circ1.radius - pow(nearestRectangleEdge.ycoord - circ1.origin.ycoord, 2)) + circ1.origin.xcoord, rect1.topright.xcoord);
	}

	//Loop through the intersection area and sum up the area
	for (i = leftBound + Resolution; i <= rightBound; i += Resolution) {
		x = i - Resolution / 2.0;
		circle_upper_temp = (circ1.origin.ycoord + sqrt((circ1.radius * circ1.radius) - pow((x - circ1.origin.xcoord), 2)));
		circle_lower_temp  = (circ1.origin.ycoord - sqrt((circ1.radius * circ1.radius) - pow((x - circ1.origin.xcoord), 2)));
		upperBound = findMin(rect1.topright.ycoord, circle_upper_temp);
		lowerBound = findMax(rect1.bottomleft.ycoord, circle_lower_temp);
		a = a + ((lowerBound - upperBound) * Resolution);
	}
	RoundToDecimal(a);
	if ( a > 0 ) {return a;}
	else {return a*(-1.0000);}
}

int main(void) {

	while (1) {
		rectangle rect1;
		circle circ1;
		point nearestRectangleEdge;
		int olap, exit_flag = 0;
		double area;
		rect1 = readRectangle(&exit_flag);

		if (exit_flag == 1) {break;}
		circ1 = readCircle();
		olap = overlap(circ1, rect1);
		if (olap == 1) {
			nearestRectangleEdge = findEdge(circ1, rect1);
			area = IntersectionArea(rect1, circ1, nearestRectangleEdge, olap);
			printf("The circle and rectangle overlap with an area of: %lf\n", area);
		} else {printf("The circle and rectangle do not overlap.\n");}
	}
	printf("Exiting Application");
	return EXIT_SUCCESS;

}

