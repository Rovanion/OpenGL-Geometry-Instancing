#include <math.h>
#include <GL/gl.h>

#include "./libraries/GLUtilities.h"
#include "./libraries/LoadObject.h"
#include "./libraries/LoadTGA.h"
#include "./libraries/VectorUtils3.h"

#include "main.h"

mat4 model_matrix = (mat4) {0, 0, 0, 0,
														0, 0, 0, 0,
														0, 0, 0, 0,
														0, 0, 0, 0 };
GLuint model_matrix_buffer;
GLuint color_buffer;

GLuint position_loc = 0;
GLuint normal_loc   = 1;
GLuint texture_loc    = 2;
GLuint matrix_loc   = 3;

void setupInstancedVertexAttributes(GLuint prog, Model* m){
	glUseProgram(prog);
	glBindVertexArray(m->vao);	// Select VAO
	// Get the locations of the vertex attributes in "prog", which is
	// the (linked) program object that we're going to be rendering
	// with. Note that this isn't really necessary because we specified
	// locations for all the attributes in our vertex shader. This code
	// could be made more concise by assuming the vertex attributes are
	// where we asked the compiler to put them.


	// Configure the regular vertex attribute arrays -
	// position and normal.
	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glVertexAttribPointer(position_loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position_loc);

	glBindBuffer(GL_ARRAY_BUFFER, m->nb);
	glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(normal_loc);

	// Likewise, we can do the same with the model matrix. Note that a
	// matrix input to the vertex shader consumes N consecutive input
	// locations, where N is the number of columns in the matrix. So...
	// we have four vertex attributes to set up.
	glGenBuffers(1, &model_matrix_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices*2*sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
	printError("setupinstancedvertexattributes(): beforeLoop");

	// Loop over each column of the matrix...
	for (int i = 0; i < 4; i++)
		{
			// Set up the vertex attribute
			glVertexAttribPointer(matrix_loc + i,              // Location
														4, GL_FLOAT, GL_FALSE,       // vec4
														0,                // Stride
														0); // Start offset
			printError("setupinstancedvertexattributes(): inLoop");
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
			float a = 0.2f + (float)n;
			float b = 0.5f + (float)n;
			float c = 1.0f + (float)n;

			matrices[n] = T(a, b, c);
		}

	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glVertexAttribPointer(position_loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position_loc);

	glBindBuffer(GL_ARRAY_BUFFER, m->nb);
	glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(normal_loc);

	glBindBuffer(GL_ARRAY_BUFFER, m->tb);
	glVertexAttribPointer(texture_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(texture_loc);

	// Done. Unmap the buffer.
	glUnmapBuffer(GL_ARRAY_BUFFER);
	// Activate instancing program
	glUseProgram(program);

	// Render INSTANCE_COUNT objects
	glDrawArraysInstanced(GL_TRIANGLES, 0, m->numIndices, count);
	printf("%d\n", m->nb);
}
