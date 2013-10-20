#include "Angel.h"
#include <iostream>
#include <string>
#include <fstream>

#define RANGE_MIN -0.75
#define RANGE_MAX 0.75
#define RANGE_CHANGE 0.05
#define WIN_HEIGHT 500
#define WIN_WIDTH 500

using namespace std;

string *arrayTabs;
double *arrayHighVal;
double *arrayLowVal;
double **arrayValues;
double **arrayNormVal;
double *arrayXValue;
double *arrayYValue;
string stringTemp;
int numTabs, numValues, valX = 0, valY = 1, numCount = 0, leftButtonDown = 0;
double numOpacity = 1, boxOpacity = 0;
vec4 *arrayAxis;
vec4 *arrayAxis2;
vec4 *arrayBox;
vec4 *currentPoints;
vec4 *currentPoints2;
vec4 *currentPoints2Lines;
vec4 *selectedBox;
vec4 *selectedLines;
vec4 *unSelectedLines;

int leftButtonDown2 = 0, indexSpot = 0;
double valueX = 0, valueY = 0, tempDifferenceValue = 0, differenceValue = 0, valueYMove = 0, valueYMoveTemp = 0;
int countTab = 0, countTabTemp = 0;
int spotLines = 0;

int window1, window2;

vec4 vColor;
GLuint vColorID;

double interpolate(double minA, double a, double maxA, double minB, double maxB)
{    
	double denominator = (maxA-minA);    
	return minB + ((a-minA) / denominator) * (maxB-minB);
}

