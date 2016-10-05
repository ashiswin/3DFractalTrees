#ifndef TREE_H
#define TREE_H

#define GLM_SWIZZLE
#include <vector>
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/gl.h>
#include "glm/glm.hpp"

using namespace std;

struct Branch {
	Branch *before;
	Branch *after;
	int branches;
	glm::vec4 endPoint;
	GLfloat angleAboutZ, angleAboutX;
	GLfloat size;
};

class Tree {
	private:
		static const int maxShrink = 900; // per thousand
		static const int minShrink = 1.0f;
		static const int maxAngle = 45;
		static const int maxBranches = 8;
		static const int minBranches = 0;
		
		vector<Branch*> branches;
		vector<Branch*> rootBranches;
		GLfloat treeRed;
		GLfloat treeGreen;
		
		GLuint vao;
		GLuint vertexBuffer, colorBuffer;
	public:
		void clear();
		int getBranches();
		void generateFractalTree(int screenHeight);
		void drawTree();
		void generateFractalTree(Branch *branch, int branchNumber, GLfloat size, GLfloat degrees, GLfloat degrees2);
};

#endif
