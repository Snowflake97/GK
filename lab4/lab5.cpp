#define _USE_MATH_DEFINES
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <iostream>
#include <glut.h>
using namespace std;

typedef float point3[3];
typedef int wspolrzedne[2];
int model = 3;	// start programu z widokiem na teksturowane jajko
static double R = 10;
static GLfloat light1[] = { 0.0, 1.0 };	// azymut,elewacja
static GLfloat light2[] = { 0.0, 1.0 };	// azymut,elewacja
static GLfloat viewer[] = { 0.0, 0.0, 10.0 }; // początkowe współrzedne obserwatora

static GLfloat pix2angle;     // przelicznik pikseli na stopnie

static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naciśnięto żadnego klawisza
							   // 1 - naciśnięty zostać lewy klawisz

static int x_point_old = 0;       // poprzednia pozycja kursora myszy(x)
static int y_point_old = 0;		// poprzednia pozycja kursora myszy(y)

static int delta_x = 0;			// odchylenie kursora o pixele(x)
static int delta_y = 0;			// odchylenie kursora o pixele(y)

GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };// położenie źródła


const int N = 50; // ustalenie stałej N
point3 tab[N][N];  // definicja tablicy puntków
point3 colors[N][N];	// definicja tablicy kolorów
point3 u_points[N][N];	// punkty u
point3 v_points[N][N];	// punkty v

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
inline float getXu(float u, float v) {
	return (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);
}
inline float getYu(float u, float v) {
	return (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
}
inline float getZu(float u, float v) {
	return (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(M_PI * v);
}
inline float getXv(float u, float v) {
	return M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI * v);
}
inline float getYv(float u, float v) {
	return 0;
}
inline float getZv(float u, float v) {
	return M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v) * -1;
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


			u_points[i][j][0] = getXu(u, v);
			u_points[i][j][1] = getYu(u, v);
			u_points[i][j][2] = getZu(u, v);

			v_points[i][j][0] = getXv(u, v);
			v_points[i][j][1] = getYv(u, v);
			v_points[i][j][2] = getZv(u, v);

		}
	}
}

