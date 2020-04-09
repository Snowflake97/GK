

#define _USE_MATH_DEFINES
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <glut.h>
#include <iostream>

using namespace std;



typedef float point3[3]; // pomocnicza tablica dla współrzędnych x,y,z
typedef float point2[2]; // pomocnicza tablica dla współrzędnych u,v
typedef int wspolrzedne[2]; // pomocnicza tablica dla współrzędnych pozycji - i,j
int model = 3;	// start programu z widokiem na siatke
static double R = 10; 
static GLfloat theta[] = { 0.0, 1.0 };	// azymut,elewacja
static GLfloat viewer[] = { 0.0, 0.0, 30.0 }; // początkowe współrzedne obserwatora

static GLfloat pix2angle;     // przelicznik pikseli na stopnie

static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naciśnięto żadnego klawisza
							   // 1 - naciśnięty zostać lewy klawisz

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy(x)
static int y_pos_old = 0;		// poprzednia pozycja kursora myszy(y)

static int delta_x = 0;			// odchylenie kursora o pixele(x)
static int delta_y = 0;			// odchylenie kursora o pixele(y)

const int N = 100; // ustalenie stałej N
point3 tab[N][N];  // definicja tablicy puntków
point3 colors[N][N];	// definicja tablicy kolorów
point3 normalVector[N][N]; // tablica znormalizowanych wektorow
point2 texture[N][N];	// punkty u,v

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



// generator tablicy losowych kolorów o rozmiarze NxN 


