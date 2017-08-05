#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

int main() {

	glm::vec4 from(1, 0, 0, 0);
	glm::mat4 tm(1.0);
	glm::mat4 model2;
	glm::mat4 model = glm::rotate(tm, (float)glm::radians(90.0f), glm::vec3(0, 0, 1));
	model2 = glm::rotate(tm, (float)glm::radians(90.0f), glm::vec3(1, 0, 0));
	
	glm::vec4 tmp = model2 * model * from;

	return 0;
}