// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow *window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/texture.hpp>


/* scroll */

float fov = 45.0f;


/*mouse*/
float mouse_Speed = 0.15f;
bool firstPress = true;
double xpos_prev = 0.0;
double ypos_prev = 0.0;
double xpos = 0.0;
double ypos = 0.0;


float yaw_val = -90.0f;		// 초기 yaw 값
float pitch_val = 0.0f;		// 초기 pitch 값


bool start_flag = false;
bool prev_space_state = false;


int global_light_flag = 1;
int horse_light_flag = 1;
int camera_light_flag = 0;

// Initial horizontal angle : toward -Z
float horizontal_Angle = 0.0f;
// Initial vertical angle : none
float vertical_Angle = 0.0f;


//glm::mat4 View_Matrix;

glm::mat4 Projection;

// 뷰행렬 조건
glm::vec3 camera_Pos;
glm::vec3 camera_direction;
glm::vec3 camera_up;


void press_spacebar(float *rotSpeed, float *moveSpeed, float *deltaTime);
void computeMouseRotates();
void computeKeyboardTranslates();
void draw_object(struct IDs IDs, struct Models models, struct buffer_info buffers, struct other_info);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void set_Entire_light_flag();

//빛의 위치
glm::vec3 lightPos1 = vec3(6.0f, 3.0f, 3.0f);
glm::vec3 lightPos2 = vec3(0.0f, 1.0f, 0.0f);


glm::mat4 View;


struct IDs {
	GLuint modelMatrixID;
	GLuint MatrixID;
	GLuint ColorCheckID;
	GLuint lightCheckID;
};

struct Models {
	glm::mat4 Model;
	glm::mat4 MVP;
};

struct buffer_info {
	GLuint vertexbuffer;
	GLuint normalbuffer;
	GLuint uvbuffer;
};