void triangles(wspolrzedne point_a, wspolrzedne point_b, wspolrzedne point_c) {
	glBegin(GL_TRIANGLES);
	wspolrzedne point;

	// petla pobierajaca 3 wierzcholki
	for (int i = 0; i < 3; i++) {
		if (i == 0) {
			point[0] = point_a[0];
			point[1] = point_a[1];
		}
		else if (i == 1) {
			point[0] = point_b[0];
			point[1] = point_b[1];
		}
		else {
			point[0] = point_c[0];
			point[1] = point_c[1];
		}
		// obliczenie wektora normalnego
		int X = point[0];
		int Y = point[1];
		point3 normVector = {
		u_points[X][Y][1] * v_points[X][Y][2] - u_points[X][Y][2] * v_points[X][Y][1],
		u_points[X][Y][2] * v_points[X][Y][0] - u_points[X][Y][0] * v_points[X][Y][2],
		u_points[X][Y][0] * v_points[X][Y][1] - u_points[X][Y][1] * v_points[X][Y][0] };
		// odleglosc
		float length = sqrt(pow(normVector[0], 2) + pow(normVector[1], 2) + pow(normVector[2], 2));
		normVector[0] = normVector[0] / length;
		normVector[1] = normVector[1] / length;
		normVector[2] = normVector[2] / length;

		if (point[0] == 0 || point[0] == N) {
			normVector[0] = 0;
			normVector[1] = -1;
			normVector[2] = 0;
		}
		else if (point[0] == N / 2) {
			normVector[0] = 0;
			normVector[1] = 1;
			normVector[2] = 0;
		}
		else if (point[0] > N / 2) {
			normVector[0] *= -1;
			normVector[1] *= -1;
			normVector[2] *= -1;
		}

		glNormal3fv(normVector); // normalizacja
		glColor4fv(colors[point[0]][point[1]]); // kolor
		glVertex3fv(tab[point[0]][point[1]]); // ustalenie wierzcholka
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
//kazdy wierzcholek pointiada wygenerowany dla siebie kolor i stale przypisany, przechowywany w tablicy colors 
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


/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia wartości odpowiednich zmiennych globalnych

void Mouse(int btn, int state, int x, int y)
{


	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_point_old = x;         // przypisanie aktualnie odczytanej pozycji kursora(x)
		y_point_old = y;		  // przypisanie aktualnie odczytanej pozycji kursora(y)

		status = 1;          // wcięnięty został lewy klawisz myszy
	}

	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		x_point_old = x;         // przypisanie aktualnie odczytanej pozycji kursora(x)
		y_point_old = y;		  // przypisanie aktualnie odczytanej pozycji kursora(y)

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

	delta_x = x - x_point_old;     // obliczenie różnicy położenia kursora myszy

	x_point_old = x;            // podstawienie bieżącego położenia jako poprzednie

	delta_y = y - y_point_old;     // obliczenie różnicy położenia kursora myszy

	y_point_old = y;



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
	// Czyszczenie macierzy bieżącej
	if (status == 1)                     // jeśli lewy klawisz myszy wcięnięty
	{
		light1[0] += delta_x * pix2angle;    // modyfikacja kąta padania światła pierwszego o kat proporcjonalny
		light1[1] += delta_y * pix2angle;

	}
	if (status == 2)                     // jeśli prawy klawisz myszy wcięnięty
	{

		light2[0] += delta_x * pix2angle;    // modyfikacja kąta padania światła drugiego o kat proporcjonalny
		light2[1] += delta_y * pix2angle;
	}
	// wyliczenie i ustawienie pozycji świateł
	light_position[0] = R * cos(light1[0] * M_PI / 180) * cos(light1[1] * M_PI / 180);
	light_position[1] = R * sin(light1[1] * M_PI / 180);
	light_position[2] = R * sin(light1[0] * M_PI / 180) * cos(light1[1] * M_PI / 180);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	light_position[0] = R * cos(light2[0] * M_PI / 180) * cos(light2[1] * M_PI / 180);
	light_position[1] = R * sin(light2[1] * M_PI / 180);
	light_position[2] = R * sin(light2[0] * M_PI / 180) * cos(light2[1] * M_PI / 180);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position);

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
		glutSolidTeapot(3.0);
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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszczący (wypełnienia okna) ustawiono na czarny
	/*************************************************************************************/
	// Definicja materiału z jakiego zrobiony jest czajnik
	GLfloat mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	// współczynniki ka =[kar,kag,kab] dla światła otoczenia
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki kd =[kdr,kdg,kdb] światła rozproszonego
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki ks =[ksr,ksg,ksb] dla światła odbitego   
	GLfloat mat_shininess = { 20.0 };
	/*************************************************************************************/
	// Definicja parametrów świateł
	GLfloat lightOnePosition[] = { -10.0, -10.0, -10.0, 1.0 };
	// położenie źródła pierwszego
	GLfloat lightTwoPosition[] = { -10.0, -10.0, -10.0, 1.0 };
	// położenie źródła drugiego
	GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	// składowe intensywności świecenia źródła światła otoczenia
	// Ia = [Iar,Iag,Iab]
	GLfloat lightOneDiffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]
	GLfloat lightTwoDiffuse[] = { 0.0, 0.0, 1.0, 1.0 };
	// składowe intensywności świecenia źródła światła pierwszego powodującego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]
	GLfloat lightOneSpecular[] = { 1.0, 1.0, 0.0, 1.0 };
	// składowe intensywności świecenia źródła światła drugiego powodującego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]
	GLfloat lightTwoSpecular[] = { 1.7, 0.7, 1.0, 1.0 };
	GLfloat att_constant = { 1.0 };
	// składowa stała ds dla modelu zmian oświetlenia w funkcji
	// odległości od źródła
	GLfloat att_linear = { 0.05 };
	// składowa liniowa dl dla modelu zmian oświetlenia w funkcji
	// odległości od źródła
	GLfloat att_quadratic = { 0.001 };
	// składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	// Zastosowanie parametrów materiału
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// Zastosowanie parametrów światła pierwszego
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightOneSpecular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);

	// Zastosowanie parametrów światła drugiego
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightTwoDiffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightTwoSpecular);
	glLightfv(GL_LIGHT2, GL_POSITION, lightTwoPosition);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, att_quadratic);
	// Wygładzanie cieni
	glShadeModel(GL_SMOOTH);
	// Aktywacja świateł
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_DEPTH_TEST);
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
	cout << "LEGENDA" << endl << "1.Zmiana modelu:\nc - czajnik\np - punkty jajka\ns - teksturowane jajko\nw - siatka lini\n\n";
	cout << "2.Sterowanie swiatlem:\n- lewy przycisk myszy(swiatlo czerwone)\n- prawy przycisk myszy(swiatlo niebieskie)\n\n";
	cout << "Podczas wcisnietego przycisku myszy:\n- ruch kursora w kierunku poziomym powoduje horyzontalna zmiane swiatla\n- ruch kursora w kierunku pionowym powoduje wertykanla zmiane swiatla";
	fill_color_array(); // wypełnienie tablicy kolorów
	fill_array(); // wypełnienie tablicy współrzędnych

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(600, 600);

	glutCreateWindow("Oswietlenie scen 3D");

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