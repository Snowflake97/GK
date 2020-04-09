// fvhfdugf.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#define _USE_MATH_DEFINES
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <glut.h>


typedef float point3[3];
int model = 2;	// start programu z widokiem na siatke
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

const int N = 30; // ustalenie stałej N
point3 tab[N][N];  // definicja tablicy puntków
point3 colors[N][N];	// definicja tablicy kolorów



// generator tablicy losowych kolorów o rozmiarze NxN 
void fill_color_array()
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			colors[i][j][0] = float(rand() % 256) / 255;
			colors[i][j][1] = float(rand() % 256) / 255;
			colors[i][j][2] = float(rand() % 256) / 255;
			colors[i][j][3] = float(rand() % 101) / 1000;

		}
	}
}

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


// funkcja wypełniająca tablice współrzednych o rozmiarze NxN
void fill_array() {

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{

			//obliczanie wartości u i v z zakresu <0,1>
			float u = float(i) / float(N);
			float v = float(j) / float(N);


			tab[i][j][0] = getx(u, v);
			tab[i][j][1] = gety(u, v);
			tab[i][j][2] = getz(u, v);

		}
	}
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

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			glBegin(GL_TRIANGLES);
			glColor4fv(colors[i][j]);
			glVertex3fv(tab[i][j]);
			glColor4fv(colors[(i + 1) % N][j]);
			glVertex3fv(tab[(i + 1) % N][j]);
			glColor4fv(colors[i][j + 1]);
			glVertex3fv(tab[i][j + 1]);
			glEnd();

			glBegin(GL_TRIANGLES);
			glColor4fv(colors[(i + 1) % N][j]);
			glVertex3fv(tab[(i + 1) % N][j]);
			glColor4fv(colors[(i + 1) % N][j + 1]);
			glVertex3fv(tab[(i + 1) % N][j + 1]);
			glColor4fv(colors[i][j + 1]);
			glVertex3fv(tab[i][j + 1]);
			glEnd();

			if (i > 0) {
				glBegin(GL_TRIANGLES);
				glColor4fv(colors[i][j]);
				glVertex3fv(tab[i][j]);
				glColor4fv(colors[(i + 1) % N][j]);
				glVertex3fv(tab[(i + 1) % N][j]);
				glColor4fv(colors[N - i][0]);
				glVertex3fv(tab[N - i][0]);
				glEnd();
			}
			glBegin(GL_TRIANGLES);
			glColor4fv(colors[(i + 1) % N][j]);
			glVertex3fv(tab[(i + 1) % N][j]);
			glColor4fv(colors[(N - i) % N][0]);
			glVertex3fv(tab[(N - i) % N][0]);
			glColor4fv(colors[N - i - 1][0]);
			glVertex3fv(tab[N - i - 1][0]);
			glEnd();

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
	if (key == 'c') model = 4; // wywolanie pomalowanego jajka z trojkatow

	RenderScene();
}
/*************************************************************************************/

// Funkcja ustalająca stan renderowania



void MyInit(void)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszczący (wypełnienia okna) ustawiono na czarny

}

/*************************************************************************************/


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
	fill_color_array(); // wypełnienie tablicy kolorów
	fill_array(); // wypełnienie tablicy współrzędnych

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(600, 600);

	glutCreateWindow("Rzutowanie perspektywiczne");

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