struct other_info {
	int color_num;
	int light_num;
	int vertex_size;
};

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 04 - Colored Cube", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.6f, 0.6f, 0.8f, 0.0f);		// R G B A

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("TransformVertexShader.glsl", "ColorFragmentShader.glsl");

	// 텍스쳐 1
	GLuint Texture = loadBMP_custom("oak_wood.bmp");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ColorCheckID = glGetUniformLocation(programID, "colorCheck");
	GLuint lightCheckID = glGetUniformLocation(programID, "light_num");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);



	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	GLuint uvbuffer1;
	glGenBuffers(1, &uvbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);



	// Read our .obj file
	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec2> uvs2;
	std::vector<glm::vec3> normals2;
	bool res2 = loadOBJ("cylinder.obj", vertices2, uvs2, normals2);


	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);

	GLuint normalbuffer2;
	glGenBuffers(1, &normalbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
	glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(glm::vec3), &normals2[0], GL_STATIC_DRAW);



	GLuint uvbuffer2;
	glGenBuffers(1, &uvbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
	glBufferData(GL_ARRAY_BUFFER, uvs2.size() * sizeof(glm::vec2), &uvs2[0], GL_STATIC_DRAW);



	//shader에 model, view, projection matrix를 전송 하기 위한 uniform 변수
	GLint modelMatrixID = glGetUniformLocation(programID, "model");
	GLint viewMatrixID = glGetUniformLocation(programID, "view");
	GLint projMatrixID = glGetUniformLocation(programID, "projection");

	//셰이더 안에서 사용할 objectColor, lightColor, lightPos, viewPos uniform변수의 위치를 받아옴
	GLint lightColorID = glGetUniformLocation(programID, "lightColor");
	GLint lightPosID = glGetUniformLocation(programID, "lightPos");
	GLint lightPos2ID = glGetUniformLocation(programID, "lightPos2");
	GLint viewPosID = glGetUniformLocation(programID, "viewPos");

	GLint global_light_flag_ID = glGetUniformLocation(programID, "global_light_flag");
	GLint horse_light_flag_ID = glGetUniformLocation(programID, "horse_light_flag");

	GLint cameraPosID = glGetUniformLocation(programID, "cameraPos");
	GLint cameraDirID = glGetUniformLocation(programID, "cameraDir");
	GLint camera_light_flag_ID = glGetUniformLocation(programID, "camera_light_flag");



	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(fov), 4.0f / 3.0f, 0.1f, 100.0f);

	// Camera matrix
	// 뷰행렬의 초기값
	camera_Pos = glm::vec3(1, 1, 11);
	camera_direction = glm::vec3(cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f)), sin(glm::radians(0.0f)), sin(glm::radians(-90.0f)) * cos(glm::radians(0.0f)));
	camera_up = glm::vec3(0, 1, 0);

	View = glm::lookAt(
		camera_Pos,		// Camera , in World Space
		camera_Pos + camera_direction,	// and looks at the origin
		camera_up		// Head is up (set to 0,-1,0 to look upside-down)
	);


	// For speed computation
	double lastTime = glfwGetTime();
	double lastFrameTime = lastTime;
	float gOrientation = 0.0f;
	float rotSpeed = 0.0f;
	float moveSpeed = 0.0f;

	// 중앙기둥 위치
	glm::vec3 gPosition1(0.0f, 0.0f, 0.0f);		// x y z

	// 기둥1의 초기위치
	glm::vec3 gPosition2(200.0f, 0.0f, 0.0f);	// 기둥1

	// 기둥2의 초기 위치
	glm::vec3 pillar2_position(-200.0f, 0.0f, 0.0f);	// 기둥2

	// 기둥3의 초기위치
	glm::vec3 pillar3_position(125.0f, 0.0f, 175.0f);	// 기둥3

	// 기둥4의 초기위치
	glm::vec3 pillar4_position(-125.0f, 0.0f, -175.0f);	// 기둥4


	// 기둥5의 초기위치
	glm::vec3 pillar5_position(125.0f, 0.0f, -175.0f);	// 기둥5


	// 기둥6의 초기위치
	glm::vec3 pillar6_position(-125.0f, 0.0f, 175.0f);		// 기둥5

	glm::vec3 gPosition4(0.0f, 1.5f, 0.0f);		// 윗부분

	glm::vec3 gPosition5(0.0f, -1.5f, 0.0f);		// 아랫부분

	// 말1
	glm::vec3 gPosition6(2.0f, 0.0f, 0.0f);

	// 말의 다리1
	glm::vec3 gPosition7(0.25f, -0.5f, 0.25f);
	// 말의 다리2
	glm::vec3 gPosition8(0.25f, -0.5f, -0.25f);
	// 말의 다리3
	glm::vec3 gPosition9(-0.25f, -0.5f, 0.25f);
	// 말의 다리4
	glm::vec3 gPosition10(-0.25f, -0.5f, -0.25f);
	// 말의 목
	glm::vec3 gPosition11(0.0f, 0.3f, -0.4f);
	// 말의 얼굴
	glm::vec3 gPosition12(0.0f, 0.12f, -0.15f);


	// 말2
	glm::vec3 horse2_position(-2.0f, 0.2f, 0.0f);

	// 말2의 목
	glm::vec3 horse2_neck(0.0f, 0.3f, 0.4f);
	// 말2의 얼굴
	glm::vec3 horse2_face(0.0f, 0.12f, 0.15f);



	// 말3
	glm::vec3 horse3_position(1.25f, -0.2f, 1.75f);


	// 말3의 다리1
	glm::vec3 horse3_leg1(0.0f, -0.5f, 0.35f);
	// 말3의 다리2
	glm::vec3 horse3_leg2(0.0f, -0.5f, -0.35f);
	// 말3의 다리3
	glm::vec3 horse3_leg3(0.35f, -0.5f, 0.0f);
	// 말3의 다리4
	glm::vec3 horse3_leg4(-0.35f, -0.5f, 0.0f);


	// 말3의 목
	glm::vec3 horse3_neck(0.3f, 0.3f, -0.3f);
	// 말3의 얼굴
	glm::vec3 horse3_face(0.10f, 0.12f, -0.10f);


	// 말4
	glm::vec3 horse4_position(-1.25f, 0.4f, -1.75f);

	// 말4의 목
	glm::vec3 horse4_neck(-0.3f, 0.3f, 0.3f);
	// 말4의 얼굴
	glm::vec3 horse4_face(-0.10f, 0.12f, 0.10f);



	// 말5
	glm::vec3 horse5_position(1.25f, -0.1f, -1.75f);


	// 말5의 다리1
	glm::vec3 horse5_leg1(0.0f, -0.5f, 0.35f);
	// 말5의 다리2
	glm::vec3 horse5_leg2(0.0f, -0.5f, -0.35f);
	// 말5의 다리3
	glm::vec3 horse5_leg3(0.35f, -0.5f, 0.0f);
	// 말5의 다리4
	glm::vec3 horse5_leg4(-0.35f, -0.5f, 0.0f);


	// 말5의 목
	glm::vec3 horse5_neck(-0.3f, 0.3f, -0.3f);
	// 말5의 얼굴
	glm::vec3 horse5_face(-0.10f, 0.12f, -0.10f);



	// 말6
	glm::vec3 horse6_position(-1.25f, 0.1f, 1.75f);


	// 말6의 다리1
	glm::vec3 horse6_leg1(0.0f, -0.5f, 0.35f);
	// 말6의 다리2
	glm::vec3 horse6_leg2(0.0f, -0.5f, -0.35f);
	// 말6의 다리3
	glm::vec3 horse6_leg3(0.35f, -0.5f, 0.0f);
	// 말6의 다리4
	glm::vec3 horse6_leg4(-0.35f, -0.5f, 0.0f);


	// 말6의 목
	glm::vec3 horse6_neck(0.3f, 0.3f, 0.3f);
	// 말6의 얼굴
	glm::vec3 horse6_face(0.10f, 0.12f, 0.10f);


	// 이동 방향을 결정하는 flag
	int flag_first_horse = 0;
	int flag_second_horse = 0;
	int flag_third_horse = 0;
	int flag_fourth_horse = 0;
	int flag_fifth_horse = 0;
	int flag_sixth_horse = 0;


	glfwSetScrollCallback(window, scroll_callback);


	do
	{


		// Compute the Model matrix from keyboard and mouse input

		computeKeyboardTranslates();


		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			if (firstPress)
			{
				glfwGetCursorPos(window, &xpos_prev, &ypos_prev);
				firstPress = false;
			}

			computeMouseRotates();
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			firstPress = true;
		}



		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - lastFrameTime);
		lastFrameTime = currentTime;


		press_spacebar(&rotSpeed, &moveSpeed, &deltaTime);
		set_Entire_light_flag();
		glUniform1i(global_light_flag_ID, global_light_flag);
		glUniform1i(horse_light_flag_ID, horse_light_flag);
		glUniform1i(camera_light_flag_ID, camera_light_flag);


		glUniform3f(cameraPosID, camera_Pos.x, camera_Pos.y, camera_Pos.z);
		glUniform3f(cameraDirID, camera_direction.x, camera_direction.y, camera_direction.z);

		gOrientation += 3.14159f * deltaTime * rotSpeed;



		//경계값을 넘어가면 flag동작
		if (gPosition6.y > 0.4f)
			flag_first_horse = 1;
		else if (gPosition6.y < -0.4f)
			flag_first_horse = 0;


		// 전체 모델의 y축 움직임
		if (flag_first_horse == 0)
			gPosition6.y += deltaTime * moveSpeed; // 위로 움직임
		else
			gPosition6.y -= deltaTime * moveSpeed; // 아래로 움직임



		//경계값을 넘어가면 flag동작
		if (horse2_position.y > 0.4f)
			flag_second_horse = 1;
		else if (horse2_position.y < -0.4f)
			flag_second_horse = 0;


		// 전체 모델의 y축 움직임
		if (flag_second_horse == 0)
			horse2_position.y += deltaTime * moveSpeed; // 위로 움직임
		else
			horse2_position.y -= deltaTime * moveSpeed; // 아래로 움직임


		//경계값을 넘어가면 flag동작
		if (horse3_position.y > 0.4f)
			flag_third_horse = 1;
		else if (horse3_position.y < -0.4f)
			flag_third_horse = 0;


		// 전체 모델의 y축 움직임
		if (flag_third_horse == 0)
			horse3_position.y += deltaTime * moveSpeed; // 위로 움직임
		else
			horse3_position.y -= deltaTime * moveSpeed; // 아래로 움직임


		//경계값을 넘어가면 flag동작
		if (horse4_position.y > 0.4f)
			flag_fourth_horse = 1;
		else if (horse4_position.y < -0.4f)
			flag_fourth_horse = 0;



		// 전체 모델의 y축 움직임
		if (flag_fourth_horse == 0)
			horse4_position.y += deltaTime * moveSpeed; // 위로 움직임
		else
			horse4_position.y -= deltaTime * moveSpeed; // 아래로 움직임



		//경계값을 넘어가면 flag동작
		if (horse5_position.y > 0.4f)
			flag_fifth_horse = 1;
		else if (horse5_position.y < -0.4f)
			flag_fifth_horse = 0;



		// 전체 모델의 y축 움직임
		if (flag_fifth_horse == 0)
			horse5_position.y += deltaTime * moveSpeed; // 위로 움직임
		else
			horse5_position.y -= deltaTime * moveSpeed; // 아래로 움직임




		//경계값을 넘어가면 flag동작
		if (horse6_position.y > 0.4f)
			flag_sixth_horse = 1;
		else if (horse6_position.y < -0.4f)
			flag_sixth_horse = 0;



		// 전체 모델의 y축 움직임
		if (flag_sixth_horse == 0)
			horse6_position.y += deltaTime * moveSpeed; // 위로 움직임
		else
			horse6_position.y -= deltaTime * moveSpeed; // 아래로 움직임




		// 회전 목마의 중앙기둥
		glm::mat4 RotationMatrix1 = eulerAngleYXZ(gOrientation, 0.0f, 0.0f);
		glm::mat4 ScalingMatrix1 = scale(mat4(), vec3(0.01f, 0.07f, 0.01f));
		glm::mat4 Model1 = glm::mat4(1.0f);
		Model1 = RotationMatrix1 * ScalingMatrix1;
		glm::mat4 MVP1 = Projection * View * Model1;

		// 회전 목마1의 기둥
		glm::mat4 TranslationMatrix2 = translate(mat4(), gPosition1 + gPosition2);		// gPosition2 만큼 이동
		glm::mat4 ScalingMatrix2 = scale(mat4(), vec3(0.01f, 0.08f, 0.01f));
		glm::mat4 Model2 = glm::mat4(1.0f);
		Model2 = RotationMatrix1 * ScalingMatrix2 * TranslationMatrix2;
		glm::mat4 MVP2 = Projection * View * Model2;

		// 회전 목마2의 기둥
		glm::mat4 TranslationMatrix_pillar2 = translate(mat4(), gPosition1 + pillar2_position);		// gPosition3 만큼 이동
		glm::mat4 Model_pillar2 = glm::mat4(1.0f);
		Model_pillar2 = RotationMatrix1 * ScalingMatrix2 * TranslationMatrix_pillar2;
		glm::mat4 MVP_pillar2 = Projection * View * Model_pillar2;


		// 회전 목마3의 기둥
		glm::mat4 TranslationMatrix_pillar3 = translate(mat4(), gPosition1 + pillar3_position);		// horse3_position 만큼 이동
		glm::mat4 Model_pillar3 = glm::mat4(1.0f);
		Model_pillar3 = RotationMatrix1 * ScalingMatrix2 * TranslationMatrix_pillar3;
		glm::mat4 MVP_pillar3 = Projection * View * Model_pillar3;


		// 회전 목마4의 기둥
		glm::mat4 TranslationMatrix_pillar4 = translate(mat4(), gPosition1 + pillar4_position);		// horse4_position 만큼 이동
		glm::mat4 Model_pillar4 = glm::mat4(1.0f);
		Model_pillar4 = RotationMatrix1 * ScalingMatrix2 * TranslationMatrix_pillar4;
		glm::mat4 MVP_pillar4 = Projection * View * Model_pillar4;


		// 회전 목마5의 기둥
		glm::mat4 TranslationMatrix_pillar5 = translate(mat4(), gPosition1 + pillar5_position);		// horse5_position 만큼 이동
		glm::mat4 Model_pillar5 = glm::mat4(1.0f);
		Model_pillar5 = RotationMatrix1 * ScalingMatrix2 * TranslationMatrix_pillar5;
		glm::mat4 MVP_pillar5 = Projection * View * Model_pillar5;

		// 회전 목마6의 기둥
		glm::mat4 TranslationMatrix_pillar6 = translate(mat4(), gPosition1 + pillar6_position);		// horse6_position 만큼 이동
		glm::mat4 Model_pillar6 = glm::mat4(1.0f);
		Model_pillar6 = RotationMatrix1 * ScalingMatrix2 * TranslationMatrix_pillar6;
		glm::mat4 MVP_pillar6 = Projection * View * Model_pillar6;



		// 회전목마 윗부분
		glm::mat4 TranslationMatrix4 = translate(mat4(), gPosition1 + gPosition4);
		glm::mat4 ScalingMatrix4 = scale(mat4(), vec3(0.2f, 0.01f, 0.2f));
		glm::mat4 Model4 = RotationMatrix1 * TranslationMatrix4 * ScalingMatrix4;
		glm::mat4 MVP4 = Projection * View * Model4;

		// 회전목마 바닥
		glm::mat4 TranslationMatrix5 = translate(mat4(), gPosition1 + gPosition5);
		glm::mat4 Model5 = RotationMatrix1 * TranslationMatrix5 * ScalingMatrix4;
		glm::mat4 MVP5 = Projection * View * Model5;


		/* 말 1 */

		// 말 1 몸통
		glm::mat4 TranslationMatrix6 = translate(mat4(), gPosition1 + gPosition6);
		glm::mat4 ScalingMatrix6 = scale(mat4(), vec3(0.3f, 0.25f, 0.5f));
		glm::mat4 Model6 = RotationMatrix1 * TranslationMatrix6 * ScalingMatrix6;
		glm::mat4 MVP6 = Projection * View * Model6;

		// 말1의 다리1
		glm::mat4 TranslationMatrix7 = translate(mat4(), gPosition1 + gPosition6 + gPosition7);
		glm::mat4 ScalingMatrix7 = scale(mat4(), vec3(0.05f, 0.25f, 0.05f));
		glm::mat4 Model7 = RotationMatrix1 * TranslationMatrix7 * ScalingMatrix7;
		glm::mat4 MVP7 = Projection * View * Model7;

		// 말1의 다리2
		glm::mat4 TranslationMatrix8 = translate(mat4(), gPosition1 + gPosition6 + gPosition8);
		glm::mat4 Model8 = RotationMatrix1 * TranslationMatrix8 * ScalingMatrix7;
		glm::mat4 MVP8 = Projection * View * Model8;

		// 말1의 다리3
		glm::mat4 TranslationMatrix9 = translate(mat4(), gPosition1 + gPosition6 + gPosition9);
		glm::mat4 Model9 = RotationMatrix1 * TranslationMatrix9 * ScalingMatrix7;
		glm::mat4 MVP9 = Projection * View * Model9;

		// 말1의 다리4
		glm::mat4 TranslationMatrix10 = translate(mat4(), gPosition1 + gPosition6 + gPosition10);
		glm::mat4 Model10 = RotationMatrix1 * TranslationMatrix10 * ScalingMatrix7;
		glm::mat4 MVP10 = Projection * View * Model10;


		// 말1의 목
		GLfloat val = -0.5f;

		glm::mat4 ShearingMatrix11 = glm::mat4(			// ShearingMatrix11은 말의 목의 각도를 몸통과 90도가 아닌 약간 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, val, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix11 = translate(mat4(), gPosition1 + gPosition6 + gPosition11);
		glm::mat4 ScalingMatrix11 = scale(mat4(), vec3(0.05f, 0.15f, 0.05f));
		glm::mat4 Model11 = RotationMatrix1 * TranslationMatrix11 * ShearingMatrix11 *  ScalingMatrix11;
		glm::mat4 MVP11 = Projection * View * Model11;


		// 말의 얼굴

		GLfloat val2 = 0.15f;

		glm::mat4 ShearingMatrix12 = glm::mat4(			// ShearingMatrix12는 말의 얼굴이 살짝 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, val2, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix12 = translate(mat4(), gPosition1 + gPosition6 + gPosition11 + gPosition12);
		glm::mat4 ScalingMatrix12 = scale(mat4(), vec3(0.1f, 0.1f, 0.2f));
		glm::mat4 Model12 = RotationMatrix1 * TranslationMatrix12 * ShearingMatrix12 * ScalingMatrix12;
		glm::mat4 MVP12 = Projection * View * Model12;


		// 말1 조명


		lightPos2.x = gPosition6.x;
		lightPos2.z = gPosition6.z;

		glm::mat4 TranslationMatrix_horse1_light = translate(mat4(), gPosition1 + lightPos2);
		glm::mat4 ScalingMatrix_horse1_light = scale(mat4(), vec3(0.15f, 0.15f, 0.15f));
		glm::mat4 Model_horse1_light = RotationMatrix1 * TranslationMatrix_horse1_light * ScalingMatrix_horse1_light;
		glm::mat4 MVP_horse1_light = Projection * View * Model_horse1_light;

		glm::vec3 horse1_light = vec3(Model_horse1_light * vec4(1.0f));


		/* 말 2 */

		// 말 2 몸통
		glm::mat4 TranslationMatrix_horse2_body = translate(mat4(), gPosition1 + horse2_position);
		glm::mat4 ScalingMatrix_horse2_body = scale(mat4(), vec3(0.3f, 0.25f, 0.5f));
		glm::mat4 Model_horse2_body = RotationMatrix1 * TranslationMatrix_horse2_body * ScalingMatrix_horse2_body;
		glm::mat4 MVP_horse2_body = Projection * View * Model_horse2_body;


		// 말2의 다리1
		glm::mat4 TranslationMatrix_horse2_leg1 = translate(mat4(), gPosition1 + horse2_position + gPosition7);
		glm::mat4 Model_horse2_leg1 = RotationMatrix1 * TranslationMatrix_horse2_leg1 * ScalingMatrix7;
		glm::mat4 MVP_horse2_leg1 = Projection * View * Model_horse2_leg1;

		// 말2의 다리2
		glm::mat4 TranslationMatrix_horse2_leg2 = translate(mat4(), gPosition1 + horse2_position + gPosition8);
		glm::mat4 Model_horse2_leg2 = RotationMatrix1 * TranslationMatrix_horse2_leg2 * ScalingMatrix7;
		glm::mat4 MVP_horse2_leg2 = Projection * View * Model_horse2_leg2;

		// 말2의 다리3
		glm::mat4 TranslationMatrix_horse2_leg3 = translate(mat4(), gPosition1 + horse2_position + gPosition9);
		glm::mat4 Model_horse2_leg3 = RotationMatrix1 * TranslationMatrix_horse2_leg3 * ScalingMatrix7;
		glm::mat4 MVP_horse2_leg3 = Projection * View * Model_horse2_leg3;

		// 말2의 다리4
		glm::mat4 TranslationMatrix_horse2_leg4 = translate(mat4(), gPosition1 + horse2_position + gPosition10);
		glm::mat4 Model_horse2_leg4 = RotationMatrix1 * TranslationMatrix_horse2_leg4 * ScalingMatrix7;
		glm::mat4 MVP_horse2_leg4 = Projection * View * Model_horse2_leg4;


		// 말2의 목

		val = 0.5f;

		ShearingMatrix11 = glm::mat4(			// ShearingMatrix11은 말의 목의 각도를 몸통과 90도가 아닌 약간 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, val, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse2_neck = translate(mat4(), gPosition1 + horse2_position + horse2_neck);
		glm::mat4 Model_horse2_neck = RotationMatrix1 * TranslationMatrix_horse2_neck * ShearingMatrix11 *  ScalingMatrix11;
		glm::mat4 MVP_horse2_neck = Projection * View * Model_horse2_neck;


		// 말2의 얼굴

		val2 = -0.15f;

		ShearingMatrix12 = glm::mat4(			// ShearingMatrix12는 말의 얼굴이 살짝 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, val2, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse2_face = translate(mat4(), gPosition1 + horse2_position + horse2_neck + horse2_face);
		glm::mat4 Model_horse2_face = RotationMatrix1 * TranslationMatrix_horse2_face * ShearingMatrix12 * ScalingMatrix12;
		glm::mat4 MVP_horse2_face = Projection * View * Model_horse2_face;


		// 말2 조명

		lightPos2.x = horse2_position.x;
		lightPos2.z = horse2_position.z;

		glm::mat4 TranslationMatrix_horse2_light = translate(mat4(), gPosition1 + lightPos2);
		glm::mat4 ScalingMatrix_horse2_light = scale(mat4(), vec3(0.15f, 0.15f, 0.15f));
		glm::mat4 Model_horse2_light = RotationMatrix1 * TranslationMatrix_horse2_light * ScalingMatrix_horse2_light;
		glm::mat4 MVP_horse2_light = Projection * View * Model_horse2_light;

		glm::vec3 horse2_light = vec3(Model_horse2_light * vec4(1.0f));



		/* 말 3 */
		glm::mat4 RotationMatrix_horse3 = glm::rotate(glm::mat4(), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));


		// 말3 몸통
		glm::mat4 TranslationMatrix_horse3_body = translate(mat4(), gPosition1 + horse3_position);
		glm::mat4 ScalingMatrix_horse3_body = scale(mat4(), vec3(0.3f, 0.25f, 0.5f));
		glm::mat4 Model_horse3_body = RotationMatrix1 * TranslationMatrix_horse3_body *  RotationMatrix_horse3 * ScalingMatrix_horse3_body;
		glm::mat4 MVP_horse3_body = Projection * View * Model_horse3_body;


		// 말3의 다리1
		glm::mat4 TranslationMatrix_horse3_leg1 = translate(mat4(), gPosition1 + horse3_position + horse3_leg1);
		glm::mat4 Model_horse3_leg1 = RotationMatrix1 * TranslationMatrix_horse3_leg1 *  RotationMatrix_horse3 * ScalingMatrix7;
		glm::mat4 MVP_horse3_leg1 = Projection * View * Model_horse3_leg1;

		// 말3의 다리2
		glm::mat4 TranslationMatrix_horse3_leg2 = translate(mat4(), gPosition1 + horse3_position + horse3_leg2);
		glm::mat4 Model_horse3_leg2 = RotationMatrix1 * TranslationMatrix_horse3_leg2 *  RotationMatrix_horse3 * ScalingMatrix7;
		glm::mat4 MVP_horse3_leg2 = Projection * View * Model_horse3_leg2;

		// 말3의 다리3
		glm::mat4 TranslationMatrix_horse3_leg3 = translate(mat4(), gPosition1 + horse3_position + horse3_leg3);
		glm::mat4 Model_horse3_leg3 = RotationMatrix1 * TranslationMatrix_horse3_leg3 * RotationMatrix_horse3 * ScalingMatrix7;
		glm::mat4 MVP_horse3_leg3 = Projection * View * Model_horse3_leg3;

		// 말3의 다리4
		glm::mat4 TranslationMatrix_horse3_leg4 = translate(mat4(), gPosition1 + horse3_position + horse3_leg4);
		glm::mat4 Model_horse3_leg4 = RotationMatrix1 * TranslationMatrix_horse3_leg4 *  RotationMatrix_horse3 * ScalingMatrix7;
		glm::mat4 MVP_horse3_leg4 = Projection * View * Model_horse3_leg4;


		// 말3의 목

		val = -0.5f;

		ShearingMatrix11 = glm::mat4(			// ShearingMatrix11은 말의 목의 각도를 몸통과 90도가 아닌 약간 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, val, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse3_neck = translate(mat4(), gPosition1 + horse3_position + horse3_neck);
		glm::mat4 Model_horse3_neck = RotationMatrix1 * TranslationMatrix_horse3_neck  *  RotationMatrix_horse3 * ShearingMatrix11 *  ScalingMatrix11;
		glm::mat4 MVP_horse3_neck = Projection * View * Model_horse3_neck;


		// 말3의 얼굴

		val2 = 0.15f;

		ShearingMatrix12 = glm::mat4(			// ShearingMatrix12는 말의 얼굴이 살짝 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, val2, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse3_face = translate(mat4(), gPosition1 + horse3_position + horse3_neck + horse3_face);
		glm::mat4 Model_horse3_face = RotationMatrix1 * TranslationMatrix_horse3_face *  RotationMatrix_horse3 * ShearingMatrix12 *  ScalingMatrix12;
		glm::mat4 MVP_horse3_face = Projection * View * Model_horse3_face;


		// 말3 조명

		lightPos2.x = horse3_position.x;
		lightPos2.z = horse3_position.z;

		glm::mat4 TranslationMatrix_horse3_light = translate(mat4(), gPosition1 + lightPos2);
		glm::mat4 ScalingMatrix_horse3_light = scale(mat4(), vec3(0.15f, 0.15f, 0.15f));
		glm::mat4 Model_horse3_light = RotationMatrix1 * TranslationMatrix_horse3_light * ScalingMatrix_horse3_light;
		glm::mat4 MVP_horse3_light = Projection * View * Model_horse3_light;

		glm::vec3 horse3_light = vec3(Model_horse3_light * vec4(1.0f));



		/* 말 4 */
		glm::mat4 RotationMatrix_horse4 = glm::rotate(glm::mat4(), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));


		// 말4 몸통
		glm::mat4 TranslationMatrix_horse4_body = translate(mat4(), gPosition1 + horse4_position);
		glm::mat4 ScalingMatrix_horse4_body = scale(mat4(), vec3(0.3f, 0.25f, 0.5f));
		glm::mat4 Model_horse4_body = RotationMatrix1 * TranslationMatrix_horse4_body *  RotationMatrix_horse4 * ScalingMatrix_horse3_body;
		glm::mat4 MVP_horse4_body = Projection * View * Model_horse4_body;


		// 말4의 다리1
		glm::mat4 TranslationMatrix_horse4_leg1 = translate(mat4(), gPosition1 + horse4_position + horse3_leg1);
		glm::mat4 Model_horse4_leg1 = RotationMatrix1 * TranslationMatrix_horse4_leg1 *  RotationMatrix_horse4 * ScalingMatrix7;
		glm::mat4 MVP_horse4_leg1 = Projection * View * Model_horse4_leg1;

		// 말4의 다리2
		glm::mat4 TranslationMatrix_horse4_leg2 = translate(mat4(), gPosition1 + horse4_position + horse3_leg2);
		glm::mat4 Model_horse4_leg2 = RotationMatrix1 * TranslationMatrix_horse4_leg2 *  RotationMatrix_horse4 * ScalingMatrix7;
		glm::mat4 MVP_horse4_leg2 = Projection * View * Model_horse4_leg2;

		// 말4의 다리3
		glm::mat4 TranslationMatrix_horse4_leg3 = translate(mat4(), gPosition1 + horse4_position + horse3_leg3);
		glm::mat4 Model_horse4_leg3 = RotationMatrix1 * TranslationMatrix_horse4_leg3 * RotationMatrix_horse4 * ScalingMatrix7;
		glm::mat4 MVP_horse4_leg3 = Projection * View * Model_horse4_leg3;

		// 말4의 다리4
		glm::mat4 TranslationMatrix_horse4_leg4 = translate(mat4(), gPosition1 + horse4_position + horse3_leg4);
		glm::mat4 Model_horse4_leg4 = RotationMatrix1 * TranslationMatrix_horse4_leg4 *  RotationMatrix_horse4 * ScalingMatrix7;
		glm::mat4 MVP_horse4_leg4 = Projection * View * Model_horse4_leg4;


		// 말4의 목

		val = 0.5f;

		ShearingMatrix11 = glm::mat4(			// ShearingMatrix11은 말의 목의 각도를 몸통과 90도가 아닌 약간 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, val, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse4_neck = translate(mat4(), gPosition1 + horse4_position + horse4_neck);
		glm::mat4 Model_horse4_neck = RotationMatrix1 * TranslationMatrix_horse4_neck  *  RotationMatrix_horse4 * ShearingMatrix11 *  ScalingMatrix11;
		glm::mat4 MVP_horse4_neck = Projection * View * Model_horse4_neck;


		// 말4의 얼굴

		val2 = -0.15f;

		ShearingMatrix12 = glm::mat4(			// ShearingMatrix12는 말의 얼굴이 살짝 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, val2, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse4_face = translate(mat4(), gPosition1 + horse4_position + horse4_neck + horse4_face);
		glm::mat4 Model_horse4_face = RotationMatrix1 * TranslationMatrix_horse4_face *  RotationMatrix_horse4 * ShearingMatrix12 *  ScalingMatrix12;
		glm::mat4 MVP_horse4_face = Projection * View * Model_horse4_face;


		// 말4 조명

		lightPos2.x = horse4_position.x;
		lightPos2.z = horse4_position.z;

		glm::mat4 TranslationMatrix_horse4_light = translate(mat4(), gPosition1 + lightPos2);
		glm::mat4 ScalingMatrix_horse4_light = scale(mat4(), vec3(0.15f, 0.15f, 0.15f));
		glm::mat4 Model_horse4_light = RotationMatrix1 * TranslationMatrix_horse4_light * ScalingMatrix_horse4_light;
		glm::mat4 MVP_horse4_light = Projection * View * Model_horse4_light;

		glm::vec3 horse4_light = vec3(Model_horse4_light * vec4(1.0f));



		/* 말 5 */
		glm::mat4 RotationMatrix_horse5 = glm::rotate(glm::mat4(), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));


		// 말5 몸통
		glm::mat4 TranslationMatrix_horse5_body = translate(mat4(), gPosition1 + horse5_position);
		glm::mat4 ScalingMatrix_horse5_body = scale(mat4(), vec3(0.3f, 0.25f, 0.5f));
		glm::mat4 Model_horse5_body = RotationMatrix1 * TranslationMatrix_horse5_body *  RotationMatrix_horse5 * ScalingMatrix_horse5_body;
		glm::mat4 MVP_horse5_body = Projection * View * Model_horse5_body;


		// 말5의 다리1
		glm::mat4 TranslationMatrix_horse5_leg1 = translate(mat4(), gPosition1 + horse5_position + horse5_leg1);
		glm::mat4 Model_horse5_leg1 = RotationMatrix1 * TranslationMatrix_horse5_leg1 *  RotationMatrix_horse5 * ScalingMatrix7;
		glm::mat4 MVP_horse5_leg1 = Projection * View * Model_horse5_leg1;

		// 말5의 다리2
		glm::mat4 TranslationMatrix_horse5_leg2 = translate(mat4(), gPosition1 + horse5_position + horse5_leg2);
		glm::mat4 Model_horse5_leg2 = RotationMatrix1 * TranslationMatrix_horse5_leg2 *  RotationMatrix_horse5 * ScalingMatrix7;
		glm::mat4 MVP_horse5_leg2 = Projection * View * Model_horse5_leg2;

		// 말5의 다리3
		glm::mat4 TranslationMatrix_horse5_leg3 = translate(mat4(), gPosition1 + horse5_position + horse5_leg3);
		glm::mat4 Model_horse5_leg3 = RotationMatrix1 * TranslationMatrix_horse5_leg3 * RotationMatrix_horse5 * ScalingMatrix7;
		glm::mat4 MVP_horse5_leg3 = Projection * View * Model_horse5_leg3;

		// 말5의 다리4
		glm::mat4 TranslationMatrix_horse5_leg4 = translate(mat4(), gPosition1 + horse5_position + horse5_leg4);
		glm::mat4 Model_horse5_leg4 = RotationMatrix1 * TranslationMatrix_horse5_leg4 *  RotationMatrix_horse5 * ScalingMatrix7;
		glm::mat4 MVP_horse5_leg4 = Projection * View * Model_horse5_leg4;


		// 말5의 목

		val = -0.5f;

		ShearingMatrix11 = glm::mat4(			// ShearingMatrix11은 말의 목의 각도를 몸통과 90도가 아닌 약간 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, val, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse5_neck = translate(mat4(), gPosition1 + horse5_position + horse5_neck);
		glm::mat4 Model_horse5_neck = RotationMatrix1 * TranslationMatrix_horse5_neck  *  RotationMatrix_horse5 * ShearingMatrix11 *  ScalingMatrix11;
		glm::mat4 MVP_horse5_neck = Projection * View * Model_horse5_neck;


		// 말5의 얼굴

		val2 = 0.15f;

		ShearingMatrix12 = glm::mat4(			// ShearingMatrix12는 말의 얼굴이 살짝 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, val2, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse5_face = translate(mat4(), gPosition1 + horse5_position + horse5_neck + horse5_face);
		glm::mat4 Model_horse5_face = RotationMatrix1 * TranslationMatrix_horse5_face *  RotationMatrix_horse5 * ShearingMatrix12 *  ScalingMatrix12;
		glm::mat4 MVP_horse5_face = Projection * View * Model_horse5_face;


		// 말5 조명

		// glm::vec3 horse5_light = vec3(RotationMatrix1 * TranslationMatrix_horse5_body * vec4(lightPos2, 1.0f));


		lightPos2.x = horse5_position.x;
		lightPos2.z = horse5_position.z;

		glm::mat4 TranslationMatrix_horse5_light = translate(mat4(), gPosition1 + lightPos2);
		glm::mat4 ScalingMatrix_horse5_light = scale(mat4(), vec3(0.15f, 0.15f, 0.15f));
		glm::mat4 Model_horse5_light = RotationMatrix1 * TranslationMatrix_horse5_light * ScalingMatrix_horse5_light;
		glm::mat4 MVP_horse5_light = Projection * View * Model_horse5_light;

		glm::vec3 horse5_light = vec3(Model_horse5_light * vec4(1.0f));



		/* 말 6 */
		glm::mat4 RotationMatrix_horse6 = glm::rotate(glm::mat4(), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));


		// 말6 몸통
		glm::mat4 TranslationMatrix_horse6_body = translate(mat4(), gPosition1 + horse6_position);
		glm::mat4 ScalingMatrix_horse6_body = scale(mat4(), vec3(0.3f, 0.25f, 0.5f));
		glm::mat4 Model_horse6_body = RotationMatrix1 * TranslationMatrix_horse6_body *  RotationMatrix_horse6 * ScalingMatrix_horse6_body;
		glm::mat4 MVP_horse6_body = Projection * View * Model_horse6_body;


		// 말6의 다리1
		glm::mat4 TranslationMatrix_horse6_leg1 = translate(mat4(), gPosition1 + horse6_position + horse6_leg1);
		glm::mat4 Model_horse6_leg1 = RotationMatrix1 * TranslationMatrix_horse6_leg1 *  RotationMatrix_horse6 * ScalingMatrix7;
		glm::mat4 MVP_horse6_leg1 = Projection * View * Model_horse6_leg1;

		// 말6의 다리2
		glm::mat4 TranslationMatrix_horse6_leg2 = translate(mat4(), gPosition1 + horse6_position + horse6_leg2);
		glm::mat4 Model_horse6_leg2 = RotationMatrix1 * TranslationMatrix_horse6_leg2 *  RotationMatrix_horse6 * ScalingMatrix7;
		glm::mat4 MVP_horse6_leg2 = Projection * View * Model_horse6_leg2;

		// 말6의 다리3
		glm::mat4 TranslationMatrix_horse6_leg3 = translate(mat4(), gPosition1 + horse6_position + horse6_leg3);
		glm::mat4 Model_horse6_leg3 = RotationMatrix1 * TranslationMatrix_horse6_leg3 * RotationMatrix_horse6 * ScalingMatrix7;
		glm::mat4 MVP_horse6_leg3 = Projection * View * Model_horse6_leg3;

		// 말6의 다리4
		glm::mat4 TranslationMatrix_horse6_leg4 = translate(mat4(), gPosition1 + horse6_position + horse6_leg4);
		glm::mat4 Model_horse6_leg4 = RotationMatrix1 * TranslationMatrix_horse6_leg4 *  RotationMatrix_horse6 * ScalingMatrix7;
		glm::mat4 MVP_horse6_leg4 = Projection * View * Model_horse6_leg4;


		// 말6의 목

		val = 0.5f;

		ShearingMatrix11 = glm::mat4(			// ShearingMatrix11은 말의 목의 각도를 몸통과 90도가 아닌 약간 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, val, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse6_neck = translate(mat4(), gPosition1 + horse6_position + horse6_neck);
		glm::mat4 Model_horse6_neck = RotationMatrix1 * TranslationMatrix_horse6_neck  *  RotationMatrix_horse6 * ShearingMatrix11 *  ScalingMatrix11;
		glm::mat4 MVP_horse6_neck = Projection * View * Model_horse6_neck;


		// 말6의 얼굴

		val2 = -0.15f;

		ShearingMatrix12 = glm::mat4(			// ShearingMatrix12는 말의 얼굴이 살짝 꺽이게 Shearing을 해준다.
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, val2, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 TranslationMatrix_horse6_face = translate(mat4(), gPosition1 + horse6_position + horse6_neck + horse6_face);
		glm::mat4 Model_horse6_face = RotationMatrix1 * TranslationMatrix_horse6_face *  RotationMatrix_horse6 * ShearingMatrix12 *  ScalingMatrix12;
		glm::mat4 MVP_horse6_face = Projection * View * Model_horse6_face;


		// 말6 조명

		// glm::vec3 horse6_light = vec3(RotationMatrix1 * TranslationMatrix_horse6_body * vec4(lightPos2, 1.0f));

		lightPos2.x = horse6_position.x;
		lightPos2.z = horse6_position.z;

		glm::mat4 TranslationMatrix_horse6_light = translate(mat4(), gPosition1 + lightPos2);
		glm::mat4 ScalingMatrix_horse6_light = scale(mat4(), vec3(0.15f, 0.15f, 0.15f));
		glm::mat4 Model_horse6_light = RotationMatrix1 * TranslationMatrix_horse6_light * ScalingMatrix_horse6_light;
		glm::mat4 MVP_horse6_light = Projection * View * Model_horse6_light;

		glm::vec3 horse6_light = vec3(Model_horse6_light * vec4(1.0f));


		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform

		//생성한 projection, view, model matrix를 shader에 전송한다.
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, &Projection[0][0]);

		//셰이더의 uniform변수에 값을 전달.
		glUniform3f(lightColorID, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosID, lightPos1.x, lightPos1.y, lightPos1.z);
		glUniform3f(viewPosID, camera_Pos.x, camera_Pos.y, camera_Pos.z);



		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);



		struct IDs IDs;
		IDs.modelMatrixID = modelMatrixID;
		IDs.MatrixID = MatrixID;
		IDs.ColorCheckID = ColorCheckID;
		IDs.lightCheckID = lightCheckID;

		struct Models models;
		struct buffer_info buffers;
		struct other_info others;


		// 회전 목마의 중앙기둥
		models.Model = Model1;
		models.MVP = MVP1;

		buffers.vertexbuffer = vertexbuffer2;
		buffers.normalbuffer = normalbuffer2;
		buffers.uvbuffer = 0;

		others.color_num = 2;
		others.light_num = 0;
		others.vertex_size = vertices2.size();

		draw_object(IDs, models, buffers, others);


		// 회전 목마의 기둥1
		models.Model = Model2;
		models.MVP = MVP2;

		buffers.vertexbuffer = vertexbuffer2;
		buffers.normalbuffer = normalbuffer2;
		buffers.uvbuffer = 0;

		others.color_num = 1;
		others.light_num = 0;
		others.vertex_size = vertices2.size();

		draw_object(IDs, models, buffers, others);


		/* 회전 목마의 기둥2 */
		models.Model = Model_pillar2;
		models.MVP = MVP_pillar2;
		draw_object(IDs, models, buffers, others);


		/* 회전 목마3의 기둥 */
		models.Model = Model_pillar3;
		models.MVP = MVP_pillar3;
		draw_object(IDs, models, buffers, others);


		/* 회전 목마4의 기둥 */
		models.Model = Model_pillar4;
		models.MVP = MVP_pillar4;
		draw_object(IDs, models, buffers, others);


		/* 회전 목마5의 기둥 */
		models.Model = Model_pillar5;
		models.MVP = MVP_pillar5;
		draw_object(IDs, models, buffers, others);


		/* 회전 목마6의 기둥 */
		models.Model = Model_pillar6;
		models.MVP = MVP_pillar6;
		draw_object(IDs, models, buffers, others);



		/* 회전목마 윗부분 */
		models.Model = Model4;
		models.MVP = MVP4;

		buffers.vertexbuffer = vertexbuffer2;
		buffers.normalbuffer = normalbuffer2;
		buffers.uvbuffer = 0;


		others.color_num = 0;
		others.light_num = 2;
		others.vertex_size = vertices2.size();

		draw_object(IDs, models, buffers, others);


		// 회전목마의 바닥

		models.Model = Model5;
		models.MVP = MVP5;

		others.light_num = 0;
		others.light_num = 2;

		draw_object(IDs, models, buffers, others);


		// 말1의 실제 조명 오브젝트

		glUniform3f(lightPos2ID, horse1_light.x, horse1_light.y, horse1_light.z);

		if (global_light_flag == false && horse_light_flag == true) {
			models.Model = Model_horse1_light;
			models.MVP = MVP_horse1_light;

			buffers.vertexbuffer = vertexbuffer;
			buffers.normalbuffer = normalbuffer;
			buffers.uvbuffer = uvbuffer1;

			others.color_num = 4;
			others.light_num = 0;
			others.vertex_size = vertices.size();

			draw_object(IDs, models, buffers, others);
		}


		// 말1의 몸통
		models.Model = Model6;
		models.MVP = MVP6;

		buffers.vertexbuffer = vertexbuffer;
		buffers.normalbuffer = normalbuffer;
		buffers.uvbuffer = uvbuffer1;

		others.color_num = 3;
		others.light_num = 1;
		others.vertex_size = vertices.size();

		draw_object(IDs, models, buffers, others);


		// 말1의 다리1
		models.Model = Model7;
		models.MVP = MVP7;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);

		// 말1의 다리2
		models.Model = Model8;
		models.MVP = MVP8;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말1의 다리3
		models.Model = Model9;
		models.MVP = MVP9;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말1의 다리4
		models.Model = Model10;
		models.MVP = MVP10;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);



		// 말1의 목
		models.Model = Model11;
		models.MVP = MVP11;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말1의 얼굴
		models.Model = Model12;
		models.MVP = MVP12;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		/* 말 2 */

		// 말2의 조명

		glUniform3f(lightPos2ID, horse2_light.x, horse2_light.y, horse2_light.z);


		// 말2의 실제 조명 오브젝트

		if (global_light_flag == false && horse_light_flag == true) {
			models.Model = Model_horse2_light;
			models.MVP = MVP_horse2_light;

			buffers.vertexbuffer = vertexbuffer;
			buffers.normalbuffer = normalbuffer;
			buffers.uvbuffer = uvbuffer1;

			others.color_num = 4;
			others.light_num = 0;
			others.vertex_size = vertices.size();

			draw_object(IDs, models, buffers, others);
		}

		// 말2의 몸통
		models.Model = Model_horse2_body;
		models.MVP = MVP_horse2_body;

		others.color_num = 3;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말2의 다리1
		models.Model = Model_horse2_leg1;
		models.MVP = MVP_horse2_leg1;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);

		// 말2의 다리2
		models.Model = Model_horse2_leg2;
		models.MVP = MVP_horse2_leg2;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말2의 다리3
		models.Model = Model_horse2_leg3;
		models.MVP = MVP_horse2_leg3;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말2의 다리4
		models.Model = Model_horse2_leg4;
		models.MVP = MVP_horse2_leg4;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);



		// 말2의 목
		models.Model = Model_horse2_neck;
		models.MVP = MVP_horse2_neck;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말2의 얼굴
		models.Model = Model_horse2_face;
		models.MVP = MVP_horse2_face;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);



		/* 말 3 */

		// 말3의 조명

		glUniform3f(lightPos2ID, horse3_light.x, horse3_light.y, horse3_light.z);


		// 말3의 실제 조명 오브젝트

		if (global_light_flag == false && horse_light_flag == true) {
			models.Model = Model_horse3_light;
			models.MVP = MVP_horse3_light;

			buffers.vertexbuffer = vertexbuffer;
			buffers.normalbuffer = normalbuffer;
			buffers.uvbuffer = uvbuffer1;

			others.color_num = 4;
			others.light_num = 0;
			others.vertex_size = vertices.size();

			draw_object(IDs, models, buffers, others);
		}


		// 말3의 몸통
		models.Model = Model_horse3_body;
		models.MVP = MVP_horse3_body;

		others.color_num = 3;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말3의 다리1
		models.Model = Model_horse3_leg1;
		models.MVP = MVP_horse3_leg1;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);

		// 말3의 다리2
		models.Model = Model_horse3_leg2;
		models.MVP = MVP_horse3_leg2;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말3의 다리3
		models.Model = Model_horse3_leg3;
		models.MVP = MVP_horse3_leg3;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말3의 다리4
		models.Model = Model_horse3_leg4;
		models.MVP = MVP_horse3_leg4;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);



		// 말3의 목
		models.Model = Model_horse3_neck;
		models.MVP = MVP_horse3_neck;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말3의 얼굴
		models.Model = Model_horse3_face;
		models.MVP = MVP_horse3_face;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);



		/* 말 4 */

		// 말4의 조명

		glUniform3f(lightPos2ID, horse4_light.x, horse4_light.y, horse4_light.z);


		// 말4의 실제 조명 오브젝트

		if (global_light_flag == false && horse_light_flag == true) {
			models.Model = Model_horse4_light;
			models.MVP = MVP_horse4_light;

			buffers.vertexbuffer = vertexbuffer;
			buffers.normalbuffer = normalbuffer;
			buffers.uvbuffer = uvbuffer1;

			others.color_num = 4;
			others.light_num = 0;
			others.vertex_size = vertices.size();

			draw_object(IDs, models, buffers, others);
		}


		// 말4의 몸통
		models.Model = Model_horse4_body;
		models.MVP = MVP_horse4_body;

		others.color_num = 3;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말4의 다리1
		models.Model = Model_horse4_leg1;
		models.MVP = MVP_horse4_leg1;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);

		// 말4의 다리2
		models.Model = Model_horse4_leg2;
		models.MVP = MVP_horse4_leg2;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말4의 다리3
		models.Model = Model_horse4_leg3;
		models.MVP = MVP_horse4_leg3;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말4의 다리4
		models.Model = Model_horse4_leg4;
		models.MVP = MVP_horse4_leg4;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);



		// 말4의 목
		models.Model = Model_horse4_neck;
		models.MVP = MVP_horse4_neck;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말4의 얼굴
		models.Model = Model_horse4_face;
		models.MVP = MVP_horse4_face;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);




		/* 말 5 */

		// 말5의 조명

		glUniform3f(lightPos2ID, horse5_light.x, horse5_light.y, horse5_light.z);


		// 말2의 실제 조명 오브젝트

		if (global_light_flag == false && horse_light_flag == true) {
			models.Model = Model_horse5_light;
			models.MVP = MVP_horse5_light;

			buffers.vertexbuffer = vertexbuffer;
			buffers.normalbuffer = normalbuffer;
			buffers.uvbuffer = uvbuffer1;

			others.color_num = 4;
			others.light_num = 0;
			others.vertex_size = vertices.size();

			draw_object(IDs, models, buffers, others);
		}


		// 말5의 몸통
		models.Model = Model_horse5_body;
		models.MVP = MVP_horse5_body;

		others.color_num = 3;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말5의 다리1
		models.Model = Model_horse5_leg1;
		models.MVP = MVP_horse5_leg1;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);

		// 말5의 다리2
		models.Model = Model_horse5_leg2;
		models.MVP = MVP_horse5_leg2;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말5의 다리3
		models.Model = Model_horse5_leg3;
		models.MVP = MVP_horse5_leg3;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말5의 다리4
		models.Model = Model_horse5_leg4;
		models.MVP = MVP_horse5_leg4;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);



		// 말5의 목
		models.Model = Model_horse5_neck;
		models.MVP = MVP_horse5_neck;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말5의 얼굴
		models.Model = Model_horse5_face;
		models.MVP = MVP_horse5_face;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);



		/* 말 6 */

		// 말6의 조명

		glUniform3f(lightPos2ID, horse6_light.x, horse6_light.y, horse6_light.z);


		// 말2의 실제 조명 오브젝트

		if (global_light_flag == false && horse_light_flag == true) {
			models.Model = Model_horse6_light;
			models.MVP = MVP_horse6_light;

			buffers.vertexbuffer = vertexbuffer;
			buffers.normalbuffer = normalbuffer;
			buffers.uvbuffer = uvbuffer1;

			others.color_num = 4;
			others.light_num = 0;
			others.vertex_size = vertices.size();

			draw_object(IDs, models, buffers, others);
		}


		// 말6의 몸통
		models.Model = Model_horse6_body;
		models.MVP = MVP_horse6_body;

		others.color_num = 3;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말6의 다리1
		models.Model = Model_horse6_leg1;
		models.MVP = MVP_horse6_leg1;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);

		// 말6의 다리2
		models.Model = Model_horse6_leg2;
		models.MVP = MVP_horse6_leg2;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말6의 다리3
		models.Model = Model_horse6_leg3;
		models.MVP = MVP_horse6_leg3;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);


		// 말6의 다리4
		models.Model = Model_horse6_leg4;
		models.MVP = MVP_horse6_leg4;
		others.light_num = 0;

		draw_object(IDs, models, buffers, others);



		// 말6의 목
		models.Model = Model_horse6_neck;
		models.MVP = MVP_horse6_neck;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		// 말6의 얼굴
		models.Model = Model_horse6_face;
		models.MVP = MVP_horse6_face;
		others.light_num = 1;

		draw_object(IDs, models, buffers, others);


		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &vertexbuffer2);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


