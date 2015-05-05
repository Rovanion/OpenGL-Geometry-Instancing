#include <math.h>
#include <GL/gl.h>

#include "./libraries/GLUtilities.h"
#include "./libraries/LoadObject.h"
#include "./libraries/LoadTGA.h"
#include "./libraries/VectorUtils3.h"

#include "main.h"

GLuint model_matrix_buffer;

void setupInstancedVertexAttributes(GLuint prog, Model* m){
	glBindVertexArray(m->vao);	// Select VAO
	// Get the locations of the vertex attributes in "prog", which is
	// the (linked) program object that we're going to be rendering
	// with. Note that this isn't really necessary because we specified
	// locations for all the attributes in our vertex shader. This code
	// could be made more concise by assuming the vertex attributes are
	// where we asked the compiler to put them.
	int position_loc    = glGetAttribLocation(prog, "position");
	int normal_loc      = glGetAttribLocation(prog, "normal");
	int color_loc       = glGetAttribLocation(prog, "color");
	int matrix_loc      = glGetAttribLocation(prog, "model_matrix");

	// Configure the regular vertex attribute arrays -
	// position and normal.
	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glVertexAttribPointer(position_loc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(position_loc);
	glBindBuffer(GL_ARRAY_BUFFER, m->nb);
	glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(normal_loc);

	// Now we set up the color array. We want each instance of our
	// geometry to assume a different color, so we'll just pack colors
	// into a buffer object and make an instanced vertex attribute out
	// of it.
	glBindBuffer(GL_ARRAY_BUFFER, m->tb);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(color_loc);
	// This is the important bit... set the divisor for the color array
	// to 1 to get OpenGL to give us a new value of "color" per-instance
	// rather than per-vertex.
	glVertexAttribDivisor(color_loc, 1);

	// Likewise, we can do the same with the model matrix. Note that a
	// matrix input to the vertex shader consumes N consecutive input
	// locations, where N is the number of columns in the matrix. So...
	// we have four vertex attributes to set up.
	glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);
	// Loop over each column of the matrix...
	for (int i = 0; i < 4; i++)
		{
			// Set up the vertex attribute
			glVertexAttribPointer(matrix_loc + i,              // Location
														4, GL_FLOAT, GL_FALSE,       // vec4
														sizeof(mat4),                // Stride
														(void *)(sizeof(vec4) * i)); // Start offset
			// Enable it
			glEnableVertexAttribArray(matrix_loc + i);
			// Make it instanced
			glVertexAttribDivisor(matrix_loc + i, 1);
		}
}


void drawInstances(GLuint program, GLuint count, GLfloat time, Model* m) {
	// Map the buffer
	mat4 * matrices = (mat4 *)glMapBuffer(GL_ARRAY_BUFFER,
																				GL_WRITE_ONLY);

	// Set model matrices for each instance
	for (GLuint n = 0; n < count; n++)
		{
			float a = 50.0f * (float)n / 4.0f;
			float b = 50.0f * (float)n / 5.0f;
			float c = 50.0f * (float)n / 6.0f;

			matrices[n] = T(a, b, c);
		}

	// Done. Unmap the buffer.
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Activate instancing program
	glUseProgram(program);

	// Render INSTANCE_COUNT objects
	glDrawArraysInstanced(GL_TRIANGLES, 0, m->numIndices, count);
}
