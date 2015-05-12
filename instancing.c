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

void drawModelInstanced(Model *m, GLuint program, GLuint count, GLfloat time, mat4 transEverything) {
	glUseProgram(program);
	if (m != NULL)
		glBindVertexArray(m->vao);	// Select VAO
	else {
    printf("Warning warning, fuckup in drawmodelinstanced");
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, model_matrix_buffer);

	mat4 model_matrixes[count * count * count];
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(matrix_loc + i);
		glVertexAttribPointer(matrix_loc + i,            // Location
													4, GL_FLOAT, GL_FALSE,     // vec4
													sizeof(mat4),   // Stride
													(void*)(sizeof(vec4) * i)); // Start offset
		glVertexAttribDivisor(matrix_loc + i, 1);
	}
	vec3 test_data[count * count * count];
	for (int x = 0; x < count; x++) {
		for (int y = 0; y < count; y++) {
			for (int z = 0; z < count; z++) {
				int pos = x + y * count + z * count * count;
				model_matrixes[pos] = Mult(Mult(T(x * 2, y * 2, z * 2), transEverything), Rx(time));
				model_matrixes[pos] = Transpose(model_matrixes[pos]);
				test_data[pos] = (vec3) { (float)x / (float)count, (float)y / (float)count, (float)z / (float)count };
			}
		}
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

	glDrawElementsInstanced(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L, count * count * count);

}