void computeMouseRotates() {

	float xoffset, yoffset;

	// Get mouse position	
	glfwGetCursorPos(window, &xpos, &ypos);

	// Compute new orientation
	if (xpos < xpos_prev)
		xoffset = -mouse_Speed * abs(xpos - xpos_prev);			// abs (xpos - xpos_prev)를 추가함. 마우스 이동거리 반영
	else if (xpos > xpos_prev)
		xoffset = mouse_Speed * abs(xpos - xpos_prev);
	else
		xoffset = 0.0;

	if (ypos < ypos_prev)
		yoffset = mouse_Speed * abs(ypos - ypos_prev);
	else if (ypos > ypos_prev)
		yoffset = -mouse_Speed * abs(ypos - ypos_prev);
	else
		yoffset = 0.0;


	yaw_val += xoffset;
	pitch_val += yoffset;


	// pitch 값을 제한.  짐벌락 방지
	if (pitch_val > 89.0f) pitch_val = 89.0f;
	if (pitch_val < -89.0f) pitch_val = -89.0f;


	glm::vec3 direction;

	// direction = glm::vec3(horizontal_Angle, vertical_Angle, horizontal_Angle);

	direction.x = cos(glm::radians(yaw_val)) * cos(glm::radians(pitch_val));
	direction.y = sin(glm::radians(pitch_val));
	direction.z = sin(glm::radians(yaw_val)) * cos(glm::radians(pitch_val));

	camera_direction = glm::normalize(direction);


	View = glm::lookAt(
		camera_Pos,                // Camera position
		camera_Pos + camera_direction,
		camera_up                  // Up direction
	);


	xpos_prev = xpos;
	ypos_prev = ypos;

}


