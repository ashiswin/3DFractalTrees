#include "Tree.h"
#include "glm/gtc/matrix_transform.hpp"
#include <stdio.h>
#include <math.h>

void Tree::clear() {
	branches.clear();
}

int Tree::getBranches() {
	return branches.size();
}

void Tree::drawTree() {
	glBindVertexArray(vao);
	
	glDrawArrays(GL_LINES, 0, branches.size());
	
	glBindVertexArray(0);
}

void Tree::generateFractalTree(int screenHeight) {
	int randVarRed = rand() % 3;
	if (randVarRed == 0)
		treeRed = 0.35f;
	else if (randVarRed == 1)
		treeRed = 0.25f;
	else if (randVarRed == 2)
		treeRed = 0.15f;
		
	int randVarGreen = rand() % 2;
	if (randVarGreen == 0)
		treeGreen = 0.15f;
	else if (randVarGreen == 1)
		treeGreen = 0.30f;
	Branch *branch = (Branch*) malloc(sizeof(Branch));
	branch->before = NULL;
	branch->angleAboutX = 0.0f;
	branch->angleAboutZ = 0.0f;
	branch->size = (rand() % screenHeight / 4) + 40.0f;
	branch->endPoint = glm::vec4(0.0f, branch->size, 0.0f, 1.0f);
	branch->branches = rand() % (maxBranches-minBranches) + 1 + 3;
	branch->after = (Branch*) malloc(sizeof(Branch) * branch->branches); // Allocate array for all child branches
	
	branches.push_back(branch);
	rootBranches.push_back(branch);
	
	printf("Root size %f\n", branch->size);
	for (int i = 0; i < branch->branches; i++) {
		int x = rand() % 2 == 1 ? -1 : 1;
		int z = rand() % 2 == 1 ? -1 : 1;
		generateFractalTree(branch, i, branch->size * (rand() % maxShrink + minShrink) / 1000.0f, (GLfloat) (x * rand() % maxAngle), (GLfloat) (z * rand() % maxAngle));
	}
	
	printf("Generating buffers...\n");
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &colorBuffer);
	
	printf("Buffer generation complete\n");
	
	GLfloat *vertices = (GLfloat*) malloc(sizeof(GLfloat) * branches.size() * 2 * 3);
	GLfloat *colors = (GLfloat*) malloc(sizeof(GLfloat) * branches.size() * 2 * 3);
	
	for(int i = 0; i < branches.size(); i++) {
		Branch *branch = branches.at(i);
		int vertexPos = i * 6;
		
		if(branch->before != NULL) {
			vertices[vertexPos] = branch->before->endPoint.x;
			vertices[vertexPos + 1] = branch->before->endPoint.y;
			vertices[vertexPos + 2] = branch->before->endPoint.z;
		}
		else {
			vertices[vertexPos] = 0.0f;
			vertices[vertexPos + 1] = 0.0f;
			vertices[vertexPos + 2] = 0.0f;
		}
		vertices[vertexPos + 3] = branch->endPoint.x;
		vertices[vertexPos + 4] = branch->endPoint.y;
		vertices[vertexPos + 5] = branch->endPoint.z;
		
		colors[vertexPos] = 1.0f / (branch->size * treeRed);
		colors[vertexPos + 1] = 1.0f / (branch->size * treeGreen);
		colors[vertexPos + 2] = 0.0f;
		colors[vertexPos + 3] = 1.0f / (branch->size * treeRed);
		colors[vertexPos + 4] = 1.0f / (branch->size * treeGreen);
		colors[vertexPos + 5] = 0.0f;
	}
	printf("Loading buffers...\n");
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * branches.size() * 2 * 3, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	printf("Loaded vertex buffer\n");
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * branches.size() * 2 * 3, colors, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint) 1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	
	printf("Loaded color buffer\n");
	glBindVertexArray(0);
	
	printf("Buffer load complete\n");
	printf("\nBranches: %lu\nVertices: %lu\nVertex Components: %lu\nColors: %lu\nColor Components: %lu\n\n", branches.size(), sizeof(vertices) / 3, sizeof(vertices) / 6, sizeof(colors) / 3, sizeof(colors) / 6);
	free(vertices);
	free(colors);
}

void Tree::generateFractalTree(Branch *branch, int branchNumber, GLfloat size, GLfloat degrees, GLfloat degrees2) {
	if (size < 5.0f || branch->endPoint.y - size < 10.0f)
		return;
	glm::mat4 transformMatrix(1.0f);
	glm::vec4 tempNewPos(0.0f, size, 0.0f, 1.0f);
	Branch *newBranch = &branch->after[branchNumber]; // Get address of allocated child branch
	
	transformMatrix = glm::translate(transformMatrix, branch->endPoint.xyz());	
	transformMatrix = glm::rotate(transformMatrix, (GLfloat) (degrees * 3.14159f / 180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	transformMatrix = glm::rotate(transformMatrix, (GLfloat) (degrees2 * 3.14159f / 180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 

	
	newBranch->endPoint = transformMatrix * tempNewPos;
	newBranch->size = size;
	newBranch->before = branch;
	newBranch->angleAboutZ = degrees;
	newBranch->angleAboutX = degrees2;
	
	branches.push_back(newBranch);
	
	newBranch->branches = rand() % (maxBranches-minBranches) + 1 + minBranches;
	newBranch->after = (Branch*) malloc(sizeof(Branch) * newBranch->branches);
	
	for (int i = 0; i < newBranch->branches; i++) {
		int x = rand() % 2 == 1 ? -1 : 1;
		int z = rand() % 2 == 1 ? -1 : 1;
		generateFractalTree(newBranch, i, size * (rand() % maxShrink + minShrink) / 1000.0f, degrees + (x * rand() % maxAngle), degrees2 + (z * rand() % maxAngle));
	}
}
