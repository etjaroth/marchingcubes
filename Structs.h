#pragma once

template<class ttype>
struct triple { // array of 3 that can be stored in a map
	ttype three[3];
	bool operator==(const triple& o) const {
		return (three[0] == o.three[0]) && (three[1] == o.three[1]) && (three[2] == o.three[2]);
	};

	bool operator<(const triple& o) const {

		if (three[0] < o.three[0]) {
			return true;
		}
		else if (three[0] == o.three[0]) {
			if (three[1] < o.three[1]) {
				return true;
			}
			else if (three[1] == o.three[1]) {
				return (three[2] < o.three[2]);
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	};
};

struct tripleHashFunction {
	template<class tType>
	std::size_t operator() (const triple<tType>& trip) const {
		std::size_t h = std::hash<tType>()(trip.three[0]);// +0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= std::hash<tType>()(trip.three[1]) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= std::hash<tType>()(trip.three[2]) + 0x9e3779b9 + (h << 6) + (h >> 2);
		//std::cout << "K: " << trip.three[0] << ", " << trip.three[1] << ", " << trip.three[2] << "\n";
		//std::cout << "H: " << h << "\n";
		return h;
		//return 0;
	}
};

// Opengl
struct Vertex {
	glm::vec4 Position;
	glm::vec4 Normal;
	glm::vec4 Material;

	// Render Infrormation
	/*
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TextureCoords;
	glm::vec3 Normal;

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float Shininess;
	//*/

	bool operator==(const Vertex& o) const {
		//return ((o.Position == Position) && (o.Color == Color) && (o.TextureCoords == TextureCoords) && (o.Normal == Normal) &&
		//	(o.Ambient == Ambient) && (o.Diffuse == Diffuse) && (o.Specular == Specular) && (o.Shininess == Shininess));
		return (o.Position == Position) && (o.Normal == Normal) && (o.Material == Material);
	}
};


//struct Texture {
//	unsigned int id;
//	std::string type;
//};


struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};



/*/ Terrain
struct mVertex { // m is for meta
	// Meta Information
	float fill;
	// Render Infrormation
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TextureCoords;
	glm::vec3 Normal;

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float Shininess;
};
*/


//struct cube { // Holds vertex fills. Names are cube coordinates in binary (see vertices[8][3])
//	mVertex v000;
//	mVertex v100;
//	mVertex v110;
//	mVertex v010;
//	mVertex v001;
//	mVertex v101;
//	mVertex v111;
//	mVertex v011;
//};


//struct triangle {
//	Vertex v0;
//	Vertex v1;
//	Vertex v2;
//};