/*
if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) != GLFW_PRESS) {

	glm::vec3 View_vector = { -View[0][0], -View[0][1], -View[0][2] };
	glm::vec3 View_vector2 = { View[0][0], View[0][1], View[0][2] };

	glm::mat4 TranslationMatrix = translate(mat4(), View_vector);
	glm::mat4 TranslationMatrix2 = translate(mat4(), View_vector2);
	glm::mat4 RotationMatrix = glm::eulerAngleYXZ(horizontal_Angle, vertical_Angle, 0.0f);

	View = TranslationMatrix2 * RotationMatrix * TranslationMatrix * View;
}


else {
	glm::mat4 RotationMatrix = glm::eulerAngleYXZ(horizontal_Angle, vertical_Angle, 0.0f);

	View = RotationMatrix * View;
}

*/




void computeKeyboardTranslates()
{
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);


	const float camera_speed = 3.0f;

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera_Pos += deltaTime * camera_speed * camera_direction;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera_Pos -= deltaTime * camera_speed * camera_direction;
	}
	// Move right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera_Pos += deltaTime * camera_speed * glm::normalize(glm::cross(camera_direction, camera_up));
	}
	// Move left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera_Pos -= deltaTime * camera_speed * glm::normalize(glm::cross(camera_direction, camera_up));
	}
	// Move up
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera_Pos += deltaTime * camera_speed  * camera_up;
	}
	// Move down
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera_Pos -= deltaTime * camera_speed  * camera_up;
	}


	View = glm::lookAt(
		camera_Pos,		// Camera , in World Space
		camera_Pos + camera_direction,	// and looks at the origin
		camera_up		// Head is up (set to 0,-1,0 to look upside-down)
	);

	lastTime = currentTime;

}


