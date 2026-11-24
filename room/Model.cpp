#define _CRT_SECURE_NO_WARNINGS
#include "Model.h"

Model::Model(const char* file) {
	Model::file = file;
	std::string content = get_file_contents(file);

	std::string ext = Model::file.substr(Model::file.find_last_of('.') + 1);
	for (auto& c : ext) c = (char)tolower(c);

	if (ext == "glb") {
		loadGLB(content);
	}
	else {
		JSON = json::parse(content);
		data = getData();   // el de siempre, para .gltf + .bin
	}

	traverseNode(0);
	std::cout << "Meshes cargados: " << meshes.size() << std::endl;
}

void Model::loadGLB(const std::string& content) {
	const unsigned char* p = (const unsigned char*)content.data();
	size_t size = content.size();

	if (size < 20) throw std::runtime_error("GLB demasiado pequeno");

	uint32_t magic, version, length;
	std::memcpy(&magic, p, 4);
	std::memcpy(&version, p + 4, 4);
	std::memcpy(&length, p + 8, 4);
	if (magic != 0x46546C67) throw std::runtime_error("No es un GLB valido");

	size_t offset = 12;
	while (offset + 8 <= size) {
		uint32_t chunkLen, chunkType;
		std::memcpy(&chunkLen, p + offset, 4);
		std::memcpy(&chunkType, p + offset + 4, 4);
		offset += 8;
		if (offset + chunkLen > size) throw std::runtime_error("Chunk GLB truncado");

		if (chunkType == 0x4E4F534A) {          // JSON
			JSON = json::parse(p + offset, p + offset + chunkLen);
		}
		else if (chunkType == 0x004E4942) {     // BIN
			data.assign(p + offset, p + offset + chunkLen);
		}
		offset += chunkLen;
	}
}
void Model::Draw(Shader& shader, Camera& camera, glm::mat4 matrix)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader, camera, matrix * matricesMeshes[i]);
}
std::vector<unsigned char> Model::getData() {

	std::string bytesText;

	std::cout << "Checking buffers..." << std::endl;
	std::cout << JSON["buffers"][0].dump(2) << std::endl;

	if (!JSON["buffers"][0].contains("uri")) {
		throw std::runtime_error(
			"El buffer no contiene URI"
		);
	}

	if (!JSON["buffers"][0]["uri"].is_string()) {
		throw std::runtime_error(
			"La URI del buffer no es un string"
		);
	}

	std::string uri = JSON["buffers"][0]["uri"].get<std::string>();

	std::string fileStr = std::string(file);
	std::string fileDir =
		fileStr.substr(0, fileStr.find_last_of("/\\") + 1);

	bytesText = get_file_contents((fileDir + uri).c_str());

	return std::vector<unsigned char>(
		bytesText.begin(), bytesText.end()
	);
}
std::vector<float>Model::getFloats(json accesor) {
	std::vector<float> floatVec;
	unsigned int buffViewInd = accesor.value("bufferView", 0);
	//the square brackets on access to the key value if it exists 
	unsigned int count = accesor["count"];
	unsigned int accByteOffset = accesor.value("byteOffset", 0);
	std::string type = accesor["type"];

	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView.value("byteOffset", 0);

	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("type is invalid (not SCALAR, VEC2, VEC3, VEC4)");

	unsigned int beginningOfData = byteOffset + accByteOffset;
	unsigned int lenghtOfData = count * 4 * numPerVert;
	//we will store the bytes in a temporary matrix 
	for (unsigned int i = beginningOfData; i < beginningOfData + lenghtOfData; i += 4) {
		float value;
		std::memcpy(&value, &data[i], sizeof(float));
		floatVec.push_back(value);
	}
	return floatVec;
}
std::vector<GLuint> Model::getIndices(json accesor) {
	std::vector<GLuint> indices;
	unsigned int buffViewInd = accesor.value("bufferView", 0);
	unsigned int count = accesor["count"];
	unsigned int accByteOffset = accesor.value("byteOffset", 0);
	unsigned int componentType = accesor["componentType"];
	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView.value("byteOffset", 0);
	unsigned int beginnningOfData = byteOffset + accByteOffset;

	if (componentType == 5125) {            // unsigned int (4 bytes)
		for (unsigned int i = beginnningOfData; i < beginnningOfData + count * 4; i += 4) {
			unsigned int value;
			std::memcpy(&value, &data[i], sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123) {       // unsigned short (2 bytes)
		for (unsigned int i = beginnningOfData; i < beginnningOfData + count * 2; i += 2) {
			unsigned short value;
			std::memcpy(&value, &data[i], sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5121) {       // unsigned byte (1 byte)
		for (unsigned int i = beginnningOfData; i < beginnningOfData + count; i++) {
			indices.push_back((GLuint)data[i]);
		}
	}
	return indices;}

std::vector<glm::vec2> Model::groupFloatsVec2(
	std::vector<float> floatVec) {

	std::vector<glm::vec2> vectors;

	for (size_t i = 0; i + 1 < floatVec.size(); i += 2) {
		vectors.push_back(
			glm::vec2(floatVec[i], floatVec[i + 1])
		);
	}

	return vectors;
}

	std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec) {
		std::vector<glm::vec3> vectors;
		for (size_t i = 0; i + 2 < floatVec.size(); i += 3) {
			vectors.push_back(glm::vec3(floatVec[i], floatVec[i + 1], floatVec[i + 2]));
		}
		return vectors;
	}

	std::vector<Vertex> Model::assembleVertices(
		std::vector<glm::vec3> positions,
		std::vector<glm::vec3> normals,
		std::vector<glm::vec2> texUVs) {

		std::vector<Vertex> vertices;
		vertices.reserve(positions.size());
		for (size_t i = 0; i < positions.size(); i++) {
			vertices.push_back(Vertex{
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				texUVs[i]
				});
		}
		return vertices;
	}
	std::vector<Texture> Model::getTextures(unsigned int indMesh) {
		std::vector<Texture> textures;

		const json& prim = JSON["meshes"][indMesh]["primitives"][0];
		if (!prim.contains("material")) return textures;

		const json& mat = JSON["materials"][prim["material"].get<unsigned int>()];
		if (!mat.contains("pbrMetallicRoughness")) return textures;
		const json& pbr = mat["pbrMetallicRoughness"];

		std::string fileStr = std::string(file);
		std::string fileDir = fileStr.substr(0, fileStr.find_last_of("/\\") + 1);

		auto loadSlot = [&](const char* key, const char* type, unsigned int unit) {
			if (!pbr.contains(key)) return;

			unsigned int texIdx = pbr[key]["index"];
			unsigned int imgIdx = JSON["textures"][texIdx]["source"];
			const json& img = JSON["images"][imgIdx];

			// clave de cache: uri, o "img<N>" si va embebida
			std::string cacheKey = img.contains("uri")
				? img["uri"].get<std::string>()
				: "img" + std::to_string(imgIdx);

			for (size_t k = 0; k < loadedTexName.size(); k++) {
				if (loadedTexName[k] == cacheKey) {
					textures.push_back(loadedTex[k]);
					return;
				}
			}

			if (img.contains("uri")) {
				Texture tex(fileDir + cacheKey, type, unit, false);
				textures.push_back(tex);
				loadedTex.push_back(tex);
			}
			else if (img.contains("bufferView")) {
				const json& bv = JSON["bufferViews"][img["bufferView"].get<unsigned int>()];
				size_t off = bv.value("byteOffset", 0);
				size_t len = bv["byteLength"].get<size_t>();
				Texture tex(&data[off], len, type, unit, false);
				textures.push_back(tex);
				loadedTex.push_back(tex);
			}
			else return;

			loadedTexName.push_back(cacheKey);
			};

		loadSlot("baseColorTexture", "diffuse", 0);
		loadSlot("metallicRoughnessTexture", "specular", 1);

		return textures;
	}
	void Model::loadMesh(unsigned int indMesh) {
		unsigned int posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
		unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
		unsigned int textAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
		unsigned int indAccInd = JSON["meshes"][indMesh]["primitives"][0]["indices"];

		std::vector<float>posVec = getFloats(JSON["accessors"][posAccInd]);
		std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
		std::vector<float>normalVec = getFloats(JSON["accessors"][normalAccInd]);
		std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
		std::vector<float> textVec = getFloats(JSON["accessors"][textAccInd]);
		std::vector<glm::vec2> textUVs = groupFloatsVec2(textVec);

		std::vector<Vertex> vertices = assembleVertices(positions, normals, textUVs);
		std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);
		std::vector<Texture> textures = getTextures(indMesh);
		std::cout << "mesh " << indMesh << " texturas: " << textures.size() << std::endl;

		meshes.push_back(Mesh(vertices, indices, textures));

		const json& prim = JSON["meshes"][indMesh]["primitives"][0];
		if (prim.contains("material")) {
			const json& mat = JSON["materials"][prim["material"].get<unsigned int>()];
			if (mat.contains("pbrMetallicRoughness") &&
				mat["pbrMetallicRoughness"].contains("baseColorFactor")) {
				const json& f = mat["pbrMetallicRoughness"]["baseColorFactor"];
				meshes.back().baseColor = glm::vec4(f[0].get<float>(), f[1].get<float>(),
					f[2].get<float>(), f[3].get<float>());
			}
		}



	}
	void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix) {
		json node = JSON["nodes"][nextNode];

		glm::vec3 translation(0.0f);
		if (node.find("translation") != node.end()) {
			float transValues[3];
			for (unsigned int i = 0; i < 3; i++)
				transValues[i] = node["translation"][i];
			translation = glm::make_vec3(transValues);
		}

		glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
		if (node.find("rotation") != node.end()) {
			rotation = glm::quat(
				node["rotation"][3].get<float>(), node["rotation"][0].get<float>(),
				node["rotation"][1].get<float>(), node["rotation"][2].get<float>());
		}

		glm::vec3 scale(1.0f);
		if (node.find("scale") != node.end()) {
			float scaleValues[3];
			for (unsigned int i = 0; i < 3; i++)
				scaleValues[i] = node["scale"][i];
			scale = glm::make_vec3(scaleValues);
		}

		glm::mat4 matNode(1.0f);
		if (node.find("matrix") != node.end()) {
			float matValues[16];
			for (unsigned int i = 0; i < 16; i++)
				matValues[i] = node["matrix"][i];
			matNode = glm::make_mat4(matValues);
		}

		glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
		glm::mat4 rot = glm::mat4_cast(rotation);
		glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

		glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

		if (node.find("mesh") != node.end()) {
			translationsMeshes.push_back(translation);
			rotationsMeshes.push_back(rotation);
			scalesMeshes.push_back(scale);
			matricesMeshes.push_back(matNextNode);

			loadMesh(node["mesh"]);
		}

		if (node.find("children") != node.end()) {
			for (unsigned int i = 0; i < node["children"].size(); i++)
				traverseNode(node["children"][i], matNextNode);
		}
	}