void readFile(string fileName)
{
	ifstream in(fileName);
	if (in.bad())
	{
		cerr << "File " << fileName << " not found. Exiting." << endl;
		system("PAUSE");
	}
	in >> numTabs >> numValues;
	

	arrayTabs = new string[numTabs];
	getline(in, stringTemp);
	for (int i = 0; i < numTabs; i++)
	{
		getline(in, arrayTabs[i]);
	}

	arrayHighVal = new double[numTabs];
	arrayLowVal = new double[numTabs];
	arrayValues = new double*[numValues];
	arrayNormVal = new double*[numValues];
	for (int i = 0; i < numTabs; i++)
	{
		arrayHighVal[i] = 0;
		arrayLowVal[i] = 1e20;
	}
	for(int i = 0; i < numValues; ++i)
	{
		arrayValues[i] = new double[numTabs];
	}
	for(int i = 0; i < numValues; ++i)
	{
		arrayNormVal[i] = new double[numTabs];
	}
	for (int i = 0; i < numValues; i++)
	{
		for (int j = 0; j < numTabs; j++)
		{
			in >> arrayValues[i][j];
			if (arrayHighVal[j] < arrayValues[i][j])
			{
				arrayHighVal[j] = arrayValues[i][j];
			}
			if (arrayLowVal[j] > arrayValues[i][j])
			{
				arrayLowVal[j] = arrayValues[i][j];
			}
		}
	}

	for (int i = 0; i < numValues; i++)
	{
		for (int j = 0; j < numTabs; j++)
		{
			arrayNormVal[i][j] = interpolate(arrayLowVal[j], arrayValues[i][j], arrayHighVal[j], RANGE_MIN, RANGE_MAX);
		}
	}
	arrayAxis = new vec4[4];
	// X
	arrayAxis[0][0] = RANGE_MIN - RANGE_CHANGE;
	arrayAxis[0][1] = RANGE_MIN - RANGE_CHANGE;
	arrayAxis[0][2] = 0;
	arrayAxis[0][3] = 1;
	arrayAxis[1][0] = RANGE_MAX + RANGE_CHANGE;
	arrayAxis[1][1] = RANGE_MIN - RANGE_CHANGE;
	arrayAxis[1][2] = 0;
	arrayAxis[1][3] = 1;
	// Y
	arrayAxis[2][0] = RANGE_MIN - RANGE_CHANGE;
	arrayAxis[2][1] = RANGE_MIN - RANGE_CHANGE;
	arrayAxis[2][2] = 0;
	arrayAxis[2][3] = 1;
	arrayAxis[3][0] = RANGE_MIN - RANGE_CHANGE;
	arrayAxis[3][1] = RANGE_MAX + RANGE_CHANGE;
	arrayAxis[3][2] = 0;
	arrayAxis[3][3] = 1;

	double divideAxis = 1.5 / ((double)numTabs - 1);
	arrayAxis2 = new vec4[2 * numTabs];
	arrayBox = new vec4[8 * numTabs];
	double countAxis = 0.0, xSpot = 0;
	int countBox = 0;
	for (int i = 0; i < 2 * numTabs; i++)
	{
		if (i == 0 || (i % 2) == 0)
		{
			xSpot = -.75 + (countAxis * divideAxis);
			arrayAxis2[i] = vec4(xSpot, -.75, 0, 1);
			arrayAxis2[i + 1] = vec4(xSpot, .75, 0, 1);
			countAxis++;
			
			arrayBox[countBox] = vec4(xSpot - .05, -.80, 0, 1);
			arrayBox[countBox + 1] = vec4(xSpot - .05, -.75, 0, 1);
			arrayBox[countBox + 2] = vec4(xSpot + .05, -.75, 0, 1);
			arrayBox[countBox + 3] = vec4(xSpot + .05, -.80, 0, 1);

			arrayBox[countBox + 4] = vec4(xSpot - .05, .75, 0, 1);
			arrayBox[countBox + 5] = vec4(xSpot - .05, .80, 0, 1);
			arrayBox[countBox + 6] = vec4(xSpot + .05, .80, 0, 1);
			arrayBox[countBox + 7] = vec4(xSpot + .05, .75, 0, 1);
			countBox += 8;
		}
	}

	currentPoints = new vec4[numValues];
	for (int i = 0; i < numValues; i++)
	{
		currentPoints[i] = vec4(arrayNormVal[i][valX], arrayNormVal[i][valY], 0, 1);
	}

	currentPoints2 = new vec4[numTabs * numValues];
	int count = 0;
	countAxis = 0.0;
	xSpot = 0;
	for (int i = 0; i < numTabs; i++)
	{
		xSpot = -.75 + (countAxis * divideAxis);
		for (int j = 0; j < numValues; j++)
		{
			currentPoints2[count] = vec4(xSpot, arrayNormVal[j][i], .5, 1);
			count++;
		}
		countAxis++;
	}

	currentPoints2Lines = new vec4[2 * ((numTabs - 1) * numValues)];
	selectedLines = new vec4[2 * ((numTabs - 1) * numValues)];
	unSelectedLines = new vec4[2 * ((numTabs - 1) * numValues)];
	count = 0;
	for (int i = 0; i < numValues; i++)
	{
		int countVal = 0;
		for (int j = 0; j < numTabs - 1; j++)
		{
			currentPoints2Lines[count] = currentPoints2[countVal + i];
			selectedLines[count] = vec4(0, 0, 0, 1);
			unSelectedLines[count] = vec4(0, 0, 0, 1);
			countVal += (numValues - 1);
			currentPoints2Lines[count + 1] = currentPoints2[countVal + i];
			selectedLines[count + 1] = vec4(0, 0, 0, 1);
			unSelectedLines[count + 1] = vec4(0, 0, 0, 1);
			count += 2;
		}
	}

	arrayXValue = new double[8];
	arrayYValue = new double[8];
	arrayXValue[0] = 0;
	arrayYValue[0] = 0;
	arrayXValue[2] = 0;
	arrayYValue[2] = 0;
	arrayXValue[3] = 0;
	arrayYValue[3] = 0;
	arrayXValue[4] = 0;
	arrayYValue[4] = 0;
	arrayXValue[5] = 0;
	arrayYValue[5] = 0;
	arrayXValue[6] = 0;
	arrayYValue[6] = 0;
	arrayXValue[1] = 0;
	arrayYValue[1] = 0;
	arrayXValue[7] = 0;
	arrayYValue[7] = 0;

	selectedBox = new vec4[8];
	for (int i = 0; i < 8; i++)
	{
		selectedBox[i] = vec4(arrayXValue[i], arrayYValue[i], 0, 1);
	}

	in.close();

}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		leftButtonDown = 1;
		arrayXValue[0] = interpolate(0, x, WIN_WIDTH, -1, 1);
		arrayYValue[0] = interpolate(0, y, WIN_HEIGHT, 1, -1);
		arrayXValue[2] = arrayXValue[0];
		arrayYValue[2] = arrayYValue[0];
		int count = 0;
		for (int i = 0; i < numValues; i++)
		{
			for (int j = 0; j < numTabs - 1; j++)
			{
				selectedLines[count] = vec4(0, 0, 0, 1);
				selectedLines[count + 1] = vec4(0, 0, 0, 1);
				count += 2;
			}
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		leftButtonDown = 0;
		for (int i = 0; i < 8; i++)
		{
			selectedBox[i] = vec4(0, 0, 0, 1);
		}
		boxOpacity = 0;
		int count = 0;
		for (int i = 0; i < numValues; i++)
		{
			for (int j = 0; j < numTabs - 1; j++)
			{
				selectedLines[count] = vec4(0, 0, 0, 1);
				selectedLines[count + 1] = vec4(0, 0, 0, 1);
				count += 2;
			}
		}
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(vec4), arrayAxis);
	glBufferSubData(GL_ARRAY_BUFFER, (4 * sizeof(vec4)), numValues * sizeof(vec4), currentPoints);
	glBufferSubData(GL_ARRAY_BUFFER, (4 * sizeof(vec4)) + (numValues * sizeof(vec4)), 8 * sizeof(vec4), selectedBox);
	glutPostRedisplay();
	glutSetWindow(window2);
	glutPostRedisplay();
	glutSetWindow(window1);
	}
}

