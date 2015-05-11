#include <math.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "./libraries/GLUtilities.h"
#include "./libraries/LoadObject.h"
#include "./libraries/LoadTGA.h"
#include "./libraries/VectorUtils3.h"

#include "main.h"

GLuint model_matrix_buffer;
GLuint test_buffer;

GLuint position_loc = 0;
GLuint normal_loc   = 1;
GLuint texture_loc  = 2;
GLuint test_loc     = 3;
GLuint matrix_loc   = 4;

void setupInstancedVertexAttributes(GLuint prog, Model* m){
	glUseProgram(prog);
	glGenBuffers(1, &model_matrix_buffer);
	glGenBuffers(1, &test_buffer);
}

void drawModelInstanced(Model *m, GLuint program, GLuint count, GLfloat time) {
	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);
	mat4 model_matrixes[count];
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(matrix_loc + i);
		glVertexAttribPointer(matrix_loc + i,            // Location
													4, GL_FLOAT, GL_FALSE,     // vec4
													sizeof(GLfloat) * 4 * 4,   // Stride
													(void*)(sizeof(GLfloat) * 4 * i)); // Start offset
		glVertexAttribDivisor(matrix_loc + i, 1);
	}

	for (int i = 0; i < count; i++) {
		float I = (float)i;
		model_matrixes[i] = (mat4) {1, 1, 1, 1,
																1, 1, 1, 1,
																1, 1, 1, 1,
																1, 1, 1, 1 };
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(model_matrixes), &model_matrixes, GL_STATIC_DRAW);


	vec4* test_data = malloc(sizeof(vec4) * m->numVertices);
	for (int i = 0; i < m->numVertices; i++) {
		test_data[i] = (vec4) { 1.0, 1.0, 1.0, 1.0 };
	}
	glBindBuffer(GL_ARRAY_BUFFER, test_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * m->numVertices,  &test_data, GL_STATIC_DRAW);
	glVertexAttribPointer(test_loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(test_loc);


	if (m != NULL){
		glBindVertexArray(m->vao);	// Select VAO
		glBindBuffer(GL_ARRAY_BUFFER, m->vb);
		glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(position_loc);

		glBindBuffer(GL_ARRAY_BUFFER, m->nb);
		glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(normal_loc);


		// VBO for texture coordinate data NEW for 5b
		if (m->texCoordArray != NULL) {
				glBindBuffer(GL_ARRAY_BUFFER, m->tb);
				glVertexAttribPointer(texture_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(texture_loc);
			}

		glDrawElementsInstanced(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L, count);
	}
}
