#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec3 normal;
out vec3 FragPos;
out vec2 UV1;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

//model, view, projection ��Ʈ������ ���� �޴´�. 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){	

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);


	//�𵨺�ȯ ��ǥ�� fragment shader�� ����. 
	FragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0f));
	
    // normal = vertexNormal;
	normal = vec3(model * vec4(vertexNormal, 0.0f));

	UV1 = vertexUV;
}