void mouseMotion(int x, int y)
{
	if (leftButtonDown == 1)
	{
		arrayXValue[3] = arrayXValue[0];
		arrayYValue[3] = interpolate(0, y, WIN_HEIGHT, 1, -1);
		arrayXValue[4] = arrayXValue[3];
		arrayYValue[4] = arrayYValue[3];
		arrayXValue[5] = interpolate(0, x, WIN_WIDTH, -1, 1);
		arrayYValue[5] = interpolate(0, y, WIN_HEIGHT, 1, -1);
		arrayXValue[6] = arrayXValue[5];
		arrayYValue[6] = arrayYValue[5];
		arrayXValue[1] = interpolate(0, x, WIN_WIDTH, -1, 1);
		arrayYValue[1] = arrayYValue[0];
		arrayXValue[7] = arrayXValue[1];
		arrayYValue[7] = arrayYValue[1];

		for (int i = 0; i < 8; i++)
		{
			selectedBox[i] = vec4(arrayXValue[i], arrayYValue[i], 0, 1);
		}
		boxOpacity = 1;
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(vec4), arrayAxis);
	glBufferSubData(GL_ARRAY_BUFFER, (4 * sizeof(vec4)), numValues * sizeof(vec4), currentPoints);
	glBufferSubData(GL_ARRAY_BUFFER, (4 * sizeof(vec4)) + (numValues * sizeof(vec4)), 8 * sizeof(vec4), selectedBox);
		int numLines = 0;
		for (int i = 0; i < numValues; i++)
		{
			if (currentPoints[i].y < selectedBox[7].y && currentPoints[i].y > selectedBox[3].y &&
				currentPoints[i].x < selectedBox[7].x && currentPoints[i].x > selectedBox[3].x)
			{
				for (int j = 0; j < numTabs - 1; j++)
				{
					if (currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].z != 0.5)
					{
						selectedLines[numLines] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].y, .1, 1);
						numLines++;
						selectedLines[numLines] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].y, .1, 1);
						numLines++;
					}
					else
					{
						selectedLines[numLines] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].y, .4, 1);
						numLines++;
						selectedLines[numLines] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].y, .4, 1);
						numLines++;
					}
				}
			}
		}
	glBufferSubData(GL_ARRAY_BUFFER, 0, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), currentPoints2Lines);
	glBufferSubData(GL_ARRAY_BUFFER, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), selectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 2 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), unSelectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * (numTabs)) * sizeof(vec4)), arrayAxis2);
	}
	glutPostRedisplay();
	glutSetWindow(window2);
	glutPostRedisplay();
	glutSetWindow(window1);
}

