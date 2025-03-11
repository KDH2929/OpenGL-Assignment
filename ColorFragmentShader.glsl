#version 330 core

// Ouput data
out vec3 color;

// Interpolated values from the vertex shaders
in vec3 FragPos;
in vec3 normal;
in vec2 UV1;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform vec3 lightPos2;

uniform int colorCheck;
uniform int light_num;

uniform int global_light_flag;
uniform int horse_light_flag;

uniform vec3 cameraPos;
uniform vec3 cameraDir;
uniform int camera_light_flag;

uniform sampler2D myTextureSampler;

void main() {

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 objectColor;



	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;



	vec3 result;
	vec3 horse1_light = vec3(0.0, 0.0, 0.0);
	vec3 additional_light = vec3(0.0, 0.0, 0.0);

	if (colorCheck == 0)
	{
		objectColor = vec3(1.0, 0.0, 0.0);	
	}

	else if (colorCheck == 1)
	{
		objectColor = vec3(0.0, 1.0, 0.0);
	}

	else if (colorCheck == 2)
	{
		objectColor = vec3(0.0, 0.0, 1.0);
	}

	else if (colorCheck == 3)
	{
		// Output color = color of the texture at the specified UV

		objectColor = texture( myTextureSampler, UV1 ).rgb;
	}



	
	if(global_light_flag == 1){			// 전체 조명

		// Ambient
		// 색상에 상수를 곱해주는것으로 표현
		float ambientStrength = 0.4;
		ambient = ambientStrength * objectColor;

		// Diffuse 
		// normal vector와 빛의 방향과의 내적 = 면에서의 전체적인 빛의 세기
		float diffuseStrength = 0.4;
		vec3 norm = normalize(normal);
		// 정점에서 빛의 위치를 향하는 벡터
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse = diffuseStrength * diff * objectColor;

		// Specular
		// 반사되는 빛 중에서 눈으로 들어오는 양을 결정
		// 반사되는 빛의 방향과 시선방향의 내적으로 빛의 세기를 구하고
		// 빛의 세기가 0이상인것을 32제곱해서 specular light를 나타낸다. 
		// specular strength와 spec을 곱해서 specular light의 세기를 구한 후 색상과 곱해서
		// specular light의 색상을 구한다.
		float specularStrength = 1.0;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		specular = specularStrength * spec * objectColor;

		//light intelsity function
		result = (ambient + diffuse + specular);

	}

	
	if(light_num == 1 && horse_light_flag == 1)
	{
		
		vec3 norm = normalize(normal);
		vec3 lightDir = normalize(lightPos2 - FragPos);
	

		// Specular
		float specularStrength = 2.0;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
		specular = specularStrength * spec * lightColor;

		//light intelsity function
		horse1_light = specular;

	}


	if(camera_light_flag == 1){

		vec3 special_case = vec3(0.0, 0.0, 0.0);

		if(light_num==2 && global_light_flag == 0){			// 특정 object가 spotlight를 비춰도 밝기가 0인 부분이 존재하여 해당 부분을 자연스럽게 표현하기 위해 추가
				float Strength = 0.5;
				special_case = Strength * objectColor;
		}

		vec3 lightPos = cameraPos;

		vec3 lightDir = normalize(lightPos - FragPos);
		vec3 norm = normalize(normal);


		vec3 spotDir = cameraDir;

		float theta = dot(lightDir, normalize(-spotDir));
		float cutOff = cos(radians(9.5f));				// 각도를 키울수록 카메라 spotlight 크기가 커진다.
		float outerCutOff = cos(radians(13.5f));		// spotlight 테두리부분을 자연스럽게 표현하기 위해 필요

		float epsilon = (cutOff - outerCutOff);
		float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);


		// Diffuse 

		float diffuseStrength = 2.0;


		// 정점에서 빛의 위치를 향하는 벡터
		
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse = diffuseStrength * diff * objectColor;

		// Specular

		float specularStrength = 1.0;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		specular = specularStrength * spec * lightColor;


		// 감쇠식
		float distance = length(lightPos - FragPos);
		float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));	// 거리에 따른 감쇠가 필요할 시 사용


		special_case *= intensity;
		diffuse  *= intensity;
		specular *= intensity;


		special_case *= attenuation;	
		diffuse  *= attenuation;	
		specular *= attenuation;
		

		additional_light = (diffuse + specular + special_case);
		
	}	
	


	

	color = result + horse1_light + additional_light;


	if (colorCheck == 4)			// 점광원이 되는 물체일시
	{
		color = vec3(1.0, 1.0, 1.0);
	}


	

}