// funkcje wyznaczające współrzedne z podanych wzorów
inline float getx(float u, float v)
{
	return ((-90 * pow(u, 5)) + (225 * pow(u, 4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * cos(3.1415 * v);
}
inline float gety(float u, float v)
{
	return  (160 * pow(u, 4)) - (320 * pow(u, 3)) + (160 * pow(u, 2));
}
inline float getz(float u, float v)
{
	return ((-90 * pow(u, 5)) + (225 * pow(u, 4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * sin(3.1415 * v);
} 
float getXu(float u, float v) {
	float PIV = M_PI * v;
	return (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(PIV);
}
float getXv(float u, float v) {
	float PIV = M_PI * v;
	return M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(PIV);
}
float getYu(float u, float v) {
	return (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
}
float getYv(float u, float v) {
	return 0;
}
float getZu(float u, float v) {
	float PIV = M_PI * v;
	return (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(PIV);
}
float getZv(float u, float v) {
	float PIV = M_PI * v;
	return -M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(PIV);
}
float vectorLen(float xv, float yv, float zv) {
	return sqrt(pow(xv, 2) + pow(yv, 2) + pow(zv, 2));
}


// funkcja wypełniająca tablice współrzednych o rozmiarze NxN
void fill_array() {

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{

			//obliczanie wartości u i v z zakresu <0,1>
			float u = float(i) / float(N);
			float v = float(j) / float(N);

			// wyliczenie wspolrzednych x,y,z
			tab[i][j][0] = getx(u, v);
			tab[i][j][1] = gety(u, v);
			tab[i][j][2] = getz(u, v);

			// wyliczenie wartosci u,v dla wspolrzednych
			float xU = getXu(u, v);
			float xV = getXv(u, v);

			float yU = getYu(u, v);
			float yV = getYv(u, v);

			float zU = getZu(u, v);
			float zV = getZv(u, v);

			// wyliczenie skladowych wektora
			float xVector = (yU * zV - zU * yV);
			float yVector = (zU * xV - xU * zV);
			float zVector = (xU * yV - yU * xV);

			// wyliczenie dlugosci wektora 
			float  len = vectorLen(xVector, yVector, zVector);

			// wektor znormalizowany
			point3 normVector = { xVector / len, yVector / len, zVector / len };

			if (i == 0 || i == N) {
				normVector[0] = 0;
				normVector[1] = -1;
				normVector[2] = 0;
			}
			else if (i == N / 2) {
				normVector[0] = 0;
				normVector[1] = 1;
				normVector[2] = 0;
			}
			else if (i > N / 2) {
				normVector[0] *= -1;
				normVector[1] *= -1;
				normVector[2] *= -1;
			}

			// przypisanie wektora do tablicy
			normalVector[i][j][0] = normVector[0];
			normalVector[i][j][1] = normVector[1];
			normalVector[i][j][2] = normVector[2];

			//punkty u,v do tablicy
			texture[i][j][0] = u;
			texture[i][j][1] = v;

		}
	}
}
void triangles(wspolrzedne point_a, wspolrzedne point_b, wspolrzedne point_c) {
	glBegin(GL_TRIANGLES);
	int X, Y;

	// petla pobierajaca 3 wierzcholki
	for (int i = 0; i < 3; i++) {
		// punkt a
		if (i == 0) {
			X = (point_a[0]);
			Y = (point_a[1]);
		}
		// punkt b
		else if (i == 1) {
			X = (point_b[0]);
			Y = (point_b[1]);
		}
		//punkt c
		else {
			X = int(point_c[0]);
			Y = int(point_c[1]);
		}
	

		glNormal3fv(normalVector[X][Y]); // normalizacja
		glTexCoord2fv(texture[X][Y]); // tekstura
		glVertex3fv(tab[X][Y]); // ustalenie wierzcholka
	}

	glEnd();
}
//funkcja rysujaca punkty współrzędnych jajka
void draw_egg() {
	glTranslated(0, -5, 0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			//iteracja po kazdym miejscu w tablicy, pobieranie punktu i wyświetalnie go jako wierzchołek
			glVertex3fv(tab[i][j]);
		}
	}
	glEnd();
}

//rysowanie jajka za pomoca prostych pionowych, poziomych i ukośnych

void draw_egg_as_net() {

	glTranslated(0, -5, 0);

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			// rysowanie liń pionowych 

			glColor3f(0.0f, 1.0f, 0.0f);
			glBegin(GL_LINES);
			glVertex3fv(tab[i][j]);
			if (i < N - 1) {
				glVertex3fv(tab[i + 1][j]);
			}
			else {
				glVertex3fv(tab[0][0]);
				glVertex3fv(tab[i][j]);

			}
			glEnd();

			// rysowanie liń poziomych
			glBegin(GL_LINES);
			if (j < N - 1) {
				glVertex3fv(tab[i][j + 1]);
				glVertex3fv(tab[i][j]);
			}
			else {
				glVertex3fv(tab[i][j]);
				if (i > 0) glVertex3fv(tab[N - i][0]);
			}
			glEnd();
			// rysowanie liń ukosnych
			glBegin(GL_LINES);
			if (j < N - 1 && i < N - 1) {
				glVertex3fv(tab[i + 1][j]);
				glVertex3fv(tab[i][j + 1]);
			}
			else {
				if (i > 0) {
					glVertex3fv(tab[i][j]);
					glVertex3fv(tab[N - i - 1][0]);
				}

			}
			glEnd();
		}

	}
}

//rysowanie jajka przez trojkaty
//kazdy wierzcholek posiada wygenerowany dla siebie kolor i stale przypisany, przechowywany w tablicy colors 
void draw_egg_as_triangle() {
	glTranslated(0, -5, 0);

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			// Pierwszy poziom (spod po przedniej stronie jajka)
			if (i == 0)
			{
				// Dolny prawy rog siatki
				if (j == (N - 1))
					triangles(wspolrzedne{ 0,j }, wspolrzedne{ 1,j }, wspolrzedne{ N - 1,0 });
				else
					triangles(wspolrzedne{ 0,j }, wspolrzedne{ 1,j }, wspolrzedne{ 1,j + 1 });
			}
			// Ostatni poziom (spod po tylnej stronie jajka)
			else if (i == (N - 1))
			{
				// G?rny prawy rog siatki
				if (j == (N - 1))
					triangles(wspolrzedne{ i,j }, wspolrzedne{ 0,j }, wspolrzedne{ 1,0 });
				else
					triangles(wspolrzedne{ i,j }, wspolrzedne{ 0,j }, wspolrzedne{ i,j + 1 });
			}
			// Wszystkie brzegi
			else if (j == (N - 1))
			{
				triangles(wspolrzedne{ i,j }, wspolrzedne{ i + 1,j }, wspolrzedne{ N - i,0 });
				triangles(wspolrzedne{ N - i,0 }, wspolrzedne{ N - (i + 1),0 }, wspolrzedne{ i + 1,j });
			}
			// Normalne wypelnianie
			else
			{
				triangles(wspolrzedne{ i,j }, wspolrzedne{ i + 1,j }, wspolrzedne{ i,j + 1 });
				triangles(wspolrzedne{ i,j + 1 }, wspolrzedne{ i + 1,j + 1 }, wspolrzedne{ i + 1,j });
			}
		}
	}
}

// różnica pomiędzy pozycją bieżącą
									  // i poprzednią kursora myszy
// inicjalizacja położenia obserwatora

/*************************************************************************************/

// Funkcja rysująca osie układu wspó?rz?dnych


/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia wartości odpowiednich zmiennych globalnych

void Mouse(int btn, int state, int x, int y)
{


	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora(x)
		y_pos_old = y;		  // przypisanie aktualnie odczytanej pozycji kursora(y)

		status = 1;          // wcięnięty został lewy klawisz myszy
	}

	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora(x)
		y_pos_old = y;		  // przypisanie aktualnie odczytanej pozycji kursora(y)

		status = 2;          // wcięnięty został prawy klawisz myszy
	}
	else

		status = 0;          // nie został wcięnięty żaden klawisz

}

/*************************************************************************************/
// Funkcja "monitoruje" położenie kursora myszy i ustawia wartości odpowiednich
// zmiennych globalnych

void Motion(GLsizei x, GLsizei y)
{

	delta_x = x - x_pos_old;     // obliczenie różnicy położenia kursora myszy

	x_pos_old = x;            // podstawienie bieżącego położenia jako poprzednie

	delta_y = y - y_pos_old;     // obliczenie różnicy położenia kursora myszy

	y_pos_old = y;



	glutPostRedisplay();     // przerysowanie obrazu sceny
}

/*************************************************************************************/


void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz?tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz?tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz?tek i koniec obrazu osi y
	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y

	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z

	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();

}