void mouse2(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		leftButtonDown2 = 1;
		valueX = interpolate(0, x, WIN_WIDTH, -1, 1);
		valueY = interpolate(0, y, WIN_HEIGHT, 1, -1);
		int countBox = 0;
		countTab = 0;
		countTabTemp = 0;
		for (int i = 0; i < 2 * numTabs; i++)
		{
			if (i == 0 || (i % 2 != 0))
			{

			}
			else
			{
				countTab += 1;
			}
			if (i == 0)
			{
				differenceValue = pow(valueX - arrayBox[countBox].x, 2) + pow(valueY - arrayBox[countBox].y, 2);
				indexSpot = i;
			}
			else
			{
				tempDifferenceValue = pow(valueX - arrayBox[countBox].x, 2) + pow(valueY - arrayBox[countBox].y, 2);
				if (differenceValue > tempDifferenceValue)
				{
					differenceValue = tempDifferenceValue;
					indexSpot = i;
					countTabTemp = countTab;
				}
			}
			countBox += 4;
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		leftButtonDown2 = 0;
	}
}

void mouseMotion2(int x, int y)
{
	if (leftButtonDown2 == 1)
	{
		valueYMove = interpolate(0, y, WIN_HEIGHT, 1, -1);
		valueYMoveTemp = arrayBox[(indexSpot * 4)].y;
		arrayBox[(indexSpot * 4)].y = valueYMove;
		arrayBox[(indexSpot * 4) + 1].y = -.05 + valueYMove;
		arrayBox[(indexSpot * 4) + 2].y = -.05 + valueYMove;
		arrayBox[(indexSpot * 4) + 3].y = valueYMove;
		int unSelect = 0;
		spotLines = 0;
		int countBox = 0;
		int numAdd = 0;
		for (int i = 0; i < numValues; i++)
		{
			for (int j = 0; j < numTabs; j++)
			{
				if (arrayBox[countBox].y > currentPoints2Lines[i * 2 * (numTabs - 1) + (j == numTabs-1 ? 2*numTabs-3 : 2*j)].y)
				{
					unSelect = 1;
				}
				if (arrayBox[countBox + 4].y < currentPoints2Lines[i * 2 * (numTabs - 1) + (j == numTabs-1 ? 2*numTabs-3 : 2*j)].y)
				{
					unSelect = 1;
				}
				if (j == 0)
				{
					numAdd = 1;
				}
				else
				{
					numAdd += 2;
				}
				countBox += 8;
			}
			numAdd = 0;
			countBox = 0;
			if (unSelect == 1)
			{
				for (int j = 0; j < numTabs - 1; j++)
				{
					currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].y, .5, 1);
					unSelectedLines[spotLines] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].y, .3, 1);
					spotLines++;
					currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].y, .5, 1);
					unSelectedLines[spotLines] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].y, .3, 1);
					spotLines++;
				}
			}
			else
			{
				for (int j = 0; j < numTabs - 1; j++)
				{
					currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j].y, .2, 1);
					currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1] = vec4(currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].x, currentPoints2Lines[i * 2 * (numTabs - 1) + 2 * j + 1].y, .2, 1);
				}
			}
			unSelect = 0;
		}
	glBufferSubData(GL_ARRAY_BUFFER, 0, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), currentPoints2Lines);
	glBufferSubData(GL_ARRAY_BUFFER, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), selectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 2 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), unSelectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * numTabs) * sizeof(vec4)), arrayAxis2);
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int width, int height)
{
	switch( key ) 
	{
	case 033:  // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Axis
	vColor = vec4(0, 0, 0, 1);
	glUniform4fv(vColorID, 1, vColor);
	glDrawArrays(GL_LINES, 0, 4);
	//Points
	vColor = vec4(0, 0, 1, 1);
	glUniform4fv(vColorID, 1, vColor);
	glDrawArrays(GL_POINTS, 4, numValues);
	//Selected Box
	vColor = vec4(1, 0, 0, 1);
	glUniform4fv(vColorID, 1, vColor);
	glDrawArrays(GL_LINES, 4 + numValues, 8);
	glFlush();
}

void display2()
{
	glBufferSubData(GL_ARRAY_BUFFER, 0, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), currentPoints2Lines);
	glBufferSubData(GL_ARRAY_BUFFER, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), selectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 2 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), unSelectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * numTabs) * sizeof(vec4)), arrayAxis2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Lines
	vColor = vec4(0, 0, 1, 1);
	glUniform4fv(vColorID, 1, vColor);
	glDrawArrays(GL_LINES, 0, 2 * ((numTabs - 1) * numValues));
	//Selected Lines
	vColor = vec4(1, 0, 0, 1);
	glUniform4fv(vColorID, 1, vColor);
	glDrawArrays(GL_LINES, 2 * ((numTabs - 1) * numValues), 2 * ((numTabs - 1) * numValues));
	//UNSelected Lines
	vColor = vec4(1, 1, 1, 1);
	glUniform4fv(vColorID, 1, vColor);
	glDrawArrays(GL_LINES, 2 * (2 * ((numTabs - 1) * numValues)), 2 * ((numTabs - 1) * numValues));
	//Axis
	vColor = vec4(0, 0, 0, 1);
	glUniform4fv(vColorID, 1, vColor);
	glDrawArrays(GL_LINES, 3 * (2 * ((numTabs - 1) * numValues)), 2 * numTabs);
	//QUADS
	vColor = vec4(1, 0, 0, 1);
	glUniform4fv(vColorID, 1, vColor);
	glBegin(GL_QUADS);
	int countBox = 0;
	for (int i = 0; i < numTabs; i++)
	{
		glVertex3d(arrayBox[countBox].x, arrayBox[countBox].y, arrayBox[countBox].z);
		glVertex3d(arrayBox[countBox + 1].x, arrayBox[countBox + 1].y, arrayBox[countBox + 1].z);
		glVertex3d(arrayBox[countBox + 2].x, arrayBox[countBox + 2].y, arrayBox[countBox + 2].z);
		glVertex3d(arrayBox[countBox + 3].x, arrayBox[countBox + 3].y, arrayBox[countBox + 3].z);

		glVertex3d(arrayBox[countBox + 4].x, arrayBox[countBox + 4].y, arrayBox[countBox + 4].z);
		glVertex3d(arrayBox[countBox + 5].x, arrayBox[countBox + 5].y, arrayBox[countBox + 5].z);
		glVertex3d(arrayBox[countBox + 6].x, arrayBox[countBox + 6].y, arrayBox[countBox + 6].z);
		glVertex3d(arrayBox[countBox + 7].x, arrayBox[countBox + 7].y, arrayBox[countBox + 7].z);
		countBox += 8;
	}
	glEnd();
	glFlush();
}

