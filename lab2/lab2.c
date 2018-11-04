#include <stdio.h>
#include <math.h>
#include <omp.h>

const int NumRectangles = 10;

int main() {

	/* We want a variable for the width of each rectangle. The total width is 1
	 * because we are working with a quarter unit circle. */
	float width = 1.0 / NumRectangles;

	/* We want a variable for the x-coordinate at which each rectangle intersects
	 * the unit circle */
	float x;

	/* We want a variable for the height of each rectangle */
	float height;

	/* We want a variable to keep a running total of the area of the rectangles */
	float area = 0.0;

	/* We want a variable for the computed value of pi */
	float pi;

	/* We want a variable to loop through the rectangles */
	int i;

	/* We want to loop over each of the rectangles and run the same code for
	 * each */
	#pragma omp parallel for private(x, i, height) reduction(+:area)
	{
		for (i = 0; i < NumRectangles; i++) {
			printf("My thread ID is %d\n", omp_get_thread_num());
			/* We want to calculate the x-coordinate where the rectangle intersects the
			 * circle by counting how many rectangle widths the rectangle is away from
			 * the origin */
			x = i * width;

			/* We want to use the x^2 + y^2 = r^2 equation to calculate the height of
			 * the rectangle, which is where the rectangle intersects the circle (y).
			 * Because the circle is a unit circle, r = 1. */
			height = sqrt(1.0 - x * x);

			/* We want to compute the area of the rectangle and add it to our running
			 * total */
			area += width * height;

		}
	}
	/* We are ready to compute pi. We need to multiply by 4 because we have found
	 * the area under only 1/4 of the unit circle. We don't need to multiply by
	 * radius squared, because the unit circle has a radius of 1. */
	pi = 4.0 * area;

	/* We want to print the final result */
	printf("%f\n", pi);

	/* We want to indicate the program has finished successfully using the Unix
	 * standard value for success */
	return 0;
}