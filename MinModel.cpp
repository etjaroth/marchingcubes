#include "MinModel.h"
void MinModel::draw(Shader& shader, Material material)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader, material);
	//std::cout << "Size: " << meshes.size() << std::endl;
}

void MinModel::loadModel(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);// | aiProcess_GenNormals | aiProcess_OptimizeMeshes);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

	bool badModel = false;
	// Check if there is stuff in the nodes
	glm::vec3 bob;
	//glm::vec2 minibob;
	//std::cout << "Testing..." << std::endl;
	for (Mesh i : meshes) {
		for (Vertex j : i.vertices) {
			//if (j.Position == bob) {
			//	badModel = true;
			//}
			//if (j.Color == bob) {
			//	badModel = true;
			//}
			//if (j.Normal == bob) {
			//	badModel = true;
			//}
			//if (j.TextureCoords == minibob) {
			//	badModel = true;
			//}
		}
	}
	//std::cout << "Is bad model? " << badModel << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
}

void MinModel::processNode(aiNode* node, const aiScene* scene)
{
	//std::cout << "Processing Node..." << std::endl;
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh MinModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	//std::cout << "Processing Mesh..." << std::endl;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		// process vertex positions, colors, normals and texture coordinates

		glm::vec3 posvec;
		posvec.x = mesh->mVertices[i].x;
		posvec.y = mesh->mVertices[i].y;
		posvec.z = mesh->mVertices[i].z;
		//vertex.Position = posvec;

		if (mesh->HasVertexColors(0)) { // does the mesh contain a color?
			glm::vec4 colorvec;
			//std::cout << "hi" << std::endl;
			colorvec.x = (float)((mesh->mColors[0][i].r) / 255.0); // not sure if that should be a [1] or a [0]
			colorvec.y = (float)((mesh->mColors[0][i].g) / 255.0);
			colorvec.z = (float)((mesh->mColors[0][i].b) / 255.0);
			colorvec.w = (float)(mesh->mColors[0][i].a);
			//std::cout << colorvec.x << ", " << colorvec.y << ", " << colorvec.z << ", " << colorvec.w << std::endl;
			//vertex.Color = colorvec;
		}
		else {
			//vertex.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		//vertex.Color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
		if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
			glm::vec2 texvec;
			texvec.x = mesh->mTextureCoords[0][i].x;
			texvec.y = mesh->mTextureCoords[0][i].y;
			//vertex.TextureCoords = texvec;
		}
		else
			//vertex.TextureCoords = glm::vec2(0.0f, 0.0f);

		glm::vec3 normvec;
		//normvec.x = mesh->mNormals[i].x;
		//normvec.y = mesh->mNormals[i].y;
		//normvec.z = mesh->mNormals[i].z;
		//vertex.Normal = normvec;

		vertices.push_back(vertex);
		//glm::vec3 printvec = glm::vec3(vertex.TextureCoords, 0.0f);
	}
	// process indices
	
	//std::cout << "Test: " << vertices.size() << std::endl;
	
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh(vertices, indices);
	/*/ process material (you didn't finish the model page because this is a minmesh not a mesh)
	if (mesh->mMaterialIndex >= 0)
	{
		[...]
	}

	return Mesh(vertices, indices, textures);
	//*/
}