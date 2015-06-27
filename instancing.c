#include <time.h>
#include <stdlib.h>
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

float* randoms;

void setupInstancedVertexAttributes(GLuint prog, int count){
	glUseProgram(prog);
	glGenBuffers(1, &model_matrix_buffer);
	glGenBuffers(1, &test_buffer);
	srand(time(NULL));
	randoms = malloc(count * sizeof(float));
	for (int i = 0; i < count; i++) {
		randoms[i] = (float)rand() / (float)(RAND_MAX / 20.0) + 0.1;
		printf("%f\n", randoms[i]);
	}

}

void drawModelInstanced(Model *m, GLuint program, GLuint count, GLfloat time, mat4 transEverything) {
	glUseProgram(program);
	if (m != NULL)
		glBindVertexArray(m->vao);	// Select VAO
	else {
    printf("Warning warning, fuckup in drawmodelinstanced");
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);

	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(matrix_loc + i);
		glVertexAttribPointer(matrix_loc + i,            // Location
													4, GL_FLOAT, GL_FALSE,     // vec4
													sizeof(mat4),   // Stride
													(void*)(sizeof(vec4) * i)); // Start offset
		glVertexAttribDivisor(matrix_loc + i, 1);
	}
	mat4 model_matrixes[count];
	vec3 test_data[count];
	for (int pos = 0; pos < count; pos++) {
		model_matrixes[pos] = Mult(Mult(Mult(transEverything,
																				 Ry(time + (float)pos / randoms[pos])),
																		T((float)pos / 300 + randoms[pos] * (float)pos / 3000, (float)pos / 75, 1)),
															 Rz(time * (pos % 12) + randoms[pos]));
		model_matrixes[pos] = Transpose(model_matrixes[pos]);
		test_data[pos] = (vec3) { (float)pos / (float)count, (float)pos / (float)count, (float)pos / (float)count };

	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(model_matrixes), &model_matrixes, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, test_buffer);
	glVertexAttribPointer(test_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(test_data), test_data, GL_STATIC_DRAW);
	glVertexAttribDivisor(test_loc, 1);
	glEnableVertexAttribArray(test_loc);

	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position_loc);

	glBindBuffer(GL_ARRAY_BUFFER, m->nb);
	glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(normal_loc);

	if (m->texCoordArray != NULL) {
		glBindBuffer(GL_ARRAY_BUFFER, m->tb);
		glVertexAttribPointer(texture_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(texture_loc);
	}

	glDrawElementsInstanced(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L, count);
}