void press_spacebar(float *rotSpeed, float *moveSpeed, float *deltaTime) {

	bool space_state = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);

	if (space_state == true && prev_space_state == false) {
		start_flag = !start_flag;
	}


	// 스페이스바를 눌러 start_flag를 true로 만들 시 speed증가

	if (start_flag == true) {

		if (*rotSpeed < 0.25f) {
			*rotSpeed += (*deltaTime) * 0.2;
		}


		if (*moveSpeed < 0.5f) {
			*moveSpeed += (*deltaTime) * 0.4;
		}

	}


	if (start_flag == false) {

		if (*rotSpeed > 0.0f) {
			*rotSpeed -= (*deltaTime) * 0.2f;


			if (*rotSpeed < 0.0f) {
				*rotSpeed = 0.0f;
			}
		}


		if (*moveSpeed > 0.0f) {
			*moveSpeed -= (*deltaTime) * 0.4f;

			if (*moveSpeed < 0.0f) {
				*moveSpeed = 0.0f;
			}
		}
	}


	prev_space_state = space_state;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;

	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 90.0f)
		fov = 90.0f;


	Projection = glm::perspective(glm::radians(fov), 4.0f / 3.0f, 0.1f, 100.0f);
}



void draw_object(struct IDs IDs, struct Models models, struct buffer_info buffers, struct other_info others) {

	glUniformMatrix4fv(IDs.modelMatrixID, 1, GL_FALSE, &models.Model[0][0]);		// model을 변경
	glUniformMatrix4fv(IDs.MatrixID, 1, GL_FALSE, &models.MVP[0][0]);

	// Set Color-related Variable
	glUniform1i(IDs.ColorCheckID, others.color_num);
	glUniform1i(IDs.lightCheckID, others.light_num);


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.vertexbuffer);
	glVertexAttribPointer(
		0,		  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,		  // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0,		  // stride
		(void *)0 // array buffer offset
	);


	// 2nd attribute buffer : normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.normalbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);


	// 3rd attribute buffer : UVs

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, buffers.uvbuffer);
	glVertexAttribPointer(
		2,                                // attribute					// location == 2 이므로 2로 설정
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 텍스쳐를 적용하지 않는 경우

	if (buffers.uvbuffer == 0) {
		glDisableVertexAttribArray(2);
	}



	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, others.vertex_size);
}



void set_Entire_light_flag() {

	static bool global_light_prev_press = false;
	static bool horse_light_prev_press = false;
	static bool camera_light_prev_press = false;

	bool global_light_mouse_state = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

	if (global_light_mouse_state == true && global_light_prev_press == false) {
		global_light_flag = !global_light_flag;
	}


	bool horse_light_press_state = (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS);

	if (horse_light_press_state == true && horse_light_prev_press == false) {
		horse_light_flag = !horse_light_flag;
	}



	bool camera_light_press_state = (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS);

	if (camera_light_press_state == true && camera_light_prev_press == false) {
		camera_light_flag = !camera_light_flag;
	}



	global_light_prev_press = global_light_mouse_state;
	horse_light_prev_press = horse_light_press_state;
	camera_light_prev_press = camera_light_press_state;
}
