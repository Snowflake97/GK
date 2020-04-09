

#define _USE_MATH_DEFINES
#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <glut.h>
#include <iostream>
#include <math.h>

using namespace std;

//predkosci obrotów
float obrotZiemi = 0.8;
float obrotSlonca = 0.1;

float poOrbicie = 0.5;

GLuint texture[2]; //tablica do przechowywania tesktur

// x i y naszej planety
float xForEarth, zForEarth;
// licznik pozycji planety
float counterForEarth = 0;
const float DEG2RAD = 3.14159 / 180.0;

bool animate = true; // zmienna animacji (on/off)

GLfloat theta[] = { 0.0, 0.0 };


static GLfloat pix2angle;     // przelicznik pikseli na stopnie


GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{
#pragma pack(1)           
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)


	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;
	fopen_s(&pFile, FileName, "rb");
	if (pFile == NULL)
		return NULL;
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
	if (pbitsperpixel == NULL)
		return NULL;
	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}
	switch (sDepth)
	{
	case 3:
		*ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;
	case 4:
		*ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;
	case 1:
		*ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;
	};
	fclose(pFile);
	return pbitsperpixel;
}


// Funkcja określająca co ma być rysowane (zawsze wywoływana, gdy trzeba
// przerysować scenę)

void RenderScene(void)
{
	theta[0] += obrotZiemi; // zmiana obrotu ziemi wokol wlasnej osi
	theta[1] += obrotSlonca; // zmiana obrotu slonca wokol wlasnej osi

	// resetowanie obrtow
	if (theta[0] > 360.0) theta[0] -= 360.0; 
	if (theta[1] > 360.0) theta[1] -= 360.0;

	counterForEarth += poOrbicie; // zmiana pozycji planety na orbicie 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();

	// ustawienie punktu obserwowania
	gluLookAt(0.95, 24.64,9.23 , 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//rysowanie orbity
	glBegin(GL_POINTS);
	for (int i = 0; i < 360; i++)
	{
		float degInRad = i * DEG2RAD;
		glColor3f(1, 0, 0);
		glVertex3f(cos(degInRad) * 14.0, 0.0f, sin(degInRad) * 7.3);
	}
	glEnd();

	//resetowanie pozycji na orbicie
	if (counterForEarth > 359)
		counterForEarth = 0;


	// utworzenie slonca, nadanie tekstury
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	GLUquadric* q = gluNewQuadric();
	gluQuadricNormals(q, GLU_SMOOTH);
	gluQuadricTexture(q, GL_TRUE);

	// rysowanie slonca w okreslonej pozycji, ustalenie kata obrotu 
	glPushMatrix();
	glTranslatef(-3.0, 0.0,-2.0);
	glRotated(theta[1], 0, 0, 1);
	gluSphere(q, 5.0, 50, 50); 
	glPopMatrix();

	// liczenie wspolrzednych dla ziemi po orbicie
	xForEarth = cos(counterForEarth * DEG2RAD) * 14.0f;//nowe x planety
	zForEarth = sin(counterForEarth * DEG2RAD) * 7.3f;//nowe y planety

	// stworzenie ziemi, nadanie tekstury
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	GLUquadric* e = gluNewQuadric();
	gluQuadricNormals(e, GLU_SMOOTH);
	gluQuadricTexture(e, GL_TRUE);

	glPushMatrix();
	glTranslatef(xForEarth, 0.0f, zForEarth); // Zmiana pozycji planety 
	glRotatef(theta[0],0,0,1); // predkosc obrotu 
	glBindTexture(GL_TEXTURE_2D, texture[1]); //nadanie tekstury
	gluSphere(e, 3.0, 50, 50); // narysowanie ziemi
	glPopMatrix();


	glFlush();
	glutSwapBuffers();
	Sleep(10); // opoznienie 10ms

	// jezeli true, przeprowadzenie animacji z opoznieniem 10 ms
	if (animate) {
		glutPostRedisplay();
	}
}

void keys(unsigned char key, int x, int y)
{
	switch (key) {
	case ' ':
		if (animate) {
			animate = false;
		}
		else {
			animate = true;
		}
		break;
	case 'q':
		poOrbicie -= 0.05; // zmniejszenie predkosci ruchu ziemi po orbicie
		break;
	case 'w':
		poOrbicie += 0.05; // zwiekszenie predkosci ruchu ziemi po orbicie
		break;
	case 'e':
		obrotZiemi -= 0.05; // zmniejszenie predkosci obrotu ziemi wokol wlasnej osi
		break;
	case 'r':
		obrotZiemi += 0.05; // zwiekszenie predkosci obrotu ziemi wokol wlasnej osi
		break;
	case 'a':
		obrotSlonca -= 0.01; // zmniejszenie predkosci obrotu slonca wokol wlasnej osi
		break;
	case 's':
		obrotSlonca += 0.01; // zwiekszenie predkosci obrotu slonca wokol wlasnej osi
		break;
	}
	glutPostRedisplay();
}
/*************************************************************************************/

// Funkcja ustalająca stan renderowania


void MyInit(void)
{
	glDepthRange(0, 200);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Kolor czyszczący (wypełnienia okna) ustawiono na biały

// definicja materialu
	GLfloat mat_ambient[] = { 1.0, 1.0, 0.0, 1.0 };
	// współczynniki ka =[kar,kag,kab] dla światła otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki kd =[kdr,kdg,kdb] światła rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki ks =[ksr,ksg,ksb] dla światła odbitego               

	GLfloat mat_shininess = { 20.0 };
	// współczynnik n opisujący połysk powierzchni
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
	// położenie źródła
	GLfloat mat_emission[] = { 1.0, 0.5, 0.5, 1.0 };
	// emisja swiatla od materialu


	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// składowe intensywności świecenia źródła światła otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant = { 1.0 };
	// składowa stała ds dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_linear = { 0.05 };
	// składowa liniowa dl dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_quadratic = { 0.001 };
	// składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);


	/*************************************************************************************/
	// Ustawienie opcji systemu oświetlania sceny

	glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
	glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
	glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
	glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora

	/*************************************************************************************/


	/*************************************************************************************/
	GLbyte* sun;
	GLbyte* earth;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;
	glGenTextures(2, texture);

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	sun = LoadTGAImage("sun.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, sun); // Zdefiniowanie tekstury 2-D
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Ustalenie trybu teksturowania
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Określenie sposobu nakładania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Określenie sposobu nakładania tekstur
	free(sun); // Zwolnienie pamięci

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	earth = LoadTGAImage("P1_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, earth); // Zdefiniowanie tekstury 2-D
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Ustalenie trybu teksturowania
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Określenie sposobu nakładania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Określenie sposobu nakładania tekstur
	free(earth); // Zwolnienie pamięci
	// Zmienne dla obrazu tekstury


}


void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

	pix2angle = 360.0 / (float)horizontal;

	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bieżącej

	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielkości okna okna widoku (viewport) w zależności
	// relacji pomiędzy wysokością i szerokością okna

	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli

void main(void)
{
	cout << "LEGENDA\n\n";
	cout << "Zatrzymanie/wznowienie animacji - spacja\n\n";
	cout << "Predkosc ruchu ziemi po orbicie:\nq - zmniejszenie\nw - zwiekszenie\n\n";
	cout << "Predkosc obrotu ziemi wookol wlasnej osi:\ne - zmniejszenie\nr - zwiekszenie\n\n";
	cout << "Predkosc obrotu slonca wookol wlasnej osi:\na - zmniejszenie\ns - zwiekszenie\n\n";


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(600, 600);

	glutCreateWindow("Projekt");

	glutKeyboardFunc(keys);

	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy


	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną
	// (callback function).  Będzie ona wywoływana za każdym razem
	// gdy zajdzie potrzeba przerysowania okna


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
	// za zmiany rozmiaru okna                       


	MyInit();
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przystąpieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// Włączenie mechanizmu usuwania niewidocznych elementów sceny

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

}