/*************************************************************************************/

// Funkcja określająca co ma być rysowane (zawsze wywoływana, gdy trzeba
// przerysować scenę)



void RenderScene(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();
	// Czyszczenie macierzy bie??cej

	if (status == 1)                     // jeśli lewy klawisz myszy wcięnięty
	{
		theta[0] += delta_x * pix2angle;    // modyfikacja kąta obrotu o kat proporcjonalny
		theta[1] += delta_y * pix2angle;

	}
	if (status == 2)                     // jeśli lewy klawisz myszy wcięnięty
	{

		R += delta_y * 0.2;    // modyfikacja kąta obrotu o kat proporcjonalny
	}


	viewer[0] = R * cos(theta[0] * M_PI / 180) * cos(theta[1] * M_PI / 180);
	viewer[1] = R * sin(theta[1] * M_PI / 180);
	viewer[2] = R * sin(theta[0] * M_PI / 180) * cos(theta[1] * M_PI / 180);

	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// Zdefiniowanie położenia obserwatora
	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej powyżej


	if (model == 1) {
		draw_egg();
	}

	if (model == 2) {
		draw_egg_as_net();
	}
	if (model == 3) {
		draw_egg_as_triangle();
	}
	if (model == 4) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glutWireTeapot(3.0);
	}

	glFlush();
	// Przekazanie poleceń rysujących do wykonania
	glutSwapBuffers();
}

void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 1; // wywolanie punktow
	if (key == 'w') model = 2; // wywolanie siatki
	if (key == 's') model = 3; // wywolanie pomalowanego jajka z trojkatow
	if (key == 'c') model = 4; // wywolanie czajnika

	RenderScene();
}
/*************************************************************************************/

// Funkcja ustalająca stan renderowania



void MyInit(void)
{

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Kolor czyszczący (wypełnienia okna) ustawiono na biały

	/*************************************************************************************/
	// Definicja materiału z jakiego zrobiony jest czajnik

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki ka =[kar,kag,kab] dla światła otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki kd =[kdr,kdg,kdb] światła rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki ks =[ksr,ksg,ksb] dla światła odbitego               

	GLfloat mat_shininess = { 20.0 };
	// współczynnik n opisujący połysk powierzchni



	// Definicja źródła światła

	GLfloat light_position[] = { 0.0, 10.0, 10.0, 1.0 }; // położenie źródła

	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	// składowe intensywności świecenia źródła światła otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
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


	/*************************************************************************************/
	// Ustawienie patrametrów materiału


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	/*************************************************************************************/
	// Ustawienie parametrów źródła

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


	/*************************************************************************************/
	// Ustawienie opcji systemu oświetlania sceny

	glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
	glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
	glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
	glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora

	/*************************************************************************************/

	// Zmienne dla obrazu tekstury
	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	pBytes = LoadTGAImage("P1_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat); //  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes); // Zdefiniowanie tekstury 2-D
	free(pBytes); // Zwolnienie pamięci
	glEnable(GL_TEXTURE_2D); // Włączenie mechanizmu teksturowania
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Ustalenie trybu teksturowania
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Określenie sposobu nakładania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Określenie sposobu nakładania tekstur

}



// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są 
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



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
	cout << "LEGENDA" << endl << "1.Zmiana modelu:\nc - czajnik\np - punkty jajka\ns - teksturowane jajko\nw - siatka lini\n\n";
	cout << "2.Sterowanie punktem patrzenia obserwatora:\n- lewy przycisk myszy(obrot) \n- prawy przycisk myszy(odleglosc)\n\n";
	cout << "Podczas wcisnietego lewego przycisku myszy:\n- ruch kursora w kierunku poziomym powoduje horyzontalna zmiane obserwatora\n- ruch kursora w kierunku pionowym powoduje wertykanla zmiane oberwatora" << endl;
	cout << "Podczas wcisnietego prawgo przycisku myszy:\n- ruch kursora w kierunku pionowym powoduje zblizenie lub oddalnie punktu obserwatora";
	fill_array(); // wypełnienie tablicy współrzędnych

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(600, 600);

	glutCreateWindow("Teksturowanie");

	glutKeyboardFunc(keys);

	glutMouseFunc(Mouse);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy


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