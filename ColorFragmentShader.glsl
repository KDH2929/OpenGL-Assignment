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



	
	if(global_light_flag == 1){			// ��ü ����

		// Ambient
		// ���� ����� �����ִ°����� ǥ��
		float ambientStrength = 0.4;
		ambient = ambientStrength * objectColor;

		// Diffuse 
		// normal vector�� ���� ������� ���� = �鿡���� ��ü���� ���� ����
		float diffuseStrength = 0.4;
		vec3 norm = normalize(normal);
		// �������� ���� ��ġ�� ���ϴ� ����
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse = diffuseStrength * diff * objectColor;

		// Specular
		// �ݻ�Ǵ� �� �߿��� ������ ������ ���� ����
		// �ݻ�Ǵ� ���� ����� �ü������� �������� ���� ���⸦ ���ϰ�
		// ���� ���Ⱑ 0�̻��ΰ��� 32�����ؼ� specular light�� ��Ÿ����. 
		// specular strength�� spec�� ���ؼ� specular light�� ���⸦ ���� �� ����� ���ؼ�
		// specular light�� ������ ���Ѵ�.
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

		if(light_num==2 && global_light_flag == 0){			// Ư�� object�� spotlight�� ���絵 ��Ⱑ 0�� �κ��� �����Ͽ� �ش� �κ��� �ڿ������� ǥ���ϱ� ���� �߰�
				float Strength = 0.5;
				special_case = Strength * objectColor;
		}

		vec3 lightPos = cameraPos;

		vec3 lightDir = normalize(lightPos - FragPos);
		vec3 norm = normalize(normal);


		vec3 spotDir = cameraDir;

		float theta = dot(lightDir, normalize(-spotDir));
		float cutOff = cos(radians(9.5f));				// ������ Ű����� ī�޶� spotlight ũ�Ⱑ Ŀ����.
		float outerCutOff = cos(radians(13.5f));		// spotlight �׵θ��κ��� �ڿ������� ǥ���ϱ� ���� �ʿ�

		float epsilon = (cutOff - outerCutOff);
		float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);


		// Diffuse 

		float diffuseStrength = 2.0;


		// �������� ���� ��ġ�� ���ϴ� ����
		
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse = diffuseStrength * diff * objectColor;

		// Specular

		float specularStrength = 1.0;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		specular = specularStrength * spec * lightColor;


		// �����
		float distance = length(lightPos - FragPos);
		float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));	// �Ÿ��� ���� ���谡 �ʿ��� �� ���


		special_case *= intensity;
		diffuse  *= intensity;
		specular *= intensity;


		special_case *= attenuation;	
		diffuse  *= attenuation;	
		specular *= attenuation;
		

		additional_light = (diffuse + specular + special_case);
		
	}	
	


	

	color = result + horse1_light + additional_light;


	if (colorCheck == 4)			// �������� �Ǵ� ��ü�Ͻ�
	{
		color = vec3(1.0, 1.0, 1.0);
	}


	

}