void init()
{   
	// Vertex array object
	GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	// Buffer object
	GLuint buffer;
    glGenBuffers(1, &buffer );
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
	glBufferData(GL_ARRAY_BUFFER, (4 * sizeof(vec4)) + (numValues * sizeof(vec4)) + (8 * sizeof(vec4)), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(vec4), arrayAxis);
	glBufferSubData(GL_ARRAY_BUFFER, (4 * sizeof(vec4)), numValues * sizeof(vec4), currentPoints);
	glBufferSubData(GL_ARRAY_BUFFER, (4 * sizeof(vec4)) + (numValues * sizeof(vec4)), 8 * sizeof(vec4), selectedBox);
	
	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	
	vColorID = glGetUniformLocation(program, "vColor");

	// Initialize the vertex position attribute from the vertex shader    
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glPointSize(5);
	glLineWidth(5);
}

void init2()
{   
	// Vertex array object
	GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	// Buffer object
	GLuint buffer;
    glGenBuffers(1, &buffer );
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, ((2 * numTabs) * sizeof(vec4)) + ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)) + ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)) + ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), currentPoints2Lines);
	glBufferSubData(GL_ARRAY_BUFFER, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), selectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 2 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), unSelectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * numTabs) * sizeof(vec4)), arrayAxis2);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	vColorID = glGetUniformLocation(program, "vColor");

	// Initialize the vertex position attribute from the vertex shader    
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glPointSize(5);
	glLineWidth(2.5);
}

