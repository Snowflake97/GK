
#include <windows.h>
#include <gl/gl.h>
#include <glut.h>
#include <stdio.h>
#include<iostream>
using namespace std;

typedef float point2[2];

void setRandomColor()
{
	glColor3f(
		float(rand() % 255) / 255,
		float(rand() % 255) / 255,
		float(rand() % 255) / 255);
}

float creatDistortion(float edge, float percent)
{
	float percent_from_numbrt = edge*percent / 100;
	float min = edge - percent_from_numbrt;
	float max = edge + percent_from_numbrt;

	float distortion = min + static_cast <float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));

	return distortion;

}

int draw(point2 lg, point2 pg, int counter, float perc)
{
	float side = pg[0] - lg[0];
	float small_side = side / 3;
	point2 ld = { lg[0], lg[1] * (-1) };
	point2 pd = { pg[0], pg[1] * (-1) };

	if (counter == 1)
	{

		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
			
				

				point2 new_ld = { ld[0] + (creatDistortion(small_side,perc) * x) , ld[1] + (creatDistortion(small_side,perc) * y) };
				point2 new_lg = { ld[0] + (creatDistortion(small_side,perc) * x) , ld[1] + (creatDistortion(small_side,perc) * y) + creatDistortion(small_side,perc) };
				point2 new_pg = { ld[0] + (creatDistortion(small_side,perc) * x) + creatDistortion(small_side,perc), ld[1] + (creatDistortion(small_side,perc) * y) + creatDistortion(small_side,perc) };
				point2 new_pd = { ld[0] + (creatDistortion(small_side,perc) * x) + creatDistortion(small_side,perc), ld[1] + (creatDistortion(small_side,perc) * y) };

				if (x == 1 && y == 1)
				{
					glBegin(GL_POLYGON);
					glColor3f(0.0f, 0.0f, 0.0f);
					glVertex2fv(new_lg);
					glVertex2fv(new_pg);
					glVertex2fv(new_pd);
					glVertex2fv(new_ld);
					glEnd();
				}
				else
				{

					glBegin(GL_POLYGON);
					glColor3f(1, 1, 1);
					//setRandomColor();
					glVertex2fv(new_lg);
					//setRandomColor();
					glVertex2fv(new_pg);
					//setRandomColor();
					glVertex2fv(new_pd);
					//setRandomColor();
					glVertex2fv(new_ld);
					glEnd();
				}
			}
		}
	}

	if (counter > 1)
	{
		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				point2 new_ld = { ld[0] + (creatDistortion(small_side,perc) * x) , ld[1] + (creatDistortion(small_side,perc) * y) };
				point2 new_lg = { ld[0] + (creatDistortion(small_side,perc) * x) , ld[1] + (creatDistortion(small_side,perc) * y) + creatDistortion(small_side,perc) };
				point2 new_pg = { ld[0] + (creatDistortion(small_side,perc) * x) + creatDistortion(small_side,perc), ld[1] + (creatDistortion(small_side,perc) * y) + creatDistortion(small_side,perc) };
				point2 new_pd = { ld[0] + (creatDistortion(small_side,perc) * x) + creatDistortion(small_side,perc), ld[1] + (creatDistortion(small_side,perc) * y) };

				if (x == 1 && y == 1)
				{
					glBegin(GL_POLYGON);
					glColor3f(0.0f, 0.0f, 0.0f);
					glVertex2fv(new_lg);
					glVertex2fv(new_pg);
					glVertex2fv(new_pd);
					glVertex2fv(new_ld);
					glEnd();
				}
				else
				{
					draw(new_lg, new_pg, counter - 1, perc);
				}
			}
		}
	}


	return 0;
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym


	point2 lg = { -90,90 };
	point2 pg = { 90,90 };
	int counter = 2;
	float perc = 0;
	draw(lg, pg, counter, perc);
	glFlush();
	int input = 0;

	while (input != 5)
	{
		cout << "1. Zwieksz stopien" << endl << "2. Zminiejsz stopien" <<endl<<"3. Zwieksz perturbacje " <<endl << "4. Zmniejsz perturbacje"<< endl << "Wybor :";
		std::cin >> input;
		cout << endl << endl;
		if (input == 1) {
			counter++;
		}
		if (input == 2) {
			if (counter > 1) {
				counter--;
			}
		}

		if (input == 3) {
			perc= perc +1;
				}
		if (input == 4) {
			if (perc >= 1) {
				perc = perc - 1;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		draw(lg, pg, counter, perc);


		glFlush();
	}

	//glFlush();
	// Przekazanie poleceń rysujących do wykonania
}



void MyInit(void)

{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor okna wnętrza okna - ustawiono na szary
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)

// Parametry horizontal i vertical (szerokość i wysokość okna) są
// przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna

{
	GLfloat AspectRatio;

	// Deklaracja zmiennej AspectRatio określającej proporcję wymiarów okna

	if (vertical == 0)
		// Zabezpieczenie pzred dzieleniem przez 0

		vertical = 1;


	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkościokna okna urządzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)


	glMatrixMode(GL_PROJECTION);
	// Określenie układu współrzędnych obserwatora

	glLoadIdentity();
	// Określenie przestrzeni ograniczającej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie współczynnika proporcji okna

	// Gdy okno na ekranie nie jest kwadratem wymagane jest
	// określenie okna obserwatora.
	// Pozwala to zachować właściwe proporcje rysowanego obiektu
	// Do określenia okna obserwatora służy funkcja glOrtho(...)

	if (horizontal <= vertical)

		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);

	else

		glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	// Określenie układu współrzędnych    

	glLoadIdentity();

}

void main(void)
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wyświetlania
	// GLUT_SINGLE - pojedynczy bufor wyświetlania
	// GLUT_RGBA - model kolorów RGB
	glutCreateWindow("Dywan");
	// Utworzenie okna i określenie treści napisu w nagłówku okna
	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback)
	// Biblioteka GLUT będzie wywoływała tą funkcję za każdym razem, gdy
	// trzeba będzie przerysować okno
	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną za
	// zmiany rozmiaru okna
	MyInit();
	// Funkcja MyInit (zdefiniowana powyżej) wykonuje wszelkie 
	// inicjalizacje konieczneprzed przystąpieniem do renderowania
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}