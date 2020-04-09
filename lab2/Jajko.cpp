
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <gl/gl.h>
#include <glut.h>
#include <math.h>
#include <conio.h>
using namespace std;


typedef float point3[3]; //definicja punktów o 3 wartościach
const int N = 150; // ustalenie stałej N
point3 tab[N][N];  // definicja tablicy puntków
point3 colors[N][N];	// definicja tablicy kolorów
int model = 1;	// start programu z widokiem na chmurę punktów
static GLfloat theta[] = { 0.0, 0.0, 0.0 }; // zmienna globalna kątów obrotu

/*************************************************************************************/


// generator tablicy losowych kolorów o rozmiarze NxN 
void fill_color_array()
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			colors[i][j][0] = float(rand() % 256) / 256;
			colors[i][j][1] = float(rand() % 256) / 256;
			colors[i][j][2] = float(rand() % 256) / 256;
		}
	}
}
// funkcja zwrotna ustalajaca kąty obrotów i odświeżająca ekran
void spinEgg()
{

	theta[0] -= 0.1;
	if (theta[0] > 360.0) theta[0] -= 360.0;

	theta[1] -= 0.1;
	if (theta[1] > 360.0) theta[1] -= 360.0;

	theta[2] -= 0.1;
	if (theta[2] > 360.0) theta[2] -= 360.0;

	glutPostRedisplay(); //odświeżenie zawartości aktualnego okna
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
			if (i < N-1) {
				glVertex3fv(tab[i + 1][j]);
			}
			else {
				glVertex3fv(tab[0][0]);
				glVertex3fv(tab[i][j]);

			}
			glEnd();

			// rysowanie liń poziomych
			glBegin(GL_LINES);
			if (j < N -1) {
				glVertex3fv(tab[i][j+1]);
				glVertex3fv(tab[i][j]);
			}
			else {
				glVertex3fv(tab[i][j]);
				if (i > 0) glVertex3fv(tab[N-i][0]);
			}
			glEnd();
			// rysowanie liń ukosnych
			glBegin(GL_LINES);
			if (j < N - 1 && i <N-1) {
				glVertex3fv(tab[i+1][j]);
				glVertex3fv(tab[i][j+1]);
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
			glColor3fv(colors[i][j]);
			glVertex3fv(tab[i][j]);
			glColor3fv(colors[(i + 1) % N][j]);
			glVertex3fv(tab[(i + 1) % N][j]);
			glColor3fv(colors[i][j + 1]);
			glVertex3fv(tab[i][j + 1]);
			glEnd();

			glBegin(GL_TRIANGLES);
			glColor3fv(colors[(i + 1) % N][j]);
			glVertex3fv(tab[(i + 1) % N][j]);
			glColor3fv(colors[(i + 1) %N][j + 1]);
			glVertex3fv(tab[(i + 1) % N][j + 1]);
			glColor3fv(colors[i][j + 1]);
			glVertex3fv(tab[i][j + 1]);
			glEnd();

			if (i > 0) {
				glBegin(GL_TRIANGLES);
				glColor3fv(colors[i][j]);
				glVertex3fv(tab[i][j]);
				glColor3fv(colors[(i + 1) % N][j]);
				glVertex3fv(tab[(i + 1) % N][j]);
				glColor3fv(colors[N - i][0]);
				glVertex3fv(tab[N - i][0]);
				glEnd();
			}
			glBegin(GL_TRIANGLES);
			glColor3fv(colors[(i + 1) % N][j]);
			glVertex3fv(tab[(i + 1) % N][j]);
			glColor3fv(colors[(N - i) % N][0]);
			glVertex3fv(tab[(N - i) % N][0]);
			glColor3fv(colors[N - i - 1][0]);
			glVertex3fv(tab[N - i - 1][0]);
			glEnd();

			}


		}
	}






// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba
// przerysować scenę)

void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej
	glColor3f(0.0f, 1.0f, 0.0f);

	glRotatef(theta[0], 1.0, 0.0, 0.0);

	glRotatef(theta[1], 0.0, 1.0, 0.0);

	glRotatef(theta[2], 0.0, 0.0, 1.0);


	if (model == 1) {
		draw_egg();
	}

	if (model == 2) {
		glColor3f(1.0f, 1.0f, 1.0f);
		draw_egg_as_net();
	}
	if (model == 3) {
		draw_egg_as_triangle();
	}

	glFlush();
	glutSwapBuffers();
	//

}

/*************************************************************************************/

// Funkcja ustalająca stan renderowania
void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 1; // wywolanie punktow
	if (key == 'w') model = 2; // wywolanie siatki
	if (key == 's') model = 3; // wywolanie pomalowanego jajka z trojkatow

	RenderScene();
}


void MyInit(void)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszcący (wypełnienia okna) ustawiono na czarny

}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  określającej proporcję
	// wymiarów okna 

	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0

		vertical = 1;

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkościokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  

	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji 

	glLoadIdentity();
	// Czyszcznie macierzy bieżącej            

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie współczynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest określenie tak zwanej
	// przestrzeni ograniczającej pozwalającej zachować właściwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczjącej służy funkcja
	// glOrtho(...)            

	if (horizontal <= vertical)

		glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);

	else

		glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);

	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu                                    

	glLoadIdentity();
	// Czyszcenie macierzy bieżącej

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli



void main(void)
{
	fill_color_array(); // wypełnienie tablicy kolorów
	fill_array(); // wypełnienie tablicy współrzędnych
	


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);
	glutIdleFunc(spinEgg); 
	glutCreateWindow("Układ współrzędnych 3-D");
	glutKeyboardFunc(keys);

	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną
	// (callback function).  Bedzie ona wywoływana za każdym razem
	// gdy zajdzie potrzba przeryswania okna 

	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
	// zazmiany rozmiaru okna      

	MyInit();
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przystąpieniem do renderowania 

	glEnable(GL_DEPTH_TEST);
	// Włączenie mechanizmu usuwania powierzchni niewidocznych

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

}

/*************************************************************************************/