void menu(int id)
{
	
	if (id == 0)
	{
		exit(EXIT_SUCCESS);
	}
	else if (id >= 1 && id < 20)
	{
		for (int i = 0; i < numValues; i++)
		{
			currentPoints[i] = vec4(arrayNormVal[i][id - 1], arrayNormVal[i][valY], 0, 1);
		}
		valX = id - 1;
	}
	else if (id >= 20)
	{
		for (int i = 0; i < numValues; i++)
		{
			currentPoints[i] = vec4(arrayNormVal[i][valX], arrayNormVal[i][id - 20], 0, 1);
		}
		valY = id - 20;
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(vec4), arrayAxis);
	glBufferSubData(GL_ARRAY_BUFFER, (4 * sizeof(vec4)), numValues * sizeof(vec4), currentPoints);
	glBufferSubData(GL_ARRAY_BUFFER, (4 * sizeof(vec4)) + (numValues * sizeof(vec4)), 8 * sizeof(vec4), selectedBox);
	glutPostRedisplay();
}

void menu2(int id)
{
	
	if (id == 0)
	{
		exit(EXIT_SUCCESS);
	}
	else if (id == 1)
	{
		numOpacity = 0;
	}
	else if (id == 2)
	{
		numOpacity = 0.25;
	}
	else if (id == 3)
	{
		numOpacity = 0.50;
	}
	else if (id == 4)
	{
		numOpacity = 0.75;
	}
	else if (id == 5)
	{
		numOpacity = 1;
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), currentPoints2Lines);
	glBufferSubData(GL_ARRAY_BUFFER, ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), selectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 2 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), unSelectedLines);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * ((2 * ((numTabs - 1) * numValues)) * sizeof(vec4)), ((2 * numTabs) * sizeof(vec4)), arrayAxis2);
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	//string fileName = "aaup.txt";
	//string fileName = "cars.txt";
	string fileName = "iris.txt";
	//string fileName = "out5d.txt";

	readFile(fileName);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);

	//First Window
	window1 = glutCreateWindow("Scatter Plot");
	glewExperimental = GL_TRUE;
	glewInit();
	init();	

	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);
    
	// create GLUT menu here
	int submenu = glutCreateMenu(menu);
	for (int i = 1; i < (1 + numTabs); i++)
	{
		glutAddMenuEntry(arrayTabs[numCount].c_str(), i);
		numCount++;
	}
	numCount = 0;
	
	int submenu2 = glutCreateMenu(menu);
	for (int i = 20; i < (20 + numTabs); i++)
	{
		glutAddMenuEntry(arrayTabs[numCount].c_str(), i);
		numCount++;
	}
	numCount = 0;

	glutCreateMenu(menu);
	glutAddSubMenu("X Axis", submenu);
	glutAddSubMenu("Y Axis", submenu2);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	//Second Window
	window2 = glutCreateWindow("Parallel Coordinates");
	glewExperimental = GL_TRUE;
	glewInit();
	init2();	

	glutDisplayFunc(display2);
	glutMouseFunc(mouse2);
	glutMotionFunc(mouseMotion2);
	glutKeyboardFunc(keyboard);
    
	// create GLUT menu here
	int submenu3 = glutCreateMenu(menu2);
	glutAddMenuEntry("0.00", 1);
	glutAddMenuEntry("0.25", 2);
	glutAddMenuEntry("0.50", 3);
	glutAddMenuEntry("0.75", 4);
	glutAddMenuEntry("1.00", 5);
	glutCreateMenu(menu2);
	glutAddSubMenu("Opacity", submenu3);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
	return 0;
}